#include "main.h"
#include "rgbwac.h"
#include "user.h"
#include "drv_mcp4728.h"
#include <string.h>

/* 单套序列使用 64 KiB 数据块和独立元数据扇区。
 * 先使旧标记失效，再擦数据块，最后擦元数据扇区，防止擦除中断后误播旧序列。 */
#define DATA_ADDRESS       0u
#define META_ADDRESS       0x10000u
#define META_SIZE          18u
#define VALID_ADDRESS      (META_ADDRESS + META_SIZE)
#define VALID_BYTE         0xA5u
#define FORMAT_VERSION     1u
#define FRAME_LIMIT        3000u
#define LIGHT_LIMIT        776u
#define LIGHTS_PER_PACKET  30u
#define RECORD_SIZE        16u
#define PACKET_SIZE        (3u + LIGHTS_PER_PACKET * RECORD_SIZE) /* 两字节帧号、一字节分包号及固定记录区。 */
#define PAGE_SIZE          256u
#define MIN_DURATION       1000u
#define MAX_DURATION       3600000000u
#define SPI_TIMEOUT_MS     5u
#define PROGRAM_TIMEOUT_MS 15u
#define BLOCK_TIMEOUT_MS   2300u
#define SECTOR_TIMEOUT_MS  500u
#define CLOCK_WRAP         65536u

/* 整帧还包含帧头、长度、命令和校验和，共 7 字节。 */
#if (PACKET_SIZE + 7u) > 512u
#error RGBWAC packet exceeds the 512-byte wire limit
#endif

#define DAC_ADDRESS        0xC0u
#define LDAC_PINS          (IIC1_LDAC_Pin | IIC2_LDAC_Pin)
#define OUTPUT_PINS        (CR_PWM_Pin | CG_PWM_Pin | CB1_PWM_Pin | CB2_PWM_Pin | \
                            CW1_PWM_Pin | CW2_PWM_Pin | CA_PWM_Pin | CC_PWM_Pin)

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

typedef enum
{
    UploadNone, UploadWaitIdle, UploadInvalidate, UploadEraseData, UploadEraseMeta,
    UploadReceive, UploadWritePage, UploadWriteMeta, UploadSeal, UploadReady, UploadFailed
} UploadState;

typedef enum { PlaybackStopped, PlaybackArmed, PlaybackTriggered, PlaybackRunning } PlaybackState;

typedef struct
{
    uint16_t levels[6];
    uint32_t duration;
} RgbwacFrame;

/* 触发和定时中断与主循环共享的状态。 */
static volatile bool selected;
static volatile PlaybackState playback;
static volatile bool frame_elapsed;
static volatile uint32_t timer_events;
static UploadState upload;
static uint16_t light_id, frame_count, received, requested_loop, playback_loop, frame_index;
static bool start_pending;
static uint16_t page_used;
static uint32_t flash_started, flash_timeout, running_crc;
static uint8_t page[PAGE_SIZE];
static uint8_t metadata[META_SIZE];
static RgbwacFrame staged_frame;
static uint16_t staged_index, current_outputs;
static bool frame_staged;

/* 从字节流读取一个小端 16 位字段。 */
static uint16_t read16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* 从字节流读取一个小端 32 位字段，用于微秒时长及 CRC。 */
static uint32_t read32(const uint8_t *p)
{
    return (uint32_t)read16(p) | ((uint32_t)read16(p + 2) << 16);
}

/* 将 16 位字段按小端字节序写入存储缓冲区。 */
static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

/* 将 32 位字段按小端字节序写入存储缓冲区。 */
static void put32(uint8_t *p, uint32_t value)
{
    put16(p, (uint16_t)value);
    put16(p + 2, (uint16_t)(value >> 16));
}

/* 用当前数据块更新 CRC32，使多页数据可以累计校验。 */
static uint32_t crc32(uint32_t crc, const uint8_t *data, uint16_t size)
{
    uint16_t i;
    uint8_t bit;
    for(i = 0; i < size; ++i)
    {
        crc ^= data[i];
        for(bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1u) ? 0xEDB88320u : 0u);
    }
    return crc;
}

