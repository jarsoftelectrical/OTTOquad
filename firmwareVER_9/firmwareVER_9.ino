#include <setjmp.h>
//#include "US.h"
#include "OTTOKame.h"
#include "MaxMatrix.h"
MaxMatrix ledmatrix=MaxMatrix(A1,A2,A3, 1); //PIN  A1=DIN, PIN A2=CS, PIN A3=CLK
//-- Library to manage serial commands
#include <OttoSerialCommand.h>
OttoSerialCommand SerialCmd;  //The SerialCommand object


#define CAL_TRIGGER_PIN 10 // link this PIN 10 to +5V to enter set up mode
#define LED_PIN 13
#define PIN_Buzzer  13
//#define PIN_NoiseSensor A6
//#define battery A7
#define TIME_INTERVAL 5000
/*
   (servo PIN CONNECTIONS)
   __________ __________ _________________
  |(pin9)_____)(pin8)  (pin2)(______(pin3)|
  |__|       |left FRONT right|        |__|
             |                |
             |                |
             |                |
   _________ |                | __________
  |(pin7)_____)(pin6)__(pin4)(______(pin5)|
  |__|                                 |__|
  
ULTRASONIC PIN_Trigger 12
ULTRASONIC PIN_Echo    11
BUZZER PIN 13
*/
MiniKame robot;
bool obstacleDetected = false;
int T=1000;              //Initial duration of movement
int moveId=0;            //Number of movement
int modeId=0;            //Number of mode
int moveSize=15;         //Asociated with the height of some movements
bool auto_mode = false;
bool random_walk = false;
bool stopSerial = false;
unsigned long cur_time, prev_serial_data_time, perv_sensor_time;
char cmd = 's';
static char prev_cmd = '.';
const char programID[]="Otto_KAME7"; //Each program will have a ID
jmp_buf jump_env;
int randomDance=0;
//---------------------------------------------------------
//-- Otto has 5 modes:
//--    * MODE = 0: Otto is awaiting  
//--    * MODE = 1: Dancing mode!  
//--    * MODE = 2: Obstacle detector mode  
//--    * MODE = 3: Noise detector mode   
//--    * MODE = 4: OttoPAD or any Teleoperation mode (listening SerialPort). 
//---------------------------------------------------------
volatile int MODE=0; //State of Otto in the principal state machine. 

