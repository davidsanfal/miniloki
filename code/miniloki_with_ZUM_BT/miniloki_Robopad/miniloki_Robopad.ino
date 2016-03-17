
//----------------------------------------------------------------
//-- Loki with ZUM BT-328 basic firmware Robopad v0
//-- (c) BQ. Released under a GPL licencse
//--
//-- 04 / 03 / 2016
//-- Author:  Anita de Prado - ana.deprado@bq.com
//
//-----------------------------------------------------------------
//-----------------------------------------------------------------

#define BQ_ZUM_BLUETOOTH      19200

/* Size of the received data buffer */
#define bufferSize 5


#define M1_PIN_A 5
#define M1_PIN_B 6
#define M2_PIN_A 9
#define M2_PIN_B 10
#define M3_PIN_A 11
#define M3_PIN_B 3

#define minSpeed 30

int motorPin[] = {M1_PIN_A, M1_PIN_B, M2_PIN_A, M2_PIN_B, M3_PIN_A, M3_PIN_B};

/*  A char buffer to storage the received data from the Bluetooth
    Serial */
char dataBuffer[bufferSize]; 

/* Buffer iterator */
int i = 0;

/* Number of characters availables in the Serial */
int numChar = 0;   


void setup() {

  /* Open the Bluetooth Serial and empty it */
  Serial.begin(BQ_ZUM_BLUETOOTH);  
  Serial.flush();     
  
  pinMode(M1_PIN_A, OUTPUT);
  pinMode(M1_PIN_B, OUTPUT);
  pinMode(M2_PIN_A, OUTPUT);
  pinMode(M2_PIN_B, OUTPUT);
  pinMode(M3_PIN_A, OUTPUT);
  pinMode(M3_PIN_B, OUTPUT);

}


void loop() {
  
 /* If there is something in the Bluetooth serial port */
  if (Serial.available() > 0) { 
   
    /* Reset the iterator and clear the buffer */
    i = 0;
    memset(dataBuffer, 0, sizeof(dataBuffer));  
    
    /* Wait for let the buffer fills up. Depends on the length of 
       the data, 1 ms for each character more or less */
    delay(bufferSize); 

    /* Number of characters availables in the Bluetooth Serial */
    numChar = Serial.available();   
    
    /* Limit the number of characters that will be read from the
       Serial to avoid reading more than the size of the buffer */
    if (numChar > bufferSize) {
          numChar = bufferSize;
    }

    /* Read the Bluetooth Serial and store it in the buffer */
    while (numChar--) {
        dataBuffer[i++] = Serial.read();

        /* As data trickles in from your serial port you are 
         grabbing as much as you can, but then when it runs out 
         (as it will after a few bytes because the processor is 
         much faster than a 9600 baud device) you exit loop, which
         then restarts, and resets i to zero, and someChar to an 
         empty array.So please be sure to keep this delay */
        delay(3);
    } 

    /* Manage the data */   
    checkData(dataBuffer);
    
  }



//-------------
  // //Read Joystick
  //  int potX = analogRead(A2);
  //  int potY = analogRead(A1);
  //  int potW = analogRead(A3);

  // //-- GLOBAL ROBOT SPEEDS:
  // //---------------------------
  //  float Vx = map(potX, 0, 1023, -255, 255);   
  //  float Vy = map(potY, 0, 1023, -255, 255); 
  //  float W = map(potW, 0, 1023, -255, 255);


  // //-- MOVE ROBOT  
  // //---------------------------
  //  MoveRobot(Vx, Vy, W);
    

  // delay(300);

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


/* Manage the buffer of data */
void checkData(char* data){  
  
   switch(data[0]) {

    /* Line follower mode button pressed */
    case 'I':
      //...
      break;

    /* Light avoider mode button pressed */
    case 'G':
      //...
      break;

    /* Obstacles avoider mode button pressed */
    case 'B':
      //... 
      break;

    /* Manual control mode button pressed */
    case 'M':
      //...
      break;

    case'S':
      /* Stop button pressed */
      MoveRobot(0, 0, 0);
      break;
    
    case 'U':
      /* Up button pressed */
      MoveRobot(255, 0, 0);
      break;
    
    case 'D':
      /* Down button pressed */
      MoveRobot(-255, 0, 0);
      break;
      
    case 'L':
      /* Left button pressed */ 
      MoveRobot(0, 0, 255);
      break;
      
    case 'R':
      /* Right button pressed */ 
      MoveRobot(0, 0, -255);
      break;
    
  } 

  /* Empty the Serial */   
  Serial.flush();

}








