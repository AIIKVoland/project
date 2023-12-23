#include <WiFi.h> 
#include <ESP_Mail_Client.h> 
 
const char* ssid = "your_SSID"; 
const char* password = "your_PASSWORD"; 
 
// SMTP сервер 
SMTPSession smtp; 
SMTP_Message message; 
 
// Учетные данные SMTP 
const char* smtpServer = "smtp.gmail.com"; 
const int smtpServerPort = 587 (с TLS) или 465 (с SSL) 
const char* emailSender = "youremail@example.com"; 
const char* emailSenderPassword = "yourPassword"; 
const char* emailRecipient = "recipient@example.com"; 
 
void setup() { 
  Serial.begin(9600); 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.println("Connecting to WiFi..."); 
  } 
  Serial.println("Connected to WiFi"); 
 
  smtp.debug(1); 
  smtp.callback(smtpCallback); 
 
  ESP_Mail_Session session; 
  session.server.host_name = smtpServer; 
  session.server.port = smtpServerPort; 
  session.login.email = emailSender; 
  session.login.password = emailSenderPassword; 
  session.login.user_domain = ""; // Оставьте пустым, если не используется 
 
  smtp.connect(&session); 
} 
 
void loop() { 
  if (Serial.available()) { 
    String flameDetected = Serial.readStringUntil('\n'); 
    if (flameDetected == "FLAME_DETECTED") { 
      message.sender.name = "ESP32"; 
      message.sender.email = emailSender; 
      message.subject = "Warning: Flame Detected!"; 
      message.addRecipient("Recipient", emailRecipient); 
 
      String textMsg = "A flame has been detected by the sensor."; 
      message.text.content = textMsg.c_str(); 
 
      if (!smtp.sendMail(&message)) { 
        Serial.println("Error sending Email, " + smtp.errorReason()); 
      } 
    } 
  } 
} 
 
// SMTP Callback function 
void smtpCallback(SMTP_Status status) { 
  // Handle SMTP status here 
}