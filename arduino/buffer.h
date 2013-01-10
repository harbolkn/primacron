/*
 * Buffer.h
 *
 * Buffer for storing read data
 */

#ifndef buffer_h
#define buffer_h

  
struct element_t{
  char *name;
  float data;
  struct element_t *next;
};

class Buffer{
public:
  Buffer* Buffer_init();

  bool push(Buffer *buffer, float data);

  float pop(Buffer*buffer);

  element_t *head;
  element_t *tail;
};

#endif
