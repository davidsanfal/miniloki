#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "conf.h"
#include "mini_loki.h"


//Board: Mi2 (https://github.com/bqlabs/Mi2)

#define SPEED_PIN_0 D8 //gpio 15 (pin 15)
#define DIRECTION_PIN_0 D4 //gpio 2 (pin 12)
#define SPEED_PIN_1 D2 //gpio 5 (pin 4)
#define DIRECTION_PIN_1 D1 //gpio 4 (pin 5)
#define SPEED_PIN_2 D6 //gpio 12 (pin SDA)
#define DIRECTION_PIN_2 D5 //gpio 14 (pin SCK)

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
String input;
boolean stringComplete = false;

WiFiServer server(80);
MiniLoki loki(SPEED_PIN_0, DIRECTION_PIN_0,
              SPEED_PIN_1, DIRECTION_PIN_1,
              SPEED_PIN_2, DIRECTION_PIN_2);
void setup(void)
{
  Serial.begin(115200);
  //Wifi configuration
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //DNS configuration
  if (!MDNS.begin("loki")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  server.begin();
  Serial.println("TCP server started");
  MDNS.addService("http", "tcp", 80);

  //Pins configuration
  pinMode(SPEED_PIN_0, OUTPUT);
  pinMode(DIRECTION_PIN_0, OUTPUT);
  pinMode(SPEED_PIN_1, OUTPUT);
  pinMode(DIRECTION_PIN_1, OUTPUT);
  pinMode(SPEED_PIN_2, OUTPUT);
  pinMode(DIRECTION_PIN_2, OUTPUT);
}

void loop(void)
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.print("Client connected");
  while (client.connected()) {
    while (client.available()) {
      input = client.readStringUntil('\n');
      stringComplete = true;
    }
    if (stringComplete) {
      loki.parse_string(input);
      stringComplete = false;
    }
  }
}

