/**
  ******************************************************************************
  * @file    ism330dhcx_io.h
  * @brief   ISM330DHCX header IO driver file for I2C
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
extern "C"
{
#endif

#include "stm32_hal.h"

#if defined (USE_HAL_EXTI_REGISTER_CALLBACKS) && (USE_HAL_EXTI_REGISTER_CALLBACKS == 1)
#if defined (USE_HAL_EXTI_USER_DATA) && (USE_HAL_EXTI_USER_DATA == 1)
#define ISM330DHCX_CALLBACKS 1
#endif /* USE_HAL_EXTI_USER_DATA */
#endif /* USE_HAL_EXTI_REGISTER_CALLBACKS */

#define ISM330DHCX_USE_I2C_POLL            /*!< Use I2C in polling mode             */
#define ISM330DHCX_I2C_POLL_TIMEOUT 0x1000 /*!< Default I2C timeout in polling mode */

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
typedef void (*ism330dhcx_io_callback_t)(ism330dhcx_io_t *pio, void *arg); /*!< IO callback function pointer definition */

typedef struct
{
  ism330dhcx_io_callback_t io_callback;
  void *arg;
} ism330dhcx_io_cb_ctx_t;

#endif /* ISM330DHCX_CALLBACKS */

/**
  * @brief I2C IO interface structure.
  */
struct ism330dhcx_io_s
{
  uint32_t id;                         /*!< Device id              */
  hal_i2c_handle_t *hi2c;              /*!< I2C handle             */
  uint8_t addr;                        /*!< ISM330DHCX I2C address */
  hal_exti_handle_t *hexti_int1;       /*!< EXTI handle INT1       */
  hal_exti_handle_t *hexti_int2;       /*!< EXTI handle INT2       */
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
