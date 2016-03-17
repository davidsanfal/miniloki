#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "../../../../../config.h"
#include "miniloki/miniloki.h"

/*
config.h

#define WIFI_SSID "wifi_name"
#define WIFI_PASSWORD "wifi_pass"
*/

#define SPEED_PIN_0_1 D3
#define SPEED_PIN_0_2 D4
#define SPEED_PIN_1_1 D7
#define SPEED_PIN_1_2 D8
#define SPEED_PIN_2_1 D2
#define SPEED_PIN_2_2 D1

#define SPEED_0_MIN 100
#define SPEED_1_MIN 25
#define SPEED_2_MIN 40

//Wifi AP credentials
const char *ssid = "OmniBoard-Wifi-AP";
const char *password = "1234567890";

String input;
boolean stringComplete = false;

WiFiServer server(80);

MiniLoki loki(SPEED_PIN_0_1, SPEED_PIN_0_2,
              SPEED_PIN_1_1, SPEED_PIN_1_2,
              SPEED_PIN_2_1, SPEED_PIN_2_2,
              SPEED_0_MIN, SPEED_1_MIN, SPEED_2_MIN);

void setup(void)
{ 
  WiFi.softAP(ssid, password);
  server.begin();

  //Pins configuration
  pinMode(SPEED_PIN_0_1, OUTPUT);
  pinMode(SPEED_PIN_0_2, OUTPUT);
  pinMode(SPEED_PIN_1_1, OUTPUT);
  pinMode(SPEED_PIN_1_2, OUTPUT);
  pinMode(SPEED_PIN_2_1, OUTPUT);
  pinMode(SPEED_PIN_2_2, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(A0, INPUT);
  loki.move();
}

void loop(void)
{
  WiFi.mode(WIFI_AP);

  WiFiClient client = server.available();
  if (!client) {
    Serial.println(WiFi.localIP());
    delay(1000);
  }
  while (client.connected()) {
    while (client.available()) {
      input = client.readStringUntil('+');
      stringComplete = true;
    }
    if (stringComplete) {
      parseData(input);
      stringComplete = false;
    }
  }
}

void parseData(String data){
   int commaIndex = data.indexOf(',');
   int plusIndex = data.indexOf('+');
   loki.speed_X = data.substring(0, commaIndex).toFloat();
   loki.speed_Y = data.substring(commaIndex+1,plusIndex).toFloat();
   loki.move();
}