/**
  ******************************************************************************
  * @file    ism330dhcx_io.c
  * @brief   ISM330DHCX IO driver file for I2C
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

#include "ism330dhcx_io.h"

/**
  * @brief  Initialize the ISM330DHCX IO I2C flavor.
  * @param  pio the IO instance object
  * @retval 0 in case of success, an error code otherwise
  */
int32_t __weak ism330dhcx_io_init(ism330dhcx_io_t *pio)
{
  (void)pio;
  return 0;
}

/**
  * @brief  The write register function for ISM330DHCX IO I2C flavor.
  * @param  pio the IO instance object
  * @param  reg the device register to write
  * @param  pdata the write data
  * @param  len the length of the write buffer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_io_write_reg(ism330dhcx_io_t *pio, const uint8_t reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = 0;
  if (HAL_I2C_MASTER_MemWrite(pio->hi2c, pio->addr, reg, HAL_I2C_MEM_ADDR_8BIT,
                              pdata, len, ISM330DHCX_I2C_POLL_TIMEOUT) != HAL_OK)
  {
    ret = -1;
  }
  return ret;
}

/**
  * @brief  The read register function for ISM330DHCX IO I2C flavor.
  * @param  pio the IO instance object
  * @param  reg the device register to read
  * @param  pdata the pointer where the read data are written
  * @param  len the length of the read buffer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_io_read_reg(ism330dhcx_io_t *pio, const uint8_t reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = 0;
  if (HAL_I2C_MASTER_MemRead(pio->hi2c, pio->addr, reg, HAL_I2C_MEM_ADDR_8BIT,
                             pdata, len, ISM330DHCX_I2C_POLL_TIMEOUT) != HAL_OK)
  {
    ret = -1;
  }
  return ret;
}

/**
  * @brief  Enable the IRQ line and start the EXTI.
  * @param  pio the IO instance object
  * @param  intpin the IO interrupt pin
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_io_enable_irq(ism330dhcx_io_t *pio, ism330dhcx_io_sensorintpin_t intpin)
{
  int32_t ret_val = -1;

  if (intpin == ISM330DHCX_IO_INT1_PIN)
  {
    hal_exti_handle_t *p_exti = pio->hexti_int1;
    if (p_exti != NULL)
    {
      ret_val = 0;
      /* Clear interrupt pending bit */
      HAL_EXTI_ClearPending(p_exti, HAL_EXTI_TRIGGER_RISING);

      /* Start the selected EXTI line on Interrupt Mode. */
      if (HAL_EXTI_Enable(p_exti, HAL_EXTI_MODE_INTERRUPT) != HAL_OK)
      {
        ret_val = -1;
      }
    }
  }

  if (intpin == ISM330DHCX_IO_INT2_PIN)
  {
    hal_exti_handle_t *p_exti = pio->hexti_int2;
    if (p_exti != NULL)
    {
      ret_val = 0;
      /* Clear interrupt pending bit */
      HAL_EXTI_ClearPending(p_exti, HAL_EXTI_TRIGGER_RISING);

      /* Start the selected EXTI line on Interrupt Mode. */
      if (HAL_EXTI_Enable(p_exti, HAL_EXTI_MODE_INTERRUPT) != HAL_OK)
      {
        ret_val = -1;
      }
    }
  }
  return ret_val;
}

/**
  * @brief  Disable the IRQ line and clear the interrupt pending bit.
  * @param  pio the IO instance object
  * @param  intpin the IO interrupt pin
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_io_disable_irq(ism330dhcx_io_t *pio, ism330dhcx_io_sensorintpin_t intpin)
{
  int32_t ret_val = -1;
  if (intpin == ISM330DHCX_IO_INT1_PIN)
  {
    hal_exti_handle_t *p_exti = pio->hexti_int1;

    if (p_exti != NULL)
    {
      ret_val = 0;
      if (HAL_EXTI_Disable(p_exti) != HAL_OK)
      {
        ret_val = -1;
      }
      /* Clear interrupt pending bit */
      HAL_EXTI_ClearPending(p_exti, HAL_EXTI_TRIGGER_RISING);

    }
  }

  if (intpin == ISM330DHCX_IO_INT2_PIN)
  {
    hal_exti_handle_t *p_exti = pio->hexti_int2;

    if (p_exti != NULL)
    {
      ret_val = 0;
      if (HAL_EXTI_Disable(p_exti) != HAL_OK)
      {
        ret_val = -1;
      }
      /* Clear interrupt pending bit */
      HAL_EXTI_ClearPending(p_exti, HAL_EXTI_TRIGGER_RISING);

    }
  }
  return ret_val;
}

#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)

static void ism330dhcx_exti_rising_callback1(hal_exti_handle_t *p_exti, hal_exti_trigger_t trigger)
{
  ism330dhcx_io_t *pio = (ism330dhcx_io_t *)HAL_EXTI_GetUserData(p_exti);
  switch (trigger)
  {
    case HAL_EXTI_TRIGGER_RISING:
      pio->exti_cb_ctx1.io_callback(pio, pio->exti_cb_ctx1.arg);
      break;
    default:
      break;
  }
}

static void ism330dhcx_exti_rising_callback2(hal_exti_handle_t *p_exti, hal_exti_trigger_t trigger)
{
  ism330dhcx_io_t *pio = (ism330dhcx_io_t *)HAL_EXTI_GetUserData(p_exti);
  switch (trigger)
  {
    case HAL_EXTI_TRIGGER_RISING:
      pio->exti_cb_ctx2.io_callback(pio, pio->exti_cb_ctx2.arg);
      break;
    default:
      break;
  }
}

/**
  * @brief  Registers a callback on the IT pin of the ISM330DHCX.
  * @param  pio the IO instance object
  * @param  io_cb the callback function to be registered
  * @param  arg the optional argument given to the callback
  * @param  intpin the IO interrupt pin
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_io_register_callback(ism330dhcx_io_t *pio, ism330dhcx_io_callback_t io_cb, void *arg,
                                        ism330dhcx_io_sensorintpin_t intpin)
{
  int32_t ret_val = -1;
  if (intpin == ISM330DHCX_IO_INT1_PIN)
  {
    hal_exti_handle_t *p_exti = pio->hexti_int1;

    if (p_exti != NULL)
    {
      ret_val = 0;
      pio->exti_cb_ctx1.io_callback = io_cb;
      pio->exti_cb_ctx1.arg = arg;
      HAL_EXTI_SetUserData(p_exti, pio);

      if (HAL_EXTI_RegisterTriggerCallback(p_exti, ism330dhcx_exti_rising_callback1) != HAL_OK)
      {
        ret_val = -1;
      }
    }
  }

  if (intpin == ISM330DHCX_IO_INT2_PIN)
  {
    hal_exti_handle_t *p_exti = pio->hexti_int2;

    if (p_exti != NULL)
    {
      ret_val = 0;
      pio->exti_cb_ctx2.io_callback = io_cb;
      pio->exti_cb_ctx2.arg = arg;
      HAL_EXTI_SetUserData(p_exti, pio);

      if (HAL_EXTI_RegisterTriggerCallback(p_exti, ism330dhcx_exti_rising_callback2) != HAL_OK)
      {
        ret_val = -1;
      }
    }
  }

  return ret_val;
}

#endif /* ISM330DHCX_CALLBACKS */
