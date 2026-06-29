/**
 ******************************************************************************
 * @file    app_mems_protocol.c
 * @brief   MEMSStudio protocol — command dispatch + 119-byte streaming frame.
 *          Ported from DataLogFusion demo_serial.c + app_mems.c, ACC+GYR only.
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
#include "app_mems_protocol.h"
#include "serial_cmd.h"
#include <stdio.h>
#include <string.h>

extern uint32_t HAL_GetTick(void);

/* Streaming state — toggled by CMD_Start/Stop_Data_Streaming */
static volatile uint8_t s_streaming_active;

/* ------------------------------------------------------------------ */
/*  Main loop                                                         */
/* ------------------------------------------------------------------ */
void MEMS_Process(void)
{
  static Msg_t  msg;
  static uint32_t last;

  uint32_t now = HAL_GetTick();

  /* Command dispatch */
  if (Com_ReceiveMsg(&msg))
    MEMS_HandleCommand(&msg);

  /* 100 Hz sensor polling + streaming — only when MEMSStudio commanded start */
  if (s_streaming_active && (now - last >= 10U))
  {
    last = now;

    MOTION_SENSOR_Axes_t acc, gyr;
    BSP_SENSOR_ACC_GetAxes(&acc);
    BSP_SENSOR_GYR_GetAxes(&gyr);

    MEMS_SendStreamingFrame(&acc, &gyr);
  }
}

/* ------------------------------------------------------------------ */
/*  Init — sensors + UART, do NOT auto-start streaming                */
/* ------------------------------------------------------------------ */
void MEMS_Init(void)
{
  /* Sensors */
  BSP_SENSOR_ACC_Init();
  BSP_SENSOR_GYR_Init();
  BSP_SENSOR_ACC_Enable();
  BSP_SENSOR_GYR_Enable();

  BSP_SENSOR_ACC_SetOutputDataRate(104.0f);
  BSP_SENSOR_ACC_SetFullScale(4);
  BSP_SENSOR_GYR_SetOutputDataRate(104.0f);
  BSP_SENSOR_GYR_SetFullScale(2000);

  /* UART RX */
  Com_StartReceive();

  s_streaming_active = 0U;
}

/* ------------------------------------------------------------------ */
/*  Command handlers                                                  */
/* ------------------------------------------------------------------ */

/** Helper: build and send a directed reply */
static void reply(Msg_t *in, uint8_t rcmd,
                  const uint8_t *payload, uint32_t plen)
{
  Msg_t out;
  out.Data[0] = in->Data[1];              /* Dest = original sender        */
  out.Data[1] = DEV_ADDR;                 /* Src  = this device            */
  out.Data[2] = (uint8_t)(rcmd | CMD_Reply_Add);
  if (payload && plen > 0U)
    memcpy(&out.Data[3], payload, plen);
  out.Len = 3U + plen;
  Com_SendMsg(&out);
}

int32_t MEMS_HandleCommand(Msg_t *Msg)
{
  if (Msg->Len < 3U) return 0;
  if (Msg->Data[0] != DEV_ADDR) return 0;

  const uint8_t cmd = Msg->Data[2];
  uint8_t buf[64];

  switch (cmd)
  {
    case CMD_Ping:
      reply(Msg, CMD_Ping, NULL, 0U);
      break;

    case CMD_Read_PresString:
    {
      uint32_t len;
      MEMS_GetPresentationString((char *)buf, &len);
      reply(Msg, CMD_Read_PresString, buf, len);
      break;
    }

    case CMD_Start_Data_Streaming:
      s_streaming_active = 1U;
      reply(Msg, CMD_Start_Data_Streaming, NULL, 0U);
      break;

    case CMD_Stop_Data_Streaming:
      s_streaming_active = 0U;
      reply(Msg, CMD_Stop_Data_Streaming, NULL, 0U);
      break;

    case CMD_Get_App_Info:
      /* AlgoFreq(4 bytes) + RequiredData(1 byte) */
      Serialize_u32(&buf[0], 100U, 4U);
      buf[4] = (uint8_t)(SENS_ACC | SENS_GYR);
      reply(Msg, CMD_Get_App_Info, buf, 5U);
      break;

    case CMD_ACCELERO_GYRO_Init:
      Serialize_u32(&buf[0], ISM330DHCX_UNICLEO_ID, 4U);
      reply(Msg, CMD_ACCELERO_GYRO_Init, buf, 4U);
      break;

    default:
      return 0;
  }
  return 1;
}

/* ------------------------------------------------------------------ */
/*  Streaming frame builder (119-byte payload, matching DataLogFusion) */
/* ------------------------------------------------------------------ */
void MEMS_SendStreamingFrame(MOTION_SENSOR_Axes_t *pAcc,
                              MOTION_SENSOR_Axes_t *pGyr)
{
  if (!s_streaming_active) return;
  if (!pAcc || !pGyr) return;

  /* Build full 119-byte payload, zero-initialised */
  static uint8_t raw[STREAMING_MSG_LENGTH];
  memset(raw, 0, sizeof(raw));

  /* Header */
  raw[0] = 0x00;        /* Dest = broadcast                           */
  raw[1] = DEV_ADDR;    /* Src  = this device                          */
  raw[2] = CMD_Start_Data_Streaming;

  /* ACC  (offsets 19–30, int32 LSB-first, unit mg) */
  Serialize_s32(&raw[OFF_ACC_X + 0], pAcc->x, 4U);
  Serialize_s32(&raw[OFF_ACC_X + 4], pAcc->y, 4U);
  Serialize_s32(&raw[OFF_ACC_X + 8], pAcc->z, 4U);

  /* GYR  (offsets 31–42, int32 LSB-first, unit mdps) */
  Serialize_s32(&raw[OFF_GYR_X + 0], pGyr->x, 4U);
  Serialize_s32(&raw[OFF_GYR_X + 4], pGyr->y, 4U);
  Serialize_s32(&raw[OFF_GYR_X + 8], pGyr->z, 4U);

  /* Wrap into Msg_t and send (Com_SendMsg adds CHK + stuffing + EOF) */
  Msg_t msg;
  msg.Len = STREAMING_MSG_LENGTH;
  memcpy(msg.Data, raw, STREAMING_MSG_LENGTH);
  Com_SendMsg(&msg);
}

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */
uint8_t MEMS_StreamingActive(void) { return s_streaming_active; }

void MEMS_GetPresentationString(char *str, uint32_t *len)
{
  *len = (uint32_t)snprintf(str, 64,
            "MEMS shield demo,4,1.0.0,ISM330DHCX 1.0.0,NUCLEO-C562RE");
}
