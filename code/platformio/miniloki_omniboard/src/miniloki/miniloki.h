#include "Arduino.h"

class MiniLoki {
  public:
    MiniLoki(int, int, int, int, int, int);
    void parse_string(String);
    void set_speed(int, float, float);
  private:
    float speed_0;
    float speed_1;
    float speed_2;
    float speed_X;
    float speed_Y;
    float speed_W;
    int speed0Pin1;
    int speed0Pin2;
    int speed1Pin1;
    int speed1Pin2;
    int speed2Pin1;
    int speed2Pin2;
};
