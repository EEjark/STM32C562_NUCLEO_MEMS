/**
  ******************************************************************************
  * @file    ism330dhcx.c
  * @brief   ISM330DHCX driver file
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
#include "ism330dhcx.h"
#include <string.h>

static ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_outputdatarate_when_enabled(ism330dhcx_object_t *p_obj,
    float_t odr);
static ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_outputdatarate_when_disabled(ism330dhcx_object_t *p_obj,
    float_t odr);
static ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_outputdatarate_when_enabled(ism330dhcx_object_t *p_obj,
    float_t odr);
static ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_outputdatarate_when_disabled(ism330dhcx_object_t *p_obj,
    float_t odr);

/**
  * @brief  Override the ISM330DHCX read register weak function.
  * @param  p_ctx the context of the reg file
  * @param  reg the device register to read
  * @param  p_data the pointer where the read data are written
  * @param  len the length of the read buffer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_read_reg(const stmdev_ctx_t *p_ctx, uint8_t reg, uint8_t *p_data, uint16_t len)
{
  int32_t ret;
  ism330dhcx_object_t *p_handle = (ism330dhcx_object_t *)(p_ctx->handle);
  ret = ism330dhcx_io_read_reg(&(p_handle->pio), reg, p_data, len);
  return ret;
}

/**
  * @brief  Override the ISM330DHCX write register weak function.
  * @param  p_ctx the context of the reg file
  * @param  reg the device register to write
  * @param  p_data the write data
  * @param  len the length of the write buffer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ism330dhcx_write_reg(const stmdev_ctx_t *p_ctx, uint8_t reg, uint8_t *p_data, uint16_t len)
{
  int32_t ret;
  ism330dhcx_object_t *p_handle = (ism330dhcx_object_t *)(p_ctx->handle);
  ret = ism330dhcx_io_write_reg(&(p_handle->pio), reg, p_data, len);
  return ret;
}

/**
  * @brief  Initialize the ISM330DHCX sensor.
  * @param  p_obj the device instance object
  * @param  id the device instance id
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_init(ism330dhcx_object_t *p_obj, uint32_t id)
{
  if (p_obj->is_initialized == 0U)
  {

    p_obj->ctx.handle = p_obj;
    p_obj->pio.id = id;

    /* Initialize IO */
    if (ism330dhcx_io_init(&(p_obj->pio)) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Set DEVICE_CONF bit */
    if (ism330dhcx_device_conf_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Set main memory bank */
    if (ism330dhcx_mem_bank_set(&(p_obj->ctx), ISM330DHCX_USER_BANK) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Enable register address automatically incremented during a multiple byte
    access with a serial interface. */
    if (ism330dhcx_auto_increment_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* SW reset */
    if (ism330dhcx_reset_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Enable register address automatically incremented during a multiple byte
    access with a serial interface. */
    if (ism330dhcx_auto_increment_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Enable BDU */
    if (ism330dhcx_block_data_update_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* FIFO mode selection */
    if (ism330dhcx_fifo_mode_set(&(p_obj->ctx), ISM330DHCX_BYPASS_MODE) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Select default output data rate. */
    p_obj->acc_odr = ISM330DHCX_XL_ODR_104Hz;

    /* Output data rate selection - power down. */
    if (ism330dhcx_xl_data_rate_set(&(p_obj->ctx), ISM330DHCX_XL_ODR_OFF) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Full scale selection. */
    if (ism330dhcx_xl_full_scale_set(&(p_obj->ctx), ISM330DHCX_2g) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Select default output data rate. */
    p_obj->gyro_odr = ISM330DHCX_GY_ODR_104Hz;

    /* Output data rate selection - power down. */
    if (ism330dhcx_gy_data_rate_set(&(p_obj->ctx), ISM330DHCX_GY_ODR_OFF) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    /* Full scale selection. */
    if (ism330dhcx_gy_full_scale_set(&(p_obj->ctx), ISM330DHCX_2000dps) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }
  }

  p_obj->is_initialized = 1;

  return ISM330DHCX_OK;
}

/**
  * @brief  Deinitialize the ISM330DHCX sensor.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_deinit(ism330dhcx_object_t *p_obj)
{
  if (p_obj->is_initialized == 1u)
  {
    /* Disable the component */
    if (ism330dhcx_drv_acc_disable(p_obj) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }

    if (ism330dhcx_drv_gyro_disable(p_obj) != ISM330DHCX_OK)
    {
      return ISM330DHCX_ERROR;
    }
  }

  /* Reset output data rate. */
  p_obj->acc_odr = ISM330DHCX_XL_ODR_OFF;
  p_obj->gyro_odr = ISM330DHCX_GY_ODR_OFF;

  p_obj->is_initialized = 0;

  return ISM330DHCX_OK;
}

/**
  * @brief  Read the ISM330DHCX id.
  * @param  p_obj the device instance object
  * @param  p_id the pointer where the id is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_readid(ism330dhcx_object_t *p_obj, uint8_t *p_id)
{
  if (ism330dhcx_device_id_get(&(p_obj->ctx), p_id) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable the ISM330DHCX accelerometer sensor.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable(ism330dhcx_object_t *p_obj)
{
  /* Check if the component is already enabled */
  if (p_obj->acc_is_enabled == 1U)
  {
    return ISM330DHCX_OK;
  }

  /* Output data rate selection. */
  if (ism330dhcx_xl_data_rate_set(&(p_obj->ctx), p_obj->acc_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  p_obj->acc_is_enabled = 1;

  return ISM330DHCX_OK;
}

/**
  * @brief  Disable the ISM330DHCX accelerometer sensor.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable(ism330dhcx_object_t *p_obj)
{
  /* Check if the component is already disabled */
  if (p_obj->acc_is_enabled == 0U)
  {
    return ISM330DHCX_OK;
  }

  /* Get current output data rate. */
  if (ism330dhcx_xl_data_rate_get(&(p_obj->ctx), &p_obj->acc_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Output data rate selection - power down. */
  if (ism330dhcx_xl_data_rate_set(&(p_obj->ctx), ISM330DHCX_XL_ODR_OFF) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  p_obj->acc_is_enabled = 0;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX accelerometer sensor sensitivity.
  * @param  p_obj the device instance object
  * @param  p_sensitivity the pointer where the sensitivity is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_sensitivity(ism330dhcx_object_t *p_obj, float_t *p_sensitivity)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_fs_xl_t fullscale;

  /* Read actual full scale selection from sensor. */
  if (ism330dhcx_xl_full_scale_get(&(p_obj->ctx), &fullscale) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Store the sensitivity based on actual full scale. */
  switch (fullscale)
  {
    case ISM330DHCX_2g:
      *p_sensitivity = ISM330DHCX_ACC_SENSITIVITY_FS_2G; /* sensitivity with 2G full scale. */
      break;

    case ISM330DHCX_4g:
      *p_sensitivity = ISM330DHCX_ACC_SENSITIVITY_FS_4G; /* sensitivity with 4G full scale. */
      break;

    case ISM330DHCX_8g:
      *p_sensitivity = ISM330DHCX_ACC_SENSITIVITY_FS_8G; /* sensitivity with 8G full scale. */
      break;

    case ISM330DHCX_16g:
      *p_sensitivity = ISM330DHCX_ACC_SENSITIVITY_FS_16G; /* sensitivity with 16G full scale. */
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Get the ISM330DHCX accelerometer sensor output data rate.
  * @param  p_obj the device instance object
  * @param  p_odr the pointer where the output data rate is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_outputdatarate(ism330dhcx_object_t *p_obj, float_t *p_odr)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_odr_xl_t odr_low_level;

  /* Get current output data rate. */
  if (ism330dhcx_xl_data_rate_get(&(p_obj->ctx), &odr_low_level) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the ODR value */
  switch (odr_low_level)
  {
    case ISM330DHCX_XL_ODR_OFF:
      *p_odr = 0.0f;
      break;

    case ISM330DHCX_XL_ODR_12Hz5:
      *p_odr = ISM330DHCX_ACC_ODR_12HZ5_FP; /* ODR value 12.5 Hz*/
      break;

    case ISM330DHCX_XL_ODR_26Hz:
      *p_odr = ISM330DHCX_ACC_ODR_26HZ_FP; /* ODR value 26.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_52Hz:
      *p_odr = ISM330DHCX_ACC_ODR_52HZ_FP; /* ODR value 52.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_104Hz:
      *p_odr = ISM330DHCX_ACC_ODR_104HZ_FP; /* ODR value 104.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_208Hz:
      *p_odr = ISM330DHCX_ACC_ODR_208HZ_FP; /* ODR value 208.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_416Hz:
      *p_odr = ISM330DHCX_ACC_ODR_416HZ_FP; /* ODR value 416.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_833Hz:
      *p_odr = ISM330DHCX_ACC_ODR_833HZ_FP; /* ODR value 833.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_1666Hz:
      *p_odr = ISM330DHCX_ACC_ODR_1666HZ_FP; /* ODR value 1666.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_3332Hz:
      *p_odr = ISM330DHCX_ACC_ODR_3332HZ_FP; /* ODR value 3332.0 Hz*/
      break;

    case ISM330DHCX_XL_ODR_6667Hz:
      *p_odr = ISM330DHCX_ACC_ODR_6667HZ_FP; /* ODR value 6667.0 Hz*/
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Set the ISM330DHCX accelerometer sensor output data rate.
  * @param  p_obj the device instance object
  * @param  odr the output data rate to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_outputdatarate(ism330dhcx_object_t *p_obj, float_t odr)
{
  /* Check if the component is enabled */
  if (p_obj->acc_is_enabled == 1U)
  {
    return ism330dhcx_drv_acc_set_outputdatarate_when_enabled(p_obj, odr);
  }
  else
  {
    return ism330dhcx_drv_acc_set_outputdatarate_when_disabled(p_obj, odr);
  }
}

/**
  * @brief  Get the ISM330DHCX accelerometer sensor full scale.
  * @param  p_obj the device instance object
  * @param  p_fullscale the pointer where the full scale is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_fullscale(ism330dhcx_object_t *p_obj, float_t *p_fullscale)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_fs_xl_t fs_low_level;

  /* Read actual full scale selection from sensor. */
  if (ism330dhcx_xl_full_scale_get(&(p_obj->ctx), &fs_low_level) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the full scale value */
  switch (fs_low_level)
  {
    case ISM330DHCX_2g:
      *p_fullscale =  ISM330DHCX_ACC_FS_2G_FP; /* full scale value 2G */
      break;

    case ISM330DHCX_4g:
      *p_fullscale =  ISM330DHCX_ACC_FS_4G_FP; /* full scale value 4G */
      break;

    case ISM330DHCX_8g:
      *p_fullscale =  ISM330DHCX_ACC_FS_8G_FP; /* full scale value 8G */
      break;

    case ISM330DHCX_16g:
      *p_fullscale = ISM330DHCX_ACC_FS_16G_FP; /* full scale value 16G */
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Set the ISM330DHCX accelerometer full scale.
  * @param  p_obj the device instance object
  * @param  fullscale the full scale to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_fullscale(ism330dhcx_object_t *p_obj, float_t fullscale)
{
  ism330dhcx_fs_xl_t new_fs;

  /* Seems like MISRA C-2012 rule 14.3a violation but only from single file statical analysis point of view because
     the parameter passed to the function is not known at the moment of analysis */
  new_fs = (fullscale <= ISM330DHCX_ACC_FS_2G_FP) ? ISM330DHCX_2g
           : (fullscale <= ISM330DHCX_ACC_FS_4G_FP) ? ISM330DHCX_4g
           : (fullscale <= ISM330DHCX_ACC_FS_8G_FP) ? ISM330DHCX_8g
           :                       ISM330DHCX_16g;

  if (ism330dhcx_xl_full_scale_set(&(p_obj->ctx), new_fs) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX accelerometer sensor raw axes.
  * @param  p_obj the device instance object
  * @param  p_value the pointer where the raw values of the axes are written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_axesraw(ism330dhcx_object_t *p_obj, ism330dhcx_axesraw_t *p_value)
{
  ism330dhcx_axis3bit16_t data_raw;

  /* Read raw data values. */
  if (ism330dhcx_acceleration_raw_get(&(p_obj->ctx), data_raw.i16bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Format the data. */
  p_value->x_axis = data_raw.i16bit[0];
  p_value->y_axis = data_raw.i16bit[1];
  p_value->z_axis = data_raw.i16bit[2];

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX accelerometer sensor axes.
  * @param  p_obj the device instance object
  * @param  p_acceleration the pointer where the 3-axis acceleration is written [unit is mg]
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_acceleration)
{
  ism330dhcx_axis3bit16_t data_raw;
  float_t sensitivity = 0.0f;

  /* Read raw data values. */
  if (ism330dhcx_acceleration_raw_get(&(p_obj->ctx), data_raw.i16bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Get ISM330DHCX actual sensitivity. */
  if (ism330dhcx_drv_acc_get_sensitivity(p_obj, &sensitivity) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Calculate the data. */
  p_acceleration->x_axis = (int32_t)((float_t)((float_t)data_raw.i16bit[0] * sensitivity));
  p_acceleration->y_axis = (int32_t)((float_t)((float_t)data_raw.i16bit[1] * sensitivity));
  p_acceleration->z_axis = (int32_t)((float_t)((float_t)data_raw.i16bit[2] * sensitivity));

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable the ISM330DHCX gyroscope sensor.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_enable(ism330dhcx_object_t *p_obj)
{
  /* Check if the component is already enabled */
  if (p_obj->gyro_is_enabled == 1U)
  {
    return ISM330DHCX_OK;
  }

  /* Output data rate selection. */
  if (ism330dhcx_gy_data_rate_set(&(p_obj->ctx), p_obj->gyro_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  p_obj->gyro_is_enabled = 1;

  return ISM330DHCX_OK;
}

/**
  * @brief  Disable the ISM330DHCX gyroscope sensor.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_disable(ism330dhcx_object_t *p_obj)
{
  /* Check if the component is already disabled */
  if (p_obj->gyro_is_enabled == 0U)
  {
    return ISM330DHCX_OK;
  }

  /* Get current output data rate. */
  if (ism330dhcx_gy_data_rate_get(&(p_obj->ctx), &p_obj->gyro_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Output data rate selection - power down. */
  if (ism330dhcx_gy_data_rate_set(&(p_obj->ctx), ISM330DHCX_GY_ODR_OFF) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  p_obj->gyro_is_enabled = 0;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX gyroscope sensor sensitivity.
  * @param  p_obj the device instance object
  * @param  p_sensitivity the pointer where the sensitivity is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_sensitivity(ism330dhcx_object_t *p_obj, float_t *p_sensitivity)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_fs_g_t fullscale;

  /* Read actual full scale selection from sensor. */
  if (ism330dhcx_gy_full_scale_get(&(p_obj->ctx), &fullscale) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Store the sensitivity based on actual full scale. */
  switch (fullscale)
  {
    case ISM330DHCX_125dps:
      *p_sensitivity = ISM330DHCX_GYRO_SENSITIVITY_FS_125DPS; /* sensitivity with 125 dps full scale. */
      break;

    case ISM330DHCX_250dps:
      *p_sensitivity = ISM330DHCX_GYRO_SENSITIVITY_FS_250DPS; /* sensitivity with 250 dps full scale. */
      break;

    case ISM330DHCX_500dps:
      *p_sensitivity = ISM330DHCX_GYRO_SENSITIVITY_FS_500DPS; /* sensitivity with 500 dps full scale. */
      break;

    case ISM330DHCX_1000dps:
      *p_sensitivity = ISM330DHCX_GYRO_SENSITIVITY_FS_1000DPS; /* sensitivity with 1000 dps full scale. */
      break;

    case ISM330DHCX_2000dps:
      *p_sensitivity = ISM330DHCX_GYRO_SENSITIVITY_FS_2000DPS; /* sensitivity with 2000 dps full scale. */
      break;

    case ISM330DHCX_4000dps:
      *p_sensitivity = ISM330DHCX_GYRO_SENSITIVITY_FS_4000DPS; /* sensitivity with 4000 dps full scale. */
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Get the ISM330DHCX gyroscope sensor output data rate.
  * @param  p_obj the device instance object
  * @param  p_odr the pointer where the output data rate is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_outputdatarate(ism330dhcx_object_t *p_obj, float_t *p_odr)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_odr_g_t odr_low_level;

  /* Get current output data rate. */
  if (ism330dhcx_gy_data_rate_get(&(p_obj->ctx), &odr_low_level) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the ODR value */
  switch (odr_low_level)
  {
    case ISM330DHCX_GY_ODR_OFF:
      *p_odr = 0.0f;
      break;

    case ISM330DHCX_GY_ODR_12Hz5:
      *p_odr = ISM330DHCX_GYRO_ODR_12HZ5_FP; /* ODR value 12.5 Hz*/
      break;

    case ISM330DHCX_GY_ODR_26Hz:
      *p_odr = ISM330DHCX_GYRO_ODR_26HZ_FP; /* ODR value 26.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_52Hz:
      *p_odr = ISM330DHCX_GYRO_ODR_52HZ_FP; /* ODR value 52.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_104Hz:
      *p_odr = ISM330DHCX_GYRO_ODR_104HZ_FP; /* ODR value 104.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_208Hz:
      *p_odr = ISM330DHCX_GYRO_ODR_208HZ_FP; /* ODR value 208.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_416Hz:
      *p_odr = ISM330DHCX_GYRO_ODR_416HZ_FP; /* ODR value 416.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_833Hz:
      *p_odr = ISM330DHCX_GYRO_ODR_833HZ_FP; /* ODR value 833.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_1666Hz:
      *p_odr =  ISM330DHCX_GYRO_ODR_1666HZ_FP; /* ODR value 1666.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_3332Hz:
      *p_odr =  ISM330DHCX_GYRO_ODR_3332HZ_FP; /* ODR value 3332.0 Hz*/
      break;

    case ISM330DHCX_GY_ODR_6667Hz:
      *p_odr =  ISM330DHCX_GYRO_ODR_6667HZ_FP; /* ODR value 6667.0 Hz*/
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Set the ISM330DHCX gyroscope sensor output data rate.
  * @param  p_obj the device instance object
  * @param  odr the output data rate to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_outputdatarate(ism330dhcx_object_t *p_obj, float_t odr)
{
  /* Check if the component is enabled */
  if (p_obj->gyro_is_enabled == 1U)
  {
    return ism330dhcx_drv_gyro_set_outputdatarate_when_enabled(p_obj, odr);
  }
  else
  {
    return ism330dhcx_drv_gyro_set_outputdatarate_when_disabled(p_obj, odr);
  }
}

/**
  * @brief  Get the ISM330DHCX gyroscope sensor full scale.
  * @param  p_obj the device instance object
  * @param  p_fullscale the pointer where the full scale is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_fullscale(ism330dhcx_object_t *p_obj, float_t *p_fullscale)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_fs_g_t fs_low_level;

  /* Read actual full scale selection from sensor. */
  if (ism330dhcx_gy_full_scale_get(&(p_obj->ctx), &fs_low_level) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the full scale value */
  switch (fs_low_level)
  {
    case ISM330DHCX_125dps:
      *p_fullscale =  ISM330DHCX_GYRO_FS_125DPS_FP; /* full scale 125 dps */
      break;

    case ISM330DHCX_250dps:
      *p_fullscale =  ISM330DHCX_GYRO_FS_250DPS_FP; /* full scale 250 dps */
      break;

    case ISM330DHCX_500dps:
      *p_fullscale =  ISM330DHCX_GYRO_FS_500DPS_FP; /* full scale 500 dps */
      break;

    case ISM330DHCX_1000dps:
      *p_fullscale = ISM330DHCX_GYRO_FS_1000DPS_FP; /* full scale 1000 dps */
      break;

    case ISM330DHCX_2000dps:
      *p_fullscale = ISM330DHCX_GYRO_FS_2000DPS_FP; /* full scale 2000 dps */
      break;

    case ISM330DHCX_4000dps:
      *p_fullscale = ISM330DHCX_GYRO_FS_4000DPS_FP; /* full scale 4000 dps */
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Set the ISM330DHCX gyroscope full scale.
  * @param  p_obj the device instance object
  * @param  fullscale the full scale to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_fullscale(ism330dhcx_object_t *p_obj, float_t fullscale)
{
  ism330dhcx_fs_g_t new_fs;

  new_fs = (fullscale <= ISM330DHCX_GYRO_FS_125DPS_FP)  ? ISM330DHCX_125dps
           : (fullscale <= ISM330DHCX_GYRO_FS_250DPS_FP)  ? ISM330DHCX_250dps
           : (fullscale <= ISM330DHCX_GYRO_FS_500DPS_FP)  ? ISM330DHCX_500dps
           : (fullscale <= ISM330DHCX_GYRO_FS_1000DPS_FP) ? ISM330DHCX_1000dps
           : (fullscale <= ISM330DHCX_GYRO_FS_2000DPS_FP) ? ISM330DHCX_2000dps
           :                          ISM330DHCX_4000dps;

  if (ism330dhcx_gy_full_scale_set(&(p_obj->ctx), new_fs) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX gyroscope sensor raw axes.
  * @param  p_obj the device instance object
  * @param  p_value the pointer where the raw values of the axes are written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_axesraw(ism330dhcx_object_t *p_obj, ism330dhcx_axesraw_t *p_value)
{
  ism330dhcx_axis3bit16_t data_raw;

  /* Read raw data values. */
  if (ism330dhcx_angular_rate_raw_get(&(p_obj->ctx), data_raw.i16bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Format the data. */
  p_value->x_axis = data_raw.i16bit[0];
  p_value->y_axis = data_raw.i16bit[1];
  p_value->z_axis = data_raw.i16bit[2];

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX gyroscope sensor axes.
  * @param  p_obj the device instance object
  * @param  p_angularrate the pointer where the 3-axis angular rate is written [unit is mdps]
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_angularrate)
{
  ism330dhcx_axis3bit16_t data_raw;
  float_t sensitivity;

  /* Read raw data values. */
  if (ism330dhcx_angular_rate_raw_get(&(p_obj->ctx), data_raw.i16bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Get ISM330DHCX actual sensitivity. */
  if (ism330dhcx_drv_gyro_get_sensitivity(p_obj, &sensitivity) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Calculate the data. */
  p_angularrate->x_axis = (int32_t)((float_t)((float_t)data_raw.i16bit[0] * sensitivity));
  p_angularrate->y_axis = (int32_t)((float_t)((float_t)data_raw.i16bit[1] * sensitivity));
  p_angularrate->z_axis = (int32_t)((float_t)((float_t)data_raw.i16bit[2] * sensitivity));

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX register value.
  * @param  p_obj the device instance object
  * @param  reg register to be read
  * @param  p_data the pointer where the value is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_read_reg(ism330dhcx_object_t *p_obj, uint8_t reg, uint8_t *p_data)
{
  if (ism330dhcx_read_reg(&(p_obj->ctx), reg, p_data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX register value.
  * @param  p_obj the device instance object
  * @param  reg register to be read
  * @param  data the value to be written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_write_reg(ism330dhcx_object_t *p_obj, uint8_t reg, uint8_t data)
{
  if (ism330dhcx_write_reg(&(p_obj->ctx), reg, &data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the interrupt latch.
  * @param  p_obj the device instance object
  * @param  status the value to be written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_set_interrupt_latch(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_lir_t setting = ISM330DHCX_ALL_INT_PULSED;

  if (status > 3U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the type of interrupt */
  switch (status)
  {
    case ISM330DHCX_ALL_INT_PULSED:
      setting = ISM330DHCX_ALL_INT_PULSED; /* all interrupts are pulsed */
      break;
    case ISM330DHCX_BASE_LATCHED_EMB_PULSED:
      setting = ISM330DHCX_BASE_LATCHED_EMB_PULSED; /* base interrupt is latched and embedded interrupt is pulsed */
      break;
    case ISM330DHCX_BASE_PULSED_EMB_LATCHED:
      setting = ISM330DHCX_BASE_PULSED_EMB_LATCHED; /* base interrupt is pulsed and embedded interrupt is latched */
      break;
    case ISM330DHCX_ALL_INT_LATCHED:
      setting = ISM330DHCX_ALL_INT_LATCHED; /* all interrupts are latched */
      break;
    default:
      break;
  }

  if (ism330dhcx_int_notification_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX accelerometer data ready on INT1 pin.
  * @param  p_obj the device instance object
  * @param  status the value to be written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_drdy_on_int1(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_reg_t reg;

  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  reg.int1_ctrl.int1_drdy_xl = status;

  if (ism330dhcx_write_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX accelerometer data ready on INT2 pin.
  * @param  p_obj the device instance object
  * @param  status the value to be written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_drdy_on_int2(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_reg_t reg;

  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT2_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  reg.int2_ctrl.int2_drdy_xl = status;

  if (ism330dhcx_write_reg(&(p_obj->ctx), ISM330DHCX_INT2_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX gyroscope data ready on INT1 pin.
  * @param  p_obj the device instance object
  * @param  status the value to be written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_drdy_on_int1(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_reg_t reg;

  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  reg.int1_ctrl.int1_drdy_g = status;

  if (ism330dhcx_write_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX gyroscope data ready on INT2 pin.
  * @param  p_obj the device instance object
  * @param  status the value to be written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_drdy_on_int2(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_reg_t reg;

  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT2_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  reg.int2_ctrl.int2_drdy_g = status;

  if (ism330dhcx_write_reg(&(p_obj->ctx), ISM330DHCX_INT2_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX data ready mode.
  * @param  p_obj the device instance object
  * @param  mode the data ready mode to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_set_drdy_mode(ism330dhcx_object_t *p_obj, uint8_t mode)
{
  ism330dhcx_dataready_pulsed_t setting = ISM330DHCX_DRDY_LATCHED;

  if (mode > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the data ready interrupt mode */
  switch (mode)
  {
    case ISM330DHCX_DRDY_LATCHED:
      setting = ISM330DHCX_DRDY_LATCHED; /* data ready interrupt is latched */
      break;
    case ISM330DHCX_DRDY_PULSED:
      setting = ISM330DHCX_DRDY_PULSED; /* data ready interrupt is pulsed */
      break;
    default:
      break;
  }

  if (ism330dhcx_data_ready_mode_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable free fall detection.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin to be used
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_free_fall_detection(ism330dhcx_object_t *p_obj,
                                                                      ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (ism330dhcx_drv_acc_set_outputdatarate(p_obj, 416.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Full scale selection */
  if (ism330dhcx_drv_acc_set_fullscale(p_obj, 2.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* FF_DUR setting */
  if (ism330dhcx_ff_dur_set(&(p_obj->ctx), 0x06) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* WAKE_DUR setting */
  if (ism330dhcx_wkup_dur_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* SLEEP_DUR setting */
  if (ism330dhcx_act_sleep_dur_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* FF_THS setting */
  if (ism330dhcx_ff_threshold_set(&(p_obj->ctx), ISM330DHCX_FF_TSH_312mg) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable free fall event on either INT1 or INT2 pin */
  switch (intpin)
  {
    case ISM330DHCX_INT1_PIN:
      if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val1.md1_cfg.int1_ff = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    case ISM330DHCX_INT2_PIN:
      if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val2.md2_cfg.int2_ff = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Disable free fall detection.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_free_fall_detection(ism330dhcx_object_t *p_obj)
{
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Disable free fall event on both INT1 and INT2 pins */
  if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val1.md1_cfg.int1_ff = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val2.md2_cfg.int2_ff = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* FF_DUR setting */
  if (ism330dhcx_ff_dur_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* FF_THS setting */
  if (ism330dhcx_ff_threshold_set(&(p_obj->ctx), ISM330DHCX_FF_TSH_156mg) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set free fall threshold.
  * @param  p_obj the device instance object
  * @param  threshold the free fall detection threshold
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_free_fall_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold)
{
  ism330dhcx_ff_ths_t setting = ISM330DHCX_FF_TSH_156mg;

  if (threshold > 7U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check the free fall threshold */
  switch (threshold)
  {
    case ISM330DHCX_FF_TSH_156mg:
      setting = ISM330DHCX_FF_TSH_156mg; /* threshold 156 mg */
      break;
    case ISM330DHCX_FF_TSH_219mg:
      setting = ISM330DHCX_FF_TSH_219mg; /* threshold 219 mg */
      break;
    case ISM330DHCX_FF_TSH_250mg:
      setting = ISM330DHCX_FF_TSH_250mg; /* threshold 250 mg */
      break;
    case ISM330DHCX_FF_TSH_312mg:
      setting = ISM330DHCX_FF_TSH_312mg; /* threshold 312 mg */
      break;
    case ISM330DHCX_FF_TSH_344mg:
      setting = ISM330DHCX_FF_TSH_344mg; /* threshold 344 mg */
      break;
    case ISM330DHCX_FF_TSH_406mg:
      setting = ISM330DHCX_FF_TSH_406mg; /* threshold 406 mg */
      break;
    case ISM330DHCX_FF_TSH_469mg:
      setting = ISM330DHCX_FF_TSH_469mg; /* threshold 469 mg */
      break;
    case ISM330DHCX_FF_TSH_500mg:
      setting = ISM330DHCX_FF_TSH_500mg; /* threshold 500 mg */
      break;
    default:
      break;
  }

  if (ism330dhcx_ff_threshold_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set free fall duration.
  * @param  p_obj the device instance object
  * @param  duration the free fall detection duration
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_free_fall_duration(ism330dhcx_object_t *p_obj, uint8_t duration)
{
  if (duration > 0x3Fu)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_ff_dur_set(&(p_obj->ctx), duration) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable wake up detection.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin to be used
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_wake_up_detection(ism330dhcx_object_t *p_obj,
                                                                    ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (ism330dhcx_drv_acc_set_outputdatarate(p_obj, 416.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Full scale selection */
  if (ism330dhcx_drv_acc_set_fullscale(p_obj, 2.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* WAKE_DUR setting */
  if (ism330dhcx_wkup_dur_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set wake up threshold. */
  if (ism330dhcx_wkup_threshold_set(&(p_obj->ctx), 0x02) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable wake up event on either INT1 or INT2 pin */
  switch (intpin)
  {
    case ISM330DHCX_INT1_PIN:
      if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val1.md1_cfg.int1_wu = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    case ISM330DHCX_INT2_PIN:
      if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val2.md2_cfg.int2_wu = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Disable wake up detection.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_wake_up_detection(ism330dhcx_object_t *p_obj)
{
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Disable wake up event on both INT1 and INT2 pins */
  if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val1.md1_cfg.int1_wu = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val2.md2_cfg.int2_wu = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset wake up threshold. */
  if (ism330dhcx_wkup_threshold_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* WAKE_DUR setting */
  if (ism330dhcx_wkup_dur_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set wake up threshold.
  * @param  p_obj the device instance object
  * @param  threshold the wake up detection threshold
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_wake_up_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold)
{
  if (threshold > 0x3Fu)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set wake up threshold. */
  if (ism330dhcx_wkup_threshold_set(&(p_obj->ctx), threshold) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set wake up duration.
  * @param  p_obj the device instance object
  * @param  duration the wake up detection duration
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_wake_up_duration(ism330dhcx_object_t *p_obj, uint8_t duration)
{
  if (duration > 3U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set wake up duration. */
  if (ism330dhcx_wkup_dur_set(&(p_obj->ctx), duration) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable single tap detection.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin to be used
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_single_tap_detection(ism330dhcx_object_t *p_obj,
                                                                       ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (ism330dhcx_drv_acc_set_outputdatarate(p_obj, 416.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Full scale selection */
  if (ism330dhcx_drv_acc_set_fullscale(p_obj, 2.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable X direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_x_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable Y direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_y_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable Z direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_z_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap threshold. */
  if (ism330dhcx_tap_threshold_x_set(&(p_obj->ctx), 0x08) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap shock time window. */
  if (ism330dhcx_tap_shock_set(&(p_obj->ctx), 0x02) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap quiet time window. */
  if (ism330dhcx_tap_quiet_set(&(p_obj->ctx), 0x01) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* _NOTE_: Tap duration time window - don't care for single tap. */

  /* _NOTE_: Single/Double Tap event - don't care of this flag for single tap. */

  /* Enable single tap event on either INT1 or INT2 pin */
  switch (intpin)
  {
    case ISM330DHCX_INT1_PIN:
      if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val1.md1_cfg.int1_single_tap = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    case ISM330DHCX_INT2_PIN:
      if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val2.md2_cfg.int2_single_tap = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Disable single tap detection.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_single_tap_detection(ism330dhcx_object_t *p_obj)
{
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Disable single tap event on both INT1 and INT2 pins */
  if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val1.md1_cfg.int1_single_tap = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val2.md2_cfg.int2_single_tap = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap quiet time window. */
  if (ism330dhcx_tap_quiet_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap shock time window. */
  if (ism330dhcx_tap_shock_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap threshold. */
  if (ism330dhcx_tap_threshold_x_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Disable Z direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_z_set(&(p_obj->ctx), PROPERTY_DISABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Disable Y direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_y_set(&(p_obj->ctx), PROPERTY_DISABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Disable X direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_x_set(&(p_obj->ctx), PROPERTY_DISABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable double tap detection.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin to be used
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_double_tap_detection(ism330dhcx_object_t *p_obj,
                                                                       ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (ism330dhcx_drv_acc_set_outputdatarate(p_obj, 416.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Full scale selection */
  if (ism330dhcx_drv_acc_set_fullscale(p_obj, 2.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable X direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_x_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable Y direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_y_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable Z direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_z_set(&(p_obj->ctx), PROPERTY_ENABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap threshold. */
  if (ism330dhcx_tap_threshold_x_set(&(p_obj->ctx), 0x08) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap shock time window. */
  if (ism330dhcx_tap_shock_set(&(p_obj->ctx), 0x03) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap quiet time window. */
  if (ism330dhcx_tap_quiet_set(&(p_obj->ctx), 0x03) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap duration time window. */
  if (ism330dhcx_tap_dur_set(&(p_obj->ctx), 0x08) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Single and double tap enabled. */
  if (ism330dhcx_tap_mode_set(&(p_obj->ctx), ISM330DHCX_BOTH_SINGLE_DOUBLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable double tap event on either INT1 or INT2 pin */
  switch (intpin)
  {
    case ISM330DHCX_INT1_PIN:
      if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val1.md1_cfg.int1_double_tap = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    case ISM330DHCX_INT2_PIN:
      if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val2.md2_cfg.int2_double_tap = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Disable double tap detection.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_double_tap_detection(ism330dhcx_object_t *p_obj)
{
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Disable double tap event on both INT1 and INT2 pins */
  if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val1.md1_cfg.int1_double_tap = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val2.md2_cfg.int2_double_tap = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Only single tap enabled. */
  if (ism330dhcx_tap_mode_set(&(p_obj->ctx), ISM330DHCX_ONLY_SINGLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap duration time window. */
  if (ism330dhcx_tap_dur_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap quiet time window. */
  if (ism330dhcx_tap_quiet_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap shock time window. */
  if (ism330dhcx_tap_shock_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset tap threshold. */
  if (ism330dhcx_tap_threshold_x_set(&(p_obj->ctx), 0x00) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Disable Z direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_z_set(&(p_obj->ctx), PROPERTY_DISABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Disable Y direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_y_set(&(p_obj->ctx), PROPERTY_DISABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Disable X direction in tap recognition. */
  if (ism330dhcx_tap_detection_on_x_set(&(p_obj->ctx), PROPERTY_DISABLE) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set tap threshold.
  * @param  p_obj the device instance object
  * @param  threshold the tap threshold
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold)
{
  if (threshold > 0x1Fu)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap threshold. */
  if (ism330dhcx_tap_threshold_x_set(&(p_obj->ctx), threshold) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set tap shock time.
  * @param  p_obj the device instance object
  * @param  time the tap shock time
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_shock_time(ism330dhcx_object_t *p_obj, uint8_t time)
{
  if (time > 3U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap shock time window. */
  if (ism330dhcx_tap_shock_set(&(p_obj->ctx), time) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set tap quiet time.
  * @param  p_obj the device instance object
  * @param  time the tap quiet time
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_quiet_time(ism330dhcx_object_t *p_obj, uint8_t time)
{
  if (time > 3U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap quiet time window. */
  if (ism330dhcx_tap_quiet_set(&(p_obj->ctx), time) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set tap duration.
  * @param  p_obj the device instance object
  * @param  duration the tap duration
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_duration_time(ism330dhcx_object_t *p_obj, uint8_t duration)
{
  if (duration > 0x0Fu)
  {
    return ISM330DHCX_ERROR;
  }

  /* Set tap duration time window. */
  if (ism330dhcx_tap_dur_set(&(p_obj->ctx), duration) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable 6D orientation detection.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin to be used
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_6d_orientation(ism330dhcx_object_t *p_obj,
                                                                 ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (ism330dhcx_drv_acc_set_outputdatarate(p_obj, 416.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Full scale selection */
  if (ism330dhcx_drv_acc_set_fullscale(p_obj, 2.0f) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* 6D orientation enabled. */
  if (ism330dhcx_6d_threshold_set(&(p_obj->ctx), ISM330DHCX_DEG_60) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Enable 6D orientation event on either INT1 or INT2 pin */
  switch (intpin)
  {
    case ISM330DHCX_INT1_PIN:
      if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val1.md1_cfg.int1_6d = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    case ISM330DHCX_INT2_PIN:
      if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }

      val2.md2_cfg.int2_6d = PROPERTY_ENABLE;

      if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
      {
        return ISM330DHCX_ERROR;
      }
      break;

    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Disable 6D orientation detection.
  * @param  p_obj the device instance object
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_6d_orientation(ism330dhcx_object_t *p_obj)
{
  ism330dhcx_pin_int1_route_t val1;
  ism330dhcx_pin_int2_route_t val2;

  /* Disable 6D orientation event on both INT1 and INT2 pins */
  if (ism330dhcx_pin_int1_route_get(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val1.md1_cfg.int1_6d = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int1_route_set(&(p_obj->ctx), &val1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_pin_int2_route_get(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  val2.md2_cfg.int2_6d = PROPERTY_DISABLE;

  if (ism330dhcx_pin_int2_route_set(&(p_obj->ctx), &val2) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Reset 6D orientation. */
  if (ism330dhcx_6d_threshold_set(&(p_obj->ctx), ISM330DHCX_DEG_80) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set 6D orientation threshold.
  * @param  p_obj the device instance object
  * @param  threshold the 6D orientation detection threshold
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_6d_orientation_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold)
{
  ism330dhcx_sixd_ths_t setting = ISM330DHCX_DEG_80;

  if (threshold > 3U)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check 6D orientation threshold */
  switch (threshold)
  {
    case ISM330DHCX_DEG_80:
      setting = ISM330DHCX_DEG_80; /* threshold 80 degrees */
      break;
    case ISM330DHCX_DEG_70:
      setting = ISM330DHCX_DEG_70; /* threshold 70 degrees */
      break;
    case ISM330DHCX_DEG_60:
      setting = ISM330DHCX_DEG_60; /* threshold 60 degrees */
      break;
    case ISM330DHCX_DEG_50:
      setting = ISM330DHCX_DEG_50; /* threshold 50 degrees */
      break;
    default:
      break;
  }

  if (ism330dhcx_6d_threshold_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of XLow orientation.
  * @param  p_obj the device instance object
  * @param  xlow the pointer where the status of XLow orientation is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_xl(ism330dhcx_object_t *p_obj, uint8_t *xlow)
{
  ism330dhcx_d6d_src_t data;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *xlow = data.xl;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of XHigh orientation.
  * @param  p_obj the device instance object
  * @param  xhigh the pointer where the status of XHigh orientation is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_xh(ism330dhcx_object_t *p_obj, uint8_t *xhigh)
{
  ism330dhcx_d6d_src_t data;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *xhigh = data.xh;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of YLow orientation.
  * @param  p_obj the device instance object
  * @param  ylow the pointer where the status of YLow orientation is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_yl(ism330dhcx_object_t *p_obj, uint8_t *ylow)
{
  ism330dhcx_d6d_src_t data;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *ylow = data.yl;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of YHigh orientation.
  * @param  p_obj the device instance object
  * @param  yhigh the pointer where the status of YHigh orientation is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_yh(ism330dhcx_object_t *p_obj, uint8_t *yhigh)
{
  ism330dhcx_d6d_src_t data;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *yhigh = data.yh;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of ZLow orientation.
  * @param  p_obj the device instance object
  * @param  zlow the pointer where the status of ZLow orientation is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_zl(ism330dhcx_object_t *p_obj, uint8_t *zlow)
{
  ism330dhcx_d6d_src_t data;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *zlow = data.zl;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of ZHigh orientation.
  * @param  p_obj the device instance object
  * @param  zhigh the pointer where the status of ZHigh orientation is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_zh(ism330dhcx_object_t *p_obj, uint8_t *zhigh)
{
  ism330dhcx_d6d_src_t data;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&data, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *zhigh = data.zh;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the status of all hardware events.
  * @param  p_obj the device instance object
  * @param  p_status the pointer where the status of all hardware events is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_event_status(ism330dhcx_object_t *p_obj,
                                                            ism330dhcx_event_status_t *p_status)
{
  ism330dhcx_wake_up_src_t wake_up_src;
  ism330dhcx_tap_src_t tap_src;
  ism330dhcx_d6d_src_t d6d_src;
  ism330dhcx_md1_cfg_t md1_cfg;
  ism330dhcx_md2_cfg_t md2_cfg;
  ism330dhcx_int1_ctrl_t int1_ctrl;

  (void)memset((void *)p_status, 0x0, sizeof(ism330dhcx_event_status_t));

  /* Read all status registers */
  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_WAKE_UP_SRC, (uint8_t *)&wake_up_src, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_TAP_SRC, (uint8_t *)&tap_src, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_D6D_SRC, (uint8_t *)&d6d_src, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_MD1_CFG, (uint8_t *)&md1_cfg, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_MD2_CFG, (uint8_t *)&md2_cfg, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, (uint8_t *)&int1_ctrl, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Check free fall event flag */
  if ((md1_cfg.int1_ff == 1U) || (md2_cfg.int2_ff == 1U))
  {
    if (wake_up_src.ff_ia == 1U)
    {
      p_status->freefallstatus = 1;
    }
  }

  /* Check wake up event flag */
  if ((md1_cfg.int1_wu == 1U) || (md2_cfg.int2_wu == 1U))
  {
    if (wake_up_src.wu_ia == 1U)
    {
      p_status->wakeupstatus = 1;
    }
  }

  /* Check single tap event flag */
  if ((md1_cfg.int1_single_tap == 1U) || (md2_cfg.int2_single_tap == 1U))
  {
    if (tap_src.single_tap == 1U)
    {
      p_status->tapstatus = 1;
    }
  }

  /* Check double tap event flag */
  if ((md1_cfg.int1_double_tap == 1U) || (md2_cfg.int2_double_tap == 1U))
  {
    if (tap_src.double_tap == 1U)
    {
      p_status->doubletapstatus = 1;
    }
  }

  /* Check 6D orientation event flag */
  if ((md1_cfg.int1_6d == 1U) || (md2_cfg.int2_6d == 1U))
  {
    if (d6d_src.d6d_ia == 1U)
    {
      p_status->d6dorientationstatus = 1;
    }
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set ISM330DHCX accelerometer self test.
  * @param  p_obj the device instance object
  * @param  status the value of st_xl in reg CTRL5_C
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_selftest(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_st_xl_t setting = ISM330DHCX_XL_ST_DISABLE;

  /* Verify that the passed parameter contains one of the valid values. */
  switch (status)
  {
    case ISM330DHCX_XL_ST_DISABLE:
      setting = ISM330DHCX_XL_ST_DISABLE; /* self test disable */
      break;
    case ISM330DHCX_XL_ST_POSITIVE:
      setting = ISM330DHCX_XL_ST_POSITIVE; /* self test positive */
      break;
    case ISM330DHCX_XL_ST_NEGATIVE:
      setting = ISM330DHCX_XL_ST_NEGATIVE; /* self test negative */
      break;
    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  if (ret == ISM330DHCX_ERROR)
  {
    return ret;
  }

  if (ism330dhcx_xl_self_test_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ret;
}

/**
  * @brief  Get the ISM330DHCX accelerometer data ready bit value.
  * @param  p_obj the device instance object
  * @param  p_status the pointer where the status of accelerometer data ready bit is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_drdy_status(ism330dhcx_object_t *p_obj, uint8_t *p_status)
{
  if (ism330dhcx_xl_flag_data_ready_get(&(p_obj->ctx), p_status) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX accelerometer initialization status.
  * @param  p_obj the device instance object
  * @param  p_status the pointer where the accelerometer initialization status is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_init_status(ism330dhcx_object_t *p_obj, uint8_t *p_status)
{
  if (p_obj == NULL)
  {
    return ISM330DHCX_ERROR;
  }

  *p_status = p_obj->is_initialized;

  return ISM330DHCX_OK;
}

/**
  * @brief  Set HP filter.
  * @param  p_obj the device instance object
  * @param  cutoff the cut-off frequency
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_hp_filter(ism330dhcx_object_t *p_obj, uint8_t cutoff)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_hp_slope_xl_en_t setting = ISM330DHCX_HP_PATH_DISABLE_ON_OUT;

  /* Verify that the passed parameter contains one of the valid values. */
  switch (cutoff)
  {
    case ISM330DHCX_HP_PATH_DISABLE_ON_OUT:
      setting = ISM330DHCX_HP_PATH_DISABLE_ON_OUT; /* high pass filter disable */
      break;
    case ISM330DHCX_SLOPE_ODR_DIV_4:
      setting = ISM330DHCX_SLOPE_ODR_DIV_4; /* slope filter bandwidth ODR/4 */
      break;
    case ISM330DHCX_HP_ODR_DIV_10:
      setting = ISM330DHCX_HP_ODR_DIV_10; /* high pass filter bandwidth ODR/10 */
      break;
    case ISM330DHCX_HP_ODR_DIV_20:
      setting = ISM330DHCX_HP_ODR_DIV_20; /* high pass filter bandwidth ODR/20 */
      break;
    case ISM330DHCX_HP_ODR_DIV_45:
      setting = ISM330DHCX_HP_ODR_DIV_45; /* high pass filter bandwidth ODR/45 */
      break;
    case ISM330DHCX_HP_ODR_DIV_100:
      setting = ISM330DHCX_HP_ODR_DIV_100; /* high pass filter bandwidth ODR/100 */
      break;
    case ISM330DHCX_HP_ODR_DIV_200:
      setting = ISM330DHCX_HP_ODR_DIV_200; /* high pass filter bandwidth ODR/200 */
      break;
    case ISM330DHCX_HP_ODR_DIV_400:
      setting = ISM330DHCX_HP_ODR_DIV_400; /* high pass filter bandwidth ODR/400 */
      break;
    case ISM330DHCX_HP_ODR_DIV_800:
      setting = ISM330DHCX_HP_ODR_DIV_800; /* high pass filter bandwidth ODR/800 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_10:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_10; /* high pass filter reference mode bandwidth ODR/10 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_20:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_20; /* high pass filter reference mode bandwidth ODR/20 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_45:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_45; /* high pass filter reference mode bandwidth ODR/45 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_100:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_100; /* high pass filter reference mode bandwidth ODR/100 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_200:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_200; /* high pass filter reference mode bandwidth ODR/200 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_400:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_400; /* high pass filter reference mode bandwidth ODR/400 */
      break;
    case ISM330DHCX_HP_REF_MD_ODR_DIV_800:
      setting = ISM330DHCX_HP_REF_MD_ODR_DIV_800; /* high pass filter reference mode bandwidth ODR/800 */
      break;
    case ISM330DHCX_LP_ODR_DIV_10:
      setting = ISM330DHCX_LP_ODR_DIV_10; /* low pass filter bandwidth ODR/10 */
      break;
    case ISM330DHCX_LP_ODR_DIV_20:
      setting = ISM330DHCX_LP_ODR_DIV_20; /* low pass filter bandwidth ODR/20 */
      break;
    case ISM330DHCX_LP_ODR_DIV_45:
      setting = ISM330DHCX_LP_ODR_DIV_45; /* low pass filter bandwidth ODR/45 */
      break;
    case ISM330DHCX_LP_ODR_DIV_100:
      setting = ISM330DHCX_LP_ODR_DIV_100; /* low pass filter bandwidth ODR/100 */
      break;
    case ISM330DHCX_LP_ODR_DIV_200:
      setting = ISM330DHCX_LP_ODR_DIV_200; /* low pass filter bandwidth ODR/200 */
      break;
    case ISM330DHCX_LP_ODR_DIV_400:
      setting = ISM330DHCX_LP_ODR_DIV_400; /* low pass filter bandwidth ODR/400 */
      break;
    case ISM330DHCX_LP_ODR_DIV_800:
      setting = ISM330DHCX_LP_ODR_DIV_800; /* low pass filter bandwidth ODR/800 */
      break;
    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  if (ret == ISM330DHCX_ERROR)
  {
    return ret;
  }

  if (ism330dhcx_xl_hp_path_on_out_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ret;
}

/**
  * @brief  Set ISM330DHCX gyroscope self test.
  * @param  p_obj the device instance object
  * @param  status the value of st_g in reg CTRL5_C
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_selftest(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_st_g_t setting = ISM330DHCX_GY_ST_DISABLE;

  /* Verify that the passed parameter contains one of the valid values. */
  switch (status)
  {
    case ISM330DHCX_GY_ST_DISABLE:
      setting = ISM330DHCX_GY_ST_DISABLE; /* self test disable */
      break;
    case ISM330DHCX_GY_ST_POSITIVE:
      setting = ISM330DHCX_GY_ST_POSITIVE; /* self test positive */
      break;
    case ISM330DHCX_GY_ST_NEGATIVE:
      setting = ISM330DHCX_GY_ST_NEGATIVE; /* self test negative */
      break;
    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  if (ret == ISM330DHCX_ERROR)
  {
    return ret;
  }

  if (ism330dhcx_gy_self_test_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ret;
}

/**
  * @brief  Get the ISM330DHCX gyroscope data ready bit value.
  * @param  p_obj the device instance object
  * @param  p_status the pointer where the status of gyroscope data ready bit is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_drdy_status(ism330dhcx_object_t *p_obj, uint8_t *p_status)
{
  if (ism330dhcx_gy_flag_data_ready_get(&(p_obj->ctx), p_status) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX gyroscope initialization status.
  * @param  p_obj the device instance object
  * @param  p_status the pointer where the gyroscope initialization status is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_init_status(ism330dhcx_object_t *p_obj, uint8_t *p_status)
{
  if (p_obj == NULL)
  {
    return ISM330DHCX_ERROR;
  }

  *p_status = p_obj->is_initialized;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX FIFO number of samples.
  * @param  p_obj the device instance object
  * @param  p_numsamples the pointer where the FIFO number of samples is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_num_samples(ism330dhcx_object_t *p_obj, uint16_t *p_numsamples)
{
  if (ism330dhcx_fifo_data_level_get(&(p_obj->ctx), p_numsamples) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX FIFO full status.
  * @param  p_obj the device instance object
  * @param  p_status the pointer where the FIFO full status is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_full_status(ism330dhcx_object_t *p_obj, uint8_t *p_status)
{
  ism330dhcx_reg_t reg;

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_FIFO_STATUS2, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *p_status = reg.fifo_status2.fifo_full_ia;

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO accelerometer BDR value.
  * @param  p_obj the device instance object
  * @param  bdr the FIFO accelerometer batched data rate value to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_acc_set_bdr(ism330dhcx_object_t *p_obj, float_t bdr)
{
  ism330dhcx_bdr_xl_t new_odr;

  new_odr = (bdr <=   12.5f) ? ISM330DHCX_XL_BATCHED_AT_12Hz5
            : (bdr <=   26.0f) ? ISM330DHCX_XL_BATCHED_AT_26Hz
            : (bdr <=   52.0f) ? ISM330DHCX_XL_BATCHED_AT_52Hz
            : (bdr <=  104.0f) ? ISM330DHCX_XL_BATCHED_AT_104Hz
            : (bdr <=  208.0f) ? ISM330DHCX_XL_BATCHED_AT_208Hz
            : (bdr <=  417.0f) ? ISM330DHCX_XL_BATCHED_AT_417Hz
            : (bdr <=  833.0f) ? ISM330DHCX_XL_BATCHED_AT_833Hz
            : (bdr <= 1667.0f) ? ISM330DHCX_XL_BATCHED_AT_1667Hz
            : (bdr <= 3333.0f) ? ISM330DHCX_XL_BATCHED_AT_3333Hz
            :                    ISM330DHCX_XL_BATCHED_AT_6667Hz;

  if (ism330dhcx_fifo_xl_batch_set(&(p_obj->ctx), new_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO gyroscope BDR value.
  * @param  p_obj the device instance object
  * @param  bdr the FIFO gyroscope batched data rate value to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_gyro_set_bdr(ism330dhcx_object_t *p_obj, float_t bdr)
{
  ism330dhcx_bdr_gy_t new_odr;

  new_odr = (bdr <=   12.5f) ? ISM330DHCX_GY_BATCHED_AT_12Hz5
            : (bdr <=   26.0f) ? ISM330DHCX_GY_BATCHED_AT_26Hz
            : (bdr <=   52.0f) ? ISM330DHCX_GY_BATCHED_AT_52Hz
            : (bdr <=  104.0f) ? ISM330DHCX_GY_BATCHED_AT_104Hz
            : (bdr <=  208.0f) ? ISM330DHCX_GY_BATCHED_AT_208Hz
            : (bdr <=  417.0f) ? ISM330DHCX_GY_BATCHED_AT_417Hz
            : (bdr <=  833.0f) ? ISM330DHCX_GY_BATCHED_AT_833Hz
            : (bdr <= 1667.0f) ? ISM330DHCX_GY_BATCHED_AT_1667Hz
            : (bdr <= 3333.0f) ? ISM330DHCX_GY_BATCHED_AT_3333Hz
            :                    ISM330DHCX_GY_BATCHED_AT_6667Hz;

  if (ism330dhcx_fifo_gy_batch_set(&(p_obj->ctx), new_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO full interrupt on INT1 pin.
  * @param  p_obj the device instance object
  * @param  status the status of FIFO full interrupt on INT1 pin to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_int1_fifo_full(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_reg_t reg;

  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  reg.int1_ctrl.int1_fifo_full = status;

  if (ism330dhcx_write_reg(&(p_obj->ctx), ISM330DHCX_INT1_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO full interrupt on INT2 pin.
  * @param  p_obj the device instance object
  * @param  status the status of FIFO full interrupt on INT2 pin to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_int2_fifo_full(ism330dhcx_object_t *p_obj, uint8_t status)
{
  ism330dhcx_reg_t reg;

  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_read_reg(&(p_obj->ctx), ISM330DHCX_INT2_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  reg.int2_ctrl.int2_fifo_full = status;

  if (ism330dhcx_write_reg(&(p_obj->ctx), ISM330DHCX_INT2_CTRL, &reg.byte, 1) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO watermark level.
  * @param  p_obj the device instance object
  * @param  watermark the FIFO watermark level to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_watermark_level(ism330dhcx_object_t *p_obj, uint16_t watermark)
{
  if (watermark > 0x1FFu)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_fifo_watermark_set(&(p_obj->ctx), watermark) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO stop on watermark.
  * @param  p_obj the device instance object
  * @param  status the status of FIFO stop on watermark to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_stop_on_fth(ism330dhcx_object_t *p_obj, uint8_t status)
{
  if (status > 1U)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_fifo_stop_on_wtm_set(&(p_obj->ctx), status) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX FIFO mode.
  * @param  p_obj the device instance object
  * @param  mode the FIFO mode to be set
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_mode(ism330dhcx_object_t *p_obj, uint8_t mode)
{
  ism330dhcx_drv_status_t ret = ISM330DHCX_OK;
  ism330dhcx_fifo_mode_t setting = ISM330DHCX_BYPASS_MODE;

  /* Verify that the passed parameter contains one of the valid values. */
  switch (mode)
  {
    case ISM330DHCX_BYPASS_MODE:
      setting = ISM330DHCX_BYPASS_MODE; /* bypass mode */
      break;
    case ISM330DHCX_FIFO_MODE:
      setting = ISM330DHCX_FIFO_MODE; /* fifo mode */
      break;
    case ISM330DHCX_STREAM_TO_FIFO_MODE:
      setting = ISM330DHCX_STREAM_TO_FIFO_MODE; /* continuous to fifo mode */
      break;
    case ISM330DHCX_BYPASS_TO_STREAM_MODE:
      setting = ISM330DHCX_BYPASS_TO_STREAM_MODE; /* bypass to continuous mode */
      break;
    case ISM330DHCX_STREAM_MODE:
      setting = ISM330DHCX_STREAM_MODE; /* continuous mode */
      break;
    case ISM330DHCX_BYPASS_TO_FIFO_MODE:
      setting = ISM330DHCX_BYPASS_TO_FIFO_MODE; /* bypass to fifo mode */
      break;
    default:
      ret = ISM330DHCX_ERROR;
      break;
  }

  if (ret == ISM330DHCX_ERROR)
  {
    return ret;
  }

  if (ism330dhcx_fifo_mode_set(&(p_obj->ctx), setting) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ret;
}

/**
  * @brief  Get the ISM330DHCX FIFO tag.
  * @param  p_obj the device instance object
  * @param  tag the pointer where the FIFO tag is written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_tag(ism330dhcx_object_t *p_obj, uint8_t *tag)
{
  ism330dhcx_fifo_tag_t tag_local;

  if (ism330dhcx_fifo_sensor_tag_get(&(p_obj->ctx), &tag_local) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  *tag = (uint8_t)tag_local;

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX FIFO raw data.
  * @param  p_obj the device instance object
  * @param  p_data the pointer where FIFO raw data are written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_data(ism330dhcx_object_t *p_obj, uint8_t *p_data)
{
  if (ism330dhcx_fifo_out_raw_get(&(p_obj->ctx), p_data) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX FIFO accelerometer single sample (16-bit data per 3 axes) and calculate acceleration.
  * @param  p_obj the device instance object
  * @param  p_acceleration the pointer where FIFO accelerometer axes are written [unit is mg]
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_acc_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_acceleration)
{
  ism330dhcx_axis3bit16_t data;
  float_t sensitivity = 0.0f;

  if (ism330dhcx_drv_fifo_get_data(p_obj, data.u8bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_drv_acc_get_sensitivity(p_obj, &sensitivity) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  p_acceleration->x_axis = (int32_t)((float_t)((float_t)data.i16bit[0] * sensitivity));
  p_acceleration->y_axis = (int32_t)((float_t)((float_t)data.i16bit[1] * sensitivity));
  p_acceleration->z_axis = (int32_t)((float_t)((float_t)data.i16bit[2] * sensitivity));

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX FIFO gyroscope single sample (16-bit data per 3 axes) and calculate angular rate.
  * @param  p_obj the device instance object
  * @param  p_angularrate the pointer where FIFO gyroscope axes are written [unit is mdps]
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_gyro_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_angularrate)
{
  ism330dhcx_axis3bit16_t data;
  float_t sensitivity = 0.0f;

  if (ism330dhcx_drv_fifo_get_data(p_obj, data.u8bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  if (ism330dhcx_drv_gyro_get_sensitivity(p_obj, &sensitivity) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  /* Calculate the data. */
  p_angularrate->x_axis = (int32_t)((float_t)((float_t)data.i16bit[0] * sensitivity));
  p_angularrate->y_axis = (int32_t)((float_t)((float_t)data.i16bit[1] * sensitivity));
  p_angularrate->z_axis = (int32_t)((float_t)((float_t)data.i16bit[2] * sensitivity));

  return ISM330DHCX_OK;
}

/**
  * @brief  Get the ISM330DHCX FIFO single word (16-bit data).
  * @param  p_obj the device instance object
  * @param  p_data_raw the pointer where FIFO single data are written
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_data_word(ism330dhcx_object_t *p_obj, int16_t *p_data_raw)
{
  ism330dhcx_axis3bit16_t data;

  if (ism330dhcx_drv_fifo_get_data(p_obj, data.u8bit) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  p_data_raw[0] = data.i16bit[0];
  p_data_raw[1] = data.i16bit[1];
  p_data_raw[2] = data.i16bit[2];

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX accelerometer sensor output data rate when the sensor is enabled.
  * @param  p_obj the device instance object
  * @param  odr the output data rate to be set
  * @retval 0 in case of success, an error code otherwise
  */
static ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_outputdatarate_when_enabled(ism330dhcx_object_t *p_obj,
    float_t odr)
{
  ism330dhcx_odr_xl_t new_odr;

  new_odr = (odr <=   ISM330DHCX_ACC_ODR_12HZ5_FP) ? ISM330DHCX_XL_ODR_12Hz5
            : (odr <=   ISM330DHCX_ACC_ODR_26HZ_FP) ? ISM330DHCX_XL_ODR_26Hz
            : (odr <=   ISM330DHCX_ACC_ODR_52HZ_FP) ? ISM330DHCX_XL_ODR_52Hz
            : (odr <=  ISM330DHCX_ACC_ODR_104HZ_FP) ? ISM330DHCX_XL_ODR_104Hz
            : (odr <=  ISM330DHCX_ACC_ODR_208HZ_FP) ? ISM330DHCX_XL_ODR_208Hz
            : (odr <=  ISM330DHCX_ACC_ODR_416HZ_FP) ? ISM330DHCX_XL_ODR_416Hz
            : (odr <=  ISM330DHCX_ACC_ODR_833HZ_FP) ? ISM330DHCX_XL_ODR_833Hz
            : (odr <= ISM330DHCX_ACC_ODR_1666HZ_FP) ? ISM330DHCX_XL_ODR_1666Hz
            : (odr <= ISM330DHCX_ACC_ODR_3332HZ_FP) ? ISM330DHCX_XL_ODR_3332Hz
            :                    ISM330DHCX_XL_ODR_6667Hz;

  /* Output data rate selection. */
  if (ism330dhcx_xl_data_rate_set(&(p_obj->ctx), new_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX accelerometer sensor output data rate when the sensor is disabled.
  * @param  p_obj the device instance object
  * @param  odr the output data rate to be set
  * @retval 0 in case of success, an error code otherwise
  */
static ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_outputdatarate_when_disabled(ism330dhcx_object_t *p_obj,
    float_t odr)
{
  p_obj->acc_odr = (odr <=   ISM330DHCX_ACC_ODR_12HZ5_FP) ? ISM330DHCX_XL_ODR_12Hz5
                   : (odr <=   ISM330DHCX_ACC_ODR_26HZ_FP) ? ISM330DHCX_XL_ODR_26Hz
                   : (odr <=   ISM330DHCX_ACC_ODR_52HZ_FP) ? ISM330DHCX_XL_ODR_52Hz
                   : (odr <=  ISM330DHCX_ACC_ODR_104HZ_FP) ? ISM330DHCX_XL_ODR_104Hz
                   : (odr <=  ISM330DHCX_ACC_ODR_208HZ_FP) ? ISM330DHCX_XL_ODR_208Hz
                   : (odr <=  ISM330DHCX_ACC_ODR_416HZ_FP) ? ISM330DHCX_XL_ODR_416Hz
                   : (odr <=  ISM330DHCX_ACC_ODR_833HZ_FP) ? ISM330DHCX_XL_ODR_833Hz
                   : (odr <= ISM330DHCX_ACC_ODR_1666HZ_FP) ? ISM330DHCX_XL_ODR_1666Hz
                   : (odr <= ISM330DHCX_ACC_ODR_3332HZ_FP) ? ISM330DHCX_XL_ODR_3332Hz
                   :                    ISM330DHCX_XL_ODR_6667Hz;

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX gyroscope sensor output data rate when the sensor is enabled.
  * @param  p_obj the device instance object
  * @param  odr the output data rate to be set
  * @retval 0 in case of success, an error code otherwise
  */
static ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_outputdatarate_when_enabled(ism330dhcx_object_t *p_obj,
    float_t odr)
{
  ism330dhcx_odr_g_t new_odr;

  new_odr = (odr <=   ISM330DHCX_GYRO_ODR_12HZ5_FP) ? ISM330DHCX_GY_ODR_12Hz5
            : (odr <=   ISM330DHCX_GYRO_ODR_26HZ_FP) ? ISM330DHCX_GY_ODR_26Hz
            : (odr <=   ISM330DHCX_GYRO_ODR_52HZ_FP) ? ISM330DHCX_GY_ODR_52Hz
            : (odr <=  ISM330DHCX_GYRO_ODR_104HZ_FP) ? ISM330DHCX_GY_ODR_104Hz
            : (odr <=  ISM330DHCX_GYRO_ODR_208HZ_FP) ? ISM330DHCX_GY_ODR_208Hz
            : (odr <=  ISM330DHCX_GYRO_ODR_416HZ_FP) ? ISM330DHCX_GY_ODR_416Hz
            : (odr <=  ISM330DHCX_GYRO_ODR_833HZ_FP) ? ISM330DHCX_GY_ODR_833Hz
            : (odr <= ISM330DHCX_GYRO_ODR_1666HZ_FP) ? ISM330DHCX_GY_ODR_1666Hz
            : (odr <= ISM330DHCX_GYRO_ODR_3332HZ_FP) ? ISM330DHCX_GY_ODR_3332Hz
            :                    ISM330DHCX_GY_ODR_6667Hz;

  /* Output data rate selection. */
  if (ism330dhcx_gy_data_rate_set(&(p_obj->ctx), new_odr) != ISM330DHCX_OK)
  {
    return ISM330DHCX_ERROR;
  }

  return ISM330DHCX_OK;
}

/**
  * @brief  Set the ISM330DHCX gyroscope sensor output data rate when the sensor is disabled.
  * @param  p_obj the device instance object
  * @param  odr the output data rate to be set
  * @retval 0 in case of success, an error code otherwise
  */
static ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_outputdatarate_when_disabled(ism330dhcx_object_t *p_obj,
    float_t odr)
{
  p_obj->gyro_odr = (odr <=   ISM330DHCX_GYRO_ODR_12HZ5_FP) ? ISM330DHCX_GY_ODR_12Hz5
                    : (odr <=   ISM330DHCX_GYRO_ODR_26HZ_FP) ? ISM330DHCX_GY_ODR_26Hz
                    : (odr <=   ISM330DHCX_GYRO_ODR_52HZ_FP) ? ISM330DHCX_GY_ODR_52Hz
                    : (odr <=  ISM330DHCX_GYRO_ODR_104HZ_FP) ? ISM330DHCX_GY_ODR_104Hz
                    : (odr <=  ISM330DHCX_GYRO_ODR_208HZ_FP) ? ISM330DHCX_GY_ODR_208Hz
                    : (odr <=  ISM330DHCX_GYRO_ODR_416HZ_FP) ? ISM330DHCX_GY_ODR_416Hz
                    : (odr <=  ISM330DHCX_GYRO_ODR_833HZ_FP) ? ISM330DHCX_GY_ODR_833Hz
                    : (odr <= ISM330DHCX_GYRO_ODR_1666HZ_FP) ? ISM330DHCX_GY_ODR_1666Hz
                    : (odr <= ISM330DHCX_GYRO_ODR_3332HZ_FP) ? ISM330DHCX_GY_ODR_3332Hz
                    :                    ISM330DHCX_GY_ODR_6667Hz;

  return ISM330DHCX_OK;
}

/**
  * @brief  Enable the interrupt.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_enable_irq(ism330dhcx_object_t *p_obj, ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret_val = ISM330DHCX_OK;
  if (intpin == ISM330DHCX_INT1_PIN)
  {
    if (ism330dhcx_io_enable_irq(&(p_obj->pio), ISM330DHCX_IO_INT1_PIN) != 0)
    {
      ret_val = ISM330DHCX_ERROR;
    }
  }

  if (intpin == ISM330DHCX_INT2_PIN)
  {
    if (ism330dhcx_io_enable_irq(&(p_obj->pio), ISM330DHCX_IO_INT2_PIN) != 0)
    {
      ret_val = ISM330DHCX_ERROR;
    }
  }

  return ret_val;
}

/**
  * @brief  Disable the interrupt.
  * @param  p_obj the device instance object
  * @param  intpin the interrupt pin
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_disable_irq(ism330dhcx_object_t *p_obj, ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret_val = ISM330DHCX_OK;
  if (intpin == ISM330DHCX_INT1_PIN)
  {
    if (ism330dhcx_io_disable_irq(&(p_obj->pio), ISM330DHCX_IO_INT1_PIN) != 0)
    {
      ret_val = ISM330DHCX_ERROR;
    }
  }

  if (intpin == ISM330DHCX_INT2_PIN)
  {
    if (ism330dhcx_io_disable_irq(&(p_obj->pio), ISM330DHCX_IO_INT2_PIN) != 0)
    {
      ret_val = ISM330DHCX_ERROR;
    }
  }

  return ret_val;
}

#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)

static void ism330dhcx_io_callback1(ism330dhcx_io_t *pio, void *arg)
{
  (void)pio;
  ism330dhcx_object_t *pobj = (ism330dhcx_object_t *)arg;

  pobj->cb_ctx_irq1.callback(pobj, pobj->cb_ctx_irq1.arg);
}

static void ism330dhcx_io_callback2(ism330dhcx_io_t *pio, void *arg)
{
  (void)pio;
  ism330dhcx_object_t *pobj = (ism330dhcx_object_t *)arg;

  pobj->cb_ctx_irq2.callback(pobj, pobj->cb_ctx_irq2.arg);
}

/**
  * @brief  Register the callback function for interrupt events.
  * @param  p_obj the device instance object
  * @param  cb the callback function
  * @param  arg the optional argument given to the callback
  * @param  intpin the interrupt pin
  * @retval 0 in case of success, an error code otherwise
  */
ism330dhcx_drv_status_t ism330dhcx_drv_register_callback(ism330dhcx_object_t *p_obj, ism330dhcx_callback_t cb,
                                                         void *arg,
                                                         ism330dhcx_sensorintpin_t intpin)
{
  ism330dhcx_drv_status_t ret_val = ISM330DHCX_ERROR;
  if (intpin == ISM330DHCX_INT1_PIN)
  {
    p_obj->cb_ctx_irq1.callback = cb;
    p_obj->cb_ctx_irq1.arg = arg;

    if (ism330dhcx_io_register_callback(&(p_obj->pio),
                                        ism330dhcx_io_callback1,
                                        (void *)p_obj,
                                        ISM330DHCX_IO_INT1_PIN) == 0)
    {
      ret_val = ISM330DHCX_OK;
    }
  }

  if (intpin == ISM330DHCX_INT2_PIN)
  {
    p_obj->cb_ctx_irq2.callback = cb;
    p_obj->cb_ctx_irq2.arg = arg;

    if (ism330dhcx_io_register_callback(&(p_obj->pio),
                                        ism330dhcx_io_callback2,
                                        (void *)p_obj,
                                        ISM330DHCX_IO_INT2_PIN) == 0)
    {
      ret_val = ISM330DHCX_OK;
    }
  }

  return ret_val;
}
#endif /* ISM330DHCX_CALLBACKS */
