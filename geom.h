/**********************************Module**********************************\
*
* geom.h
*
* 3D FlowerBox screen saver
* Geometry header file
*
* History:
*  Wed Jul 19 14:50:27 1995	-by-	Drew Bliss [drewb]
*   Created
*
* Copyright (c) 1995 Microsoft Corporation
*
\**************************************************************************/
#pragma once
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>

#ifndef PI
#define PI 3.14159265358979323846f // pi
#endif

#define FLT float
#define MAXSIDES 8
#define MAXSPTS ((MAXSUBDIV+1)*(MAXSUBDIV+1))
#define MAXPTS (MAXSIDES*MAXSPTS)
#define MAXSFACES (MAXSUBDIV*MAXSUBDIV)
#define MAXFACES (MAXSIDES*MAXSFACES)
#define MAXFPTS 4
#define MINSUBDIV 2
#define MAXSUBDIV 10

// Geometry of a shape

struct PT3
{
    float x, y, z;
};

// A side of a shape
typedef struct _SIDE
{
    // Number of triangle strips in this side
    int nstrips;
    // Number of vertices per strip
    int* strip_size;
    // Indices for each point in the triangle strips
    unsigned int* strip_index;

    // The number of elements for glDrawElements call
    int num_eles;
    // Index buffer for glDrawElements
    int* dBuf;
} SIDE;

typedef struct _GEOMETRY
{
    void (*init)(struct _GEOMETRY* geom);

    // Number of sides
    int nsides;
    // Sides
    SIDE sides[MAXSIDES];

    // Data for each vertex in the shape
    PT3* pts, * npts;
    PT3* normals;

    // Total number of vertices
    int total_pts;

    // Scaling control
    FLT min_sf, max_sf, sf_inc;

    // Initial scale factor setup control
    FLT init_sf;

} GEOMETRY;

#define GEOM_CUBE       0
#define GEOM_TETRA      1
#define GEOM_PYRAMIDS   2

extern GEOMETRY* geom_table[];

void InitVlen(GEOMETRY* geom, int npts, PT3* pts);
void UpdatePts(GEOMETRY* geom, FLT sf);
void DrawGeom(GEOMETRY* geom, FLT timer);

#define V3Sub(a, b, r) \
    ((r)->x = (a)->x-(b)->x, (r)->y = (a)->y-(b)->y, (r)->z = (a)->z-(b)->z)
#define V3Add(a, b, r) \
    ((r)->x = (a)->x+(b)->x, (r)->y = (a)->y+(b)->y, (r)->z = (a)->z+(b)->z)
#define V3Cross(a, b, r) \
    ((r)->x = (a)->y*(b)->z-(b)->y*(a)->z,\
     (r)->y = (a)->z*(b)->x-(b)->z*(a)->x,\
     (r)->z = (a)->x*(b)->y-(b)->x*(a)->y)
extern FLT V3Len(PT3* v);