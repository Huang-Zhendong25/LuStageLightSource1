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

## 5. 修改过程注意事项（经验教训）

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

## 7. 单灯验证（USB-485 直连）

1. 光源正常供电，USB-485 接光源 RS485 A/B（A、B 接反则对调）。
2. 串口助手：`250000, 8, N, 1`；HEX 发送/显示。
3. 发查询帧（查 ID=1）：

   ```
   4C 55 04 00 03 55 01 00 59
   ```

4. 期望收到：

   ```
   4C 55 05 00 03 55 01 00 <TEMP> <CS>
   ```

例如温度 30℃（0x1E）：`4C 55 05 00 03 55 01 00 1E 77`。

## 8. 联调（与控制板配合）

完整链路：PC 发 UDP 查询 -> 控制板广播到 8 路 RS485 -> 被查询光源应答 -> 控制板 UDP 转回 PC。详见控制板仓库 README。