#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define MQPIN A0
#define PIRPIN 3
#define LEDPIN 4
#define I2C_ADDR 0x27
// #define BUZZERPIN 13
#define FLAMEPIN 6
#define LIGHT_SENSORPIN A1 // Analog pin connected to the light sensor

const int lightMin = 1023; // Maximum light value (minimum resistance)
const int lightMax = 0;
const int buzzerpin = 13;    // Minimum light value (maximum resistance)

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud for ESP32
  dht.begin();
  pinMode(PIRPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(buzzerpin, OUTPUT);
  pinMode(FLAMEPIN, INPUT);
  pinMode(LIGHT_SENSORPIN, INPUT);

  lcd.init(); // Initialize the display
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

  // Отправляем на ESP32 данные разделенные запятыми
String dataString = "{\"h\":" + String(h, 2) + ", \"t\":" + String(t, 2) + ", \"mq\":" + String(mqValue) + ", \"pir\":" + String(pirValue) + ", \"flame\":" + String(flameValue) + ", \"light\":" + String(lightLevelScaled) + "}";
Serial.println(dataString);

  delay(10000); // Ожидание перед следующим циклом измерений
}