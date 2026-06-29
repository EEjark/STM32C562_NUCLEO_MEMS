/**
  ******************************************************************************
  * @file    ism330dhcx_io.c
  * @brief   ISM330DHCX IO driver file for Custom IO
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
/* Includes ------------------------------------------------------------------*/
#include "ism330dhcx_io.h"
/**
  * @brief Initializes the IO layer by assigning resources to the IO object.
  * @param  pio       : Pointer to io interface object.
  * @retval 0 in case of success, an error code otherwise.
  */
int32_t ism330dhcx_io_init(ism330dhcx_io_t *pio)
{
  switch (pio->id)
  {
    /* assign resources to the pio according to its ID */
    /* case ISM330DHCX_0: ... */
    default:
      return -1; /* unrecognized object: return an error */
  }
  return 0;
}
/**
  * @brief  The write register function for ISM330DHCX IO Custom flavor.
  * @param  pio the IO instance object.
  * @param  reg the device register to write.
  * @param  pdata the write data.
  * @param  len the length of the write buffer.
  * @retval 0 in case of success, an error code otherwise.
  */
int32_t ism330dhcx_io_write_reg(ism330dhcx_io_t *pio, const uint8_t reg, uint8_t *pdata, uint16_t len)
{
  return -1;
}
/**
  * @brief  The read register function for ISM330DHCX IO Custom flavor.
  * @param  pio the IO instance object.
  * @param  reg the device register to read.
  * @param  pdata the pointer where the read data are written.
  * @param  len the length of the read buffer.
  * @retval 0 in case of success, an error code otherwise.
  */
int32_t ism330dhcx_io_read_reg(ism330dhcx_io_t *pio, const uint8_t reg, uint8_t *pdata, uint16_t len)
{
  return -1;
}

/**
  * @brief  Enable the IRQ line and start the EXTI.
  * @param  pio the IO instance object.
  * @param  intpin the IO interrupt pin.
  * @retval 0 in case of success, an error code otherwise.
  */
int32_t ism330dhcx_io_enable_irq(ism330dhcx_io_t *pio, ism330dhcx_io_sensorintpin_t intpin)
{
  return -1;
}
/**
  * @brief  Disable the IRQ line and clear the interrupt pending bit.
  * @param  pio the IO instance object.
  * @param  intpin the IO interrupt pin.
  * @retval 0 in case of success, an error code otherwise.
  */
int32_t ism330dhcx_io_disable_irq(ism330dhcx_io_t *pio, ism330dhcx_io_sensorintpin_t intpin)
{
  return -1;
}
#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
/**
  * @brief  Registers a callback on the IT pin of the ISM330DHCX.
  * @param  pio the IO instance object.
  * @param  io_cb the callback function to be registered.
  * @param  arg the optional argument given to the callback.
  * @param  intpin the IO interrupt pin.
  * @retval 0 in case of success, an error code otherwise.
  */
int32_t ism330dhcx_io_register_callback(ism330dhcx_io_t *pio, ism330dhcx_io_callback_t io_cb, void *arg,
                                        ism330dhcx_io_sensorintpin_t intpin)
{
  if (intpin == ISM330DHCX_IO_INT1_PIN)
  {
    pio->exti_cb_ctx1.io_callback = io_cb;
    pio->exti_cb_ctx1.arg = arg;
  }
  if (intpin == ISM330DHCX_IO_INT2_PIN)
  {
    pio->exti_cb_ctx2.io_callback = io_cb;
    pio->exti_cb_ctx2.arg = arg;
  }

  /* Implement your own callback mechanism here. */
  return -1;
}
#endif /* ISM330DHCX_CALLBACKS */
