#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"
#include "timer.h"
#include "minifmod.h"

#define PI 3.14159265358979323846f // pi
#define PID PI/180.0f			// pi ratio
#define CR 1.0f/256.0f		// color ratio

Timer* timer;
float timer_global=0;
float timer_global_previous=0;
float timer_fps=0;
float timer_fps_total=0;
float timer_fps_average=0;
float timer_fps_min=32768;
float timer_fps_max=0;
float timer_music=0;
float timer_max=0;
int frame_counter=0;
int loop_counter=-1;
bool done=false;

FMUSIC_MODULE* mod;			// music handle
int mod_ord=-1;					// pattern order
int mod_row=-1;					// row number
int mod_prv_row=0;			// previous row number
int mod_time=0;					// time
//float mod_tempo=59.625f;// time for one row
bool mod_play=false;		// flag

HDC				hDC=NULL;			// GDI device context
HGLRC			hRC=NULL;			// rendering context
HWND			hWnd=NULL;		// window handle
HINSTANCE	hInstance;		// instance application

int keys[256];					// keyboard array
int active=true;				// window active flag
bool fullscreen=false;	// fullscreen flag
bool pause=false;				// pause flag
float nearplane=0.5f;		// nearplane
float farplane=1000.0f;	// farplane
bool polygon=true;			// polygon mode
int ratio_2d=1;					// 2d ratio
/* fov variable					*/
bool fov_flag=false;		// flag
float fov_base=60;			// base fov angle
float fov=fov_base;			// field of view angle
float fov_angle=0;			// angle
float fov_value=0;			// value
/* window variable			*/
int window_w=800;				// width
int window_h=500;				// height
int screen_w;						// width
int screen_h;						// height
int window_color=32;		// color depth
int window_depth=16;		// depth buffer
/* object variable			*/
float	p_x=0;						// position x
float	p_y=0;						// position y
float	p_z=0;						// position z
float	a_x=0;						// angle x
float	a_y=0;						// angle y
float	a_z=0;						// angle z
float	main_angle;				// main angle
float	main_angle_prv;		// previous main angle
/* color variable				*/
float color_inc=0.025f;	// color incrementation
float base_r=0.4f;			// base r
float base_g=0.4f;			// base g
float base_b=0.2f;			// base b
float bgd_base_r=base_r;// red base value
float bgd_base_g=base_g;// green base value
float bgd_base_b=base_b;// blue base value
float bgd_r=0;					// red value
float bgd_g=0;					// green value
float bgd_b=0;					// blue value
/* fog variable					*/
float fog_color[]={ base_r,base_g,base_b,1.0f };	// fog color definition
/* liner variable				*/
bool liner_flag=false;	// flag
int car;								// ascii code
int liner_length;				// text length
int liner_n;						// line number
int liner_max;					// line max length
int liner_line;					// line increment
int liner_i;						// char increment
int liner_count=0;			// counter
int liner_count_start=0;// counter start
float liner_angle;			// angle
int liner_w;						// width
int liner_h;						// height
float liner_color;			// color increment
int liner_vtx[8];				// vertex array
/* text variable				*/
char* name="Bin - Demos are cool";
char* txt_dos="\r\r\rThe Bin Demo operating system\rVersion 1.0 No 554\r\rDemos are cool\r\r\r\rF1: display debug infos\r\rF2: wireframe rendering\r\rF3: start now\r\r\r\rSPACE: fullscreen/windowed\r\r\rA>_";
char* txt_info1="\r\r\r  Bin - \"Demos are cool\"\r\r   A demo inspired by \r\rRazor 1911 - \"Insert no coin\"\r                 \r                 ";
char* txt_info2="\r\r\r  Text2 \r                 \r                 ";
char* txt_info3="\r\r\r  Text3 \r                 \r                 ";
char* txt_info4="\r\r\r  Text4 \r                 \r                 ";
char* txt_info5="\r\r\r  Text5 \r                 \r                 ";
char* txt_hidden1="\r\r\r   - Credits 1 -   \r\r code:    rez \r code:    bin \r\r      - * -      \r                 \r                 ";
char* txt_hidden2="\r\r\r   - Credits 2 -   \r\r music: chris \r music:   bin \r\r      - * -      \r                 \r                 ";
char* txt_hidden3="\r\r\rThanks goes to:    \r\r keops: timer code \r\r4mat,coda,bubsy \r                   \r                   ";
char* txt_hidden4="\r\r\r  bye \r                 \r                 ";
char* txt=txt_dos;
/* cube variable				*/
bool cube_flag=false;		// flag
int cube_n=16;					// number
float cube_size=1.0f;		// size
float cube_x[256];			// position x
float cube_y[256];			// position y
float cube_z[256];			// position z
float cube_a[256];			// angle
float cube_w=cube_size*0.5f;// width
float cube_h=cube_size*2.0f;// height
float cube_ratio=PID*cube_n;// ratio
float cube_angle=0;			// angle
float cube_vtx[72]={
	// Front face
	-cube_size/2, -cube_size/2,  cube_size/2,    cube_size/2, -cube_size/2,  cube_size/2,    cube_size/2,  cube_size/2,  cube_size/2,   -cube_size/2,  cube_size/2,  cube_size/2,
	// Back face
	-cube_size/2, -cube_size/2, -cube_size/2,   -cube_size/2,  cube_size/2, -cube_size/2,    cube_size/2,  cube_size/2, -cube_size/2,    cube_size/2, -cube_size/2, -cube_size/2,
	// Left face
	-cube_size/2, -cube_size/2, -cube_size/2,   -cube_size/2,  cube_size/2, -cube_size/2,   -cube_size/2,  cube_size/2,  cube_size/2,   -cube_size/2, -cube_size/2,  cube_size/2,
	// Right face
	cube_size/2, -cube_size/2, -cube_size/2,    cube_size/2,  cube_size/2, -cube_size/2,    cube_size/2,  cube_size/2,  cube_size/2,    cube_size/2, -cube_size/2,  cube_size/2,
	// Top face
	-cube_size/2,  cube_size/2,  cube_size/2,    cube_size/2,  cube_size/2,  cube_size/2,    cube_size/2,  cube_size/2, -cube_size/2,   -cube_size/2,  cube_size/2, -cube_size/2,
	// Bottom face
	-cube_size/2, -cube_size/2,  cube_size/2,   -cube_size/2, -cube_size/2, -cube_size/2,    cube_size/2, -cube_size/2, -cube_size/2,    cube_size/2, -cube_size/2,  cube_size/2
};
float cube_tex[40]={
	0.0f, 0.745f, 0.25f, 0.745f, 0.25f, 0.75f, 0.0f, 0.75f,
	0.0f, 0.745f, 0.0f, 0.75f, 0.25f, 0.75f, 0.25f, 0.745f,
	0.25f, 0.735f, 0.0f, 0.735f, 0.0f, 0.74f, 0.25f, 0.74f,
	0.0f, 0.735f, 0.25f, 0.735f, 0.25f, 0.74f, 0.0f, 0.74f,
	0.25f, 0.75f, 0.25f, 1.0f, 0.0f, 1.0f, 0.0f, 0.75f
};
float cube_face_tex[48]={
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //front
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //leftside
};
float cube_white_col[72];         // cube white vertex colors
float cube_blue_col[72];         // cube brown vertex colors
float circuit_tex[16]={ 0.75f,0.75f,0.75f,1.0f,0.625f,1.0f,0.625f,0.75f,0.625f,0.75f,0.625f,1.0f,0.5f,1.0f,0.5f,0.75f };
float circuit_vtx[24]={ cube_w,0,cube_w,cube_w,0,-cube_w,0,0,-cube_w,0,0,cube_w,0,0,cube_w,0,0,-cube_w,-cube_w,0,-cube_w,-cube_w,0,cube_w };
float chipset_vtx[60*9];// chip vertex array
float chipset_tex[40*9];// chip texture array
/* circuit variable			*/
bool circuit_flag=false;// flag
/* logo variable				*/
bool logo_flag=false;		// flag
int logo_w;							// width
int logo_h;							// height
int logo_margin;				// margin
int logo_vtx[8];				// vertex array
float logo_tex[]={ 0.5f,1.0f,0.4375f,1.0f,0.4375f,0.9375f,0.5f,0.9375f };
/* loop variable				*/
int loop_w;							// width
int loop_h;							// height
int loop_margin;				// margin
int loop_vtx[8];				// vertex array
float loop_tex[]={ 0.46484375f,0.76953125f,0.25f,0.76953125f,0.25f,0.75f,0.46484375f,0.75f };
/* glenz variable */
bool glenz_flag=false;
int glenz_frame=0;
int glenz_scale_frame=0;
const int glenz_n=81;
float glenz_pos[glenz_n]=
{
	//frame1
	0,0,2.6, //head
	0,0,0, //body
	1,-1.5,0.5, //back arm inner
	1,-2,0, //back arm outer
	-0.5, -0.5, -2, //crooked leg inner
	-0.5, -1, -2.5, //crooked leg outer
	-1,1.5,0.5, //front arm inner
	-1,2,1, //front arm outer
	0.5, 0.5, -2.5, //straight leg
	//frame2
	0,0,2.6, //head
	0,0,0, //body
	-1,-1.5,0.5, //back arm inner
	-1,-2,0, //back arm outer
	0.5, 0.5, -2, //crooked leg inner
	0.5, 0, -2.5, //crooked leg outer
	1,1.5,0.5, //front arm inner
	1,2,1, //front arm outer
	-0.5, -0.5, -2.5, //straight leg
	//frame3
	0,0,2.6, //head
	0,0,0, //body
	-1,0,0.5, //straight arm
	1,0,0.5, //straight arm
	-0.5, 0, -2.5, //straight leg
	0.5, 0, -2.5, //straight leg
	//0, 0, 0, //empty
	//0, 0, 0, //empty
	//0, 0, 0, //empty
};
float glenz_scale[glenz_n]=
{
	//frame1
	1, 1, 1, //head
	0.7, 1, 1.6, //body
	0.5, 1, 0.5, //right arm inner
	0.5, 0.5, 1, //right arm outer
	0.5, 0.5, 1, //crooked leg inner
	0.5, 1, 0.5, //crooked leg outer
	0.5, 1, 0.5, //left arm inner
	0.5, 0.5, 1, //left arm outer
	0.5, 0.5, 1.5, //straight leg
	//frame2
	1, 1, 1, //head
	0.7, 1, 1.6, //body
	0.5, 1, 0.5, //right arm inner
	0.5, 0.5, 1, //right arm outer
	0.5, 0.5, 1, //crooked leg inner
	0.5, 1, 0.5, //crooked leg outer
	0.5, 1, 0.5, //left arm inner
	0.5, 0.5, 1, //left arm outer
	0.5, 0.5, 1.5, //straight leg
	//frame3
	1, 1, 1, //head
	0.7, 1, 1.6, //body
	0.5, 0.5, 1.5, //straight arm
	0.5, 0.5, 1.5, //straight arm
	0.5, 0.5, 1.5, //straight leg
	0.5, 0.5, 1.5, //straight leg
	//0, 0, 0, //empty
	//0, 0, 0, //empty
	//0, 0, 0, //empty
};
/* intro variable				*/
bool intro_flag=false;	// flag
const int intro_n=36;					// number
int intro_i=1;					// counter
float intro_radius;			// radius
float intro_angle=0;		// angle
/* tunnel variable			*/
bool tunnel_flag=false;	// flag
const int tunnel_n1=64;				// depth number
const int tunnel_n2=16;				// circle number
float tunnel_angle=0;		// angle
float tunnel_x[64];			// position x
float tunnel_y[64];			// position y
float tunnel_z[64];			// position z
float tunnel_depth=0.25f;// depth
float tunnel_radius=1.5f;// radius
float tunnel_path=4.0f;	// path radius
float tunnel_vtx[]={ -0.125f,-0.1875f,0.125f,-0.1875f,0.125f,0.1875f,-0.125f,0.1875f };
float tunnel_tex[]={ 0.25f,0.78125f,0.3125f,0.78125f,0.3125f,0.875f,0.25f,0.875f };
const int star_n=2560;		// total star number
float star_x[star_n];			// position x
float star_y[star_n];			// position y
float star_z[star_n];			// position z
float star_vtx[]={ -0.0375f,-0.0375f,0.0375f,-0.0375f,0.0375f,0.0375f,-0.0375f,0.0375f };
float star_tex[]={ 0.40625f,0.96875f,0.375f,0.96875f,0.375f,1.0f,0.40625f,1.0f };
/* vote variable				*/
bool vote_flag=false;		// flag
int vote_n1=48;					// number x
int vote_n2=48;					// number y
float vote_w=0.5f;			// space between dot
float vote_vtx[]={ -0.325f,-0.325f,0.325f,-0.325f,0.325f,0.325f,-0.325f,0.325f };
/* tekk variable				*/
bool tekk_flag=false;		// flag
int tekk_bar=48;				// bar number
int tekk_n=64;					// polygon per bar
float tekk_w=0.5f;			// space between bar
float tekk_size=0.125f;	// bar size
float tekk_radius=1.5f;	// radius
bool tekk_zoom_flag=false;// zoom flag
float tekk_zoom_angle=0;// zoom angle
float tekk_zoom_value=0;// zoom value
float tekk_vtx[147456];	// vertex array
float tekk_col[147456];	// color array
int youtube_vtx[16];		// vertex array
/* hidden variable			*/
bool hidden=false;			// flag
bool hidden_flag=false;	// flag
float hidden_radius=27.0f;// radius
float hidden_vtx[]={ -0.25f,-0.25f,0.25f,-0.25f,0.25f,0.25f,-0.25f,0.25f };
float hidden_tex[]={ 0.437f,0.812f,0.3755f,0.812f,0.3755f,0.8745f,0.437f,0.8745f };
float gameboy_tex[]={ 0.25f,0.78125f,0.3125f,0.78125f,0.3125f,0.875f,0.25f,0.875f };
/* flash variable				*/
bool flash_flag=false;	// flag
float flash_angle=0;		// angle
/* dos variable					*/
bool dos_flag=true;// flag
bool skip_dos=false;
float dos_time=6.0f;
int dos_w;							// width
int dos_h;							// height
int dos_vtx[8];					// vertex array
int shell_vtx[8];				// vertex array
float shell_tex[]={ 1.0f,0.9453125f,1.0f,1.0f,0.75f,1.0f,0.75f,0.9453125f };
/* debug variable				*/
bool debug_flag=false;// flag
bool debug_test=true;		// test
int debug_w;						// width
int debug_h;						// height
int debug_vtx[8];				// vertex array
/* flash variable		*/
int scanline_vtx[8];		// vertex array
float scanline_tex[8];	// texture array
/* synchro variable			*/
bool synchro_flag=false;// flag
float synchro_angle=0;	// angle
float synchro_value=0;	// value
bool sync2_flag=false;	// flag
float sync2_angle=0;		// angle
float sync2_value=0;		// value
float sync2_mul;				// multiplicator
/* beat variable				*/
bool beat_flag=false;		// flag
float beat_angle=0;			// angle
float beat_value=0;			// value
/* fade variable				*/
bool fade_flag=false;		// flag
float fade_angle=0;			// angle
float fade_value=0;			// value
/* move variable				*/
bool move_flag=false;		// flag
float move_angle=0;			// angle
float move_value=0;			// value
/* speed variable				*/
bool speed_flag=false;	// flag
float speed_angle=0;		// angle
float speed_value=0;		// value

