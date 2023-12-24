#include <WiFi.h> 
#include <ESPAsyncWebServer.h> 
#include <SQLite3.h> 
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h> 
#include <ESP_Mail_Client.h> 
 
const char* ssid = "your_SSID"; // Замените на имя вашей Wi-Fi сети 
const char* password = "your_PASSWORD"; // Замените на пароль вашей Wi-Fi сети 
 
// Настройки Telegram Bot 
const char* botToken = "your_BOT_token"; // Токен вашего Telegram бота 
WiFiClientSecure client; 
UniversalTelegramBot bot(botToken, client); 
 
// Настройки SMTP для отправки email 
SMTPSession smtp; 
ESP_Mail_Session mailSession; 
 
const char* smtpServer = "smtp.yourmailserver.com"; // Адрес SMTP сервера 
const int smtpServerPort = 465; // Порт SMTP сервера, обычно 465 или 587 
const char* emailSender = "youremail@example.com"; // Ваш email 
const char* emailPassword = "yourEmailPassword"; // Пароль от вашего email 
const char* emailRecipient = "recipient@example.com"; // Email получателя уведомлений 
 
// Настройки сервера и базы данных 
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
 
  // Настройка SMTP 
  smtp.debug(1); 
  mailSession.server.host_name = smtpServer; 
  mailSession.server.port = smtpServerPort; 
  mailSession.login.email = emailSender; 
  mailSession.login.password = emailPassword; 
  smtp.connect(&mailSession); 
 
  // Настройка сервера 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { 
    request->send(200, "text/plain", "Hello, world"); 
  }); 
  server.begin(); 
 
  // Настройка базы данных 
  if (!db.open("/data.db")) { 
    Serial.println("Failed to open database"); 
    return; 
  } 
  char* errMsg; 
  db.exec("CREATE TABLE IF NOT EXISTS sensor_data (id INTEGER PRIMARY KEY, data TEXT)", 0, 0, &errMsg); 
  if (errMsg) { 
    Serial.println("Error creating table: " + String(errMsg)); 
    db.free(errMsg); 
  } 
} 
 
void loop() { 
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1); 
  if (numNewMessages) { 
    handleNewMessages(numNewMessages); 
  } 
 
  if (Serial.available()) { 
    String dataFromArduino = Serial.readStringUntil('\n'); 
    DynamicJsonDocument doc(1024); 
    deserializeJson(doc, dataFromArduino); 
 
    // Обработка данных от Arduino 
    float h = doc["h"]; // Влажность 
    float t = doc["t"]; // Температура 
    // Обработка других данных (mq, pir, flame, light) 
    int mqValue = doc["mq"]; 
    int pirValue = doc["pir"]; 
    int flameValue = doc["flame"]; 
    int lightLevel = doc["light"]; 
 
    // Сохранение данных в базу данных SQLite 
    char* errMsg; 
    String sql = "INSERT INTO sensor_data (data) VALUES ('" + dataFromArduino + "')"; 
    db.exec(sql.c_str(), 0, 0, &errMsg); 
    if (errMsg) { 
      Serial.println("Error inserting data: " + String(errMsg)); 
      db.free(errMsg); 
    } 
 
    // Отправка email при обнаружении пламени 
    if (flameValue == 1) { // Проверка значения с датчика пламени 
      sendEmail("Warning: Flame Detected!", "A flame has been detected by the sensor."); 
    } 
  } 
} 
 
void handleNewMessages(int numNewMessages) { 
  for (int i = 0; i < numNewMessages; i++) { 
    String chat_id = bot.messages[i].chat_id; 
    String text = bot.messages[i].text; 
 
    if (text == "/start") { 
      String welcome = "Привет! Я ваш умный дом. Введите команду для получения данных."; 
      bot.sendMessage(chat_id, welcome, ""); 
    } 
    // Можно добавить другие команды для обработки 
  } 
} 
 
void sendEmail(String subject, String body) { 
  SMTP_Message message; 
  message.sender.name = "ESP32 Smart Home"; 
  message.sender.email = emailSender; 
  message.subject = subject; 
  message.addRecipient("Recipient", emailRecipient); 
  message.text.content = body.c_str(); 
 
  if (!smtp.sendMail(&message)) { 
    Serial.println("Error sending Email: " + smtp.errorReason()); 
  } 
}