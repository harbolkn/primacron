/*
 * main.c
 *
 * Workstation based program
 * Send serial data to arduino based off G-code
 *
 * Work flow:
 *  Parse G-Code file to data file
 *  Read data file sending to arduino
 */

#include <stdio.h>
#include <string.h>

#include "serial.h"

int main(void){
  //Initialize system
  serial_open(); // /dev/ttyUSB0 port set
  serial_init();

  //G-Code handling
  

  //data.txt now written
  //Send over serial
  FILE *fp; //File pointer
  char output;

  fp = fopen("workstation/data.txt", "r");

  if(fp == NULL) printf("Error: File not opened\n");
  else {
    printf("File opened\n");

    while(!feof(fp)) {
      output = getc(fp);
      serial_write(output);
    }

    fclose(fp);
  }

  return 0;
}
