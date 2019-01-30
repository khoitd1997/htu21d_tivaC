# htu21d_tivaC

Port of the htu21d driver for the tivaC, this is customized for personal use so definitely read the code carefully before you use it.

## Design

I try to separate the logic from the specific i2c details of the tivaC so that I can port this driver to other mcu in the future, the htu21d_i2c header and source file will be where most of the tivaC i2c specific goes while the htu21d.c will contain mostly logic specific to the htu21d.

## Credits

I copied most of sparkfun macros for register definition [here][1], the rest is just implementation of the datasheet.

## Features

- No hold data read for temperature and humidity
- Resolution read/write
- Software reset

## Directory Structure

- include/:
  - htu21d.h: function prototypes of main features, also contain Doxygen docs
- src/: source code of the driver
  - htu21d_hw.h: register macros and other constants specific to the htu21d
  - htu21d_i2c.c and .h: i2c helper layer specific to the tivaC
  - htu21d.c: implementation of logic specific to the htu21d
  - main.c: contain example of how to use the implemented features

Other directory will belong to Code Composer Studio project.

[1]:https://github.com/sparkfun/SparkFun_HTU21D_Breakout_Arduino_Library/blob/master/src/SparkFunHTU21D.h
