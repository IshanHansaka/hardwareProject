#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>

// Replace with your network credentials
const char* WIFI_SSID = "Dialog 4G 370";
const char* WIFI_PASSWORD = "Fc5814ED";

// Replace with your Firebase project credentials
#define FIREBASE_PROJECT_ID "first-year-hardware-project"
#define FIREBASE_DATABASE_URL "https://first-year-hardware-project.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyAxgTKliDO4MNop_gZEH_led3kpI2MlfBs"

// Define Firebase Data objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  // Configure Firebase
  config.api_key = FIREBASE_API_KEY;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Optional: Check connection status
  if (Firebase.ready()) {
    Serial.println("Firebase connection successful");
  } else {
    Serial.print("Firebase connection error: ");
    Serial.println(fbdo.errorReason());
  }

  // Initialize time (optional)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  if (!getLocalTime(nullptr)) {
    Serial.println("Failed to obtain time");
    return;
  }
}

void loop() {
  // Get current date and time
  char dateString[20];
  char timeString[20];
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get local time");
    return;
  }

  // Format date and time strings
  strftime(dateString, sizeof(dateString), "%Y-%m-%d", &timeinfo);
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);

  // Create Firestore paths
  String collectionPath = "/time";  // Replace with your collection path
  String documentPath = collectionPath + "/" + dateString + "-" + timeString;  // Unique document path based on date and time

  // Prepare JSON data to be stored in Firestore
  FirebaseJson json;
  json.set("date", dateString);
  json.set("time", timeString);

  // Set data within the Firestore document
  if (Firebase.Firestore.setDocument(&fbdo, documentPath.c_str(), &json)) {
    Serial.println("Data written successfully to Firestore");
  } else {
    Serial.print("Error writing data: ");
    Serial.println(fbdo.errorReason());
  }

  delay(60000); // Wait 1 minute before writing the next document
}
