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
#include "Adafruit_PM25AQI.h"

/*!
 *  @brief  Creates a new UARTDevice object.
 *  @param  serial
 *          Pointer to a Stream object (e.g., Serial, SoftwareSerial).
 */
UARTDevice::UARTDevice(Stream *serial) { _serial_dev = serial; }

/*!
 *  @brief  Dtor for the UARTDevice class.
 */
UARTDevice::~UARTDevice() {
  if (_generic_dev != nullptr) {
    delete _generic_dev;
    _generic_dev = nullptr;
  }
  if (_serial_dev != nullptr) {
    _serial_dev = nullptr;
  }
}

/*!
 *  @brief  Initializes a new GenericDevice instance using the uart_* callbacks.
 */
bool UARTDevice::CreateDevice() {
  if (_generic_dev != nullptr) {
    return false; // already initialized
  }
  _generic_dev = new Adafruit_GenericDevice(this, uart_read, uart_write);
  return _generic_dev->begin();
}

/*!
 *  @brief  Returns the next byte (character) of incoming serial data without
 * removing it from the internal serial buffer
 *  @return The first byte of incoming serial data available (or -1 if no data
 * is available).
 */
int UARTDevice::peek() {
  if (_serial_dev != nullptr) {
    return _serial_dev->peek();
  }
  return -1;
}

/*!
 *  @brief  Gets the number of bytes (characters) available for reading from the
 * serial port.
 *  @return The number of bytes available to read
 */
int UARTDevice::available() {
  if (_serial_dev != nullptr) {
    return _serial_dev->available();
  }
  return 0;
}

/*!
 *  @brief  Reads the next byte (character) from the serial port and removes it
 * from the internal serial buffer.
 *  @return The first byte of incoming serial data available (or -1 if no data
 * is available).
 */
int UARTDevice::read() {
  if (_serial_dev != nullptr) {
    return _serial_dev->read();
  }
  return -1;
}

/*!
 *  @brief  Static callback for writing data to UART, called by GenericDevice
 * whenever data needs to be sent.
 *  @param  thiz
 *          Pointer to the UARTDevice instance.
 *  @param  buffer
 *          Pointer to the data buffer to write.
 *  @param  len
 *          Length of the data buffer.
 *  @return True if successful, false otherwise.
 */
bool UARTDevice::uart_write(void *thiz, const uint8_t *buffer, size_t len) {
  UARTDevice *dev = (UARTDevice *)thiz;
  dev->_serial_dev->write(buffer, len);
  return true;
}

/*!
 *  @brief  Static callback for reading data from UART, called by GenericDevice
 * whenever data needs to be read.
 *  @param  thiz
 *          Pointer to the UARTDevice instance.
 *  @param  buffer
 *          Pointer to the buffer where data will be stored.
 *  @param  len
 *          Length of the data to read.
 *  @return True if data is read successfully, false if a timeout occured.
 */
bool UARTDevice::uart_read(void *thiz, uint8_t *buffer, size_t len) {
  UARTDevice *dev = (UARTDevice *)thiz;
  uint16_t timeout = 100;
  while (dev->_serial_dev->available() < len && timeout--) {
    delay(1);
  }
  if (timeout == 0) {
    return false;
  }
  for (size_t i = 0; i < len; i++) {
    buffer[i] = dev->_serial_dev->read();
  }
  return true;
}

/*!
 *  @brief  Ctor for the Adafruit_PM25AQI_UART class.
 */
Adafruit_PM25AQI_UART::Adafruit_PM25AQI_UART(bool is_pm1006) {
  _is_pm1006 = is_pm1006;
}

/*!
 *  @brief  Dtor for the Adafruit_PM25AQI_UART class.
 */
Adafruit_PM25AQI_UART::~Adafruit_PM25AQI_UART() {
  if (_uart_dev != nullptr) {
    delete _uart_dev;
    _uart_dev = nullptr;
  }
}

/*!
 *  @brief  Attempts to initialize a PMSA003I sensor on a specified I2C bus.
 *  @return True if successfully initialized, false otherwise.
 */
bool Adafruit_PM25AQI_UART::begin(Stream *theSerial) {
  if (_serial_dev != nullptr || _uart_dev != nullptr)
    return false; // already initialized

  _serial_dev = theSerial;
  _uart_dev = new UARTDevice(_serial_dev);
  return _uart_dev->CreateDevice();
}

