# C562_TEST1 工程总览

> 基于 STM32CubeMX2 + CMSIS-Pack 的 NUCLEO-C562RE 演示工程。
> 通过 UART 协议与上位机 **MEMSStudio** 通信，上报 6 轴 (ACC+GYR) 传感器数据。

## 1. 硬件目标

| 项目     | 值                                          |
|----------|---------------------------------------------|
| MCU      | STM32C562RET6 (Cortex-M33, FPU SP, DSP, MPU) |
| 板子     | NUCLEO-C562RE                               |
| 系统时钟 | 144 MHz（HSI 16 MHz × PLL 9，默认系统配置）  |
| ROM      | 0x0800_0000 — 0x0807_FFFF (512 KB Flash)    |
| RAM      | 0x2000_0000 — 0x2001_FFFF (128 KB SRAM)     |
| 堆/栈    | HEAP=512 B, STACK=1024 B（链接脚本定义）    |
| 调试     | ST-Link VCP (USART2) 用于 MEMSStudio 通信    |

外设分配（CubeMX 生成）：
- **USART2** — 115200 bps，TX=PA2 / RX=PA3，连 ST-Link VCP
- **I2C1** — 接板载 ISM330DHCX (地址 `ISM330DHCX_I2C_ADD_H` = 0x6B)
- **TIM2** — 时基
- **TIM17** — 可选 PWM
- **GPIO** — 板载 LED/Button

---

## 2. 软件分层

```
┌──────────────────────────────────────────────────────────────────────┐
│  main.c                                  [应用入口]                  │
│   · 100Hz 轮询                                                              │
│   · 系统初始化 (mx_system_init)                                              │
├──────────────────────────────────────────────────────────────────────┤
│  App/                                                               │
│    app_mems_protocol.{c,h}            [协议业务] 命令分发 + 流帧构建  │
│    mems_control.{c,h}                 [BSP 适配] ism330dhcx → 统一 API │
├──────────────────────────────────────────────────────────────────────┤
│  Middlewares/ComProtocol/                                            │
│    Src/com.c                          [链路层] UART IDLE RX + 帧缓存  │
│    Src/serial_protocol.c              [编解码] 字节填充 + 校验和      │
│    Inc/com.h, serial_protocol.h, serial_cmd.h                        │
├──────────────────────────────────────────────────────────────────────┤
│  part_drivers/                                                       │
│    ism330dhcx/{ism330dhcx.c, regs/ism330dhcx_reg.c,                 │
│                interfaces/i2c/ism330dhcx_io.c}  [IMU 驱动]            │
│    button/gpio/button.c               [按键驱动]                      │
│    led/{gpio, pwm}/led_*.c            [LED 驱动]                      │
├──────────────────────────────────────────────────────────────────────┤
│  stm32c5xx_drivers/{hal, ll, timebases, os_port, utils}  [BSP HAL/LL]│
│  stm32c5xx_dfp/{Include, Source}                  [Cortex-M33 DFP]   │
│  arch/cmsis/                                       [CMSIS 6.2 Core]  │
│  generated/hal/ + generated/parts/ + generated/utilities/  [CubeMX 产出]│
│  utilities/syscalls/                              [newlib stubs]      │
│  user_modifiable/Device/STM32C562RET6/            [启动/链接/系统]    │
└──────────────────────────────────────────────────────────────────────┘
```

---

## 3. 文件清单（应用层）

### 3.1 根目录（用户源）

| 文件                      | 行数 | 作用                                                    |
|---------------------------|------|---------------------------------------------------------|
| `main.c`                  | ~50  | 系统初始化 → 传感器 init/enable → 100Hz 轮询            |
| `main.h`                  | ~40  | 引入 `mx_hal_def.h` / `mx_system.h`                     |
| `CMakeLists.txt`          | 44   | 项目根，构建入口                                        |
| `CMakePresets.json`       | 30   | Ninja + `debug_GCC_NUCLEO-C562RE` preset                |
| `memsstudio.md`           | —    | DataLogFusion ↔ MEMSStudio 协议详细分析（**已撰写**）  |
| `PROJECT_OVERVIEW.md`     | —    | 本文件                                                  |

### 3.2 App/

| 文件                    | 作用                                                            |
|-------------------------|-----------------------------------------------------------------|
| `app_mems_protocol.h`   | API 声明 + 流帧布局（29 字节，含 payload/header 常量）+ SENS_* |
| `app_mems_protocol.c`   | `MEMS_HandleCommand` / `MEMS_SendStreamingFrame` / `MEMS_GetPresentationString` |
| `mems_control.h`        | `MOTION_SENSOR_Axes_t` + `BSP_SENSOR_ACC/GYR_*` 声明            |
| `mems_control.c`        | 包装 `ism330dhcx_drv_*`（Init/Enable/Disable/Get/ODR/FS/ID）    |
| `CMakeLists.txt`        | App 子构建（target_sources + include）                          |

### 3.3 Middlewares/ComProtocol/

