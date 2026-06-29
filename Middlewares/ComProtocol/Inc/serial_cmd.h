/**
 ******************************************************************************
 * @file    serial_cmd.h
 * @brief   Serial command codes for MEMSStudio communication
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

#ifndef SERIAL_CMD_H
#define SERIAL_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Generic CMD (0x00 - 0x0F) */
#define CMD_Ping                  0x01
#define CMD_Read_PresString      0x02
#define CMD_Start_Data_Streaming 0x08
#define CMD_Stop_Data_Streaming   0x09
#define CMD_Get_App_Info         0x12
#define CMD_ChangeSF             0x07    /* 6X / 9X fusion switch */

/* Sensor CMD */
#define CMD_ACCELERO_GYRO_Init   0x76
#define CMD_MAGNETO_Init        0x7A

/* Reply flag: CMD | Reply_Add = response CMD */
#define CMD_Reply_Add           0x80U

/* Board address (this device) */
#ifndef DEV_ADDR
#define DEV_ADDR                50U
#endif

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_CMD_H */
