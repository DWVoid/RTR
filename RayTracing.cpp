#include "RayTracing.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>                         /* var arg stuff */
#include "Trace.h"
#include "Colour.h"
#include "Graphics.h"
#include "Algebra.h"
#include <string.h>

int HW_cmd_show;
HINSTANCE HW_instance;
char HW_class_name[] = "RayTracing windows";
HWND HW_wnd;                                /* window */
HPALETTE HW_palette;                        /* the palette headers */


HDC HW_mem;
HBITMAP HW_bmp;
RECT HW_rect;

char path[128]="";

Scene *w;

void HWI_null_function(void) {}
void (*HW_application_main)(void) = HWI_null_function;
void (*HW_application_key_handler)(int key_code);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**********************************************************\
 * Quiting with a message.                                *
\**********************************************************/

#define HW_MAX_ERROR_MESSAGE 256

void HW_error(char *s,...)
{
	char str[HW_MAX_ERROR_MESSAGE];
	va_list lst;

	va_start(lst,s);
	vsprintf(str,s,lst);                       /* forming the message */
	va_end(lst);

	MessageBox(NULL,str,"3D RayTracing",MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL);

	HW_close_event_loop();
	exit(0);                                   /* above might not be enough */
}
/**********************************************************\
 * Quitting the event loop.                               *
\**********************************************************/

void HW_close_event_loop(void)
{
    PostMessage(HW_wnd, WM_CLOSE, 0, 0L);         /* telling ourselves to quit */
}
/**********************************************************\
 * Implementations for fast memory fill.                  *
\**********************************************************/

void HW_set_int(int *d, long l, int v)
{
    for (size_t i = 0; i < l; ++i) *d++ = v;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#define HW_MAX_CLINE_OPTIONS 20


int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, 
				   int nShowCmd )
{
	WNDCLASS w;
	int n;
	char *start,*end;
	char *o[HW_MAX_CLINE_OPTIONS];

	HW_cmd_show = nShowCmd;
	if ((HW_instance = hPrevInstance) == NULL) 
	{
		w.style = CS_HREDRAW|CS_VREDRAW;
		w.lpfnWndProc = WndProc;
		w.cbClsExtra = 0;
		w.cbWndExtra = 0;
		w.hInstance = hInstance;
		w.hIcon = NULL;
		w.hCursor = NULL;
		w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		w.lpszMenuName = NULL;
		w.lpszClassName = HW_class_name;

		if (!RegisterClass(&w)) 
		{
			return FALSE;
		}
	}
	
	n = 0;
	o[n++] = "";
	start = lpCmdLine;
	while ((end = strchr(start, ' ')) != NULL) 
	{
		if (n >= HW_MAX_CLINE_OPTIONS)
		{
			HW_error("(RayTracing windows) Way too many command line options.\n");
		}

		if (end != start)
		{
			o[n++] = start;
		}

		*end = 0;
		start = end + 1;
	}

	if (strlen(start) > 0)
	{
		o[n++] = start;
	}

	return(main(n,0));

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
	case WM_PAINT: 
		HW_application_main();
		break;
	case WM_ERASEBKGND:
		return(1L); 
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		HW_application_key_handler(wParam);
		break;
	default:
		return(DefWindowProc(hWnd,message,wParam,lParam));
	}
	
	return(0L);
}

/**********************************************************\
 * Creating a window.                                     *
\**********************************************************/

#if defined(_RGB_)
void HW_init_screen(char *display_name,
                    char *screen_name
                   )
#endif
{
	PAINTSTRUCT ps;
	int i,remap;

	HW_wnd = CreateWindow(HW_class_name,
						screen_name,
						WS_SYSMENU,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						HW_SCREEN_X_SIZE,
						HW_SCREEN_Y_SIZE+GetSystemMetrics(SM_CYCAPTION),
						NULL,
						NULL,
						HW_instance,
						NULL);

	HW_mem=CreateCompatibleDC(BeginPaint(HW_wnd,&ps));
	if(((GetDeviceCaps(ps.hdc,PLANES))!=1)||
	((GetDeviceCaps(ps.hdc,BITSPIXEL))!=sizeof(HW_pixel)*8)
	)

	HW_error("(Hardware) I'd rather have %d bit screen.",
		   sizeof(HW_pixel)*8
		  );

	HW_bmp=CreateCompatibleBitmap(ps.hdc,HW_SCREEN_X_SIZE,HW_SCREEN_Y_SIZE);
	SelectObject(HW_mem,HW_bmp);

	EndPaint(HW_wnd,&ps);

	HW_rect.left=HW_rect.top=0;

	//窗口的大小
	HW_rect.right=HW_SCREEN_X_SIZE;
	HW_rect.bottom=HW_SCREEN_Y_SIZE;

	ShowWindow(HW_wnd,HW_cmd_show);            /* generate messages */
	UpdateWindow(HW_wnd);
}



//////////////////////////////////////////////////////////////////////////////////
/**********************************************************\
 * Rendering the bitmap into the window.                  *
\**********************************************************/

