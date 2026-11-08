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

## 3. 本功能在光源侧的改动

### 修改文件

- `LustageBody_Light_20241022/Core/Inc/user.h`
- `LustageBody_Light_20241022/Core/Src/user.c`

### 3.1 新增命令字（user.h）

```c
#define CMD_INFO_LightSource_Temperature        0x0355
```

### 3.2 新增查询处理分支（user.c, Comm_Function_Handler）

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

要点：

- 只有帧内 ID 与自身 `Device_ID` 相等的光源才应答，保证“单灯查询、单灯应答”，避免 485 总线冲突。
- 温度值直接取 `LED_Para_Struct.Temp_Current`（`LED_OTP_Handler` 已周期维护，18B20 采集 + 5 取 1 中值滤波）。

### 3.3 应答发送函数

```c
static uint8_t Comm_RespBuff[16];   // 全局缓冲，避免DMA期间局部变量失效

static void Comm_Send_Data(uint8_t *pData, uint16_t Len)
{
    HAL_UART_AbortReceive(&Comm_Uart);   // 先停掉常驻RX，避免自发自收/回环
    RS485_Mode_Tx();
    HAL_UART_Transmit_DMA(&Comm_Uart, pData, Len);
    // DE回接收由 HAL_UART_TxCpltCallback 完成
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
    Comm_RespBuff[6] = (uint8_t)(Light_ID & 0xFF);        // 光源ID 低字节
    Comm_RespBuff[7] = (uint8_t)((Light_ID >> 8) & 0xFF); // 光源ID 高字节
    Comm_RespBuff[8] = (uint8_t)Temp;                     // 当前温度

    for(uint8_t i = 4 ; i < 9 ; i++)
    {
        sum += Comm_RespBuff[i];
    }
    Comm_RespBuff[9] = sum;     // 校验和

    Comm_Send_Data(Comm_RespBuff, 10);
}
```

### 3.4 485 方向与接收恢复

```c
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &Comm_Uart)
    {
        uint32_t Timeout = 10000;
        while((!__HAL_UART_GET_FLAG(huart, UART_FLAG_TC)) && (Timeout--))
        {
        }
        RS485_Mode_Rx();
        HAL_UARTEx_ReceiveToIdle_DMA(&Comm_Uart, Comm_RxBuff, Comm_RxBuffSzie);
    }
}
```

要点：

- 等 UART 真正“发送完成(TC)”再拉低 DE 切回接收，保证最后 1 字节/停止位不被截断。
- 发完后立即重新挂起 `ReceiveToIdle_DMA`，恢复常驻接收。

## 4. 编译与烧录

- 用 Keil MDK 打开 `LustageBody_Light_20241022/MDK-ARM/LustageBody_Light_20240218.uvprojx` 编译。
- 通过 SWD/ST-Link（或项目使用的烧录方式）烧录到光源板。

## 5. 单灯验证（USB-485 直连）

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

## 6. 联调（与控制板配合）

完整链路：PC 发 UDP 查询 -> 控制板广播到 8 路 RS485 -> 被查询光源应答 -> 控制板 UDP 转回 PC。详见控制板仓库 README。

## 7. 注意事项

- 应答 Cmd 与查询一致，均为 `0x0355`（大端 `03 55`）。
- 发送前会 `HAL_UART_AbortReceive` 停掉常驻 RX；若实际硬件存在总线回环，该处理可避免“自发自收导致重复应答”。
- 只精确匹配 `Device_ID`，不响应 `0xFFFF` 广播 ID（保证单灯应答）。