# DataLogFusion 与 MEMSStudio 通信协议分析

## 1. 通信架构

```
MEMSStudio (PC) <-- UART DMA --> MCU (DataLogFusion)
                            |
                     串口协议 (Byte Stuffing + CheckSum)
```

- **物理层**：UART（默认 115200 bps，DataLogFusion 用 VCP 通过 ST-Link 桥到 PC）
- **链路层**：Byte-Stuffing 帧封装（避开 0xF0/0xF1）+ 末尾校验和
- **会话层**：命令-响应 + 周期性数据流（TIM 中断触发）

---

## 2. 串口帧格式

每帧 `Msg_t` 结构：

```
[DestAddr][SrcAddr][CMD][Payload...][CheckSum][0xF0(EOF)]
   1字节     1字节    1字节   N字节     1字节    1字节
```

**关键控制字符（字节填充防冲突）：**

| 字符       | 值    | 含义                         |
|------------|-------|------------------------------|
| `Msg_EOF`  | 0xF0 | 帧结束标记                   |
| `Msg_BS`   | 0xF1 | 转义前缀（Byte-Stuff）       |
| `Msg_BS_EOF` | 0xF2 | 0xF0 的转义形式              |
| `Msg_BS+Msg_BS` | 0xF1 0xF1 | 0xF1 的转义形式         |

**编码过程（`ByteStuffCopyByte`）：** 0xF0 → 0xF1 0xF2，0xF1 → 0xF1 0xF1，其他原样

**解码过程（`ReverseByteStuffCopy`）：** 检测到 0xF1 后读下一字节，0xF2→0xF0，0xF1→0xF1，其他为非法

**校验和（`CHK_CheckAndRemove`）：** 发送端 `CHK_ComputeAndAdd`（逐字节求补 `chk -= Data[i]`），接收端校验 `Σ Data[i] + checksum == 0`

---

## 3. 命令码 (serial_cmd.h)

| CMD                          | 值     | 方向     | 说明                                       |
|------------------------------|--------|----------|--------------------------------------------|
| `CMD_Ping`                   | 0x01   | PC→MCU   | 心跳，MCU 回复 Ping                       |
| `CMD_Read_PresString`        | 0x02   | PC→MCU   | 读取设备描述字符串                         |
| `CMD_Start_Data_Streaming`   | 0x08   | PC→MCU   | 启动数据流，Payload=传感器使能掩码          |
| `CMD_Stop_Data_Streaming`    | 0x09   | PC→MCU   | 停止数据流                                 |
| `CMD_ChangeSF`               | 0x07   | PC→MCU   | 切换 6 轴 / 9 轴融合模式                  |
| `CMD_ACCELERO_GYRO_Init`     | 0x76   | PC→MCU   | 查询 Acc+Gyro 传感器 ID                    |
| `CMD_MAGNETO_Init`           | 0x7A   | PC→MCU   | 查询 Mag 传感器 ID                         |
| `CMD_PRESSURE_Init`          | 0x60   | PC→MCU   | 查询 Pressure 传感器 ID                    |
| `CMD_HUMIDITY_TEMPERATURE_Init` | 0x62 | PC→MCU | 查询温湿度传感器 ID                        |
| `CMD_Get_App_Info`           | 0x12   | PC→MCU   | 获取算法频率、需求数据量                   |
| `CMD_Offline_Data`           | 0x10   | PC→MCU   | 发送离线数据（用于回放）                   |
| `CMD_Use_Offline_Data`       | 0x11   | PC→MCU   | 切换离线/在线模式                          |
| `CMD_Reply_Add`              | 0x80   | —        | 应答标志：回复 CMD = 原 CMD \| 0x80        |

---

## 4. 启动数据流流程

**PC 发送：**
```
[0x00][DEV_ADDR][CMD_Start_Data_Streaming][SensorMask:4字节]
```
- `SensorMask` 位标志（DataLogFusion 中）：
  - `PRESSURE_SENSOR  = 0x00000001`
  - `TEMPERATURE_SENSOR = 0x00000002`
  - `HUMIDITY_SENSOR  = 0x00000004`
  - `UV_SENSOR        = 0x00000008`
  - `ACCELEROMETER_SENSOR = 0x00000010`
  - `GYROSCOPE_SENSOR  = 0x00000020`
  - `MAGNETIC_SENSOR   = 0x00000040`

**MCU 回复：**
```
[Dest][Src][CMD_Start_Data_Streaming | 0x80][]
```
- 同时启动 TIM 周期性中断（`HAL_TIM_PeriodElapsedCallback`），以 `ALGO_FREQ`（DataLogFusion 默认 100Hz）发送数据帧

---

## 5. 数据流格式 (app_mems.c 中 `FX_Data_Handler`)

每帧 `STREAMING_MSG_LENGTH = 119` 字节有效载荷，格式：

