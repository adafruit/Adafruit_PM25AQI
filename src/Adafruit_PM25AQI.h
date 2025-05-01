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
#include "Adafruit_AQIUtils.h"
#include "Adafruit_PM25AQI_I2C.h"
#include "Adafruit_PM25AQI_UART.h"
#include "Arduino.h"

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

  // AQI conversion results:
  uint16_t aqi_pm25_us;     ///< pm2.5 AQI of United States
  uint16_t aqi_pm100_us;    ///< pm10 AQI of United States
  uint16_t aqi_pm25_china;  ///< pm2.5 AQI of China
  uint16_t aqi_pm100_china; ///< pm10 AQI of China

} PM25_AQI_Data;

class Adafruit_PM25AQI_I2C;  ///< forward declaration
class Adafruit_PM25AQI_UART; ///< forward declaration

class Adafruit_PM25AQI {
public:
  Adafruit_PM25AQI();
  virtual ~Adafruit_PM25AQI();
  virtual bool begin() = 0;
  void ConvertAQIData(PM25_AQI_Data *data);
  // These are backwards compatible with the "old" library
  bool begin_I2C(TwoWire *theWire = &Wire);
  bool begin_UART(Stream *theStream);
  // TODO: Implement
  // bool begin_UART_Cubic(Stream *theStream);
  // bool begin_UART_PM25(Stream *theStream);
  virtual bool read(PM25_AQI_Data *data);

protected:
  Adafruit_PM25AQI_I2C *_pm25_i2c = nullptr;
  Adafruit_PM25AQI_UART *_pm25_uart = nullptr;
  Adafruit_AQIUtils *_aqi_utils = nullptr;
};

#endif
