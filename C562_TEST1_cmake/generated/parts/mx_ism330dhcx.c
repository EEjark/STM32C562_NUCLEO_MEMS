/**
  ******************************************************************************
  * file           : mx_ism330dhcx.c
  * brief          : Code generation for the ISM330DHCX part driver.
  ******************************************************************************
  * attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the mx_ism330dhcx_license.md file
  * in the same directory as the generated code.
  * If no mx_ism330dhcx_license.md file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mx_ism330dhcx.h"

static ism330dhcx_object_t ism330dhcx_0_obj;
int32_t ism330dhcx_io_init(ism330dhcx_io_t *pio)
{
  switch (pio->id)
  {

  case ISM330DHCX_0:
    pio->hi2c = ISM330DHCX_0_I2C_GETHANDLE();
    pio->addr = ISM330DHCX_0_I2C_ADDRESS;
    pio->hexti_int1 = NULL;
    pio->hexti_int2 = NULL;
    break;
  default:
    break;
  }
  return 0;
}

ism330dhcx_object_t *mx_ism330dhcx_0_getobject(void)
{
  return &ism330dhcx_0_obj;
}
