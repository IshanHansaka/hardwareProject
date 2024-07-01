#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"

// Replace these with your WiFi credentials
const char* ssid = "Dialog 4G 370";
const char* password = "Fc5814ED";

// Replace these with your Firebase project credentials
#define API_KEY "AIzaSyAxgTKliDO4MNop_gZEH_led3kpI2MlfBs"
#define FIREBASE_PROJECT_ID "first-year-hardware-project"
#define USER_EMAIL "your_email@example.com"
#define USER_PASSWORD "your_password"
#define DATABASE_URL "https://first-year-hardware-project.firebaseio.com" // Add your Firebase database URL

// Initialize Firebase data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void FirestoreTokenStatusCallback(TokenInfo info) {
  Serial.printf("Token Info: type = %s, status = %s\n", getTokenType(info), getTokenStatus(info));
}

void firebaseInit() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL; // Set the database URL
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = FirestoreTokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void firestoreDataUpdate() {
  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    String documentPath = "testData/staticNumber";

    FirebaseJson content;
    content.set("fields/number/integerValue", 42);

    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "number")) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  firebaseInit();
}

void loop() {
  firestoreDataUpdate();
  delay(60000); // Delay for a minute before next reading
}
