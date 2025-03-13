/*!
 * @file Adafruit_PM25AQI.h
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

#ifndef ADAFRUIT_PM25AQI_H
#define ADAFRUIT_PM25AQI_H

#include <Adafruit_PM25AQI_Base.h>

// the i2c address
#define PMSA003I_I2CADDR_DEFAULT 0x12 ///< PMSA003I has only one I2C address

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          PM2.5 Air Quality Sensor
 */
class Adafruit_PM25AQI {
public:
  Adafruit_PM25AQI();
  ~Adafruit_PM25AQI();

  [[deprecated("Please use Adafruit_PM25AQI_PMSA003I directly")]]
  bool begin_I2C(TwoWire *theWire = &Wire,
                 uint8_t addr = PMSA003I_I2CADDR_DEFAULT);

  [[deprecated("Please use appropriate UART driver class directly")]]
  bool begin_UART(Stream *SerialStream);

  bool read(PM25_AQI_Data *data);

private:
  Adafruit_PM25AQI_Base *driver = nullptr;
  // Removed old private members as they're now handled by specific drivers
};

#endif