float angle, radius, scale;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// wndProc declaration

static PIXELFORMATDESCRIPTOR pfd=
{
sizeof(PIXELFORMATDESCRIPTOR),
1,											// version number
PFD_DRAW_TO_WINDOW|			// format must support window
PFD_SUPPORT_OPENGL|			// format must support openGL
PFD_DOUBLEBUFFER,				// must support double buffering
PFD_TYPE_RGBA,					// request an RGBA format
window_color,						// select our color depth
0,0,0,0,0,0,						// color bits ignored
0,											// no alpha buffer
0,											// shift bit ignored
0,											// no accumulation buffer
0,0,0,0,								// accumulation bits ignored
window_depth,						// z-buffer (depth buffer)
0,											// no stencil buffer
0,											// no auxiliary buffer
PFD_MAIN_PLANE,					// main drawing layer
0,											// reserved
0,0,0										// layer masks ignored
};

typedef struct
{
	int length, pos;
	void* data;
} MEMFILE;

MEMFILE* memfile;
HRSRC	rec;

unsigned int memopen(char* name)
{
	HGLOBAL	handle;
	memfile=(MEMFILE*)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, sizeof(MEMFILE));
	rec=FindResource(NULL, name, RT_RCDATA);
	handle=LoadResource(NULL, rec);
	memfile->data=LockResource(handle);
	memfile->length=SizeofResource(NULL, rec);
	memfile->pos=0;
	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE* memfile=(MEMFILE*)handle;
	GlobalFree(memfile);
}

int memread(void* buffer, int size, unsigned int handle)
{
	MEMFILE* memfile=(MEMFILE*)handle;
	if (memfile->pos+size>=memfile->length) size=memfile->length-memfile->pos;
	memcpy(buffer, (char*)memfile->data+memfile->pos, size);
	memfile->pos+=size;
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE* memfile=(MEMFILE*)handle;
	if (mode==SEEK_SET) memfile->pos=pos;
	else if (mode==SEEK_CUR) memfile->pos+=pos;
	else if (mode==SEEK_END) memfile->pos=memfile->length+pos;
	if (memfile->pos>memfile->length) memfile->pos=memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE* memfile=(MEMFILE*)handle;
	return memfile->pos;
}

int load_tex(WORD file, GLint clamp, GLint mipmap)
{
	HBITMAP hBMP;	// bitmap handle
	BITMAP BMP;		// bitmap structure
	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(file), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBMP)
	{
		GetObject(hBMP, sizeof(BMP), &BMP);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, file);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BMP.bmWidth, BMP.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mipmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
		DeleteObject(hBMP);
	}
	return 0;
}

void init3d(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluPerspective(fov, (float)((float)width/(float)height), nearplane, farplane); // aspect ratio
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
	// Set camera position and orientation
	gluLookAt(0, -5+cosf(main_angle*0.25f), 2+cosf(main_angle*0.25f)*10.0,                    // Camera position
		0, 0, 0,           // Look-at point
		1, 0, 0);                      // Up vector
}

void init2d(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluOrtho2D(0, width, height, 0);	// init orthographic mode
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
}

void calc_txt()
{
	liner_length=strlen(txt);
	liner_count=0;
	liner_angle=main_angle;
	liner_n=0;
	liner_max=0;
	liner_i=0;
	for (int i=0; i<liner_length; i++)
	{
		if ((byte)txt[i]!=13)
		{
			liner_i++;
		}
		else
		{
			if (liner_i>liner_max) liner_max=liner_i;
			liner_n++;
			liner_i=0;
		}
	}
	if (liner_i>liner_max) liner_max=liner_i;
	fade_value=1.0f;
}

