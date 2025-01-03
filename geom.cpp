/**********************************Module**********************************\
*
* geom.c
*
* 3D FlowerBox screen saver
* Geometry routines
*
* History:
*  Wed Jul 19 14:50:27 1995	-by-	Drew Bliss [drewb]
*   Created
*
* Copyright (c) 1995 Microsoft Corporation
*
\**************************************************************************/

#include "geom.h"

// Sphere radius
#define RADIUS 1

#define SUBDIVS 6

FLT V3Len(PT3* v)
{
	return (FLT)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

// Values to map a 2D point onto a 3D plane
// Base point and axes to map X and Y coordinates onto
typedef struct _PLANE_MAP
{
	PT3 base, x_axis, y_axis;
} PLANE_MAP;

// Data area used by the current geometry
// Base points and generated points
PT3 pts[MAXPTS], npts[MAXPTS];
// Scaling factor for spherical projection
FLT vlen[MAXPTS];
// Vertex data indices
int index[MAXPTS * 2];
// Triangle strip sizes
int strip_size[MAXSIDES * MAXSUBDIV];

/******************************Public*Routine******************************\
*
* InitVlen
*
* Precomputes scaling factor for spherical projection
*
* History:
*  Mon Jul 24 14:59:03 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

void InitVlen(GEOMETRY* geom, int npts, PT3* pts)
{
	FLT d;
	FLT* vl;

	vl = vlen;
	while (npts-- > 0)
	{
		d = V3Len(pts);
		// Geometries are created with size one, filling the area
		// from -.5 to .5.  This leads to distances generally less
		// than one, which leaves off half of the interesting morphing
		// effects due to the projection
		// Scaling up the scaling factor allows the values to
		// be both above and below one
		d *= geom->init_sf;

		*vl++ = (RADIUS - d) / d;

		pts++;
	}
}

/******************************Public*Routine******************************\
*
* MapToSide
*
* Takes x,y coordinates in the range 0-1 and maps them onto the given
* side plane for the current geometry
*
* History:
*  Mon Jul 24 15:10:34 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

void MapToSide(PLANE_MAP* map, FLT x, FLT y, PT3* pt)
{
	pt->x = x * map->x_axis.x + y * map->y_axis.x + map->base.x;
	pt->y = x * map->x_axis.y + y * map->y_axis.y + map->base.y;
	pt->z = x * map->x_axis.z + y * map->y_axis.z + map->base.z;
}



/******************************Public*Routine******************************\
*
* InitPyramids
*
* Initializes double pyramid geometry
*
* History:
*  Wed Jul 26 18:37:11 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

#define PYRAMIDS_SIDES 8

PLANE_MAP pyramids_planes[PYRAMIDS_SIDES] =
{
	-0.5f, 0.0f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  0.5f, -0.5f,
	 0.5f, 0.0f,  0.5f, -1.0f, 0.0f,  0.0f,  0.0f, -0.5f, -0.5f,
	 0.5f, 0.0f,  0.5f,  0.0f, 0.0f, -1.0f, -0.5f,  0.5f,  0.0f,
	 0.5f, 0.0f, -0.5f,  0.0f, 0.0f,  1.0f, -0.5f, -0.5f,  0.0f,
	 0.5f, 0.0f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  0.5f,  0.5f,
	-0.5f, 0.0f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -0.5f,  0.5f,
	-0.5f, 0.0f, -0.5f,  0.0f, 0.0f,  1.0f,  0.5f,  0.5f,  0.0f,
	-0.5f, 0.0f,  0.5f,  0.0f, 0.0f, -1.0f,  0.5f, -0.5f,  0.0f
};

void InitPyramids(GEOMETRY* geom)
{
	int side, x, y, k, num_in_side;
	PT3* pt;
	int* sz, * idx, idc, i0, i1, i2, i3, ss;
	int side_pts;
	int base_pt;
	int row_pts;
	FLT fx, fy;

	side_pts = (SUBDIVS + 2) * (SUBDIVS + 1) / 2;

	geom->nsides = PYRAMIDS_SIDES;
	geom->pts = &pts[0];
	geom->npts = &npts[0];

	geom->min_sf = -1.1f;
	geom->max_sf = 5.2f;
	geom->sf_inc = 0.05f;
	geom->init_sf = 3.0f;

	// Generate triangle strip data
	sz = &strip_size[0];
	idx = &index[0];
	base_pt = 0;

	for (side = 0; side < geom->nsides; side++) {
		geom->sides[side].nstrips = SUBDIVS;
		geom->sides[side].strip_size = sz;
		geom->sides[side].strip_index = (unsigned*)idx;

		for (x = 0; x < SUBDIVS; x++) {
			row_pts = SUBDIVS - x + 1;
			*sz++ = row_pts * 2 - 1;

			*idx++ = base_pt;
			for (y = 0; y < row_pts - 1; y++) {
				*idx++ = base_pt + row_pts + y;
				*idx++ = base_pt + 1 + y;
			}

			base_pt += row_pts;
		}

		base_pt++;
	}
	// Generate base vertices
	pt = geom->pts;
	for (side = 0; side < geom->nsides; side++)
	{
		for (x = 0; x < SUBDIVS + 1; x++)
		{
			fx = (FLT)x / SUBDIVS;
			for (y = 0; y < SUBDIVS - x + 1; y++)
			{
				MapToSide(&pyramids_planes[side],
					fx + (FLT)y / (SUBDIVS * 2),
					(FLT)y / SUBDIVS,
					pt);
				pt++;
			}
		}
	}

	geom->total_pts = geom->nsides * side_pts;
}

/******************************Public*Routine******************************\
*
* InitCylinder
*
* Initializes the cylinder geometry
*
* History:
*  Fri Jul 28 16:12:39 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

void InitCylinder(GEOMETRY* geom)
{
	int side, x, y, k, num_in_side;
	PT3* pt;
	int* sz, * idx, ss, idc, i0, i1, i2, i3;
	int base_pt;
	int row_pts;
	FLT fx, fy, fz;
	double ang;

	geom->nsides = 1;
	geom->pts = &pts[0];
	geom->npts = &npts[0];

	geom->min_sf = -2.5f;
	geom->max_sf = 8.5f;
	geom->sf_inc = 0.05f;
	geom->init_sf = 2.1f;

	// Generate triangle strip data
	// If version 1.1 then allocate the index buffer for glDrawElements
	sz = &strip_size[0];
	idx = &index[0];
	side = 0;
	geom->sides[side].nstrips = SUBDIVS;
	geom->sides[side].strip_size = sz;
	geom->sides[side].strip_index = (unsigned*)idx;

	row_pts = SUBDIVS + 1;
	base_pt = 0;
	for (x = 0; x < SUBDIVS; x++) {
		*sz++ = row_pts * 2;

		for (y = 0; y < row_pts; y++) {
			// Wrap around at the edge so the cylinder normals
			// are properly averaged
			if (x == SUBDIVS - 1) {
				*idx++ = y;
			}
			else {
				*idx++ = base_pt + row_pts + y;
			}
			*idx++ = base_pt + y;
		}

		base_pt += row_pts;
	}

	// Generate base vertices
	pt = geom->pts;
	ang = 0;
	for (x = 0; x < SUBDIVS; x++)
	{
		fx = (FLT)cos(ang) * 0.5f;
		fz = (FLT)sin(ang) * 0.5f;
		for (y = 0; y < SUBDIVS + 1; y++)
		{
			pt->x = fx;
			pt->y = (FLT)y / SUBDIVS - 0.5f;
			pt->z = fz;
			pt++;
		}
		ang += (2 * PI) / SUBDIVS;
	}
	geom->total_pts = geom->nsides * (SUBDIVS + 1) * SUBDIVS;
}

/******************************Public*Routine******************************\
*
* InitSpring
*
* Initializes the spring geometry
*
* History:
*  Fri Jul 28 16:12:39 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

#define SPRING_RADIUS 0.1f
#define SPRING_CENTER (0.5f-SPRING_RADIUS)

void InitSpring(GEOMETRY* geom)
{
	int side, x, y, k, num_in_side;
	PT3* pt;
	int* sz, * idx, idc, ss, i0, i1, i2, i3;
	double ang_center, ang_surf;
	FLT cs, sn;
	FLT rad;
	PLANE_MAP plane;
	int spin_pts;
	int row_pts;

	geom->nsides = 1;
	geom->pts = &pts[0];
	geom->npts = &npts[0];

	geom->min_sf = -2.2f;
	geom->max_sf = 0.2f;
	geom->sf_inc = 0.05f;
	geom->init_sf = 1.0f;

	// Generate triangle strip data
	// If version 1.1 then allocate the index buffer for glDrawElements
	sz = &strip_size[0];
	idx = &index[0];
	side = 0;
	geom->sides[side].nstrips = SUBDIVS;
	geom->sides[side].strip_size = sz;
	geom->sides[side].strip_index = (unsigned*)idx;

	row_pts = SUBDIVS;
	spin_pts = 4 * SUBDIVS + 1;
	for (x = 0; x < SUBDIVS; x++) {
		*sz++ = spin_pts * 2;

		for (y = 0; y < spin_pts; y++) {
			*idx++ = x + row_pts * y;
			// Wrap around at the edge so the cylindrical surface
			// of the tube is seamless.  Without this the normal
			// averaging would be incorrect and a seam would be visible
			if (x == SUBDIVS - 1) {
				*idx++ = row_pts * y;
			}
			else {
				*idx++ = x + row_pts * y + 1;
			}
		}
	}

	// Generate base vertices
	pt = geom->pts;
	ang_center = 0;
	plane.y_axis.x = 0.0f;
	plane.y_axis.y = SPRING_RADIUS;
	plane.y_axis.z = 0.0f;
	plane.x_axis.y = 0.0f;
	for (x = 0; x < spin_pts; x++)
	{
		cs = (FLT)cos(ang_center);
		sn = (FLT)sin(ang_center);
		rad = 0.5f - (FLT)x / (spin_pts - 1) * (SPRING_CENTER / 2);
		plane.base.x = cs * rad;
		plane.base.y = -0.5f + (FLT)x / (spin_pts - 1);
		plane.base.z = sn * rad;
		plane.x_axis.x = cs * SPRING_RADIUS;
		plane.x_axis.z = sn * SPRING_RADIUS;

		ang_surf = 0;
		for (y = 0; y < SUBDIVS; y++)
		{
			MapToSide(&plane,
				(FLT)cos(ang_surf), (FLT)sin(ang_surf),
				pt);
			pt++;
			ang_surf += (2 * PI) / SUBDIVS;
		}

		ang_center += (4 * PI) / (spin_pts - 1);
	}

	geom->total_pts = geom->nsides * spin_pts * SUBDIVS;
}

/******************************Public*Routine******************************\
*
* DrawGeom
*
* Draw the current geometry
*
* History:
*  Wed Jul 19 14:53:02 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

void DrawGeom(GEOMETRY* geom)
{
	int side, strip;
	int* idx, idc, idxv;
	for (side = 0; side < geom->nsides; side++) {
		idx = (int*)geom->sides[side].strip_index;
		for (strip = 0; strip < geom->sides[side].nstrips; strip++) {
			glBegin(GL_TRIANGLE_STRIP);
			for (idc = 0; idc < geom->sides[side].strip_size[strip]; idc++) {
				idxv = *idx++;
				glVertex3fv((GLfloat*)&geom->npts[idxv]);
			}
			glEnd();
		}
	}
}

/******************************Public*Routine******************************\
*
* UpdatePts
*
* Project the point array through a sphere according to the given scale factor
*
* History:
*  Wed Jul 19 14:53:53 1995	-by-	Drew Bliss [drewb]
*   Created
*
\**************************************************************************/

void UpdatePts(GEOMETRY* geom, FLT sf)
{
	int pt;
	FLT f, * vl;
	PT3* v;
	PT3* p;

	vl = vlen;
	p = &geom->pts[0];
	v = &geom->npts[0];
	for (pt = 0; pt < geom->total_pts; pt++)
	{
		f = (*vl++) * sf + 1;
		v->x = p->x * f;
		v->y = p->y * f;
		v->z = p->z * f;
		p++;
		v++;
	}
}

