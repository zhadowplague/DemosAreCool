#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"
#include "timer.h"
#include "minifmod.h"
#include "geom.h"
#include "utils.h"
#include "noise.h"

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
float main_angle;
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
char* txt_dos2="F11: fullscreen/windowed";
char* txt_dos3="F12: credits";
char* txt_loop="Looping";

char* txt_hidden1[]={ "- Credits 1 -", "code: rez", "code: bin", "code: drewb", nullptr};
char* txt_hidden2[]={ "- Credits 2 -", "music: bin", "credits-music: OneyNG", nullptr };
char* txt_hidden3[]={ "Thanks for inspiration", "rez,keops,4mat,coda,bubsy", nullptr };
char* txt_hidden4[]={ "bye", nullptr };
char** txt=txt_hidden1;
/* ground variable				*/
bool ground_flag=false;		
const int ground_n=8;
const int ground_l=ground_n*ground_n;
const int middle_ground_cube=ground_n/2;
const float ground_ratio=PID*ground_n;
float ground_x[ground_l];			
float ground_y[ground_l];			
float ground_z[ground_l];			
float ground_col[ground_l][96]; // colors rgba per cube
/* falling cube variable				*/
const int falling_cubes=32;
int current_falling_cube_i=0;
float cube_x[falling_cubes];
float cube_y[falling_cubes];
float cube_z[falling_cubes];
float cube_col[falling_cubes][96];
/* cube variable				*/
const int verts_in_cube=72;
float cube_vtx[verts_in_cube]={
	  // Front face
    -0.5f, -0.5f,  0.5f,  // Bottom-left
     0.5f, -0.5f,  0.5f,  // Bottom-right
     0.5f,  0.5f,  0.5f,  // Top-right
    -0.5f,  0.5f,  0.5f,  // Top-left

    // Back face
    -0.5f, -0.5f, -0.5f,  // Bottom-left
    -0.5f,  0.5f, -0.5f,  // Top-left
     0.5f,  0.5f, -0.5f,  // Top-right
     0.5f, -0.5f, -0.5f,  // Bottom-right

    // Left face
    -0.5f, -0.5f, -0.5f,  // Bottom-left
    -0.5f, -0.5f,  0.5f,  // Bottom-right
    -0.5f,  0.5f,  0.5f,  // Top-right
    -0.5f,  0.5f, -0.5f,  // Top-left

    // Right face
     0.5f, -0.5f, -0.5f,  // Bottom-left
     0.5f,  0.5f, -0.5f,  // Top-left
     0.5f,  0.5f,  0.5f,  // Top-right
     0.5f, -0.5f,  0.5f,  // Bottom-right

    // Top face
    -0.5f,  0.5f,  0.5f,  // Bottom-left
     0.5f,  0.5f,  0.5f,  // Bottom-right
     0.5f,  0.5f, -0.5f,  // Top-right
    -0.5f,  0.5f, -0.5f,  // Top-left

    // Bottom face
    -0.5f, -0.5f,  0.5f,  // Bottom-left
    -0.5f, -0.5f, -0.5f,  // Bottom-right
     0.5f, -0.5f, -0.5f,  // Top-right
     0.5f, -0.5f,  0.5f   // Top-left
};
float cube_face_tex[48]={ //cube uv for face in icon
	1, 0.4, 1, 1, 0, 1,0, 0.4, //front
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //
	1, 0.65f, 1, 0.94f, 0.79, 0.94f,0.79f, 0.65f, //leftside
};
float cube_bin_tex[48]={ //cube uv for "bin" in icon
	0, 0.6, 0, 0, 1, 0,1, 0.6,
	0, 0.6, 0, 0, 1, 0,1, 0.6,
	0, 0.6, 0, 0, 1, 0,1, 0.6,
	0, 0.6, 0, 0, 1, 0,1, 0.6,
	0, 0.6, 0, 0, 1, 0,1, 0.6,
	0, 0.6, 0, 0, 1, 0,1, 0.6,
};
/* sun variable */
bool sun_flag=false;
GEOMETRY cur_geom;
Noise noise;
float sf;
float sfi;
/* glenz variable */
bool glenz_flag=true;
bool jump_flag=false;
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
const int intro_n=48;					// number
int intro_i=1;					// counter
float intro_angle=0;		// angle
/* tunnel variable			*/
bool stars_flag=false;	// flag
const int stars_z_dist=16;				// depth number
const int star_n=1280;		// total star number
float stars[star_n*3];			// star positions xyz
/* lake variable				*/
bool lake_flag=false;		// flag
const int lake_subdiv=8;
const int lake_size=lake_subdiv*lake_subdiv*4*3;
float lake_quad_vtx[lake_size];
float lake_vtx[lake_size];
float lake_particle_vtx[lake_size];
float lake_col[lake_size];
float lake_particle_col[1024];
/* horizon variable				*/
bool horizon_flag=false;		// flag
const int horizon_bar=24;				// bar number
const int horizon_n=64;					// polygon per bar
const float horizon_v=16.0f;					// verts per quad
const float horizon_w=0.5f;			// space between bar
const float horizon_size=0.125f;	// bar size
const float horizon_radius=1.5f;	// radius
float horizon_vtx[147456];	// vertex array
float horizon_col[147456];	// color array
/* hidden variable			*/
bool hidden_flag=false;	// flag
/* fade in variable				*/
bool fade_in=true;	// flag
float fade_value=0;		// angle
/* dos variable					*/
bool dos_flag=true;// flag
bool skip_dos=false;
float dos_time=6.0f;
/* beat variable				*/
bool beat2_flag=false;	// flag
float beat2_angle=0;		// angle
float beat2_value=0;		// value
unsigned beat2_cntr=0;
bool beat_flag=false;		// flag
float beat_angle=0;			// angle
float beat_value=0;			// value
/* speed variable				*/
bool speed_flag=false;	// flag
float speed_angle=0;		// angle
float speed_value=0;		// value

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

