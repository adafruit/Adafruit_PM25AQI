#ifndef ADAFRUIT_PM25AQI_PMSA003I_H
#define ADAFRUIT_PM25AQI_PMSA003I_H

#include "Adafruit_PM25AQI_I2C_Base.h"

#define PMSA003I_I2CADDR_DEFAULT 0x12

/*!
 *  @brief  PM2.5 sensor driver for Plantower PMSA003I
 */
class Adafruit_PM25AQI_PMSA003I : public Adafruit_PM25AQI_I2C_Base {
public:
  virtual bool read(PM25_AQI_Data *data) override;
};

#endif
