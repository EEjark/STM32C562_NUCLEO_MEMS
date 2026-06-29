# STM32C562_NUCLEO_MEMS

> ISM330DHCX 6-Axis IMU streaming to MEMSStudio via UART on NUCLEO-C562RE

## Overview

STM32C562-based sensor data acquisition system that streams accelerometer and gyroscope data to [MEMSStudio](https://www.st.com/en/embedded-software/stmems-memsstudio.html) using the ST proprietary serial protocol.

- **MCU:** STM32C562RET6 (Cortex-M33, 144 MHz)
- **Sensor:** ISM330DHCX (I2C, 6-axis ACC + GYR)
- **Output:** 100 Hz data stream → MEMSStudio over USART2
- **Board:** NUCLEO-C562RE

## Features

- ✅ MEMSStudio-compatible serial protocol (byte-stuffed, checksummed)
- ✅ Full command dispatch: Ping, PresString, Sensor Init, App Info, Start/Stop Streaming
- ✅ 100 Hz ACC + GYR data frames
- ✅ No RTOS, no external dependencies — pure HAL + CMSIS
- ✅ Clean layered architecture: App / Middleware / Drivers

## Quick Start

### Prerequisites

- STM32Cube CLI or STM32CubeIDE
- CMake ≥ 3.20 + Ninja
- `arm-none-eabi-gcc` 14.3.1

### Build

```bash
cmake --preset debug_GCC_NUCLEO-C562RE
cmake --build --preset debug_GCC_NUCLEO-C562RE
```

### Flash & Run

1. Flash `C562_TEST1_cmake/build/debug_GCC_NUCLEO-C562RE/C562_TEST1.elf`
2. Connect USART2 (ST-Link VCP) to MEMSStudio
3. MEMSStudio auto-discovers the device and starts streaming

## Project Structure

```
├── C562_TEST1.ioc2              # CubeMX project file
├── C562_TEST/                   # CubeMX IDE metadata
└── C562_TEST1_cmake/            # CMake-based firmware
    ├── App/                     # Application layer (protocol + BSP)
    ├── Middlewares/ComProtocol/ # UART frame protocol stack
    ├── part_drivers/            # ISM330DHCX, button, LED drivers
    ├── generated/               # CubeMX generated HAL init code
    ├── stm32c5xx_drivers/       # STM32C5 HAL + LL drivers
    ├── stm32c5xx_dfp/           # Device Family Pack
    ├── cmake/                   # CMake toolchain + config
    ├── main.c                   # Entry point
    └── CMakeLists.txt
```

## Documentation

- [README.md](C562_TEST1_cmake/README.md) — Detailed project README
- [PROJECT_OVERVIEW.md](C562_TEST1_cmake/PROJECT_OVERVIEW.md) — Full architecture & design notes
- [memsstudio.md](C562_TEST1_cmake/memsstudio.md) — MEMSStudio protocol analysis

## MEMSStudio Handshake

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
MCU → MEMSStudio:  [100 Hz data frames…]
```

## License

Copyright (c) 2026 STMicroelectronics. All rights reserved.
See LICENSE files in subdirectories for component-specific terms.
