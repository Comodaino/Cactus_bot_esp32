#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "LinkedListLib.h"
#include "time.h"
#include "utils.h"


struct tm lastWater;
bool need;
String id;

void setup()
{
  id = "";
  need = false;
  pinMode(RED_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Add root certificate for api.telegram.org
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
}

int checkWater() {
  struct tm currWater;
  getLocalTime(&currWater);
    int currTime = (currWater.tm_mon * 100) + currWater.tm_mday;
    int lastTime = (lastWater.tm_mon * 100) + lastWater.tm_mday;
    Serial.println(currTime);
    Serial.println(lastTime);
    Serial.println(currTime - lastTime);
    if (currTime >= startSpring && currTime <= startSummer){ // spring
    Serial.println("Spring");
        if(currTime - lastTime >= waterIntSpring) return 1;
        return 0;
    }
    if (currTime >= startSummer && currTime <= startAutumn){ // summer
    Serial.println("Summer");
        if(currTime - lastTime >= waterIntSummer) return 1;
        return 0;
    }
    if (currTime >= startAutumn && currTime <= startWinter){ // autumn
    Serial.println("Autumn");
        if(currTime - lastTime >= waterIntAutumn) return 1;
        return 0;
    }
    if (currTime >= startWinter && currTime <= 0){ // winter
    Serial.println("Winter");
        if(currTime - lastTime >= waterIntWinter) return 1;
        return 0;
    }
        if (currTime >= 0 && currTime <= startSpring){ // winter
    Serial.println("Winter");
        if(currTime - lastTime >= waterIntWinter) return 1;
        return 0;
    }
    return 0;
}

void water_loop() {
  need = false;
  Serial.println(checkWater());
  if(!checkWater()) return;
  need = true;
  if (millis() - water_lasttime > WATER_MTBS) bot.sendMessage(id, "water pls\n", "Markdown");
}

void telegram_loop() {
    if (millis() - bot_lasttime > BOT_MTBS) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages) {
            Serial.println("got new message");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        bot_lasttime = millis();
    }
}

void handleNewMessages(int numNewMessages) {
    Serial.print("handleNewMessages was called, # of new messages: ");
    Serial.println(numNewMessages);

    // Iterates all the messages received
    for (int i = 0; i < numNewMessages; i++) {
        // Saves message parameters
        String text = bot.messages[i].text;
        String chat_id = bot.messages[i].chat_id;
        String message;

        id = chat_id;

        if (text == "/watered") {
            if(!getLocalTime(&lastWater)) message = "There has been a problem.\n";
            else message = "Watered the cactus.\n";
            bot.sendMessage(chat_id, message, "Markdown");
        }
        else if (text == "/start") {
            message = "Welcome to the cactus bot\n";
            message += "\n\n";
            message += "/watered : tell the bot you watered the cactuses today\n";
            bot.sendMessage(chat_id, message, "Markdown");
        }
        else if (text == "/debug") {
            message = "Debug started\n";
            if(!getLocalTime(&lastWater)) message = "There has been a problem.\n";
            lastWater.tm_mday--;
            lastWater.tm_mday--;
            lastWater.tm_mday--;
            bot.sendMessage(chat_id, message, "Markdown");

        }
        else {
            message = "Not an acceptable command, please use /watered\n";
            bot.sendMessage(chat_id, message, "Markdown");
        }
    }
}


void loop() {
    delay(10);
    telegram_loop();
    delay(10);
    water_loop();
    delay(10);
    if(need) digitalWrite(RED_PIN, HIGH);
    else digitalWrite(RED_PIN, LOW);
}
