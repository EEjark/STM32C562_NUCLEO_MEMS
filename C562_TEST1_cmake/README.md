# C562_MEMS — ISM330DHCX Sensor Fusion with MEMSStudio Protocol

STM32C562-based IMU data acquisition system that streams accelerometer and gyroscope
data to [MEMSStudio](https://www.st.com/en/embedded-software/stmems-memsstudio.html) (formerly Unicleo-GUI) via UART using the ST proprietary serial protocol.

## Features

- **6-axis IMU** — ISM330DHCX accelerometer + gyroscope (I2C)
- **MEMSStudio compatible** — Byte-stuffed serial protocol with checksum, full command dispatch (Ping, PresString, Sensor Init, App Info, Start/Stop Streaming)
- **100 Hz data stream** — 119-byte frames matching DataLogFusion layout
- **No external dependencies** — Pure CMSIS + STM32 HAL, no RTOS or third-party libraries
- **Clean layered architecture** — App / Middlewares / Drivers separation

## Hardware

| Component | Detail |
|---|---|
| MCU | STM32C562RET6 (Cortex-M33, 100 MHz) |
| Sensor | ISM330DHCX (I2C1, address 0x6B) |
| UART | USART2 (PA2-TX, PA3-RX, 115200 bps) |
| Board | NUCLEO-C562RE |

## Project Structure

```
.
├── main.c / main.h              # Entry point: MX_Init → superloop
├── App/                         # Application layer
│   ├── app_mems_protocol.c/h    #   MEMSStudio protocol handler + streaming
│   └── mems_control.c/h         #   BSP adaptation (sensor → part_driver)
├── Middlewares/
│   └── ComProtocol/             # Communication protocol stack
│       ├── Inc/
│       │   ├── com.h            #   UART send/receive API
│       │   ├── serial_protocol.h#   Frame structure + byte stuffing + checksum
│       │   └── serial_cmd.h     #   MEMSStudio command codes
│       └── Src/
│           ├── com.c            #   UART IDLE-line RX + blocking TX
│           └── serial_protocol.c#   Byte stuffing / unstuffing / serialization
├── part_drivers/                # ISM330DHCX part driver
├── generated/                   # CubeMX2 generated code (HAL init, RCC, I2C, UART…)
├── stm32c5xx_drivers/           # STM32C5 HAL & LL drivers
├── stm32c5xx_dfp/               # Device Family Pack (startup, system, CMSIS)
├── user_modifiable/             # Linker script, startup source
├── cmake/                       # CMake toolchain + flags + target config
└── CMakeLists.txt / CMakePresets.json
```

## Build

Requires **STM32Cube CLI** or **STM32CubeIDE** with CMake + Ninja + `arm-none-eabi-gcc` 14.3.1.

```bash
cube-cmake --build build/debug_GCC_NUCLEO-C562RE --target C562_TEST1
```

Or configure via CMakePresets:

```bash
cmake --preset debug_GCC_NUCLEO-C562RE
cmake --build --preset debug_GCC_NUCLEO-C562RE
```

## Flash & Run

1. Flash `build/debug_GCC_NUCLEO-C562RE/C562_TEST1.elf` to the NUCLEO-C562RE board
2. Connect USART2 (PA2/PA3) to PC via USB-UART adapter (115200 8N1)
3. Launch MEMSStudio, select the COM port
4. MEMSStudio auto-discovers the device and starts streaming

## MEMSStudio Connection Handshake

```
MEMSStudio → MCU:  PING                    (0x01)
MCU → MEMSStudio:  PONG                    (0x81)
MEMSStudio → MCU:  Read PresString         (0x02)
MCU → MEMSStudio:  "MEMS shield demo,…"    (0x82)
MEMSStudio → MCU:  ACCELERO_GYRO_Init      (0x76)
MCU → MEMSStudio:  UNICLEO_ID = 11         (0xF6)
MEMSStudio → MCU:  Get_App_Info             (0x12)
MCU → MEMSStudio:  AlgoFreq=100Hz, ACC|GYR (0x92)
MEMSStudio → MCU:  Start_Data_Streaming    (0x08)
MCU → MEMSStudio:  ACK                     (0x88)
MCU → MEMSStudio:  [100 Hz data frames…]
```

## Streaming Frame Layout (119 bytes)

```
 0- 2: Header   (Dest=0x00, Src=50, CMD=0x08)
 3- 6: RTC time (zero, no RTC on this board)
 7-10: Pressure float  (zero)
11-14: Temp float      (zero)
15-18: Humidity float  (zero)
19-30: Acc XYZ  (3× int32, mg)
31-42: Gyr XYZ  (3× int32, mdps)
43-118: Zero (MAG, Quaternion, Rotation, Gravity, LinAcc, Heading, HeadingErr, Elapsed)
```

## License

Copyright (c) 2026 STMicroelectronics. All rights reserved.  
See LICENSE files in subdirectories for component-specific terms.
