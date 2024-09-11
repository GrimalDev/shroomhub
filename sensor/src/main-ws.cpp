#include "clientConfig.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_VEML7700.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

using namespace websockets;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define foggerPin 14
#define DHTPIN 12
#define co2Pin A0
#define co2Zero 76.63

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_VEML7700 veml = Adafruit_VEML7700();
DHT dht(DHTPIN, DHT22);
WebsocketsClient client;

unsigned long lastReconnectAttempt = 0;
unsigned long lastWSTime = 0;
float humi = 0;
float temp = 0;
int co2ppm = 0;
float foggerGoal = 90;

void connectToWebSocket() {
  Serial.println("Connecting to WebSocket...");

  if (client.connect(webSocketServerURL)) {
    Serial.println("Connected to WebSocket");
  } else {
    Serial.println("Failed to connect. Will retry...");
  }
}

void responseHandler(WebsocketsMessage message) {
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, message.data());
  if (error) {
    Serial.println("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  if (doc.containsKey("fogger")) {
    foggerGoal = doc["fogger"].as<float>();
  }
}

float getTemperature() { return dht.readTemperature(); }

void getLight() {}

float getHumidity() { return dht.readHumidity(); }

int getC02() {
  int co2now[10];
  int co2raw = 0;
  int co2comp = 0;
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
  return map(co2comp, 0, 1023, 400, 5000);
}

void fogger(float goal) {
  if (goal < foggerGoal) {
    digitalWrite(foggerPin, HIGH);
  } else {
    digitalWrite(foggerPin, LOW);
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Connect to WiFi
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    display.println("Connecting to WiFi...");
    display.display();
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected to WiFi");
  Serial.println("Connected to WiFi");
  display.display();

  delay(500);

  // Setup WebSocket events
  client.onMessage([](WebsocketsMessage message) { responseHandler(message); });

  client.onEvent([](WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WebSocket connection");
      Serial.println("WebSocket connection opened");
      display.display();
    } else if (event == WebsocketsEvent::ConnectionClosed) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WebSocket connection closed");
      Serial.println("WebSocket connection closed. Attempting to reconnect...");
      display.display();
    } else if (event == WebsocketsEvent::GotPing) {
      Serial.println("Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
      Serial.println("Got a Pong!");
    }
  });

  connectToWebSocket();

  pinMode(foggerPin, OUTPUT);
  pinMode(co2Pin, INPUT);

  dht.begin();
}

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);

  // Check if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    display.println("WiFi disconnected. Reconnecting...");
    Serial.println("WiFi disconnected. Reconnecting...");
    display.display();
    WiFi.begin(wifiSSID, wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      display.println("Reconnecting to WiFi...");
      Serial.println("Reconnecting to WiFi...");
      display.display();
    }
    display.println("Reconnected to WiFi");
    Serial.println("Reconnected to WiFi");
    display.display();
    delay(500);
    connectToWebSocket(); // Try to reconnect WebSocket after WiFi is back
  }

  // messages if connected to server or not
  if (client.available()) {
    display.println("Server found");
    Serial.println("Server found");
  } else {
    display.println("No server");
    Serial.println("No server");
  }

  // Keep WebSocket client alive
  if (client.available()) {
    client.poll();
  }

  // Attempt WebSocket reconnection if disconnected
  if (!client.available()) {
    unsigned long currentTime = millis();
    if (currentTime - lastReconnectAttempt > 5000) { // Retry every 5 seconds
      connectToWebSocket();
      lastReconnectAttempt = currentTime;
    }
  }

  // float lux = veml.readLux();

  humi = getHumidity();
  temp = getTemperature();
  co2ppm = getC02();

  display.println("Humidity: " + String(humi) + "%");
  display.println("Fogger Goal: " + String(foggerGoal) + "%");
  display.println("Temperature: " + String(temp) + "C");
  display.println("CO2: " + String(co2ppm) + " ppm");
  // display.println("Light: " + String(lux) + " lux");

  JsonDocument jsonDoc;
  jsonDoc["humi"] = humi;
  jsonDoc["temp"] = temp;
  jsonDoc["co2"] = co2ppm;
  jsonDoc["foggerGoal"] = foggerGoal;

  // Serialize JSON to string
  String jsonData;
  serializeJson(jsonDoc, jsonData);

  if (client.available() && millis() - lastWSTime > 2000) {
    client.send(jsonData);
    lastWSTime = millis();
  }

  fogger(humi);

  display.display();
}
