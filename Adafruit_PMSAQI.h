/*!
 * @file Adafruit_PMSAQI.h
 *
 * This is the documentation for Adafruit's PMS AQI driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit PMS air quality sensors: http://www.adafruit.com/products/4632
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

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

// the i2c address
#define PMSA003I_I2CADDR_DEFAULT 0x12 ///< PMSA003I has only one I2C address

typedef struct PMSAQIdata {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
} PMS_AQI_Data;

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          PMSA003I Air Quality Sensor
 */
class Adafruit_PMSAQI {
public:
  Adafruit_PMSAQI();
  bool begin_I2C(TwoWire *theWire = &Wire);
  bool read(PMS_AQI_Data *data);

private:
  Adafruit_I2CDevice *i2c_dev = NULL;
  uint8_t _readbuffer[32];
};