| 文件                    | 作用                                                            |
|-------------------------|-----------------------------------------------------------------|
| `com.h`                 | `Com_StartReceive/ReceiveMsg/SendMsg` + `UART_TX_BUFFER_SIZE`  |
| `com.c`                 | UART IDLE-line RX + 帧缓存 + 阻塞发送                          |
| `serial_protocol.h`     | `Msg_t` / `Msg_EOF/BS/BS_EOF` / API 声明                        |
| `serial_protocol.c`     | ByteStuff + CHK + Serialize_u32/s32 + FloatToArray              |
| `serial_cmd.h`          | CMD 码（Ping/PresString/Start-Stop/Get_App_Info/ACC_GYRO_Init）+ DEV_ADDR + Reply_Add |
| `CMakeLists.txt`        | Middleware 子构建                                                |

---

## 4. 通信协议（要点）

帧格式：`[Dest(1)][Src(1)][CMD(1)][Payload(N)][CHK(1)][0xF0(1)]`

字节填充：`0xF0 → 0xF1 0xF2`，`0xF1 → 0xF1 0xF1`

校验和：发送端 `chk -= Data[i]` 累加；接收端 `Σ Data[i] == 0` 校验

支持的命令：
| CMD                       | 值    | 行为                                       |
|---------------------------|-------|--------------------------------------------|
| `CMD_Ping`                | 0x01  | 回 Ping                                    |
| `CMD_Read_PresString`     | 0x02  | 设备描述字符串                             |
| `CMD_Start_Data_Streaming`| 0x08  | 启动流                                    |
| `CMD_Stop_Data_Streaming` | 0x09  | 停止流                                    |
| `CMD_Get_App_Info`        | 0x12  | AlgoFreq(4) + RequiredData(1)              |
| `CMD_ACCELERO_GYRO_Init`  | 0x76  | 4 字节 sensor ID                           |

数据流 payload (24 字节)：
```
[ACC_X int32 mg][ACC_Y][ACC_Z][GYR_X int32 mdps][GYR_Y][GYR_Z]
```

---

## 5. 构建系统

### 5.1 工具链

- CMake ≥ 3.20
- Ninja（preset 指定）
- arm-none-eabi-gcc 14.3.1 + newlib-nano
- 链接器: `--specs=nano.specs --specs=nosys.specs`

### 5.2 关键宏/编译选项

| 宏              | 作用                                          |
|-----------------|-----------------------------------------------|
| `STM32C562xx`   | MCU 型号                                      |
| `_RTE_`         | 启用 RTE 组件包含                              |
| `CMSIS_USE_*`   | 启用各 CMSIS-Pack 组件（按 components.cmake 列表） |
| `-mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard` | CPU+FPU 配置 |
| `-O0 -g3`       | Debug 构建（preset 名带 debug_）              |
| `--gc-sections` | 删除未引用段                                  |

### 5.3 CMake 模块

| 模块                        | 作用                                                            |
|-----------------------------|-----------------------------------------------------------------|
| `cmake/target.cmake`        | 设置 `CMSIS_D*` 设备属性                                        |
| `cmake/GCC.14.3.1.cmake`    | 工具链文件：定义 `CPU_FLAGS` 等                                |
| `cmake/flags.cmake`         | include path / 编译选项 / 链接选项                             |
| `cmake/files.cmake`         | root 源文件 + `add_subdirectory(App/ComProtocol)`               |
| `cmake/components.cmake`    | 列出所有启用的 CMSIS-Pack 组件（55 项）+ link 列表             |

### 5.4 完整构建步骤

```bash
# 配置
cmake --preset debug_GCC_NUCLEO-C562RE

# 构建
cmake --build --preset debug_GCC_NUCLEO-C562RE

# 产物
# build/debug_GCC_NUCLEO-C562RE/C562_TEST1.elf
# build/debug_GCC_NUCLEO-C562RE/C562_TEST1.map
# build/debug_GCC_NUCLEO-C562RE/compile_commands.json  (clangd 用)
```

---

## 6. 启动与运行流程

```
Reset_Handler (startup_stm32c562xx.c)
  → SystemInit (system_stm32c5xx.c, 配置 FPU/中断向量)
  → __libc_init_array
  → main()
      mx_system_init()        // 时钟 / ICache / USART2 / I2C1 / GPIO
      BSP_SENSOR_ACC_Init()   // mems_control.c → ism330dhcx_drv_init
      BSP_SENSOR_GYR_Init()
      BSP_SENSOR_ACC_Enable()
      BSP_SENSOR_GYR_Enable()
      BSP_SENSOR_ACC_SetOutputDataRate(104.0f)
      BSP_SENSOR_GYR_SetOutputDataRate(104.0f)
      BSP_SENSOR_ACC_SetFullScale(4)        // ±4g
      BSP_SENSOR_GYR_SetFullScale(2000)     // ±2000dps
      Com_StartReceive()                    // HAL_UART_ReceiveToIdle_IT
      for (;;) {
        if (Com_ReceiveMsg(&msg))
          MEMS_HandleCommand(&msg);
        if (now - last >= 10ms) {           // 100Hz
          BSP_SENSOR_*_GetAxes(...);
          MEMS_SendStreamingFrame(&acc, &gyr);  // → Com_SendMsg → UART
        }
      }
```