| 偏移 | 长度 | 内容                                |
|------|------|-------------------------------------|
| 3    | 1    | Hours                               |
| 4    | 1    | Minutes                             |
| 5    | 1    | Seconds                             |
| 6    | 1    | SubSec                              |
| 7    | 4    | Pressure `[float]`                  |
| 11   | 4    | Temperature `[float]`               |
| 15   | 4    | Humidity `[float]`                  |
| 19   | 12   | ACC XYZ `[int32 × 3]` 单位 mg       |
| 31   | 12   | GYR XYZ `[int32 × 3]` 单位 mdps     |
| 43   | 12   | MAG XYZ `[int32 × 3]` 单位 mGauss   |
| 55   | 16   | Quaternion `[float × 4]`            |
| 71   | 12   | Rotation Yaw/Pitch/Roll `[float × 3]` |
| 83   | 12   | Gravity XYZ `[float × 3]`           |
| 95   | 12   | LinearAcc XYZ `[float × 3]`         |
| 107  | 4    | Heading `[float]`                   |
| 111  | 4    | HeadingErr `[float]`                |
| 115  | 4    | Elapsed time `[μs]`                 |

---

## 6. 关键实现文件

| 文件                          | 作用                                                                 |
|-------------------------------|----------------------------------------------------------------------|
| `com.c`                       | UART DMA 收发循环缓冲，帧边界检测（0xF0）                            |
| `serial_protocol.c`           | 字节填充/去填充、校验和计算、序列化/反序列化                          |
| `demo_serial.c`               | 命令处理分发器（`HandleMSG`），回复构建                              |
| `app_mems.c`                  | TIM 中断触发，数据采集，数据填入 Msg 流，`UART_SendMsg` 发送        |
| `motion_fx_manager.c`         | MotionFX 9X/6X 融合算法封装（`run/start_6X/stop_9X/MagCal_*`）       |
| `iks02a1_mems_control.c`      | 板级 BSP 适配层（ACC/GYR/MAG/PRESS/TEMP/HUM 的 Init/Enable/Get）    |
| `sensor_unicleo_id.h`         | MEMSStudio 识别的传感器型号 ID 表                                    |

---

## 7. 关键函数/宏接口

```c
// 帧封装
int32_t  ByteStuffCopyByte(uint8_t *Dest, uint8_t Source);
int32_t  ByteStuffCopy(uint8_t *Dest, Msg_t *Source);
int32_t  ReverseByteStuffCopy(Msg_t *Dest, uint8_t *Source);

// 校验
void     CHK_ComputeAndAdd(Msg_t *Msg);
int32_t  CHK_CheckAndRemove(Msg_t *Msg);

// 序列化
void     Serialize_s32(uint8_t *Dest, int32_t Source, uint32_t Len);
void     Serialize_u32(uint8_t *Dest, uint32_t Source, uint32_t Len);
uint32_t Deserialize_u32(uint8_t *Source, uint32_t Len);
void     FloatToArray(uint8_t *Dest, float Data);

// 帧头/回复宏
void BUILD_REPLY_HEADER(Msg_t *Msg);    // [0]=Src [1]=DEV_ADDR [2]+=0x80
void INIT_STREAMING_HEADER(Msg_t *Msg); // [0]=Dest [1]=DEV_ADDR [2]=0x08

// UART 收发
void    UART_StartReceiveMsg(void);
int32_t UART_ReceivedMSG(Msg_t *Msg);
void    UART_SendMsg(Msg_t *Msg);

// 命令处理
int32_t HandleMSG(Msg_t *Msg);
```

---

## 8. MEMSStudio 作为"上位机"的核心要点

1. **UART DMA 环形缓冲** — 持续接收，以 0xF0 识别帧边界
2. **反向字节填充** — 解码 0xF1 0xF2 → 0xF0，0xF1 0xF1 → 0xF1
3. **校验和** — `sum(Data) == 0` 则通过
4. **命令分发** — 解析 CMD 字段，调用对应处理函数
5. **启动流** — TIM 周期性发送数据帧，PC 端接收并解析绘图

---

## 9. DataLogFusion 工程分层（参考结构）

```
┌──────────────────────────────────────────────────────────┐
│  app_mems.c           [应用层]  TIM 中断 + 数据流调度     │
├──────────────────────────────────────────────────────────┤
│  demo_serial.c        [协议层]  HandleMSG / InitHeader    │
│  motion_fx_manager.c  [算法层]  MotionFX 9X/6X            │
├──────────────────────────────────────────────────────────┤
│  iks02a1_mems_control.c  [BSP 适配]  ACC/GYR/MAG Init/... │
│  iks02a1_motion_sensors.c [驱动]   IKS02A1 → ism330dhcx │
├──────────────────────────────────────────────────────────┤
│  com.c                [链路层]  UART DMA + 帧边界         │
│  serial_protocol.c    [编解码]  Byte-Stuffing + Checksum  │
├──────────────────────────────────────────────────────────┤
│  stm32u5xx_hal_uart / stm32u5xx_hal_tim / stm32u5xx_hal │
└──────────────────────────────────────────────────────────┘
```

---

## 10. C562_TEST1 工程当前状态（移植前）

