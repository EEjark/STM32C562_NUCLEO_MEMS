/**
 ******************************************************************************
 * @file    mems_control.h
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
#ifndef MEMS_CONTROL_H
#define MEMS_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ism330dhcx.h"

/* Axis data structure (mg for accel, mdps for gyro) */
typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} MOTION_SENSOR_Axes_t;

/* ACC + GYR only — MAG / PRESS / TEMP / HUM not present on this board */
void BSP_SENSOR_ACC_Init(void);
void BSP_SENSOR_GYR_Init(void);

void BSP_SENSOR_ACC_Enable(void);
void BSP_SENSOR_GYR_Enable(void);

void BSP_SENSOR_ACC_Disable(void);
void BSP_SENSOR_GYR_Disable(void);

void BSP_SENSOR_ACC_GetAxes(MOTION_SENSOR_Axes_t *Axes);
void BSP_SENSOR_GYR_GetAxes(MOTION_SENSOR_Axes_t *Axes);

void BSP_SENSOR_ACC_SetOutputDataRate(float Odr);
void BSP_SENSOR_GYR_SetOutputDataRate(float Odr);

void BSP_SENSOR_ACC_GetOutputDataRate(float *Odr);
void BSP_SENSOR_GYR_GetOutputDataRate(float *Odr);

void BSP_SENSOR_ACC_SetFullScale(int32_t Fullscale);
void BSP_SENSOR_GYR_SetFullScale(int32_t Fullscale);

void BSP_SENSOR_ACC_GetFullScale(int32_t *Fullscale);
void BSP_SENSOR_GYR_GetFullScale(int32_t *Fullscale);

uint8_t BSP_SENSOR_ACC_GYR_ReadID(void);

#ifdef __cplusplus
}
#endif

#endif /* MEMS_CONTROL_H */
