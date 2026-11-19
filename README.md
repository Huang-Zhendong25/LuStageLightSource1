# LustageBody_Light_20241022pp1

光源（灯板）固件工程，基于 STM32F407，通过 RS485 与控制板通信。

本仓库在原始工程 `LustageBody_Light_20241022pp1` 的基础上，实现了命令字 `0x0355`（查询指定光源灯板温度）的应答功能。

## 1. 系统通信架构

```
上位机(PC)  --(UDP)-->  控制板  --(8路RS485)-->  光源(本工程)
          <--(UDP)---           <--(RS485)---
```

- 控制板把上位机下发的数据帧解析后，原样广播到 8 路 RS485。
- 光源挂在某一串口上，按帧内“光源ID”判断是否是自己。
- 本功能：上位机查询某一光源温度，控制板广播查询帧，被查询的光源通过 RS485 回传 10 字节温度应答，控制板再把应答原样 UDP 转回上位机。

## 2. 通信帧格式（LUSTER 协议）

```
4C 55 | Len(2, 小端) | Cmd(2, 大端) | Data... | CheckSum(1)
```

- `Len` = Cmd + Data 的总字节数（不含帧头与校验）。
- `CheckSum` = Cmd 起、到最后一个 Data 的字节累加，取低 8 位。

### 0x0355 温度查询帧（上位机 -> 控制板 -> 光源）

```
4C 55 04 00 03 55 ID_L ID_H CS        // 9 字节
```

- Cmd = 0x0355，内存顺序 `03 55`（大端）。
- 光源ID 小端。

### 0x0355 温度应答帧（光源 -> 控制板 -> 上位机）

```
4C 55 05 00 03 55 ID_L ID_H TEMP CS   // 10 字节
```

- Cmd = 0x0355（与查询一致）。
- TEMP = 当前温度，0x00~0x7D 对应 0~125℃。

## 3. 相对原始代码的改进及原因

### 3.1 新增 0x0355 温度查询应答

原始光源只接收控制板广播的命令并执行（调光、数码管、固件升级等），**没有任何主动回传能力**。

本实现新增 `case CMD_INFO_LightSource_Temperature`，当帧内“光源ID”与自身 `Device_ID` 相等时，回传 10 字节温度应答。

- 原因：协议要求被查询的光源返回 `4C 55 05 00 03 55 ID ID TEMP CS`。
- 温度值直接取 `LED_Para_Struct.Temp_Current`（`LED_OTP_Handler` 已周期维护，18B20 采集 + 5 取 1 中值滤波），不在查询时临时读 18B20，避免阻塞。

### 3.2 发送前中止常驻 RX（`HAL_UART_AbortReceive`）

`Comm_Send_Data` 发送前先中止常驻的 `HAL_UARTEx_ReceiveToIdle_DMA`。

- 原因：若硬件 485 存在自发自收 / 回环，光源发出的 10 字节应答会被自己的 RX 重新收进来，再被当成一条 `0x0355` 命令 → 再次匹配 ID → 再次应答，形成无限循环。中止 RX 可彻底避免。

### 3.3 发送完成后重新挂起 RX（`HAL_UART_TxCpltCallback`）

发送完成回调里除了 `RS485_Mode_Rx()`，还重新调用 `HAL_UARTEx_ReceiveToIdle_DMA`。

- 原因：发送前中止了 RX，所以发送完成后必须重新挂起，否则后续任何命令都收不到。

### 3.4 新增 `HAL_UART_ErrorCallback` 错误恢复

```c
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &Comm_Uart)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
        HAL_UART_AbortReceive(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, Comm_RxBuff, Comm_RxBuffSzie);
    }
}
```

- 原因：RS485 总线噪声 / 过载（ORE）等错误会把常驻接收挂死；在错误回调里清 ORE、中止并重新挂起，可保证长期运行不丢接收。

### 3.5 去掉中断里的等待循环

`HAL_UART_TxCpltCallback` 里**不再**写 `while(!TC)` 等待循环。

- 原因：本工程 HAL 的 TxCpltCallback 是在 UART **TC 中断**里被调用的，此时 TC 已置位、最后一字节已移出，无需再等；且中断里长时间等待是坏习惯。

## 4. 实现细节

### 4.1 命令字定义（user.h）

```c
#define CMD_INFO_LightSource_Temperature       0x0355
```

### 4.2 查询处理分支（user.c, Comm_Function_Handler）

```c
case CMD_INFO_LightSource_Temperature:
{
    uint16_t Query_ID = (Comm_Function_Struct.Format_Data.Data[1]<<8)
                      +  Comm_Function_Struct.Format_Data.Data[0];

    if(LED_Para_Struct.Device_ID == Query_ID)   // 只精确匹配自己的ID
    {
        Comm_Send_Temperature_Response(LED_Para_Struct.Device_ID,
                                       LED_Para_Struct.Temp_Current);
    }
}
break;
```

