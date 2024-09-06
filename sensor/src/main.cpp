#include <Arduino.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

const char *ssid = "Baliette-LOW-WIFI";
const char *password = "mYF7PtCXaF9947EW2a9HcjAtd";

const char *websocket_server_url = "ws://192.168.100.16:4040/ws";

WebsocketsClient client;
unsigned long lastReconnectAttempt = 0;

void connectToWebSocket() {
  Serial.println("Connecting to WebSocket...");

  if (client.connect(websocket_server_url)) {
    Serial.println("Connected to WebSocket");
  } else {
    Serial.println("Failed to connect. Will retry...");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup WebSocket events
  client.onMessage([](WebsocketsMessage message) {
    Serial.print("Received: ");
    Serial.println(message.data());
  });

  client.onEvent([](WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
      Serial.println("WebSocket connection opened");
    } else if (event == WebsocketsEvent::ConnectionClosed) {
      Serial.println("WebSocket connection closed. Attempting to reconnect...");
    } else if (event == WebsocketsEvent::GotPing) {
      Serial.println("Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
      Serial.println("Got a Pong!");
    }
  });

  connectToWebSocket();
}

void loop() {
  // Keep WebSocket client alive
  if (client.available()) {
    client.poll();
  }

  // Attempt reconnection if disconnected
  if (!client.available()) {
    unsigned long currentTime = millis();
    if (currentTime - lastReconnectAttempt > 5000) { // Retry every 5 seconds
      connectToWebSocket();
      lastReconnectAttempt = currentTime;
    }
  }

  // Send a message every 5 seconds
  static unsigned long lastTime = 0;
  if (client.available() && millis() - lastTime > 5000) {
    client.send("Hello world");
    lastTime = millis();
  }
}
