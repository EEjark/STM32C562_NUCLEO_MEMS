/**
  ******************************************************************************
  * file           : mx_ism330dhcx.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX_ISM330DHCX_H
#define MX_ISM330DHCX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "ism330dhcx.h"
#include "mx_hal_def.h"

/* Exported macros --------------------------------------------------------- */


/* Part Config ID */
#define ISM330DHCX_0                              0 /* Main label */
#define MX_ISM330DHCX                                  0 /* Extra label */

/* alias for the getter function based on the extra label */
#define MX_ISM330DHCX_getobject() mx_ism330dhcx_0_getobject()

#define ISM330DHCX_0_I2C_GETHANDLE mx_i2c1_i2c_gethandle
#define ISM330DHCX_0_I2C_ADDRESS ISM330DHCX_I2C_ADD_H
ism330dhcx_object_t *mx_ism330dhcx_0_getobject(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MX_ISM330DHCX_H */
