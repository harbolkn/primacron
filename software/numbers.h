/*
 * numbers.h
 *
 * Number conversion header file
 */

#ifndef numbers_h
#define numbers_h

void whole_part(int size, float *value, float *char_value);

void frac_part(int size, float *value, float *char_value);

float feed_to_speed(float *feed_rate);

void to_millimeters(float *value);

void mm_to_steps (float *value);

#endif
