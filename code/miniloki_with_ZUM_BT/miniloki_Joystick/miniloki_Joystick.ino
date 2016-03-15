
//----------------------------------------------------------------
//-- Loki with ZUM BT-328 basic firmware v0
//-- (c) BQ. Released under a GPL licencse
//--
//-- 04 / 03 / 2016
//-- Author:  Anita de Prado - ana.deprado@bq.com
//
//-----------------------------------------------------------------
//-----------------------------------------------------------------



#define M1_PIN_A 5
#define M1_PIN_B 6
#define M2_PIN_A 9
#define M2_PIN_B 10
#define M3_PIN_A 11
#define M3_PIN_B 3

#define minSpeed 30

int motorPin[] = {M1_PIN_A, M1_PIN_B, M2_PIN_A, M2_PIN_B, M3_PIN_A, M3_PIN_B};


void setup() {

  Serial.begin(9600);	

  pinMode(M1_PIN_A, OUTPUT);
  pinMode(M1_PIN_B, OUTPUT);
  pinMode(M2_PIN_A, OUTPUT);
  pinMode(M2_PIN_B, OUTPUT);
  pinMode(M3_PIN_A, OUTPUT);
  pinMode(M3_PIN_B, OUTPUT);
}


void loop() {
  
  //Read Joystick
   int potX = analogRead(A2);
   int potY = analogRead(A1);
   int potW = analogRead(A3);

  //-- GLOBAL ROBOT SPEEDS:
  //---------------------------
   float Vx = map(potX, 0, 1023, -255, 255);   
   float Vy = map(potY, 0, 1023, -255, 255); 
   float W = map(potW, 0, 1023, -255, 255);


  //-- MOVE ROBOT  
  //---------------------------
   MoveRobot(Vx, Vy, W);
    

  delay(300);

}



float GetNorm(float spd1, float spd2, float spd3){

  spd1 = abs(spd1);
  spd2 = abs(spd2);
  spd3 = abs(spd3);

  float _norm = 255;

  _norm = max(_norm, spd1);
  _norm = max(_norm, spd2);
  _norm = max(_norm, spd3);

  return 255/_norm;


}



void MoveMotor(int id, int _speed){

  id*=2;

  if(abs(_speed) < minSpeed){        //No escribimos nunca una velocidad menor que minSpeed

    digitalWrite(motorPin[id], LOW);    //A
    digitalWrite(motorPin[id+1], LOW);  //B

  }else if(_speed > 0){                         //(_speed > minSpeed)
        
    analogWrite(motorPin[id], int(_speed));     //A
    analogWrite(motorPin[id+1], 0);             //B
          
  }else{                                        //(_speed < -minSpeed)

    analogWrite(motorPin[id], 0);               //A
    analogWrite(motorPin[id+1], int(-_speed));  //B

  }


}


void MoveRobot(float _Vx, float _Vy, float _W){

  //-- CALCULE MOTOR SPEEDS:
  //---------------------------
   float speed_1 = (- _Vx * sin(PI / 3)) - (_Vy * cos(PI / 3)) + _W;
   float speed_2 =  _Vy + _W;   
   float speed_3 = (_Vx * sin(PI / 3)) - (_Vy * cos(PI / 3)) + _W;

  
   float norm = GetNorm(speed_1,speed_2,speed_3);
  

  //-- MOVE MOTORS
   MoveMotor(0, speed_1*norm);
   MoveMotor(1, speed_2*norm);
   MoveMotor(2, speed_3*norm);



}









