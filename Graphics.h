#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "RayTracing.h"           /* HW_pixel */

extern HW_pixel *G_c_buffer;
extern long G_c_buffer_size;
void G_init_graphics();

void G_clear(HW_pixel colour,int red,int green,int blue);
void G_pixel(int *vertex,HW_pixel colour);

#endif
