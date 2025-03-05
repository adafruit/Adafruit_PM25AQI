#include "Adafruit_PM25AQI_PMSA003I.h"

bool Adafruit_PM25AQI_PMSA003I::read(PM25_AQI_Data *data) {
  if (!i2c_dev || !data) {
    return false;
  }

  if (!i2c_dev->read(_buffer, _bufferSize)) {
    return false;
  }

  if (!verify_starting_bytes(_buffer) ||
      !verify_checksum(_buffer, _bufferSize)) {
    return false;
  }

  decode_data(_buffer, data);
  return true;
}
