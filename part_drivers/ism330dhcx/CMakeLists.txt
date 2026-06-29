# origin-pack: STMicroelectronics::ism330dhcx_part_drivers@1.0.0
# file-format: 1.0.0
project(STMicroelectronics_ism330dhcx_part_drivers_1_0_0)
cmake_minimum_required(VERSION 3.20)
add_library(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE)

# List of all CMSIS properties that influence conditions for this package


# Device specific defined symbols








# Enable all components in this package
if(CMSIS_ENTIRE_STMicroelectronics_ism330dhcx_part_drivers_1_0_0)
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX Reg#Cversion:2.0.1")
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX Core#Cversion:0.10.0")
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX IO#Cvariant:I2C Polling#Cversion:0.2.0")
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX IO#Cvariant:SPI Polling#Cversion:0.2.0")
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX IO#Cvariant:Custom#Cversion:0.2.0")
endif()

# All conditions used by this package

# condition: STMicroelectronics.ism330dhcx_part_drivers.1.0.0:ISM330DHCX CFG Condition
# description: STMicroelectronics configuration for ISM330DHCX part driver
set(STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_CFG_Condition "1")
message(DEBUG "CMSIS condition STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_CFG_Condition enabled")


# condition: STMicroelectronics.ism330dhcx_part_drivers.1.0.0:ISM330DHCX Core Condition
# description: STMicroelectronics ISM330DHCX Core Driver
set(STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Core_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Board Part#.*Cgroup:Sensor#.*Csub:ISM330DHCX IO(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Board Part#.*Cgroup:Sensor#.*Csub:ISM330DHCX Reg(#.*|$)>,>>>")
message(DEBUG "CMSIS condition STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Core_Condition enabled")


# condition: STMicroelectronics.ism330dhcx_part_drivers.1.0.0:ISM330DHCX Custom IO Condition
# description: 
set(STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Custom_IO_Condition "1")
message(DEBUG "CMSIS condition STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Custom_IO_Condition enabled")


# condition: STMicroelectronics.ism330dhcx_part_drivers.1.0.0:ISM330DHCX IO I2C Polling Condition
# description: STMicroelectronics ISM330DHCX IO I2C Polling Driver
set(STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_I2C_Polling_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:I2C(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:GPIO(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:EXTI(#.*|$)>,>>>")
message(DEBUG "CMSIS condition STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_I2C_Polling_Condition enabled")


# condition: STMicroelectronics.ism330dhcx_part_drivers.1.0.0:ISM330DHCX IO SPI Polling Condition
# description: STMicroelectronics ISM330DHCX IO SPI Polling Driver
set(STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_SPI_Polling_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:SPI(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:GPIO(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:EXTI(#.*|$)>,>>>")
message(DEBUG "CMSIS condition STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_SPI_Polling_Condition enabled")

# Files and components in this package
if("Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX Reg#Cversion:2.0.1" IN_LIST CMSIS_COMPONENTS_LIST)  # https://github.com/STMicroelectronics/ism330dhcx-pid/releases/tag/v2.0.1
  message(DEBUG "Using component Board_Part_Sensor_ISM330DHCX_Reg_2_0_1")
  target_compile_definitions(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "-DCMSIS_USE_Board_Part_Sensor_ISM330DHCX_Reg_2_0_1=1")
  target_include_directories(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/regs")
  target_sources(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/regs/ism330dhcx_reg.c")
endif()

if("Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX Core#Cversion:0.10.0" IN_LIST CMSIS_COMPONENTS_LIST)  # Driver for ISM330DHCX MEMS sensor
  message(DEBUG "Using component Board_Part_Sensor_ISM330DHCX_Core_0_10_0")
  target_compile_definitions(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Core_Condition}:-DCMSIS_USE_Board_Part_Sensor_ISM330DHCX_Core_0_10_0=1>")
  target_include_directories(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Core_Condition}:${CMAKE_CURRENT_LIST_DIR}/.>")
  target_sources(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Core_Condition}:${CMAKE_CURRENT_LIST_DIR}/ism330dhcx.c>")
endif()

if("Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX IO#Cvariant:I2C Polling#Cversion:0.2.0" IN_LIST CMSIS_COMPONENTS_LIST)  # I2C IO in polling mode for ISM330DHCX MEMS sensor
  message(DEBUG "Using component Board_Part_Sensor_ISM330DHCX_IO_I2C_Polling_0_2_0")
  target_compile_definitions(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_I2C_Polling_Condition}:-DCMSIS_USE_Board_Part_Sensor_ISM330DHCX_IO_I2C_Polling_0_2_0=1>")
  target_include_directories(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_I2C_Polling_Condition}:${CMAKE_CURRENT_LIST_DIR}/interfaces/i2c>")
  target_sources(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_I2C_Polling_Condition}:${CMAKE_CURRENT_LIST_DIR}/interfaces/i2c/ism330dhcx_io.c>")
endif()

if("Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX IO#Cvariant:SPI Polling#Cversion:0.2.0" IN_LIST CMSIS_COMPONENTS_LIST)  # SPI IO in polling mode for ISM330DHCX MEMS sensor
  message(DEBUG "Using component Board_Part_Sensor_ISM330DHCX_IO_SPI_Polling_0_2_0")
  target_compile_definitions(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_SPI_Polling_Condition}:-DCMSIS_USE_Board_Part_Sensor_ISM330DHCX_IO_SPI_Polling_0_2_0=1>")
  target_include_directories(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_SPI_Polling_Condition}:${CMAKE_CURRENT_LIST_DIR}/interfaces/spi>")
  target_sources(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_IO_SPI_Polling_Condition}:${CMAKE_CURRENT_LIST_DIR}/interfaces/spi/ism330dhcx_io.c>")
endif()

if("Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:Sensor#Csub:ISM330DHCX IO#Cvariant:Custom#Cversion:0.2.0" IN_LIST CMSIS_COMPONENTS_LIST)  # Custom I/O layer for ISM330DHCX IO
  message(DEBUG "Using component Board_Part_Sensor_ISM330DHCX_IO_Custom_0_2_0")
  target_compile_definitions(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Custom_IO_Condition}:-DCMSIS_USE_Board_Part_Sensor_ISM330DHCX_IO_Custom_0_2_0=1>")
  target_include_directories(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Custom_IO_Condition}:${CMAKE_CURRENT_LIST_DIR}/interfaces/custom>")
  if(EXISTS ${CMSIS_RTE_FOLDER}/Board Part/ism330dhcx_io.c)
    target_sources(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Custom_IO_Condition}:${CMSIS_RTE_FOLDER}/Board Part/ism330dhcx_io.c>")
  else()
    target_sources(STMicroelectronics_ism330dhcx_part_drivers_1_0_0 INTERFACE "$<${STMicroelectronics.ism330dhcx_part_drivers.1.0.0_ISM330DHCX_Custom_IO_Condition}:${CMAKE_CURRENT_LIST_DIR}/interfaces/custom/ism330dhcx_io.c>")
  endif()
endif()

