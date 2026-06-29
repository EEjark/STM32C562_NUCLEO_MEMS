/**
 ******************************************************************************
 * @file    com.h
 * @brief   UART communication interface
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

#ifndef COM_H
#define COM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "serial_protocol.h"

/* TX buffer sized to Msg_MaxLen + headroom for byte-stuffing expansion */
#define UART_TX_BUFFER_SIZE  (2U * Msg_MaxLen)

/**
 * @brief  Start UART reception (IDLE-line detection mode)
 */
void Com_StartReceive(void);

/**
 * @brief  Check if a complete message has been received
 * @retval 1 = message ready in Msg (checksum verified/stripped), 0 = no frame
 */
int32_t Com_ReceiveMsg(Msg_t *Msg);

/**
 * @brief  Send a framed message (blocking)
 * @note  Adds header, checksum, byte-stuffing, EOF internally
 */
void Com_SendMsg(Msg_t *Msg);

#ifdef __cplusplus
}
#endif

#endif /* COM_H */
