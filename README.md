# STM32C562_NUCLEO_MEMS

> ISM330DHCX 6-Axis IMU streaming to MEMSStudio via UART on NUCLEO-C562RE
>
> 基于 NUCLEO-C562RE，通过 UART 将 ISM330DHCX 六轴 IMU 数据上报至 MEMSStudio

---

## Overview / 概述

STM32C562-based sensor data acquisition system that streams accelerometer and gyroscope data to [MEMSStudio](https://www.st.com/en/embedded-software/stmems-memsstudio.html) using the ST proprietary serial protocol.

基于 STM32C562 的传感器数据采集系统，使用 ST 专有串行协议将加速度计和陀螺仪数据流式传输至 [MEMSStudio](https://www.st.com/en/embedded-software/stmems-memsstudio.html)。

| Item / 项目 | Detail / 详情 |
|---|---|
| **MCU** | STM32C562RET6 (Cortex-M33, 144 MHz) |
| **Sensor / 传感器** | ISM330DHCX (I2C, 6-axis ACC + GYR / 六轴加速度+陀螺仪) |
| **Output / 输出** | 100 Hz data stream → MEMSStudio over USART2 |
| **Board / 板卡** | NUCLEO-C562RE |

## Features / 特性

- ✅ MEMSStudio-compatible serial protocol (byte-stuffed, checksummed) — 兼容 MEMSStudio 串行协议（字节填充 + 校验和）
- ✅ Full command dispatch: Ping, PresString, Sensor Init, App Info, Start/Stop Streaming — 完整命令分发
- ✅ 100 Hz ACC + GYR data frames / 数据帧
- ✅ No RTOS, no external dependencies — pure HAL + CMSIS / 无 RTOS，无外部依赖，纯 HAL + CMSIS
- ✅ Clean layered architecture: App / Middleware / Drivers / 清晰的分层架构

## Quick Start / 快速开始

### Prerequisites / 前置条件

- STM32Cube CLI or STM32CubeIDE / STM32Cube CLI 或 STM32CubeIDE
- CMake ≥ 3.20 + Ninja
- `arm-none-eabi-gcc` 14.3.1

### Build / 构建

```bash
cmake --preset debug_GCC_NUCLEO-C562RE
cmake --build --preset debug_GCC_NUCLEO-C562RE
```

### Flash & Run / 烧录与运行

1. Flash `C562_TEST1_cmake/build/debug_GCC_NUCLEO-C562RE/C562_TEST1.elf` to the board / 烧录到板卡
2. Connect USART2 (ST-Link VCP) to MEMSStudio / 将 USART2（ST-Link 虚拟串口）连接至 MEMSStudio
3. MEMSStudio auto-discovers the device and starts streaming / MEMSStudio 自动发现设备并开始数据流

## Project Structure / 项目结构

```
├── C562_TEST1.ioc2              # CubeMX project file / CubeMX 工程文件
├── C562_TEST/                   # CubeMX IDE metadata / IDE 元数据
└── C562_TEST1_cmake/            # CMake-based firmware / CMake 固件工程
    ├── App/                     # Application layer (protocol + BSP) / 应用层
    ├── Middlewares/ComProtocol/ # UART frame protocol stack / UART 帧协议栈
    ├── part_drivers/            # ISM330DHCX, button, LED drivers / 外设驱动
    ├── generated/               # CubeMX generated HAL init code / CubeMX 生成代码
    ├── stm32c5xx_drivers/       # STM32C5 HAL + LL drivers / HAL/LL 驱动
    ├── stm32c5xx_dfp/           # Device Family Pack / 器件家族包
    ├── cmake/                   # CMake toolchain + config / CMake 工具链配置
    ├── main.c                   # Entry point / 入口
    └── CMakeLists.txt
```

## Documentation / 文档

| Document / 文档 | Description / 说明 |
|---|---|
| [README.md](C562_TEST1_cmake/README.md) | Detailed project README / 详细项目说明 |
| [PROJECT_OVERVIEW.md](C562_TEST1_cmake/PROJECT_OVERVIEW.md) | Full architecture & design notes / 完整架构与设计笔记 |
| [memsstudio.md](C562_TEST1_cmake/memsstudio.md) | MEMSStudio protocol analysis / 协议分析 |

## MEMSStudio Handshake / 握手协议

```
MEMSStudio → MCU:  PING                    (0x01)
MCU → MEMSStudio:  PONG                    (0x81)
MEMSStudio → MCU:  Read PresString         (0x02)
MCU → MEMSStudio:  "MEMS shield demo,…"    (0x82)
MEMSStudio → MCU:  ACCELERO_GYRO_Init      (0x76)
MCU → MEMSStudio:  UNICLEO_ID = 11         (0xF6)
MEMSStudio → MCU:  Get_App_Info            (0x12)
MCU → MEMSStudio:  AlgoFreq=100Hz          (0x92)
MEMSStudio → MCU:  Start_Data_Streaming    (0x08)
MCU → MEMSStudio:  [100 Hz data frames…]   / 100Hz 数据帧
```

## Streaming Frame / 数据流帧格式 (119 bytes)

```
 0- 2: Header   (Dest=0x00, Src=50, CMD=0x08)
 3- 6: RTC time (zero — no RTC on this board / 无 RTC)
 7-10: Pressure float  (zero / 未使用)
11-14: Temp float      (zero / 未使用)
15-18: Humidity float  (zero / 未使用)
19-30: Acc XYZ  (3× int32, mg)      / 加速度
31-42: Gyr XYZ  (3× int32, mdps)    / 角速度
43-118: Zero (MAG, Quaternion, Rotation, Gravity, LinAcc, Heading, HeadingErr, Elapsed / 未使用)
```

## Current Status / 当前状态

- ✅ Build passing / 构建通过 ([31/31] Linking → C562_TEST1.elf)
- ✅ MEMSStudio command handling / 命令处理完整
- ✅ ACC+GYR data streaming @ 100Hz / 数据流正常
- ✅ FS=±4g / ±2000dps, ODR=104Hz
- ⚠️ No MotionFX fusion / 未集成 MotionFX 融合算法
- ⚠️ Superloop (no RTOS) / 主循环轮询（无 RTOS）
- ⚠️ Blocking UART TX / 串口发送为阻塞模式

## License / 许可证

Copyright (c) 2026 STMicroelectronics. All rights reserved. / 版权所有。
See LICENSE files in subdirectories for component-specific terms. / 各组件许可见子目录 LICENSE 文件。
