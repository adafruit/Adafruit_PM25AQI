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
 * This library also works with the Cubic PM1006 UART Air Quality Sensor.
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
 * Modified by Brent Rubell for Adafruit Industries for use with Cubic PM1006
 * Air Quality Sensor.
 *
 * @section license License
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_PM25AQI.h"
#include "Adafruit_PM25AQI_I2C.h"
#include "Adafruit_PM25AQI_UART.h"

/*!
 *  @brief  Instantiates a new PM25AQI class
 */
Adafruit_PM25AQI::Adafruit_PM25AQI() { _aqi_utils = new Adafruit_AQIUtils(); }

/*!
 *  @brief  Default implementation of begin() - this is a no-op
 *          since this class is meant to be specialized.
 *  @return Always returns false, derived classes must implement
 */
bool Adafruit_PM25AQI::begin() {
  return false; // Base class can't be directly instantiated
}

Adafruit_PM25AQI::~Adafruit_PM25AQI() {
  if (_aqi_utils) {
    delete _aqi_utils;
    _aqi_utils = nullptr;
  }

  if (_pm25_i2c != nullptr) {
    delete _pm25_i2c;
    _pm25_i2c = nullptr;
  }

  if (_pm25_uart != nullptr) {
    delete _pm25_uart;
    _pm25_uart = nullptr;
  }
}

/*!
 *  @brief  Converts the PM2.5 raw data to country-specific AQI values
 *          and fills the PM25_AQI_Data struct with the results.
 *  @param  data
 *          Pointer to PM25_AQI_Data struct.
 */
void Adafruit_PM25AQI::ConvertAQIData(PM25_AQI_Data *data) {
  data->aqi_pm25_us = _aqi_utils->pm25_aqi_us(data->pm25_env);
  data->aqi_pm25_china = _aqi_utils->pm25_aqi_china(data->pm25_env);
  data->aqi_pm100_us = _aqi_utils->pm100_aqi_us(data->pm100_env);
  data->aqi_pm100_china = _aqi_utils->pm100_aqi_china(data->pm100_env);
}

/*!
 *  @brief  Setups the hardware and detects a valid PMSA003I. Initializes I2C.
 *  @param  theWire
 *          Optional pointer to I2C interface, otherwise use Wire
 *  @return True if PMSA003I found on I2C, False if something went wrong!
 */
bool Adafruit_PM25AQI::begin_I2C(TwoWire *theWire) {
  if (_pm25_i2c != nullptr) {
    return false;
  }

  _pm25_i2c = new Adafruit_PM25AQI_I2C();
  if (!_pm25_i2c) {
    return false;
  }
  // Attempt to initialize the I2C PM2.5 sensor
  return _pm25_i2c->begin(theWire);
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5
 *  @param  theSerial
 *          Pointer to Stream (HardwareSerial/SoftwareSerial) interface
 *  @param  is_pm1006
 *         True the sensor is a Cubic PM1006, False otherwise.
 *  @return True
 */
bool Adafruit_PM25AQI::begin_UART(Stream *theSerial, bool is_pm1006) {
  if (_pm25_uart != nullptr) {
    return false;
  }
  _pm25_uart = new Adafruit_PM25AQI_UART();
  return _pm25_uart->begin(theSerial);
}

/*!
 *  @brief  Default implementation of read() that delegates to the
 * implementation-specific classes
 *  @param  data
 *          Pointer to PM25_AQI_Data that will be filled by read()ing
 *  @return True on successful read, false if timed out or bad data
 */
bool Adafruit_PM25AQI::read(PM25_AQI_Data *data) {
  if (_pm25_i2c != nullptr) {
    return _pm25_i2c->read(data);
  } else if (_pm25_uart != nullptr) {
    return _pm25_uart->read(data);
  }
  return false;
}