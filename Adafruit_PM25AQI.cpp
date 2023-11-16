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
  size_t bufLen = sizeof(buffer);
  uint16_t sum = 0;
  uint8_t csum = 0;
  bool is_pm1006 = false;

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
    while ((skipped < 32) && (serial_dev->peek() != 0x42) &&
           (serial_dev->peek() != 0x16)) {
      serial_dev->read();
      skipped++;
      if (!serial_dev->available()) {
        return false;
      }
    }

    // Check for the start character in the stream for both sensors
    if ((serial_dev->peek() != 0x42) && (serial_dev->peek() != 0x16)) {
      serial_dev->read();
      return false;
    }

    // Are we using the Cubic PM1006 sensor?
    if (serial_dev->peek() == 0x16) {
      is_pm1006 = true; // Set flag to indicate we are using the PM1006
      bufLen =
          20; // Reduce buffer read length to 20 bytes. Last 12 bytes ignored.
    }

    // Are there enough bytes to read from?
    if (serial_dev->available() < bufLen) {
      return false;
    }

    // Read all available bytes from the serial stream
    serial_dev->readBytes(buffer, bufLen);
  } else {
    return false;
  }

  // Validate start byte is correct if using Adafruit PM sensors
  if ((!is_pm1006 && buffer[0] != 0x42)) {
    return false;
  }

  // Validate start header is correct if using Cubic PM1006 sensor
  if (is_pm1006 &&
      (buffer[0] != 0x16 || buffer[1] != 0x11 || buffer[2] != 0x0B)) {
    return false;
  }

  // Calculate checksum
  if (!is_pm1006) {
    for (uint8_t i = 0; i < 30; i++) {
      sum += buffer[i];
    }
  } else {
    for (uint8_t i = 0; i < bufLen; i++) {
      csum += buffer[i];
    }
  }

  // Since header and checksum are OK, parse data from the buffer
  if (!is_pm1006) {
    // The data comes in endian'd, this solves it so it works on all platforms
    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
      buffer_u16[i] = buffer[2 + i * 2 + 1];
      buffer_u16[i] += (buffer[2 + i * 2] << 8);
    }
    // put it into a nice struct :)
    memcpy((void *)data, (void *)buffer_u16, 30);
  } else {
    // Cubic PM1006 sensor only produces a pm25_env reading
    data->pm25_env = (buffer[5] << 8) | buffer[6];
    data->checksum = sum;
  }

  // Validate checksum
  if ((is_pm1006 && csum != 0) || (!is_pm1006 && sum != data->checksum)) {
    return false;
  }

  // success!
  return true;
}
