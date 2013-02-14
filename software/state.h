/*
 * state.h
 *
 * Control state control based on interface between arduino and computer
 */

#ifndef state_h
#define state_h

#define NO_ACTION '$' 
#define INITIALIZE '%' 
#define EXECUTE '^'

extern char state;

void get_state();

void write_line(FILE *fp);

#endif
