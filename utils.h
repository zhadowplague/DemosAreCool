#pragma once

float lerp(float a, float b, float t);

float ilerp(float a, float b, float x);

// Fade curve
float fade(float t);

// Gradient function for 2D
float grad(int hash, float x, float y);

void quad(float* vertices, int subdivisions);