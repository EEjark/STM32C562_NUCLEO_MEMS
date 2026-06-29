/**
 ******************************************************************************
 * @file    mems_control.c
 * @brief   BSP adaptation layer: maps BSP_SENSOR_* to ism330dhcx part driver
 *          (ported from DataLogFusion iks02a1_mems_control.c, ACC+GYR only)
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "mems_control.h"
#include "mx_ism330dhcx.h"
#include <stddef.h>

/* Accelerometer / Gyroscope share the same ISM330DHCX instance (sensor #0).
 * The driver uses pio.id to address the bus, which is set inside
 * MX_ISM330DHCX_getobject(); we do not redefine a per-board ID here. */

/* -------- Init -------- */
void BSP_SENSOR_ACC_Init(void)
{
  /* ISM330DHCX_0 = 0x00 is the instance/bus ID used by mx_ism330dhcx_io_init().
   * ISM330DHCX_ID = 0x6B is the WHO_AM_I register address — not used for init. */
  (void)ism330dhcx_drv_init(MX_ISM330DHCX_getobject(), ISM330DHCX_0);
}

void BSP_SENSOR_GYR_Init(void)
{
  /* The driver is shared, so a second init is a no-op once acc has been inited.
   * Keep the call site symmetric with the DataLogFusion reference. */
  (void)ism330dhcx_drv_init(MX_ISM330DHCX_getobject(), ISM330DHCX_0);
}

/* -------- Enable / Disable -------- */
void BSP_SENSOR_ACC_Enable(void)
{
  (void)ism330dhcx_drv_acc_enable(MX_ISM330DHCX_getobject());
}

void BSP_SENSOR_GYR_Enable(void)
{
  (void)ism330dhcx_drv_gyro_enable(MX_ISM330DHCX_getobject());
}

void BSP_SENSOR_ACC_Disable(void)
{
  (void)ism330dhcx_drv_acc_disable(MX_ISM330DHCX_getobject());
}

void BSP_SENSOR_GYR_Disable(void)
{
  (void)ism330dhcx_drv_gyro_disable(MX_ISM330DHCX_getobject());
}

/* -------- Read axes -------- */
void BSP_SENSOR_ACC_GetAxes(MOTION_SENSOR_Axes_t *Axes)
{
  ism330dhcx_axes_t raw;
  if (Axes == NULL) return;
  if (ism330dhcx_drv_acc_get_axes(MX_ISM330DHCX_getobject(), &raw) == ISM330DHCX_OK)
  {
    Axes->x = raw.x_axis;
    Axes->y = raw.y_axis;
    Axes->z = raw.z_axis;
  }
}

void BSP_SENSOR_GYR_GetAxes(MOTION_SENSOR_Axes_t *Axes)
{
  ism330dhcx_axes_t raw;
  if (Axes == NULL) return;
  if (ism330dhcx_drv_gyro_get_axes(MX_ISM330DHCX_getobject(), &raw) == ISM330DHCX_OK)
  {
    Axes->x = raw.x_axis;
    Axes->y = raw.y_axis;
    Axes->z = raw.z_axis;
  }
}

/* -------- ODR -------- */
void BSP_SENSOR_ACC_SetOutputDataRate(float Odr)
{
  (void)ism330dhcx_drv_acc_set_outputdatarate(MX_ISM330DHCX_getobject(), (float_t)Odr);
}

void BSP_SENSOR_GYR_SetOutputDataRate(float Odr)
{
  (void)ism330dhcx_drv_gyro_set_outputdatarate(MX_ISM330DHCX_getobject(), (float_t)Odr);
}

void BSP_SENSOR_ACC_GetOutputDataRate(float *Odr)
{
  if (Odr == NULL) return;
  (void)ism330dhcx_drv_acc_get_outputdatarate(MX_ISM330DHCX_getobject(), (float_t *)Odr);
}

void BSP_SENSOR_GYR_GetOutputDataRate(float *Odr)
{
  if (Odr == NULL) return;
  (void)ism330dhcx_drv_gyro_get_outputdatarate(MX_ISM330DHCX_getobject(), (float_t *)Odr);
}

/* -------- Full scale -------- */
void BSP_SENSOR_ACC_SetFullScale(int32_t Fullscale)
{
  (void)ism330dhcx_drv_acc_set_fullscale(MX_ISM330DHCX_getobject(), (float_t)Fullscale);
}

void BSP_SENSOR_GYR_SetFullScale(int32_t Fullscale)
{
  (void)ism330dhcx_drv_gyro_set_fullscale(MX_ISM330DHCX_getobject(), (float_t)Fullscale);
}

void BSP_SENSOR_ACC_GetFullScale(int32_t *Fullscale)
{
  float_t v = 0.0f;
  if (Fullscale == NULL) return;
  if (ism330dhcx_drv_acc_get_fullscale(MX_ISM330DHCX_getobject(), &v) == ISM330DHCX_OK)
  {
    *Fullscale = (int32_t)v;
  }
}

void BSP_SENSOR_GYR_GetFullScale(int32_t *Fullscale)
{
  float_t v = 0.0f;
  if (Fullscale == NULL) return;
  if (ism330dhcx_drv_gyro_get_fullscale(MX_ISM330DHCX_getobject(), &v) == ISM330DHCX_OK)
  {
    *Fullscale = (int32_t)v;
  }
}

/* -------- WHO_AM_I readback (used by CMD_ACCELERO_GYRO_Init reply) -------- */
uint8_t BSP_SENSOR_ACC_GYR_ReadID(void)
{
  uint8_t id = 0;
  (void)ism330dhcx_drv_readid(MX_ISM330DHCX_getobject(), &id);
  return id;
}
