#include <WiFi.h>
#include <FirebaseClient.h>

const char* WIFI_SSID = "Dialog 4G 370";
const char* WIFI_PASSWORD = "Fc5814ED";

const char* FIREBASE_PROJECT_ID = "first-year-hardware-project";
const char* FIREBASE_DATABASE_URL = "https://first-year-hardware-project.firebaseio.com";
const char* FIREBASE_API_KEY = "AIzaSyAxgTKliDO4MNop_gZEH_led3kpI2MlfBs";

const char* FIREBASE_CREDENTIALS_PATH = "credentials.json";

FirebaseData fbdo;
FirebaseJson json;

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

  // Initialize Firebase after Wi-Fi connection is established
  Firebase.begin(FIREBASE_PROJECT_ID, FIREBASE_DATABASE_URL, FIREBASE_API_KEY);

  // Set credentials file path
  Firebase.setFirebaseJsonData(fbdo, json);
  Firebase.setStorageCredentialsFile(fbdo, FIREBASE_CREDENTIALS_PATH);

  // Optional: Check connection status
  if (Firebase.ready()) {
    Serial.println("Firebase connection successful");
  } else {
    Serial.print("Firebase connection error: ");
    Serial.println(Firebase.errorReason());
  }
}

// Rest of your code for creating documents and interacting with Firebase...
void loop() {
  // Create a new document with a unique ID in a collection called "data"
  FirebaseFirestore firestore = Firebase.firestore();
  String docID = Firebase.firestore().createDocument(&fbdo, "data");

  if (Firebase.success()) {
    Serial.println("Document created successfully with ID: " + docID);

    // Get the current date and time (adjust format as needed)
    char dateString[20];
    char timeString[20];
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to get local time");
      return;
    }

    strftime(dateString, sizeof(dateString), "%Y-%m-%d", &timeinfo);
    strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);

    // Create a JSON object to store data
    FirebaseJson data;
    data.set("date", dateString);
    data.set("time", timeString);

    // Set data within the newly created document
    if (Firebase.firestore().setDocument(&fbdo, "data/" + docID, &data)) {
      Serial.println("Data written successfully to the document");
    } else {
      Serial.print("Error writing data: ");
      Serial.println(Firebase.errorReason());
    }
  } else {
    Serial.print("Error creating document: ");
    Serial.println(Firebase.errorReason());
  }

  delay(5000); // Adjust delay for how often you want to create documents
}
