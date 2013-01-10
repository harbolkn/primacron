/*
 * Buffer.cpp
 *
 * Buffer for storing input data
 * FIFO Buffer
 */

#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"

//Add item to buffer's tail
bool Buffer::push(Buffer *buffer, float data){
  struct element_t *element = (element_t *)malloc(sizeof(struct element_t)*1);

  element->data = data;
  element->next = buffer->head;

  if(buffer->head == NULL){
    buffer->head = element;
    buffer->tail = element;
  }

  buffer->tail->next = element;

  buffer->tail = element;

  return 1;
}

float Buffer::pop(Buffer *buffer){
 struct element_t new_head = *(buffer->head);
 new_head.next = buffer->head;

 if(buffer->head == buffer->tail){
  buffer->head = buffer->tail = NULL;
 }
 else{
  buffer->head = buffer->head->next;
 }

 free(new_head.next);

 return new_head.data;
}

Buffer* Buffer::Buffer_init(){
  Buffer *buffer = (Buffer *)malloc(sizeof(Buffer)*1);
  buffer->head = buffer->tail = NULL;

  return buffer;
}
