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
  bool is_pm1006 = false;
  uint8_t bufLen = 32;

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
    // Find the start character in the stream (0x42 for Adafruit PM sensors,
    // 0x16 for the Cubic PM1006)
    Serial.println("L91");
    while ((skipped < 32) && (serial_dev->peek() != 0x42) &&
           (serial_dev->peek() != 0x16)) {
      serial_dev->read();
      Serial.print("peek L94: ");
      Serial.println(serial_dev->peek());
      delay(15);
      skipped++;
      if (!serial_dev->available()) {
        return false;
      }
    }
    // Check for the start character in the stream
    Serial.println("Checking for start character in stream");
    if ((serial_dev->peek() != 0x42) || (serial_dev->peek() != 0x16)) {
      Serial.println("ERROR: Did not find start character in stream!");
      Serial.println(serial_dev->peek());
      serial_dev->read();
      delay(15);
      return false; // We did not find the start character
    }
    // Are we using the PM1006?
    Serial.println("Checking for PM1006");
    if (serial_dev->peek() == 0x16) {
      is_pm1006 = true;
    }

    // TODO: Can we do the check above in here instead to optimize?
    Serial.println("Reading all bytes");
    if (!is_pm1006) {
      // Start character was found, now read all 32 bytes
      if (serial_dev->available() < 32) {
        return false;
      }
      serial_dev->readBytes(buffer, 32);
    } else {
      // Start character was found, now read 20 bytes
      // Note: PM1006 only requires 20 bytes, but we will ignore the last 12
      // bytes when parsing
      if (serial_dev->available() < 20) {
        return false;
      }
      Serial.println("reading pm1006 all bytes");
      serial_dev->readBytes(buffer, 20);
    }
  } else {
    return false;
  }

  Serial.println("Validating start bytes");
  // Validate start byte is correct for Adafruit PM sensors
  if ((!is_pm1006 && buffer[0] != 0x42)) {
    return false;
  }

  // Validate start header is correct for Cubic PM1006
  if (is_pm1006 &&
      (buffer[0] != 0x16 || buffer[1] != 0x11 || buffer[2] != 0x0B)) {
    return false;
  }

  // TODO: Could we make this better?
  if (is_pm1006) {
    bufLen = 20;
  }

  // Calculate the checksum
  for (uint8_t i = 0; i < bufLen; i++) {
    sum += buffer[i];
  }
  // Validate checksum
  if ((is_pm1006 && sum != 0) || (!is_pm1006 && sum != data->checksum)) {
    return false; // Invalid checksum
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
    // Parse from PM1006
    // NOTE: PM1006 only produces a pm25_env reading!
    data->pm25_env = (buffer[5] << 8) | buffer[6];
    data->checksum = sum;
  }

  // success!
  return true;
}
