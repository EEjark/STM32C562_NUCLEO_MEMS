/**
 ******************************************************************************
 * @file    serial_protocol.c
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

#include "serial_protocol.h"
#include <string.h>

/**
 * @brief Byte stuffing for one byte
 */
int32_t ByteStuffCopyByte(uint8_t *Dest, uint8_t Source)
{
  switch (Source)
  {
    case Msg_EOF:
      Dest[0] = Msg_BS;
      Dest[1] = Msg_BS_EOF;
      return 2;
    case Msg_BS:
      Dest[0] = Msg_BS;
      Dest[1] = Msg_BS;
      return 2;
    default:
      Dest[0] = Source;
      return 1;
  }
}

/**
 * @brief Byte stuffing for a full message (adds trailing Msg_EOF)
 */
int32_t ByteStuffCopy(uint8_t *Dest, Msg_t *Source)
{
  uint32_t i;
  int32_t count = 0;

  for (i = 0; i < Source->Len; i++)
  {
    count += ByteStuffCopyByte(&Dest[count], Source->Data[i]);
  }
  Dest[count] = Msg_EOF;
  return count + 1;
}

/**
 * @brief Reverse unstuffing for two input bytes
 */
int32_t ReverseByteStuffCopyByte2(uint8_t Source0, uint8_t Source1, uint8_t *Dest)
{
  if (Source0 == (uint8_t)Msg_BS)
  {
    if (Source1 == (uint8_t)Msg_BS_EOF) { *Dest = Msg_EOF; return 2; }
    if (Source1 == (uint8_t)Msg_BS)     { *Dest = Msg_BS;  return 2; }
    return 0; /* invalid */
  }
  *Dest = Source0;
  return 1;
}

/**
 * @brief Reverse byte unstuffing for a message up to Msg_EOF
 */
int32_t ReverseByteStuffCopy(Msg_t *Dest, uint8_t *Source)
{
  uint32_t count = 0;
  int state = 0; /* 0=normal, 1=after BS */

  while (*Source != (uint8_t)Msg_EOF)
  {
    if (state == 0)
    {
      if (*Source == (uint8_t)Msg_BS)
        state = 1;
      else
        Dest->Data[count++] = *Source;
    }
    else /* state == 1, just read the escape byte */
    {
      if (*Source == (uint8_t)Msg_BS_EOF)
        Dest->Data[count++] = Msg_EOF;
      else if (*Source == (uint8_t)Msg_BS)
        Dest->Data[count++] = Msg_BS;
      else
        return 0; /* invalid sequence */
      state = 0;
    }
    Source++;
    if (count >= Msg_MaxLen) return 0;
  }
  Dest->Len = count;
  return (state == 0) ? 1 : 0;
}

/**
 * @brief Compute and append checksum (sum + checksum = 0)
 */
void CHK_ComputeAndAdd(Msg_t *Msg)
{
  uint8_t chk = 0;
  uint32_t i;
  for (i = 0; i < Msg->Len; i++)
    chk -= Msg->Data[i];
  Msg->Data[i] = chk;
  Msg->Len++;
}

/**
 * @brief Verify and remove trailing checksum byte
 * @retval 1 if OK, 0 if failed
 */
int32_t CHK_CheckAndRemove(Msg_t *Msg)
{
  uint8_t chk = 0;
  uint32_t i;
  for (i = 0; i < Msg->Len; i++)
    chk += Msg->Data[i];
  Msg->Len--;
  return (chk == 0U) ? 1 : 0;
}

/**
 * @brief Serialize uint32_t LSB-first
 */
void Serialize_u32(uint8_t *Dest, uint32_t Source, uint32_t Len)
{
  uint32_t i;
  for (i = 0; i < Len; i++)
  {
    Dest[i] = (uint8_t)(Source & 0xFFU);
    Source >>= 8;
  }
}

/**
 * @brief Serialize int32_t LSB-first
 */
void Serialize_s32(uint8_t *Dest, int32_t Source, uint32_t Len)
{
  Serialize_u32(Dest, (uint32_t)Source, Len);
}

/**
 * @brief Deserialize uint32_t LSB-first
 */
uint32_t Deserialize_u32(uint8_t *Source, uint32_t Len)
{
  uint32_t val = Source[--Len];
  while (Len > 0U) { val <<= 8; val += Source[--Len]; }
  return val;
}

/**
 * @brief Serialize float (plain memcpy)
 */
void FloatToArray(uint8_t *Dest, float Data)
{
  (void)memcpy(Dest, (const void *)&Data, 4);
}
