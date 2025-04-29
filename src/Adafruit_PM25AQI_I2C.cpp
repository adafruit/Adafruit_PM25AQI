#include "Adafruit_PM25AQI_I2C.h"

/*!
 *  @brief  Ctor for the Adafruit_PM25AQI_I2C class.
 */
Adafruit_PM25AQI_I2C::Adafruit_PM25AQI_I2C() {
  // TODO
}

/*!
 *  @brief  Dtor for the Adafruit_PM25AQI_I2C class.
 */
Adafruit_PM25AQI_I2C::~Adafruit_PM25AQI_I2C() {
  if (_i2c_dev != nullptr) {
    delete _i2c_dev;
    _i2c_dev = nullptr;
  }
}

/*!
 *  @brief  Attempts to initialize a PMSA003I sensor on a specified I2C bus.
 *  @param  theWire
 *          Optionally provided pointer to an I2C interface, uses Wire by
 * default.
 *  @param  i2c_addr
 *          Optional I2C address, defaults to PMSA003I_DEFAULT_ADDRESS.
 *  @return True if successfully initialized, false otherwise.
 */
bool Adafruit_PM25AQI_I2C::begin(TwoWire *theWire, uint8_t i2c_addr) {
  if (_i2c_dev == nullptr) {
    _i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);
  }

  if (!_i2c_dev->begin()) {
    return false;
  }

  return true;
}
