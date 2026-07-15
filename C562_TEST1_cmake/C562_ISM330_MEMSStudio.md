# C562 + ISM330DHCX 兼容 MEMSStudio 开发笔记

---

## 1. 概述

本文档记录如何基于 **STM32C562RET6** + **ISM330DHCX** 实现与 **MEMSStudio**（原 Unicleo-GUI）的串口协议兼容，使其能够自动发现设备并实时显示六轴传感器数据。

本文涵盖协议分析、帧格式定义、命令处理流程，以及实际调试中的关键细节。

---

## 2. 硬件架构

```
+------------------+       USART2 (115200 bps)       +------------------+
|  NUCLEO-C562RE  | <-------- ST-Link VCP --------> |   MEMSStudio     |
|                  |                                 |   (PC GUI)       |
|  STM32C562RET6   |                                 |                  |
|  +------------+  |                                 |                  |
|  | ISM330DHCX |  | <--- I2C1 (400kHz) --->       |                  |
|  | (0x6B)     |  |                                 |                  |
|  +------------+  |                                 +------------------+
+------------------+
```

| 引脚 | 连接 | 说明 |
|---|---|---|
| PA2 (TX) | ST-Link TX → PC | USART2 发送 |
| PA3 (RX) | ST-Link RX ← PC | USART2 接收 |
| PB6 (SCL) | ISM330DHCX SCL | I2C1 时钟 |
| PB7 (SDA) | ISM330DHCX SDA | I2C1 数据 |
| 3V3 / GND | ISM330DHCX VCC/GND | 电源 |

> **注意:** ISM330DHCX 的 I2C 地址为 `0x6B`（`ISM330DHCX_I2C_ADD_H`），接法为 SA0 悬空（默认高）。

---

## 3. 串口协议概述

MEMSStudio 使用 ST 私有的 **DataLogFusion** 串行协议，核心特性：

1. **Byte-Stuffing** — 防止帧内出现协议控制字符（0xF0/0xF1）
2. **CheckSum** — 校验和验证数据完整性
3. **命令-响应模式** — PC 发命令，MCU 回复
4. **主动数据流** — MCU 周期性主动上报传感器数据

### 3.1 帧结构

```
[DestAddr][SrcAddr][CMD][Payload...][CheckSum][0xF0]
   1B       1B      1B     N B        1B       1B
```

| 字段 | 大小 | 描述 |
|---|---|---|
| `DestAddr` | 1 byte | 目标地址（MCU 地址 = `DEV_ADDR`） |
| `SrcAddr` | 1 byte | 源地址（PC = `0x00`） |
| `CMD` | 1 byte | 命令码 |
| `Payload` | N bytes | 命令数据 |
| `CheckSum` | 1 byte | 校验和（`sum(Data) == 0`） |
| `0xF0` | 1 byte | 帧结束标记（`Msg_EOF`） |

### 3.2 字节填充规则

| 原字符 | 编码后 | 说明 |
|---|---|---|
| `0xF0` | `0xF1 0xF2` | 帧结束标记转义 |
| `0xF1` | `0xF1 0xF1` | 转义前缀自身转义 |
| 其他 | 原样保留 | 不做变换 |

解码时：遇到 `0xF1` 则读取下一字节，`0xF2` → `0xF0`，`0xF1` → `0xF1`。

### 3.3 校验和

发送端：对所有数据字节（含 DestAddr/SrcAddr/CMD/Payload）做 **按位取反累加**：
```c
uint8_t chk = 0;
for (i = 0; i < len; i++) chk -= Data[i];
```

接收端：验证 `Σ Data[i] + CheckSum == 0`（即校验和不为零错误）。

---

## 4. 命令码

| CMD 名称 | 值 | 方向 | 描述 |
|---|---|---|---|
| `CMD_Ping` | `0x01` | PC→MCU | 心跳请求，MCU 回复 Pong |
| `CMD_Read_PresString` | `0x02` | PC→MCU | 读取设备描述字符串 |
| `CMD_Start_Data_Streaming` | `0x08` | PC→MCU | 启动数据流（Payload 含传感器使能掩码） |
| `CMD_Stop_Data_Streaming` | `0x09` | PC→MCU | 停止数据流 |
| `CMD_ChangeSF` | `0x07` | PC→MCU | 切换 6轴/9轴 融合模式 |
| `CMD_ACCELERO_GYRO_Init` | `0x76` | PC→MCU | 初始化 Acc+Gyro，MCU 回复传感器 ID |
| `CMD_Get_App_Info` | `0x12` | PC→MCU | 获取算法频率、数据需求信息 |
| 回复标志 | `0x80` | MCU→PC | 回复时 CMD = 原 CMD \| 0x80 |

---

## 5. 握手流程

完整 MEMSStudio 连接握手顺序：

