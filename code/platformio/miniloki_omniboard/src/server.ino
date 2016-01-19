#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "../../../../../config.h"
#include "miniloki/miniloki.h"

/*
config.h

#define WIFI_SSID "wifi_name"
#define WIFI_PASSWORD "wifi_pass"
#define OVERMIND_IP "master_ip"
#define OVERMIND_PORT master_port
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

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
String input;
boolean stringComplete = false;

WiFiServer server(80);
MiniLoki loki(SPEED_PIN_0_1, SPEED_PIN_0_2,
              SPEED_PIN_1_1, SPEED_PIN_1_2,
              SPEED_PIN_2_1, SPEED_PIN_2_2,
              SPEED_0_MIN, SPEED_1_MIN, SPEED_2_MIN);

void setup(void)
{ 
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  //String DNS_name = init_swarm("mini_loki");
  String DNS_name = "miniloki0";

  if (!MDNS.begin(DNS_name.c_str())) {
    while (1) {
      delay(1000);
    }
  }
  server.begin();
  MDNS.addService("http", "tcp", 80);

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
  WiFiClient client = server.available();
  if (!client) {
    Serial.println(WiFi.localIP());
    delay(1000);
  }
  while (client.connected()) {
    while (client.available()) {
      input = client.readStringUntil('\n');
      stringComplete = true;
    }
    if (stringComplete) {
      client.write(analogRead(A0));
      parse_string(input, &loki.speed_X, &loki.speed_Y, &loki.speed_W);
      loki.move();
      stringComplete = false;
    }
  }
}

void parse_string(String inputString, float* speed_X, float* speed_Y, float* speed_W) {
  int message_substring = 0;
  String substr = "";
  for (int i = 0 ; i < inputString.length(); i++) {
    if (inputString[i] == ',' || i == inputString.length()-1) {
      message_substring++;
      switch (message_substring) {
        case 1:
          *speed_X = substr.toFloat();
          substr = "";
        case 2:
          *speed_Y = substr.toFloat();
          substr = "";
        case 3:
          *speed_W = substr.toFloat();
          substr = "";
      }
    }
    else substr += (char)inputString[i];
  }
}

String init_swarm(String robot_name){
  String name;
  WiFiClient client;
  while (!client.connect(OVERMIND_IP, OVERMIND_PORT)) delay(100);
  IPAddress   localAddr = WiFi.localIP();
  byte oct1 = localAddr[0];
  byte oct2 = localAddr[1];
  byte oct3 = localAddr[2];
  byte oct4 = localAddr[3];
  char local_IP[16];  
  sprintf(local_IP, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
  String msg = "{";
  msg += "\"ip\": ";msg += "\"";msg += (String)local_IP;msg += "\"";msg += ",";
  msg += "\"type\": ";msg += "\"";msg += robot_name;msg += "\"";msg += ",";
  msg += "\"chip_id\": ";msg += (String)ESP.getChipId();msg += "}";
  client.print(msg);
  while (!client.available()) delay(10);
  while(client.available()) name = client.readStringUntil('\n');
  client.stop();
  return name;
}