/*
 * settings.c
 *
 * Default settings for machine
 */

#include "settings.h"
#include "numbers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

settings_t settings;

void settings_init(){
  memset(&settings, 0, sizeof(settings));

  settings.steps_per_mm = 78.74;
  settings.microsteps = 16;
  settings.default_feed_rate = 200;
  settings.default_speed = feed_to_speed(&settings.default_feed_rate);
  settings.max_feed_rate = 450;
  settings.max_speed = feed_to_speed(&settings.max_feed_rate);
  settings.max_acceleration = 80000;
  settings.x_axis_step = 6;
  settings.x_axis_dir = 7;
  settings.y_axis_step = 2;
  settings.y_axis_dir = 3;
  settings.z_axis_step = 4;
  settings.z_axis_dir = 5;
}
