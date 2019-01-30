#ifndef _HTU21D_H
#define _HTU21D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
  HTU21D_HUMIDITY_12_TEMP_14 = 0x00,
  HTU21D_HUMIDITY_8_TEMP_12  = 0x01,
  HTU21D_HUMIDITY_10_TEMP_13 = 0x80,
  HTU21D_HUMIDITY_11_TEMP_11 = 0x81,
} Htu21d_resolution;

typedef enum { HTU21D_ERROR_NONE = 0, HTU21D_ERROR_NO_DATA, HTU21D_ERROR_COMM } Htu21d_error;

typedef struct {
  uint32_t          i2c_base;
  Htu21d_resolution res;
} Htu21d;

void htu21d_init(const Htu21d *htu);

Htu21d_error htu21d_read_resolution(Htu21d *htu);
Htu21d_error htu21d_write_resolution(Htu21d *htu, const Htu21d_resolution res);

void         htu21d_start_humid_read(const Htu21d *htu);
Htu21d_error htu21d_check_humid_read(const Htu21d *htu, float *output);
void         htu21d_start_temp_read(const Htu21d *htu);
Htu21d_error htu21d_check_temp_read(const Htu21d *htu, float *output);

#ifdef __cplusplus
}
#endif
#endif