/*!
 *  @brief  Attempts to read PM2.5 data from an Adafruit AQ sensor.
 *  @param  data
 *          Pointer to PM25_AQI_Data struct.
 *  @return True on successful read, False if timed out or bad data.
 */
bool Adafruit_PM25AQI_UART::read_PM25(PM25_AQI_Data *data) {
  uint8_t buffer[32];
  size_t bufLen = sizeof(buffer);
  uint16_t sum = 0;
  uint8_t csum = 0;

  if (!data || !_uart_dev) {
    return false;
  }

  if (!_uart_dev->available()) {
    return false;
  }

  int skipped = 0;
  while ((skipped < 32) && (_uart_dev->peek() != 0x42)) {
    _uart_dev->read();
    skipped++;
    if (!_uart_dev->available()) {
      return false;
    }
  }

  // Check for the start character in the stream
  if ((_uart_dev->peek() != 0x42)) {
    _uart_dev->read();
    return false;
  }

  // Are there enough bytes to read from?
  if (_uart_dev->available() < bufLen)
    return false;

  // Read all available bytes from the serial stream
  _uart_dev->getGenericDevice()->read(buffer, bufLen);

  // Validate start byte is correct if using Adafruit PM sensors
  if (buffer[0] != 0x42 || buffer[1] != 0x4d)
    return false;

  // Calculate checksum
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // Since header and checksum are OK, parse data from the buffer
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }
  // put it into a nice struct :)
  memcpy((void *)data, (void *)buffer_u16, 30);

  // Validate checksum
  if (sum != data->checksum)
    return false;

  // convert raw concentrations to AQI using parent class method
  this->ConvertAQIData(data);

  // success!
  return true;
}

/*!
 *  @brief  Attempts to read PM2.5 data from the Cubic PM1006 sensor.
 *  @param  data
 *          Pointer to PM25_AQI_Data struct.
 *  @return True on successful read, False if timed out or bad data.
 */
bool Adafruit_PM25AQI_UART::read_PM1006(PM25_AQI_Data *data) {
  uint8_t buffer[20];
  size_t bufLen = 20;
  uint16_t sum = 0;
  uint8_t csum = 0;

  if (!data || !_uart_dev) {
    return false;
  }

  if (!_uart_dev->available()) {
    return false;
  }

  int skipped = 0;
  while ((skipped < 32) && (_uart_dev->peek() != 0x16) &&
         (_uart_dev->peek() != 0x42)) {
    _uart_dev->read();
    skipped++;
    if (!_uart_dev->available()) {
      return false;
    }
  }

  // Check for the start character in the stream
  if ((_uart_dev->peek() == 16)) {
    _uart_dev->read();
    return false;
  }

  // Are there enough bytes to read from?
  if (_uart_dev->available() < bufLen) {
    return false;
  }

  // Read all available bytes from the serial stream
  _uart_dev->getGenericDevice()->read(buffer, bufLen);

  // Validate start header is correct if using Cubic PM1006 sensor
  if (buffer[0] != 0x16 || buffer[1] != 0x11 || buffer[2] != 0x0B) {
    return false;
  }

  // Calculate checksum
  for (uint8_t i = 0; i < bufLen; i++) {
    csum += buffer[i];
  }

  // Since header and checksum are OK, parse data from the buffer
  // (Cubic PM1006 sensor only produces a pm25_env reading)
  data->pm25_env = (buffer[5] << 8) | buffer[6];
  data->checksum = sum;

  // Validate checksum
  if (csum != 0) {
    return false;
  }

  // convert raw concentrations to AQI using parent class method
  this->ConvertAQIData(data);

  // success!
  return true;
}

/*!
 *  @brief  Attempts to read PM2.5 data from the AQ sensor.
 *  @param  data
 *          Pointer to PM25_AQI_Data struct.
 *  @return True on successful read, False if timed out or bad data.
 */
bool Adafruit_PM25AQI_UART::read(PM25_AQI_Data *data) {
  if (_is_pm1006) {
    return read_PM1006(data);
  } else {
    return read_PM25(data);
  }
}