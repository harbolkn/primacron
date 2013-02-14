/*
 * gcode.c
 *
 * G-code parser
 * Make data.txt file from gcode file
 */

#include "gcode.h"
#include <math.h>
#include "settings.h"
#include "numbers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


parse_state_t gc;

void parse_value(float *value, int *int_value, char *line, int *i){
  float *char_value = (float *)malloc(sizeof(float)*15);
  int size = 0;
  int is_whole = 1, is_neg = 0;

  char eval = (char)line[*i+1];

        while((eval >= '0' && eval <= '9') || eval == '.' || eval == '-'){

          switch(eval){
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                char_value[size] = (float)(eval - '0');
                size++;
                break;
            case '.':
                is_whole = 0;
                whole_part(size, value, char_value);
                
                free(char_value);
                char_value = (float*)malloc(sizeof(float)*15);

                size = 0;
                break;
            case '-':
                is_neg = 1;
                break;
          }

          *i=*i+1;
          eval = (char)line[*i+1];
        }

        if(is_neg){
          *value = *value * -1;
        }
        if(is_whole){
          whole_part(size, value, char_value);
          *int_value = (int)*value;
        }
        else{
          frac_part(size, value, char_value);
        }

  free(char_value);
}

void gc_init(){
  //Set everything to 0
  memset(&gc, 0 , sizeof(gc));

  gc.status_code = FLOW_RUNNING; //Set running
  gc.unit_code = UNIT_MILLIS; //Millimeters default mode
  gc.feed_code = FEED_RATE_MIN; //Feed rate default as mm/min
  //motion_code - No default motion code, must be set
  gc.distance_code = DISTANCE_MODE_ABSOLUTE; //Default to absolute positions

  gc.feed_rate = settings.default_feed_rate;
  gc.speed = settings.default_speed;

  //All target and current positions set to 0
}

unsigned char gc_parse_line(char *line, int size){
  int i = 0;
  
  //Items in line
  char letter;
  float value;
  int int_value;

  while(i <= size && line[i] != '\n' && (int)line[i] != 13){
    value = 0;
    int_value = 0;
    letter = (char)line[i];
    letter = toupper(letter);
    parse_value(&value, &int_value, line, &i);
    int_value = value;

    switch(letter){
      case 'G': //G prefaced codes
        switch(int_value){
          case 0: 
              gc.motion_code = MOTION_MODE_SEEK; 
              gc.feed_rate = settings.max_feed_rate;
              gc.speed = settings.max_speed;
              break;
          case 1: 
              gc.motion_code = MOTION_MODE_LINEAR; 
              gc.feed_rate = settings.default_feed_rate;
              gc.speed = settings.default_speed;
              break;
          case 2: 
              gc.motion_code = MOTION_MODE_CW_ARC; 
              gc.feed_rate = settings.default_feed_rate;
              gc.speed = settings.default_speed;
              break;
          case 3: 
              gc.motion_code = MOTION_MODE_CCW_ARC; 
              gc.feed_rate = settings.default_feed_rate;
              gc.speed = settings.default_speed;
              break;
          case 20: gc.unit_code = UNIT_INCHES; break;
          case 21: gc.unit_code = UNIT_MILLIS; break;
          case 90: gc.distance_code = DISTANCE_MODE_ABSOLUTE; break;
          case 91: gc.distance_code = DISTANCE_MODE_RELATIVE; break;
          case 93: gc.feed_code = FEED_INV_RATE; break;
          case 94: gc.feed_code = FEED_RATE_MIN; break;
        }break;
      case 'M': //M prefaced codes
        switch(int_value){
          case 0: gc.status_code = FLOW_PROG_PAUSE; break;
          case 1: gc.status_code = FLOW_OPT_PAUSE; break;
          case 2: case 30: gc.status_code = FLOW_END_PROGRAM; break;
        }break;
      case 'F': //Feed Rate calculation
        switch(gc.feed_code){
          case FEED_INV_RATE:
              gc.feed_rate = 1/value;
              gc.speed = feed_to_speed(&gc.feed_rate);
              break;
          case FEED_RATE_MIN:
              gc.feed_rate = value;
              gc.speed = feed_to_speed(&gc.feed_rate);
              break;
        }
          if(gc.feed_rate > settings.max_feed_rate){
            gc.feed_rate = settings.max_feed_rate;
            gc.speed = settings.max_speed;
          }
        
        break;
      //Axis values, read as floats
      //Input based on unit_code, covert all to millimeters
      case 'X': //X axes target
        mm_to_steps(&value);
        switch(gc.unit_code){
          case UNIT_INCHES: to_millimeters(&value); break;
        }
        switch(gc.distance_code){
          case DISTANCE_MODE_ABSOLUTE:
            gc.targetPos[0] = value; break;
          case DISTANCE_MODE_RELATIVE:
            gc.targetPos[0] = gc.currentPos[0] + value; break;
        }break;
      case 'Y': //Y axes target
        mm_to_steps(&value);
        switch(gc.unit_code){
          case UNIT_INCHES: to_millimeters(&value); break;
        }
        switch(gc.distance_code){
          case DISTANCE_MODE_ABSOLUTE:
            gc.targetPos[1] = value; break;
          case DISTANCE_MODE_RELATIVE:
            gc.targetPos[1] = gc.currentPos[1] + value; break;
        }break;
      case 'Z': //Z axes target
        mm_to_steps(&value);
        switch(gc.unit_code){
          case UNIT_INCHES: to_millimeters(&value); break;
        }
        switch(gc.distance_code){
          case DISTANCE_MODE_ABSOLUTE:
            gc.targetPos[2] = value; break;
          case DISTANCE_MODE_RELATIVE:
            gc.targetPos[2] = gc.currentPos[2] + value; break;
        }break;
    }
 
    i++;
  }

  return 0;
}


//Take values parsed from gcode and write to data.txt to execute
void gc_execute_line(){
  //Current Position
  gc.currentPos[0] = gc.targetPos[0];
  gc.currentPos[1] = gc.targetPos[1];
  gc.currentPos[2] = gc.targetPos[2];
  
  //File handling
  FILE *fp;
  fp = fopen("data.txt", "a+");

  if(fp == NULL) printf("Error: File not opened\n");
  else{

    fprintf(fp, "%f ", gc.speed);
    fprintf(fp, "%d ", gc.currentPos[0]);
    fprintf(fp, "%d ", gc.currentPos[1]);
    fprintf(fp, "%d\n", gc.currentPos[2]);

    fclose(fp);
  }
}
