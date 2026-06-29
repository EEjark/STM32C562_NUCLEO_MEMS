/**
  ******************************************************************************
  * @file    ism330dhcx.h
  * @brief   ISM330DHCX header driver file
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

#ifndef ISM330DHCX_H
#define ISM330DHCX_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ism330dhcx_reg.h"
#include "ism330dhcx_io.h"

typedef struct ism330dhcx_object_s ism330dhcx_object_t;   /*!< ISM330DHCX object structure type */

#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
/*!< callback function pointer definition */
typedef void (*ism330dhcx_callback_t)(ism330dhcx_object_t *pobj, void *arg);

/**
  * @brief Callback context structure.
  */
typedef struct
{
  ism330dhcx_callback_t callback;   /*!< Callback function to register           */
  void *arg;                     /*!< Optional argument given to the callback */
} ism330dhcx_cb_ctx_t;

#endif /* ISM330DHCX_CALLBACKS */

/**
  * @brief ISM330DHCX interrupt structure.
  */
typedef enum
{
  ISM330DHCX_INT1_PIN,
  ISM330DHCX_INT2_PIN,
} ism330dhcx_sensorintpin_t;

/**
  * @brief ISM330DHCX raw axes structure.
  */
typedef struct
{
  int16_t x_axis;
  int16_t y_axis;
  int16_t z_axis;
} ism330dhcx_axesraw_t;

/**
  * @brief ISM330DHCX axes structure.
  */
typedef struct
{
  int32_t x_axis;
  int32_t y_axis;
  int32_t z_axis;
} ism330dhcx_axes_t;

/**
  * @brief ISM330DHCX hardware events structure.
  */
typedef struct
{
  uint32_t freefallstatus : 1;
  uint32_t tapstatus : 1;
  uint32_t doubletapstatus : 1;
  uint32_t wakeupstatus : 1;
  uint32_t stepstatus : 1;
  uint32_t tiltstatus : 1;
  uint32_t d6dorientationstatus : 1;
  uint32_t sleepstatus : 1;
} ism330dhcx_event_status_t;

/**
  * @brief ISM330DHCX int16_t array and uint8_t array union.
  */
typedef union
{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} ism330dhcx_axis3bit16_t;

/**
  * @brief ISM330DHCX int16_t and uint8_t array union.
  */
typedef union
{
  int16_t i16bit;
  uint8_t u8bit[2];
} ism330dhcx_axis1bit16_t;

/**
  * @brief ISM330DHCX int32_t array and uint8_t array union.
  */
typedef union
{
  int32_t i32bit[3];
  uint8_t u8bit[12];
} ism330dhcx_axis3bit32_t;

/**
  * @brief ISM330DHCX int32_t and uint8_t array union.
  */
typedef union
{
  int32_t i32bit;
  uint8_t u8bit[4];
} ism330dhcx_axis1bit32_t;

/**
  * @brief ISM330DHCX object structure.
  */
struct ism330dhcx_object_s
{
  ism330dhcx_io_t     pio;                 /*!< IO                               */
  stmdev_ctx_t        ctx;                 /*!< ctx                              */
  uint8_t             is_initialized;      /*!< is_initialized                   */
  uint8_t             acc_is_enabled;      /*!< accelerometer is enabled         */
  uint8_t             gyro_is_enabled;     /*!< gyroscope is enabled             */
  ism330dhcx_odr_xl_t acc_odr;             /*!< accelerometer odr                */
  ism330dhcx_odr_g_t  gyro_odr;            /*!< gyroscope odr                    */
#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
  ism330dhcx_cb_ctx_t    cb_ctx_irq1;      /*!< callback context for INT1 events */
  ism330dhcx_cb_ctx_t    cb_ctx_irq2;      /*!< callback context for INT2 events */
#endif /* ISM330DHCX_CALLBACKS */
};

/**
  * @brief ISM330DHCX status.
  */
typedef enum
{
  ISM330DHCX_OK = 0,
  ISM330DHCX_ERROR = -1,
} ism330dhcx_drv_status_t;

/* ISM330DHCX BUS type */
#define ISM330DHCX_I2C_BUS                 0U     /*!< ISM330DHCX I2C Bus            */
#define ISM330DHCX_SPI_4WIRES_BUS          1U     /*!< ISM330DHCX SPI 4-Wires Bus    */
#define ISM330DHCX_SPI_3WIRES_BUS          2U     /*!< ISM330DHCX SPI 3-Wires Bus    */
#define ISM330DHCX_I3C_BUS                 3U     /*!< ISM330DHCX I3C Bus            */

