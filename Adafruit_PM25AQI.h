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

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

// the i2c address
#define PMSA003I_I2CADDR_DEFAULT 0x12 ///< PMSA003I has only one I2C address

/**! Structure holding Plantower's standard packet **/
typedef struct PMSAQIdata {
  uint16_t framelen;       ///< How long this data chunk is
  uint16_t pm10_standard,  ///< Standard PM1.0
      pm25_standard,       ///< Standard PM2.5
      pm100_standard;      ///< Standard PM10.0
  uint16_t pm10_env,       ///< Environmental PM1.0
      pm25_env,            ///< Environmental PM2.5
      pm100_env;           ///< Environmental PM10.0
  uint16_t particles_03um, ///< 0.3um Particle Count
      particles_05um,      ///< 0.5um Particle Count
      particles_10um,      ///< 1.0um Particle Count
      particles_25um,      ///< 2.5um Particle Count
      particles_50um,      ///< 5.0um Particle Count
      particles_100um;     ///< 10.0um Particle Count
  uint16_t unused;         ///< Unused (version + error code)

  uint16_t checksum; ///< Packet checksum

  // verbose infos:
  uint8_t startbyte_fail; ///< startbyte check fail?: 1 - fail, 0 - success
  uint8_t checksum_fail;  ///< checksum check fail?: 1 - fail, 0 - success
  uint8_t version;        ///< version number
  uint8_t error_code;     ///< error code
  uint8_t raw[32];        ///< raw packet data

  // AQI conversion results:
  uint8_t aqi_pm25_us;     ///< pm2.5 AQI of United States
  uint8_t aqi_pm100_us;    ///< pm10 AQI of United States
  uint8_t aqi_pm25_china;  ///< pm2.5 AQI of China
  uint8_t aqi_pm100_china; ///< pm10 AQI of China

} PM25_AQI_Data;

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          PM2.5 Air Quality Sensor
 */
class Adafruit_PM25AQI {
public:
  Adafruit_PM25AQI();
  bool begin_I2C(TwoWire *theWire = &Wire);
  bool begin_UART(Stream *theStream);
  bool read(PM25_AQI_Data *data);

  uint16_t pm25_aqi_us(float concentration);
  uint16_t pm25_aqi_china(float concentration);
  uint16_t pm100_aqi_us(float concentration);
  uint16_t pm100_aqi_china(float concentration);
  float linear(uint16_t aqi_high, uint16_t aqi_low, float conc_high,
               float conc_low, float concentration);

private:
  Adafruit_I2CDevice *i2c_dev = NULL;
  Stream *serial_dev = NULL;
  uint8_t _readbuffer[32];
};

#endif
