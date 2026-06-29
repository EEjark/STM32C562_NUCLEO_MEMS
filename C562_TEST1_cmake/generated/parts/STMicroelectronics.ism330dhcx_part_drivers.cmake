# origin-pack: generated_STMicroelectronics::ism330dhcx_part_drivers@0.5.0
# file-format: 1.0.0
project(generated_STMicroelectronics_ism330dhcx_part_drivers_0_5_0)
cmake_minimum_required(VERSION 3.20)
add_library(generated_STMicroelectronics_ism330dhcx_part_drivers_0_5_0 INTERFACE)

# List of all CMSIS properties that influence conditions for this package


# Device specific defined symbols








# Enable all components in this package
if(CMSIS_ENTIRE_generated_STMicroelectronics_ism330dhcx_part_drivers_0_5_0)
  list(APPEND CMSIS_COMPONENTS_LIST "Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:STM32CubeMX2 Config#Csub:ISM330DHCX#Cversion:0.12.0#generated:true")
endif()

# All conditions used by this package

# condition: generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0:ISM330DHCX CFG Condition
# description: STMicroelectronics configuration for ISM330DHCX part driver
set(generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_CFG_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Board Part#.*Cgroup:Sensor#.*Csub:ISM330DHCX Core(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Board Part#.*Cgroup:Sensor#.*Csub:ISM330DHCX IO#.*Cvariant:I2C Polling(#.*|$)>,>>>")
message(DEBUG "CMSIS condition generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_CFG_Condition enabled")


# condition: generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0:ISM330DHCX Core Condition
# description: STMicroelectronics ISM330DHCX Core Driver
set(generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_Core_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Board Part#.*Cgroup:Sensor#.*Csub:ISM330DHCX IO(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Board Part#.*Cgroup:Sensor#.*Csub:ISM330DHCX Reg(#.*|$)>,>>>")
message(DEBUG "CMSIS condition generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_Core_Condition enabled")


# condition: generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0:ISM330DHCX Custom IO Condition
# description: 
set(generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_Custom_IO_Condition "1")
message(DEBUG "CMSIS condition generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_Custom_IO_Condition enabled")


# condition: generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0:ISM330DHCX IO I2C Polling Condition
# description: STMicroelectronics ISM330DHCX IO I2C Polling Driver
set(generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_IO_I2C_Polling_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:I2C(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:GPIO(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:EXTI(#.*|$)>,>>>")
message(DEBUG "CMSIS condition generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_IO_I2C_Polling_Condition enabled")


# condition: generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0:ISM330DHCX IO SPI Polling Condition
# description: STMicroelectronics ISM330DHCX IO SPI Polling Driver
set(generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_IO_SPI_Polling_Condition "$<AND:$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:SPI(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:GPIO(#.*|$)>,>>,$<NOT:$<STREQUAL:$<FILTER:${CMSIS_COMPONENTS_LIST},INCLUDE,.*Cclass:Device#.*Cgroup:STM32 HAL#.*Csub:EXTI(#.*|$)>,>>>")
message(DEBUG "CMSIS condition generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_IO_SPI_Polling_Condition enabled")

# Files and components in this package
if("Cvendor:STMicroelectronics#Cclass:Board Part#Cgroup:STM32CubeMX2 Config#Csub:ISM330DHCX#Cversion:0.12.0#generated:true" IN_LIST CMSIS_COMPONENTS_LIST)  # TO BE DEFINED
  message(DEBUG "Using component generated_Board_Part_STM32CubeMX2_Config_ISM330DHCX_0_12_0")
  target_compile_definitions(generated_STMicroelectronics_ism330dhcx_part_drivers_0_5_0 INTERFACE "$<${generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_CFG_Condition}:-DCMSIS_USE_generated_Board_Part_STM32CubeMX2_Config_ISM330DHCX_0_12_0=1>")
  target_include_directories(generated_STMicroelectronics_ism330dhcx_part_drivers_0_5_0 INTERFACE "$<${generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_CFG_Condition}:${CMAKE_CURRENT_LIST_DIR}/.>")
  target_sources(generated_STMicroelectronics_ism330dhcx_part_drivers_0_5_0 INTERFACE "$<${generated_STMicroelectronics.ism330dhcx_part_drivers.0.5.0_ISM330DHCX_CFG_Condition}:${CMAKE_CURRENT_LIST_DIR}/mx_ism330dhcx.c>")
endif()

