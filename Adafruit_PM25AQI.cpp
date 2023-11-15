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
    // TODO: Test compatibilituy with adafruit pm25 uart sensor
    // TOOD: Test with adafruit pm25 i2c sensor
    // TODO: Test with cubic pm1006 sensor

    // TODO: go back and re-read all this code

    // Find the start character in the stream (0x42 for Adafruit PM sensors,
    // 0x16 for the Cubic PM1006)
    // TODO: Put back check for cubic!
    int skipped = 0;
    while ((skipped < 20) && (serial_dev->peek() != 0x16)) {
      serial_dev->read() skipped++;
      if (!serial_dev->available()) {
        return false;
      }
    }

    // Check for the start character in the stream for the Cubic PM1006
    // TODO: put back check for adafruit AQ
    if ((serial_dev->peek() != 0x16)) {
      serial_dev->read();
      return false; // We did not find the start character
    }
    // Are we using the PM1006?
    if (serial_dev->peek() == 0x16) {
      is_pm1006 = true; // Set flag
      bufLen = 20; // Reduce buffer read length to 20 bytes. Last 12 bytes will
                   // be ignored.
    }

    // Are there enough bytes to read from?
    if (serial_dev->available() < bufLen)
      return false;
    // Start character was found, now read the desired amount of bytes
    serial_dev->readBytes(buffer, 32);

  } else {
    return false;
  }

  // Validate start byte is correct for Adafruit PM sensors
  if ((!is_pm1006 && buffer[0] != 0x42)) {
    return false;
  }

  // Validate start header is correct for Cubic PM1006
  if (is_pm1006 &&
      (buffer[0] != 0x16 || buffer[1] != 0x11 || buffer[2] != 0x0B)) {
    return false;
  }

  // Calculate checksum
  // TODO: This is only for PM1006, the PM25 sensor uses int for sum!
  uint8_t csum = 0;
  for (uint8_t i = 0; i < bufLen; i++) {
    csum += buffer[i];
  }

  // Validate checksum
  if ((is_pm1006 && csum != 0) || (!is_pm1006 && sum != data->checksum)) {
    return false; // Invalid checksum!
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
    // Parse from PM1006. This sensor only produces a pm25_env reading!
    data->pm25_env = (buffer[5] << 8) | buffer[6];
    data->checksum = sum;
  }

  // success!
  return true;
}
