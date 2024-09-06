
#include "DHT.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_VEML7700.h>
#include <Arduino.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define foggerPin 14
#define DHTPIN 12
#define co2Pin A0
#define co2Zero 76.63
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_VEML7700 veml = Adafruit_VEML7700();
DHT dht(DHTPIN, DHT11);

void setup() {
  Wire.begin();
  Serial.begin(9600);

  pinMode(foggerPin, OUTPUT);
  pinMode(co2Pin, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  dht.begin();
}

void getTemperature() {}

void getLight() {}

void getMoisture() {}

void getC02() {}

void loop() {

  display.clearDisplay();
  display.setCursor(0, 0);

  // float lux = veml.readLux();
  float humi = dht.readHumidity();
  float tempC = dht.readTemperature();
  int co2now[10];
  int co2raw = 0;
  int co2comp = 0;
  int co2ppm = 0;
  int zzz = 0;

  for (int x = 0; x < 10; x++) {
    co2now[x] = analogRead(A0);
    delay(200);
  }

  for (int x = 0; x < 10; x++) {
    zzz = zzz + co2now[x];
  }
  co2raw = zzz / 10;
  co2comp = co2raw - co2Zero;
  co2ppm = map(co2comp, 0, 1023, 400, 5000);

  display.println("Humidity: " + String(humi) + "%");
  display.println("Temperature: " + String(tempC) + "C");
  display.println("CO2: " + String(co2ppm) + " ppm");
  // display.println("Light: " + String(lux) + " lux");

  if (humi < 90) {
    digitalWrite(foggerPin, HIGH);
  } else {
    digitalWrite(foggerPin, LOW);
  }

  display.display();

  delay(1000);
}