void setup() {
  
  //Serial communication initialization
  Serial.begin(9600);  
  // set up Matrix display
  ledmatrix.init();
  ledmatrix.setIntensity(1);

  randomSeed(analogRead(A6));
  //
  robot.init(PIN_Buzzer);
  delay(2000);
  //MY servo 2 needed to be reversed so we do it here after the init 
  //robot.reverseServo(0);
  //robot.reverseServo(1);
  robot.reverseServo(2);
  //robot.reverseServo(3);
  //robot.reverseServo(4);
  //robot.reverseServo(5);
  //robot.reverseServo(6);
  //robot.reverseServo(7);
  { //begin:  servo calibrating if PIN 10 is HIGH
    bool state = true;
    pinMode(CAL_TRIGGER_PIN, OUTPUT);
    digitalWrite(CAL_TRIGGER_PIN, 0);
    pinMode(CAL_TRIGGER_PIN, INPUT);
    while (digitalRead(CAL_TRIGGER_PIN)) {
       robot.home();//Otto HOME position
      digitalWrite(LED_PIN, 1); // on calibrating SOUND BUZZER, sounds every 2 seconds
      delay(100);
      digitalWrite(LED_PIN, 0); // off calibrating STOP BUZZER
      delay(2000);
      robot.refresh();
      state = !state;
    }
    digitalWrite(LED_PIN, 0); // off calibrating STOP BUZZER
  }//end:
  perv_sensor_time = prev_serial_data_time = millis();
  //robot.run();
  //robot.turnL();
  //robot.turnR();
  //robot.pushUp();
  //robot.upDown();
  //robot.dance();
  //robot.frontBack();
  //robot.moonwalkL();
  //robot.omniWalk();
  //robot.hello();
  //robot.jump();
  // Animation Uuuuuh - A little moment of initial surprise
 //-----
  for(int i=0; i<2; i++){
      for (int i=0;i<8;i++){  
        robot.putAnimationMouth(littleUuh,i);
        delay(150);
      }
  }
    //Smile for a happy Otto :)
    robot.putMouth(smile);
    robot.sing(S_happy);
    delay(200);  

    //Setup callbacks for SerialCommand commands 
  SerialCmd.addCommand("S", receiveStop);      //  sendAck & sendFinalAck
  SerialCmd.addCommand("L", receiveLED);       //  sendAck & sendFinalAck
  SerialCmd.addCommand("M", receiveMovement);  //  sendAck & sendFinalAck
  SerialCmd.addCommand("H", receiveGesture);   //  sendAck & sendFinalAck
  SerialCmd.addCommand("K", receiveSing);      //  sendAck & sendFinalAck
  SerialCmd.addCommand("D", requestDistance);
  //SCmd.addCommand("B", requestBattery);   // 
  SerialCmd.addCommand("I", requestProgramId);
  SerialCmd.addCommand("J", requestMode);
  SerialCmd.addDefaultHandler(receiveStop);
}
///////////////////////////////////////////////////////////////////
//-- Principal Loop ---------------------------------------------//
///////////////////////////////////////////////////////////////////
void loop() {

 if (Serial.available()>0 && MODE!=4){
    SerialCmd.readSerial();  
    //MODE=4;
    robot.putMouth(happyOpen);
  }
  
    switch (MODE) {

      //-- MODE 0 - Otto is awaiting
      //---------------------------------------------------------
      case 0:
      robot.putMouth(bigSurprise);
robot.jump();
robot.putMouth(happyOpen);
delay(5000);

        break;
      

      //-- MODE 1 - Dance Mode!
      //---------------------------------------------------------
      case 1:
        randomDance=random(5,15); //5,20
       if (randomDance == 5) randomDance = 6;
        robot.putMouth(random(10,21));
        robot.home();//Otto stop
        pause(750);
        gaits(randomDance);
        
        pause(8000);
        break;


      //-- MODE 2 - Obstacle detector mode
      //---------------------------------------------------------
      case 2:
       robot.run(0); //Otto walk straight
            //robot.refresh();
            robot.putMouth(smile);
            pause(500);
            obstacleDetector();
            pause(500);
            
        if(obstacleDetected){

             robot.home();//Otto stop
             robot.putMouth(bigSurprise);
             pause(3000);
             robot.putMouth(xMouth);
             robot.run(1); //Otto walk back
             pause(3000);
             robot.putMouth(sad);
             robot.home();//Otto stop
             pause(3000);
             robot.turnL(1, 550);
             pause(3000);
             obstacleDetector();
        }

          robot.refresh();
        break;


      //-- MODE 3 - Noise detector mode
      //---------------------------------------------------------  
      case 3:
       
        
        break;
        

      //-- MODE 4 - OttoPAD or any Teleoperation mode (listening SerialPort) 
      //---------------------------------------------------------
      case 4:

        SerialCmd.readSerial();
        
        //If Otto is moving yet
        if (robot.getRestState()==false){  
          //move(moveId);
         
          gaits(moveId);
        }

         
          
       robot.refresh();
      
        break;      


      default:
          MODE=0;
          break;
    }

}
//////////////////////////////////////////////////////////////////////////
void  gaits(int cmd) 
{
  
  bool manualMode = false;
    
    bool taken = true;
    if (prev_cmd == cmd) return;

    switch (cmd) 
    {
      case 1:
      //MiniKame::run(int dir, float steps, float T) 
        robot.run(0);
        break;
      case 2:
        robot.run(1);
        break;
      case 3:
        robot.turnL(1, 550);
        break;
      case 4:
        robot.turnR(1, 550);
        break;
      case 5:
        robot.home();
        break;
      case 6:
        robot.pushUp();
        break;
      case 7:
        robot.upDown();
        break;
      case 8:
        robot.waveHAND();
        break;
      case 9:
        robot.Hide();
        break;
      case 10:
        robot.omniWalk(true);
        break;
      case 11:
        robot.omniWalk(false);
        break;
      case 12:
        robot.dance(1, 1000);
        break;
      case 13:
       robot.frontBack(1, 750);
        break;
      case 14:
       robot.jump();
        break;
        case 15:
       robot.scared();
        break;

      default:
        taken = false;
        manualMode = true;
    }
     if(!manualMode){
    sendFinalAck();

  }
  if (taken) prev_cmd = cmd;
  return taken;
}
////////////////////////////////////////////////////////////////////
void pause(int period) 
  {
    long timeout = millis() + period;
    do 
      {
      robot.refresh();
    } 
    while (millis() <= timeout);
  }