```
MEMSStudio (PC)                          MCU (C562)
     |                                        |
     | -------- PING (0x01) ------------->   |
     | <------- PONG (0x81) ---------------   |
     |                                        |
     | -------- Read PresString (0x02) --->  |
     | <------- "MEMS shield demo..." (0x82) |
     |                                        |
     | -------- ACCELERO_GYRO_Init (0x76) ->  |
     | <------- UNICLEO_ID = 11 (0xF6) -----  |
     |                                        |
     | -------- Get_App_Info (0x12) ------->  |
     | <------- AlgoFreq=100Hz (0x92) ------  |
     |                                        |
     | -------- Start_Data_Streaming (0x08) ->|
     | <------- ACK (0x88) -----------------  |
     |                                        |
     | <====== 100Hz ACC+GYR data stream === |
```

> 握手流程是确定性的——任意一步失败，MEMSStudio 将无法开始数据流。

---

## 6. 数据流格式

### 6.1 精简格式（仅 ACC+GYR）

当前 C562_TEST1 使用 24 字节有效载荷：

| 偏移 | 大小 | 字段 | 单位 | 类型 |
|---|---|---|---|---|
| 3 | 4 | ACC_X | mg | int32 |
| 7 | 4 | ACC_Y | mg | int32 |
| 11 | 4 | ACC_Z | mg | int32 |
| 15 | 4 | GYR_X | mdps | int32 |
| 19 | 4 | GYR_Y | mdps | int32 |
| 23 | 4 | GYR_Z | mdps | int32 |

总帧长：Header(3) + Payload(24) + CheckSum(1) + EOF(1) = **29 字节**

### 6.2 完整格式（119 字节）

如果启用 MotionFX 融合算法，需扩展至完整 119 字节：

| 偏移 | 大小 | 字段 | 单位 | 描述 |
|---|---|---|---|---|
| 0–2 | 3 | Header | — | Dest=0x00, Src=DEV_ADDR, CMD=0x08 |
| 3–6 | 4 | RTC time | — | 无 RTC 时为 0 |
| 7–10 | 4 | Pressure | float | 未使用时为 0 |
| 11–14 | 4 | Temperature | float | 未使用时为 0 |
| 15–18 | 4 | Humidity | float | 未使用时为 0 |
| 19–30 | 12 | ACC XYZ | int32 mg | 加速度计 |
| 31–42 | 12 | GYR XYZ | int32 mdps | 陀螺仪 |
| 43–54 | 12 | MAG XYZ | int32 mGauss | 磁力计（未用） |
| 55–70 | 16 | Quaternion | float×4 | 四元数（融合输出） |
| 71–82 | 12 | Rotation YPR | float×3 | 欧拉角 |
| 83–94 | 12 | Gravity | float×3 | 重力分量 |
| 95–106 | 12 | LinearAcc | float×3 | 线性加速度 |
| 107–110 | 4 | Heading | float | 航向角 |
| 111–114 | 4 | HeadingErr | float | 航向误差 |
| 115–118 | 4 | Elapsed | uint32 μs | 时间戳 |

> 当前 C562_TEST1 采用精简 24 字节格式，未启用 MotionFX。若需扩展至 119 字节，需引入 `motion_fx_manager` 并实现 Quaternion 等字段计算。

---

## 7. 关键实现细节

### 7.1 传感器初始化

```c
/* I2C address: 0x6B (SA0 floating) */
/* Full scale: ±4g (ACC), ±2000dps (GYR) */
/* ODR: 104 Hz (closest to 100Hz target) */

BSP_SENSOR_ACC_Init();           /* → ism330dhcx_init() → I2C write reg */
BSP_SENSOR_GYR_Init();
BSP_SENSOR_ACC_SetOutputDataRate(104.0f);  /* 104Hz ≈ 100Hz */
BSP_SENSOR_GYR_SetOutputDataRate(104.0f);
BSP_SENSOR_ACC_SetFullScale(4);             /* ±4g */
BSP_SENSOR_GYR_SetFullScale(2000);          /* ±2000dps */
BSP_SENSOR_ACC_Enable();
BSP_SENSOR_GYR_Enable();
```

### 7.2 UART 接收（IDLE 线检测）

C562_TEST1 使用 `HAL_UART_ReceiveToIdle_IT`（IDLE 中断）检测帧边界，而非 DMA 环形缓冲：

```c
/* com.c — UART IDLE RX */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart,
                              uint16_t Size, uint8_t Halt)
{
    /* 数据存入 RxBuffer，IDLE 时触发完整帧回调 */
    if (huart == &huart2) {
        Com_ReceiveCallback(rx_buffer, Size);
    }
}

void Com_StartReceive(void) {
    HAL_UART_ReceiveToIdle_IT(&huart2, rx_buffer, RX_BUFFER_SIZE);
}
```

### 7.3 命令分发

```c
/* app_mems_protocol.c */
int32_t MEMS_HandleCommand(Msg_t *msg)
{
    switch (msg->Command) {
    case CMD_Ping:                return MEMS_HandlePing(msg);
    case CMD_Read_PresString:     return MEMS_HandlePresString(msg);
    case CMD_ACCELERO_GYRO_Init:  return MEMS_HandleAccGyrInit(msg);
    case CMD_Get_App_Info:        return MEMS_HandleAppInfo(msg);
    case CMD_Start_Data_Streaming: return MEMS_HandleStartStreaming(msg);
    case CMD_Stop_Data_Streaming: return MEMS_HandleStopStreaming(msg);
    default: return -1; /* unknown command */
    }
}
```

