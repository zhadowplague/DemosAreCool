#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"
#include "timer.h"
#include "minifmod.h"
#include "geom.h"

#define PI 3.14159265358979323846f // pi
#define PID PI/180.0f			// pi ratio
#define CR 1.0f/256.0f		// color ratio

Timer* timer;
float timer_global=0;
float timer_global_previous=0;
float timer_delta=0;
float timer_music=0;
float timer_max=0;
int loop_counter=0;
bool is_looping=false;
bool done=false;

FMUSIC_MODULE* mod;			// music handle
int mod_ord=-1;					// pattern order
int mod_row=-1;					// row number
int mod_prv_row=0;			// previous row number
int mod_time=0;					// time
bool mod_play=false;		// flag

HDC				hDC=NULL;			// GDI device context
HGLRC			hRC=NULL;			// rendering context
HWND			hWnd=NULL;		// window handle
HINSTANCE	hInstance;		// instance application
HFONT hFont;
GLuint fontBase;

int keys[256];					// keyboard array
int active=true;				// window active flag
bool fullscreen=false;	// fullscreen flag
float nearplane=0.5f;		// nearplane
float farplane=1000.0f;	// farplane
bool polygon=true;			// polygon mode
int ratio_2d=1;					// 2d ratio
/* fov variable					*/
const float fov_base=80;			// base fov angle
float fov=fov_base;			// field of view angle
/* window variable			*/
int window_w=800;				// width
int window_h=500;				// height
int screen_w;						// width
int screen_h;						// height
int window_color=32;		// color depth
int window_depth=16;		// depth buffer
/* object variable			*/
float	main_angle;				// main angle
float	main_angle_prv;		// previous main angle
float camera_target_y;
float camera_target_x;
float camera_target_z;
float camera_vel_y;
float camera_vel_z;
float camera_vel_x;
float camera_current_y=3;
float camera_current_z=5;
float camera_current_x=0;
float camera_spin;
float camera_smooth=0;
/* color/fogvariable				*/
const float initial_r=0.4f;
const float initial_g=0.4f;
const float initial_b=0.2f;
float fog_color[]={ initial_r,initial_g,initial_b,1.0f };	// fog color definition
/* text variable				*/
char* name="Bin - Demos are cool";
char* txt_dos1="F2: wireframe rendering";
char* txt_dos2="F12: credits";
char* txt_dos3="SPACE: fullscreen/windowed";
char* txt_loop="Looping";

char* txt_hidden1[]={ "- Credits 1 -", "code: rez", "code: bin", nullptr };
char* txt_hidden2[]={ "- Credits 2 -", "music: bin", "credits-music: OneyNG", nullptr };
char* txt_hidden3[]={ "Thanks for inspiration", "rez,keops,4mat,coda,bubsy", nullptr };
char* txt_hidden4[]={ "bye", nullptr };
char** txt=txt_hidden1;
/* cube variable				*/
bool cube_flag=false;		// flag
const int cube_n=16;					// number
const float cube_size=1.0f;		// size
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
float cube_face_tex[48]={
	1, 0.4, 1, 1, 0, 1,0, 0.4, //front
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //leftside
};
float cube_white_col[72];         // cube white vertex colors
float circuit_vtx[24]={ cube_w,0,cube_w,cube_w,0,-cube_w,0,0,-cube_w,0,0,cube_w,0,0,cube_w,0,0,-cube_w,-cube_w,0,-cube_w,-cube_w,0,cube_w };
/* circuit variable			*/
bool circuit_flag=false;// flag
/* glenz variable */
bool glenz_flag=true;
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
const int intro_n=36+12;					// number
int intro_i=1;					// counter
float intro_angle=0;		// angle
/* tunnel variable			*/
bool stars_flag=false;	// flag
const int stars_z_dist=32;				// depth number
const int star_n=1280;		// total star number
float star_x[star_n];			// position x
float star_y[star_n];			// position y
float star_z[star_n];			// position z
/* lake variable				*/
bool lake_flag=false;		// flag
int lake_n1=48;					// number x
int lake_n2=48;					// number y
const float lake_w=0.5f;			// space between dot
//Shapes
float quad_vtx[]={ -0.325f,-0.325f,0.325f,-0.325f,0.325f,0.325f,-0.325f,0.325f };
/* tekk variable				*/
bool tekk_flag=false;		// flag
const int tekk_bar=24;				// bar number
const int tekk_n=64;					// polygon per bar
const float tekk_v=16.0f;					// verts per quad
const float tekk_w=0.5f;			// space between bar
const float tekk_size=0.125f;	// bar size
const float tekk_radius=1.5f;	// radius
float tekk_vtx[147456];	// vertex array
float tekk_col[147456];	// color array
/* hidden variable			*/
bool hidden_flag=false;	// flag
/* fade in variable				*/
bool fade_in=true;	// flag
float fade_value=0;		// angle
/* dos variable					*/
bool dos_flag=true;// flag
bool skip_dos=false;
float dos_time=6.0f;
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

