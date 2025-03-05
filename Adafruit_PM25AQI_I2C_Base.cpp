#include "Adafruit_PM25AQI_I2C_Base.h"

Adafruit_PM25AQI_I2C_Base::~Adafruit_PM25AQI_I2C_Base() {
  if (i2c_dev) {
    delete i2c_dev;
    i2c_dev = nullptr;
  }
}

bool Adafruit_PM25AQI_I2C_Base::begin_I2C(TwoWire *theWire, uint8_t addr) {
  if (i2c_dev) {
    delete i2c_dev;
    i2c_dev = nullptr;
  }
  i2c_dev = new Adafruit_I2CDevice(addr, theWire);
  return i2c_dev->begin();
}