void flash()
{
	flash_flag=true;
	flash_angle=main_angle;
}

void fov_anim()
{
	fov_flag=true;
	fov_angle=main_angle;
}

void synchro()
{
	synchro_flag=true;
	synchro_angle=main_angle;
}

void sync2(float mul)
{
	sync2_flag=true;
	sync2_angle=main_angle;
	sync2_mul=mul;
}

void beat()
{
	beat_flag=true;
	beat_angle=main_angle;
}

void fade()
{
	fade_flag=true;
	fade_angle=main_angle;
}

void move()
{
	move_flag=true;
	move_angle=main_angle;
}

void speed()
{
	speed_flag=true;
	speed_angle=main_angle;
}

void tekk_zoom()
{
	tekk_zoom_flag=true;
	tekk_zoom_angle=main_angle;
	tekk_zoom_value=0;
}

void glenz() {
	int x=0;
	int y=1;
	int z=2;
	for (int i=0; i<glenz_n; i+=3)
	{
		float a=glenz_pos[i+x];
		glenz_pos[i+x]=glenz_pos[i+y];
		glenz_pos[i+y]=a;
		a=glenz_pos[i+x];
		glenz_pos[i+x]=glenz_pos[i+z];
		glenz_pos[i+z]=a;

		glenz_pos[i+x]*=0.5;
		glenz_pos[i+y]*=0.5;
		glenz_pos[i+z]*=0.5;

		a=glenz_scale[i+x];
		glenz_scale[i+x]=glenz_scale[i+y];
		glenz_scale[i+y]=a;
		a=glenz_scale[i+x];
		glenz_scale[i+x]=glenz_scale[i+z];
		glenz_scale[i+z]=a;
	}
}

void pins(int n1, int n2, float x, float y, float z, float a, float b, bool type)
{
	float k=0.015625f;
	a=type ? a : -a;
	b=type ? -b : b;
	float vertex[]={ x+a,0,z+b,x,0,z+b,x,0,z-b,x+a,0,z-b,x+a,-y,z+b,x+a,0,z+b,x+a,0,z-b,x+a,-y,z-b,x+a,-y,z-b,x+a,0,z-b,x+a,0,z+b,x+a,-y,z+b,x+a,-y*3,z+b*0.5f,x+a,-y,z+b*0.5f,x+a,-y,z-b*0.5f,x+a,-y*3,z-b*0.5f,x+a,-y*3,z-b*0.5f,x+a,-y,z-b*0.5f,x+a,-y,z+b*0.5f,x+a,-y*3,z+b*0.5f };
	float texture[]={ 0.3125f,0.875f+k+k,0.3125f,0.875f+k,0.25f,0.875f+k,0.25f,0.875f+k+k,0.375f,0.875f+k+k,0.375f,0.875f+k+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.375f,0.875f+k+k,0.375f,0.875f+k+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k };
	for (int i=0; i<60; i++) chipset_vtx[n1+i]=vertex[i];
	for (int i=0; i<40; i++) chipset_tex[n2+i]=texture[i];
}

void chipset(float x, float y, float z, float a, float b)
{
	float k=0.015625f;
	float vertex[]={ x,y,z,-x,y,z,-x,-y,z,x,-y,z,-x,y,-z,x,y,-z,x,-y,-z,-x,-y,-z,-x,y,z,-x,y,-z,-x,-y,-z,-x,-y,z,x,y,-z,x,y,z,x,-y,z,x,-y,-z,x,y,z,x,y,-z,-x,y,-z,-x,y,z };
	float texture[]={ 0.375f,0.875f+k*3,0.25f,0.875f+k*3,0.25f,0.875f+k*2,0.375f,0.875f+k*2,0.25f,0.875f+k*3,0.375f,0.875f+k*3,0.375f,0.875f+k*2,0.25f,0.875f+k*2,0.375f,0.875f+k*4,0.25f,0.875f+k*4,0.25f,0.875f+k*3,0.375f,0.875f+k*3,0.25f,0.875f+k*4,0.375f,0.875f+k*4,0.375f,0.875f+k*3,0.25f,0.875f+k*3,0.375f,1.0f,0.25f,1.0f,0.25f,0.9375f,0.375f,0.9375f };
	for (int i=0; i<60; i++) chipset_vtx[i]=vertex[i];
	for (int i=0; i<40; i++) chipset_tex[i]=texture[i];
	pins(60, 40, -x, y, -z*0.75f, a, b, false);
	pins(60*2, 40*2, -x, y, -z*0.25f, a, b, false);
	pins(60*3, 40*3, -x, y, z*0.25f, a, b, false);
	pins(60*4, 40*4, -x, y, z*0.75f, a, b, false);
	pins(60*5, 40*5, x, y, -z*0.75f, a, b, true);
	pins(60*6, 40*6, x, y, -z*0.25f, a, b, true);
	pins(60*7, 40*7, x, y, z*0.25f, a, b, true);
	pins(60*8, 40*8, x, y, z*0.75f, a, b, true);
}

void rectangle(int x, int y, int w, int h)
{
	glLoadIdentity();
	glBegin(GL_QUADS);
	glVertex2i(x+w, y);
	glVertex2i(x, y);
	glVertex2i(x, y+h);
	glVertex2i(x+w, y+h);
	glEnd();
}

void cube()
{
	for (int i=0; i<72; i++)
	{
		cube_white_col[i]=1;
	}
	for (int i=0; i<=24; i+=3) {
		cube_blue_col[i]=34.0/255.0;
		cube_blue_col[i+1]=67.0/255.0;
		cube_blue_col[i+2]=182.0/255.0;
	}
}

int InitGL(void)
{
	glClearDepth(1.0f);								// set depth buffer
	glDepthMask(GL_TRUE);							// do not write z-buffer
	glEnable(GL_CULL_FACE);						// disable cull face
	glCullFace(GL_BACK);							// don't draw front face
	glDisable(GL_LIGHTING);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// fog
	glFogi(GL_FOG_MODE, GL_LINEAR);		// fog mode
	glFogfv(GL_FOG_COLOR, fog_color);	// fog color
	glFogf(GL_FOG_DENSITY, 1.0f);			// fog density
	glHint(GL_FOG_HINT, GL_NICEST);		// fog hint value
	glFogf(GL_FOG_START, 2.0f);				// fog start depth
	glFogf(GL_FOG_END, 24.0f);					// fog end depth
	// load texture
	load_tex(IDB_BITMAP1, GL_REPEAT, GL_NEAREST);
	// initialize some variable
	timer=new Timer();
	calc_txt();
	glenz();
	cube();
	chipset(cube_w*0.25f, cube_w*0.0625f, cube_w*0.5f, cube_w*0.1f, cube_w*0.0625f);
	for (int i=0; i<star_n; i++)
	{
		float angle=(rand()%3600)*0.1f;
		radius=((rand()%1000)*0.01f);
		radius=tunnel_radius*1.125f+((radius<0.0f) ? -radius : radius);
		star_x[i]=radius*cosf(angle);
		star_y[i]=radius*sinf(angle);
		star_z[i]=(rand()%(int)(tunnel_depth*tunnel_n1*1000))*0.001f;
		if (angle>180) star_z[i]=-star_z[i];
	}
	float y=0;
	int k=0;
	float x1, x2, y1, y2;
	for (int i=0; i<tekk_bar; i++)
	{
		x1=-tekk_w;
		x2=0;
		y+=tekk_w*2.0f;
		y1=y-tekk_size;
		y2=y+tekk_size;
		for (int j=0; j<tekk_n; j++)
		{
			x1+=tekk_w;
			x2+=tekk_w;
			float angle2=1080.0f*PID/tekk_n*j+main_angle*2.0f;
			tekk_vtx[k]=x1;
			tekk_vtx[k+1]=y2;
			tekk_vtx[k+3]=x1;
			tekk_vtx[k+4]=y1;
			tekk_vtx[k+6]=x2;
			tekk_vtx[k+7]=y1;
			tekk_vtx[k+9]=x2;
			tekk_vtx[k+10]=y2;
			tekk_vtx[k+12]=x1;
			tekk_vtx[k+13]=y1;
			tekk_vtx[k+15]=x1;
			tekk_vtx[k+16]=y2;
			tekk_vtx[k+18]=x2;
			tekk_vtx[k+19]=y2;
			tekk_vtx[k+21]=x2;
			tekk_vtx[k+22]=y1;
			tekk_vtx[k+24]=x1;
			tekk_vtx[k+25]=y1;
			tekk_vtx[k+27]=x1;
			tekk_vtx[k+28]=y1;
			tekk_vtx[k+29]=0;
			tekk_vtx[k+30]=x2;
			tekk_vtx[k+31]=y1;
			tekk_vtx[k+32]=0;
			tekk_vtx[k+33]=x2;
			tekk_vtx[k+34]=y1;
			tekk_vtx[k+36]=x1;
			tekk_vtx[k+37]=y2;
			tekk_vtx[k+38]=0;
			tekk_vtx[k+39]=x1;
			tekk_vtx[k+40]=y2;
			tekk_vtx[k+42]=x2;
			tekk_vtx[k+43]=y2;
			tekk_vtx[k+45]=x2;
			tekk_vtx[k+46]=y2;
			tekk_vtx[k+47]=0;
			float r=0.25f+0.25f*cosf(angle2);
			float g=0.25f;
			float b=0.25f+0.25f*sinf(angle2);
			tekk_col[k]=0.45f;
			tekk_col[k+1]=0.5f;
			tekk_col[k+2]=0.55f;
			tekk_col[k+3]=0.45f;
			tekk_col[k+4]=0.5f;
			tekk_col[k+5]=0.55f;
			tekk_col[k+6]=0.45f;
			tekk_col[k+7]=0.5f;
			tekk_col[k+8]=0.55f;
			tekk_col[k+9]=0.45f;
			tekk_col[k+10]=0.5f;
			tekk_col[k+11]=0.55f;
			tekk_col[k+12]=0.875f;
			tekk_col[k+13]=0;
			tekk_col[k+14]=0;
			tekk_col[k+15]=0.875f;
			tekk_col[k+16]=0;
			tekk_col[k+17]=0;
			tekk_col[k+18]=0.875f;
			tekk_col[k+19]=0;
			tekk_col[k+20]=0;
			tekk_col[k+21]=0.875f;
			tekk_col[k+22]=0;
			tekk_col[k+23]=0;
			tekk_col[k+24]=r;
			tekk_col[k+25]=g;
			tekk_col[k+26]=b;
			tekk_col[k+27]=r;
			tekk_col[k+28]=g;
			tekk_col[k+29]=b;
			tekk_col[k+30]=r;
			tekk_col[k+31]=g;
			tekk_col[k+32]=b;
			tekk_col[k+33]=r;
			tekk_col[k+34]=g;
			tekk_col[k+35]=b;
			tekk_col[k+36]=r;
			tekk_col[k+37]=g;
			tekk_col[k+38]=b;
			tekk_col[k+39]=r;
			tekk_col[k+40]=g;
			tekk_col[k+41]=b;
			tekk_col[k+42]=r;
			tekk_col[k+43]=g;
			tekk_col[k+44]=b;
			tekk_col[k+45]=r;
			tekk_col[k+46]=g;
			tekk_col[k+47]=b;
			k+=48;
		}
	}
	return true;
}

