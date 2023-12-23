#include <WiFi.h> 
#include <ESPAsyncWebServer.h> 
#include <ArduinoJson.h> 
 
const char* ssid = "your_SSID"; 
const char* password = "your_PASSWORD"; 
 
AsyncWebServer server(80); 
DynamicJsonDocument sensorData(1024); 
 
void setup() { 
  Serial.begin(9600); 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.println("Connecting to WiFi..."); 
  } 
  Serial.println("Connected to WiFi"); 
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { 
    request->send_P(200, "text/html", R"rawliteral( 
    <html> 
      <head> 
        <title>Умный дом</title> 
        <script> 
          function updateSensorData() { 
            var xhttp = new XMLHttpRequest(); 
            xhttp.onreadystatechange = function() { 
              if (this.readyState == 4 && this.status == 200) { 
                var data = JSON.parse(this.responseText); 
                document.getElementById("humidity").innerHTML = data.h; 
                document.getElementById("temperature").innerHTML = data.t; 
                document.getElementById("mq").innerHTML = data.mq; 
                document.getElementById("pir").innerHTML = data.pir; 
                document.getElementById("flame").innerHTML = data.flame; 
                document.getElementById("light").innerHTML = data.light; 
              } 
            }; 
            xhttp.open("GET", "/sensordata", true); 
            xhttp.send(); 
          } 
          setInterval(updateSensorData, 1000); // Обновление данных каждую секунду 
        </script> 
      </head> 
      <body> 
        <h1>Данные с датчиков</h1> 
        <p>Влажность: <span id="humidity"></span>%</p> 
        <p>Температура: <span id="temperature"></span>°C</p> 
        <p>Концентрация газа (MQ): <span id="mq"></span></p> 
        <p>Датчик движения (PIR): <span id="pir"></span></p> 
        <p>Датчик пламени: <span id="flame"></span></p> 
        <p>Уровень освещенности: <span id="light"></span></p> 
      </body> 
    </html> 
    )rawliteral"); 
  }); 
 
  server.on("/sensordata", HTTP_GET, [](AsyncWebServerRequest *request) { 
    String json; 
    serializeJson(sensorData, json); 
    request->send(200, "application/json", json); 
  }); 
 
  server.begin(); 
} 
 
void loop() { 
  if (Serial.available()) { 
    String jsonData = Serial.readStringUntil('\n'); 
    deserializeJson(sensorData, jsonData); 
  } 
}