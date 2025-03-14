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
#include "Adafruit_PM25AQI_Base.h"
#include "Adafruit_PM25AQI_PMSA003I.h"
#include "Adafruit_PM25AQI_UART_PM1006.h"
#include "Adafruit_PM25AQI_UART_PMS5003.h"
#include <math.h>

/*!
 *  @brief  Instantiates a new PM25AQI class
 */
Adafruit_PM25AQI::Adafruit_PM25AQI() { driver = nullptr; }

Adafruit_PM25AQI::~Adafruit_PM25AQI() {
  if (driver) {
    delete driver;
    driver = nullptr;
  }
}

/*!
 *  @brief  Setups the hardware and detects a valid PMSA003I. Initializes I2C.
 *  @param  theWire
 *          Optional pointer to I2C interface, otherwise use Wire
 *  @param  addr
 *          Optional I2C address, default is PMSA003I_I2CADDR_DEFAULT (0x12)
 *  @return True if PMSA003I found on I2C, False if something went wrong!
 */
bool Adafruit_PM25AQI::begin_I2C(TwoWire *theWire, uint8_t addr) {
  if (driver) {
    delete driver;
    driver = nullptr;
  }
  driver = new Adafruit_PM25AQI_PMSA003I();
  return driver->begin_I2C(theWire, addr);
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5 sensor
 *  @param  SerialStream
 *          Pointer to Stream (HardwareSerial/SoftwareSerial) interface
 *  @return True if valid sensor start bytes found, else false
 */
bool Adafruit_PM25AQI::begin_UART(Stream *SerialStream) {
  if (driver) {
    delete driver;
    driver = nullptr;
  }
  uint8_t retries = 0;
  while (retries < 32) {
    for (uint8_t i = 0; i < 32; i++) {
      int peek_byte = SerialStream->peek();
      if (SerialStream->available() || peek_byte != -1) {
        if (peek_byte == 0x42) {
          PM25AQI_DEBUG_PRINTLN(
              "[Adafruit_PM25AQI::begin_UART] Suspected PMS5003");
          driver = new Adafruit_PM25AQI_UART_PMS5003();
          PM25AQI_DEBUG_PRINTLN(
              "[Adafruit_PM25AQI::begin_UART] Trying to begin PMS5003 UART");
          return driver->begin_UART(SerialStream);
        } else if (peek_byte == 0x16) {
          PM25AQI_DEBUG_PRINTLN(
              "[Adafruit_PM25AQI::begin_UART] Suspected PM1006");
          driver = new Adafruit_PM25AQI_UART_PM1006();
          PM25AQI_DEBUG_PRINTLN(
              "[Adafruit_PM25AQI::begin_UART] Trying to begin PM1006 UART");
          return driver->begin_UART(SerialStream);
        } else {
          PM25AQI_DEBUG_PRINT("[Adafruit_PM25AQI::begin_UART] Skipping byte: ");
          PM25AQI_DEBUG_PRINTLN(peek_byte, 16);
          // Read the byte to skip and keep going
          SerialStream->read();
        }
        PM25AQI_DEBUG_PRINTLN("[Adafruit_PM25AQI::begin_UART] Invalid start "
                              "byte, trying the next byte");
      } else {
        PM25AQI_DEBUG_PRINTLN("[Adafruit_PM25AQI::begin_UART] No serial data "
                              "available, retrying in 1ms");
        delay(1);
      }
    }
    PM25AQI_DEBUG_PRINT("[Adafruit_PM25AQI::begin_UART] No valid start byte "
                        "found in 32bytes, retrying ");
    PM25AQI_DEBUG_PRINT(retries++);
    PM25AQI_DEBUG_PRINTLN(" of 32");
  }
  return false;
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5
 *  @param  data
 *          Pointer to PM25_AQI_Data that will be filled by read()ing
 *  @return True on successful read, false if timed out or bad data
 */
bool Adafruit_PM25AQI::read(PM25_AQI_Data *data) {
  if (!driver) {
    PM25AQI_DEBUG_PRINTLN("[Adafruit_PM25AQI::read] No driver available");
    return false;
  }
  if (!data) {
    PM25AQI_DEBUG_PRINTLN("[Adafruit_PM25AQI::read] No data buffer available");
    return false;
  }
  PM25AQI_DEBUG_PRINTLN("[Adafruit_PM25AQI::read] Reading data via driver");
  return driver->read(data);
}
