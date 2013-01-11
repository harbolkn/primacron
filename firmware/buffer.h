/*
 * Buffer.h
 *
 * Buffer for storing read data
 */

#ifndef buffer_h
#define buffer_h

#include "element.h"

class Buffer{
public:
  Element *head;
  Element *tail;
  int buf_size;

  Buffer();
  float push(float data);
  float pop();
  float* print_buf();
  float print_head();
  int print_size();
};

#endif
