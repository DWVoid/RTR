#include <windows.h>
#include <string.h>                         /* memset memcpy */

#define main _main                          /* to accomodate windows */

void HW_set_int(int *dst, long lng, int val);

typedef int HW_pixel;
#define HW_set_pixel(dst,lng,val) HW_set_int(dst,lng,val)
#define HW_copy_pixel(src,dst,lng) memcpy(dst,src,lng*sizeof(int))

#define HW_SCREEN_X_SIZE 600
#define HW_SCREEN_Y_SIZE 600                /* number of pixels total */
#define HW_SCREEN_LINE_SIZE HW_SCREEN_X_SIZE

int main(int n, char **o);

void HW_error(char *s,...);
void HW_close_event_loop(void);
void HW_init_event_loop(void (*application_main)(void),
                        void (*application_key_handler)(int key_code)
                       );

