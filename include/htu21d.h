#ifndef _HTU21D_H
#define _HTU21D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief possible resolutions for both humidity and temp
 *
 */
typedef enum {
  HTU21D_HUMIDITY_12_TEMP_14 = 0x00,
  HTU21D_HUMIDITY_8_TEMP_12  = 0x01,
  HTU21D_HUMIDITY_10_TEMP_13 = 0x80,
  HTU21D_HUMIDITY_11_TEMP_11 = 0x81,
} Htu21d_resolution;

/**
 * @brief error return for htu
 *
 */
typedef enum {
  HTU21D_ERROR_NONE = 0,  ///< no error
  HTU21D_ERROR_NO_DATA,   ///< no data
  HTU21D_ERROR_COMM       ///< general error
} Htu21d_error;

/**
 * @brief represents an htu21d device, needed for all operations
 *
 */
typedef struct {
  uint32_t          i2c_base;  ///< base addr of the i2c controller like I2C0_BASE
  Htu21d_resolution res;       ///< current resolution
} Htu21d;

/**
 * @brief prepare the i2c clock, gpio clock and pins
 * This function is hard coded because the amount of params needed is pretty high
 *
 * @param htu device struct pointer
 */
void htu21d_init(const Htu21d *htu);

/**
 * @brief read and store the current resolution of device into the htu struct
 *
 * @param htu device struct pointer
 * @return Htu21d_error
 */
Htu21d_error htu21d_read_resolution(Htu21d *htu);

/**
 * @brief write new resolution to device and update device struct
 *
 * @param htu device struct pointer
 * @param res the target resolution
 * @return Htu21d_error
 */
Htu21d_error htu21d_write_resolution(Htu21d *htu, const Htu21d_resolution res);

/**
 * @brief ask the sensor to start reading humidity
 * Time until results are available depends on the sampling mode
 *
 * @param htu device struct pointer
 */
void htu21d_start_humid_read(const Htu21d *htu);

/**
 * @brief check if the sensor has finished sampling humidity
 *
 * @param htu device struct pointer
 * @param output contain the newest humidity reading if there is no error
 * @return Htu21d_error can be none if data available, no data if sensor is not ready or other
 * errors depending on issue
 */
Htu21d_error htu21d_check_humid_read(const Htu21d *htu, float *output);

/**
 * @brief ask the sensor to start reading temperature
 * Time until results are available depends on the sampling mode

 * @param htu device struct pointer
 */
void htu21d_start_temp_read(const Htu21d *htu);

/**
 * @brief check if the sensor has finished sampling temperature
 * Time until results are available depends on the sampling mode
 *
 * @param htu device struct pointer
 * @param output contain the newest temperature reading if there is no error
 * @return Htu21d_error can be none if data available, no data if sensor is not ready or other
 * errors depending on issue
 */
Htu21d_error htu21d_check_temp_read(const Htu21d *htu, float *output);

/**
 * @brief perform a software reset on the sensor
 *
 * @param htu device struct pointer
 */
void htu21d_soft_reset(const Htu21d *htu);

#ifdef __cplusplus
}
#endif
#endif