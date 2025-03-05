#include "Adafruit_PM25AQI.h"

Adafruit_PM25AQI_Base::Adafruit_PM25AQI_Base() {
  PM25AQI_DEBUG_PRINTLN("Base constructor");
  if (_buffer) {
    delete[] _buffer;
  }
  _buffer = new uint8_t[_bufferSize];
}

Adafruit_PM25AQI_Base::~Adafruit_PM25AQI_Base() {
  delete[] _buffer;
  _buffer = nullptr;
}

bool Adafruit_PM25AQI_Base::read(PM25_AQI_Data *data) {
  PM25AQI_DEBUG_PRINTLN("[ERROR] Messed up classes! Base read");
  return false;
}

bool Adafruit_PM25AQI_Base::verify_starting_bytes(uint8_t *buffer) {
  PM25AQI_DEBUG_PRINTLN("Base verify_starting_bytes");
  // PMSA003I uses same protocol as PMS5003: 0x42 0x4D
  return (buffer[0] == 0x42 && buffer[1] == 0x4D);
}

bool Adafruit_PM25AQI_Base::verify_checksum(uint8_t *buffer, size_t bufLen) {
  PM25AQI_DEBUG_PRINTLN("Base verify_checksum");
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }
  uint16_t check = buffer[30] << 8;
  check += buffer[31];
  return check == sum;
}

void Adafruit_PM25AQI_Base::decode_data(uint8_t *buffer, PM25_AQI_Data *data) {
  PM25AQI_DEBUG_PRINTLN("Base decode_data");
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // Copy to data struct
  memcpy((void *)data, (void *)buffer_u16, 30);

  if (data->pm25_standard == 0 && data->pm25_env != 0) {
    // PM1006 doesn't report standardised data (only env)
    return;
  }
  data->aqi_pm100_us = pm100_aqi_us(data->pm100_standard);
  data->aqi_pm100_china = pm100_aqi_china(data->pm100_standard);
  data->aqi_pm25_us = pm25_aqi_us(data->pm25_standard);
  data->aqi_pm25_china = pm25_aqi_china(data->pm25_standard);
}

uint16_t Adafruit_PM25AQI_Base::pm25_aqi_us(float concentration) {
  float c;
  float AQI;
  c = (floor(10 * concentration)) / 10;
  if (c < 0)
    AQI = 0;
  else if (c >= 0 && c < 12.1f) {
    AQI = linear(50, 0, 12, 0, c);
  } else if (c >= 12.1f && c < 35.5f) {
    AQI = linear(100, 51, 35.4f, 12.1f, c);
  } else if (c >= 35.5f && c < 55.5f) {
    AQI = linear(150, 101, 55.4f, 35.5f, c);
  } else if (c >= 55.5f && c < 150.5f) {
    AQI = linear(200, 151, 150.4f, 55.5f, c);
  } else if (c >= 150.5f && c < 250.5f) {
    AQI = linear(300, 201, 250.4f, 150.5f, c);
  } else if (c >= 250.5f && c < 350.5f) {
    AQI = linear(400, 301, 350.4f, 250.5f, c);
  } else if (c >= 350.5f && c < 500.5f) {
    AQI = linear(500, 401, 500.4f, 350.5f, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

uint16_t Adafruit_PM25AQI_Base::pm100_aqi_us(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c < 55) {
    AQI = linear(50, 0, 55, 0, c);
  } else if (c < 155) {
    AQI = linear(100, 51, 155, 55, c);
  } else if (c < 255) {
    AQI = linear(150, 101, 255, 155, c);
  } else if (c < 355) {
    AQI = linear(200, 151, 355, 255, c);
  } else if (c < 425) {
    AQI = linear(300, 201, 425, 355, c);
  } else if (c < 505) {
    AQI = linear(400, 301, 505, 425, c);
  } else if (c < 605) {
    AQI = linear(500, 401, 605, 505, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

uint16_t Adafruit_PM25AQI_Base::pm25_aqi_china(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c <= 35) {
    AQI = linear(50, 0, 35, 0, c);
  } else if (c <= 75) {
    AQI = linear(100, 51, 75, 35, c);
  } else if (c <= 115) {
    AQI = linear(150, 101, 115, 75, c);
  } else if (c <= 150) {
    AQI = linear(200, 151, 150, 115, c);
  } else if (c <= 250) {
    AQI = linear(300, 201, 250, 150, c);
  } else if (c <= 350) {
    AQI = linear(400, 301, 350, 250, c);
  } else if (c <= 500) {
    AQI = linear(500, 401, 500, 350, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

uint16_t Adafruit_PM25AQI_Base::pm100_aqi_china(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c <= 50) {
    AQI = linear(50, 0, 50, 0, c);
  } else if (c <= 150) {
    AQI = linear(100, 51, 150, 50, c);
  } else if (c <= 250) {
    AQI = linear(150, 101, 250, 150, c);
  } else if (c <= 350) {
    AQI = linear(200, 151, 350, 250, c);
  } else if (c <= 420) {
    AQI = linear(300, 201, 420, 350, c);
  } else if (c <= 500) {
    AQI = linear(400, 301, 500, 420, c);
  } else if (c <= 600) {
    AQI = linear(500, 401, 600, 500, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

float Adafruit_PM25AQI_Base::linear(uint16_t aqi_high, uint16_t aqi_low,
                                    float conc_high, float conc_low,
                                    float concentration) {
  float f;
  f = ((concentration - conc_low) / (conc_high - conc_low)) *
          (aqi_high - aqi_low) +
      aqi_low;
  return f;
}
