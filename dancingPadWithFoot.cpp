#include <WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "Dialog 4G 370";
const char* password = "Fc5814ED";

WebSocketsServer webSocket = WebSocketsServer(81);

const int sensorPins[4] = {13, 14, 12, 18}; // Pins for touch sensors
boolean sensorStates[4] = {false, false, false, false}; // Array of sensor states

#define buzzer  2

int receivedInt = -1; // Initialize received integer to an invalid value

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Set sensor pins as inputs
  for (int i = 0; i < 4; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  pinMode(buzzer, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());  // Print the IP address

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // Check for sensor activation
  for (int i = 0; i < 4; i++) {
    int sensorValue = digitalRead(sensorPins[i]);
    if (sensorValue == HIGH) {
      sensorStates[i] = true;
    } else {
      if (sensorStates[i] && sensorValue == LOW) {
        sensorStates[i] = false;      
        if (receivedInt == i && receivedInt != -1) {
          playBuzzer(i+1);
          webSocket.sendTXT(0, "0"); // Send response "0" to React app
        }
      }
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    receivedInt = atoi((char*)payload);
    Serial.print("Received integer: ");
    Serial.println(receivedInt);
  } else if (type == WStype_DISCONNECTED) {
    receivedInt = -1;
  }
}

void playBuzzer(int x) {
  tone(buzzer, 650 + (x * 100));
  delay(300);
  noTone(buzzer);
}