#include "Colour.h"               /* colour models. */

int CL_div[CL_COLOUR_LEVELS][CL_LIGHT_LEVELS];
HW_pixel *CL_intensities;                   /* intensity table */

//初始化内部查找表，具体不是特别清楚
void CL_init_colour(void)
{
    for (size_t i = 0;i < CL_COLOUR_LEVELS; ++i)
        for (size_t j = 0;j < CL_LIGHT_LEVELS; ++j)
            //CL_LIGHT_MASK is for clamping light??? 0xff???
            CL_div[i][j] = (i*j) / CL_LIGHT_MASK;
}

/**********************************************************\
 * Composing a colour.                                    *
 *                                                        *
 * RETURNS: Packed colour, storable in a colourmap.       *
 * --------                                               *
\**********************************************************/

HW_pixel CL_colour(int red_light, int green_light, int blue_light)
{
 return((CL_clamp_red(red_light)<<CL_RED_SHIFT) |
        (CL_clamp_green(green_light)<<CL_GREEN_SHIFT) |
        (CL_clamp_blue(blue_light)<<CL_BLUE_SHIFT)
       );
}

/**********************************************************\
 * Lighting a single pixel.                               *
 *                                                        *
 * RETURNS: Packed colour, storable in a colourmap.       *
 * --------                                               *
\**********************************************************/

HW_pixel CL_light(HW_pixel colour,int red_light,
                                  int green_light,
                                  int blue_light
                  )
{
 return((CL_div[CL_red(colour)][CL_clamp_light(red_light)]<<CL_RED_SHIFT)       |
        (CL_div[CL_green(colour)][CL_clamp_light(green_light)]<<CL_GREEN_SHIFT) |
        (CL_div[CL_blue(colour)][CL_clamp_light(blue_light)]<<CL_BLUE_SHIFT)
       );
}