//复制图像位图到物理设备存储器
void HW_blit(void)
{
	PAINTSTRUCT ps;

	BeginPaint(HW_wnd,&ps);

	SelectPalette(ps.hdc,HW_palette,FALSE);
	RealizePalette(ps.hdc);
	SetMapMode(ps.hdc,MM_TEXT);
	SetBitmapBits(HW_bmp,G_c_buffer_size*sizeof(HW_pixel),G_c_buffer);
	BitBlt(ps.hdc,0,0,HW_SCREEN_X_SIZE,HW_SCREEN_Y_SIZE,HW_mem,0,0,SRCCOPY);

	EndPaint(HW_wnd,&ps);
}

void app_main(void)                         /* rendering loop */
{
	//光线跟踪场景中设置摄像机
	TR_set_camera(0,0,500, 0,0,0, 1,0,0, 0,1,0);
	//TR_set_camera(0,0,900, 0,0,0, 1,0,0, 0,1,0);
    //TR_set_camera(0, -1000, 50,  0, -500, 50,  1, 0, 0,  0, 0, 1);

	//关键中的关键，光线跟踪窗口中的pixel
	TR_trace_world(w,10);
	HW_blit();
}

void app_handler(int kk)                    /* event handler */
{
	HW_close_event_loop();
}

void HW_close_screen(void)
{
	DeleteDC(HW_mem);
	DeleteObject(HW_bmp);
}

Scene* PersetWorld()
{
    Scene* w = new Scene;
    w->tr_ambient = Vec3d(0.1, 0.1, 0.1);

    PointLight* l = new PointLight[2];

    l[0].tr_centre = Vec3d(-500, -50, -400);
    l[0].tr_intensity = Vec3d(0.4, 0.4, 0.4);
    l[1].tr_centre = Vec3d(300, -50, -400);
    l[1].tr_intensity = Vec3d(0.5, 0.5, 0.5);
    w->tr_point_lights.push_back(l);
    w->tr_point_lights.push_back(l + 1);

    TPolygon * p = new TPolygon;
    p->tr_vertices.push_back(Vec3d(-300, 130, 300));
    p->tr_vertices.push_back(Vec3d(300, 130, 300));
    p->tr_vertices.push_back(Vec3d(300, 130, 0));
    p->tr_vertices.push_back(Vec3d(-300, 130, 0));
    p->tr_vertices.push_back(Vec3d(-300, 130, 300));
    p->tr_normal = Vec3d(0, 0, 0);
    p->tr_material.tr_ambient = Vec3d(0.6, 0.6, 0.6);
    p->tr_material.tr_diffuse = Vec3d(0.6, 0.6, 0.6);
    p->tr_material.tr_specular = 0.9;
    p->tr_material.tr_exponent = 30;
    p->tr_material.tr_reflect = 0.3;
    w->tr_objects.push_back(p);

    Sphere *s1 = new Sphere;
    s1->tr_radius = 75;
    s1->tr_centre = Vec3d(-100, -70, 500);
    s1->tr_material.tr_ambient = Vec3d(1, 0.5, 0);
    s1->tr_material.tr_diffuse = Vec3d(1, 0.5, 0);
    s1->tr_material.tr_specular = 0.9;
    s1->tr_material.tr_exponent = 30;
    s1->tr_material.tr_reflect = 0.4;
    w->tr_objects.push_back(s1);

    Sphere *s2 = new Sphere;
    s2->tr_radius = 75;
    s2->tr_centre = Vec3d(90, 55, 120);
    s2->tr_material.tr_ambient = Vec3d(1, 0, 0);
    s2->tr_material.tr_diffuse = Vec3d(1, 0, 0);
    s2->tr_material.tr_specular = 0.9;
    s2->tr_material.tr_exponent = 30;
    s2->tr_material.tr_reflect = 0.4;
    w->tr_objects.push_back(s2);

    Sphere *s3 = new Sphere;
    s3->tr_radius = 75;
    s3->tr_centre = Vec3d(-90, 55, 120);
    s3->tr_material.tr_ambient = Vec3d(0, 1, 1);
    s3->tr_material.tr_diffuse = Vec3d(0, 1, 1);
    s3->tr_material.tr_specular = 0.6;
    s3->tr_material.tr_exponent = 30;
    s3->tr_material.tr_reflect = 0.3;
    w->tr_objects.push_back(s3);

    return w;
}

int main(int n, char **o)
{
	char *display;
	if (n == 2) 
		display = o[1];
	else
		display = NULL;

	w = PersetWorld();
	CL_init_colour();
    TR_init_world(w);
	G_init_graphics();
	HW_init_screen(display,"3D RayTracing");
	HW_init_event_loop(app_main,app_handler);
	HW_close_screen();

	return(1);
}

/**********************************************************\
 * Running the event loop.                                *
\**********************************************************/

//事件循环函数，第一个函数指针被不停执行，第二个响应外部时间
void HW_init_event_loop(void (*application_main)(void),
                        void (*application_key_handler)(int key_code)
                       )
{
	MSG msg;

	HW_application_main=application_main;
	HW_application_key_handler=application_key_handler;

	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))  /* this IS sensitive part! */
		{
			if(msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			InvalidateRect(HW_wnd,&HW_rect,TRUE);
			UpdateWindow(HW_wnd);
		}
		Sleep(1);
	}
}


