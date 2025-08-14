#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <GyverOLED.h>

// Создаем объект для работы с OLED-дисплеем
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

// Wi-Fi подключение настройки
const char* ssid_ap = "SSID";
const char* password_ap = "PASSWORD";

// Токен вашего бота
const char* bot_token = "TOKEN";

WiFiClientSecure client;
UniversalTelegramBot bot(bot_token, client);

unsigned long lastTimeBotRan;
const int botRequestDelay = 1000;

void setup() {
  oled.init();
  oled.clear();
  oled.home();

  pinMode(2, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid_ap, password_ap);
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    oled.home();
    oled.print("Connecting to WiFi...");
    delay(3000);
    oled.clear();
  }

  oled.clear();
  oled.home();
  oled.print("WiFi connected!");
  delay(3000);
  oled.clear();
  oled.home();

  const char* start_bot_messages[] = {"start bot", "start bot.", "start bot..", "start bot..."};
  for(int i = 0; i < 4; i++) {
    oled.print(start_bot_messages[i]);
    delay(500);
    oled.clear();
    oled.home();
  }

  oled.print("bot started");

  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

void handleNewMessages(int numMessages) {
  for (int i = 0; i < numMessages; i++) {
    String us_id = String(bot.messages[i].from_id);
    if(us_id == "ADMIN_ID")
    {
      String chat_id = String(bot.messages[i].chat_id);
      String message_text = bot.messages[i].text;
      String from_name = bot.messages[i].from_name;
  
      Serial.print("Message from ID: ");
      Serial.println(us_id);

      if (from_name == "") from_name = "Anonymous";

      if (message_text == "/start") {
        String start_command = "Приветствую, " + from_name + "\n";
        start_command += "Ваш ID: " + us_id + "\n";
        bot.sendMessage(chat_id, start_command, "");
      }else{}
    }
  }
}

void loop() {
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  delay(1000);

  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}






