#ifndef ADAFRUIT_PM25AQI_UART_BASE_H
#define ADAFRUIT_PM25AQI_UART_BASE_H

#include "Adafruit_PM25AQI.h"

class Adafruit_PM25AQI_UART_Base : public Adafruit_PM25AQI_Base {
public:
  ~Adafruit_PM25AQI_UART_Base() override;  // Need this for serial_dev cleanup
  bool begin_I2C(TwoWire *theWire = &Wire, uint8_t addr = 0x12) override { return false; }
  virtual bool begin_UART(Stream *theStream) override;
  virtual bool read(PM25_AQI_Data *data) override;

protected:
  // virtual bool verify_starting_bytes(uint8_t *buffer) override;
  // virtual bool verify_checksum(uint8_t *buffer, size_t bufLen) override;
  // virtual void decode_data(uint8_t *buffer, PM25_AQI_Data *data) override;
  bool read_uart_data(uint8_t *buffer, size_t bufLen);
  
  Stream *serial_dev = nullptr;
};

#endif
