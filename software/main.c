/*
 * main.c
 *
 * Send serial data to arduino based off G-code
 *
 * Work flow:
 *  Parse G-Code file to data file
 *  Read data file sending to arduino
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gcode.h"
#include "numbers.h"
#include "serial.h"
#include "settings.h"
#include "setup.h"


int main(int argc, char *argv[]){
  //Initialize system
  serial_open(); // /dev/ttyUSB0 port set
  serial_init();

  //Setup command
  settings_init();  
  setup_data();

  //G-Code handling
  gc_init();
  char *input_file = argv[1];
  char *input_line, input_char[1];
  int input_size = 0;
  FILE *input;
  input = fopen(input_file, "r");
  
  input_line = (char *)malloc(sizeof(char)*100);

  if(input == NULL) printf("Error: File not opened\n");
  else{
    printf("%s opened\n\n", argv[1]);

    while(!feof(input)){
      input_char[0] = getc(input);
      if(input_char[0] != '\n'){
        input_line = strcat(input_line,  input_char);
        input_size++;
      }
      else{
        gc_parse_line(input_line, input_size);
        gc_execute_line();
        free(input_line);
        input_line = (char*)malloc(sizeof(char)*100);
        input_size = 0;
      }
    }
  }

  free(input_line);

  //data.txt now written
  //Send over serial
  FILE *fp; //File pointer
  char output;

  fp = fopen("data.txt", "r+");

  if(fp == NULL) printf("Error: File not opened\n");
  else {

    while(!feof(fp)) {
      output = getc(fp);
      serial_write(output);
    }

    fclose(fp);
  }

  return 0;
}