unsigned int memopen(char* name)
{
	HGLOBAL	handle;
	auto memfile=(MEMFILE*)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, sizeof(MEMFILE));
	auto rec=FindResource(NULL, name, RT_RCDATA);
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

void beat()
{
	beat_flag=true;
	beat_angle=main_angle;

	beat2_cntr++;
	if (beat2_cntr%9u==0) {
		beat2_flag=true;
		beat2_angle=main_angle;
	}

	cube_y[current_falling_cube_i]=15;
	float radius=rand()%6+8;
	cube_x[current_falling_cube_i]=radius*cosf(rand());
	cube_z[current_falling_cube_i]=radius*sinf(rand());
	for (int l=0; l<96; l+=4) {
		cube_col[current_falling_cube_i][l]=0;
		cube_col[current_falling_cube_i][l+1]=fabs(cube_x[current_falling_cube_i]);
		cube_col[current_falling_cube_i][l+2]=fabs(cube_z[current_falling_cube_i]);
	}
	current_falling_cube_i++;
	if (current_falling_cube_i==falling_cubes)
		current_falling_cube_i=0;
}

void speed()
{
	speed_flag=true;
	speed_angle=main_angle;
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

int InitGL(void)
{
	// Reset geometry
	InitPyramids(&cur_geom);
	InitVlen(&cur_geom, cur_geom.total_pts, cur_geom.pts);
	sf=0.0f;
	sfi=cur_geom.sf_inc;
	UpdatePts(&cur_geom, sf);
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
	// initialize timer
	timer=new Timer();
	//glenz
	{
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
	//lake
	quad(lake_vtx, lake_subdiv);
	quad(lake_quad_vtx, lake_subdiv);
	quad(lake_particle_vtx, lake_subdiv);
	//ground
	{
		int k=0;
		for (int i=0; i<ground_n; i++)
		{
			float x=-(ground_n-1)*0.5f+i;
			for (int j=0; j<ground_n; j++)
			{
				ground_x[k]=x;
				ground_z[k]=-(ground_n-1)*0.5f+j;
				float alpha=(j<=2) ? ilerp(0,2,j) : ilerp(7, 5, max(j,5));
				for (int l=0; l<92; l+=4) {
					ground_col[k][l+3]=max(0.25f, alpha);
				}
				k++;
			}
		}
	}
	//stars
	for (int i=0; i<star_n; i++)
	{
		float angle=(rand()%3600)*0.1f;
		float radius=((rand()%750)*0.01f);
		radius=1.5f*1.125f+((radius<0.0f) ? -radius : radius);
		stars[i]=radius*cosf(angle);
		stars[i+1]=radius*sinf(angle);
		stars[i+2]=(rand()%(int)(0.25f*stars_z_dist*1000))*0.001f;
		if (angle>180) 
			stars[i+2]=-stars[i+2];
	}
	//horizon
	float y=0;
	int k=0;
	float x1, x2, y1, y2;
	y1=y-horizon_size;
	y2=y+horizon_size;
	for (int i=0; i<horizon_bar; i++)
	{
		x1=-horizon_w;
		x2=0;
		for (int j=0; j<horizon_n; j++)
		{
			x1+=horizon_w;
			x2+=horizon_w;
			float angle2=1080.0f*PID/horizon_n*j+main_angle*2.0f;
			horizon_vtx[k]=x1;
			horizon_vtx[k+1]=y2;
			horizon_vtx[k+3]=x1;
			horizon_vtx[k+4]=y1;
			horizon_vtx[k+6]=x2;
			horizon_vtx[k+7]=y1;
			horizon_vtx[k+9]=x2;
			horizon_vtx[k+10]=y2;
			horizon_vtx[k+12]=x1;
			horizon_vtx[k+13]=y1;
			horizon_vtx[k+15]=x1;
			horizon_vtx[k+16]=y2;
			horizon_vtx[k+18]=x2;
			horizon_vtx[k+19]=y2;
			horizon_vtx[k+21]=x2;
			horizon_vtx[k+22]=y1;
			horizon_vtx[k+24]=x1;
			horizon_vtx[k+25]=y1;
			horizon_vtx[k+27]=x1;
			horizon_vtx[k+28]=y1;
			horizon_vtx[k+29]=0;
			horizon_vtx[k+30]=x2;
			horizon_vtx[k+31]=y1;
			horizon_vtx[k+32]=0;
			horizon_vtx[k+33]=x2;
			horizon_vtx[k+34]=y1;
			horizon_vtx[k+36]=x1;
			horizon_vtx[k+37]=y2;
			horizon_vtx[k+38]=0;
			horizon_vtx[k+39]=x1;
			horizon_vtx[k+40]=y2;
			horizon_vtx[k+42]=x2;
			horizon_vtx[k+43]=y2;
			horizon_vtx[k+45]=x2;
			horizon_vtx[k+46]=y2;
			horizon_vtx[k+47]=0;
			float r=0.25f+0.25f*cosf(angle2);
			float g=0.25f;
			float b=0.25f+0.25f*sinf(angle2);
			horizon_col[k]=0.45f;
			horizon_col[k+1]=0.5f;
			horizon_col[k+2]=0.55f;
			horizon_col[k+3]=0.45f;
			horizon_col[k+4]=0.5f;
			horizon_col[k+5]=0.55f;
			horizon_col[k+6]=0.45f;
			horizon_col[k+7]=0.5f;
			horizon_col[k+8]=0.55f;
			horizon_col[k+9]=0.45f;
			horizon_col[k+10]=0.5f;
			horizon_col[k+11]=0.55f;
			horizon_col[k+12]=0.875f;
			horizon_col[k+13]=0;
			horizon_col[k+14]=0;
			horizon_col[k+15]=0.875f;
			horizon_col[k+16]=0;
			horizon_col[k+17]=0;
			horizon_col[k+18]=0.875f;
			horizon_col[k+19]=0;
			horizon_col[k+20]=0;
			horizon_col[k+21]=0.875f;
			horizon_col[k+22]=0;
			horizon_col[k+23]=0;
			horizon_col[k+24]=r;
			horizon_col[k+25]=g;
			horizon_col[k+26]=b;
			horizon_col[k+27]=r;
			horizon_col[k+28]=g;
			horizon_col[k+29]=b;
			horizon_col[k+30]=r;
			horizon_col[k+31]=g;
			horizon_col[k+32]=b;
			horizon_col[k+33]=r;
			horizon_col[k+34]=g;
			horizon_col[k+35]=b;
			horizon_col[k+36]=r;
			horizon_col[k+37]=g;
			horizon_col[k+38]=b;
			horizon_col[k+39]=r;
			horizon_col[k+40]=g;
			horizon_col[k+41]=b;
			horizon_col[k+42]=r;
			horizon_col[k+43]=g;
			horizon_col[k+44]=b;
			horizon_col[k+45]=r;
			horizon_col[k+46]=g;
			horizon_col[k+47]=b;
			k+=48;
		}
	}
	return true;
}

void SyncDrums()
{
	switch (mod_row%32)
	{
	case 0:
	case 4:
	case 8:
	case 10:
	case 14:
	case 18:
	case 22:
	case 24:
	case 31:
		beat();
		break;
	}
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
	case 6:
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
				if ((mod_ord>3&&mod_ord<7)&&mod_row%4==0) glenz_frame=(glenz_frame==0 ? 1 : 0);
				switch (mod_ord)
				{
				case 0:
					if (mod_row==0) {
						is_looping=loop_counter>0;
						intro_flag=true;
						stars_flag=true;
						ground_flag=false;
						sun_flag=false;
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
						horizon_flag=true;
						fade_in=false;
						stars_flag=false;
						sun_flag=true;
						lake_flag=true;
						ground_flag=true;
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
						ground_y[middle_ground_cube]=0;
						horizon_flag=false;
						ground_flag=false;
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
	if (beat2_flag)
	{
		float angle=(main_angle-beat2_angle)*1.5f;
		beat2_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			beat2_value=0;
			beat2_flag=false;
		}
	}
	if (beat_flag)
	{
		float angle=(main_angle-beat_angle)*1.5f;
		beat_value=1.0f-sinf(angle);
		if (angle>90.0f*PID)
		{
			beat_value=0;
			beat_flag=false;
		}
	}
	if (speed_flag)
	{
		float angle=(main_angle-speed_angle)*0.45f;
		speed_value=cosf(angle);
	}

	// init 3d
	glViewport(0, 0, screen_w, screen_h);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluPerspective(fov, (float)((float)screen_w/(float)screen_h), nearplane, farplane); // aspect ratio
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
	// Set camera position and orientation
	gluLookAt(camera_current_x, camera_current_y, camera_current_z,                    // Camera position
		0, 0, 0,           // Look-at point
		1, 0, 0);                      // Up vector	
	glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FOG);
	glEnable(GL_DEPTH_TEST);

	// draw ground
	if (ground_flag)
	{
		const float radius=0.2*cosf(main_angle*0.25f);
		int k=0;
		for (int i=0; i<ground_n; i++)
		{
			const float x=i*ground_ratio;
			for (int j=0; j<ground_n; j++)
			{
				const float y=j*ground_ratio;
				const float wave=cosf(main_angle*0.5f+y)+sinf(x);
				ground_y[k]=0.5*wave;

				const float n=noise.noise(x, y);
				const float r=n*wave;
				const float g=n*sinf(wave);
				const float b=n*cosf(wave+PI);
				for (int l=0; l<96; l+=4) {
					ground_col[k][l]=fabs(r);
					ground_col[k][l+1]=fabs(g);
					ground_col[k][l+2]=fabs(b);
				}
				k++;
			}
		}
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
		for (int i=0; i<k; i++)
		{
			glColorPointer(4, GL_FLOAT, 0, ground_col[i]);
			glPushMatrix();//x=up y=right
			glTranslatef(-5, 0, 0);
			glRotatef(-90, 0, 0, 1);
			glScalef(4, 4, 4);
			glTranslatef(ground_x[i], ground_y[i], ground_z[i]);
			glDrawArrays(GL_QUADS, 0, 24);
			glPopMatrix();
		}

		glDisable(GL_BLEND);
		for (int i=0; i<falling_cubes; i++) 
		{
			glColorPointer(4, GL_FLOAT, 0, cube_col[i]);
			glPushMatrix();//x=up y=right
			glTranslatef(cube_y[i], cube_x[i], cube_z[i]);
			glRotatef(timer_global*100, 1, 1, 0);
			glScalef(2, 2, 2);
			glDrawArrays(GL_QUADS, 0, 24);
			glPopMatrix();
			cube_y[i]-=timer_delta*25;
			for (int l=0; l<96; l+=4) {
				cube_col[current_falling_cube_i][l+3]=cube_y[i]>0?1:0;
			}
		}
		glDisableClientState(GL_COLOR_ARRAY);
	}
	//draw sun
	if (sun_flag) 
	{
		sf += sfi * timer_delta * 50;
		if (sf > cur_geom.max_sf ||
			sf < cur_geom.min_sf)
		{
			sfi = -sfi;
		}
		UpdatePts(&cur_geom, sf);

		float c=stars_flag ? 1 : 0.5f;
		glColor3f(1, 1, c);
		glDisable(GL_FOG);
		glDisable(GL_DEPTH_TEST);
		glPushMatrix();//x=up y=right
		glTranslatef(8, 16, 0);
		glRotatef(timer_global*100, 1, 1, 0);
		glScalef(4, 4, 4);
		DrawGeom(&cur_geom);
		glPopMatrix();
	}
	//draw intro fog and glenz standing on box
	if (intro_flag)
	{
		glEnable(GL_DEPTH_TEST);
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
	//draw stars
	if (stars_flag)
	{
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glPointSize(4.0f);
		glVertexPointer(3, GL_FLOAT, 0, stars);
		const float percent=(float)intro_i/(float)intro_n;
		const float c=min(1, max(0.5, percent)*1.5f);
		glColor3f(c, c, c);
		const int stars_to_render=min(star_n, star_n*percent);
		glDrawArrays(GL_POINTS, 0, stars_to_render);
	}
	//draw lake
	if (lake_flag)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA);
		int k=0;
		for (int z=0; z<lake_size; z+=3) {
			const float wave_amplitude=0.5;
			const float wave_speed=1.5;
			float xPos=lake_quad_vtx[z];
			float zPos=lake_quad_vtx[z+1]; 
			lake_vtx[z+2]=lake_quad_vtx[z+2]+wave_amplitude*sin((xPos+zPos)+wave_speed*timer_global);
			lake_particle_vtx[z+2]=lake_quad_vtx[z+2]+sin((xPos+zPos)+wave_speed*(timer_global-1));
			lake_col[z]=0;
			lake_col[z+1]=0.5+0.5*fabs(sin(lake_vtx[z+2]));
			lake_col[z+2]=0.5+0.5*fabs(sin(lake_vtx[z+2]));
			lake_particle_col[k]=lake_col[z];
			lake_particle_col[k+1]=lake_col[z+1];
			lake_particle_col[k+2]=lake_col[z+2];
			lake_particle_col[k+3]=lake_particle_vtx[z+2]<0.4 ? 0 : 1;
			k+=4;
		}
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, lake_col);
		glVertexPointer(3, GL_FLOAT, 0, lake_vtx);
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glTranslatef(-20, 16, -8);//x=fwd z=up
		glScalef(5, 5, 5);
		glDrawArrays(GL_QUADS, 0, 256);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColorPointer(4, GL_FLOAT, 0, lake_particle_col);
		glVertexPointer(3, GL_FLOAT, 0, lake_particle_vtx);
		glDrawArrays(GL_POINTS, 0, 256);
		glPopMatrix();
		glDisableClientState(GL_COLOR_ARRAY);
	}
	// draw horizon
	if (horizon_flag)
	{
		float y=0;
		int k=0;
		float z1, z2=0;
		float a1 = main_angle * 0.5f;
		for (int i=0; i<horizon_bar; i++)
		{
			float angle=horizon_radius*cosf(1080.0f*PID/horizon_bar*i+cosf(i*1.5f)+a1);
			float a2 = main_angle * 2.0f;
			for (int j=0; j<horizon_n; j++)
			{
				float angle2=1080.0f*PID/horizon_n*j+a2;
				float z=beat2_value*2.0f*(rand()%100)/200.0f*cosf((main_angle-beat2_angle)*horizon_v);
				z1=z2;
				z2=(float)fabs(angle+horizon_radius*sinf(angle2))+z;
				if (z2<0.5f) z2=0.5f;
				horizon_vtx[k+2]=z1;
				horizon_vtx[k+5]=z1;
				horizon_vtx[k+8]=z2;
				horizon_vtx[k+11]=z2;
				horizon_vtx[k+14]=z1;
				horizon_vtx[k+17]=z1;
				horizon_vtx[k+20]=z2;
				horizon_vtx[k+23]=z2;
				horizon_vtx[k+26]=z1;
				horizon_vtx[k+35]=z2;
				horizon_vtx[k+41]=z1;
				horizon_vtx[k+44]=z2;
				k+=48;
			}
		}
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, horizon_vtx);
		glColorPointer(3, GL_FLOAT, 0, horizon_col);
		glBlendFunc(GL_SRC_COLOR, GL_DST_ALPHA);
		for (int i=0; i<horizon_bar; i++)
		{
			glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glRotatef(270, 0, 0, 1);
			glTranslatef(-16, 0, 0);
			glRotatef(360*(float)i/(float)horizon_bar, 0, 0, 1);
			glTranslatef(0, 32, 0);
			glScalef(2, 2, 2);
			glDrawArrays(GL_QUADS, horizon_n*i*horizon_v, horizon_n*horizon_v);
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
		glEnable(GL_DEPTH_TEST);
		glColor3f(1, 1, 1);
		int frame=27*glenz_frame;
		glPushMatrix();
		glTranslatef(glenz_pos[0+frame], glenz_pos[1+frame], glenz_pos[2+frame]);
		glTranslatef(ground_y[middle_ground_cube]*4, 0, 0);
		glScalef(glenz_scale[0+frame], glenz_scale[1+frame], glenz_scale[2+frame]);
		glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, cube_face_tex);
		glDrawArrays(GL_QUADS, 0, 24);
		glPopMatrix();

		int endFrame=27*(glenz_frame+1);
		for (int i=frame+3; i<endFrame; i+=3) {
			glPushMatrix();
			glTranslatef(glenz_pos[i], glenz_pos[i+1], glenz_pos[i+2]);
			glTranslatef(ground_y[middle_ground_cube]*4, 0, 0);
			glScalef(glenz_scale[i], glenz_scale[i+1], glenz_scale[i+2]);
			glVertexPointer(3, GL_FLOAT, 0, cube_vtx);
			glTexCoordPointer(2, GL_FLOAT, 0, cube_bin_tex);
			glDrawArrays(GL_QUADS, 0, 24);
			glPopMatrix();
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
	}
	
	//init2d
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

#if _DEBUG
	GLenum err;
	while ((err=glGetError())!=GL_NO_ERROR)
	{
		printf("statement to attach breakpoint to");
	}
#endif

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
				jump_flag=true;
				keys[VK_SPACE]=false;
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
			if (keys[VK_F11])
			{
				KillGLWindow();
				fullscreen=!fullscreen;
				if (!CreateGLWindow(name)) return 0;		// quit if window not created
				keys[VK_F11]=false;
			}
			if (keys[VK_F12])
			{
				intro_flag=false;
				ground_flag=false;
				speed_flag=false;
				stars_flag=true;
				lake_flag=false;
				horizon_flag=false;
				glenz_flag=false;
				hidden_flag=true;
				dos_flag=false;
				fade_in=false;
				skip_dos=true;
				intro_i=intro_n;
				loop_counter=0;
				mod_play=true;
				mod_ord=-1;
				mod_row=-1;
				mod_prv_row=0;
				FMUSIC_StopSong(mod);
				FMUSIC_FreeSong(mod);
				mod=FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_XM1), NULL);
				FMUSIC_PlaySong(mod);
				keys[VK_F12]=false;
			}
		}
	}
	// shutdown
	FMUSIC_FreeSong(mod);					// stop and kill music
	KillGLWindow();								// kill the window
	return (msg.wParam);					// exit the program
}