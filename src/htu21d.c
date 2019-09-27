#include "htu21d.h"

#include "htu21d_hw.h"
#include "htu21d_i2c.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

#include "swo_segger.h"

static unsigned long htu21d_get_meas_delayms(Htu21d_resolution res, uint8_t type) {
  switch (res) {
    case HTU21D_HUMIDITY_12_TEMP_14:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 15 : 16;
      break;
    case HTU21D_HUMIDITY_8_TEMP_12:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 2 : 14;
      break;
    case HTU21D_HUMIDITY_10_TEMP_13:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 4 : 15;
      break;
    case HTU21D_HUMIDITY_11_TEMP_11:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 7 : 7;
      break;
  }
  return 0;
}

Htu21d_error htu21d_read_resolution(Htu21d *htu) {
  uint32_t res;
  uint32_t err = htu21d_i2c_read_reg(htu, HTU21D_READ_USER_REG, &res);
  if (I2C_MASTER_ERR_NONE != err) {
    UARTprintf("htu21d: Error while reading resolution");
    return HTU21D_ERROR_COMM;
  }
  htu->res = (Htu21d_resolution)(res & HTU21D_USER_REGISTER_RESOLUTION_MASK);
  return HTU21D_ERROR_NONE;
}

Htu21d_error htu21d_write_resolution(Htu21d *htu, const Htu21d_resolution res) {
  Htu21d_error err = htu21d_read_resolution(htu);
  if (HTU21D_ERROR_NONE != err) {
    UARTprintf("htu21d: error while reading resolution inside write");
    return err;
  }

  htu21d_i2c_write_reg(htu,
                       HTU21D_WRITE_USER_REG,
                       res | (((htu->res) & 0xff) & ~HTU21D_USER_REGISTER_RESOLUTION_MASK));

  htu->res = res;
  return HTU21D_ERROR_NONE;
}

void htu21d_soft_reset(const Htu21d *htu) { htu21d_i2c_write(htu, HTU21D_SOFT_RESET); }

static void htu21d_start_data_read(const Htu21d *htu, const uint8_t type) {
  htu21d_i2c_write(htu, type);
}

static Htu21d_error htu21d_check_data_read(const Htu21d *htu,
                                           float (*convFunc)(const uint32_t),
                                           float *out) {
  uint8_t buf[2];

  uint32_t err = htu21d_i2c_read_list_no_hold(htu, buf, 2);

  if (I2C_MASTER_ERR_ADDR_ACK == err) {
    // UARTprintf("No data\n");
    return HTU21D_ERROR_NO_DATA;
  } else if (I2C_MASTER_ERR_NONE == err) {
    uint32_t raw = ((buf[0] << 8) + (buf[1] & 0xfc)) & 0xffff;
    *out         = convFunc(raw);
    return HTU21D_ERROR_NONE;
  } else {
    // UARTprintf("Unkown error\n");
    return HTU21D_ERROR_COMM;
  }
  return err;
}

static float conv_to_temp(const uint32_t raw) { return -46.85 + (0.00268127441 * (float)raw); }

void htu21d_start_temp_read(const Htu21d *htu) {
  htu21d_start_data_read(htu, HTU21D_TRIGGER_TEMP_MEASURE_NOHOLD);
}

Htu21d_error htu21d_check_temp_read(const Htu21d *htu, float *output) {
  return htu21d_check_data_read(htu, conv_to_temp, output);
}

static float conv_to_humid(const uint32_t raw) {
  float res = -6 + (0.00190734863 * (float)raw);
  res       = (res > 100) ? 100 : res;
  res       = (res < 0) ? 0 : res;
  return res;
}

void htu21d_start_humid_read(const Htu21d *htu) {
  htu21d_start_data_read(htu, HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD);
}

Htu21d_error htu21d_check_humid_read(const Htu21d *htu, float *output) {
  return htu21d_check_data_read(htu, conv_to_humid, output);
}

// change base on your applications
void htu21d_init(const Htu21d *htu) {
  if (false == SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0)) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    while (false == SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0)) {
      // wait for it to be ready
    }
  }

  if (false == SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (false == SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {
      // wait for it to be ready
    }
  }

  GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  GPIOPinConfigure(GPIO_PB3_I2C0SDA);

  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

  I2CMasterInitExpClk(htu->i2c_base, SysCtlClockGet(), false);
  htu21d_write_resolution(htu, htu->res);
}
