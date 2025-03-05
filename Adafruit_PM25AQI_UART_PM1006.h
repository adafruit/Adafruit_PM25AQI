#ifndef ADAFRUIT_PM25AQI_UART_PM1006_H
#define ADAFRUIT_PM25AQI_UART_PM1006_H

#include "Adafruit_PM25AQI_UART_Base.h"

/*!
 *  @brief  PM2.5 sensor driver for Cubit PM1006
 */
class Adafruit_PM25AQI_UART_PM1006 : public Adafruit_PM25AQI_UART_Base {
public:
  Adafruit_PM25AQI_UART_PM1006();

protected:
  // Must override these because PM1006 protocol is different
  bool verify_starting_bytes(uint8_t *buffer) override;
  bool verify_checksum(uint8_t *buffer, size_t bufLen) override;
  void decode_data(uint8_t *buffer, PM25_AQI_Data *data) override;
};

#endif
