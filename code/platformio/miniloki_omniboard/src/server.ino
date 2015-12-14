#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "config.h"
#include "miniloki/miniloki.h"
#include "gy_85/GY_85.h"
#include <Wire.h>
#include "MadgwickAHRS/MadgwickAHRS.h"

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

GY_85 GY85;
int newMillis = 0;
int prevMillis = 0;
float Roll = 0;
float Pitch = 0;
float Yaw = 0;

void setup(void)
{ 
  Wire.begin(D6, D5);
  delay(10);
  GY85.init();
  delay(10);
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
  pinMode(SPEED_PIN_0_1, OUTPUT);
  pinMode(SPEED_PIN_0_2, OUTPUT);
  pinMode(SPEED_PIN_1_1, OUTPUT);
  pinMode(SPEED_PIN_1_2, OUTPUT);
  pinMode(SPEED_PIN_2_1, OUTPUT);
  pinMode(SPEED_PIN_2_2, OUTPUT);
  loki.parse_string("0,0,0,");
}

void loop(void)
{
  WiFiClient client = server.available();
  if (!client) {
        newMillis = millis();
        float gx = (float)GY85.gyro_x( GY85.readGyro() );
        float gy = (float)GY85.gyro_y( GY85.readGyro() );
        float gz = (float)GY85.gyro_z( GY85.readGyro() );
        int dtime = newMillis - prevMillis;
        prevMillis = newMillis;
        Roll += gx * dtime * 0.001;
        Pitch += gy * dtime * 0.001;
        Yaw += gz * dtime * -0.001;
        Serial.print(Roll);Serial.print(" ");Serial.print(Pitch);Serial.print(" ");Serial.println(Yaw);
        delay(100);
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