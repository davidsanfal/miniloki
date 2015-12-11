#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "config.h"
#include "miniloki/miniloki.h"

//Board: Mi2 (https://github.com/bqlabs/Mi2)
#define SPEED_PIN_0 D8 //gpio 15 (pin 15)
#define DIRECTION_PIN_0 D4 //gpio 2 (pin 12)
#define SPEED_PIN_1 D2 //gpio 5 (pin 4)
#define DIRECTION_PIN_1 D1 //gpio 4 (pin 5)
#define SPEED_PIN_2 D7 //gpio 12 (pin SDA)
#define DIRECTION_PIN_2 D0 //gpio 14 (pin SCK)

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
  Serial.begin(9600);
  //Wifi configuration
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  String DNS_name = init_swarm("mini_loki");

  if (!MDNS.begin(DNS_name.c_str())) {
    while (1) {
      delay(1000);
    }
  }
  server.begin();
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
      }
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

String init_swarm(String robot_name){
  String name;
  WiFiClient client;
  while (!client.connect(OVERMIND_IP, OVERMIND_PORT)) {
    delay(100);
  }
  IPAddress   localAddr = WiFi.localIP();
  byte oct1 = localAddr[0];
  byte oct2 = localAddr[1];
  byte oct3 = localAddr[2];
  byte oct4 = localAddr[3];
  char local_IP[16];  
  sprintf(local_IP, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
  String msg = "{";
  msg += "\"ip\": ";msg += "\"";msg += (String)local_IP;msg += "\"";
  msg += ",";
  msg += "\"type\": ";msg += "\"";msg += robot_name;msg += "\"";
  msg += ",";
  msg += "\"chip_id\": ";msg += (String)ESP.getChipId();
  msg += "}";
  client.print(msg);
  while (!client.available()) {
      delay(10);
    }
  
  while(client.available()){
    name = client.readStringUntil('\n');
  }
  client.stop();
  return name;
}