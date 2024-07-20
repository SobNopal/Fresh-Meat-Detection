#include <Wire.h>
#include <ArduinoJson.h>
#include <WiFi.h> // Include the WiFi library for ESP32
#include <HTTPClient.h> // Include the HTTPClient library for ESP32
#include "SparkFun_AS7265X.h"

const char *ssid = "Capstone";
const char *password = "capstoneaja";
const char *apiEndpoint = "http://192.168.137.1:8000/api/kesegaran"; // Replace this with your API endpoint

AS7265X sensor;
unsigned long startTime;
int readingsCount = 0;
const int maxReadings = 240; // 24 jam == 1440 minutes / 6 minutes = 240 readings
// const int maxReadings = 15; // 90 minutes / 6 minutes = 15 readings

void setup() {
  Serial.begin(115200);
  Serial.println("AS7265x Spectral Triad Example");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if (!sensor.begin()) {
    Serial.println("Sensor does not appear to be connected. Please check wiring. Freezing...");
    while (1);
  }

  Serial.println("A,B,C,D,E,F,G,H,R,I,S,J,T,U,V,W,K,L");
  startTime = millis();
}

void loop() {
  if (readingsCount < maxReadings) {
    if (millis() - startTime >= 30000) { // Take a reading every 6 minutes (360,000 milliseconds)
      Serial.println("Taking sensor measurements...");
      sensor.takeMeasurements();

      Serial.println("Sending data to API...");
      String jsonString = constructJSONString(); // Construct JSON string
      sendDataToAPI(jsonString); // Send data to API

      readingsCount++;
      startTime = millis(); // Reset start time for the next reading
    }
  } else {
    Serial.println("Completed all readings. Stopping.");
    while(1); // Stop the loop after completing all readings
  }
}

String constructJSONString() {
  DynamicJsonDocument doc(512);
  doc["k_daging"] = "Ayam";
  
  // Create an array for sensor data
  JsonArray kesegaran = doc.createNestedArray("k_kes_daging");
  kesegaran.add(sensor.getCalibratedA());
  kesegaran.add(sensor.getCalibratedB());
  kesegaran.add(sensor.getCalibratedC());
  kesegaran.add(sensor.getCalibratedD());
  kesegaran.add(sensor.getCalibratedE());
  kesegaran.add(sensor.getCalibratedF());
  kesegaran.add(sensor.getCalibratedG());
  kesegaran.add(sensor.getCalibratedH());
  kesegaran.add(sensor.getCalibratedR());
  kesegaran.add(sensor.getCalibratedI());
  kesegaran.add(sensor.getCalibratedS());
  kesegaran.add(sensor.getCalibratedJ());
  kesegaran.add(sensor.getCalibratedT());
  kesegaran.add(sensor.getCalibratedU());
  kesegaran.add(sensor.getCalibratedF());
  kesegaran.add(sensor.getCalibratedW());
  kesegaran.add(sensor.getCalibratedK());
  kesegaran.add(sensor.getCalibratedL());
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

void sendDataToAPI(String jsonString) {
  HTTPClient http;
  
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}