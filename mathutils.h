#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <inttypes.h>

#ifndef PI
#define PI 3.1415926
#endif

typedef struct v2{
	float x;
	float y;
}v2;

typedef struct v3{
	float x;
	float y;
	float z;
}v3;

typedef struct v4{
	float x;
	float y;
	float w;
	float h;
}v4;

float lenDirX(float len, float dir);

float lenDirY(float len, float dir);

int8_t sign(float val);

float distancePoint(float x, float y, float xx, float yy);
float distancePointV2(v2, v2);

float directionPoint(float x, float y, float xx, float yy);
float directionPointV2(v2, v2);

uint8_t pointInRect(float x, float y, v4);
uint8_t pointInRectV2(v2, v4);

uint8_t pointInRectB(float x, float y, v4);
uint8_t pointInRectV2B(v2, v4);

void approachZero(int32_t* val, int32_t amount);
void approachZeroF(float* val, float amount);

#endif
