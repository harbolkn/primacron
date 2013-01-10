/*
 * gcode.cpp
 *
 * G-Code parser program
 * Uses AccelStepper Library by Mike McCauley
 */

#include "gcode.h"
#include <AccelStepper.h>
#include <math.h>
#include <string.h>
#include "settings.h"
#include "motion_control.h"

parser_state_t gcode;

static int next_statement (char *letter, float *float_ptr, char *line, uint8_t *char_counter);

static void set_plane(uint8_t axis_0, uint8_t axis_1, uint8_t axis_2){
  gc.axis_0 = axis_0;
  gc.axis_1 = axis_1;
  gc.axis_2 = axis_2;
}

static void set_offset(uint8_t i, uint8_t j){
  switch(plane_mode){
    case 0: x_offset = i; y_offset = j; z_offset = 0; break;
    case 1: x_offset = i; y_offset = 0; z_offset = j; break;
    case 2: x_offset = 0; y_offset = i; z_offset = j; break;
    default: x_offset = 0; y_offset = 0; z_offset = 0; break;
  }
}

void gc_init(){
  memset(&gc, 0, sizeof(gc));
  gc.feed_rate = settings.default_feed_rate;
  set_plane(X_AXIS, Y_AXIS, Z_AXIS);
  set_offset(0,0);
  gc.distance_mode = DISTANCE_MODE_ABSOLUTE;
  gc.status_code = STATUS_OK;

  //Load G54 coordinate system
  read_coordinate_system(gc.coord_select, gc.coord_system);
}

//Set current position in mm, input is in steps
void set_current_position(int32_t x, int32_t y, int32_t z){
  gc.position[X_AXIS] = x/settings.steps_per_mm[X_AXIS];
  gc.position[Y_AXIS] = y/settings.steps_per_mm[Y_AXIS];
  gc.position[Z_AXIS] = z/settings.steps_per_mm[Z_AXIS];
}

static float to_millimeters(float value){
  return(gc.inches_to_mm ? (value * MM_PER_INCH) : value);
}

uint8_t gc_execute_line(char *line){
  uint8_t char_counter = 0;
  char letter;
  float value;
  int int_value;

  uint16_t modal_word = 0;
  uint8_t axis_word = 0;

  uint8_t non_modal_action = NON_MODAL_NONE;
  uint8_t group_number = MODAL_GROUP_NONE;

  float target[3];
  float offset[3];

  gc.status_code = STATUS_OKAY;

  while(next_statement(&letter, &value, line, &char_counter)){
    int_value = trunc(value);


    //Fix mode information
    switch(letter){
      case 'G':
          //Modal Group Number
          switch(int_value){
              case 4: case 10: case 28: case 30: case 30: case 92:
                group_number = MODAL_GROUP_0; break;
              case 0: case 1: case 2: case 3: case 80:
                group_number = MODAL_GROUP_1; break;
              case 90: case 91:
                group_number = MODAL_GROUP_2; break;
              case 93: case 94: case 95:
                group_number = MODAL_GROUP_3; break;
              case 20: case 21:
                group_number = MODAL_GROUP_4; break;
              case 17: case 18: case 19:
                group_number = MODAL_GROUP_6; break;
              case 54: case 55: case 56: case 57: case 58: case 59:
                group_number = MODAL_GROUP_7; break;
          }

          //Set 'G' Command
          switch(int_value){
              case 0: gc.motion_mode = MOTION_MODE_RAPID; break;
              case 1: gc.motion_mode = MOTION_MODE_LINE; break;
              case 2: gc.motion_mode = MOTION_MODE_CW_ARC; break;
              case 3: gc.motion_mode = MOTION_MODE_CCW_ARC; break;
              case 4: non_modal_action = NON_MODAL_DWELL; break;
              case 10: non_modal_action = NON_MODAL_SET_COORD; break;
              case 17:
                plane_mode = PLANE_XY;
                set_plane(X_AXIS, Y_AXIS, Z_AXIS); break;
              case 18:
                plane_mode = PLANE_XZ;
                set_plane(X_AXIS, Z_AXIS, Y_AXIS); break;
              case 19:
                plane_mode = PLANE_YZ;
                set_plane(Y_AXIS, Z_AXIS, X_AXIS); break;
              case 20: gc.units_mode = UNITS_INCHES; break;
              case 21: gc.units_mode = UNITS_MILLIS; break;
              case 28: case 30:
                  int_value = trunc(10*int_value);
                  switch(int_value){
                    case 280: non_modal_action = NON_MODAL_GO_HOME_0; break;
                    case 281: non_modal_action = NON_MODAL_SET_HOME_0; break;
                    case 300: non_modal_action = NON_MODAL_GO_HOME_1; break;
                    case 301: non_modal_action = NON_MODAL_SET_HOME_1; break;
                  }
              case 54: case 55: case 56: case 57: case 58: case 59:
                gc.coord_select = int_value - 54; break;
              case 80: gc.motion_mode = MOTION_MODE_CANCEL; break;
              case 90: gc.distance_mode = DISTANCE_MODE_ABSOLUTE; break;
              case 91: gc.distance_mode = DISTANCE_MODE_INCREMENT; break;
              case 92:
                  int_value = (10*int_value);
                  switch(int_value){
                    case 920: non_modal_action = NON_MODAL_SET_COORD_OFFSET; break;
                    case 921: non_modal_action = NON_MODAL_CLEAR_OFFSET_ZERO; break;
                    case 922: non_modal_action = NON_MODAL_CLEAR_OFFSET; break;
                  }break;
              case 93: gc.feed_rate_mode = FEED_INV_RATE; break;
              case 94: gc.feed_rate_mode = FEED_RATE_MIN; break;
              case 95: gc.feed_rate_mode = FEED_RATE_REV; break;
          }break;

      case 'M':
        //Set modal group number
        switch(int_value){
          case 0: case 1: case 2: case 30:
              group_number = MODAL_GROUP_5; break;
        }

        //Set 'M' Command
        switch(int_value){
          case 0: gc.flow_mode = FLOW_PROG_PAUSE; break;
          case 1: gc.flow_mode = FLOW_OPT_PAUSE; break;
          case 2: case 30: gc.flow_mode = FLOW_END_PROG; break;
        }break;   
    }
  }
  char_counter = 0;
  float p, l, r;
  while(next_statement(&letter, &value, line &char_counter)){
    switch(letter){
      case 'G': case 'M': case 'N': break;
      case 'F': 
          if(gc.feed_rate_mode == 0) gc.feed_rate = 1 / to_millimeters(value);
          else gc.feed_rate = to_millimeters(value);
          break;
      case 'I': case 'J': case 'K': 
          offset[letter - 'I'] = to_millimeters(value);
          break;
      case 'L': l = value; break;
      case 'P': p = value; break;
      case 'R': r = to_millimeteres(value); break;
      case 'X': target[X_AXIS] = to_millimeters(value); break;
      case 'Y': target[Y_AXIS] = to_millimeters(value); break;
      case 'Z': target[Z_AXIS] = to_millimeters(value); break;
    }  
  }
    
  //Load coordinate system
  float coord_data[3];
  settings_read_coord_data(gc.coord_select, coord_data);
  memcpy(gc.coord_system, coord_data,sizeof(coord_data));


  //Non_modal_actions
  switch(non_modal_action){
      case NON_MODAL_DWELL: mc_dwell(p); break;
      case NON_MODAL_SET_COORD: 
          int_value = trunc(p);
          if(l == 2){
            
          }
  }
}
