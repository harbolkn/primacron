/*
 * Buffer.cpp
 *
 * Buffer for storing input data
 * FIFO Buffer
 */

#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"
#include "element.h"


//Add item to buffer's tail
float Buffer::push (float data){
  Element *element = (Element *)malloc(sizeof(Element)*1);
  element->data = data;
  element->next = head;

  if(head == NULL){
    head = element;
    tail = element;
  }

  tail->next = element;

  tail = element;

  buf_size++;

  return tail->data;
}

float Buffer::pop(){
  Element new_head = *(head);
  new_head.next = head;

  if(head == tail){
    head = tail = NULL;
  }
  else{
    head = head->next;
  }

  buf_size--;

  return new_head.data;
}

float Buffer::print_head(){
  return head->data;
}

float* Buffer::print_buf(){
  Element *ref = head;
  float *buffer = (float *)malloc(sizeof(float)*buf_size);

  int temp = buf_size-1;

  while(temp>=0){ 
    buffer[temp] = ref->data;
    ref = ref->next;
    temp--;
  }

  free(buffer);
  
  return buffer;
}

int Buffer::print_size(){
  return buf_size;
}

Buffer::Buffer(){
  head = tail = NULL; 
  buf_size = 0;
}


