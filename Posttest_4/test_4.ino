#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "flowerfluffp";
const char* password = "12345678";

// Initialize Telegram BOT
#define BOTtoken "6820133973:AAGlU00Z9tF_QX_BKiW4Z9U9pID3vITegTY"  // your Bot Token (Get from Botfather)
#define DHTPIN D5
#define LED1 D1
#define LED2 D2
#define LED3 D3
#define LED4 D4

#define DHTTYPE DHT11
// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "-4101494768"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
bool ledState = LOW;

DHT dht(DHTPIN, DHTTYPE);

// Track which user controls which light
String user1ControlledLight = "911502373";
String user2ControlledLight = "1168965898";
String user3ControlledLight = "";
String prevMessage = "";

void handleLEDMessage(String chat_id, String from_name) {
  String welcome = "Pilih LED" + from_name + ":\n";
  Serial.print(prevMessage);
  welcome += prevMessage == "/off" ? "LED akan dimatikan:\n" : "LED akan dinyalakan:\n";
  welcome += "/led1 Pilih LED Auzan \n";
  welcome += "/led2 Pilih LED Galuh \n";
  welcome += "/led3 Pilih LED Abdullah \n";
  welcome += "/led4 Pilih LED Universal \n";
  bot.sendMessage(chat_id, welcome, "");
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    Serial.println(chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Selamat datang, " + from_name + ".\n";
      welcome += "Pencet perintah sesuai kebutuhan dan kemauan.\n\n";
      welcome += "/on LED \n";
      welcome += "/off LED \n";
      welcome += "/show Kepunyaan LED \n";
      welcome += "/dht Status (Suhu dan Kelembapan)";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/on" || text == "/off") {
      prevMessage = text;
      handleLEDMessage(chat_id, from_name);
    }

    if (text == "/show") {
      String welcome = "Yang Punya LED:   \n\n";
      welcome += "LED1 Auzan\n";
      welcome += "LED2 Galuh\n";
      welcome += "LED3 Abdullah\n";
      welcome += "LED4 univ\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    // LIGHT LED
    if (prevMessage == "/on") {
      String from_id = bot.messages[i].from_id;
      if (text == "/led1") {
        if (from_id == user1ControlledLight) {
          bot.sendMessage(chat_id, "LED 1 Hidup", "");
          digitalWrite(LED1, HIGH);
        } else {
          bot.sendMessage(chat_id, "Error!! LED 1 Tidak Menyala Karena bukan Auzan", "");
        }
      }

      if (text == "/led2") {
        if (from_id == user2ControlledLight) {
          bot.sendMessage(chat_id, "LED 2 Hidup", "");
          digitalWrite(LED2, HIGH);
        } else {
          bot.sendMessage(chat_id, "Error!! LED 2 Tidak Menyala Karena bukan Galuh", "");
        }
      }
      if (text == "/led3") {
        if (from_id == user3ControlledLight) {
          bot.sendMessage(chat_id, "LED 3 Hidup", "");
          digitalWrite(LED3, HIGH);
        } else {
          bot.sendMessage(chat_id, "Error!! LED 3 Tidak Menyala Karena bukan Abdullah", "");
        }
      }
      if (text == "/led4") {
          bot.sendMessage(chat_id, "LED 4 Hidup", "");
          digitalWrite(LED4, HIGH);
        }
      }
    if (prevMessage == "/off") {
      String from_id = bot.messages[i].from_id;
      if (text == "/led1") {
        if (from_id == user1ControlledLight) {
          bot.sendMessage(chat_id, "LED 1 Mati", "");
          digitalWrite(LED1, LOW);
        } else {
          bot.sendMessage(chat_id, "Error!! LED 1 Tidak bisa mati Karena Bukan Auzan", "");
        }
      }

      if (text == "/led2") {
        if (from_id == user2ControlledLight) {
          bot.sendMessage(chat_id, "LED 2 Mati", "");
          digitalWrite(LED2, LOW);
        } else {
          bot.sendMessage(chat_id, "Error!! LED 2 Tidak bisa mati Karena Bukan Galuh", "");
        }
      }

      if (text == "/led3") {
        if (from_id == user3ControlledLight) {
          bot.sendMessage(chat_id, "LED 3 Mati", "");
          digitalWrite(LED3, LOW);
        } else {
          bot.sendMessage(chat_id, "Error!! LED 3 Tidak bisa mati Karena Bukan Abdullah", "");
        }
      }
      if (text == "/led4") {
          bot.sendMessage(chat_id, "LED 3 Mati", "");
          digitalWrite(LED4, LOW);
        }
      }
    if (text == "/dht") {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      String message = "";
      message += "Suhu : " + String(t) + "°C\n";
      message += "Kelembapan : " + String(h) + " %";
      bot.sendMessage(chat_id, message, "");
    }

  }
}

void setup() {
  Serial.begin(115200);

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
  client.setTrustAnchors(&cert);     // Add root certificate for api.telegram.org
#endif

  dht.begin();
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED1, ledState);
  digitalWrite(LED2, ledState);
  digitalWrite(LED3, ledState);


  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}


void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}