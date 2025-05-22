#include <Deneyap_Bmp180.h>

Deneyap_Bmp180 bmp;

void setup() {
    Serial.begin(115200);
    delay(1000); // Başlangıç bekleme süresi

    if (!bmp.begin()) { // Varsayılan adres 0x77
        Serial.println("BMP180 bulunamadi! Baglantilari ve I2C adresini kontrol edin.");
        while (1);
    }
    Serial.println("BMP180 basariyla baslatildi!");
}

void loop() {
    float sicaklik = bmp.readTemperature();
    float basinc = bmp.readPressure() / 100.0; // hPa'ya çevirme
    float yukseklik = bmp.readAltitude();

    Serial.print("Sicaklik: ");
    Serial.print(sicaklik);
    Serial.println(" °C");

    S