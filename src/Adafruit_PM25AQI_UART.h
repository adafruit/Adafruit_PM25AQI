/*!
 * @file Adafruit_PM25AQI_UART.h
 *
 * This is the documentation for Adafruit's PM25 AQI driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit PM25 air quality sensors: http://www.adafruit.com/products/4632
 *
 * These sensors use I2C or UART to communicate.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef ADAFRUIT_PM25AQI_UART_H
#define ADAFRUIT_PM25AQI_UART_H
#include "Adafruit_GenericDevice.h"
#include "Adafruit_PM25AQI.h"

#define ADAFRUIT_PM_START_BYTE 0x42 ///< Start byte for Adafruit's PM25 sensors
#define PMSA003I_START_BYTE 0x16    ///< Start byte for Cubic PM1006

class UARTDevice {
public:
  UARTDevice(Stream *serial);
  ~UARTDevice();
  bool CreateDevice();
  int peek();
  int available();
  int read();
  static bool uart_write(void *thiz, const uint8_t *buffer, size_t len);
  static bool uart_read(void *thiz, uint8_t *buffer, size_t len);
  Adafruit_GenericDevice *getGenericDevice() { return _generic_dev; }

private:
  Adafruit_GenericDevice *_generic_dev = nullptr;
  Stream *_serial_dev = nullptr;
};

/*!
 *  @brief  UART interface for the Adafruit PM2.5 Air Quality Sensor
            and Plantower PMSA003I Sensor.
 */
class Adafruit_PM25AQI_UART : public Adafruit_PM25AQI {
public:
  Adafruit_PM25AQI_UART();
  ~Adafruit_PM25AQI_UART();
  bool begin(Stream *theSerial); // TODO: What should this take in?
  // TODO: MAYBE BREAK OUT READ INTO READ_CUBIC OR READ_PM
  // TODO: WE COULD ALSO BREAK OUT THE CLASS INTO ADAFRUIT_PM25AQI_UART_CUBIC
  // AND ADAFRUIT_PM25AQI_UART_ADAFRUIT or we have a begin_UART(is_cubic) or
  // something to set it up properly?
  virtual bool read(PM25_AQI_Data *data);

private:
  Stream *_serial_dev = nullptr;
  UARTDevice *_uart_dev = nullptr;
  uint8_t _readbuffer[32];
};

#endif // ADAFRUIT_PM25AQI_UART_H
