/*
 * Serial.c
 *
 * Handle serial communication with Arduino
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h> //UNIX function definitions
#include <fcntl.h> //File control definitions
#include <termios.h> //Terminal control definitions
#include "serial.h"

port_t port; //defint port

void serial_init(){
  struct termios config;
  if(!isatty(port.fd)){
    printf("Error: Port not found\n");
  }
  if(tcgetattr(port.fd, &config) < 0){
    printf("Error: Attribute handling failure\n");
  }
  //Input flags - Turn off input processing
  //convert break to null byte, no CR to NL translation,
  //no NL to CR translation, don't mark parity errors or breaks
  //no inpute parity check, don't strip high bit off
  //no XON/XOFF software flow control
  config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR |
                     PARMRK | INPCK | ISTRIP | IXON);

  //Output flags - Turns off output processing
  //no CR to NL translation, no NL to CR-NL translation,
  //no NL to CR translation, no column 0 CR suppression,
  //no Ctrl-D suppression, no fill characters, no case mapping,
  //no local ouptut processing
  config.c_oflag = 0;

  //No line processing:
  //echo off, echo newline off, canonical mode off,
  //extended input processing off, signal chars off
  config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

  //Turn off character processing
  //clear current char size mask, no parity checking,
  //no output processing, force 8 bit input
  config.c_cflag &= ~(CSIZE | PARENB);
  config.c_cflag |= CS8;

  //One input byte is enough to eturn from read()
  //Intern-character timer off
  config.c_cc[VMIN] = 1;
  config.c_cc[VTIME] = 0;

  //Communication speed (simple version, using te predefined constants)
  if(cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0)
    printf("Error: Baud Rate set failure\n");

  //Apply configuration
  if(tcsetattr(port.fd, TCSAFLUSH, &config) < 0)
    printf("Error: Attributes not set\n");
}

int serial_open(){
  port.fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);

  if (port.fd == -1){
    printf("Error: Could not open ttyUSB0 port\n");
  }
  else{
    printf("Serial port opened successfully\n");
    fcntl(port.fd, F_SETFL, 0);
  }

  return(port.fd);
}

void serial_write(unsigned char data){
  int n = write(port.fd, &data, sizeof(data));
  
  if (n < 0){
    printf("Error: Write() failure\n");
  }
  else{
    //printf("Wrote: %c\n",data);
  }
}

unsigned char serial_read(){
  return fcntl(port.fd, F_SETFL, 0); //Non blocking read
}

void serial_close(){
  close(port.fd);
  printf("Port Closed\n");
}
