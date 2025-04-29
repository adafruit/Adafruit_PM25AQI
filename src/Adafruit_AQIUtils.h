/*!
 * @file Adafruit_AQIUtils.h
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 *
 * @section author Author
 * Written by Ladyada for Adafruit Industries.
 * Modified by Brent Rubell for Adafruit Industries
 *
 * @section license License
 * BSD license, all text here must be included in any redistribution.
 *
 */
#ifndef ADAFRUIT_AQIUTILS_CPP
#define ADAFRUIT_AQIUTILS_CPP
#include "Arduino.h"
#include <math.h>

#define ERR_AQI_OUT_OF_RANGE 99999 ///< AQI out of range

class Adafruit_AQIUtils {
public:
  float MapLinear(uint16_t aqi_high, uint16_t aqi_low, float conc_high,
                  float conc_low, float concentration);
  uint16_t pm25_aqi_us(float concentration);
  uint16_t pm25_aqi_china(float concentration);
  uint16_t pm100_aqi_us(float concentration);
  uint16_t pm100_aqi_china(float concentration);
};
#endif // ADAFRUIT_AQIUTILS_CPP