///////////////////////////////////////////////////////////////////
//-- Functions --------------------------------------------------//
///////////////////////////////////////////////////////////////////

//-- Function to read distance sensor & to actualize obstacleDetected variable
void obstacleDetector(){

   int distance = robot.getDistance();

        if(distance<15){
          obstacleDetected = true;
        }else{
          obstacleDetected = false;
        }
}
//-- Function to receive Stop command.
void receiveStop(){

    sendAck();
    robot.home();
    prev_cmd  = '.';
    sendFinalAck();

}
//-- Function to send Ack comand (A)
void sendAck(){

  delay(30);

  Serial.print(F("&&"));
  Serial.print(F("A"));
  Serial.println(F("%%"));
  Serial.flush();
}


//-- Function to send final Ack comand (F)
void sendFinalAck(){

  delay(30);

  Serial.print(F("&&"));
  Serial.print(F("F"));
  Serial.println(F("%%"));
  Serial.flush();
}
//-- Function to receive movement commands
void receiveMovement(){
    sendAck();
    
    if (robot.getRestState()==true){
        robot.setRestState(false);
    }

    robot.clearMouth();
    //Definition of Movement Bluetooth commands
    //M  MoveID  T   MoveSize  
    char *arg; 
    arg = SerialCmd.next(); 
    if (arg != NULL) {
      moveId=atoi(arg);
      robot.putMouth(smile);
      }
    else{
      robot.putMouth(xMouth); // bad command
      delay(2000);
      robot.clearMouth();
      moveId=0; //stop
    }
    
    arg = SerialCmd.next(); 
    if (arg != NULL) {T=atoi(arg);}
    else{
      T=1000;
    }

    arg = SerialCmd.next(); 
    if (arg != NULL) {moveSize=atoi(arg);}
    else{
      moveSize =15;
    }
}
//-- Function to receive LED commands
void receiveLED(){  

    //sendAck & stop if necessary
    sendAck();
    robot.home();

    //Examples of receiveLED Bluetooth commands
    //L 000000001000010100100011000000000
    //L 001111111111111111111111111111111 (todos los LED encendidos)
    unsigned long int matrix;
    char *arg;
    char *endstr;
    arg=SerialCmd.next();
    //Serial.println (arg);
    if (arg != NULL) {
      matrix=strtoul(arg,&endstr,2);    // Converts a char string to unsigned long integer
      robot.putMouth(matrix,false);
    }else{
      robot.putMouth(xMouth); // bad command
      delay(2000);
      robot.clearMouth();
    }

    sendFinalAck();

}
//-- Function to receive sing commands
void receiveSing() {
  //sendAck & stop if necessary
  sendAck();
   robot.home();

  //Definition of Sing Bluetooth commands
  //K  SingID
  int sing = 0;
  char *arg;
  arg = SerialCmd.next();
  if (arg != NULL) sing = atoi(arg);
  else  {
  robot.putMouth(xMouth); // bad command   
  delay(2000); // 
  robot.clearMouth();
  }
  

  switch (sing) {
    case 1: //K 1
      robot.sing(S_connection);
      break;
    case 2: //K 2
      robot.sing(S_disconnection);
      break;
    case 3: //K 3
      robot.sing(S_surprise);
      break;
    case 4: //K 4
      robot.sing(S_OhOoh);
      break;
    case 5: //K 5
      robot.sing(S_OhOoh2);
      break;
    case 6: //K 6
      robot.sing(S_cuddly);
      break;
    case 7: //K 7
      robot.sing(S_sleeping);
      break;
    case 8: //K 8
      robot.sing(S_happy);
      break;
    case 9: //K 9
      robot.sing(S_superHappy);
      break;
    case 10: //K 10
      robot.sing(S_happy_short);
      break;
    case 11: //K 11
      robot.sing(S_sad);
      break;
    case 12: //K 12
      robot.sing(S_confused);
      break;
    case 13: //K 13
      robot.sing(S_fart1);
      break;
    case 14: //K 14
      robot.sing(S_fart2);
      break;
    case 15: //K 15
      robot.sing(S_fart3);
      break;
    case 16: //K 16
      robot.sing(S_mode1);
      break;
    case 17: //K 17
      robot.sing(S_mode2);
      break;
    case 18: //K 18
      robot.sing(S_mode3);
      break;
    case 19: //K 19
      robot.sing(S_buttonPushed);
      break;
    default:
      break;
  }
  sendFinalAck();
}

