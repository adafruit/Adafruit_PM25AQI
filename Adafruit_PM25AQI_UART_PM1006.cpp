#include "Adafruit_PM25AQI_UART_PM1006.h"

Adafruit_PM25AQI_UART_PM1006::Adafruit_PM25AQI_UART_PM1006() {
  PM25AQI_DEBUG_PRINTLN("Adafruit_PM25AQI_UART_PM1006::Adafruit_PM25AQI_UART_PM1006");
  _bufferSize = 20;
  if (_buffer) {
    delete[] _buffer;
  }
  _buffer = new uint8_t[_bufferSize];
}

bool Adafruit_PM25AQI_UART_PM1006::verify_starting_bytes(uint8_t *buffer) {
  PM25AQI_DEBUG_PRINTLN("Adafruit_PM25AQI_UART_PM1006::verify_starting_bytes");
  return (buffer[0] == 0x16 && buffer[1] == 0x11 && buffer[2] == 0x0B);
}

bool Adafruit_PM25AQI_UART_PM1006::verify_checksum(uint8_t *buffer, size_t bufLen) {
  PM25AQI_DEBUG_PRINTLN("Adafruit_PM25AQI_UART_PM1006::verify_checksum");
  uint8_t sum = 0;
  for (uint8_t i = 0; i < bufLen; i++) {
    sum += buffer[i];
  }
  return sum == 0;
}

void Adafruit_PM25AQI_UART_PM1006::decode_data(uint8_t *buffer, PM25_AQI_Data *data) {
  PM25AQI_DEBUG_PRINTLN("Adafruit_PM25AQI_UART_PM1006::decode_data");
  // PM1006 only provides PM2.5 data
  data->pm25_env = (buffer[5] << 8) | buffer[6];
  
  // Clear other fields
  memset(data, 0, sizeof(PM25_AQI_Data));
  data->pm25_env = (buffer[5] << 8) | buffer[6];
}
