#include <EEPROM.h>
#include "OTTOKame.h"

/*
   (servo index, pin to attach pwm)
   __________ __________ _________________
  |(3,9)_____)(1,8)      (0,2)(______(2,3)|
  |__|       |left FRONT right|        |__|
             |                |
             |                |
             |                |
   _________ |                | __________
  |(7,7)_____)(5,6)______(4,4)(______(6,5)|
  |__|                                 |__|

*/
//comment below manually setting trim in MiniKame() constructor
#define __LOAD_TRIM_FROM_EEPROM__
#define EEPROM_MAGIC  0xabcd
#define EEPROM_OFFSET 2   //eeprom starting offset to store trim[]

// FRONT_RIGHT_HIP servo 0  Pin 2
// FRONT_LEFT_HIP servo 1  Pin 8
// FRONT_RIGHT_LEG servo 2  Pin 3
// FRONT_LEFT_LEG servo 3  Pin 9
// BACK_RIGHT_HIP servo 4  Pin 4
// BACK_LEFT_HIP servo 5  Pin 6
// BACK_RIGHT_LEG servo 6  Pin 5
// BACK_LEFT_LEG servo 7  Pin 7
MiniKame::MiniKame():/* reverse{0, 0, 0, 0, 0, 0, 0, 0}, */trim{0, 0, 0, 0, 0, 0, 0, 0} {
  board_pins[FRONT_RIGHT_HIP] = 2; // front left inner
  board_pins[FRONT_LEFT_HIP] = 8; // front right inner
  board_pins[BACK_RIGHT_HIP] = 4; // back left inner
  board_pins[BACK_LEFT_HIP] = 6; // back right inner
  board_pins[FRONT_RIGHT_LEG] = 3; // front left outer
  board_pins[FRONT_LEFT_LEG] = 9; // front right outer
  board_pins[BACK_RIGHT_LEG] = 5; // back left outer
  board_pins[BACK_LEFT_LEG] = 7; // back right outer
}
void MiniKame::reverseServo(int id) {
  if (reverse[id])
    reverse[id] = 0;
  else
    reverse[id] = 1;
}
void MiniKame::init(int Buzzer) {
  //Buzzer & noise sensor pins: 


   //Buzzer & noise sensor pins: 
  pinBuzzer = Buzzer;
  //pinMode(NoiseSensor,INPUT);

  /*
     trim[] for calibrating servo deviation,
     initial posture (home) should like below
     in symmetric
        \       / front left
         \_____/
         |     |->
         |_____|->
         /     \
        /       \ front right
  */
  /*
    trim[FRONT_LEFT_HIP] = 0;
    trim[FRONT_RIGHT_HIP] = -8;
    trim[BACK_LEFT_HIP] = 8;
    trim[BACK_RIGHT_HIP] = 5;

    trim[FRONT_LEFT_LEG] = 2;
    trim[FRONT_RIGHT_LEG] = -6;
    trim[BACK_LEFT_LEG] = 6;
    trim[BACK_RIGHT_LEG] = 5;
  */
#ifdef __LOAD_TRIM_FROM_EEPROM__
  int val = EEPROMReadWord(0);
  if (val != EEPROM_MAGIC) {
    EEPROMWriteWord(0, EEPROM_MAGIC);
    storeTrim();
  }
#endif

  for (int i = 0; i < 8; i++) {
    servo[i].attach(board_pins[i]);

#ifdef __LOAD_TRIM_FROM_EEPROM__
    int val = EEPROMReadWord(i * 2 + EEPROM_OFFSET);
    if (val >= -90 && val <= 90) {
      trim[i] = val;
    }
#endif
  }

  home();
  us.init(12, 11);
}
void MiniKame::attachServo(){
   for (int i = 0; i < 8; i++) {
    servo[i].attach(board_pins[i]);
  }
}
void MiniKame::detachServo(){
   for (int i = 0; i < 8; i++) {
    servo[i].detach();
  }
}
void MiniKame::turnL(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  //int hi = 23;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {0, 180, 90, 90, 180, 0, 90, 90};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::turnR(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {180, 0, 90, 90, 0, 180, 90, 90};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::dance(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }
  digitalWrite(13, 0);
  int x_amp = 0;
  int z_amp = 40;
  int ap = 30;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {0, 0, 0, 270, 0, 0, 90, 180};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::frontBack(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int x_amp = 30;
  int z_amp = 25;
  int ap = 20;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {0, 180, 270, 90, 0, 180, 90, 270};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::run(int dir, float steps, float T) {
 
        setRestState(true);

digitalWrite(13, 0);
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 0;
  int front_x = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {    90 + ap - front_x,
                      90 - ap + front_x,
                      90 - hi,
                      90 + hi,
                      90 - ap - front_x,
                      90 + ap + front_x,
                      90 + hi,
                      90 - hi
                 };
  int phase[] = {0, 0, 90, 90, 180, 180, 90, 90};
  if (dir == 1) {
    phase[0] = phase[1] = 180;
    phase[4] = phase[5] = 0;
  }
  execute(steps, period, amplitude, offset, phase);
  setRestState(false);
}

void MiniKame::omniWalk(bool side, float T, float turn_factor) {
  if(getRestState()==true){
        setRestState(false);
  }
  digitalWrite(13, 0);
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;
  int front_x = 6 * (1 - pow(turn_factor, 2));
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {    90 + ap - front_x,
                      90 - ap + front_x,
                      90 - hi,
                      90 + hi,
                      90 - ap - front_x,
                      90 + ap + front_x,
                      90 + hi,
                      90 - hi
                 };

  int phase[8];
  if (side) {
    int phase1[] =  {0,   0,   90,  90,  180, 180, 90,  90};
    int phase2R[] = {0,   180, 90,  90,  180, 0,   90,  90};
    for (int i = 0; i < 8; i++)
      phase[i] = phase1[i] * (1 - turn_factor) + phase2R[i] * turn_factor;
  }
  else {
    int phase1[] =  {0,   0,   90,  90,  180, 180, 90,  90};
    int phase2L[] = {180, 0,   90,  90,  0,   180, 90,  90};
    for (int i = 0; i < 8; i++)
      phase[i] = phase1[i] * (1 - turn_factor) + phase2L[i] * turn_factor;// + oscillator[i].getPhaseProgress();
  }

  execute(1, period, amplitude, offset, phase);
}

void MiniKame::moonwalkL(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int z_amp = 45;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {0, 0, z_amp, z_amp, 0, 0, z_amp, z_amp};
  int offset[] = {90, 90, 90, 90, 90, 90, 90, 90};
  int phase[] = {0, 0, 0, 120, 0, 0, 180, 290};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::walk(int dir, float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int x_amp = 15;
  int z_amp = 20;
  int ap = 20;
  int hi = -10;
  float period[] = {T, T, T / 2, T / 2, T, T, T / 2, T / 2};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {   90 + ap,
                     90 - ap,
                     90 - hi,
                     90 + hi,
                     90 - ap,
                     90 + ap,
                     90 + hi,
                     90 - hi
                 };
  int  phase[] = {270, 270, 270, 90, 90, 90, 90, 270};
  if (dir == 0) { //backward
    phase[0] = phase[1] = 90;
    phase[4] = phase[5] = 270;
  }
  for (int i = 0; i < 8; i++) {
    oscillator[i].reset();
    oscillator[i].setPeriod(period[i]);
    oscillator[i].setAmplitude(amplitude[i]);
    oscillator[i].setPhase(phase[i]);
    oscillator[i].setOffset(offset[i]);
    oscillator[i].start();
  }
  unsigned long _init_time = millis();
  unsigned long _now_time = _init_time;
  unsigned long _final_time = _init_time + period[0] * steps;
  bool side;

  while (_now_time < _final_time) {
    side = (int)((_now_time - _init_time) / (period[0] / 2)) % 2;

    setServo(0, oscillator[0].update()); //FRONT_RIGHT_HIP
    setServo(1, oscillator[1].update()); //FRONT_LEFT_HIP
    setServo(4, oscillator[4].update()); //BACK_RIGHT_HIP
    setServo(5, oscillator[5].update()); //BACK_LEFT_HIP

    if (side == 0) {
      setServo(3, oscillator[3].update()); //FRONT_LEFT_LEG
      setServo(6, oscillator[6].update()); //BACK_RIGHT_LEG
    }
    else {
      setServo(2, oscillator[2].update()); //FRONT_RIGHT_LEG
      setServo(7, oscillator[7].update()); //BACK_LEFT_LEG
    }
    pause(1);
    _now_time = millis();
  }

}

void MiniKame::upDown(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int x_amp = 0;
  int z_amp = 35;
  int ap = 20;
  //int hi = 25;
  int hi = 0;
  int front_x = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {    90 + ap - front_x,
                      90 - ap + front_x,
                      90 - hi,
                      90 + hi,
                      90 - ap - front_x,
                      90 + ap + front_x,
                      90 + hi,
                      90 - hi
                 };
  int phase[] = {0, 0, 90, 270, 180, 180, 270, 90};

  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::pushUp(float steps, float T) {
  if(getRestState()==true){
        setRestState(false);
  }

  int z_amp = 40;
  int x_amp = 65;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {0, 0, z_amp, z_amp, 0, 0, 0, 0};
  int offset[] = {90, 90, 90 - hi, 90 + hi, 90 - x_amp, 90 + x_amp, 90 + hi, 90 - hi};
  int phase[] = {0, 0, 0, 180, 0, 0, 0, 180};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::home() {


  int ap = 20;
  int hi = 0;
  int position[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  for (int i = 0; i < 8; i++) {
    if (position[i] + trim[i] <= 180 && position[i] + trim[i] > 0) {
      oscillator[i].stop();
      setServo(i, position[i] + trim[i]);
    }
    isOttoResting=true;
  }
}

void MiniKame::hello() {
  float sentado[] = {90 + 15, 90 - 15, 90 - 65, 90 + 65, 90 + 20, 90 - 20, 90 + 10, 90 - 10};
  moveServos(150, sentado);
  pause(200);

  int z_amp = 40;
  int x_amp = 60;
  int T = 350;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {0, 50, 0, 50, 0, 0, 0, 0};
  int offset[] = {
    90 + 15, 40,
    90 - 10, 90 + 10,
    90 + 20, 90 - 20,
    90 + 65, 90
  };

  int phase[] = {0, 0, 0, 90, 0, 0, 0, 0};

  execute(4, period, amplitude, offset, phase);

  float goingUp[] = {160, 20, 90, 90, 90 - 20, 90 + 20, 90 + 10, 90 - 10};
  moveServos(500, goingUp);
  pause(200);

}

void MiniKame::jump() {
  //float sentado[] = {90 + 15, 90 - 15, 90 - 65, 90 + 65, 90 + 20, 90 - 20, 90 + 10, 90 - 10};
  float sentado[] = {
    90 + 15, 90 - 15, //front hips servos
    90 - 10, 90 + 10, //front leg servos
    90 + 10, 90 - 10, // back hip servos
    90 + 65, 90 - 65  // back leg servos
  };
  int ap = 20;
  int hi = 35;
  float salto[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap * 3, 90 + ap * 3, 90 + hi, 90 - hi};

  moveServos(150, sentado);
  pause(200);
  moveServos(0, salto);
  pause(100);
  home();
}
bool MiniKame::getRestState(){

    return isOttoResting;
}

void MiniKame::setRestState(bool state){

    isOttoResting = state;
}

//////////////////////////////////////////////////////////////////////////////////////
void MiniKame::moveServos(int time, float target[8]) {
  if(getRestState()==true){
        setRestState(false);
  }
attachServo();
  float _increment[8];
  float _servo_position[8] = {90, 90, 90, 90, 90, 90, 90, 90};
  unsigned long _final_time;
  unsigned long _partial_time;
  if (time > 10) {
    for (int i = 0; i < 8; i++)  _increment[i] = (target[i] - (_servo_position[i] + trim[i])) / (time / 10.0);
    _final_time =  millis() + time;

    while (millis() < _final_time) {
      _partial_time = millis() + 10;
      for (int i = 0; i < 8; i++) setServo(i, (_servo_position[i] + trim[i]) + _increment[i]);
      //while (millis() < _partial_time); //pause
      pause(_partial_time);
    }
  }
  else {
    for (int i = 0; i < 8; i++) setServo(i, target[i]);
  }
  for (int i = 0; i < 8; i++) _servo_position[i] = target[i];
}

void MiniKame::setServo(int id, float target) {
  attachServo();
  if (!reverse[id])
  servo[id].write(target + trim[id]);
    
  else
  servo[id].write(180 - (target + trim[id]));
    
}


void MiniKame::execute(float steps, float period[8], int amplitude[8], int offset[8], int phase[8]) {
  if(getRestState()==true){
        setRestState(false);
  }
attachServo();
          for (int i = 0; i < 8; i++) {
          oscillator[i].setPeriod(period[i]);
          oscillator[i].setAmplitude(amplitude[i]);
          oscillator[i].setPhase(phase[i]);
          oscillator[i].setOffset(offset[i]);
          oscillator[i].start();
          oscillator[i].setTime(millis());
       
      }
}

void MiniKame::refresh() {
  for (int i = 0; i < 8; i++) {
    if (oscillator[i].isStop()){
      servo[i].detach();
    continue;
    }
    setServo(i, oscillator[i].update());
  }
}

void MiniKame::storeTrim() {
  for (int i = 0; i < 8; i++) {
    EEPROMWriteWord(i * 2 + EEPROM_OFFSET, trim[i]);
    delay(100);
  }
}

// load/send only trim of hip servo
void MiniKame::loadTrim() {
  //FRONT_LEFT/RIGHT_HIP
  for (int i = 0; i < 4; i++) {
    Serial.write(EEPROM.read(i + EEPROM_OFFSET));
  }

  //BACK_LEFT/RIGHT_HIP
  for (int i = 8; i < 12; i++) {
    Serial.write(EEPROM.read(i + EEPROM_OFFSET));
  }
}

int MiniKame::EEPROMReadWord(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void MiniKame::EEPROMWriteWord(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}


///////////////////////////////////////////////////////////////////
//-- MOUTHS & ANIMATIONS ----------------------------------------//
///////////////////////////////////////////////////////////////////

              
unsigned long int MiniKame::getMouthShape(int number){
  unsigned long int types []={zero_code,one_code,two_code,three_code,four_code,five_code,six_code,seven_code,eight_code,
  nine_code,smile_code,happyOpen_code,happyClosed_code,heart_code,bigSurprise_code,smallSurprise_code,tongueOut_code,
  vamp1_code,vamp2_code,lineMouth_code,confused_code,diagonal_code,sad_code,sadOpen_code,sadClosed_code,
  okMouth_code, xMouth_code,interrogation_code,thunder_code,culito_code,angry_code};

  return types[number];
}


unsigned long int MiniKame::getAnimShape(int anim, int index){

  unsigned long int littleUuh_code[]={
     0b00000000000000001100001100000000,
     0b00000000000000000110000110000000,
     0b00000000000000000011000011000000,
     0b00000000000000000110000110000000,
     0b00000000000000001100001100000000,
     0b00000000000000011000011000000000,
     0b00000000000000110000110000000000,
     0b00000000000000011000011000000000  
  };

  unsigned long int dreamMouth_code[]={
     0b00000000000000000000110000110000,
     0b00000000000000010000101000010000,  
     0b00000000011000100100100100011000,
     0b00000000000000010000101000010000           
  };

  unsigned long int adivinawi_code[]={
     0b00100001000000000000000000100001,
     0b00010010100001000000100001010010,
     0b00001100010010100001010010001100,
     0b00000000001100010010001100000000,
     0b00000000000000001100000000000000,
     0b00000000000000000000000000000000
  };

  unsigned long int wave_code[]={
     0b00001100010010100001000000000000,
     0b00000110001001010000100000000000,
     0b00000011000100001000010000100000,
     0b00000001000010000100001000110000,
     0b00000000000001000010100100011000,
     0b00000000000000100001010010001100,
     0b00000000100000010000001001000110,
     0b00100000010000001000000100000011,
     0b00110000001000000100000010000001,
     0b00011000100100000010000001000000    
  };


  unsigned long int otto_code[]={
     0b00001100010010010010010010001100,
     0b00000000000000000000000000000000,
     0b00011100001000001000001000001000,
     0b00000000000000000000000000000000,
     0b00011100001000001000001000001000,
     0b00000000000000000000000000000000,
     0b00001100010010010010010010001100,
     0b00000000000000000000000000000000  
  };


  switch  (anim){

    case littleUuh:
        return littleUuh_code[index];
        break;
    case dreamMouth:
        return dreamMouth_code[index];
        break;
    case adivinawi:
        return adivinawi_code[index];
        break;
    case wave:
        return wave_code[index];
        break;
   // case otto:
       // return otto_code[index];
        //break;        
  }   
}


void MiniKame::putAnimationMouth(unsigned long int aniMouth, int index){

      ledmatrix.writeFull(getAnimShape(aniMouth,index));
}


void MiniKame::putMouth(unsigned long int mouth, bool predefined){

  if (predefined){
    ledmatrix.writeFull(getMouthShape(mouth));
  }
  else{
    ledmatrix.writeFull(mouth);
  }
}


void MiniKame::clearMouth(){

  ledmatrix.clearMatrix();
}

///////////////////////////////////////////////////////////////////
//-- SENSORS FUNCTIONS  -----------------------------------------//
///////////////////////////////////////////////////////////////////

//---------------------------------------------------------
//-- Otto getDistance: return Otto's ultrasonic sensor measure
//---------------------------------------------------------
float MiniKame::getDistance(){

  return us.read();
}


//---------------------------------------------------------
//-- Otto getNoise: return Otto's noise sensor measure
//---------------------------------------------------------
int MiniKame::getNoise(){

  int noiseLevel = 0;
  int noiseReadings = 0;
  int numReadings = 2;  

    noiseLevel = analogRead(pinNoiseSensor);

    for(int i=0; i<numReadings; i++){
        noiseReadings += analogRead(pinNoiseSensor);
        delay(4); // delay in between reads for stability
    }

    noiseLevel = noiseReadings / numReadings;

    return noiseLevel;
}
//---------------------------------------------------------
//-- Otto getBatteryLevel: return battery voltage percent
//---------------------------------------------------------
double MiniKame::getBatteryLevel(){

  //The first read of the batery is often a wrong reading, so we will discard this value. 
    double batteryLevel = battery.readBatPercent();
    double batteryReadings = 0;
    int numReadings = 10;

    for(int i=0; i<numReadings; i++){
        batteryReadings += battery.readBatPercent();
        delay(1); // delay in between reads for stability
    }

    batteryLevel = batteryReadings / numReadings;

    return batteryLevel;
}


double MiniKame::getBatteryVoltage(){

  //The first read of the batery is often a wrong reading, so we will discard this value. 
    double batteryLevel = battery.readBatVoltage();
    double batteryReadings = 0;
    int numReadings = 10;

    for(int i=0; i<numReadings; i++){
        batteryReadings += battery.readBatVoltage();
        delay(1); // delay in between reads for stability
    }

    batteryLevel = batteryReadings / numReadings;

    return batteryLevel;
}

///////////////////////////////////////////////////////////////////
//-- SOUNDS -----------------------------------------------------//
///////////////////////////////////////////////////////////////////

void MiniKame::_tone (float noteFrequency, long noteDuration, int silentDuration){

    // tone(10,261,500);
    // delay(500);

      if(silentDuration==0){silentDuration=1;}

      TimerFreeTone(MiniKame::pinBuzzer, noteFrequency, noteDuration);
      //delay(noteDuration);       //REMOVED FOR TimerFreeTone, PUT BACK for TONE       milliseconds to microseconds
      //noTone(PIN_Buzzer);
      
      //delay(silentDuration);     //REMOVED FOR TimerFreeTone, PUT BACK for TONE
}


void MiniKame::bendTones (float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration){

  //Examples:
  //  bendTones (880, 2093, 1.02, 18, 1);
  //  bendTones (note_A5, note_C7, 1.02, 18, 0);

  if(silentDuration==0){silentDuration=1;}

  if(initFrequency < finalFrequency)
  {
      for (int i=initFrequency; i<finalFrequency; i=i*prop) {
          _tone(i, noteDuration, silentDuration);
      }

  } else{

      for (int i=initFrequency; i>finalFrequency; i=i/prop) {
          _tone(i, noteDuration, silentDuration);
      }
  }
}


void MiniKame::sing(int songName){
  switch(songName){

    case S_connection:
      _tone(note_E5,50,30);
      _tone(note_E6,55,25);
      _tone(note_A6,60,10);
    break;

    case S_disconnection:
      _tone(note_E5,50,30);
      _tone(note_A6,55,25);
      _tone(note_E6,50,10);
    break;

    case S_buttonPushed:
      bendTones (note_E6, note_G6, 1.03, 20, 2);
      delay(30);
      bendTones (note_E6, note_D7, 1.04, 10, 2);
    break;

    case S_mode1:
      bendTones (note_E6, note_A6, 1.02, 30, 10);  //1318.51 to 1760
    break;

    case S_mode2:
      bendTones (note_G6, note_D7, 1.03, 30, 10);  //1567.98 to 2349.32
    break;

    case S_mode3:
      _tone(note_E6,50,100); //D6
      _tone(note_G6,50,80);  //E6
      _tone(note_D7,300,0);  //G6
    break;

    case S_surprise:
      bendTones(800, 2150, 1.02, 10, 1);
      bendTones(2149, 800, 1.03, 7, 1);
    break;

    case S_OhOoh:
      bendTones(880, 2000, 1.04, 8, 3); //A5 = 880
      delay(200);

      for (int i=880; i<2000; i=i*1.04) {
           _tone(note_B5,5,10);
      }
    break;

    case S_OhOoh2:
      bendTones(1880, 3000, 1.03, 8, 3);
      delay(200);

      for (int i=1880; i<3000; i=i*1.03) {
          _tone(note_C6,10,10);
      }
    break;

    case S_cuddly:
      bendTones(700, 900, 1.03, 16, 4);
      bendTones(899, 650, 1.01, 18, 7);
    break;

    case S_sleeping:
      bendTones(100, 500, 1.04, 10, 10);
      delay(500);
      bendTones(400, 100, 1.04, 10, 1);
    break;

    case S_happy:
      bendTones(1500, 2500, 1.05, 20, 8);
      bendTones(2499, 1500, 1.05, 25, 8);
    break;

    case S_superHappy:
      bendTones(2000, 6000, 1.05, 8, 3);
      delay(50);
      bendTones(5999, 2000, 1.05, 13, 2);
    break;

    case S_happy_short:
      bendTones(1500, 2000, 1.05, 15, 8);
      delay(100);
      bendTones(1900, 2500, 1.05, 10, 8);
    break;

    case S_sad:
      bendTones(880, 669, 1.02, 20, 200);
    break;

    case S_confused:
      bendTones(1000, 1700, 1.03, 8, 2); 
      bendTones(1699, 500, 1.04, 8, 3);
      bendTones(1000, 1700, 1.05, 9, 10);
    break;

    case S_fart1:
      bendTones(1600, 3000, 1.02, 2, 15);
    break;

    case S_fart2:
      bendTones(2000, 6000, 1.02, 2, 20);
    break;

    case S_fart3:
      bendTones(1600, 4000, 1.02, 2, 20);
      bendTones(4000, 3000, 1.02, 2, 20);
    break;

  }
}


///////////////////////////////////////////////////////////////////
//-- GESTURES ---------------------------------------------------//
///////////////////////////////////////////////////////////////////

void MiniKame::playGesture(int gesture){

 // int sadPos[4]=      {110, 70, 20, 160};
  //int bedPos[4]=      {100, 80, 60, 120};
  //int fartPos_1[4]=   {90, 90, 145, 122}; //rightBend
  //int fartPos_2[4]=   {90, 90, 80, 122};
  //int fartPos_3[4]=   {90, 90, 145, 80};
  //int confusedPos[4]= {110, 70, 90, 90};
  //int angryPos[4]=    {90, 90, 70, 110};
  //int headLeft[4]=    {110, 110, 90, 90};
  //int headRight[4]=   {70, 70, 90, 90};
  //int fretfulPos[4]=  {90, 90, 90, 110};
  //int bendPos_1[4]=   {90, 90, 70, 35};
  //int bendPos_2[4]=   {90, 90, 55, 35};
  //int bendPos_3[4]=   {90, 90, 42, 35};
  //int bendPos_4[4]=   {90, 90, 34, 35};
  
  switch(gesture){

    case OttoHappy: 
        //_tone(note_E5,50,30);
        putMouth(smile);
       // sing(S_happy_short);
       // swing(1,800,20); 
       // sing(S_happy_short);

        home();
        putMouth(happyOpen);
    break;


    case OttoSuperHappy:
        putMouth(happyOpen);
        //sing(S_happy);
        delay(500);
        putMouth(happyClosed);
       // tiptoeSwing(1,500,20);
       delay(500);
        putMouth(happyOpen);
       // sing(S_superHappy);
       delay(500);
        putMouth(happyClosed);
       // tiptoeSwing(1,500,20); 

        home();  
        delay(1000);
        putMouth(happyOpen);
    break;


    case OttoSad: 
        putMouth(sad);
       // _moveServos(700, sadPos);     
       // bendTones(880, 830, 1.02, 20, 200);
        putMouth(sadClosed);
       // bendTones(830, 790, 1.02, 20, 200);  
        putMouth(sadOpen);
       // bendTones(790, 740, 1.02, 20, 200);
        putMouth(sadClosed);
       // bendTones(740, 700, 1.02, 20, 200);
        putMouth(sadOpen);
        //bendTones(700, 669, 1.02, 20, 200);
        putMouth(sad);
        delay(500);

        home();
        delay(1000);
        putMouth(happyOpen);
    break;





    
    case OttoLove:
        putMouth(heart);
       // sing(S_cuddly);
        //crusaito(2,1500,15,1);

        home(); 
       // sing(S_happy_short);  
       delay(500);
       putMouth(happyOpen);
        delay(500);
         putMouth(heart);
       // sing(S_cuddly);
        //crusaito(2,1500,15,1);
        home(); 
       // sing(S_happy_short);  
        delay(1000);
    
        putMouth(happyOpen);
    break;




  }
}    



