/*
 * gcode.h
 *
 * G-code parser
 */

#ifndef gcode_h
#define gcode_h

//Define modale group numbers
//Used to differentiate types of commands within the code
//Modal groups are mutually exclusive and cannot execute on the same line
#define MODAL_GROUP_NONE  0
#define MODAL_GROUP_0     1 //Non-modal commands
#define MODAL_GROUP_1     2 //Motion
#define MODAL_GROUP_2     3 //Distance
#define MODAL_GROUP_3     4 //Feed Rate
#define MODAL_GROUP_4     5 //Units
#define MODAL_GROUP_5     6 //Stopping
#define MODAL_GROUP_6     7 //Plane Selection
#define MODAL_GROUP_7     8 //Coordinate System Selection [G54-G59.3]

#define NON_MODAL_NONE        0
#define NON_MODAL_DWELL       1 //G4 - Dwell (seconds)
#define NON_MODAL_SET_COORD   2 //G10 L2[0] - Set coordinate systems
#define NON_MODAL_GO_HOME_0   3 //G28 - Return to reference point 1
#define NON_MODAL_SET_HOME_0  4 //G28.1 - Set reference point 1
#define NON_MODAL_GO_HOME_1   5 //G30 - Return to reference point 2
#define NON_MODAL_SET_HOME_1  6 //G30.1 - Set reference point 2
#define NON_MODAL_SET_COORD_OFFSET  7 //G92 - Offset coordinate system
#define NON_MODAL_CLEAR_OFFSET_ZERO 8 //G92.1 - Cancel offset and set to zero
#define NON_MODAL_CLEAR_OFFSET      9 //G92.2 - Cancel offset and leave parameter

#define MOTION_MODE_RAPID   0 //G0 - Coordinated Straight Motion Rapid, no cutting
#define MOTION_MODE_LINE    1 //G1 - Coordinated Straight Motion Feed Rate
#define MOTION_MODE_CW_ARC  2 //G2 - Clockwise Arc
#define MOTION_MODE_CCW_ARC 3 //G3 - Counterclockwise Arc
#define MOTION_MODE_CANCEL  4 //G80 - Cancel motion mode

#define DISTANCE_MODE_ABSOLUTE  0 //G90 - Absolute Distance Mode
#define DISTANCE_MODE_ARC_ABS   1 //G90.1 - Arc center I,J,K absolute
#define DISTANCE_MODE_INCREMENT 2 //G91 - Incremental Distance Mode
#define DISTANCE_MODE_ARC_INC   3 //G91.1 - Arc center I,J,K relative to start point

#define FEED_INV_RATE   0 //G93 - Inverse time feed rate
#define FEED_RATE_MIN   1 //G94 - Units per minute feed rate
#define FEED_RATE_REV   2 //G95 - Units per revolution feed rate

#define UNITS_INCHES    0 //G20 - Inches
#define UNITS_MILLIS    1 //G21 - Milimeters

#define FLOW_RUNNING         0 //Running
#define FLOW_PROG_PAUSE      1 //M0 - Program Pause
#define FLOW_OPT_PAUSE       2 //M1 - Optional Pause
#define FLOW_END_PROG        3 //M2,M30 - End Program

#define PLANE_XY    0 //G17 - Select XY Plane
#define PLANE_XZ    1 //G18 - Select XZ Plane
#define PLANE_YZ    2 //G19 - Select YZ Plane

typedef struct{
  uint8_t status_code;
  uint8_t motion_mode;      //Motion code
  uint8_t distance_mode;    //Distance code
  uint8_t feed_rate_mode;   //Feed code
  uint8_t units_mode;       //0-inches, 1-millimeters
  uint8_t flow_mode;        //Flow code
  uint8_t plane_mode;       //Plane code

  uint8_t coord_select;     //Working coordinate system
  uint8_t axis_0,
          axis_1,
          axis_2,
          x_offset
          y_offset,
          z_offset
          radius,
          turns;            //Axis as set by plane mode
  float coord_system[3];    //Reference points from coordinate systems (G54+)
  float coord_offset[3];    //G92 coordinate system after offest
  float postion[3];         //Current position


  float feed_rate;          //FXX - Millimeters/min
} parse_state_t;
extern parse_state_t gcode;

//Initialize
void gc_init();

//Execute on block of g-code
uint8_t gc_execute_line(char *line);

//Set g-code parser position
void set_current_position(int32_t x, int32_t y, int32_t z);

#endif
