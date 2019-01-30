#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// hardware
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "include/htu21d.h"

void uartConfigure(uint32_t baudRate) {
  // Enable the GPIO Peripheral used by the UART.
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

  // Enable UART0
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}

  // Configure GPIO Pins for UART mode.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // Use the internal 16MHz oscillator as the UART clock source.
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

  // Initialize the UART for console I/O.
  UARTStdioConfig(0, baudRate, 16000000);
}

#define UART_BAUD 115200

int main(void) {
  ROM_FPULazyStackingEnable();

  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

  uartConfigure(UART_BAUD);
  UARTprintf("Starting\n");

  Htu21d htu = {
      .i2c_base = I2C0_BASE,
      .res      = HTU21D_HUMIDITY_8_TEMP_12,
  };

  htu21d_init(&htu);

  // read and write resolution
  //   htu21d_write_resolution(&htu, HTU21D_HUMIDITY_11_TEMP_11);
  //   htu21d_read_resolution(&htu);

  float res = -1;
  htu21d_start_humid_read(&htu);

  for (;;) {
    char         tempChar[500];
    Htu21d_error err = htu21d_check_humid_read(&htu, &res);
    if (err == HTU21D_ERROR_NONE) {
      sprintf(tempChar, "Error is %d, humid is %f\n", err, res);
      UARTprintf(tempChar);
      htu21d_start_humid_read(&htu);
    }

    for (uint32_t i = 0; i < 700000; ++i) {
      // wait loop
    }
  }

  return 0;
}
