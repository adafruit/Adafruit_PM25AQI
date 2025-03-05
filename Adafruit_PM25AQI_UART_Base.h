#ifndef ADAFRUIT_PM25AQI_UART_BASE_H
#define ADAFRUIT_PM25AQI_UART_BASE_H

#include "Adafruit_PM25AQI.h"

/*!
 *  @brief  Base class for PM2.5 sensor driver for UART
 */
class Adafruit_PM25AQI_UART_Base : public Adafruit_PM25AQI_Base {
public:
  ~Adafruit_PM25AQI_UART_Base() override; // Need this for serial_dev cleanup
  bool begin_I2C(TwoWire *theWire = &Wire, uint8_t addr = 0x12) override {
    return false;
  }
  virtual bool begin_UART(Stream *theStream) override;
  virtual bool read(PM25_AQI_Data *data) override;

protected:
  /*!
   *  @brief  Read data from the UART stream
   *  @param  buffer
   *          The buffer to read data into
   *  @param  bufLen
   *          The length of the buffer
   *  @return True if the data was read successfully, false if there was an
   * error
   */
  bool read_uart_data(uint8_t *buffer, size_t bufLen);

  Stream *serial_dev = nullptr; ///< The serial device
};

#endif