/* ISM330DHCX accelerometer sensitivity */
#define ISM330DHCX_ACC_SENSITIVITY_FS_2G   0.061f  /*!< ISM330DHCX 2g sensitivity          */
#define ISM330DHCX_ACC_SENSITIVITY_FS_4G   0.122f  /*!< ISM330DHCX 4g sensitivity          */
#define ISM330DHCX_ACC_SENSITIVITY_FS_8G   0.244f  /*!< ISM330DHCX 8g sensitivity          */
#define ISM330DHCX_ACC_SENSITIVITY_FS_16G  0.488f  /*!< ISM330DHCX 16g sensitivity         */

/* ISM330DHCX gyroscope sensitivity */
#define ISM330DHCX_GYRO_SENSITIVITY_FS_125DPS    4.375f  /*!< ISM330DHCX 125 dps sensitivity          */
#define ISM330DHCX_GYRO_SENSITIVITY_FS_250DPS    8.750f  /*!< ISM330DHCX 250 dps sensitivity          */
#define ISM330DHCX_GYRO_SENSITIVITY_FS_500DPS   17.500f  /*!< ISM330DHCX 500 dps sensitivity          */
#define ISM330DHCX_GYRO_SENSITIVITY_FS_1000DPS  35.000f  /*!< ISM330DHCX 1000 dps sensitivity         */
#define ISM330DHCX_GYRO_SENSITIVITY_FS_2000DPS  70.000f  /*!< ISM330DHCX 2000 dps sensitivity         */
#define ISM330DHCX_GYRO_SENSITIVITY_FS_4000DPS 140.000f  /*!< ISM330DHCX 4000 dps sensitivity         */

/* ISM330DHCX accelerometer output data rate values */
#define ISM330DHCX_ACC_ODR_12HZ5_FP 12.5f     /*!< ISM330DHCX Accelerometer ODR 12.5 Hz        */
#define ISM330DHCX_ACC_ODR_26HZ_FP 26.0f      /*!< ISM330DHCX Accelerometer ODR 26 Hz          */
#define ISM330DHCX_ACC_ODR_52HZ_FP 52.0f      /*!< ISM330DHCX Accelerometer ODR 52 Hz          */
#define ISM330DHCX_ACC_ODR_104HZ_FP 104.0f    /*!< ISM330DHCX Accelerometer ODR 104 Hz         */
#define ISM330DHCX_ACC_ODR_208HZ_FP 208.0f    /*!< ISM330DHCX Accelerometer ODR 208 Hz         */
#define ISM330DHCX_ACC_ODR_416HZ_FP 416.0f    /*!< ISM330DHCX Accelerometer ODR 416 Hz         */
#define ISM330DHCX_ACC_ODR_833HZ_FP 833.0f    /*!< ISM330DHCX Accelerometer ODR 833 Hz         */
#define ISM330DHCX_ACC_ODR_1666HZ_FP 1666.0f  /*!< ISM330DHCX Accelerometer ODR 1666 Hz        */
#define ISM330DHCX_ACC_ODR_3332HZ_FP 3332.0f  /*!< ISM330DHCX Accelerometer ODR 3332 Hz        */
#define ISM330DHCX_ACC_ODR_6667HZ_FP 6667.0f  /*!< ISM330DHCX Accelerometer ODR 6667 Hz        */

/* ISM330DHCX accelerometer full scale values */
#define ISM330DHCX_ACC_FS_2G_FP 2.0f    /*!< ISM330DHCX Accelerometer full scale 2g        */
#define ISM330DHCX_ACC_FS_4G_FP 4.0f    /*!< ISM330DHCX Accelerometer full scale 4g        */
#define ISM330DHCX_ACC_FS_8G_FP 8.0f    /*!< ISM330DHCX Accelerometer full scale 8g        */
#define ISM330DHCX_ACC_FS_16G_FP 16.0f  /*!< ISM330DHCX Accelerometer full scale 16g       */

