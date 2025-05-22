#ifndef DENEYAP_BMP180_H
#define DENEYAP_BMP180_H

#include <Arduino.h>
#include <Wire.h>

class Deneyap_Bmp180 {
public:
    Deneyap_Bmp180();
    bool begin(uint8_t address = 0x77); // Varsayılan adres 0x77

    float readTemperature();    // Sıcaklık (Celsius)
    float readPressure();       // Basınç (Pa)
    float readAltitude(float seaLevelhPa = 1013.25); // Yükseklik (metre)

private:
    uint8_t _address;
    int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    uint16_t ac4, ac5, ac6;
    int32_t b5;

    int16_t readRawTemperature();
    int32_t readRawPressure();
    void readCalibrationData();
    uint16_t read16(uint8_t reg);
    uint8_t read8(uint8_t reg);
};

#endif
