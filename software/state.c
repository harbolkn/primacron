/*
 * state.c
 *
 * Serial state control
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "state.h"
#include "serial.h"

char state;

void get_state(){
  char read = serial_read();

  if(read == NO_ACTION){
   state = NO_ACTION;
  }
  else if(read == INITIALIZE){
    state = INITIALIZE;
  }
  else if(read == EXECUTE){
    state = EXECUTE;
  }
}

void write_line(FILE *fp){
  char write;

  while(!feof(fp)){
    write = getc(fp);
    serial_write(write);
    if(write == '\n'){
      state = NO_ACTION;
      break;
    }
  }

  if(write == (int)0)
    fclose(fp);
}
