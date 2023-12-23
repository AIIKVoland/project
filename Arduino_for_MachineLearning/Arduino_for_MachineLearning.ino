#include "DHT.h" 
#include <LiquidCrystal_I2C.h> 
 
#define DHTPIN 2 
#define DHTTYPE DHT11 
#define MQPIN A0 
#define PIRPIN 3 
#define LEDPIN 4 
#define I2C_ADDR 0x27 
#define FLAMEPIN 6 
#define LIGHT_SENSORPIN A1 
 
const int lightMin = 1023; 
const int lightMax = 0; 
const int buzzerpin = 13; 
DHT dht(DHTPIN, DHTTYPE); 
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2); 
 
float prevTemp = 0.0; 
float prevHumidity = 0.0; 
int prevMQValue = 0; 
int prevLightLevelScaled = 0; 
 
const float alpha = 0.2; 
 
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
 
  float tempEMA = alpha * t + (1 - alpha) * prevTemp; 
  float humidityEMA = alpha * h + (1 - alpha) * prevHumidity; 
  int mqEMA = static_cast<int>(alpha * mqValue + (1 - alpha) * prevMQValue); 
  int lightEMA = static_cast<int>(alpha * lightLevelScaled + (1 - alpha) * prevLightLevelScaled); 
 
  prevTemp = tempEMA; 
  prevHumidity = humidityEMA; 
  prevMQValue = mqEMA; 
  prevLightLevelScaled = lightEMA; 
 
  // Вывод прогнозных значений на LCD монитор 
  lcd.setCursor(0, 1); // Перевод курсора в начало второй строки 
  lcd.print("PredTemp: "); 
  lcd.print(tempEMA, 1); // Вывод прогнозной температуры 
  lcd.print("C  "); 
  lcd.print("PredHumid: "); 
  lcd.print(humidityEMA, 1); // Вывод прогнозной влажности 
  lcd.print("%  "); 
  lcd.print("PredGas: "); 
  lcd.print(mqEMA); // Вывод прогнозного значения газа 
  lcd.print("  "); 
  lcd.print("PredLight: "); 
  lcd.print(lightEMA); // Вывод прогнозного уровня освещенности 
  lcd.print("  "); 
 
  String dataString = String(tempEMA) + ","; 
  dataString += String(humidityEMA) + ","; 
  dataString += String(mqEMA) + ","; 
  dataString += String(pirValue) + ","; 
  dataString += String(flameValue) + ","; 
  dataString += String(lightEMA); 
 
  Serial.println(dataString); 
 
  delay(10000); 
}