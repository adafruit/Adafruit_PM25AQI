/****************************************************************************************
  Test sketch for using the Adafruit_PM25AQI library with
  the Cubic PM1006 UART Air Quality Sensor.

  This sensor is present on the VINDRIKTNING Air Quality sensor, sold by IKEA.

  Check out the guide below for the wiring diagram:
  https://learn.adafruit.com/ikea-vindriktning-hack-with-qt-py-esp32-s3-and-adafruit-io


  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  Modified by Brent Rubell for Adafruit Industries
  MIT license, all text above must be included in any redistribution
 *****************************************************************************************/
#include "Adafruit_PM25AQI.h"

// For UNO and others (without hardware serial) we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
// comment these two lines if using hardware serial
//#include <SoftwareSerial.h>
//SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Cubic PM1006 Air Quality Sensor");

  // Wait one second for sensor to boot up!
  delay(1000);

  // If using serial, initialize it and set baudrate before starting!
  Serial1.begin(9600);
  // Uncomment the following for use with software serial
  //pmSerial.begin(9600);

  if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find Cubic PM1006 sensor!");
    while (1) delay(10);
  }

  Serial.println("Cubic PM1006 found!");
}

void loop() {
  PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    Serial.println("Could not read from PM1006 sensor");
    delay(5000);  // Sample every 5 seconds
    return;
  }
  Serial.println("AQI reading success");

  Serial.println();
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (environmental)"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm25_env);
  Serial.println(F("---------------------------------------"));

  delay(20000); // Wait 20 seconds and get another reading from the IKEA Vindriktning
}