int DrawGLScene(void) // draw scene
{
	frame_counter++;
	// synchro
	timer->update();
	timer_global_previous=timer_global;
	timer_global=timer->elapsed;
	if (!pause)
	{
		// compute rotation
		main_angle_prv=main_angle;
		main_angle=timer_global*100.0f*PID;
	}
	timer_fps=1.0f/(timer_global-timer_global_previous);
	timer_fps_total+=timer_fps;
	timer_fps_average=timer_fps_total/frame_counter;
	if (timer_fps<timer_fps_min) timer_fps_min=timer_fps;
	if (timer_fps>timer_fps_max) timer_fps_max=timer_fps;
	// start music
	if (!mod_play&&(timer_global>dos_time||skip_dos))
	{
		dos_flag=false;
		mod_play=true;
		FMUSIC_PlaySong(mod);
		timer_music=timer_global;
	}
	if (mod_play)
	{
		mod_time=FMUSIC_GetTime(mod);
		mod_prv_row=mod_row;
		mod_row=FMUSIC_GetRow(mod);
		if (mod_row>mod_prv_row+1) mod_row=mod_prv_row;
		if (mod_row!=mod_prv_row)
		{
			if (!hidden)
			{
				if (mod_row==0)
				{
					timer_max=(timer_global-timer_music)*1000.0f;
					timer_music=timer_global;
					mod_ord=FMUSIC_GetOrder(mod);
				}
				if (((mod_ord>7&&mod_ord<16)||(mod_ord>17&&mod_ord<24&&mod_ord!=21))&&(mod_row%16==8||mod_row==52)) synchro();
				if ((mod_ord==16||mod_ord==17)&&(mod_row==52)) synchro();
				if ((mod_ord==21)&&(mod_row==8||mod_row==24||mod_row==56)) synchro();
				if ((mod_ord>27&&mod_ord<32)&&(mod_row%8==0)) sync2(1.75f);
				if ((loop_counter>0)&&(mod_row%16==0)) beat();
				if (mod_ord>3&&mod_row%4==0) glenz_frame=(glenz_frame==0 ? 1 : 0);
				switch (mod_ord)
				{
				case 0:
					switch (mod_row)
					{
					case 0:
						intro_flag=true;
						tunnel_flag=true;
						glenz_flag=true;
						cube_flag=false;
						circuit_flag=false;
						logo_flag=true;
						glenz_frame=2;
						flash();
						intro_i=intro_n;
						break;
					case  3: synchro(); break;
					case  6: intro_i--; synchro(); break;
					case 2:
					case 4:
					case 12:
					case 14:
					case 16:
					case 18:
					case 24:
					case 26:
					case 28:
					case 30:
						intro_i--; break;
					}
					break;
				case 1:
				case 2:
					switch (mod_row)
					{
					case 0:
					case 2:
					case 4:
					case 12:
					case 14:
					case 16:
					case 18:
					case 24:
					case 26:
					case 28:
					case 30:
						intro_i--; break;
					case  3: synchro(); break;
					case  6: intro_i--; synchro(); break;
					}
					break;
				case 3:
					switch (mod_row)
					{
					case  0:  synchro(); break;
					case  6: synchro(); break;
					case 12:  synchro(); break;
					case 18: speed(); break;
					}
					break;
				case 4:
					if (mod_row==0)
					{
						tunnel_flag=false;
						cube_flag=true;
						cube_angle=main_angle;
						circuit_flag=false;
						intro_flag=false;
						move_flag=false;
						liner_flag=true;
						txt=txt_info1;
						speed_flag=false;
						speed_value=1.0f;
						logo_flag=true;
						calc_txt();
						flash();
						fov_anim();
						bgd_base_r=0.125f;
						bgd_base_g=0.15f;
						bgd_base_b=0.1f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 7:
					if (mod_row==54) speed();
					break;
				case 8:
					if (mod_row==0)
					{
						loop_counter++;
						logo_flag=true;
						tekk_flag=false;
						cube_flag=true;
						circuit_flag=true;
						glenz_flag=true;
						intro_radius=3.0f;
						liner_flag=true;
						speed_flag=false;
						txt=txt_info2;
						calc_txt();
						flash();
						if (loop_counter>0) fov_anim();
						bgd_base_r=0;
						bgd_base_g=0.125f;
						bgd_base_b=0.25f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 11:
					if (mod_row==40)
					{
						fade();
						sync2(0.5f);
					}
					break;
				case 12:
					if (mod_row==0)
					{
						logo_flag=true;
						cube_flag=false;
						circuit_flag=false;
						tunnel_flag=true;
						tunnel_angle=main_angle;
						glenz_flag=false;
						liner_flag=true;
						txt=txt_info3;
						calc_txt();
						flash();
						for (int i=0; i<tunnel_n1; i++)
						{
							angle=540.0f*PID;
							float x=tunnel_path*sinf(angle*0.125f)-tunnel_path*cosf(angle*0.375f);
							float y=tunnel_path*sinf(angle*0.25f)-tunnel_path*cosf(angle*0.25f);
							tunnel_x[i]=x;
							tunnel_y[i]=y;
							tunnel_z[i]=-tunnel_depth*i;
						}
						bgd_base_r=0.0f;
						bgd_base_g=0.05f;
						bgd_base_b=0.1f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 15:
					switch (mod_row)
					{
					case 40: fade(); break;
					case 48: speed(); break;
					}
					break;
				case 16:
					if (mod_row==0)
					{
						logo_flag=true;
						tunnel_flag=false;
						glenz_flag=true;
						intro_radius=1.0f;
						liner_flag=true;
						speed_flag=false;
						txt=txt_info4;
						calc_txt();
						flash();
						bgd_base_r=0.25f;
						bgd_base_g=0.25f;
						bgd_base_b=0.375f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 19:
					if (mod_row==40) fade();
					break;
				case 20:
					if (mod_row==0)
					{
						txt=txt_info5;
						logo_flag=true;
						vote_flag=true;
						glenz_flag=false;
						liner_flag=true;
						calc_txt();
						flash();
						sync2(0.5f);
						bgd_base_r=0.25f;
						bgd_base_g=0.125f;
						bgd_base_b=0.25f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 23:
					if (mod_row==40) fade();
					break;
				case 24:
					if (mod_row==0)
					{
						logo_flag=true;
						vote_flag=false;
						glenz_flag=true;
						intro_radius=1.0f;
						flash();
						fov_anim();
						bgd_base_r=0.3f;
						bgd_base_g=0.25f;
						bgd_base_b=0.2f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 26:
					switch (mod_row)
					{
					case 0: case 6: case 12: case 32: case 38: case 44: synchro(); break;
					}
					break;
				case 27:
					switch (mod_row)
					{
					case  0: case 6: case 12: case 38: case 44: synchro(); break;
					case 32: synchro(); speed(); break;
					case 40: fade(); break;
					}
					break;
				case 28:
					if (mod_row==0)
					{
						logo_flag=true;
						glenz_flag=false;
						tekk_flag=true;
						liner_flag=false;
						calc_txt();
						flash();
						bgd_base_r=0.25f;
						bgd_base_g=0.2f;
						bgd_base_b=0.0f;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					break;
				case 30:
					if (mod_row==32) tekk_zoom();
					break;
				case 31:
					switch (mod_row)
					{
					case 32: case 48: case 56: case 60: case 62: synchro(); break;
					}
					break;
				}
			}
			else
			{
				if (mod_row==0)
				{
					timer_max=(timer_global-timer_music)*1000.0f;
					timer_music=timer_global;
					int last_ord=mod_ord;
					mod_ord=FMUSIC_GetOrder(mod);
					if (mod_ord==0)
					{
						logo_flag=true;
						liner_flag=true;
						hidden_flag=true;
						flash();
						bgd_base_r=0;
						bgd_base_g=0;
						bgd_base_b=0;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					if (last_ord!=mod_ord) {
						switch (mod_ord)
						{
						case 0: txt=txt_hidden1; calc_txt(); break;
						case 2: txt=txt_hidden2; calc_txt(); break;
						case 3: txt=txt_hidden3; calc_txt(); break;
						case 5: txt=txt_hidden4; calc_txt(); break;
						}
					}
				}
				if (mod_row%8==4) synchro();
			}
		}
	}
	if (synchro_flag)
	{
		angle=(main_angle-synchro_angle)*2.0f;
		synchro_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			synchro_value=0;
			synchro_flag=false;
		}
	}
	if (sync2_flag)
	{
		angle=(main_angle-sync2_angle)*sync2_mul;
		sync2_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			sync2_value=0;
			sync2_flag=false;
		}
	}
	if (beat_flag)
	{
		angle=(main_angle-beat_angle)*1.5f;
		beat_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			beat_value=0;
			beat_flag=false;
		}
	}
	if (fade_flag)
	{
		angle=(main_angle-fade_angle)*1.0f;
		fade_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			fade_value=0;
			fade_flag=false;
		}
	}
	if (move_flag)
	{
		angle=(main_angle-move_angle)*2.0f;
		move_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			move_value=0;
		}
	}
	if (speed_flag)
	{
		angle=(main_angle-speed_angle)*0.45f;
		speed_value=cosf(angle);
		if (angle>90.0f*PID)
		{
			speed_value=0;
			speed_flag=false;
		}
	}
	// clear screen and depth buffer
	fov=fov_base+15.0f*sinf((main_angle-fov_angle)*0.25f);
	if (fov_flag)
	{
		angle=(main_angle-fov_angle)*0.25f;
		fov_value=sinf(angle);
		fov+=fov_value*fov_base-fov_base;
		if (angle>90.0f*PID) fov_flag=false;
	}
	init3d(screen_w, screen_h);
	glClearColor(bgd_base_r, bgd_base_g, bgd_base_b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	// draw cube
	if (cube_flag)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glDisable(GL_BLEND);
		angle=cosf((cube_angle-main_angle)*0.125f);
		radius=cube_size-cube_size*angle;
		p_x=0;
		p_y=3.0f-2.0f*angle;
		p_z=-14.0f+2.0f*angle;
		a_x=60.0f+10.0f*angle;
		a_y=-60.0f+(cube_angle-main_angle)*8.0f+speed_value*180.0f;
		a_z=0;
		float h=(float)(cube_size*0.1f+(circuit_flag ? 0.2f-fabs(synchro_value*0.2f*cosf((main_angle-synchro_angle)*8.0f)) : 0));
		int k=0;
		for (int i=0; i<cube_n; i++)
		{
			float a=i*cube_ratio;
			float x=-(cube_n-1)*cube_size*0.5f+i*cube_size;
			for (int j=0; j<cube_n; j++)
			{
				cube_x[k]=x;
				float b=j*cube_ratio;
				cube_a[k]=((cube_angle-main_angle)*0.5f+a+b);
				cube_y[k]=cube_size+radius*cosf(cube_a[k]);
				cube_z[k]=-(cube_n-1)*cube_size*0.5f+j*cube_size;
				k++;
			}
		}
		glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, cube_tex);
		for (int i=0; i<k; i++)
		{
			glLoadIdentity();
			glTranslatef(p_x, p_y, p_z);
			glRotatef(a_x, 1.0f, 0, 0);
			glRotatef(a_y, 0, 1.0f, 0);
			glTranslatef(cube_x[i], cube_y[i], cube_z[i]);
			glColorPointer(3, GL_FLOAT, 0, cube_white_col);
			glDrawArrays(GL_QUADS, 0, 20);
		}
		// draw circuit
		if (circuit_flag)
		{
			glVertexPointer(3, GL_FLOAT, 0, circuit_vtx);
			glTexCoordPointer(2, GL_FLOAT, 0, circuit_tex);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_DST_ALPHA);
			for (int i=0; i<k; i++)
			{
				glLoadIdentity();
				glTranslatef(p_x, p_y, p_z);
				glRotatef(a_x, 1.0f, 0, 0);
				glRotatef(a_y, 0, 1.0f, 0);
				glTranslatef(cube_x[i], cube_y[i]+cube_size*0.75f+0.01f, cube_z[i]);
				float c1=0.5f+0.5f*cosf(cube_a[i]*4.0f);
				float c2=0.5f+0.5f*sinf(cube_a[i]*4.0f);
				float circuit_col[]={ c1,0,c2,c2,c1,0,c1,-c2,-c2,c1,-c2,-c2,c1,-c2,-c2,c1,-c2,-c2,0,c2,c1,c1,0,c2 };
				glColorPointer(3, GL_FLOAT, 0, circuit_col);
				glDrawArrays(GL_QUADS, 0, 8);
			}
			glDisable(GL_BLEND);
		}
		glDisableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, chipset_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, chipset_tex);
		glColor3f(1.0f, 1.0f, 1.0f);
		for (int i=0; i<k; i++)
		{
			glLoadIdentity();
			glTranslatef(p_x, p_y, p_z);
			glRotatef(a_x, 1.0f, 0, 0);
			glRotatef(a_y, 0, 1.0f, 0);
			glTranslatef(cube_x[i], cube_y[i]+cube_size*0.75f+0.01f+h, cube_z[i]);
			glDrawArrays(GL_QUADS, 0, 180);
		}
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_BLEND);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if (intro_flag||glenz_flag||tunnel_flag||vote_flag||tekk_flag||hidden_flag)
	{
		fov=fov_base+((!vote_flag) ? 30.0f : 10.0f);
		init3d(screen_w, screen_h);
	}
	// draw intro
	if (intro_flag)
	{

	}
	glDisable(GL_DEPTH_TEST);
	// draw tunnel
	if (tunnel_flag)
	{
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		/*glVertexPointer(2, GL_FLOAT, 0, tunnel_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, tunnel_tex);
		angle=(main_angle-tunnel_angle)+540.0f*PID;
		float x=tunnel_path*sinf(angle*0.125f)-tunnel_path*cosf(angle*0.375f);
		float y=tunnel_path*sinf(angle*0.25f)-tunnel_path*cosf(angle*0.25f);
		angle-=590.0f*PID;
		radius=synchro_value*0.1f*cosf((main_angle-synchro_angle)*16.0f);
		p_x=-(tunnel_path*sinf(angle*0.125f)-tunnel_path*cosf(angle*0.375f))-(speed_flag ? (1.0f-speed_value)*tunnel_path*5.0f : 0.0f);
		p_y=-(tunnel_path*sinf(angle*0.25f)-tunnel_path*cosf(angle*0.25f)+radius);
		a_z=p_x*10.0f;
		for (int i=0; i<tunnel_n1; i++)
		{
			tunnel_z[i]+=(main_angle-main_angle_prv)*1.5f;
			if (tunnel_z[i]>0.0f)
			{
				tunnel_x[i]=x;
				tunnel_y[i]=y;
				tunnel_z[i]-=tunnel_n1*tunnel_depth;
			}
			angle=360.0f*PID/tunnel_n2;
			float angle2=720.0f/tunnel_n1*i;
			for (int j=0; j<tunnel_n2; j++)
			{
				float c=(tunnel_z[i]<-1.0f) ? 1.0f : -tunnel_z[i];
				glPushMatrix();
				glColor3f((0.5f+0.5f*cosf(angle*j))*c, 0.5f*c, (0.5f+0.5f*sinf(angle*j))*c);
				glRotatef(a_z, 0, 0, 1.0f);
				glTranslatef(p_x+tunnel_x[i]+tunnel_radius*cosf(angle*j+angle2*PID), p_y+tunnel_y[i]+tunnel_radius*sinf(angle*j+angle2*PID), tunnel_z[i]);
				glRotatef(360.0f/tunnel_n2*j+angle2, 0, 0, 1.0f);
				glDrawArrays(GL_QUADS, 0, 4);
				glPopMatrix();
			}
		}*/
		// draw star
		glVertexPointer(2, GL_FLOAT, 0, star_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, star_tex);
		glColor3f(1.0f, 1.0f, 1.0f);
		int stars_to_render=star_n*((float)intro_i/(float)intro_n);
		for (int i=0; i<stars_to_render; i++)
		{
			glPushMatrix();
			glTranslatef(star_x[i], star_y[i], star_z[i]);
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
			modelview[0]=1.0f;
			modelview[1]=0.0f;
			modelview[2]=0.0f;
			modelview[4]=0.0f;
			modelview[5]=1.0f;
			modelview[6]=0.0f;
			modelview[8]=0.0f;
			modelview[9]=0.0f;
			modelview[10]=1.0f;
			glLoadMatrixf(modelview);
			glRotatef(a_z, 0, 0, 1.0f);
			glDrawArrays(GL_QUADS, 0, 4);
			glPopMatrix();
		}
	}
	glEnable(GL_BLEND);
	// draw vote
	glEnable(GL_TEXTURE_2D);
	if (vote_flag)
	{
		glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA);
		float w=0.325f;
		float h=0.325f;
		angle=sync2_value*cosf((main_angle-sync2_angle)*1.25f);
		a_x=50.0f;
		a_y=-main_angle*16.0f-120.0f*angle;
		p_x=-vote_n1*vote_w*0.25f;
		p_y=-32.0f*angle;
		p_z=-16.0f;
		float z=-vote_n2*vote_w*0.5f;
		radius=1.0f;
		glVertexPointer(2, GL_FLOAT, 0, vote_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, star_tex);
		for (int i=0; i<vote_n1; i++)
		{
			float x=p_x+i*vote_w;
			angle=720.0f*PID/vote_n1*i+cosf(i*0.375f)+main_angle*1.625f;
			for (int j=0; j<vote_n2; j++)
			{
				float y=-0.5f-radius*1.5f*cosf(main_angle*0.25f)+sinf((i+j)*0.25f)+radius*cosf(angle)+radius*sinf(720.0f*1.5f*PID/vote_n2*j+main_angle);
				glColor3f(0.5f+0.5f*cosf(90.0f*PID*y), 1.0f, 0.5f+0.5f*sinf(90.0f*PID*y));
				if (y<-2.0f&&y>-2.5f) glColor3f(0.625f, 1.0f, 0.75f);
				if (y<-1.0f) y=-y-1.35f;
				if (y<-0.625f) y=-0.5f;
				glPushMatrix();
				glTranslatef(p_y, 0, p_z);
				glRotatef(a_x, 1.0f, 0, 0);
				glRotatef(a_y, 0, 1.0f, 0);
				glTranslatef(p_x+x, y, z+j*vote_w);
				glRotatef(-a_y, 0, 1.0f, 0);
				glRotatef(-a_x, 1.0f, 0, 0);
				glDrawArrays(GL_QUADS, 0, 4);
				glPopMatrix();
			}
		}
	}
	// draw tekk
	if (tekk_flag)
	{
		glDisable(GL_TEXTURE_2D);
		if (tekk_zoom_flag)
		{
			angle=(main_angle-tekk_zoom_angle)*0.15f;
			tekk_zoom_value=1.0f-cosf(angle);
			if (angle>90.0f*PID)
			{
				tekk_zoom_value=0;
				tekk_zoom_flag=false;
			}
			bgd_base_r=0.25f+tekk_zoom_value*0.25f;
			bgd_base_g=0.2f-tekk_zoom_value*0.125f;
			bgd_base_b=0.0f+tekk_zoom_value*0.125f;
			fog_color[0]=bgd_base_r;
			fog_color[1]=bgd_base_g;
			fog_color[2]=bgd_base_b;
			glFogfv(GL_FOG_COLOR, fog_color);
		}
		p_x=-2.0f;
		p_y=-tekk_bar*tekk_w+0.5f*cosf(main_angle*0.5f);
		p_z=-7.0f;
		a_x=-36.0f;
		a_y=90.0f;
		a_z=0;
		float y=0;
		int k=0;
		float z1, z2=0;
		float a1=main_angle*0.5f+tekk_zoom_value*4.0f;
		for (int i=0; i<tekk_bar; i++)
		{
			angle=tekk_radius*cosf(1080.0f*PID/tekk_bar*i+cosf(i*1.5f)+a1);
			float a2=main_angle*2.0f+tekk_zoom_value*24.0f;
			for (int j=0; j<tekk_n; j++)
			{
				float angle2=1080.0f*PID/tekk_n*j+a2;
				float z=sync2_value*2.0f*(rand()%100)/200.0f*cosf((main_angle-sync2_angle)*16.0f);
				z1=z2;
				z2=(float)fabs(angle+tekk_radius*sinf(angle2))+z;
				if (z2<0.5f) z2=0.5f;
				tekk_vtx[k+2]=z1;
				tekk_vtx[k+5]=z1;
				tekk_vtx[k+8]=z2;
				tekk_vtx[k+11]=z2;
				tekk_vtx[k+14]=z1;
				tekk_vtx[k+17]=z1;
				tekk_vtx[k+20]=z2;
				tekk_vtx[k+23]=z2;
				tekk_vtx[k+26]=z1;
				tekk_vtx[k+35]=z2;
				tekk_vtx[k+41]=z1;
				tekk_vtx[k+44]=z2;
				k+=48;
			}
		}
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, tekk_vtx);
		glColorPointer(3, GL_FLOAT, 0, tekk_col);
		glBlendFunc(GL_SRC_COLOR, GL_DST_ALPHA);
		glLoadIdentity();
		glRotatef(a_x, 1.0f, 0, 0);
		glRotatef(a_z, 0, 1.0f, 0);
		glRotatef(a_y, 0, 0, 1.0f);
		glTranslatef(p_x, p_y, p_z);
		glDrawArrays(GL_QUADS, 0, tekk_bar*tekk_n*16);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
	}
	glDisable(GL_FOG);
	// draw glenz
	if (glenz_flag)
	{
		int frame=27*glenz_frame;
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glPushMatrix();
		glTranslatef(glenz_pos[0+frame], glenz_pos[1+frame], glenz_pos[2+frame]);
		glScalef(glenz_scale[0+frame], glenz_scale[1+frame], glenz_scale[2+frame]);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, cube_face_tex);
		glColorPointer(3, GL_FLOAT, 0, cube_white_col);
		glDrawArrays(GL_QUADS, 0, 24);
		glDisableClientState(GL_COLOR_ARRAY);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		int endFrame=27*(glenz_frame+1);
		for (int i=frame+3; i<endFrame; i+=3) {
			glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
			glPushMatrix();
			glTranslatef(glenz_pos[i], glenz_pos[i+1], glenz_pos[i+2]);
			glScalef(glenz_scale[i], glenz_scale[i+1], glenz_scale[i+2]);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
			glColorPointer(3, GL_FLOAT, 0, cube_blue_col);
			glDrawArrays(GL_QUADS, 0, 24);
			glDisableClientState(GL_COLOR_ARRAY);
			glPopMatrix();
		}
		glEnable(GL_TEXTURE_2D);
	}
	init2d(screen_w, screen_h);
	// draw liner
	if (liner_flag)
	{
		glVertexPointer(2, GL_INT, 0, liner_vtx);
		glBlendFunc(GL_ONE, GL_ONE);
		int j=0;
		liner_line=-1;
		liner_count=(int)((main_angle-liner_angle)*20.0f)-5;
		if (liner_count>liner_length) liner_count=liner_length;
		float x=0;
		float y=0;
		for (int i=0; i<liner_count; i++)
		{
			j++;
			car=(byte)txt[i];
			if (car>32)
			{
				liner_color=(liner_count-i)*(0.05f); if (liner_color>1.0f) liner_color=1.0f;
				angle=main_angle*2.0f+j*PID*12.0f;
				radius=1.0f+(1.0f-liner_color)*6.0f;
				glLoadIdentity();
				glColor3f(liner_color*fade_value, liner_color*fade_value, liner_color*fade_value);
				glRotatef(6.0f*cosf(angle)*(1.0f-liner_color), 0, 0, 1.0f);
				glTranslatef(x+j*liner_w*2.0f+liner_w*1.5f*cosf(angle), y+liner_h*2.0f*cosf(angle), 0);
				glRotatef(-12.5f*sinf(angle), 0, 0, 1.0f);
				glScalef(radius, radius, 0);
				float l_w=(car%16)*0.03125f;
				float l_h=(car-car%16)*0.001953125f;
				float liner_tex[]={ l_w,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h-0.00001f,l_w,0.75f-l_h-0.00001f };
				glTexCoordPointer(2, GL_FLOAT, 0, liner_tex);
				glDrawArrays(GL_QUADS, 0, 4);
			}
			if (car==13)
			{
				j=0;
				liner_line++;
				x=screen_w*0.5f-liner_max*liner_w;
				y=screen_h*0.5f-(liner_n-1)*liner_h*1.25f+liner_line*liner_h*2.5f;
			}
		}
	}
	// draw dos
	if (dos_flag)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor3f(0, 0, 0);
		rectangle(0, 0, screen_w, screen_h);
		glColor3f(1.0f, 1.0f, 1.0f);
		rectangle(0, 0, screen_w-(int)(26.5f*ratio_2d), 10*ratio_2d);
		rectangle(screen_w-(int)(25.5f*ratio_2d), 0, 11*ratio_2d, 10*ratio_2d);
		rectangle(screen_w-(int)(13.5f*ratio_2d), 0, 11*ratio_2d, 10*ratio_2d);
		rectangle(screen_w-(int)(1.5f*ratio_2d), 0, (int)(1.5f*ratio_2d), 10*ratio_2d);
		rectangle(0, 10*ratio_2d, 1*ratio_2d, screen_h-10*ratio_2d);
		rectangle(screen_w-1*ratio_2d, 10*ratio_2d, 1*ratio_2d, screen_h-10*ratio_2d);
		rectangle(1*ratio_2d, screen_h-1*ratio_2d, screen_w-2*ratio_2d, 1*ratio_2d);
		glColor3f(0, 0, 0);
		rectangle((int)(34.5f*ratio_2d), 2*ratio_2d, screen_w-(int)(62.5f*ratio_2d), 2*ratio_2d);
		rectangle((int)(34.5f*ratio_2d), 6*ratio_2d, screen_w-(int)(62.5f*ratio_2d), 2*ratio_2d);
		rectangle(screen_w-(int)(24.5f*ratio_2d), 1*ratio_2d, 7*ratio_2d, 6*ratio_2d);
		glColor3f(1.0f, 1.0f, 1.0f);
		rectangle(screen_w-(int)(23.5f*ratio_2d), 2*ratio_2d, 5*ratio_2d, 4*ratio_2d);
		glColor3f(0, 0, 0);
		rectangle(screen_w-(int)(22.5f*ratio_2d), 3*ratio_2d, 7*ratio_2d, 6*ratio_2d);
		rectangle(screen_w-(int)(12.5f*ratio_2d), 1*ratio_2d, 7*ratio_2d, 6*ratio_2d);
		glColor3f(0, 0, 0);
		rectangle(screen_w-(int)(10.5f*ratio_2d), 2*ratio_2d, 7*ratio_2d, 7*ratio_2d);
		glColor3f(1.0f, 1.0f, 1.0f);
		rectangle(screen_w-(int)(9.5f*ratio_2d), 3*ratio_2d, 5*ratio_2d, 5*ratio_2d);
		rectangle(screen_w-8*ratio_2d, screen_h-9*ratio_2d, 7*ratio_2d, 8*ratio_2d);
		glColor3f(0, 0, 0);
		rectangle(screen_w-7*ratio_2d, screen_h-8*ratio_2d, 6*ratio_2d, 7*ratio_2d);
		glColor3f(1.0f, 1.0f, 1.0f);
		rectangle(12*ratio_2d, screen_h-78*ratio_2d, 60*ratio_2d*timer_global, 7*ratio_2d);
		rectangle(screen_w-4*ratio_2d, screen_h-8*ratio_2d, 4*ratio_2d, 2*ratio_2d);
		rectangle(screen_w-7*ratio_2d, screen_h-5*ratio_2d, 2*ratio_2d, 5*ratio_2d);
		rectangle(screen_w-6*ratio_2d, screen_h-7*ratio_2d, 1*ratio_2d, 1*ratio_2d);
		rectangle(screen_w-4*ratio_2d, screen_h-5*ratio_2d, 2*ratio_2d, 3*ratio_2d);
		glEnable(GL_TEXTURE_2D);
		glVertexPointer(2, GL_INT, 0, shell_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, shell_tex);
		glDrawArrays(GL_QUADS, 0, 4);
		// text
		glVertexPointer(2, GL_INT, 0, dos_vtx);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		int j=0;
		liner_line=-1;
		liner_count=liner_length;
		float x=0;
		float y=0;
		for (int i=0; i<liner_count; i++)
		{
			j++;
			car=(byte)txt[i];
			if (car>32)
			{
				glLoadIdentity();
				glTranslated(x+j*(dos_w+2*ratio_2d), y, 0);
				if (car>32)
				{
					float l_w=(car%16)*0.03125f;
					float l_h=(car-car%16)*0.001953125f;
					float dos_tex[]={ l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h,l_w,0.75f-l_h,l_w,0.75f-l_h-0.03125f };
					glTexCoordPointer(2, GL_FLOAT, 0, dos_tex);
					glDrawArrays(GL_QUADS, 0, 4);
				}
			}
			if (car==13)
			{
				j=0;
				liner_line++;
				x=(float)(2*ratio_2d);
				y=(float)(15*ratio_2d+liner_line*(dos_h+4*ratio_2d));
			}
		}
	}
	// draw flash
	if (flash_flag)
	{
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		angle=(main_angle-flash_angle)*1.0f;
		if (angle>90.0f*PID) flash_flag=false;
		float c=sinf(angle);
		glColor3f(c, c, c);
		glLoadIdentity();
		glVertexPointer(2, GL_INT, 0, scanline_vtx);
		glDrawArrays(GL_QUADS, 0, 4);
		glEnable(GL_TEXTURE_2D);
	}
	if (speed_flag)
	{
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		float c=speed_value;
		glColor3f(c, c, c);
		glLoadIdentity();
		glVertexPointer(2, GL_INT, 0, scanline_vtx);
		glDrawArrays(GL_QUADS, 0, 4);
		glEnable(GL_TEXTURE_2D);
	}
	// draw logo
	if (logo_flag)
	{
		glLoadIdentity();
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glTranslatef((float)(screen_w-logo_w-logo_margin), (float)(screen_h-logo_h-logo_margin-fabs(ratio_2d*4*synchro_value*cosf((main_angle-synchro_angle)*10.0f))), 0);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertexPointer(2, GL_INT, 0, logo_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, logo_tex);
		glDrawArrays(GL_QUADS, 0, 4);
	}
	// draw loop
	if (loop_counter>0)
	{
		float c=beat_value;
		glLoadIdentity();
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glTranslated(loop_margin, screen_h-loop_h-loop_margin, 0);
		glColor3f(0.25f+c*0.5f, 0.25f+c*0.125f, 0.25f-c*0.25f);
		glVertexPointer(2, GL_INT, 0, loop_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, loop_tex);
		glDrawArrays(GL_QUADS, 0, 4);
	}
	// draw debug
	if (debug_flag)
	{
		char debug[512];
		sprintf(debug, "--+ DEBUG +--\nscreen=%dx%d\n2d ratio=%dx\nfps=%3.1f\naverage=%3.1f\nfps min=%3.1f\nfps max=%3.1f\nmusic=%02d-%02d\ntimer=%3.3fs\npattern=%1.3fs\nloop=%dx", screen_w, screen_h, ratio_2d, timer_fps, timer_fps_average, timer_fps_min, timer_fps_max, mod_ord, mod_row, mod_time*0.001f, timer_max*0.001f, loop_counter);
		glVertexPointer(2, GL_INT, 0, debug_vtx);
		glBlendFunc(GL_ONE, GL_ONE);
		glLoadIdentity();
		glColor3f(0.25f, 0.25f, 0.25f);
		glTranslated(-debug_w+2*ratio_2d, debug_h+2*ratio_2d, 0);
		int j=0;
		for (int i=0; i<(int)strlen(debug); i++)
		{
			j++;
			car=(byte)debug[i];
			if (car==10)
			{
				glTranslated(-j*debug_w*2, debug_h*2, 0);
				j=0;
			}
			glTranslated(debug_w*2, 0, 0);
			if (car>32)
			{
				float l_w=(car%16)*0.03125f;
				float l_h=(car-car%16)*0.001953125f;
				float debug_tex[]={ l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h,l_w,0.75f-l_h,l_w,0.75f-l_h-0.03125f };
				glTexCoordPointer(2, GL_FLOAT, 0, debug_tex);
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}
	}
	return true;
}

