/*
 * numbers.c
 *
 * Handles number conversions
 */

#include <math.h>
#include <stdio.h>
#include "numbers.h"
#include "settings.h"

void whole_part(int size, float *value, float *char_value){
  int temp = size - 1;
  int k;

  for(k = 0; k <= temp; k++){
    *value += (char_value[size-1])*(pow(10,k));
    size--;
  }
}

void frac_part(int size, float *value, float *char_value){
  int k;

  for(k = 0; k <= size - 1; k++){
    *value += (char_value[k])*(pow(10,-(k+1)));
  }
}

float feed_to_speed(float *feed_rate){
  return *feed_rate * FEED_CONVERSION;
}

void to_millimeters(float *value){
  *value = *value * MM_CONVERSION;
}

void mm_to_steps(float *value){
  *value = *value * settings.steps_per_mm;
}