要点：只精确匹配 `Device_ID`，不响应 `0xFFFF` 广播 ID，保证“单灯查询、单灯应答”，避免 485 总线冲突。

### 4.3 应答发送函数（user.c）

```c
static uint8_t Comm_RespBuff[16];   // 全局缓冲，避免DMA期间局部变量失效

static void Comm_Send_Data(uint8_t *pData, uint16_t Len)
{
    HAL_UART_AbortReceive(&Comm_Uart);   // 先停掉常驻RX，避免自发自收/回环
    RS485_Mode_Tx();
    HAL_UART_Transmit_DMA(&Comm_Uart, pData, Len);
    // DE回接收 + 重挂RX 由 HAL_UART_TxCpltCallback 完成
}

static void Comm_Send_Temperature_Response(uint16_t Light_ID, int8_t Temp)
{
    uint8_t sum = 0;

    Comm_RespBuff[0] = 0x4C;   // 帧头 L
    Comm_RespBuff[1] = 0x55;   // 帧头 U
    Comm_RespBuff[2] = 0x05;   // Len 低字节
    Comm_RespBuff[3] = 0x00;   // Len 高字节
    Comm_RespBuff[4] = 0x03;   // Cmd 高字节 0x0355
    Comm_RespBuff[5] = 0x55;   // Cmd 低字节 0x0355
    Comm_RespBuff[6] = (uint8_t)(Light_ID & 0xFF);
    Comm_RespBuff[7] = (uint8_t)((Light_ID >> 8) & 0xFF);
    Comm_RespBuff[8] = (uint8_t)Temp;

    for(uint8_t i = 4 ; i < 9 ; i++)
    {
        sum += Comm_RespBuff[i];
    }
    Comm_RespBuff[9] = sum;     // 校验和

    Comm_Send_Data(Comm_RespBuff, 10);
}
```

### 4.4 485 方向与接收恢复

```c
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &Comm_Uart)
    {
        RS485_Mode_Rx();
        HAL_UARTEx_ReceiveToIdle_DMA(&Comm_Uart, Comm_RxBuff, Comm_RxBuffSzie);
    }
}
```

## 5. 修改过程注意事项

1. **应答缓冲区必须用全局 / 静态**：`HAL_UART_Transmit_DMA` 是异步的，函数返回后 DMA 仍在读缓冲区，局部变量会被销毁导致乱码。
2. **发送前要中止常驻 RX**：485 半双工 + 自发自收的硬件上，不中止 RX 会把自身应答当命令再次处理，造成死循环。
3. **发送完成要重挂 RX**：中止过 RX 就必须在发送完成回调里重新挂起，否则后续命令收不到。
4. **中断里不要 busy-wait**：`while(!TC)` 等待是多余的（TC 中断里 TC 已置位），也违反“中断要短”的原则。
5. **要加 `HAL_UART_ErrorCallback` 恢复**：485 总线抖动产生的 ORE/FE 错误会挂死接收，必须清 ORE 并重新挂起。
6. **命令字字节序**：光源侧按大端解析 `Cmd[0]<<8 + Cmd[1]`，应答帧里 Cmd 也写大端 `03 55`；与控制板内部值（0x5503）不是一回事。
7. **只精确匹配 ID 应答**：不要响应 `0xFFFF` 广播，否则多灯同时回包会冲突。
8. **温度查询不要临时读 18B20**：直接使用 `LED_OTP_Handler` 周期维护好的 `Temp_Current`，避免在命令处理里做长时间阻塞操作。

## 6. 编译与烧录

- 用 Keil MDK 打开 `LustageBody_Light_20241022/MDK-ARM/LustageBody_Light_20240218.uvprojx` 编译。
- 通过 SWD/ST-Link（或项目使用的烧录方式）烧录到光源板。

## 7. 联调（与控制板配合）

完整链路：PC 发 UDP 查询 -> 控制板广播到 8 路 RS485 -> 被查询光源应答 -> 控制板 UDP 转回 PC。详见控制板仓库 README。

## 8. 六通道亮度连播

### 8.1 功能与下发顺序

上位机每解析一个 CSV，就生成一个播放帧。每帧可为 776 个光源分别设置 `R、G、B、W、A、C` 六路亮度和单帧持续时间。光源根据自己的 ID，从收到的分包中提取本机记录。每个光源可保存最多 **3000 帧**，每帧支持 **1 ms～1 h**，设定步进为 **1 µs**。