/* ISM330DHCX gyroscope output data rate values */
#define ISM330DHCX_GYRO_ODR_12HZ5_FP 12.5f     /*!< ISM330DHCX Gyroscope ODR 12.5 Hz        */
#define ISM330DHCX_GYRO_ODR_26HZ_FP 26.0f      /*!< ISM330DHCX Gyroscope ODR 26 Hz          */
#define ISM330DHCX_GYRO_ODR_52HZ_FP 52.0f      /*!< ISM330DHCX Gyroscope ODR 52 Hz          */
#define ISM330DHCX_GYRO_ODR_104HZ_FP 104.0f    /*!< ISM330DHCX Gyroscope ODR 104 Hz         */
#define ISM330DHCX_GYRO_ODR_208HZ_FP 208.0f    /*!< ISM330DHCX Gyroscope ODR 208 Hz         */
#define ISM330DHCX_GYRO_ODR_416HZ_FP 416.0f    /*!< ISM330DHCX Gyroscope ODR 416 Hz         */
#define ISM330DHCX_GYRO_ODR_833HZ_FP 833.0f    /*!< ISM330DHCX Gyroscope ODR 833 Hz         */
#define ISM330DHCX_GYRO_ODR_1666HZ_FP 1666.0f  /*!< ISM330DHCX Gyroscope ODR 1666 Hz        */
#define ISM330DHCX_GYRO_ODR_3332HZ_FP 3332.0f  /*!< ISM330DHCX Gyroscope ODR 3332 Hz        */
#define ISM330DHCX_GYRO_ODR_6667HZ_FP 6667.0f  /*!< ISM330DHCX Gyroscope ODR 6667 Hz        */

/* ISM330DHCX gyroscope full scale values */
#define ISM330DHCX_GYRO_FS_125DPS_FP 125.0f    /*!< ISM330DHCX Gyroscope full scale 125 dps         */
#define ISM330DHCX_GYRO_FS_250DPS_FP 250.0f    /*!< ISM330DHCX Gyroscope full scale 250 dps         */
#define ISM330DHCX_GYRO_FS_500DPS_FP 500.0f    /*!< ISM330DHCX Gyroscope full scale 500 dps         */
#define ISM330DHCX_GYRO_FS_1000DPS_FP 1000.0f  /*!< ISM330DHCX Gyroscope full scale 1000 dps        */
#define ISM330DHCX_GYRO_FS_2000DPS_FP 2000.0f  /*!< ISM330DHCX Gyroscope full scale 2000 dps        */
#define ISM330DHCX_GYRO_FS_4000DPS_FP 4000.0f  /*!< ISM330DHCX Gyroscope full scale 4000 dps        */