### 7.4 构建流帧

```c
/* 24-byte ACC+GYR frame */
void MEMS_SendStreamingFrame(MOTION_SENSOR_Axes_t *acc,
                              MOTION_SENSOR_Axes_t *gyr)
{
    Msg_t msg;
    uint8_t payload[24];
    uint8_t i = 0;

    /* Header */
    msg.Dest = 0x00;
    msg.Src  = DEV_ADDR;
    msg.Command = CMD_Start_Data_Streaming;

    /* ACC XYZ (int32, mg) */
    Serialize_s32(&payload[i], acc->x, 4); i += 4;
    Serialize_s32(&payload[i], acc->y, 4); i += 4;
    Serialize_s32(&payload[i], acc->z, 4); i += 4;

    /* GYR XYZ (int32, mdps) */
    Serialize_s32(&payload[i], gyr->x, 4); i += 4;
    Serialize_s32(&payload[i], gyr->y, 4); i += 4;
    Serialize_s32(&payload[i], gyr->z, 4); i += 4;

    msg.Length = i;
    msg.Data   = payload;

    CHK_ComputeAndAdd(&msg);  /* add checksum */
    ByteStuffCopy(stuffed, &msg);
    Com_SendMsg(&msg);         /* UART transmit */
}
```

---

## 8. 常见问题与调试

| 问题 | 可能原因 | 解决方案 |
|---|---|---|
| MEMSStudio 不识别设备 | 握手顺序不对或超时 | 检查 PING/PONG 是否正常，串口波形是否正确 |
| 数据乱码或帧解析错误 | 波特率不匹配（应为 115200） | 确认 PC 端 COM 端口设置 |
| 字节填充后帧超长 | Payload 中意外出现 0xF0/0xF1 | 检查 `ByteStuffCopyByte` 是否覆盖所有数据 |
| 校验和不匹配 | 发送端/接收端 CHK 计算方式不同 | 确认 `chk -= Data[i]` 格式 |
| 传感器数据全零 | I2C 通信失败或 ISM330DHCX 未使能 | 检查 WHO_AM_I (0x0F = 0x6B)，确认 SDA/SCL 上拉电阻 |
| 数据流只有一帧 | Start 命令未正确回复 ACK | 确认 `CMD | 0x80` 回复格式 |

### 8.1 逻辑分析仪调试

推荐使用 Saleae 或类似工具抓取 UART 波形，关键观察点：

1. **PING (0x01)** — MCU 是否在 1s 内回复 **0x81**
2. **帧尾 0xF0** — 每个回复帧是否以 `0xF0` 结尾
3. **字节填充** — Payload 中出现 `0xF0` 时是否被正确替换为 `0xF1 0xF2`

> 协议调试建议使用逻辑分析仪同时抓取 UART TX/RX 两路波形，检查 PING→PONG 时序、帧尾 0xF0 标记以及字节填充是否正确。

---

## 9. 相关文件索引

| 文件 | 职责 |
|---|---|
| `App/app_mems_protocol.c/h` | MEMSStudio 协议处理、流帧构建 |
| `App/mems_control.c/h` | BSP 适配层：`BSP_SENSOR_*` API |
| `Middlewares/ComProtocol/Src/com.c` | UART IDLE 接收、阻塞发送 |
| `Middlewares/ComProtocol/Src/serial_protocol.c` | Byte-Stuffing、CHK、序列化/反序列化 |
| `Middlewares/ComProtocol/Inc/serial_cmd.h` | 命令码定义 (`CMD_*`, `DEV_ADDR`) |
| `part_drivers/ism330dhcx/*` | ISM330DHCX 驱动（ST 标准 part driver） |
| `main.c` | 系统初始化、100Hz 主循环 |
| `generated/hal/mx_usart2.c/h` | CubeMX 生成的 USART2 初始化 |

---

## 10. 总结

| 项目 | 状态 | 说明 |
|---|---|---|
| PING/PONG | ✅ | 0x01 ↔ 0x81 |
| PresString | ✅ | 设备描述字符串 |
| ACC+GYR Init | ✅ | UNICLEO_ID = 11 |
| App Info | ✅ | AlgoFreq = 100Hz |
| Start/Stop Streaming | ✅ | 100Hz 数据流 |
| Byte-Stuffing | ✅ | 0xF0→0xF1 0xF2, 0xF1→0xF1 0xF1 |
| CheckSum | ✅ | `sum + chk == 0` |
| MotionFX fusion | ❌ | 未实现（保持精简） |
| DMA TX | ❌ | 当前为阻塞发送 |

C562_TEST1 已完整实现 MEMSStudio 兼容的 ACC+GYR 数据流功能，代码结构清晰（App/Middleware/Driver 三层分离），适合作为 MEMSStudio 协议移植的参考起点。
