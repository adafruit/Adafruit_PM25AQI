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
    if (serial_dev->read() != 0x42) { // read assuming you got the first byte
      return false;
    }
    // now catching 1 in 256 edge-case where last byte of checksum is 0x42 too,
    // being mistaken for first start byte, leading to byte shift and checksum
    // mismatch
    if (serial_dev->peek() ==
        0x42) {           // peek if next byte might match first start byte
      serial_dev->read(); // if yes last assumption was wrong, read again
                          // assuming you now got the start
    }
    if (serial_dev->read() != 0x4d) { // read & check the second byte
      return false;
    }
    // Now the first two bytes are guaranteed to be 0x42 0x4d but have already
    // been read
    buffer[0] = 0x42;
    buffer[1] = 0x4d;
    // Now read remaining 30 bytes
    if (serial_dev->available() < 30) {
      return false;
    }
    for (uint8_t i = 2; i < 32; i++) {
      buffer[i] = serial_dev->read();
    }
  } else {
    return false;
  }

  // Check that start byte is correct!
  if (buffer[0] != 0x42) {
    return false;
  }

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // The data comes in endian'd, this solves it so it works on all platforms
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

  // success!
  return true;
}