//-- Function to send ultrasonic sensor measure (distance in "cm")
void requestDistance(){

    robot.home();  //stop if necessary  

    int distance = robot.getDistance();
    Serial.print(F("&&"));
    Serial.print(F("D "));
    Serial.print(distance);
    Serial.println(F("%%"));
    Serial.flush();
}
//-- Function to send battery voltage percent
void requestBattery(){

    robot.home();  //stop if necessary

    //The first read of the batery is often a wrong reading, so we will discard this value. 
    double batteryLevel = robot.getBatteryLevel();

    Serial.print(F("&&"));
    Serial.print(F("B "));
    Serial.print(batteryLevel);
    Serial.println(F("%%"));
    Serial.flush();
}


//-- Function to send program ID
void requestProgramId(){

    robot.home();   //stop if necessary

    Serial.print(F("&&"));
    Serial.print(F("I "));
    Serial.print(programID);
    Serial.println(F("%%"));
    Serial.flush();
}
//-- Function to receive mode selection.
void requestMode(){

    sendAck();
    robot.home();
    //Definition of Mode Bluetooth commands
    //J ModeID    
    char *arg; 
    arg = SerialCmd.next(); 
    if (arg != NULL) 
    {
      modeId=atoi(arg);
      robot.putMouth(heart);
      delay(1000);
      robot.clearMouth();
      }
    else{
      robot.putMouth(xMouth); // bad command
      delay(2000);
      robot.clearMouth();
      modeId=0; //stop
    }
switch (modeId) {
      case 0: //
        MODE = 0;
        break;
      case 1: //
        MODE = 1;
        robot.sing(S_mode1);
        robot.putMouth(one);
        delay(1000);
    delay(200);
        break;
        case 2: //
        MODE = 2;
        robot.sing(S_mode2);
        robot.putMouth(two);
        delay(1000);
        break;
        case 3: //
        MODE = 3;
        robot.sing(S_mode3);
        robot.putMouth(three);
        delay(1000);        
        break;
        case 4: //
        robot.sing(S_mode1);
        robot.putMouth(four);
        delay(1000);       
        MODE = 4;
        break;
     default:
        MODE = 0;
        break;
   }
sendFinalAck();
robot.clearMouth();
robot.putMouth(smile);

}
 
//-- Function to receive gesture commands
void receiveGesture(){

    //sendAck & stop if necessary
    sendAck();
    robot.home(); 

    //Definition of Gesture Bluetooth commands
    //H  GestureID  
    int gesture = 0;
    char *arg; 
    arg = SerialCmd.next(); 
    if (arg != NULL) {gesture=atoi(arg);}
    else 
    {
      robot.putMouth(xMouth);
      delay(2000);
      robot.clearMouth();
    }

    switch (gesture) {
      case 1: //H 1 
        robot.playGesture(OttoHappy);
        break;
      case 2: //H 2 
        robot.playGesture(OttoSuperHappy);
        break;
      case 3: //H 3 
        robot.playGesture(OttoSad);
        break;
      
      case 7: //H 7 
        robot.playGesture(OttoLove);
        break;   
      
        
      default:
        break;
    }

    sendFinalAck();
}

