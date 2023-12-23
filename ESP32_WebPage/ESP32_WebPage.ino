#include "DHT.h" 
#include <LiquidCrystal_I2C.h> 
#include <ArduinoJson.h> 
 
#define DHTPIN 2 
#define DHTTYPE DHT11 
#define MQPIN A0 
#define PIRPIN 3 
#define LEDPIN 4 
#define I2C_ADDR 0x27 
#define FLAMEPIN 6 
#define LIGHT_SENSORPIN A1 
 
const int lightMin = 1023; // Maximum light value (minimum resistance) 
const int lightMax = 0; 
const int buzzerpin = 13; 
 
DHT dht(DHTPIN, DHTTYPE); 
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2); 
 
void setup() { 
  Serial.begin(9600); 
  dht.begin(); 
  pinMode(PIRPIN, INPUT); 
  pinMode(LEDPIN, OUTPUT); 
  pinMode(buzzerpin, OUTPUT); 
  pinMode(FLAMEPIN, INPUT); 
  pinMode(LIGHT_SENSORPIN, INPUT); 
 
  lcd.init(); 
  lcd.backlight(); 
} 
 
void loop() { 
  float h = dht.readHumidity(); 
  float t = dht.readTemperature(); 
  int mqValue = analogRead(MQPIN); 
  int pirValue = digitalRead(PIRPIN); 
  int flameValue = digitalRead(FLAMEPIN); 
  int lightLevelRaw = analogRead(LIGHT_SENSORPIN); 
  int lightLevelScaled = map(lightLevelRaw, lightMin, lightMax, 0, 10); 
  lightLevelScaled = constrain(lightLevelScaled, 0, 10); 
 
  lcd.clear(); 
 
  if (isnan(h) || isnan(t)) { 
    lcd.print("DHT Error!"); 
  } else { 
    lcd.setCursor(0, 0); 
    lcd.print("Temp: "); 
    lcd.print(t); 
    lcd.print("C"); 
     
    lcd.setCursor(0, 1); 
    lcd.print("Humid: "); 
    lcd.print(h); 
    lcd.print("%"); 
  } 
 
  digitalWrite(LEDPIN, pirValue == HIGH ? HIGH : LOW); 
 
  if (flameValue == LOW) { 
    tone(buzzerpin, 400); 
    delay(50); 
  } else { 
    noTone(buzzerpin); 
    delay(50); 
  } 
 
  // Создание JSON-объекта и добавление данных 
  StaticJsonDocument<256> jsonDoc; 
  jsonDoc["h"] = h; 
  jsonDoc["t"] = t; 
  jsonDoc["mq"] = mqValue; 
  jsonDoc["pir"] = pirValue; 
  jsonDoc["flame"] = flameValue; 
  jsonDoc["light"] = lightLevelScaled; 
 
  // Сериализация JSON-объекта в строку и отправка 
  String dataString; 
  serializeJson(jsonDoc, dataString); 
  Serial.println(dataString); 
 
  delay(10000); // Ожидание перед следующим циклом измерений 
}