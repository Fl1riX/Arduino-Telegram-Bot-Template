#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <GyverOLED.h>
#include <esp_task_wdt.h>

// Создаем объект для работы с OLED-дисплеем
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

const char* ssid_ap = "TP-Link_156";
const char* password_ap = "9508077991";
const char* admin_id = "1330775721";
const char* bot_token = "6534323844:AAH04FY6zvv6_yFSKwq7OSgpaecNA0Gr3V4";
const char* start_bot_messages[] = {"starting bot", "starting bot.", "starting bot..", "starting bot..."};
const int botRequestDelay = 3000;

esp_task_wdt_config_t config = {
    .timeout_ms = 5000,    // Таймаут 5 секунд
    .idle_core_mask = 0,   // Не сбрасывать по idle
    .trigger_panic = true  // Сбрасывать процессор при зависании
};

WiFiClientSecure client;
UniversalTelegramBot bot(bot_token, client);

unsigned long lastTimeBotRan;

void setup() {
  oled.init();
  oled.clear();
  oled.home();

  pinMode(2, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid_ap, password_ap);
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    oled.print("Connecting to WiFi...");
  }
  oled.setCursor(0, 2);
  oled.print("WiFi connected!");
  delay(3000);

  for(int i = 0; i < 4; i++) {
    oled.print(start_bot_messages[i]);
    delay(500);
    oled.clear();
    oled.home();
  }

  oled.print("Bot started!");
  delay(2000);
  oled.clear();
  oled.home();
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  esp_task_wdt_init(&config);
}

void handleNewMessages(int numMessages) {
  static int oled_y;
  
  for (int i = 0; i < numMessages; i++) {
    char us_id[16];
    bot.messages[i].from_id.toCharArray(us_id, sizeof(us_id));
    
    if(strcmp(us_id, admin_id) == 0)
    {
      uint32_t chat_id = (uint32_t)bot.messages[i].chat_id.toInt();
      const char* message_text = bot.messages[i].text;
      const char* from_name = bot.messages[i].from_name;
  
      Serial.print("Message from ID: ");
      Serial.println(us_id);

      if (oled_y <= 7) {
      oled.setCursor(0, oled_y);

      oled.print(us_id);
      oled_y ++;
      } else if (oled_y > 7) {
        oled.clear();
        oled.home();

        oled.print(us_id);
        oled_y = 0;
      }

      if (from_name == "") from_name = "Anonymous";

      if (message_text == "/start") {
        char* start_command[128];
        snprintf(start_message, sizeof(start_message), "Приветствую, %s\nВаш ID: %s\n", from_name, us_id);
        bot.sendMessage(String(chat_id), start_command, "");
      } else {
      }
    }
  }
}

void led_blink() {
  static uint32_t led_tmr;
  if (millis() - led_tmr >= 1000){
    led_tmr = millis();
    digitalWrite(2, !digitalRead(2));
  }
}

void loop() {
  led_blink();
  
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  lastTimeBotRan = millis();
    
  esp_task_wdt_reset();
  }
}
