#include "htu21d_i2c.h"
#include "htu21d.h"
#include "htu21d_hw.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

void htu21d_i2c_write(const Htu21d *htu, const uint8_t command) {
  htu21d_i2c_wait_bus(htu);
  I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, false);
  I2CMasterDataPut(htu->i2c_base, command);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_SINGLE_SEND);
  htu21d_i2c_wait_master(htu);
  htu21d_i2c_wait_bus(htu);
}

void htu21d_i2c_write_reg(const Htu21d *htu, const uint8_t command, const uint8_t dataToWrite) {
  htu21d_i2c_wait_bus(htu);

  I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, false);
  I2CMasterDataPut(htu->i2c_base, command);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_SEND_START);
  htu21d_i2c_wait_master(htu);

  I2CMasterDataPut(htu->i2c_base, dataToWrite);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_SEND_FINISH);
  htu21d_i2c_wait_master(htu);
  htu21d_i2c_wait_bus(htu);
}

uint32_t htu21d_i2c_read_reg(const Htu21d *htu, const uint8_t command, uint32_t *out) {
  htu21d_i2c_wait_bus(htu);
  I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, false);
  I2CMasterDataPut(htu->i2c_base, command);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_SEND_START);
  htu21d_i2c_wait_master(htu);

  I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, true);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_SINGLE_RECEIVE);
  htu21d_i2c_wait_master(htu);
  htu21d_i2c_wait_bus(htu);

  *out = I2CMasterDataGet(htu->i2c_base);
  return I2CMasterErr(htu->i2c_base);
}

uint32_t htu21d_i2c_read_list_no_hold(const Htu21d *htu,
                                      uint8_t *     recvData,
                                      const uint8_t totalData) {
  assert(NULL != recvData);
  assert(totalData > 1);

  htu21d_i2c_wait_bus(htu);

  I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, true);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_START);
  htu21d_i2c_wait_master(htu);

  uint32_t err = I2CMasterErr(htu->i2c_base);
  if (I2C_MASTER_ERR_NONE == err) {
    recvData[0] = I2CMasterDataGet(htu->i2c_base);
  } else {
    I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    return err;
  }

  uint8_t dataIndex = 1;
  for (dataIndex = 1; dataIndex < totalData - 1; ++dataIndex) {
    I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, true);
    I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
    htu21d_i2c_wait_master(htu);
    recvData[dataIndex] = I2CMasterDataGet(htu->i2c_base);
  }

  I2CMasterSlaveAddrSet(htu->i2c_base, HTU21D_ADDRESS, true);
  I2CMasterControl(htu->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
  htu21d_i2c_wait_master(htu);
  htu21d_i2c_wait_bus(htu);
  recvData[dataIndex] = I2CMasterDataGet(htu->i2c_base);

  return I2CMasterErr(htu->i2c_base);
}

void htu21d_i2c_wait_master(const Htu21d *htu) {
  while (I2CMasterBusy(htu->i2c_base)) {
    // wait until the master is not busy
  }
}

void htu21d_i2c_wait_bus(const Htu21d *htu) {
  while (I2CMasterBusBusy(htu->i2c_base)) {
    // wait until the bus is not busy
  }
}
