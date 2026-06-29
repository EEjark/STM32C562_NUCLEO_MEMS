/**
 ******************************************************************************
 * @file    serial_protocol.h
 * @brief   Serial protocol: byte stuffing, checksum, serialization
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

#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Framing control characters */
#define Msg_EOF    0xF0
#define Msg_BS     0xF1
#define Msg_BS_EOF 0xF2

#define Msg_MaxLen 256

/**
 * @brief Serial message structure
 */
typedef struct
{
  uint32_t Len;
  uint8_t Data[Msg_MaxLen];
} Msg_t;

/**
 * @brief Byte stuffing: copy one byte with escaping
 * @retval Number of output bytes (1 or 2)
 */
int32_t ByteStuffCopyByte(uint8_t *Dest, uint8_t Source);

/**
 * @brief Byte stuffing: copy a message
 * @retval Total output byte count including trailing Msg_EOF
 */
int32_t ByteStuffCopy(uint8_t *Dest, Msg_t *Source);

/**
 * @brief Reverse byte unstuffing for two input bytes
 * @retval 2 if valid escape, 1 if normal byte, 0 if invalid
 */
int32_t ReverseByteStuffCopyByte2(uint8_t Source0, uint8_t Source1, uint8_t *Dest);

/**
 * @brief Reverse byte unstuffing for a message (reads up to Msg_EOF)
 * @retval 1 if success, 0 if error
 */
int32_t ReverseByteStuffCopy(Msg_t *Dest, uint8_t *Source);

/**
 * @brief Compute and append checksum (two's complement sum, result + data = 0)
 */
void CHK_ComputeAndAdd(Msg_t *Msg);

/**
 * @brief Verify and remove checksum
 * @retval 1 if OK, 0 if failed
 */
int32_t CHK_CheckAndRemove(Msg_t *Msg);

/**
 * @brief Serialize uint32_t (LSB first)
 */
void Serialize_u32(uint8_t *Dest, uint32_t Source, uint32_t Len);

/**
 * @brief Serialize int32_t (LSB first)
 */
void Serialize_s32(uint8_t *Dest, int32_t Source, uint32_t Len);

/**
 * @brief Deserialize uint32_t (LSB first)
 */
uint32_t Deserialize_u32(uint8_t *Source, uint32_t Len);

/**
 * @brief Serialize float (memcpy)
 */
void FloatToArray(uint8_t *Dest, float Data);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_PROTOCOL_H */
