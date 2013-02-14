/*
 * Serial.h
 *
 * Header file for serial interface
 */

#ifndef serial_h
#define serial_h

typedef struct{
  int fd;
}port_t;
extern port_t port;

void serial_init();

int serial_open();

void serial_write(unsigned char data);

char serial_read();

void serial_close();

#endif
