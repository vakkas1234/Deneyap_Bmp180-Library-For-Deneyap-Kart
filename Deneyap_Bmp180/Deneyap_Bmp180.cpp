#include "Deneyap_Bmp180.h"

Deneyap_Bmp180::Deneyap_Bmp180() {}

bool Deneyap_Bmp180::begin(uint8_t address) {
    _address = address;
    Wire.begin();
    if (read8(0xD0) != 0x55) { // BMP180 kimlik kontrolü
        return false;
    }
    readCalibrationData();
    return true;
}

void Deneyap_Bmp180::readCalibrationData() {
    ac1 = read16(0xAA);
    ac2 = read16(0xAC);
    ac3 = read16(0xAE);
    ac4 = read16(0xB0);
    ac5 = read16(0xB2);
    ac6 = read16(0xB4);
    b1 = read16(0xB6);
    b2 = read16(0xB8);
    mb = read16(0xBA);
    mc = read16(0xBC);
    md = read16(0xBE);
}

uint8_t Deneyap_Bmp180::read8(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)1);
    return Wire.read();
}

uint16_t Deneyap_Bmp180::read16(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)2);
    return (Wire.read() << 8) | Wire.read();
}

int16_t Deneyap_Bmp180::readRawTemperature() {
    Wire.beginTransmission(_address);
    Wire.write(0xF4);
    Wire.write(0x2E); // Sıcaklık okuma komutu
    Wire.endTransmission();
    delay(5); // Ölçüm süresi
    return read16(0xF6);
}

int32_t Deneyap_Bmp180::readRawPressure() {
    Wire.beginTransmission(_address);
    Wire.write(0xF4);
    Wire.write(0x34); // Basınç okuma komutu (OSS=0)
    Wire.endTransmission();
    delay(8); // Ölçüm süresi
    Wire.beginTransmission(_address);
    Wire.write(0xF6);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)3);
    uint32_t msb = Wire.read();
    uint32_t lsb = Wire.read();
    uint32_t xlsb = Wire.read();
    return ((msb << 16) | (lsb << 8) | xlsb) >> (8 - (0x34 >> 6 & 0x03));
}

float Deneyap_Bmp180::readTemperature() {
    int32_t UT = readRawTemperature();
    int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
    int32_t X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md);
    b5 = X1 + X2;
    return ((b5 + 8) >> 4) / 10.0;
}

float Deneyap_Bmp180::readPressure() {
    int32_t UP = readRawPressure();
    int32_t B6 = b5 - 4000;
    int32_t X1 = ((int32_t)b2 * ((B6 * B6) >> 12)) >> 11;
    int32_t X2 = ((int32_t)ac2 * B6) >> 11;
    int32_t X3 = X1 + X2;
    int32_t B3 = ((((int32_t)ac1 * 4 + X3) + 2) >> 2);
    X1 = ((int32_t)ac3 * B6) >> 13;
    X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    uint32_t B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
    uint32_t B7 = ((uint32_t)UP - B3) * (uint32_t)50000;
    int32_t p = (B7 < 0x80000000) ? (B7 * 2 / B4) : (B7 / B4 * 2);
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;
    return p + ((X1 + X2 + 3791) >> 4);
}


float Deneyap_Bmp180::readAltitude(float seaLevelhPa) {
    float pressure = readPressure() / 100.0;
    return 44330.0 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
}