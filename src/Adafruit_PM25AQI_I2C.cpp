#include "Adafruit_PM25AQI_I2C.h"
#include "Adafruit_PM25AQI.h"

/*!
 *  @brief  Ctor for the Adafruit_PM25AQI_I2C class.
 */
Adafruit_PM25AQI_I2C::Adafruit_PM25AQI_I2C() {
  // Nothing additional to initialize
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

/*!
 *  @brief  Attempts to read PM2.5 data from the AQ sensor.
 *  @param  data
 *          Pointer to PM25_AQI_Data struct.
 *  @return True on successful read, False if timed out or bad data.
 */
bool Adafruit_PM25AQI_I2C::read(PM25_AQI_Data *data) {
  uint8_t buffer[32];
  size_t bufLen = sizeof(buffer);
  uint16_t sum = 0;

  if (!data && _i2c_dev == nullptr) {
    return false; // Objects improperly initialized, early-out
  }

  if (!_i2c_dev->read(buffer, 32)) {
    return false; // I2C read failed, early-out
  }

  // Validate start byte is correct if using Adafruit PM sensors
  if (buffer[0] != 0x42 || buffer[1] != 0x4d) {
    return false;
  }

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
  if (sum != data->checksum) {
    return false;
  }

  // convert raw concentrations to AQI
  this->ConvertAQIData(data);

  // success!
  return true;
}