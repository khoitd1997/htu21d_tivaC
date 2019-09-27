#ifndef _HTU21D_I2C_H
#define _HTU21D_I2C_H
#ifdef __cplusplus
extern "C" {
#endif
#include "htu21d.h"

#include <stdint.h>

void     htu21d_i2c_write(const Htu21d *htu, const uint8_t command);
void     htu21d_i2c_write_reg(const Htu21d *htu, const uint8_t command, const uint8_t dataToWrite);
uint32_t htu21d_i2c_read_reg(const Htu21d *htu, const uint8_t command, uint32_t *out);
uint32_t htu21d_i2c_read_list_no_hold(const Htu21d *htu,
                                      uint8_t *     recvData,
                                      const uint8_t totalData);
void     htu21d_i2c_wait_master(const Htu21d *htu);
void     htu21d_i2c_wait_bus(const Htu21d *htu);

#ifdef __cplusplus
}
#endif
#endif