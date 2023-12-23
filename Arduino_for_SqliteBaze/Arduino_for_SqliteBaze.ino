#include <WiFi.h> 
#include <ESPAsyncWebServer.h> 
#include <SQLite3.h> 
#include <ArduinoJson.h> 
 
const char* ssid = "your_SSID"; 
const char* password = "your_PASSWORD"; 
 
AsyncWebServer server(80); 
SQLite3 db; 
 
void setup() { 
  Serial.begin(9600); 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.println("Connecting to WiFi..."); 
  } 
  Serial.println("Connected to WiFi"); 
 
  if (!db.open("/data.db")) { 
    Serial.println("Failed to open database"); 
    return; 
  } 
 
  char* errMsg; 
  db.exec("CREATE TABLE IF NOT EXISTS sensor_data (id INTEGER PRIMARY KEY, humidity FLOAT, temperature FLOAT, mqValue INTEGER, pirValue INTEGER, flameValue INTEGER, lightLevel INTEGER)", 0, 0, &errMsg); 
  if (errMsg) { 
    Serial.println("Error creating table: " + String(errMsg)); 
    db.free(errMsg); 
  } 
} 
 
void loop() { 
  if (Serial.available()) { 
    String jsonData = Serial.readStringUntil('\n'); 
    DynamicJsonDocument doc(1024); 
    auto error = deserializeJson(doc, jsonData); 
    if (error) { 
      Serial.println("Error parsing JSON"); 
      return; 
    } 
 
    // Получение данных из JSON 
    float humidity = doc["h"]; 
    float temperature = doc["t"]; 
    int mqValue = doc["mq"]; 
    int pirValue = doc["pir"]; 
    int flameValue = doc["flame"]; 
    int lightLevel = doc["light"]; 
 
    // Подготовка и выполнение запроса на вставку данных 
    char* errMsg; 
    String sql = "INSERT INTO sensor_data (humidity, temperature, mqValue, pirValue, flameValue, lightLevel) VALUES (" + String(humidity, 2) + ", " + String(temperature, 2) + ", " + String(mqValue) + ", " + String(pirValue) + ", " + String(flameValue) + ", " + String(lightLevel) + ")"; 
    db.exec(sql.c_str(), 0, 0, &errMsg); 
    if (errMsg) { 
      Serial.println("Error inserting data: " + String(errMsg)); 
      db.free(errMsg); 
    } 
  } 
}