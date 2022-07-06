#include <math.h>
#include <stdlib.h>
#include "mathutils.h"

float lenDirX(float len, float dir){
	return len * cos(dir*PI/180.0);
}

float lenDirY(float len, float dir){
	return len * -sin(dir*PI/180.0);
}

int8_t sign(float val){
	return val == 0 ? 0 : val > 0 ? 1 : -1;
}

float distancePoint(float x, float y, float xx, float yy){
	x -= xx;
	y -= yy;
	x *= x;
	y *= y;
	return sqrt(x+y);
}

float distancePointV2(v2 a, v2 b){
	return distancePoint(a.x, a.y, b.x, b.y);
}

float directionPoint(float x, float y, float xx, float yy){
	float res = (atan2(yy-y, xx-x)*180)/PI;
	return (yy > y) ? (360-res) : res;
}

float directionPointV2(v2 a, v2 b){
	return directionPoint(a.x, a.y, b.x, b.y);
}

uint8_t pointInRect(float x, float y, v4 r){
	if (r.w < 0){
		r.w *= -1;
		r.x -= r.w;
	}
	if (r.h < 0){
		r.h *= -1;
		r.y -= r.w;
	}
	return (x >= r.x && x <= r.x+r.w) && 
		(y >= r.y && y <= r.y+r.h);
}

uint8_t pointInRectV2(v2 a, v4 b){
	return pointInRect(a.x, a.y, b);
}

uint8_t pointInRectB(float x, float y, v4 r){
	if (r.x > r.w){
		float temp = r.x;
		r.x = r.w;
		r.w = temp;
	}
	if (r.y > r.h){
		float temp = r.y;
		r.y = r.h;
		r.h = temp;
	}
	return (x >= r.x && x <= r.w) && 
		(y >= r.y && y <= r.h);
}

uint8_t pointInRectV2B(v2 a, v4 b){
	return pointInRectB(a.x, a.y, b);
}

void approachZero(int32_t* val, int32_t amount){
	uint8_t s = (*val>0);
	int8_t flip = s-(s==0);
	*val += flip * (amount*(*val!=0));
	*val *= (s == (*val>0));
}

void approachZeroF(float* val, float amount){
	uint8_t s = (*val>0);
	int8_t flip = s-(s==0);
	*val -= flip * (amount*(*val!=0));
	*val *= (s == (*val>0));
}
