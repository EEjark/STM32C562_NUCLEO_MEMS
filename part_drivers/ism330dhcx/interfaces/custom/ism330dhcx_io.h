/**
  ******************************************************************************
  * @file    ism330dhcx_io.h
  * @brief   ISM330DHCX header IO driver file for Custom IO.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ISM330DHCX_IO_H
#define ISM330DHCX_IO_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
/*
 * Callback features are optional; the feature flag ISM330DHCX_CALLBACKS signals their availability.
 * You can conditionally define this flag based on any preprocessor condition.
 * Examples using feature flags in mx_hal_def.h: USE_HAL_EXTI_REGISTER_CALLBACKS, USE_HAL_EXTI_USER_DATA...
 */
#define ISM330DHCX_CALLBACKS 1

/**
  * @brief ISM330DHCX IO interrupt pin.
  */
typedef enum
{
  ISM330DHCX_IO_INT1_PIN,     /*!< IO int1 pin         */
  ISM330DHCX_IO_INT2_PIN,     /*!< IO int2 pin         */
} ism330dhcx_io_sensorintpin_t;

typedef struct ism330dhcx_io_s ism330dhcx_io_t;
#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
typedef void (*ism330dhcx_io_callback_t)(ism330dhcx_io_t *pio, uint8_t polarity, void *arg); /*!< IO callback function pointer definition */
typedef struct
{
  ism330dhcx_io_callback_t io_callback;
  void *arg;
} ism330dhcx_io_cb_ctx_t;
#endif /* ISM330DHCX_CALLBACKS */
struct ism330dhcx_io_s
{
  uint32_t                   id;
  /* Add fields as needed to store HAL handles and other resources */
#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
  ism330dhcx_io_cb_ctx_t exti_cb_ctx1;
  ism330dhcx_io_cb_ctx_t exti_cb_ctx2;
#endif /* ISM330DHCX_CALLBACKS */
};
int32_t ism330dhcx_io_init(ism330dhcx_io_t *pio);
int32_t ism330dhcx_io_write_reg(ism330dhcx_io_t *pio, const uint8_t reg, uint8_t *pdata, uint16_t len);
int32_t ism330dhcx_io_read_reg(ism330dhcx_io_t *pio, const uint8_t reg, uint8_t *pdata, uint16_t len);
int32_t ism330dhcx_io_enable_irq(ism330dhcx_io_t *pio, ism330dhcx_io_sensorintpin_t intpin);
int32_t ism330dhcx_io_disable_irq(ism330dhcx_io_t *pio, ism330dhcx_io_sensorintpin_t intpin);
#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
int32_t ism330dhcx_io_register_callback(ism330dhcx_io_t *pio, ism330dhcx_io_callback_t io_cb, void *arg,
                                        ism330dhcx_io_sensorintpin_t intpin);
#endif /* ISM330DHCX_CALLBACKS */
#ifdef __cplusplus
}
#endif
#endif /* ISM330DHCX_IO_H */
