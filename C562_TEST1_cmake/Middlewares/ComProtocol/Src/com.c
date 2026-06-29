/**
 ******************************************************************************
 * @file    com.c
 * @brief   UART communication - IDLE-line RX + polling TX
 *          RX: HAL_UART_ReceiveToIdle_IT captures raw bytes, callback stores.
 *          Com_ReceiveMsg finds frame boundary (0xF0), unstuffs, verifies chk.
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

#include "com.h"
#include "serial_cmd.h"
#include "mx_usart2.h"
#include <string.h>

#define UART_RX_BUF_SIZE  Msg_MaxLen

/* RX state */
static uint8_t  s_rx_buf[UART_RX_BUF_SIZE];
static uint16_t s_rx_len;          /* bytes captured by IDLE callback */

/**
 * @brief  Start UART reception (IDLE-line)
 */
void Com_StartReceive(void)
{
  s_rx_len = 0;
  (void)HAL_UART_ReceiveToIdle_IT(mx_usart2_uart_gethandle(),
                                 s_rx_buf, UART_RX_BUF_SIZE);
}

/**
 * @brief  Send a message (blocking). Does NOT modify Msg->Data[0..1].
 *         Caller must set Dest/Src before calling.
 */
void Com_SendMsg(Msg_t *Msg)
{
  static uint8_t tx_buf[UART_TX_BUFFER_SIZE];

  CHK_ComputeAndAdd(Msg);                              /* append checksum */
  int32_t len = ByteStuffCopy(tx_buf, Msg);            /* stuff + add EOF */
  (void)HAL_UART_Transmit(mx_usart2_uart_gethandle(),
                          tx_buf, (uint32_t)len, HAL_MAX_DELAY);
}

/**
 * @brief  Check for a complete message in RX buffer.
 *         Scans for Msg_EOF, unstuffs, verifies checksum.
 * @retval 1 = valid message in Msg, 0 = no complete frame
 */
int32_t Com_ReceiveMsg(Msg_t *Msg)
{
  if (s_rx_len == 0) return 0;

  /* Scan for Msg_EOF in raw buffer */
  uint16_t i;
  int     found = 0;
  for (i = 0; i < s_rx_len; i++)
  {
    if (s_rx_buf[i] == (uint8_t)Msg_EOF)
    {
      found = 1;
      break;
    }
  }
  if (!found) return 0;

  /* The raw bytes from index 0 to EOF index are byte-stuffed.
   * Use ReverseByteStuffCopy to unstuff into Msg->Data.
   * The function stops at Msg_EOF and does NOT include it in the output. */
  if (!ReverseByteStuffCopy(Msg, s_rx_buf))
  {
    /* Invalid escape sequence - discard and restart */
    s_rx_len = 0;
    Com_StartReceive();
    return 0;
  }

  /* Restart reception for next frame */
  s_rx_len = 0;
  Com_StartReceive();

  /* Now verify checksum on the unstuffed data */
  return CHK_CheckAndRemove(Msg) ? 1 : 0;
}

/**
 * @brief  UART RX callback - fires on IDLE line
 */
void HAL_UART_RxCpltCallback(hal_uart_handle_t *huart, uint32_t Size,
                              hal_uart_rx_event_types_t rx_event)
{
  (void)huart;
  (void)rx_event;
  if (Size > 0 && Size < UART_RX_BUF_SIZE)
  {
    /* Store raw bytes; Msg_EOF may be inside if frame is complete */
    s_rx_len = (uint16_t)Size;
  }
}
