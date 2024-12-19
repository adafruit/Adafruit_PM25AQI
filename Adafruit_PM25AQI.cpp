/*!
 * @file Adafruit_PM25AQI.cpp
 *
 * @mainpage Adafruit PM2.5 air quality sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's PM2.5 AQI driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit PM2.5 Air quality sensors: http://www.adafruit.com/products/4632
 *
 * These sensors use I2C or UART to communicate.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 *
 * @section author Author
 * Written by Ladyada for Adafruit Industries.
 *
 * @section license License
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_PM25AQI.h"
#include <math.h>

/*!
 *  @brief  Instantiates a new PM25AQI class
 */
Adafruit_PM25AQI::Adafruit_PM25AQI() {}

/*!
 *  @brief  Setups the hardware and detects a valid PMSA003I. Initializes I2C.
 *  @param  theWire
 *          Optional pointer to I2C interface, otherwise use Wire
 *  @return True if PMSA003I found on I2C, False if something went wrong!
 */
bool Adafruit_PM25AQI::begin_I2C(TwoWire *theWire) {
  if (!i2c_dev) {
    i2c_dev = new Adafruit_I2CDevice(PMSA003I_I2CADDR_DEFAULT, theWire);
  }

  if (!i2c_dev->begin()) {
    return false;
  }

  return true;
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5
 *  @param  theSerial
 *          Pointer to Stream (HardwareSerial/SoftwareSerial) interface
 *  @return True
 */
bool Adafruit_PM25AQI::begin_UART(Stream *theSerial) {
  serial_dev = theSerial;

  return true;
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5
 *  @param  data
 *          Pointer to PM25_AQI_Data that will be filled by read()ing
 *  @return True on successful read, false if timed out or bad data
 */
bool Adafruit_PM25AQI::read(PM25_AQI_Data *data) {
  uint8_t buffer[32];
  uint16_t sum = 0;

  if (!data) {
    return false;
  }

  if (i2c_dev) { // ok using i2c?
    if (!i2c_dev->read(buffer, 32)) {
      return false;
    }
  } else if (serial_dev) { // ok using uart
    if (!serial_dev->available()) {
      return false;
    }
    int skipped = 0;
    while ((skipped < 32) && (serial_dev->peek() != 0x42)) {
      serial_dev->read();
      skipped++;
      if (!serial_dev->available()) {
        return false;
      }
    }
    if (serial_dev->peek() != 0x42) {
      serial_dev->read();
      return false;
    }
    // Now read all 32 bytes
    if (serial_dev->available() < 32) {
      return false;
    }
    serial_dev->readBytes(buffer, 32);
  } else {
    return false;
  }

  // Check that start byte is correct!
  if (buffer[0] != 0x42 || buffer[1] != 0x4d) {
    return false;
  }

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // The data comes in big endian (MSB first), this solves it so it works on all
  // platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)data, (void *)buffer_u16, 30);

  if (sum != data->checksum) {
    return false;
  }

  // convert concentration to AQI
  data->aqi_pm25_us = pm25_aqi_us(data->pm25_env);
  data->aqi_pm25_china = pm25_aqi_china(data->pm25_env);
  data->aqi_pm100_us = pm100_aqi_us(data->pm100_env);
  data->aqi_pm100_china = pm100_aqi_china(data->pm100_env);

  // success!
  return true;
}

/*!
 *  @brief  Get AQI of PM2.5 in US standard
 *  @param  concentration
 *          the environmental concentration of pm2.5 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm25_aqi_us(float concentration) {
  float c;
  float AQI;
  c = (floor(10 * concentration)) / 10;
  if (c < 0)
    AQI = 0;
  else if (c >= 0 && c < 12.1f) {
    AQI = linear(50, 0, 12, 0, c);
  } else if (c >= 12.1f && c < 35.5f) {
    AQI = linear(100, 51, 35.4f, 12.1f, c);
  } else if (c >= 35.5f && c < 55.5f) {
    AQI = linear(150, 101, 55.4f, 35.5f, c);
  } else if (c >= 55.5f && c < 150.5f) {
    AQI = linear(200, 151, 150.4f, 55.5f, c);
  } else if (c >= 150.5f && c < 250.5f) {
    AQI = linear(300, 201, 250.4f, 150.5f, c);
  } else if (c >= 250.5f && c < 350.5f) {
    AQI = linear(400, 301, 350.4f, 250.5f, c);
  } else if (c >= 350.5f && c < 500.5f) {
    AQI = linear(500, 401, 500.4f, 350.5f, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Get AQI of PM10 in US standard
 *  @param  concentration
 *          the environmental concentration of pm10 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm100_aqi_us(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c < 55) {
    AQI = linear(50, 0, 55, 0, c);
  } else if (c < 155) {
    AQI = linear(100, 51, 155, 55, c);
  } else if (c < 255) {
    AQI = linear(150, 101, 255, 155, c);
  } else if (c < 355) {
    AQI = linear(200, 151, 355, 255, c);
  } else if (c < 425) {
    AQI = linear(300, 201, 425, 355, c);
  } else if (c < 505) {
    AQI = linear(400, 301, 505, 425, c);
  } else if (c < 605) {
    AQI = linear(500, 401, 605, 505, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Get AQI of PM2.5 in China standard
 *  @param  concentration
 *          the environmental concentration of pm2.5 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm25_aqi_china(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c <= 35) {
    AQI = linear(50, 0, 35, 0, c);
  } else if (c <= 75) {
    AQI = linear(100, 51, 75, 35, c);
  } else if (c <= 115) {
    AQI = linear(150, 101, 115, 75, c);
  } else if (c <= 150) {
    AQI = linear(200, 151, 150, 115, c);
  } else if (c <= 250) {
    AQI = linear(300, 201, 250, 150, c);
  } else if (c <= 350) {
    AQI = linear(400, 301, 350, 250, c);
  } else if (c <= 500) {
    AQI = linear(500, 401, 500, 350, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Get AQI of PM10 in China standard
 *  @param  concentration
 *          the environmental concentration of pm10 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm100_aqi_china(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c <= 50) {
    AQI = linear(50, 0, 50, 0, c);
  } else if (c <= 150) {
    AQI = linear(100, 51, 150, 50, c);
  } else if (c <= 250) {
    AQI = linear(150, 101, 250, 150, c);
  } else if (c <= 350) {
    AQI = linear(200, 151, 350, 250, c);
  } else if (c <= 420) {
    AQI = linear(300, 201, 420, 350, c);
  } else if (c <= 500) {
    AQI = linear(400, 301, 500, 420, c);
  } else if (c <= 600) {
    AQI = linear(500, 401, 600, 500, c);
  } else {
    AQI = 99999; //
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
float Adafruit_PM25AQI::linear(uint16_t aqi_high, uint16_t aqi_low,
                               float conc_high, float conc_low,
                               float concentration) {
  float f;
  f = ((concentration - conc_low) / (conc_high - conc_low)) *
          (aqi_high - aqi_low) +
      aqi_low;
  return f;
}