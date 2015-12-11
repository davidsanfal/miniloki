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
    int speed0Pin;
    int direction0Pin;
    int speed1Pin;
    int direction1Pin;
    int speed2Pin;
    int direction2Pin;
};
