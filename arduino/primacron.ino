/*
 * primacron.ino
 *
 * Upload to arduino for execution of G-code
 */
#include <AccelStepper.h>

//Meta data
uint8_t _off=1
uint8_t status;
int i=0;

#define NO_ACTION 0
#define INITIALIZE 1
#define EXECUTE 2

//Initialization Data
uint8_t _xpins[2], _ypins[2], _zpins[2];
float max_speed, accel;
long _currentPos[3];

//Execution Data
float speed;
long _targetPos[3];

#define DIRECTION_CW 0
#define DIRECTIONE_CCW 1

void setup(){
  Serial.begin(9600);
  status = INITIALIZE;
}

void loop(){
  byte inputByte;

  if (status != NO_ACTION){
    //Get byte from serial interface
    if(Serial.available()>0){
      inputByte = Serial.read();
    
      switch(status){
        case NO_ACTION: break;
        case INITIALIZE: //Happens before first new line
          if (inputByte = 0x0A) setup(); i=0; status = EXECUTE; //End of initialization
          else if (inputByte = 0x20) ; //Do nothing
          else {
            switch(i){
            case 0: X_AXIS[0] = inputByte; Serial.write(X_AXIS[0]); i++; break;
            case 1: X_AXIS[1] = inputByte; Serial.write(X_AXIS[1]); i++; break;
            case 2: Y_AXIS[0] = inputByte; Serial.write(Y_AXIS[0]); i++; break;
            case 3: Y_AXIS[1] = inputByte; Serial.write(Y_AXIS[1]); i++; break;
            case 4: Z_AXIS[0] = inputByte; Serial.write(Z_AXIS[0]); i++; break;
            case 5: Z_AXIS[1] = inputByte; Serial.write(Z_AXIS[1]); i++; break;
            case 6: max_speed = inputByte; Serial.write(max_speed); i++; break;
            case 7: accel = inputByte; Serial.write(accel); i++; break;
            case 8: _currentPos[0] = inputByte; Serial.write(_currentPos[0]); i++; break;
            case 9: _currentPos[1] = inputByte; Serial.write(_currentPos[1]); i++; break;
            case 10: _currentPos[2] = inputeByte; Serial.write(_currentPos[2]); i++; break;
            default: Serial.write("Error: To many initialization commands");
                     Serial.write("Command:");
                     Serial.write(inputByte);
            }
          } break;
        case EXECUTE://Happens on every line past init
          if (inputByte = 0x0A) run(); break; //Check if execute has completed
          else if (inputByte = 0x20) ; //Do nothing
          else{
            switch(i){//Think of switching this with a ring buffer
            case 0: _speed = inputByte; Serial.write(_speed); i++; break;
            case 1: _targetPos[0] = inputByte; Serial.write(_targetPos[0]); i++; break;
            case 2: _targetPos[1] = inputByte; Serial.write(_targetPos[1]); i++; break;
            case 3: _targetPos[2] = inputByte; Serial.write(_targetPos[2]); i++; break;
            default: Serial.write("Error: Too many execute commands");
                     Serial.write("Command:");
                     Serial.write(inputByte);
            }
          }
      }
    }
  }
  else{
   //NO_ACTION state
   Serial.write("Waiting");
  }
}

boolean setup(){
  //Initialize steppers
  AccelStepper X_AXIS(1, _xpins[0], _xpins[1]);
  AccelStepper Y_AXIS(1, _ypins[0], _ypins[1]);
  AccelStepper Z_AXIS(1, _zpins[0], _zpins[1]);

  //Max speeds
  X_AXIS.setMaxSpeed(max_speed);
  Y_AXIS.setMaxSpeed(max_speed);
  Z_AXIS.setMaxSpeed(max_speed);

  //Accelerations
  X_AXIS.setAcceleration(acceleration);
  Y_AXIS.setAcceleration(acceleration);
  Z_AXIS.setAcceleration(acceleration);

  //Current Position
  X_AXIS.setCurrentPosition(_currentPos[0]);
  Y_AXIS.setCurrentPosition(_currentPos[1]);
  Z_AXIS.setCurrentPosition(_currentPos[2]);

  return 1;
}

boolean run(){
  //Set speed
  X_AXIS.setSpeed(_speed);
  Y_AXIS.setSpeed(_speed);
  Z_AXIS.setSpeed(_speed);

  //Start moving
  X_AXIS.moveTo(_targetPos[0]);
  Y_AXIS.moveTo(_targetPos[1]);
  Z_AXIS.moveTo(_targetPos[2]);

  wait();
}

boolean wait(){
  Serial.end();

  if(X_AXIS.distanceToGo == 0 && Y_AXIS.distanceToGo == 0 && Z_AXIS.distanceToGo == 0){
    status = EXECUTE;
    Serial.begin(9600);
    return 1;
  }
  else {
    status = NO_ACTION;
    
    //Run motors
    X_AXIS.run();
    Y_AXIS.run();
    Z_AXIS.run();

    wait();
    return 0;
  }
}