void load_tex(WORD file, GLint clamp, GLint mipmap)
{
	auto a=LoadImage(hInstance, MAKEINTRESOURCE(file), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	HICON hIcon=static_cast<HICON>(a);	
	ICONINFO iconInfo;
	GetIconInfo(hIcon, &iconInfo);
	HBITMAP hBMP=iconInfo.hbmColor;
	BITMAP bmp;
	GetObject(hBMP, sizeof(BITMAP), &bmp);
	BITMAPINFO bmpInfo={ 0 };
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth=bmp.bmWidth;
	bmpInfo.bmiHeader.biHeight=bmp.bmHeight; // Negative to avoid inversion
	bmpInfo.bmiHeader.biPlanes=1;
	bmpInfo.bmiHeader.biBitCount=32; // Assume RGBA format
	bmpInfo.bmiHeader.biCompression=BI_RGB;
	BYTE* pixels=new BYTE[bmp.bmWidth*bmp.bmHeight*4];
	GetDIBits(hDC, hBMP, 0, bmp.bmHeight, pixels, &bmpInfo, DIB_RGB_COLORS);
	GetObject(hBMP, sizeof(bmp), &bmp);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.bmWidth, bmp.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	delete[] pixels;
	DeleteObject(hBMP);
	DeleteObject(iconInfo.hbmMask);
	DestroyIcon(hIcon);
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
	gluLookAt(camera_current_x, camera_current_y, camera_current_z,                    // Camera position
		0, 0, 0,           // Look-at point
		1, 0, 0);                      // Up vector
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

void speed()
{
	speed_flag=true;
	speed_angle=main_angle;
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

GEOMETRY* cur_geom;
float sf;
float sfi;

int InitGL(void)
{
	// Reset geometry
	cur_geom = geom_table[2];
	cur_geom->init(cur_geom);
	InitVlen(cur_geom, cur_geom->total_pts, cur_geom->pts);
	sf = 0.0f;
	sfi = cur_geom->sf_inc;
	UpdatePts(cur_geom, sf);
	glClearDepth(1.0f);								// set depth buffer
	glDepthMask(GL_TRUE);							// do not write z-buffer
	glEnable(GL_CULL_FACE);						// disable cull face
	glCullFace(GL_BACK);							// don't draw front face
	glDisable(GL_LIGHTING);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, cube_face_tex);
	// fog
	glFogi(GL_FOG_MODE, GL_LINEAR);		// fog mode
	glFogfv(GL_FOG_COLOR, fog_color);	// fog color
	glFogf(GL_FOG_DENSITY, 1.0f);			// fog density
	glHint(GL_FOG_HINT, GL_NICEST);		// fog hint value
	glFogf(GL_FOG_START, 2.0f);				// fog start depth
	glFogf(GL_FOG_END, 24.0f);					// fog end depth
	// load texture
	load_tex(IDI_ICON1, GL_REPEAT, GL_NEAREST);
	// Create a font for OpenGL
	hFont=CreateFont(
		-16, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS,
		NONANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH,
		"Courier New");
	SelectObject(hDC, hFont);
	fontBase=glGenLists(96); // ASCII characters 32-127
	wglUseFontBitmaps(hDC, 32, 96, fontBase);
	// initialize some variable
	timer=new Timer();
	glenz();
	//cube
	for (int i = 0; i < 72; i++)
	{
		cube_white_col[i] = 1;
	}
	//stars
	for (int i=0; i<star_n; i++)
	{
		float angle=(rand()%3600)*0.1f;
		radius=((rand()%1000)*0.01f);
		radius=1.5f*1.125f+((radius<0.0f) ? -radius : radius);
		star_x[i]=radius*cosf(angle);
		star_y[i]=radius*sinf(angle);
		star_z[i]=(rand()%(int)(0.25f*stars_z_dist*1000))*0.001f;
		if (angle>180) star_z[i]=-star_z[i];
	}
	float y=0;
	int k=0;
	float x1, x2, y1, y2;
	y1=y-tekk_size;
	y2=y+tekk_size;
	for (int i=0; i<tekk_bar; i++)
	{
		x1=-tekk_w;
		x2=0;
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

void SyncDrums()
{
	if (mod_row % 8 == 0)
		synchro();
}

float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime) {
	smoothTime=(smoothTime<0.0001f) ? 0.0001f : smoothTime;
	float omega=2.0f/smoothTime;
	float x=omega*deltaTime;
	float expFactor=1.0f/(1.0f+x+0.48f*x*x+0.235f*x*x*x);
	float change=current-target;
	float originalTo=target;
	target=current-change;
	float temp=(currentVelocity+omega*change)*deltaTime;
	currentVelocity=(currentVelocity-omega*temp)*expFactor;
	float newPosition=target+(change+temp)*expFactor;
	if ((originalTo-current)>0.0f==(newPosition>originalTo)) {
		newPosition=originalTo;
		currentVelocity=(newPosition-originalTo)/deltaTime;
	}
	return newPosition;
}

void IntroSync()
{
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
}

void MakeBillboard()
{
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
}

int DrawGLScene(void) // draw scene
{
	// synchro
	timer->update();
	timer_delta=timer_global-timer_global_previous;
	timer_global_previous=timer_global;
	timer_global=timer->elapsed;
	// compute rotation
	main_angle_prv=main_angle;
	main_angle=timer_global*100.0f*PID;
	// start music
	if (!mod_play&&(timer_global>dos_time||skip_dos))
	{
		dos_flag=false;
		mod_play=true;
		fade_in=false;
		FMUSIC_PlaySong(mod);
		timer_music=timer_global;
		camera_current_y=-cos(timer_global)*6;
		camera_current_z=sin(timer_global)*6;
		camera_current_x=2;
	}
	if (mod_play)
	{
		mod_time=FMUSIC_GetTime(mod);
		mod_prv_row=mod_row;
		mod_row=FMUSIC_GetRow(mod);
		if (mod_row>mod_prv_row+1) mod_row=mod_prv_row;
		if (mod_row!=mod_prv_row)
		{
			if (!hidden_flag)
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
				if ((is_looping)&&(mod_row%16==0)) beat();
				if ((mod_ord>3&&mod_ord<7)&&mod_row%4==0) glenz_frame=(glenz_frame==0 ? 1 : 0);
				switch (mod_ord)
				{
				case 0:
					if (mod_row==0) {
						is_looping=loop_counter>0;
						intro_flag=true;
						stars_flag=true;
						cube_flag=false;
						lake_flag=false;
						circuit_flag=false;
						glenz_frame=2;
						intro_i=intro_n;
						camera_smooth=0;
					}
					IntroSync();
					break;
				case 1:
				case 2:
					IntroSync();
					break;
				case 3:
					IntroSync();
					if (mod_row==24) {
						speed();
						fade_in=true;
					}
					break;
				case 4:
					if (mod_row==0)
					{
						tekk_flag=true;
						fade_in=false;
						stars_flag=false;
						lake_flag=true;
						cube_flag=true;
						cube_angle=main_angle;
						circuit_flag=false;
						intro_flag=false;
						speed_flag=false;
						speed_value=1.0f;
						camera_current_x=1;
						camera_current_y=3;
						camera_current_z=24;
						camera_target_x=3;
						camera_target_y=-2;
						camera_target_z=-24;
						camera_smooth=5;
					}
					SyncDrums();
					break;
				case 5:
					camera_target_y=-16;
					camera_target_z=16;
					camera_target_x=6;
					SyncDrums();
					break;
				case 6:
					if (mod_row==0) {
						camera_target_y=3;
						camera_target_z=6;
						camera_target_x=7;
					}
					SyncDrums();
					if (mod_row==57) {
						camera_target_y=-cos(camera_spin)*6;
						camera_target_z=sin(camera_spin)*6;
						camera_target_x=-3;
						camera_smooth=0.3;
						speed();
					}
					break;
				case 7:
					if (mod_row==0)
					{
						tekk_flag=false;
						cube_flag=true;
						circuit_flag=true;
						speed_flag=false;
						lake_flag=false;
						stars_flag=true;
						intro_i=0;
						intro_flag=true;
						glenz_frame=2;
						camera_smooth=0;
					}
					else {
						intro_i++;
					}
					break;
				case 8:
					if (mod_row==0) {
						loop_counter++;
						fade_in=false;
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
						fog_color[0]=0;
						fog_color[1]=0;
						fog_color[2]=0;
						glFogfv(GL_FOG_COLOR, fog_color);
					}
					if (last_ord!=mod_ord) {
						switch (mod_ord)
						{
						case 0: txt=txt_hidden1; break;
						case 2: txt=txt_hidden2; break;
						case 3: txt=txt_hidden3; break;
						}
					}
				}
				if (mod_row==96&&mod_ord==5) {
					txt=txt_hidden4;
				}
				if (mod_row==126&&mod_ord==5)
					done=true;
				if (mod_row%8==4) synchro();
			}
		}
		if (!hidden_flag) {
			switch (mod_ord)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				camera_spin+=timer_delta*(speed_flag?3:0.5);
				camera_target_y=-cos(camera_spin)*6;
				camera_target_z=sin(camera_spin)*6;
				camera_target_x=2;
				break;
			case 7:
			case 8:
				camera_spin+=timer_delta*(speed_flag ? 3 : 0.5);
				camera_target_y=-cos(camera_spin)*6;
				camera_target_z=sin(camera_spin)*6;
				camera_target_x=-3;
				break;
			}
			camera_current_y=SmoothDamp(camera_current_y, camera_target_y, camera_vel_y, camera_smooth, timer_delta);
			camera_current_z=SmoothDamp(camera_current_z, camera_target_z, camera_vel_z, camera_smooth, timer_delta);
			camera_current_x=SmoothDamp(camera_current_x, camera_target_x, camera_vel_x, camera_smooth, timer_delta);
		}
		else {
			camera_spin+=timer_delta*(speed_flag ? 3 : 0.5);
			camera_current_y=-cos(camera_spin)*6;
			camera_current_z=sin(camera_spin)*6;
			camera_current_x=-3;
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
	if (speed_flag)
	{
		angle=(main_angle-speed_angle)*0.45f;
		speed_value=cosf(angle);
	}
	// clear screen and depth buffer
	init3d(screen_w, screen_h);
	glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FOG);
	glEnable(GL_DEPTH_TEST);

	// draw ground
	if (cube_flag)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glDisable(GL_BLEND);
		angle=cosf((cube_angle-main_angle)*0.125f);
		radius=cube_size-cube_size*angle;
		float p_x=0;
		float p_y=3.0f-2.0f*angle;
		float p_z=-14.0f+2.0f*angle;
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
		float a_x=60.0f+10.0f*angle;
		float a_y=-60.0f+(cube_angle-main_angle)*8.0f+speed_value*180.0f;
		glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
		for (int i=0; i<k; i++)
		{
			glPushMatrix();
			glTranslatef(p_x, p_y, p_z);
			glRotatef(a_x, 1.0f, 0, 0);
			glRotatef(a_y, 0, 1.0f, 0);
			glTranslatef(cube_x[i], cube_y[i], cube_z[i]);
			glColorPointer(3, GL_FLOAT, 0, cube_white_col);
			glDrawArrays(GL_QUADS, 0, 20);
			glPopMatrix();
		}

		//draw sun
		sf += sfi * timer_delta * 50;
		if (sf > cur_geom->max_sf ||
			sf < cur_geom->min_sf)
		{
			sfi = -sfi;
		}
		UpdatePts(cur_geom, sf);
		DrawGeom(cur_geom, timer_global);

		if (circuit_flag)
		{
			glVertexPointer(3, GL_FLOAT, 0, circuit_vtx);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_DST_ALPHA);
			for (int i=0; i<k; i++)
			{
				glPushMatrix();
				glTranslatef(p_x, p_y, p_z);
				glRotatef(a_x, 1.0f, 0, 0);
				glRotatef(a_y, 0, 1.0f, 0);
				glTranslatef(cube_x[i], cube_y[i]+cube_size*0.75f+0.01f, cube_z[i]);
				float c1=0.5f+0.5f*cosf(cube_a[i]*4.0f);
				float c2=0.5f+0.5f*sinf(cube_a[i]*4.0f);
				float circuit_col[]={ c1,0,c2,c2,c1,0,c1,-c2,-c2,c1,-c2,-c2,c1,-c2,-c2,c1,-c2,-c2,0,c2,c1,c1,0,c2 };
				glColorPointer(3, GL_FLOAT, 0, circuit_col);
				glDrawArrays(GL_QUADS, 0, 8);
				glPopMatrix();
			}
			glDisable(GL_BLEND);
		}

		glDisableClientState(GL_COLOR_ARRAY);
	}

	if (intro_flag)
	{
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glPushMatrix();
		glTranslatef(-2.5f, 0, 0);
		glScalef(1, 3, 3);
		glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
		glDrawArrays(GL_QUADS, 0, 24);
		glPopMatrix();

		float introPercentage=max(0, 1-((float)intro_i/(float)intro_n));
		fog_color[0]=initial_r*introPercentage;
		fog_color[1]=initial_g*introPercentage;
		fog_color[2]=initial_b*introPercentage;
		glFogfv(GL_FOG_COLOR, fog_color);
	}
	glDisable(GL_DEPTH_TEST);
	if (stars_flag)
	{
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glVertexPointer(2, GL_FLOAT, 0, quad_vtx);
		glColor3f(1.0f, 1.0f, 1.0f);
		int stars_to_render=min(star_n, star_n*((float)intro_i/(float)intro_n));
		for (int i=0; i<stars_to_render; i++)
		{
			glPushMatrix();
			glTranslatef(star_x[i], star_y[i], star_z[i]);
			MakeBillboard();
			glRotatef(0, 0, 0, 1.0f);
			glScalef(0.1f, 0.1f, 0.1f);
			glDrawArrays(GL_QUADS, 0, 4);
			glPopMatrix();
		}
	}
	glEnable(GL_BLEND);
	if (lake_flag)
	{
		glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA);
		float w=0.325f;
		float h=0.325f;
		angle=sync2_value*cosf((main_angle-sync2_angle)*1.25f);
		float p_x=-lake_n1*lake_w*0.25f;
		float z=-lake_n2*lake_w*0.5f;
		radius=1.0f;
		glVertexPointer(2, GL_FLOAT, 0, quad_vtx);
		for (int i=0; i<lake_n1; i++)
		{
			float x=p_x+i*lake_w;
			angle=720.0f*PID/lake_n1*i+cosf(i*0.375f)+main_angle*1.625f;
			for (int j=0; j<lake_n2; j++)
			{
				float y=-0.5f-radius*1.5f*cosf(main_angle*0.25f)+sinf((i+j)*0.25f)+radius*cosf(angle)+radius*sinf(720.0f*1.5f*PID/lake_n2*j+main_angle);
				glColor3f(0.5f+0.5f*cosf(90.0f*PID*y), 1.0f, 0.5f+0.5f*sinf(90.0f*PID*y));
				if (y<-2.0f&&y>-2.5f) 
					glColor3f(0.625f, 1.0f, 0.75f);
				if (y<-1.0f) 
					y=-y-1.35f;
				if (y<-0.625f) 
					y=-0.5f;
				glPushMatrix();
				glTranslatef(y-4, p_x+x+20, z+j*lake_w);
				MakeBillboard();
				glDrawArrays(GL_QUADS, 0, 4);
				glPopMatrix();
			}
		}
	}
	// draw tekk
	if (tekk_flag)
	{
		float y=0;
		int k=0;
		float z1, z2=0;
		float a1 = main_angle * 0.5f;
		for (int i=0; i<tekk_bar; i++)
		{
			angle=tekk_radius*cosf(1080.0f*PID/tekk_bar*i+cosf(i*1.5f)+a1);
			float a2 = main_angle * 2.0f;
			for (int j=0; j<tekk_n; j++)
			{
				float angle2=1080.0f*PID/tekk_n*j+a2;
				float z=sync2_value*2.0f*(rand()%100)/200.0f*cosf((main_angle-sync2_angle)*tekk_v);
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
		for (int i=0; i<tekk_bar; i++)
		{
			glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glRotatef(270, 0, 0, 1);
			glTranslatef(-16, 0, 0);
			glRotatef(360*(float)i/(float)tekk_bar, 0, 0, 1);
			glTranslatef(0, 32, 0);
			glDrawArrays(GL_QUADS, tekk_n*i*tekk_v, tekk_n*tekk_v);
			glPopMatrix();
		}
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisable(GL_FOG);
	glDisable(GL_BLEND);
	// draw glenz
	if (glenz_flag)
	{
		glEnable(GL_TEXTURE_2D);
		int frame=27*glenz_frame;
		glEnable(GL_DEPTH_TEST);
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
			glPushMatrix();
			glTranslatef(glenz_pos[i], glenz_pos[i+1], glenz_pos[i+2]);
			glScalef(glenz_scale[i], glenz_scale[i+1], glenz_scale[i+2]);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
			glColorPointer(3, GL_FLOAT, 0, cube_white_col);
			glDrawArrays(GL_QUADS, 0, 24);
			glDisableClientState(GL_COLOR_ARRAY);
			glPopMatrix();
		}
		glDisable(GL_DEPTH_TEST);
	}
	//init2d(screen_w, screen_h);
	glViewport(0, 0, screen_w, screen_h);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluOrtho2D(0, screen_w, screen_h, 0);	// init orthographic mode
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix

	// draw liner
	if (hidden_flag)
	{
		glPushAttrib(GL_LIST_BIT);
		glListBase(fontBase-32);
		for (auto i=0; txt[i]!=nullptr; i++)
		{
			glRasterPos2f(screen_w*0.5-(strlen(txt[i])*5.0), screen_h*0.4+cosf(main_angle*2)*20+i*30);
			glCallLists(strlen(txt[i]), GL_UNSIGNED_BYTE, txt[i]);
		}
		glPopAttrib();
	}
	// draw fade
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA);
	fade_value=!fade_in ? min(1, max(0, fade_value+timer_delta)) : max(0, min(1, fade_value-timer_delta));
	glColor4f(0, 0, 0, fade_value);
	rectangle(0, 0, screen_w, screen_h);
	// draw dos
	if (dos_flag)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		rectangle(20*ratio_2d, screen_h-78*ratio_2d, 60*ratio_2d*timer_global, 7*ratio_2d);

		glPushAttrib(GL_LIST_BIT);
		glListBase(fontBase-32);
		glRasterPos2f(20*ratio_2d, 50*ratio_2d);
		glCallLists(strlen(name), GL_UNSIGNED_BYTE, name);
		glRasterPos2f(20*ratio_2d, 90*ratio_2d);
		glCallLists(strlen(txt_dos1), GL_UNSIGNED_BYTE, txt_dos1);
		glRasterPos2f(20*ratio_2d, 110*ratio_2d);
		glCallLists(strlen(txt_dos2), GL_UNSIGNED_BYTE, txt_dos2);
		glRasterPos2f(20*ratio_2d, 130*ratio_2d);
		glCallLists(strlen(txt_dos3), GL_UNSIGNED_BYTE, txt_dos3);
		glPopAttrib();
	}

	// draw loop
	if (is_looping)
	{
		float c=beat_value;
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glColor3f(0.25f+c*0.5f, 0.25f+c*0.125f, 0.25f-c*0.25f);
		glPushAttrib(GL_LIST_BIT);
		glListBase(fontBase-32);
		glRasterPos2f(10, screen_h-10);
		glCallLists(strlen(txt_loop), GL_UNSIGNED_BYTE, txt_loop);
		glPopAttrib();
	}
	return true;
}

void KillGLWindow(void)
{
	glDeleteLists(fontBase, 96);
	DeleteObject(hFont);
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
	if (!UnregisterClass("bin", hInstance)) { MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK|MB_ICONINFORMATION); hInstance=NULL; }
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
	ratio_2d=(int)(screen_w/400);
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
	wc.lpszClassName="bin";							// set class name
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
		"bin",												// class name
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
				speed_flag=false;
				stars_flag=true;
				lake_flag=false;
				tekk_flag=false;
				glenz_flag=false;
				hidden_flag=true;
				dos_flag=false;
				fade_in=false;
				skip_dos=true;
				intro_i=intro_n;
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