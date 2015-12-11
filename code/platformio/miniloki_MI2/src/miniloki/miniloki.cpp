#include "miniloki.h"

MiniLoki::MiniLoki(int spd0Pin, int dir0Pin, int spd1Pin, int dir1Pin, int spd2Pin, int dir2Pin) {
  speed0Pin = spd0Pin;
  direction0Pin = dir0Pin;
  speed1Pin = spd1Pin;
  direction1Pin = dir1Pin;
  speed2Pin = spd2Pin;
  direction2Pin = dir2Pin;
  speed_0 = 0;
  speed_1 = 0;
  speed_2 = 0;
  speed_X = 0;
  speed_Y = 0;
  speed_W = 0;

}

void MiniLoki::parse_string(String inputString) {
  int message_substring = 0;
  String substr = "";
  for (int i = 0 ; i < inputString.length(); i++) {
    if (inputString[i] == ',' || i == inputString.length()-1) {
      message_substring++;
      switch (message_substring) {
        case 1:
          speed_X = substr.toFloat();
          substr = "";
        case 2:
          speed_Y = substr.toFloat();
          substr = "";
        case 3:
          speed_W = substr.toFloat();
          substr = "";
      }
    }
    else substr += (char)inputString[i];
  }
  speed_0 = (-speed_X / sin(PI / 3)) + (-speed_Y * cos(PI / 3)) - speed_W;
  speed_1 = speed_Y - speed_W;
  speed_2 = (speed_X / sin(PI / 3)) + (-speed_Y * cos(PI / 3)) - speed_W;
  float norm = 1.0;
  if (abs(speed_0) > 1 || abs(speed_1) > 1 || abs(speed_2) > 1) {
    if (abs(speed_0) >= abs(speed_1)) {
      norm = abs(speed_0);
      if (abs(speed_2) > abs(speed_0)) norm = abs(speed_2);

    }
    else {
      norm = abs(speed_1);
      if (abs(speed_2) > abs(speed_1)) norm = abs(speed_2);
    }
  }
  set_speed(0, speed_0, norm);
  set_speed(1, speed_1, norm);
  set_speed(2, speed_2, norm);
}

void MiniLoki::set_speed(int motor, float spd, float norm) {
  int dir = 1;
  if (spd < 0) dir = 0;
  spd = map(abs(spd / norm) * 100, 0.0, 100.0, 0, 1024);
  switch (motor) {
    case 0:
      digitalWrite(direction0Pin, dir);
      analogWrite(speed0Pin, spd);
    case 1:
      digitalWrite(direction1Pin, dir);
      analogWrite(speed1Pin, spd);
    case 2:
      digitalWrite(direction2Pin, dir);
      analogWrite(speed2Pin, spd);
  }
}
