/**
 ******************************************************************************
 * @file    app_mems_protocol.h
 * @brief   MEMSStudio protocol handler — streaming ACC+GYR only.
 *          Frame layout matches DataLogFusion's STREAMING_MSG_LENGTH = 119.
 *          Unused fields (RTC/Press/Temp/Hum/MAG/Quat/Rot/Gravity/... ) are
 *          zero-filled so MEMSStudio parser does not reject the frame.
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
#ifndef APP_MEMS_PROTOCOL_H
#define APP_MEMS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "com.h"
#include "mems_control.h"

/* Full DataLogFusion-compatible streaming message (header + payload) */
#define STREAMING_MSG_LENGTH  119U

/* ACC+GYR offsets inside the 119-byte payload (after 3-byte header) */
#define OFF_ACC_X  19U
#define OFF_GYR_X  31U

/* Sensor enable mask — must match DataLogFusion */
#define SENS_ACC   0x01U
#define SENS_GYR   0x02U

/* Unicleo / MEMSStudio sensor ID for ISM330DHCX */
#define ISM330DHCX_UNICLEO_ID  11U

void MEMS_Init(void);
void MEMS_Process(void);
int32_t MEMS_HandleCommand(Msg_t *Msg);
void MEMS_SendStreamingFrame(MOTION_SENSOR_Axes_t *pAcc, MOTION_SENSOR_Axes_t *pGyr);
uint8_t MEMS_StreamingActive(void);
void MEMS_GetPresentationString(char *str, uint32_t *len);

#ifdef __cplusplus
}
#endif

#endif /* APP_MEMS_PROTOCOL_H */
