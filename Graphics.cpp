#include "RayTracing.h"           /* hardware specific stuff */
#include "Colour.h"               /* colour and light */
#include "Graphics.h"           /* graphics functions */

HW_pixel *G_c_buffer;                       /* the bitmap's bits */
int G_page_start;                           /* always 0 for _MONO_ */
long G_c_buffer_size;                       /* allocated size for clearings */

int *G_z_buffer;                            /* Z buffer */
long G_z_buffer_size;                       /* it's size */

//为颜色表分配空间
void G_init_graphics()
{
    //空白空间的计算
    G_c_buffer_size = HW_SCREEN_LINE_SIZE*HW_SCREEN_Y_SIZE;

    //HW_pixel==int
    G_c_buffer = new HW_pixel[G_c_buffer_size];

    G_page_start = 0;                            /* page 0 by default */
    if (G_c_buffer == nullptr) HW_error("(Graphics) Not enough memory.\n");
               
    G_z_buffer_size = HW_SCREEN_LINE_SIZE*HW_SCREEN_Y_SIZE;

    //不同的就是int,HW_pixel
    G_z_buffer = new int[G_z_buffer_size];
    if (G_z_buffer == nullptr) HW_error("(Graphics) Not enough memory.\n");
}

/**********************************************************\
 * Clearing the bitmap with the specified colour.         *
\**********************************************************/


void G_clear(HW_pixel colour, int red, int green, int blue)
{
    HW_set_pixel(G_c_buffer, G_c_buffer_size, CL_light(colour, red, green, blue));
    HW_set_int(G_z_buffer, G_z_buffer_size, INT_MAX);
}

/**********************************************************\
 * Setting a pixel.                                       *
\**********************************************************/

void G_pixel(int *vertex, HW_pixel colour)
{
    G_c_buffer[vertex[1] * HW_SCREEN_LINE_SIZE + vertex[0] + G_page_start] 
        = colour;
}