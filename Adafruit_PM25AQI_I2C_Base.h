#ifndef ADAFRUIT_PM25AQI_I2C_BASE_H
#define ADAFRUIT_PM25AQI_I2C_BASE_H

#include "Adafruit_PM25AQI_Base.h"
#include <Adafruit_I2CDevice.h>

/*!
 *  @brief  Base class for PM2.5 sensor driver for I2C
 */
class Adafruit_PM25AQI_I2C_Base : public Adafruit_PM25AQI_Base {
public:
  virtual ~Adafruit_PM25AQI_I2C_Base() override;
  bool begin_I2C(TwoWire *theWire, uint8_t addr) override;
  bool begin_UART(Stream *SerialStream) override { return false; }

protected:
  Adafruit_I2CDevice *i2c_dev = nullptr; ///< The I2C device
};

#endif
