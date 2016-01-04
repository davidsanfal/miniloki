#include "miniloki.h"

MiniLoki::MiniLoki(int spd0Pin1, int spd0Pin2,
                   int spd1Pin1, int spd1Pin2,
                   int spd2Pin1, int spd2Pin2,
                   int spd0Min, int spd1Min, int spd2Min) {
  speed0Pin1 = spd0Pin1;
  speed0Pin2 = spd0Pin2;
  speed1Pin1 = spd1Pin1;
  speed1Pin2 = spd1Pin2;
  speed2Pin1 = spd2Pin1;
  speed2Pin2 = spd2Pin2;
  speed_0 = 0;
  speed_1 = 0;
  speed_2 = 0;
  speed_X = 0;
  speed_Y = 0;
  speed_W = 0;
  speed0Min = spd0Min;
  speed1Min = spd1Min;
  speed2Min = spd2Min;

}

void MiniLoki::move() {
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
  int spd1=1024, spd2=1024, _spd, _min_spd;
  switch (motor) {
    case 0:
      _min_spd = speed0Min;
    case 1:
      _min_spd = speed1Min;
    case 2:
      _min_spd = speed2Min;
  }
  _spd = map(abs(spd / norm) * 100, 0.0, 100.0, 1024, _min_spd);
  if (spd > 0) spd2 = _spd;
  else spd1 = _spd;
  if (spd == 0){
    spd2 = 1024;
    spd1 = 1024;
  }
  switch (motor) {
    case 0:
      analogWrite(speed0Pin1, spd1);
      analogWrite(speed0Pin2, spd2);
    case 1:
      analogWrite(speed1Pin1, spd1);
      analogWrite(speed1Pin2, spd2);
    case 2:
      analogWrite(speed2Pin1, spd1);
      analogWrite(speed2Pin2, spd2);
  }
}
