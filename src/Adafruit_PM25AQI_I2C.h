/*!
 * @file ADAFRUIT_PM25AQI_I2C.h
 *
 * This is the documentation for Adafruit's PM25 AQI driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit PM25 air quality sensors: http://www.adafruit.com/products/4632
 *
 * These sensors use I2C or UART to communicate.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef ADAFRUIT_PM25AQI_I2C_H
#define ADAFRUIT_PM25AQI_I2C_H
#include "Adafruit_PM25AQI.h"
#include <Adafruit_I2CDevice.h>

#define PMSA003I_DEFAULT_ADDRESS 0x12 ///< PMSA003I has only one I2C address

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          PM2.5 Air Quality Sensor
 */
class Adafruit_PM25AQI_I2C : public Adafruit_PM25AQI {
public:
  Adafruit_PM25AQI_I2C();
  ~Adafruit_PM25AQI_I2C();
  bool begin(TwoWire *theWire = &Wire,
             uint8_t i2c_addr = PMSA003I_DEFAULT_ADDRESS);
  bool read(PM25_AQI_Data *data) override;

private:
  Adafruit_I2CDevice *_i2c_dev = nullptr;
  uint8_t _readbuffer[32];
};

#endif // ADAFRUIT_PM25AQI_I2C_H
