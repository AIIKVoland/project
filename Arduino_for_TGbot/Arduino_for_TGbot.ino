#include <WiFi.h> 
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h> 
 
// Замените на свои данные сети Wi-Fi 
const char* ssid = "your_SSID"; 
const char* password = "your_PASSWORD"; 
 
// Токен Телеграм-бота 
const char* botToken = "your_BOT_token"; 
 
WiFiClientSecure client; 
UniversalTelegramBot bot(botToken, client); 
 
// Переменные для хранения данных с датчиков 
String h, t, mqValue, pirValue, flameValue, lightLevel; 
 
void handleNewMessages(int numNewMessages) { 
  for (int i = 0; i < numNewMessages; i++) { 
    String chat_id = bot.messages[i].chat_id; 
    String text = bot.messages[i].text; 
 
    if (text == "h") { 
      bot.sendMessage(chat_id, "Влажность: " + h + "%", ""); 
    } else if (text == "t") { 
      bot.sendMessage(chat_id, "Температура: " + t + "°C", ""); 
    } else if (text == "mqValue") { 
      bot.sendMessage(chat_id, "Концентрация газа: " + mqValue, ""); 
    } else if (text == "pirValue") { 
      bot.sendMessage(chat_id, "Датчик движения: " + (pirValue == "1" ? "Обнаружено" : "Не обнаружено"), ""); 
    } else if (text == "flameValue") { 
      bot.sendMessage(chat_id, "Датчик пламени: " + (flameValue == "1" ? "Обнаружено" : "Не обнаружено"), ""); 
    } else if (text == "lightLevel") { 
      bot.sendMessage(chat_id, "Уровень освещенности: " + lightLevel, ""); 
    } else { 
      bot.sendMessage(chat_id, "Неизвестная команда", ""); 
    } 
  } 
} 
 
void setup() { 
  Serial.begin(9600); // Для общения с Arduino 
  Serial2.begin(9600); // Для общения с ESP32 
 
  // Подключение к Wi-Fi 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.println("Connecting to WiFi..."); 
  } 
  Serial.println("Connected to WiFi"); 
 
  client.setInsecure(); 
} 
 
void loop() { 
  if (WiFi.status() == WL_CONNECTED) { 
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1); 
    handleNewMessages(numNewMessages); 
  } 
 
  if (Serial2.available()) { 
    String dataFromArduino = Serial2.readStringUntil('\n'); 
    // Парсим данные в формате: "h:xx.xx,t:xx.xx,mq:xxxx,pir:x,flame:x,light:xx" 
    int hIndex = dataFromArduino.indexOf("h:"); 
    int tIndex = dataFromArduino.indexOf(",t:"); 
    int mqIndex = dataFromArduino.indexOf(",mq:"); 
    int pirIndex = dataFromArduino.indexOf(",pir:"); 
    int flameIndex = dataFromArduino.indexOf(",flame:"); 
    int lightIndex = dataFromArduino.indexOf(",light:"); 
 
    h = dataFromArduino.substring(hIndex + 2, tIndex); 
    t = dataFromArduino.substring(tIndex + 3, mqIndex); 
    mqValue = dataFromArduino.substring(mqIndex + 4, pirIndex); 
    pirValue = dataFromArduino.substring(pirIndex + 5, flameIndex); 
    flameValue = dataFromArduino.substring(flameIndex + 7, lightIndex); 
    lightLevel = dataFromArduino.substring(lightIndex + 7); 
  } 
 
  delay(1000); 
}