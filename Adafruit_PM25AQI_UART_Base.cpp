#include "Adafruit_PM25AQI_UART_Base.h"

Adafruit_PM25AQI_UART_Base::~Adafruit_PM25AQI_UART_Base() {
  if (serial_dev) {
    delete serial_dev;
    serial_dev = nullptr;
  }
}

bool Adafruit_PM25AQI_UART_Base::begin_UART(Stream *theStream) {
  if (serial_dev) {
    delete serial_dev;
    serial_dev = nullptr;
  }
  serial_dev = theStream;
  return serial_dev != nullptr;
}

bool Adafruit_PM25AQI_UART_Base::read_uart_data(uint8_t *buffer,
                                                size_t bufLen) {
  if (!serial_dev) {
    PM25AQI_DEBUG_PRINTLN(
        "NO_DEVICE: No serial device available - unable to read_uart_data");
    return false;
  }
  if (!serial_dev->available() && (serial_dev->peek() == -1)) {
    PM25AQI_DEBUG_PRINTLN(
        "NO_DATA: No serial data available - unable to read_uart_data");
    return false;
  }

  // Skip until we find a potential start byte
  int skipped = 0;
  while ((skipped < 32) && (serial_dev->peek() != 0x42) &&
         (serial_dev->peek() != 0x16)) {
    PM25AQI_DEBUG_PRINT("SKIP: Saw byte ");
    PM25AQI_DEBUG_PRINTLN(serial_dev->peek(), HEX);
    serial_dev->read();
    skipped++;
    if (!serial_dev->available() && (serial_dev->peek() == -1)) {
      PM25AQI_DEBUG_PRINT("NO_DATA_SKIP: No serial data available - unable to "
                          "read_uart_data, skipped: ");
      PM25AQI_DEBUG_PRINTLN(skipped - 1);
      return false;
    }
  }

  // Are there enough bytes to read?
  if (serial_dev->available() < bufLen) {
    PM25AQI_DEBUG_PRINTLN("DATA_TOO_SHORT: Not enough serial data available - "
                          "unable to read_uart_data");
    return false;
  }

  // Read the data
  int8_t returned_bytes = serial_dev->readBytes(buffer, bufLen);
  if (serial_dev->available()) {
    PM25AQI_DEBUG_PRINT("EXTRA_DATA: Extra serial data available - in "
                        "read_uart_data, flushing: ");
    Serial.flush();
    PM25AQI_DEBUG_PRINTLN(serial_dev->available());
    Serial.flush();
    delay(10);
    while (serial_dev->available()) {
      serial_dev->read();
      yield(); // TODO: Tyeth - Remove and retest
    }
  }
  if (returned_bytes != bufLen) {
    PM25AQI_DEBUG_PRINT("DATA_READ_LEN_ERR: returned_bytes(");
    PM25AQI_DEBUG_PRINT(returned_bytes);
    PM25AQI_DEBUG_PRINT(") != bufLen(");
    PM25AQI_DEBUG_PRINT(bufLen);
    PM25AQI_DEBUG_PRINTLN(") - unable to read_uart_data");
    return false;
  }
  return true;
}

bool Adafruit_PM25AQI_UART_Base::read(PM25_AQI_Data *data) {
  PM25AQI_DEBUG_PRINT("Adafruit_PM25AQI_UART_Base::read (available: ");
  PM25AQI_DEBUG_PRINT(serial_dev->available());
  PM25AQI_DEBUG_PRINTLN(" - Buffer length: " + String(_bufferSize) + ")");
  if (!data || !read_uart_data(_buffer, _bufferSize)) {
    return false;
  }

  if (!verify_starting_bytes(_buffer) ||
      !verify_checksum(_buffer, _bufferSize)) {
    return false;
  }

  decode_data(_buffer, data);
  return true;
}
