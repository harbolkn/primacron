#include "buffer.h"
#include "element.h"
#include <AccelStepper.h>

//Meta Data
int i=0, j=0;
double a;
char value[100];
uint8_t state;
bool dec=0; //Assume no decimal part

#define NO_ACTION 0
#define INITIALIZE 1
#define EXECUTE 2

//Buffer variables
Buffer buffer;

#define DIRECTION_CW 0
#define DIRECTION_CCW 1

AccelStepper axes[3];

void setup(){
  Serial.begin(9600);
  state = INITIALIZE;
}

void loop(){
  char input;
  
  if(Serial.available()){
    input = (char)Serial.read();
    
    switch(state){
      case NO_ACTION: break;
      case INITIALIZE: //First line of data.txt
          switch(input){
            case ' ': 
                if(dec == 1) frac_part();
                else whole_part();
                break; //Advance to next input
            case '\n': 
                if(dec == 1) frac_part();
                else whole_part();
                machine_setup(); break;
            case '.': dec = 1; whole_part(); break;
            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                  value[j] = input - '0'; j++; break;
          }break;
       case EXECUTE: 
          switch(input){
            case ' ': frac_part(); break; //Advance to next input
            case '\n': frac_part();
                exec(); break;
            case '.': whole_part(); break;
            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                  value[j] = input - '0'; j++; break;
          }break;
    }
  }
}

void machine_setup(){
  Serial.println();
  Serial.println("Initiating");
  
  AccelStepper X_AXIS(1, (uint8_t)buffer.pop(), (uint8_t)buffer.pop());
  AccelStepper Y_AXIS(1, (uint8_t)buffer.pop(), (uint8_t)buffer.pop());
  AccelStepper Z_AXIS(1, (uint8_t)buffer.pop(), (uint8_t)buffer.pop());
  
  axes[0] = X_AXIS;
  axes[1] = Y_AXIS;
  axes[2] = Z_AXIS;
  
  double max_speed = buffer.pop();
  Serial.print("Max-speed: ");
  Serial.println(max_speed);
  axes[0].setMaxSpeed((float)max_speed);
  axes[1].setMaxSpeed((float)max_speed);
  axes[2].setMaxSpeed((float)max_speed);
  
  double accel = buffer.pop();
  Serial.print("Acceleration: ");
  Serial.println(accel);
  axes[0].setAcceleration((float)accel);
  axes[1].setAcceleration((float)accel);
  axes[2].setAcceleration((float)accel);
  
  axes[0].setCurrentPosition((long)buffer.pop());
  axes[1].setCurrentPosition((long)buffer.pop());
  axes[2].setCurrentPosition((long)buffer.pop());  
  
  Serial.println("Error Check:");
  Serial.print("Buffer Empty: ");
  Serial.println(buffer.print_size());
  Serial.println();
  
  state = EXECUTE;
}

void exec(){
  Serial.println();
  Serial.println("Executing");
  
  AccelStepper X_AXIS = axes[0];
  AccelStepper Y_AXIS = axes[1];
  AccelStepper Z_AXIS = axes[2];
  
  double _speed = buffer.pop();
  Serial.print("Speed: ");
  Serial.println((float)_speed);
  X_AXIS.setSpeed((float)_speed);
  Y_AXIS.setSpeed((float)_speed);
  Z_AXIS.setSpeed((float)_speed);

  X_AXIS.moveTo((long)buffer.pop());
  Y_AXIS.moveTo((long)buffer.pop());
  Z_AXIS.moveTo((long)buffer.pop());  
  
  Serial.println("Error Check:");
  Serial.print("Buffer Empty: ");
  Serial.println(buffer.print_size());
  Serial.println();
  
  wait();
}

int wait(){
  Serial.println();
  Serial.println("Waiting for motion complete...");
  
  AccelStepper X_AXIS = axes[0];
  AccelStepper Y_AXIS = axes[1];
  AccelStepper Z_AXIS = axes[2];
  
  if(X_AXIS.distanceToGo() == 0 && Y_AXIS.distanceToGo() == 0 && Z_AXIS.distanceToGo() == 0){
    Serial.println("Reached Target");
    state = EXECUTE;
    return 1;
  }
  else{
    state = NO_ACTION;
    
    X_AXIS.run();
    Y_AXIS.run();
    Z_AXIS.run();
    
    wait();
    return 0;
  }
}

void whole_part(){
  int temp = j-1;
  a = 0;
  for(int k=0; k<=temp; k++){
    j--;
    a += ((double)value[j])*(pow(10,k));
    value[j]=0;
  }
  j=0;
}

void frac_part(){
  for(int k=0; k<=j-1; k++){
    a += ((double)value[k])*(pow(10,-(k+1)));
    value[k]=0;
  }
  j=0;

  buffer.push((float)a);
  Serial.print("Pushed: ");
  Serial.println(a);
}
