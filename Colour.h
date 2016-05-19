#ifndef _COLOUR_H_
#define _COLOUR_H_
#include "RayTracing.h"           /* HW_pixel */

#define CL_LNG_COLOUR           3           /* R G B */
                      /* 32 bits per pixel */
#define CL_COLOUR_LEVELS      256           /* number of colours */
#define CL_LIGHT_LEVELS       256           /* gradations of light intensity */
#define CL_COLOUR_MASK_RED   0xff           /* for clamping colours */
#define CL_COLOUR_MASK_GREEN 0xff           
#define CL_COLOUR_MASK_BLUE  0xff           
#define CL_LIGHT_MASK        0xff           /* for clamping light */
#define CL_RED_SHIFT           16           /* offset for red */
#define CL_GREEN_SHIFT          8           /* offset for green */
#define CL_BLUE_SHIFT           0           /* offset for blue */

#define CL_clamp_red(colour)   (((colour)>CL_COLOUR_MASK_RED)?CL_COLOUR_MASK_RED:(colour))
#define CL_clamp_green(colour) (((colour)>CL_COLOUR_MASK_GREEN)?CL_COLOUR_MASK_GREEN:(colour))
#define CL_clamp_blue(colour)  (((colour)>CL_COLOUR_MASK_BLUE)?CL_COLOUR_MASK_BLUE:(colour))
#define CL_clamp_light(colour) (((colour)>CL_LIGHT_MASK)?CL_LIGHT_MASK:(colour))
#define CL_red(colour)   (((colour)>>CL_RED_SHIFT)&CL_COLOUR_MASK_RED)
#define CL_green(colour) (((colour)>>CL_GREEN_SHIFT)&CL_COLOUR_MASK_GREEN)
#define CL_blue(colour)  (((colour)>>CL_BLUE_SHIFT)&CL_COLOUR_MASK_BLUE)

void CL_init_colour();

HW_pixel CL_colour(int red_light, int green_light, int blue_light);
HW_pixel CL_light(HW_pixel colour,
    int red_light, int green_light, int blue_light);
#endif