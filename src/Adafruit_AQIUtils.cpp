/*!
 * @file Adafruit_AQIUtils.cpp
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
#include "Adafruit_AQIUtils.h"

/*!
 *  @brief  Get AQI of PM2.5 in US standard
 *  @param  concentration
 *          the environmental concentration of pm2.5 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. ERR_AQI_OUT_OF_RANGE for
 * out of range.
 */
uint16_t Adafruit_AQIUtils::pm25_aqi_us(float concentration) {
  float AQI;
  float c = (floor(10 * concentration)) / 10;
  if (c < 0) {
    AQI = 0;
  } else if (c >= 0 && c < 12.1f) {
    AQI = MapLinear(50, 0, 12, 0, c);
  } else if (c >= 12.1f && c < 35.5f) {
    AQI = MapLinear(100, 51, 35.4f, 12.1f, c);
  } else if (c >= 35.5f && c < 55.5f) {
    AQI = MapLinear(150, 101, 55.4f, 35.5f, c);
  } else if (c >= 55.5f && c < 150.5f) {
    AQI = MapLinear(200, 151, 150.4f, 55.5f, c);
  } else if (c >= 150.5f && c < 250.5f) {
    AQI = MapLinear(300, 201, 250.4f, 150.5f, c);
  } else if (c >= 250.5f && c < 350.5f) {
    AQI = MapLinear(400, 301, 350.4f, 250.5f, c);
  } else if (c >= 350.5f && c < 500.5f) {
    AQI = MapLinear(500, 401, 500.4f, 350.5f, c);
  } else {
    AQI = ERR_AQI_OUT_OF_RANGE; //
  }
  return round(AQI);
}

uint16_t Adafruit_AQIUtils::pm25_aqi_china(float concentration) {
  float AQI;
  float c = concentration;
  if (c < 0) {
    AQI = 0;
  } else if (c <= 35) {
    AQI = MapLinear(50, 0, 35, 0, c);
  } else if (c <= 75) {
    AQI = MapLinear(100, 51, 75, 35, c);
  } else if (c <= 115) {
    AQI = MapLinear(150, 101, 115, 75, c);
  } else if (c <= 150) {
    AQI = MapLinear(200, 151, 150, 115, c);
  } else if (c <= 250) {
    AQI = MapLinear(300, 201, 250, 150, c);
  } else if (c <= 350) {
    AQI = MapLinear(400, 301, 350, 250, c);
  } else if (c <= 500) {
    AQI = MapLinear(500, 401, 500, 350, c);
  } else {
    AQI = ERR_AQI_OUT_OF_RANGE; //
  }
  return round(AQI);
}

uint16_t Adafruit_AQIUtils::pm100_aqi_us(float concentration) {
  float AQI;
  float c = concentration;
  if (c < 0) {
    AQI = 0;
  } else if (c < 55) {
    AQI = MapLinear(50, 0, 55, 0, c);
  } else if (c < 155) {
    AQI = MapLinear(100, 51, 155, 55, c);
  } else if (c < 255) {
    AQI = MapLinear(150, 101, 255, 155, c);
  } else if (c < 355) {
    AQI = MapLinear(200, 151, 355, 255, c);
  } else if (c < 425) {
    AQI = MapLinear(300, 201, 425, 355, c);
  } else if (c < 505) {
    AQI = MapLinear(400, 301, 505, 425, c);
  } else if (c < 605) {
    AQI = MapLinear(500, 401, 605, 505, c);
  } else {
    AQI = ERR_AQI_OUT_OF_RANGE; //
  }
  return round(AQI);
}

uint16_t Adafruit_AQIUtils::pm100_aqi_china(float concentration) {
  float AQI;
  float c = concentration;
  if (c < 0) {
    AQI = 0;
  } else if (c <= 35) {
    AQI = MapLinear(50, 0, 35, 0, c);
  } else if (c <= 75) {
    AQI = MapLinear(100, 51, 75, 35, c);
  } else if (c <= 115) {
    AQI = MapLinear(150, 101, 115, 75, c);
  } else if (c <= 150) {
    AQI = MapLinear(200, 151, 150, 115, c);
  } else if (c <= 250) {
    AQI = MapLinear(300, 201, 250, 150, c);
  } else if (c <= 350) {
    AQI = MapLinear(400, 301, 350, 250, c);
  } else if (c <= 500) {
    AQI = MapLinear(500, 401, 500, 350, c);
  } else {
    AQI = ERR_AQI_OUT_OF_RANGE; //
  }
  return round(AQI);
}

/*!
 *  @brief  Linearly map a concentration value to its AQI level
 *  @param  aqi_high max aqi of the calculating range
 *  @param  aqi_low min aqi of the calculating range
 *  @param  conc_high max concentration value (ug/m3) of the calculating range
 *  @param  conc_low min concentration value (ug/m3) of the calculating range
 *  @param  concentration
 *          the concentration value to be calculated
 *  @return Calculated AQI value
 */
float Adafruit_AQIUtils::MapLinear(uint16_t aqi_high, uint16_t aqi_low,
                                   float conc_high, float conc_low,
                                   float concentration) {
  float f;
  f = ((concentration - conc_low) / (conc_high - conc_low)) *
          (aqi_high - aqi_low) +
      aqi_low;
  return f;
}