void KillGLWindow(void)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);	// switch back to desktop
		ShowCursor(false);							// show mouse pointer
	}
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL)) MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK|MB_ICONINFORMATION);
		if (!wglDeleteContext(hRC)) MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK|MB_ICONINFORMATION);
		hRC=NULL;
	}
	if (hDC&&!ReleaseDC(hWnd, hDC)) { MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK|MB_ICONINFORMATION); hDC=NULL; }
	if (hWnd&&!DestroyWindow(hWnd)) { MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK|MB_ICONINFORMATION); hWnd=NULL; }
	if (!UnregisterClass("razor1911", hInstance)) { MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK|MB_ICONINFORMATION); hInstance=NULL; }
	delete timer;
}

int CreateGLWindow(char* title)
{
	GLuint PixelFormat;												// pixel format result
	WNDCLASS wc;															// windows class structure
	DWORD dwExStyle;													// window extended style
	DWORD dwStyle;														// window style
	RECT WindowRect;													// upper_left/lower_right values
	int w=GetSystemMetrics(SM_CXSCREEN);
	int h=GetSystemMetrics(SM_CYSCREEN);
	screen_w=fullscreen ? w : window_w;
	screen_h=fullscreen ? h : window_h;
	timer_fps_min=32768;
	timer_fps_max=0;
	ratio_2d=(int)(screen_w/400);
	logo_w=16*ratio_2d;
	logo_h=16*ratio_2d;
	logo_margin=2*ratio_2d;
	logo_vtx[0]=logo_w;
	logo_vtx[1]=0;
	logo_vtx[2]=0;
	logo_vtx[3]=0;
	logo_vtx[4]=0;
	logo_vtx[5]=logo_h;
	logo_vtx[6]=logo_w;
	logo_vtx[7]=logo_h;
	loop_w=110*ratio_2d;
	loop_h=10*ratio_2d;
	loop_margin=3*ratio_2d;
	loop_vtx[0]=loop_w;
	loop_vtx[1]=0;
	loop_vtx[2]=0;
	loop_vtx[3]=0;
	loop_vtx[4]=0;
	loop_vtx[5]=loop_h;
	loop_vtx[6]=loop_w;
	loop_vtx[7]=loop_h;
	liner_w=4*ratio_2d;
	liner_h=4*ratio_2d;
	liner_vtx[0]=-liner_w;
	liner_vtx[1]=(int)(liner_h*1.5f);
	liner_vtx[2]=liner_w;
	liner_vtx[3]=liner_h;
	liner_vtx[4]=liner_w;
	liner_vtx[5]=(int)(-liner_h*1.5f);
	liner_vtx[6]=-liner_w;
	liner_vtx[7]=-liner_h;
	youtube_vtx[0]=-liner_w;
	youtube_vtx[1]=(int)(liner_h*1.5f);
	youtube_vtx[2]=0;
	youtube_vtx[3]=(int)(liner_h*1.25f);
	youtube_vtx[4]=0;
	youtube_vtx[5]=-liner_h;
	youtube_vtx[6]=-liner_w;
	youtube_vtx[7]=-liner_h;
	youtube_vtx[8]=0;
	youtube_vtx[9]=(int)(liner_h*1.25f);
	youtube_vtx[10]=liner_w;
	youtube_vtx[11]=liner_h;
	youtube_vtx[12]=liner_w;
	youtube_vtx[13]=-liner_h;
	youtube_vtx[14]=0;
	youtube_vtx[15]=-liner_h;
	dos_w=2*ratio_2d;
	dos_h=4*ratio_2d;
	dos_vtx[0]=dos_w;
	dos_vtx[1]=dos_h;
	dos_vtx[2]=dos_w;
	dos_vtx[3]=-dos_h;
	dos_vtx[4]=-dos_w;
	dos_vtx[5]=-dos_h;
	dos_vtx[6]=-dos_w;
	dos_vtx[7]=dos_h;
	shell_vtx[0]=33*ratio_2d;
	shell_vtx[1]=8*ratio_2d;
	shell_vtx[2]=33*ratio_2d;
	shell_vtx[3]=1*ratio_2d;
	shell_vtx[4]=1*ratio_2d;
	shell_vtx[5]=1*ratio_2d;
	shell_vtx[6]=1*ratio_2d;
	shell_vtx[7]=8*ratio_2d;
	debug_w=2*ratio_2d;
	debug_h=4*ratio_2d;
	debug_vtx[0]=debug_w;
	debug_vtx[1]=debug_h;
	debug_vtx[2]=debug_w;
	debug_vtx[3]=-debug_h;
	debug_vtx[4]=-debug_w;
	debug_vtx[5]=-debug_h;
	debug_vtx[6]=-debug_w;
	debug_vtx[7]=debug_h;
	scanline_tex[0]=0;
	scanline_tex[1]=0.7325f;
	scanline_tex[2]=0;
	scanline_tex[3]=0.7275f;
	scanline_tex[4]=screen_h/256.0f;
	scanline_tex[5]=0.7275f;
	scanline_tex[6]=screen_h/256.0f;
	scanline_tex[7]=0.7325f;
	scanline_vtx[0]=screen_w;
	scanline_vtx[1]=0;
	scanline_vtx[2]=0;
	scanline_vtx[3]=0;
	scanline_vtx[4]=0;
	scanline_vtx[5]=screen_h;
	scanline_vtx[6]=screen_w;
	scanline_vtx[7]=screen_h;
	main_angle_prv=0;
	WindowRect.left=(long)(fullscreen ? 0 : 2);		// set left value
	WindowRect.right=(long)screen_w;					// set right value
	WindowRect.top=(long)(fullscreen ? 0 : 2);		// set top value
	WindowRect.bottom=(long)screen_h;					// set bottom value
	pfd.cColorBits=window_color;							// set color depth
	hInstance=GetModuleHandle(NULL);					// window instance
	wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;	// redraw on size, own DC for window
	wc.lpfnWndProc=(WNDPROC)WndProc;					// WndProc handles messages
	wc.cbClsExtra=0;													// no extra window data
	wc.cbWndExtra=0;													// no extra window data
	wc.hInstance=hInstance;										// set the instance
	wc.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));	// load default icon
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);		// load arrow pointer
	wc.hbrBackground=NULL;										// no background
	wc.lpszMenuName=NULL;											// no menu
	wc.lpszClassName="razor1911";							// set class name
	if (!RegisterClass(&wc))										// register window class
	{
		MessageBox(NULL, "windows FAIL", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (fullscreen)
	{
		DEVMODE dmScreenSettings;															// device mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// is memory cleared?
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// devmode structure size
		dmScreenSettings.dmPelsWidth=screen_w;								// screen width
		dmScreenSettings.dmPelsHeight=screen_h;								// screen height
		dmScreenSettings.dmBitsPerPel=window_color;						// bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// set selected mode
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, "your video card sucks.\nuse windowed mode instead?", "ERROR", MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=false;
			}
			else
			{
				MessageBox(NULL, "the cake is a lie", "ERROR", MB_OK|MB_ICONSTOP);
				return false;
			}
		}
	}
	if (fullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;									// window extended style
		dwStyle=WS_POPUP;														// windows style
		ShowCursor(false);													// hide cursor
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW|WS_EX_CLIENTEDGE;	// window extended style
		dwStyle=WS_OVERLAPPEDWINDOW;								// windows style
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);	// adjust window to requested size
	// create window
	if (!(hWnd=CreateWindowEx(dwExStyle,		// extended style for window
		"razor1911",												// class name
		title,															// window title
		(dwStyle|														// defined window style
			WS_CLIPSIBLINGS|										// required window style
			WS_CLIPCHILDREN)										// required window style
		&~WS_THICKFRAME 										// window style (no-resize)
		//&~WS_SYSMENU												// window style (system menu)
		&~WS_MAXIMIZEBOX 										// window style (maximize)
		&~WS_MINIMIZEBOX,										// window style (minimize)
		(int)((w-screen_w)/2),							// window position x
		(int)((h-screen_h)/2),							// window position y
		(WindowRect.right-WindowRect.left),	// window width
		(WindowRect.bottom-WindowRect.top),	// window height
		NULL,																// no parent window
		NULL,																// no menu
		hInstance,													// instance
		NULL)))															// don't pass anything to WM_CREATE!
	{
		KillGLWindow();
		MessageBox(NULL, "Window creation FAIL", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!(hDC=GetDC(hWnd)))	// Did We Get A Device Context?
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!(PixelFormat=ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))	// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!(hRC=wglCreateContext(hDC)))	// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!wglMakeCurrent(hDC, hRC))	// Try To Activate The Rendering Context
	{
		KillGLWindow();
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	ShowWindow(hWnd, SW_SHOW);		// show window
	SetForegroundWindow(hWnd);	// set higher priority
	SetFocus(hWnd);							// set keyboard focus to window
	init3d(screen_w, screen_h);	// set up perspective of GL screen
	if (!InitGL())								// initialize GL window
	{
		KillGLWindow();
		MessageBox(NULL, "init FAIL", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

// window handle,window message,additional message,additional message
LRESULT CALLBACK WndProc(HWND	hWnd, UINT	uMsg, WPARAM	wParam, LPARAM	lParam)
{
	switch (uMsg)								// check windows messages
	{
	case WM_ACTIVATE:					// watch for window activate message
	{
		if (!HIWORD(wParam))			// check minimization state
		{
			active=true;					// program is active
		}
		else
		{
			active=false;					// program is no longer active
		}
		return 0;								// return to the message loop
	}
	case WM_SYSCOMMAND:				// intercept system commands
	{
		switch (wParam)					// check system calls
		{
		case SC_SCREENSAVE:		// screensaver trying to start?
		case SC_MONITORPOWER:	// monitor trying to enter powersave?
			return 0;							// prevent from happening
		}
		break;									// exit
	}
	case WM_CLOSE:						// close message?
	{
		PostQuitMessage(0);			// post quit message
		return 0;
	}
	case WM_KEYDOWN:					// key down?
	{
		keys[wParam]=true;			// mark key as true
		return 0;
	}
	case WM_KEYUP:						// key released?
	{
		keys[wParam]=false;			// mark key as false
		return 0;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam); // pass all unhandled messages to DefWindowProc
}

// instance,previous instance,command line parameters,window show state
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;																		// windows message structure
	done=false;																	// exit loop
	// create openGL window
	if (!CreateGLWindow(name)) return 0;					// quit if window not created
	// load and play music
	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
	FSOUND_Init(44100, 0);
	mod=FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_XM2), NULL);
	// main loop
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// a message is waiting?
		{
			if (msg.message==WM_QUIT)								// a quit message?
			{
				done=true;														// quit window
			}
			else																		// a window message?
			{
				TranslateMessage(&msg);								// translate message
				DispatchMessage(&msg);								// dispatch message
			}
		}
		else
		{
			// draw the scene, watch for escape key and quit message from DrawGLScene()
			if ((active&&!DrawGLScene())||keys[VK_ESCAPE]) done=true; else SwapBuffers(hDC);	// exit or swap buffers
			if (keys[VK_SPACE])
			{
				KillGLWindow();
				fullscreen=!fullscreen;
				if (!CreateGLWindow(name)) return 0;		// quit if window not created
				keys[VK_SPACE]=false;
			}
			if (keys[VK_F1])
			{
				debug_flag=!debug_flag;
				timer_fps_total=0;
				timer_fps_min=32768;
				timer_fps_max=0;
				frame_counter=0;
				keys[VK_F1]=false;
			}
			if (keys[VK_F2])
			{
				polygon=!polygon;
				glPolygonMode(GL_FRONT, polygon ? GL_FILL : GL_LINE);
				keys[VK_F2]=false;
			}
			if (keys[VK_F3])
			{
				skip_dos=true;
				keys[VK_F3]=false;
			}
			if (keys[VK_F12])
			{
				intro_flag=false;
				cube_flag=false;
				circuit_flag=false;
				move_flag=false;
				speed_flag=false;
				tunnel_flag=true;
				vote_flag=false;
				tekk_flag=false;
				glenz_flag=false;
				hidden=true;
				loop_counter=0;
				mod_play=false;
				mod_ord=-1;
				mod_row=-1;
				mod_prv_row=0;
				FMUSIC_StopSong(mod);
				FMUSIC_FreeSong(mod);
				mod=FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_XM1), NULL);
				keys[VK_F12]=false;
			}
		}
	}
	// shutdown
	FMUSIC_FreeSong(mod);					// stop and kill music
	KillGLWindow();								// kill the window
	return (msg.wParam);					// exit the program
}