/* 在一次片选有效期间发送 SPI 命令并按需读取响应。 */
static bool flash_transfer(const uint8_t *command, uint16_t command_size,
                           uint8_t *response, uint16_t response_size)
{
    HAL_StatusTypeDef status;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    status = HAL_SPI_Transmit(&hspi1, (uint8_t *)command, command_size, SPI_TIMEOUT_MS);
    if((status == HAL_OK) && (response_size != 0))
        status = HAL_SPI_Receive(&hspi1, response, response_size, SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    return status == HAL_OK;
}

/* 读取 W25Q128 状态寄存器，供上层检查忙状态和写使能。 */
static bool flash_status(uint8_t *status)
{
    const uint8_t command = 0x05;
    return flash_transfer(&command, 1, status, 1);
}

/* 从指定 24 位 Flash 地址连续读取数据。 */
static bool flash_read(uint32_t address, uint8_t *data, uint16_t size)
{
    const uint8_t command[] = {0x03, (uint8_t)(address >> 16),
                              (uint8_t)(address >> 8), (uint8_t)address};
    return flash_transfer(command, sizeof(command), data, size);
}

/* 分段回读并逐字节核对写入内容，限制栈缓冲占用。 */
static bool flash_verify(uint32_t address, const uint8_t *expected, uint16_t size)
{
    uint8_t actual[32];
    uint16_t count;
    while(size != 0)
    {
        count = (size > sizeof(actual)) ? sizeof(actual) : size;
        if(!flash_read(address, actual, count) || memcmp(actual, expected, count) != 0)
            return false;
        size -= count;
        address += count;
        expected += count;
    }
    return true;
}

/* 检查写使能和页边界，发起页编程或擦除并记录超时起点。 */
static bool flash_begin(uint8_t opcode, uint32_t address, const uint8_t *data,
                        uint16_t size, uint32_t timeout)
{
    const uint8_t enable = 0x06;
    uint8_t status;
    uint8_t command[] = {opcode, (uint8_t)(address >> 16),
                        (uint8_t)(address >> 8), (uint8_t)address};
    HAL_StatusTypeDef result;

    /* NOR 页编程不能跨页，跨页时芯片会从当前页开头继续写。 */
    if(size > PAGE_SIZE - (address % PAGE_SIZE)) return false;
    if(!flash_transfer(&enable, 1, NULL, 0) || !flash_status(&status) ||
       ((status & 0x03u) != 0x02u)) return false;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    result = HAL_SPI_Transmit(&hspi1, command, sizeof(command), SPI_TIMEOUT_MS);
    if((result == HAL_OK) && (size != 0))
        result = HAL_SPI_Transmit(&hspi1, (uint8_t *)data, size, SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    flash_started = HAL_GetTick();
    flash_timeout = timeout;
    return result == HAL_OK;
}

/* 停止六通道定时并清除上一次播放留下的事件。 */
static void clear_timer(void)
{
    /* 只清理 TIM1 本身，保留 TIM9、TIM10 共享中断的使能状态。 */
    TIM1->DIER = 0;
    TIM1->CR1 = 0;
    TIM1->SR = 0;
    timer_events = 0;
    frame_elapsed = false;
}

/* 将 TIM1 配置为 1 MHz 计数并启用比较中断。 */
static void configure_timer(void)
{
    uint32_t clock = HAL_RCC_GetPCLK2Freq();
    if((RCC->CFGR & RCC_CFGR_PPRE2) != 0) clock *= 2u;
    clear_timer();
    TIM1->PSC = clock / 1000000u - 1u;
    TIM1->ARR = CLOCK_WRAP - 1u;
    TIM1->RCR = 0;
    TIM1->CCMR1 = 0; /* 比较匹配只产生事件，不驱动复用引脚。 */
    TIM1->EGR = TIM_EGR_UG;
    TIM1->SR = 0;
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
}

/* 按当前帧时长准备 TIM1，等待实际输出切换后启动。 */
static void prepare_timer(uint32_t duration)
{
    const uint32_t remainder = duration % CLOCK_WRAP;
    clear_timer();
    TIM1->CR1 = TIM_CR1_URS; /* 仅硬件溢出可请求更新中断。 */
    TIM1->EGR = TIM_EGR_UG; /* 启动前复位计数器和预分频器相位。 */
    TIM1->SR = 0;
    /* 整周期按溢出次数计数；有余数时计同一偏移处的比较事件，首个事件提前至余数处。 */
    if(remainder == 0)
    {
        timer_events = duration / CLOCK_WRAP;
        TIM1->DIER = TIM_DIER_UIE;
    }
    else
    {
        timer_events = duration / CLOCK_WRAP + 1u;
        TIM1->CCR1 = remainder;
        TIM1->DIER = TIM_DIER_CC1IE;
    }
}

/* 处理 TIM1 溢出或比较事件，到期时通知主循环换帧。 */
void Rgbwac_TimerIRQ(uint32_t flag)
{
    uint32_t enabled = (flag == TIM_SR_UIF) ? TIM_DIER_UIE : TIM_DIER_CC1IE;
    if(((TIM1->SR & flag) == 0) || ((TIM1->DIER & enabled) == 0)) return;
    TIM1->SR = ~flag;
    if(--timer_events == 0)
    {
        TIM1->DIER = 0;
        TIM1->CR1 = 0;
        frame_elapsed = true;
    }
}

/* 在主循环一次更新六通道使能位，保留同一 GPIO 端口的其他引脚。 */
static void set_outputs(uint16_t mask)
{
    GPIOD->BSRR = (uint32_t)mask | ((uint32_t)(OUTPUT_PINS & ~mask) << 16);
    current_outputs = mask;
}

/* 把帧内非零亮度转换为灯板的输出引脚掩码。 */
static uint16_t output_mask(const RgbwacFrame *frame)
{
    const uint16_t pins[] = {CR_PWM_Pin, CG_PWM_Pin, CB1_PWM_Pin | CB2_PWM_Pin,
                            CW1_PWM_Pin | CW2_PWM_Pin, CA_PWM_Pin, CC_PWM_Pin};
    uint16_t mask = 0;
    uint8_t i;
    for(i = 0; i < 6; ++i)
        if(frame->levels[i] != 0) mask |= pins[i];
    return mask;
}

/* 通过 I²C 将一颗 DAC 的亮度写入易失输入寄存器。 */
static bool stage_dac(I2C_HandleTypeDef *i2c, const uint16_t *levels, uint8_t count)
{
    uint8_t buffer[12];
    uint8_t channel;
    for(channel = 0; channel < count; ++channel)
    {
        uint16_t value = levels[channel];
        /* 多路写入命令只写易失输入寄存器，UDAC 置 1，等待 LDAC 再更新输出。 */
        buffer[channel * 3] = (uint8_t)(0x41u | (channel << 1));
        buffer[channel * 3 + 1] = (uint8_t)(normal_model | (value >> 8));
        buffer[channel * 3 + 2] = (uint8_t)value;
    }
    return HAL_I2C_Master_Transmit(i2c, DAC_ADDRESS, buffer, count * 3u, 2) == HAL_OK;
}

/* 从外部 Flash 读取指定帧，检查时长、限幅亮度并预装到两颗 DAC。 */
static bool stage_frame(uint16_t index)
{
    uint8_t record[RECORD_SIZE];
    uint8_t channel;
    if((index >= frame_count) || !flash_read(DATA_ADDRESS + (uint32_t)index * RECORD_SIZE,
                                           record, sizeof(record))) return false;
    staged_frame.duration = read32(record + 12);
    if((staged_frame.duration < MIN_DURATION) || (staged_frame.duration > MAX_DURATION))
        return false;
    for(channel = 0; channel < 6; ++channel)
    {
        uint16_t value = read16(record + channel * 2u);
        staged_frame.levels[channel] = (value > CMD_Power_Max) ? CMD_Power_Max : value;
    }
    GPIOB->BSRR = LDAC_PINS;
    if(!stage_dac(&hi2c1, staged_frame.levels, 4) ||
       !stage_dac(&hi2c2, staged_frame.levels + 4, 2)) return false;
    staged_index = index;
    frame_staged = true;
    return true;
}

/* 锁存预装的亮度，更新输出使能，并开始计算此帧的持续时间。 */
static void apply_staged_frame(void)
{
    uint16_t mask = output_mask(&staged_frame);
    prepare_timer(staged_frame.duration);
    /* 上一帧未点亮的通道 DAC 值为 0。先使能新通道，再锁存新亮度，最后关闭退出的通道。 */
    if(playback == PlaybackRunning) set_outputs(current_outputs | mask);
    GPIOB->BSRR = (uint32_t)LDAC_PINS << 16;
    set_outputs(mask);
    TIM1->CR1 |= TIM_CR1_CEN;
    frame_index = staged_index;
    frame_staged = false;
    playback = PlaybackRunning;
}

/* 判断命令和播放流程当前是否由六通道模块接管。 */
bool Rgbwac_IsSelected(void) { return selected; }
/* 判断软启动是否已准备完毕并正在等待外部触发。 */
bool Rgbwac_IsArmed(void) { return playback == PlaybackArmed; }

/* 由 TIM6 确认首个有效触发后记录启动事件。 */
void Rgbwac_Trigger(void)
{
    if(playback == PlaybackArmed) playback = PlaybackTriggered;
}

/* 解除触发和计时并熄灯，保留六通道模式和外部 Flash 数据。 */
void Rgbwac_Stop(void)
{
    if(!selected) return;
    playback = PlaybackStopped;
    start_pending = false;
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
    HAL_TIM_Base_Stop_IT(&htim6);
    __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
    __HAL_GPIO_EXTI_CLEAR_IT(TRIG_MCU_Pin);
    clear_timer();
    frame_staged = false;
    set_outputs(0);
}

/* 停止六通道并退出当前上传状态，交还输出和命令处理。 */
void Rgbwac_Leave(void)
{
    if(!selected) return;
    Rgbwac_Stop();
    GPIOB->BSRR = (uint32_t)LDAC_PINS << 16;
    requested_loop = 0;
    upload = UploadNone;
    selected = false;
}

/* 记录上传失败并取消待启动请求。 */
static void upload_failed(void)
{
    upload = UploadFailed;
    start_pending = false;
}

/* 准备接收指定数量的帧；帧数为 0 时改为校验并选择已保存的序列。 */
void Rgbwac_Prepare(uint16_t count, uint16_t device_id)
{
    selected = true;
    Rgbwac_Stop();
    configure_timer();
    requested_loop = 0;
    frame_count = count;
    light_id = device_id;
    received = 0;
    page_used = 0;
    running_crc = 0xFFFFFFFFu;
    if((count > FRAME_LIMIT) || (device_id < 1) || (device_id > LIGHT_LIMIT))
    {
        upload_failed();
        return;
    }
    upload = UploadWaitIdle;
    flash_started = HAL_GetTick();
    flash_timeout = BLOCK_TIMEOUT_MS + SECTOR_TIMEOUT_MS;
}

/* 校验分包并提取本机记录，按帧号顺序累积到 Flash 页缓冲。 */
void Rgbwac_Receive(const uint8_t *data, uint16_t size)
{
    uint16_t number, index;
    const uint8_t *record;
    uint32_t duration;
    if(!selected || (light_id < 1) || (light_id > LIGHT_LIMIT) || (frame_count == 0))
        return;
    /* 同一个 CSV 的各分包使用同一帧号；仅本机所在分包包含本机记录。 */
    if((size != PACKET_SIZE) || (data[2] != (light_id - 1u) / LIGHTS_PER_PACKET + 1u))
        return;
    number = read16(data);
    /* 丢弃重复帧；出现帧号断档时由下方分支终止本次上传。 */
    if((number == 0) || (number > frame_count) || (number <= received)) return;
    if((upload != UploadReceive) || (number != received + 1u))
    {
        if(upload != UploadNone) upload_failed();
        return;
    }
    index = (light_id - 1u) % LIGHTS_PER_PACKET;
    record = data + 3u + index * RECORD_SIZE;
    duration = read32(record + 12);
    if((duration < MIN_DURATION) || (duration > MAX_DURATION))
    {
        upload_failed();
        return;
    }

    memcpy(page + page_used, record, RECORD_SIZE);
    running_crc = crc32(running_crc, record, RECORD_SIZE);
    page_used += RECORD_SIZE;
    ++received;
    /* 满一页时写入，接收完最后一帧时也要写入不足一页的尾部。 */
    if((page_used == PAGE_SIZE) || (received == frame_count))
    {
        uint32_t address = DATA_ADDRESS + (uint32_t)received * RECORD_SIZE - page_used;
        if(flash_begin(0x02, address, page, page_used, PROGRAM_TIMEOUT_MS))
            upload = UploadWritePage;
        else
            upload_failed();
    }
}

/* 读取 JEDEC ID，确认连接的器件为受支持的 W25Q128。 */
static bool flash_identify(void)
{
    const uint8_t command = 0x9F;
    uint8_t id[3];
    return flash_transfer(&command, 1, id, sizeof(id)) &&
           (id[0] == 0xEF) && (id[1] == 0x40) && (id[2] == 0x18);
}

/* 核对有效标记、格式、本机 ID 和 CRC，确认 Flash 序列可以播放。 */
static bool validate_stored(void)
{
    uint8_t seal;
    uint16_t count, records;
    uint32_t address, crc = 0xFFFFFFFFu;
    if(!flash_read(VALID_ADDRESS, &seal, 1) || (seal != VALID_BYTE) ||
       !flash_read(META_ADDRESS, metadata, sizeof(metadata)) ||
       (memcmp(metadata, "RGB6", 4) != 0) || (read16(metadata + 4) != FORMAT_VERSION) ||
       (read16(metadata + 8) != light_id) ||
       ((crc32(0xFFFFFFFFu, metadata, 14) ^ 0xFFFFFFFFu) != read32(metadata + 14)))
        return false;
    count = read16(metadata + 6);
    if((count == 0) || (count > FRAME_LIMIT)) return false;
    address = DATA_ADDRESS;
    records = count;
    while(records != 0)
    {
        uint16_t chunk = (records > PAGE_SIZE / RECORD_SIZE) ? PAGE_SIZE / RECORD_SIZE : records;
        uint16_t size = chunk * RECORD_SIZE;
        if(!flash_read(address, page, size)) return false;
        crc = crc32(crc, page, size);
        address += size;
        records -= chunk;
    }
    if((crc ^ 0xFFFFFFFFu) != read32(metadata + 10)) return false;
    frame_count = count;
    received = count;
    return true;
}

/* 发起序列数据块擦除，将后续处理交给上传状态机。 */
static bool erase_data(void)
{
    if(!flash_begin(0xD8, DATA_ADDRESS, NULL, 0, BLOCK_TIMEOUT_MS)) return false;
    upload = UploadEraseData;
    return true;
}

/* 轮询 Flash 并推进准备、擦除、回读核对和提交流程。 */
static bool upload_step(void)
{
    uint8_t status;
    const uint8_t invalid = 0;
    const uint8_t valid = VALID_BYTE;
    uint8_t seal;
    if(!flash_status(&status)) return false;
    /* 忙时让主循环继续处理其他任务，累计等待超时则退出上传。 */
    if((status & 1u) != 0)
        return (uint32_t)(HAL_GetTick() - flash_started) < flash_timeout;
    if((upload != UploadWaitIdle) && ((status & 2u) != 0)) return false;

    switch(upload)
    {
        case UploadWaitIdle:
            if(!flash_identify()) return false;
            if(frame_count == 0)
            {
                if(!validate_stored()) return false;
                upload = UploadReady;
                return true;
            }
            if(!flash_read(VALID_ADDRESS, &seal, 1)) return false;
            if(seal != VALID_BYTE) return erase_data();
            if(!flash_begin(0x02, VALID_ADDRESS, &invalid, 1, PROGRAM_TIMEOUT_MS)) return false;
            upload = UploadInvalidate;
            return true;
        case UploadInvalidate:
            return flash_verify(VALID_ADDRESS, &invalid, 1) && erase_data();
        case UploadEraseData:
            if(!flash_begin(0x20, META_ADDRESS, NULL, 0, SECTOR_TIMEOUT_MS)) return false;
            upload = UploadEraseMeta;
            return true;
        case UploadEraseMeta:
            if(!flash_read(VALID_ADDRESS, &seal, 1) || (seal != 0xFFu)) return false;
            upload = UploadReceive;
            return true;
        case UploadWritePage:
            if(!flash_verify(DATA_ADDRESS + (uint32_t)received * RECORD_SIZE - page_used,
                             page, page_used)) return false;
            page_used = 0;
            if(received != frame_count)
            {
                upload = UploadReceive;
                return true;
            }
            memcpy(metadata, "RGB6", 4);
            put16(metadata + 4, FORMAT_VERSION);
            put16(metadata + 6, frame_count);
            put16(metadata + 8, light_id);
            put32(metadata + 10, running_crc ^ 0xFFFFFFFFu);
            put32(metadata + 14, crc32(0xFFFFFFFFu, metadata, 14) ^ 0xFFFFFFFFu);
            if(!flash_begin(0x02, META_ADDRESS, metadata, sizeof(metadata), PROGRAM_TIMEOUT_MS))
                return false;
            upload = UploadWriteMeta;
            return true;
        case UploadWriteMeta:
            /* 数据页和元数据均校验后才写有效标记，掉电未完成的序列不能启动。 */
            if(!flash_verify(META_ADDRESS, metadata, sizeof(metadata)) ||
               !flash_begin(0x02, VALID_ADDRESS, &valid, 1, PROGRAM_TIMEOUT_MS)) return false;
            upload = UploadSeal;
            return true;
        case UploadSeal:
            if(!flash_verify(VALID_ADDRESS, &valid, 1)) return false;
            upload = UploadReady;
            return true;
        default:
            return false;
    }
}

/* 记录下一次软启动使用的循环帧数，0 表示完整播放一次。 */
void Rgbwac_SetLoopLength(uint16_t length) { requested_loop = length; }

/* 处理软启动，固定本轮循环长度，等待序列准备完成后再开放触发。 */
void Rgbwac_Start(void)
{
    Rgbwac_Stop();
    /* 循环长度仅用于本次启动，下次未重新下发时恢复为单次播放。 */
    playback_loop = requested_loop;
    requested_loop = 0;
    if((upload == UploadWaitIdle) && (frame_count == 0))
    {
        start_pending = true;
        return;
    }
    if((frame_count != 0) && (received == frame_count) && (playback_loop <= frame_count) &&
       ((upload == UploadReady) || (upload == UploadWritePage) ||
        (upload == UploadWriteMeta) || (upload == UploadSeal)))
        start_pending = true;
}

/* 计算下一帧索引，处理循环回绕和单次播放结束。 */
static bool next_index(uint16_t *next)
{
    *next = frame_index + 1u;
    if(playback_loop == 0) return *next < frame_count;
    if(*next == playback_loop) *next = 0;
    return true;
}

/* 在主循环推进 Flash 操作，并处理触发、到期换帧和预装。 */
void Rgbwac_Service(void)
{
    uint16_t next;
    if(!selected) return;
    if((upload != UploadReceive) && (upload != UploadReady) &&
       (upload != UploadFailed) && (upload != UploadNone))
    {
        if(!upload_step()) upload_failed();
    }
    /* 首帧成功预装后才开放触发，避免首个脉冲到达时数据尚未就绪。 */
    if(start_pending && (upload == UploadReady))
    {
        start_pending = false;
        if((playback_loop > frame_count) || !stage_frame(0)) return;
        __HAL_GPIO_EXTI_CLEAR_IT(TRIG_MCU_Pin);
        playback = PlaybackArmed;
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    }

    if(playback == PlaybackTriggered)
    {
        HAL_NVIC_DisableIRQ(EXTI0_IRQn);
        apply_staged_frame();
    }
    else if(playback == PlaybackRunning && frame_elapsed)
    {
        if(!next_index(&next) || !frame_staged || (staged_index != next))
        {
            Rgbwac_Stop();
            return;
        }
        apply_staged_frame();
    }

    /* 当前帧输出期间预装下一帧，换帧时无需等待 SPI 读取或 I²C 写入。 */
    if((playback == PlaybackRunning) && !frame_staged && next_index(&next) && !stage_frame(next))
        Rgbwac_Stop();
}
