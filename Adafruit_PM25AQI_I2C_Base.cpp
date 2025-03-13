#include "Adafruit_PM25AQI_I2C_Base.h"

/*!
 *  @brief  Destructor for I2C sensor base class (constructor calls base)
 */
Adafruit_PM25AQI_I2C_Base::~Adafruit_PM25AQI_I2C_Base() {
  if (i2c_dev) {
    delete i2c_dev;
    i2c_dev = nullptr;
  }
}

/*!
  *  @brief  Setups the hardware and detects a valid PMSA003I. Initializes I2C.
  *  @param  theWire
  *          Optional pointer to I2C interface, otherwise use Wire
  *  @param  addr
  *          Optional I2C address, default is PMSA003I_I2CADDR_DEFAULT (0x12)
  *  @return True if PMSA003I found on I2C, False if something went wrong!
  */
bool Adafruit_PM25AQI_I2C_Base::begin_I2C(TwoWire *theWire, uint8_t addr) {
  if (i2c_dev) {
    delete i2c_dev;
    i2c_dev = nullptr;
  }
  i2c_dev = new Adafruit_I2CDevice(addr, theWire);
  return i2c_dev->begin();
}
