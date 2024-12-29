#include "utils.h"

float ilerp(float a, float b, float x) {
	return (x-a)/(b-a);
}

float lerp(float a, float b, float t) {
	return a+t*(b-a);
}

float fade(float t)
{
	return t*t*t*(t*(t*6-15)+10);
}

float grad(int hash, float x, float y) {
	int h=hash&7; // There are 8 possible gradients
	float u=h<4 ? x : y;
	float v=h<4 ? y : x;
	return ((h&1) ? -u : u)+((h&2) ? -v : v);
}


void quad(float* vertices, int subdivisions) {
	//Expected length of vertices array is = subdivisions(width) * subdivisions(height) * 4(verts per quad) * 3(xyz);
	const float size=10;
	const float stepX=size/subdivisions;
	const float stepY=size/subdivisions;

	int index=0;
	for (int i=0; i<subdivisions; ++i) {
		for (int j=0; j<subdivisions; ++j) {
			float x0=i*stepX;
			float y0=j*stepY;
			float x1=x0+stepX;
			float y1=y0+stepY;

			// Define the quad vertices in 3D space
			vertices[index++]=x0; vertices[index++]=y0; vertices[index++]=0; // Bottom-left
			vertices[index++]=x1; vertices[index++]=y0; vertices[index++]=0; // Bottom-right
			vertices[index++]=x1; vertices[index++]=y1; vertices[index++]=0; // Top-right
			vertices[index++]=x0; vertices[index++]=y1; vertices[index++]=0; // Top-left
		}
	}
}