---

## 7. 关键约束与已修复问题

1. **HAL UART 回调签名变化**
   `HAL_UART_RxCpltCallback` 现在是 3 参 `(huart, size, rx_event_types_t)`，
   com.c 同步更新（com.c:89）。

2. **不要重定义 `ISM330DHCX_ID`**
   ism330dhcx_reg.h 已经定义它为 `0x6B`（WHO_AM_I）。
   mems_control.c 改为不传 ID 参数（`MX_ISM330DHCX_getobject()` 内已配置 pio.id）。

3. **`UART_TX_BUFFER_SIZE` 必须在 com.h 中定义**
   字节填充会把 Msg 翻倍，所以 `2 * Msg_MaxLen` 是安全的。

4. **App/ 和 Middlewares/ComProtocol/ 的 CMakeLists 各自独立**
   通过 `cmake/files.cmake` 中的 `add_subdirectory` 接入。

---

## 8. 目录树（深度≤3，省略中间文件）

```
C562_TEST1_cmake/
├── App/                          # 用户应用层
│   ├── app_mems_protocol.{c,h}   # 协议业务
│   ├── mems_control.{c,h}        # BSP 适配
│   └── CMakeLists.txt
├── Middlewares/
│   └── ComProtocol/              # UART 帧协议
│       ├── Inc/{com,serial_protocol,serial_cmd}.h
│       ├── Src/{com,serial_protocol}.c
│       └── CMakeLists.txt
├── arch/cmsis/                   # CMSIS 6.2 Core
├── generated/
│   ├── hal/                      # CubeMX 时钟/外设初始化 (USART2, I2C1, TIM, RCC…)
│   ├── parts/                    # CubeMX 板级对象 (button, led, ism330dhcx)
│   └── utilities/                # CubeMX syscalls
├── part_drivers/
│   ├── button/gpio/              # 板载按键
│   ├── ism330dhcx/               # 6 轴 IMU (核心)
│   │   ├── ism330dhcx.{c,h}
│   │   ├── regs/ism330dhcx_reg.{c,h}
│   │   └── interfaces/i2c/ism330dhcx_io.{c,h}
│   └── led/{gpio, pwm}/          # 板载 LED
├── stm32c5xx_dfp/                # CMSIS-Pack: STM32C5 系列 DFP
│   ├── Include/                  # CMSIS 头
│   └── Source/Templates/         # startup / system 模板
├── stm32c5xx_drivers/            # ST HAL + LL 驱动
│   ├── hal/                      # HAL: uart, i2c, gpio, tim…
│   ├── ll/                       # LL: usart, i2c, rcc…
│   ├── timebases/                # HAL timebase
│   ├── os_port/ utils/ templates/
├── user_modifiable/
│   └── Device/STM32C562RET6/     # startup + system + linker (用户可改)
├── utilities/syscalls/           # newlib stubs (read/write/sbrk)
├── cmake/                        # CMake 模块
├── build/debug_GCC_NUCLEO-C562RE/  # 构建产物
├── main.{c,h}
├── CMakeLists.txt
├── CMakePresets.json
├── memsstudio.md                 # 协议分析
└── PROJECT_OVERVIEW.md           # 本文件
```

---

## 9. 当前状态

- ✅ Build 通过（[31/31] Linking → C562_TEST1.elf）
- ✅ MEMSStudio 命令处理：Ping / PresString / Start-Stop / Get_App_Info / ACC_GYRO_Init
- ✅ 数据流：ACC+GYR，100Hz，FS=±4g / ±2000dps，ODR=104Hz
- ✅ 代码分层清晰：BSP 适配 / 协议业务 / 链路 / 编解码 / 驱动
- ⚠️ 未引入 MotionFX 融合算法（用户要求保持精简）
- ⚠️ 主循环无 RTOS — 100Hz 轮询，命令处理在同一循环
- ⚠️ 串口发送为阻塞（`HAL_UART_Transmit ... HAL_MAX_DELAY`），可在 100Hz 下可承受
- ⚠️ 未做硬实时保证；如有更高要求应改 RTOS 或 TIM+DMA

---

## 10. 后续可拓展点

1. **MotionFX 融合层** — 引入 `motion_fx_manager.{c,h}`，扩展流帧至 119 字节（含 Quaternion/Rotation/Gravity/LinAcc）
2. **RTOS** — FreeRTOS 集成（CMSIS-Pack 中已有），把传感器读取/发送放入独立任务
3. **DMA TX** — 把 `Com_SendMsg` 改为 DMA 非阻塞
4. **多传感器** — 在 mems_control.c 中加入 MAG/PRESS/TEMP/HUM 适配层
5. **离线回放** — 实现 `CMD_Offline_Data` (0x10) / `CMD_Use_Offline_Data` (0x11)
6. **NACK / Reset / DFU** — 实现其余命令
