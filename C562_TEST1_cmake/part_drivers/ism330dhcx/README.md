# __Board Parts: ISM330DHCX__

## __Summary__

Part ISM330DHCX provides APIs to drive the ISM330DHCX component through I²C or SPI.

## __Description__

### Peripherals initialization:

Part ISM330DHCX assumes that the initialization of all needed peripherals (I²C, SPI, GPIO) is done by the main application.
After the MCU IPs have been initialized, the selected resources can be assigned to ISM330DHCX objects. This is done in `ism330dhcx_io_init()`, which the user application must override.
That function is called by `ism330dhcx_drv_init()`; the `.id` field of the `ism330dhcx_io_t` structure should be used to discriminate the resources to be assigned.

## __How to use it?__

To use the ISM330DHCX part API, the application should define the `ism330dhcx_object_t` object and the `ism330dhcx_io_init()` function to assign hardware resources to it.
The link between the two is made by `ism330dhcx_drv_init()`, which should be called after all the MCU peripherals have been initialized.

Once this is successfully done, all the other part APIs can be used freely.

## __Contributing__

STM32 customers and users who want to contribute to this component can follow instructions on the [STMicroelectronics GitHub page]( https://github.com/STMicroelectronics)

## __Keywords__
- STM32
- BSP
- Parts
- ISM330DHCX

## __License__

Copyright (c) 2026 STMicroelectronics.

This software is licensed under terms that can be found in the LICENSE file in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