| 资源              | 已有 / 缺失 | 备注                                         |
|-------------------|-------------|----------------------------------------------|
| `serial_protocol.c/h` | 已有     | Byte-Stuffing + CHK + Serialize/Deserialize  |
| `serial_cmd.h`    | 已有（精简）| 缺少 `CMD_Offline_Data/Use_Offline_Data/Set_DateTime/Enter_DFU_Mode/Reset` |
| `com.c/h`         | 已有（基于 `HAL_UART_ReceiveToIdle_IT`）| 与 DataLogFusion 的 `HAL_UART_Receive_DMA + 环形缓冲` 实现方式不同 |
| `app_mems_protocol.c/h` | 已有 | 已实现 Ping/PresString/Start-Stop/ACC_GYR_Init 简单命令 + 24 字节 ACC+GYR 流 |
| `motion_fx_manager` | 缺失       | 后续若需要融合算法需补全                     |
| `mems_control.c/h` | 缺失       | BSP 适配层：`BSP_SENSOR_*` 函数集缺失         |
| `sensor_unicleo_id.h` | 缺失    | 传感器 ID 需补 `ISM330DHCX_UNICLEO_ID = 11`  |
| `fw_version.h`    | 缺失        | 演示字符串使用                                |
| `MX_DataLogFusion_Process` 循环 | 缺失 | TIM + RTOS 调度                               |

**当前数据流格式（C562_TEST1）：**
```
[0x00][DEV_ADDR][CMD_Start_Data_Streaming]
  ACC_x(4) ACC_y(4) ACC_z(4) GYR_x(4) GYR_y(4) GYR_z(4) = 24 字节
  [CHK][0xF0]
```
总长 26 字节（Header 3 + Payload 24 + CHK 1 + EOF 1），**仅 ACC+GYR**，未含 Time/Press/Temp/Hum/Quaternion。

---

## 11. 移植目标（仅 ACC+GYR）

### 11.1 移植策略

1. **保留现有 `serial_protocol.c/h` + `com.c/h`**：Byte-Stuffing/校验/UART 收发已可用
2. **保留现有 `app_mems_protocol.c/h`**：已能处理 Ping/PresString/Start-Stop/ACC_GYR_Init
3. **新增 `mems_control.c/h`（BSP 适配层）**：把 `ism330dhcx` part_driver 包装成 `BSP_SENSOR_ACC/GYR_*` 统一 API
4. **新增 `sensor_unicleo_id.h`**：提供 `ISM330DHCX_UNICLEO_ID = 11` 等宏
5. **新增 `fw_version.h`**：用于 `Get_PresentationString`
6. **新增 `bsp_ip_conf.h`**：把 LED 等 BSP 元素集中声明（参考 DataLogFusion）
7. **CMake 集成**：在 `cmake/components.cmake` 中加 `Middlewares/ComProtocol` 与 `App` 路径

### 11.2 数据流精简（仅 ACC+GYR）

保持与 C562_TEST1 当前一致的格式（24 字节有效载荷）：

| 偏移 | 长度 | 内容         | 单位  | 类型   |
|------|------|--------------|-------|--------|
| 3    | 4    | ACC_X        | mg    | int32  |
| 7    | 4    | ACC_Y        | mg    | int32  |
| 11   | 4    | ACC_Z        | mg    | int32  |
| 15   | 4    | GYR_X        | mdps  | int32  |
| 19   | 4    | GYR_Y        | mdps  | int32  |
| 23   | 4    | GYR_Z        | mdps  | int32  |
| 24   | 1    | CHK          |       | uint8  |
| 25   | 1    | EOF (0xF0)   |       | uint8  |

注：第 0~2 字节为 `Dest(0x00)/Src(DEV_ADDR)/CMD(0x08)`。

### 11.3 分层架构（目标）

```
┌──────────────────────────────────────────────────────────┐
│  main.c                  [应用入口]  系统初始化 + 循环    │
├──────────────────────────────────────────────────────────┤
│  App/app_mems_protocol.c [协议业务]  命令解析 + 流帧构建  │
│  App/mems_control.c      [BSP 适配]  BSP_SENSOR_*         │
├──────────────────────────────────────────────────────────┤
│  Middlewares/ComProtocol/Inc  com.h     [链路层]          │
│  Middlewares/ComProtocol/Src  com.c                       │
│  Middlewares/ComProtocol/Inc  serial_protocol.h           │
│  Middlewares/ComProtocol/Src  serial_protocol.c           │
├──────────────────────────────────────────────────────────┤
│  generated/hal/mx_usart2.*        [HAL UART]             │
│  part_drivers/ism330dhcx/*        [传感器驱动]           │
│  stm32c5xx_drivers/*              [STM32 HAL]            │
└──────────────────────────────────────────────────────────┘
```

### 11.4 主循环（main.c）

```c
for (;;) {
    if (Com_ReceiveMsg(&msg)) {
        MEMS_HandleCommand(&msg);
    }
    if (now - last >= 20U) {                /* 50Hz 轮询 */
        last = now;
        BSP_SENSOR_ACC_GetAxes(&acc);
        BSP_SENSOR_GYR_GetAxes(&gyr);
        MEMS_SendStreamingFrame(&acc, &gyr);
    }
}
```

注：C562_TEST1 暂时不引入 MotionFX（保持精简），所以"算法"层（motion_fx_manager）暂不移植；数据包里只放 ACC+GYR。
