/*!
 * @file Adafruit_PM25AQI_UART.cpp
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
#include "Adafruit_PM25AQI_UART.h"

/*!
 *  @brief  Ctor for the Adafruit_PM25AQI_UART class.
 */
Adafruit_PM25AQI_UART::Adafruit_PM25AQI_UART() {
  // Nothing additional to initialize
}

/*!
 *  @brief  Dtor for the Adafruit_PM25AQI_UART class.
 */
Adafruit_PM25AQI_UART::~Adafruit_PM25AQI_UART() {
  // TODO
}

/*!
 *  @brief  Attempts to initialize a PMSA003I sensor on a specified I2C bus.
 *  @return True if successfully initialized, false otherwise.
 */
bool Adafruit_PM25AQI_UART::begin(Stream *theSerial) {
  if (_serial_dev != nullptr)
    return false;

  _serial_dev = theSerial;
  return true;
}

/*!
 *  @brief  Attempts to read PM2.5 data from the AQ sensor.
 *  @param  data
 *          Pointer to PM25_AQI_Data struct.
 *  @return True on successful read, False if timed out or bad data.
 */
bool Adafruit_PM25AQI_UART::read(PM25_AQI_Data *data) {
  uint8_t buffer[32];
  size_t bufLen = sizeof(buffer);
  uint16_t sum = 0;
  uint8_t csum = 0;
  bool is_pm1006 = false;

  if (!data || (_serial_dev == nullptr)) {
    return false;
  }

  if (!_serial_dev->available()) {
    return false;
  }

  int skipped = 0;
  // Skip over garbage data until we find the expected start sequence
  while ((skipped < 32) && (_serial_dev->peek() != ADAFRUIT_PM_START_BYTE) &&
         (_serial_dev->peek() != PMSA003I_START_BYTE)) {
    _serial_dev->read();
    skipped++;
    if (!_serial_dev->available()) {
      return false;
    }

    // Check for the start character in the stream for both sensors
    if ((_serial_dev->peek() != ADAFRUIT_PM_START_BYTE) &&
        (_serial_dev->peek() != PMSA003I_START_BYTE)) {
      _serial_dev->read();
      return false;
    }

    // Are we using the Cubic PM1006 sensor?
    if (_serial_dev->peek() == PMSA003I_START_BYTE) {
      is_pm1006 = true; // Set flag to indicate we are using the PM1006
      bufLen =
          20; // Reduce buffer read length to 20 bytes. Last 12 bytes ignored.
    }

    // Are there enough bytes to read from?
    if (_serial_dev->available() < bufLen) {
      return false;
    }

    // Read all available bytes from the serial stream
    _serial_dev->readBytes(buffer, bufLen);

    // Validate start byte for Adafruit PM sensors
    if ((!is_pm1006 &&
         (buffer[0] != ADAFRUIT_PM_START_BYTE || buffer[1] != 0x4d))) {
      return false;
    }

    // Validate start header for Cubic PM1006 sensor
    if (is_pm1006 && (buffer[0] != PMSA003I_START_BYTE || buffer[1] != 0x11 ||
                      buffer[2] != 0x0B)) {
      return false;
    }

    // Calculate checksum
    if (!is_pm1006) {
      // for adafruit pm sensors
      for (uint8_t i = 0; i < 30; i++) {
        sum += buffer[i];
      }
    } else {
      // for cubic pm1006 sensor
      for (uint8_t i = 0; i < bufLen; i++) {
        csum += buffer[i];
      }
    }

    // Since header and checksum are OK, parse data from the buffer
    if (!is_pm1006) {
      // For adafruit pm sensors, data comes in endian'd, this solves it so it
      // works on all platforms
      uint16_t buffer_u16[15];
      for (uint8_t i = 0; i < 15; i++) {
        buffer_u16[i] = buffer[2 + i * 2 + 1];
        buffer_u16[i] += (buffer[2 + i * 2] << 8);
      }
      // put it into a nice struct :)
      memcpy((void *)data, (void *)buffer_u16, 30);
    } else {
      // Cubic PM1006 sensors only produce a pm25_env reading
      data->pm25_env = (buffer[5] << 8) | buffer[6];
      data->checksum = sum;
    }

    // Validate checksum
    if ((is_pm1006 && csum != 0) || (!is_pm1006 && sum != data->checksum)) {
      return false;
    }

    // convert raw concentrations to AQI using parent class method
    this->ConvertAQIData(data);

    // success!
    return true;
  }