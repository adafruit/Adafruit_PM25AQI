#ifndef ADAFRUIT_PM25AQI_BASE_H
#define ADAFRUIT_PM25AQI_BASE_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#if 1
#define PM25AQI_DEBUG_PRINT(...)                                               \
  Serial.print(__VA_ARGS__);                                                   \
  Serial.flush()
#define PM25AQI_DEBUG_PRINTLN(...)                                             \
  Serial.println(__VA_ARGS__);                                                 \
  Serial.flush()
#else
#define PM25AQI_DEBUG_PRINT(...)
#define PM25AQI_DEBUG_PRINTLN(...)
#endif

/**! Structure holding Plantower's standard packet **/
typedef struct PMSAQIdata {
  uint16_t framelen;       ///< How long this data chunk is
  uint16_t pm10_standard,  ///< Standard PM1.0
      pm25_standard,       ///< Standard PM2.5
      pm100_standard;      ///< Standard PM10.0
  uint16_t pm10_env,       ///< Environmental PM1.0
      pm25_env,            ///< Environmental PM2.5
      pm100_env;           ///< Environmental PM10.0
  uint16_t particles_03um, ///< 0.3um Particle Count
      particles_05um,      ///< 0.5um Particle Count
      particles_10um,      ///< 1.0um Particle Count
      particles_25um,      ///< 2.5um Particle Count
      particles_50um,      ///< 5.0um Particle Count
      particles_100um;     ///< 10.0um Particle Count
  uint16_t unused;         ///< Unused (version + error code)

  uint16_t checksum; ///< Packet checksum

  // AQI conversion results:
  uint8_t aqi_pm25_us;     ///< pm2.5 AQI of United States
  uint8_t aqi_pm100_us;    ///< pm10 AQI of United States
  uint8_t aqi_pm25_china;  ///< pm2.5 AQI of China
  uint8_t aqi_pm100_china; ///< pm10 AQI of China

} PM25_AQI_Data;

/*!
 * Base class with state/helpers for interacting with PM2.5 Air Quality Sensor
 */
class Adafruit_PM25AQI_Base {
public:
  Adafruit_PM25AQI_Base();
  virtual ~Adafruit_PM25AQI_Base();

  /*!
   *  @brief  Setups the hardware and detects a valid PMSA003I. Initializes I2C.
   *  @param  theWire
   *          Optional pointer to I2C interface, otherwise use Wire
   *  @param  addr
   *          Optional I2C address, default is PMSA003I_I2CADDR_DEFAULT (0x12)
   *  @return True if PMSA003I found on I2C, False if something went wrong!
   */
  virtual bool begin_I2C(TwoWire *theWire = &Wire, uint8_t addr = 0x12) = 0;

  /*!
   *  @brief  Setups the hardware and detects a valid UART PM2.5 sensor
   *  @param  theStream
   *          Pointer to Stream (HardwareSerial/SoftwareSerial) interface
   *  @return True
   */
  virtual bool begin_UART(Stream *theStream) = 0;

  /*!
   *  @brief  Read the sensor data into the data struct
   *  @param  data
   *          The data struct to fill
   *  @return True if the data was read successfully, false if there was an
   * error
   */
  virtual bool read(PM25_AQI_Data *data);

  /*!
   *  @brief  Verify the starting bytes of the data packet
   *  @param  buffer
   *          The buffer with the data from the sensor
   *  @return True if the starting bytes are correct, false if they are
   * incorrect
   */
  virtual bool verify_starting_bytes(uint8_t *buffer);

  /*!
   *  @brief  Verify the checksum of the data packet
   *  @param  buffer
   *          The buffer with the data from the sensor
   *  @param  bufLen
   *          The length of the buffer
   *  @return True if the checksum is correct, false if it is incorrect
   */
  virtual bool verify_checksum(uint8_t *buffer, size_t bufLen);

  /*!
   *  @brief  Decode the data from the sensor into the data struct
   *  @param  buffer
   *          The buffer with the data from the sensor
   *  @param  data
   *          The data struct to fill
   */
  virtual void decode_data(uint8_t *buffer, PM25_AQI_Data *data);

  /*!
   *  @brief  Get AQI of PM2.5 in US standard
   *  @param  concentration
   *          the environmental concentration of pm2.5 in ug/m3
   *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of
   * range.
   */
  uint16_t pm25_aqi_us(float concentration);

  /*!
   *  @brief  Get AQI of PM10 in US standard
   *  @param  concentration
   *          the environmental concentration of pm10 in ug/m3
   *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of
   * range.
   */
  uint16_t pm25_aqi_china(float concentration);

  /*!
   *  @brief  Get AQI of PM2.5 in China standard
   *  @param  concentration
   *          the environmental concentration of pm2.5 in ug/m3
   *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of
   * range.
   */
  uint16_t pm100_aqi_us(float concentration);

  /*!
   *  @brief  Get AQI of PM10 in China standard
   *  @param  concentration
   *          the environmental concentration of pm10 in ug/m3
   *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of
   * range.
   */
  uint16_t pm100_aqi_china(float concentration);

  /*!
   *  @brief  Linearly map a concentration value to its AQI level
   *  @param  aqi_high max aqi of the calculating range
   *  @param  aqi_low min aqi of the calculating range
   *  @param  conc_high max concentration value (ug/m3) of the calculating range
   *  @param  conc_low min concentration value (ug/m3) of the calculating range
   *  @param  concentration
   *          the concentration value to be calculated
   *  @return Calculated AQI value
   */
  float linear(uint16_t aqi_high, uint16_t aqi_low, float conc_high,
               float conc_low, float concentration);

protected:
  uint8_t _bufferSize = 32;   ///< Size of the data buffer
  uint8_t *_buffer = nullptr; ///< Data buffer
};

#endif
