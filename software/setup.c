/*
 * setup.c
 *
 * Write data.txt for settings
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "setup.h"
#include "settings.h"

void setup_data(){
  FILE *fp;
  fp = fopen("data.txt", "w+");

  if(fp == NULL) printf("Error: File not opened\n");
  else{
    
    fprintf(fp, "%d ", settings.x_axis_step);
    fprintf(fp, "%d ", settings.x_axis_dir);
    fprintf(fp, "%d ", settings.y_axis_step);
    fprintf(fp, "%d ", settings.y_axis_dir);
    fprintf(fp, "%d ", settings.z_axis_step);
    fprintf(fp, "%d ", settings.z_axis_dir);
    fprintf(fp, "%f ", settings.max_speed);
    fprintf(fp, "%f ", settings.max_acceleration);
    fprintf(fp, "%d ", settings.home_coord[0]);
    fprintf(fp, "%d ", settings.home_coord[1]);
    fprintf(fp, "%d\n", settings.home_coord[2]);

    fclose(fp);
  }
}
