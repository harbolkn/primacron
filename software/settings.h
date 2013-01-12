/*
 * settings.h
 *
 * Default settings for CNC
 */

#ifndef settings_h
#define settings_h

#define MM_CONVERSION 25.4
#define FEED_CONVERSION 20.9974 //steps/sec speed

typedef struct{
  float steps_per_mm;
  unsigned char microsteps;
  //Feed rate expressed in mm/min
  float default_feed_rate;
  float default_speed;
  float max_feed_rate;
  float max_speed;
  float max_acceleration;
  unsigned char home_coord[3];
  unsigned char x_axis_step, x_axis_dir;
  unsigned char y_axis_step, y_axis_dir;
  unsigned char z_axis_step, z_axis_dir;
}settings_t;
extern settings_t settings;

void settings_init();

#endif