| 步骤 | 命令 | 行为 |
| --- | --- | --- |
| 配置触发 | `0x0455` | 给控制器设置有效的触发频率和次数。如果还需用 `0x0255` 设置系统模式，请在准备帧之前发送。 |
| 准备上传 | `0x02AC` | 发送 CSV 总数（1～3000）。光源使旧序列失效并擦除外部 Flash。联调时建议等待 3 秒。 |
| 下发各帧 | `0x02AD` | 每个 CSV 按顺序发送 26 个分包。联调时建议相邻分包间隔 200 ms，最后一包后也留一个间隔。光源收齐并校验后自动提交。 |
| 设置循环（可选） | `0x0155` | 两字节小端循环长度。未发送或为 0 时播放全部帧一次。设为 `N` 时循环前 `N` 帧，`N` 大于已保存帧数时拒绝启动。 |
| 软启动 | `0x02FF` | 准备好首帧后等待第一个有效外部触发，之后按帧时长自动推进。循环长度只对本次软启动有效。 |
| 停止 | `0x03FF` | 结束本次播放并关灯，已提交的数据仍可用。 |

控制器使用配套的 `LustageController_20241010pp1` 固件，把两条新命令原样广播到八路 RS485。准备帧和数据帧均为单向发送，光源不会返回接收或保存状态。上传和播放期间，上位机不发起温度查询。

断电重启后，可发送 CSV 总数为 `0` 的 `0x02AC` 选择已经提交的序列。随后按需发送 `0x0155`，再发 `0x02FF`。软启动后，一次外部触发即可开始播放，后续触发不会换帧或重播。单次播放在末帧到期时关灯并结束。

### 8.2 实现方法

- [`rgbwac.c`](LustageBody_Light_20241022/Core/Src/rgbwac.c) 通过 SPI1 访问 W25Q128JVSIQ。每个光源只保存本机记录，3000 帧共 48,000 字节。Flash 使用一个 64 KiB 数据块和一个 4 KiB 元数据扇区，RAM 只保留页缓存和预装帧。每页写后回读，全部记录校验通过后才写有效标记。缺帧、写入失败或掉电未提交的序列不会启动。
- [`user.c`](LustageBody_Light_20241022/Core/Src/user.c) 接入六通道命令和主循环。光源先通过 TIM6 确认首个触发，再由 TIM1 按 1 MHz 计时，以比较事件或累计溢出覆盖短帧和长帧。定时中断只置标志，主循环负责换帧。
- 当前帧仍在输出时，主循环读取下一帧，并将亮度写入两颗 MCP4728 的易失输入寄存器。到期后通过 LDAC 锁存亮度并更新通道使能。正常换帧和循环回绕不插入软件全灭操作。各光源在首个触发后使用各自的本地时钟。
- 旧模式继续使用各自的参数和内部 Flash。收到光源升级准备或升级启动命令时，先退出六通道再进入原升级流程。固件升级协议、Bootloader 和内部 Flash 地址沿用原工程。

### 8.3 数据帧结构

帧头固定为 `4C 55`。长度 `Len` 使用小端，值为 **命令字字节数加数据字节数**。命令字按高字节在前发送。校验和为命令字起至最后一个数据字节的累加和，取低 8 位。

**准备帧 `0x02AC`，共 9 字节**

```text
4C 55 | 04 00 | 02 AC | CSV 总数(2，小端) | 校验和(1)
```

| 完整帧（十六进制） | 含义 |
| --- | --- |
| `4C 55 04 00 02 AC 01 00 AF` | 擦除并准备接收 1 帧 |
| `4C 55 04 00 02 AC B8 0B 71` | 擦除并准备接收 3000 帧 |
| `4C 55 04 00 02 AC 00 00 AE` | 选择已保存的序列，不擦除 |

**数据帧 `0x02AD`，固定 490 字节**

```text
4C 55 | E5 01 | 02 AD | CSV 序号(2，小端) | 分包号(1)
      | 30 条光源记录，每条 16 字节 | 校验和(1)
```

| 单条光源记录 | 类型 | 内容 |
| --- | --- | --- |
| `R、G、B、W、A、C` | 6 个小端 `uint16` | 各路亮度。DAC 输出按工程中的 `CMD_Power_Max`（2500）限幅。 |
| 持续时间 | 小端 `uint32` | 单位为 µs，有效范围 `1000～3600000000`。例如 `E8 03 00 00` 为 1 ms，`00 A4 93 D6` 为 1 h。 |

CSV 序号从 1 起，同一个 CSV 的 26 个分包使用相同序号。第 1～25 包各有 30 条有效记录。第 26 包对应 ID 751～776，共 26 条有效记录，末尾补 4 条全零记录。补零也计入长度和校验和。按 ID 排列时，本机分包号为 `(ID - 1) / 30 + 1`，包内索引为 `(ID - 1) % 30`，使用整数除法和零起始索引。

### 8.4 验证范围

Keil 已完成编译。主机模拟覆盖了 490 字节分包、3000 帧写入与回读、序列恢复、漏包拒绝启动、单次触发连播、循环回绕和 TIM1 计时边界。联调时仍需在实板测量连续 1 ms 帧的输出时长和帧间光输出，核对 LDAC 接线，并回归光源升级与重启。