ism330dhcx_drv_status_t ism330dhcx_drv_init(ism330dhcx_object_t *p_obj, uint32_t id);
ism330dhcx_drv_status_t ism330dhcx_drv_deinit(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_readid(ism330dhcx_object_t *p_obj, uint8_t *p_id);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_sensitivity(ism330dhcx_object_t *p_obj, float_t *p_sensitivity);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_outputdatarate(ism330dhcx_object_t *p_obj, float_t *p_odr);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_outputdatarate(ism330dhcx_object_t *p_obj, float_t odr);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_fullscale(ism330dhcx_object_t *p_obj, float_t *p_fullscale);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_fullscale(ism330dhcx_object_t *p_obj, float_t fullscale);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_axesraw(ism330dhcx_object_t *p_obj, ism330dhcx_axesraw_t *p_value);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_acceleration);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_enable(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_disable(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_sensitivity(ism330dhcx_object_t *p_obj, float_t *p_sensitivity);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_outputdatarate(ism330dhcx_object_t *p_obj, float_t *p_odr);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_outputdatarate(ism330dhcx_object_t *p_obj, float_t odr);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_fullscale(ism330dhcx_object_t *p_obj, float_t *p_fullscale);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_fullscale(ism330dhcx_object_t *p_obj, float_t fullscale);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_axesraw(ism330dhcx_object_t *p_obj, ism330dhcx_axesraw_t *p_value);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_angularrate);
ism330dhcx_drv_status_t ism330dhcx_drv_read_reg(ism330dhcx_object_t *p_obj, uint8_t reg, uint8_t *p_data);
ism330dhcx_drv_status_t ism330dhcx_drv_write_reg(ism330dhcx_object_t *p_obj, uint8_t reg, uint8_t data);
ism330dhcx_drv_status_t ism330dhcx_drv_set_interrupt_latch(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_drdy_on_int1(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_drdy_on_int2(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_drdy_on_int1(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_drdy_on_int2(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_set_drdy_mode(ism330dhcx_object_t *p_obj, uint8_t mode);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_free_fall_detection(ism330dhcx_object_t *p_obj,
                                                                      ism330dhcx_sensorintpin_t intpin);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_free_fall_detection(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_free_fall_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_free_fall_duration(ism330dhcx_object_t *p_obj, uint8_t duration);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_wake_up_detection(ism330dhcx_object_t *p_obj,
                                                                    ism330dhcx_sensorintpin_t intpin);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_wake_up_detection(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_wake_up_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_wake_up_duration(ism330dhcx_object_t *p_obj, uint8_t duration);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_single_tap_detection(ism330dhcx_object_t *p_obj,
                                                                       ism330dhcx_sensorintpin_t intpin);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_single_tap_detection(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_double_tap_detection(ism330dhcx_object_t *p_obj,
                                                                       ism330dhcx_sensorintpin_t intpin);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_double_tap_detection(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_shock_time(ism330dhcx_object_t *p_obj, uint8_t time);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_quiet_time(ism330dhcx_object_t *p_obj, uint8_t time);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_tap_duration_time(ism330dhcx_object_t *p_obj, uint8_t duration);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_6d_orientation(ism330dhcx_object_t *p_obj,
                                                                 ism330dhcx_sensorintpin_t intpin);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_disable_6d_orientation(ism330dhcx_object_t *p_obj);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_6d_orientation_threshold(ism330dhcx_object_t *p_obj, uint8_t threshold);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_xl(ism330dhcx_object_t *p_obj, uint8_t *xlow);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_xh(ism330dhcx_object_t *p_obj, uint8_t *xhigh);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_yl(ism330dhcx_object_t *p_obj, uint8_t *ylow);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_yh(ism330dhcx_object_t *p_obj, uint8_t *yhigh);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_zl(ism330dhcx_object_t *p_obj, uint8_t *zlow);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_6d_orientation_zh(ism330dhcx_object_t *p_obj, uint8_t *zhigh);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_event_status(ism330dhcx_object_t *p_obj,
                                                            ism330dhcx_event_status_t *p_status);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_set_selftest(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_drdy_status(ism330dhcx_object_t *p_obj, uint8_t *p_status);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_get_init_status(ism330dhcx_object_t *p_obj, uint8_t *p_status);
ism330dhcx_drv_status_t ism330dhcx_drv_acc_enable_hp_filter(ism330dhcx_object_t *p_obj, uint8_t cutoff);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_set_selftest(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_drdy_status(ism330dhcx_object_t *p_obj, uint8_t *p_status);
ism330dhcx_drv_status_t ism330dhcx_drv_gyro_get_init_status(ism330dhcx_object_t *p_obj, uint8_t *p_status);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_num_samples(ism330dhcx_object_t *p_obj, uint16_t *p_numsamples);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_full_status(ism330dhcx_object_t *p_obj, uint8_t *p_status);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_acc_set_bdr(ism330dhcx_object_t *p_obj, float_t bdr);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_gyro_set_bdr(ism330dhcx_object_t *p_obj, float_t bdr);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_int1_fifo_full(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_int2_fifo_full(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_watermark_level(ism330dhcx_object_t *p_obj, uint16_t watermark);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_stop_on_fth(ism330dhcx_object_t *p_obj, uint8_t status);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_set_mode(ism330dhcx_object_t *p_obj, uint8_t mode);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_tag(ism330dhcx_object_t *p_obj, uint8_t *tag);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_data(ism330dhcx_object_t *p_obj, uint8_t *p_data);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_acc_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_acceleration);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_gyro_get_axes(ism330dhcx_object_t *p_obj, ism330dhcx_axes_t *p_angularrate);
ism330dhcx_drv_status_t ism330dhcx_drv_fifo_get_data_word(ism330dhcx_object_t *p_obj, int16_t *p_data_raw);
ism330dhcx_drv_status_t ism330dhcx_drv_enable_irq(ism330dhcx_object_t *p_obj, ism330dhcx_sensorintpin_t intpin);
ism330dhcx_drv_status_t ism330dhcx_drv_disable_irq(ism330dhcx_object_t *p_obj, ism330dhcx_sensorintpin_t intpin);

#if defined (ISM330DHCX_CALLBACKS) && (ISM330DHCX_CALLBACKS == 1)
ism330dhcx_drv_status_t ism330dhcx_drv_register_callback(ism330dhcx_object_t *pobj, ism330dhcx_callback_t cb, void *arg,
                                                         ism330dhcx_sensorintpin_t intpin);
#endif /* ISM330DHCX_CALLBACKS */

#ifdef __cplusplus
}
#endif

#endif /* ISM330DHCX */
