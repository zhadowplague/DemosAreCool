#pragma once
#include "utils.h"
#include <math.h>
#include <stdlib.h>

class Noise
{
    int perm[512];

public:
    Noise() {
        for (int i=0; i<256; i++) {
            perm[i]=perm[i+256]=rand()%255;
        }
    }

    float noise(float x, float y) {
        int X=(int)floor(x)&255;
        int Y=(int)floor(y)&255;

        x-=floor(x);
        y-=floor(y);

        float u=fade(x);
        float v=fade(y);

        int A=perm[X]+Y;
        int AA=perm[A];
        int AB=perm[A+1];
        int B=perm[X+1]+Y;
        int BA=perm[B];
        int BB=perm[B+1];

        return lerp(
            lerp(grad(perm[AA], x, y), grad(perm[BA], x-1, y), u),
            lerp(grad(perm[AB], x, y-1), grad(perm[BB], x-1, y-1), u),
            v);
    }
};

