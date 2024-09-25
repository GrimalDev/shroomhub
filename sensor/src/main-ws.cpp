#include "clientConfig.h"

using namespace websockets;

const int OLED_RESET = -1;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_VEML7700 veml = Adafruit_VEML7700();
DHT dht(DHT_PIN, DHT22);
WebsocketsClient client;

long lastReconnectAttempt = 0;
long lastWSTime = 0;
long sensorLastCollectionMillis;
long sensorLastReadMillis; 
bool sensorDataReady = false;
int sensorNumberReads = 0;

struct SensorData {
  float dataTotals = 0;
  float getCollectionAverage() { return dataTotals / SENSOR_COLLECTION_SIZE; };
  void (*collectData)();
};

SensorData Humidity;
SensorData Temperature;
SensorData Co2ppm;
float foggerGoal = FOGGER_DEFAULT_GOAL;

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

void readHumidity() {
  float tempH = dht.readHumidity();
  if (isnan(tempH)) {
    Serial.println("Failed to read humidity from DHT sensor!");
  } else {
    Humidity.dataTotals += tempH;
  }
}

void readTemperature() {
  float tempT = dht.readTemperature();
  if (isnan(tempT)) {
    Serial.println("Failed to read humidity from DHT sensor!");
  } else {
    Temperature.dataTotals += tempT;
  }
}

void readLight() {}

void readCo2ppm() {
  Co2ppm.dataTotals += analogRead(A0);
}

void fogger(float goal) {
  if (goal < foggerGoal) {
    digitalWrite(FOGGER_PIN, HIGH);
  } else {
    digitalWrite(FOGGER_PIN, LOW);
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

  pinMode(FOGGER_PIN, OUTPUT);
  pinMode(CO2_PIN, INPUT);

  dht.begin();

  Humidity.collectData = readHumidity;
  Temperature.collectData = readTemperature;
  Co2ppm.collectData = readCo2ppm;

  display.clearDisplay();
  display.setCursor(0, 0);

  if (client.available()) {
    display.println("Server found");
  } else {
    display.println("No server found");
  }

  display.println("Humidity: ...%");
  display.println("Fogger Goal: " + String(foggerGoal) + "%");
  display.println("Temperature: ...C");
  display.println("CO2: ...ppm");
}

void loop() {
  // Check if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0, 0);

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

  // Attempt WebSocket reconnection if disconnected
  if (!client.available()) {
    long currentTime = millis();
    if (currentTime - lastReconnectAttempt > 5000) { // Retry every 5 seconds
      connectToWebSocket();
      lastReconnectAttempt = currentTime;
    }
  } else {
    // Keep WebSocket client alive
    client.poll();
  }

  // Run tasks if it's time
  long currentMillis = millis();

  if (currentMillis - sensorLastCollectionMillis >= SENSOR_COLLECTION_COOLDOWN_TIME) {
    if (currentMillis - sensorLastReadMillis >= SENSOR_READ_COOLDOWN_TIME) {
      Humidity.collectData();
      Temperature.collectData();
      Co2ppm.collectData();

      sensorNumberReads ++;

      sensorLastReadMillis = currentMillis;
    }
  }

  // Check if tasks are not ready
  if (sensorNumberReads >= SENSOR_COLLECTION_SIZE) {
    display.clearDisplay();
    display.setCursor(0, 0);

    if (client.available()) {
      display.println("Server found");
    } else {
      display.println("No server found");
    }
     
    float humi = Humidity.getCollectionAverage();
    float temp = Temperature.getCollectionAverage();
    float co2 = Co2ppm.getCollectionAverage();

    // Process the data
    display.println("Humidity: " + String(humi) + "%");
    display.println("Fogger Goal: " + String(foggerGoal) + "%");
    display.println("Temperature: " + String(temp) + "C");
    display.println("CO2: " + String(co2) + " ppm");
    // display.println("Light: " + String(lux) + " lux");

    JsonDocument jsonDoc;
    jsonDoc["humi"] = humi;
    jsonDoc["temp"] = temp;
    jsonDoc["co2"] = co2;
    jsonDoc["foggerGoal"] = foggerGoal;

    // Serialize JSON to string
    String jsonData;
    serializeJson(jsonDoc, jsonData);

    if (client.available() && millis() - lastWSTime > 2000) {
      client.send(jsonData);
      lastWSTime = millis();
    }

    fogger(humi);

    sensorNumberReads = 0;
    Humidity.dataTotals = 0;
    Temperature.dataTotals = 0;
    Co2ppm.dataTotals = 0;

    sensorLastCollectionMillis = currentMillis;
  }

  display.display();
}
