/*
 * Gcode.h
 *
 * Gcode parser
 * Make data.txt file (format in data.template)
 * Use commands from gcode.txt
 */

#ifndef gcode_h
#define gcode_h

#define DISTANCE_MODE_ABSOLUTE 0 //G90.X, absolute mode
#define DISTANCE_MODE_RELATIVE 1 //G91.X, relative mode

//Sustain motion mode till set differently
#define MOTION_MODE_SEEK 0 //G0 mode, rapid movement, no cut
#define MOTION_MODE_LINEAR 1 //G1 mode, normal motion, w[/o] cut
#define MOTION_MODE_CW_ARC 2 //G2 mode, move clockwise, create arc
#define MOTION_MODE_CCW_ARC 3 //G3 mode, move counter, create arc

//Control starting, stopping and end motion
#define FLOW_RUNNING 0 //Running Normally
#define FLOW_PROG_PAUSE 1 //M0 - Program Pause
#define FLOW_OPT_PAUSE 2 //M1 - Optional Pause
#define FLOW_END_PROGRAM 3 //M2,M30  - End Program

//Unit mode, default to millimeters
#define UNIT_INCHES 0 //G20 - Inches
#define UNIT_MILLIS 1 //G21 - Millimeters

//Feed rate, determine how to convert feed to speed (mm/min)
#define FEED_INV_RATE 0 //G93 - Inverse time feed rate (min/mm)
#define FEED_RATE_MIN 1 //G94 - Units per minute feed rate

typedef struct{
  unsigned char status_code; //Flow control
  unsigned char unit_code; //Unit mode
  unsigned char feed_code; //Feed rate control mode
  unsigned char motion_code; //Motion mode
  unsigned char distance_code; //Distance mode

  int currentPos[3]; //Current position
  int targetPos[3]; //Target position

  float feed_rate; //Feed rate, mm/min
  float speed; //Speed, steps/sec
} parse_state_t;
extern parse_state_t gc;

//Initialize
void gc_init();

//Parse one line of g-code
//Set parse_state values
unsigned char gc_parse_line(char *line, int size);

//Execute one line of g-code
//Write parse_state values to data.txt for execution by arduino
void gc_execute_line();

//Determine value for code
void parse_value(float *value, int *int_value, char *line, int *i);

#endif
