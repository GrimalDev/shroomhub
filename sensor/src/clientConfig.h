#ifndef CLIENT_H
#define CLIENT_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VEML7700.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

const char *wifiSSID = WIFI_SSID;
const char *wifiPassword = WIFI_PASSWORD;
const char *webSocketServerURL = WEBSOCKET_URL;

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int SENSOR_COLLECTION_COOLDOWN_TIME = 10000;
const int SENSOR_READ_COOLDOWN_TIME = 2000;
const int SENSOR_COLLECTION_SIZE = 10;
const int FOGGER_DEFAULT_GOAL = 90; 

const int FOGGER_PIN = 14;
const int DHT_PIN = 12;
const int CO2_PIN = A0;
const float CO2_ZERO = 76.63;

#endif
