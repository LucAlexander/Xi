#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "mathutils.h"

uint8_t hash_v2(v2 key, uint32_t capacity){
	uint64_t x = key.x;
	uint64_t y = key.y;
	return (x << 32 | y) % capacity;
}

uint8_t v2_equals(v2 a, v2 b){
	return 
		(a.x==b.x)&&
		(a.y==b.y)
	;
}

uint8_t v3_equals(v3 a, v3 b){
	return 
		(a.x==b.x)&&
		(a.z==b.z)&&
		(a.y==b.y)
	;
}

uint8_t v4_equals(v4 a, v4 b){
	return 
		(a.x==b.x)&&
		(a.w==b.w)&&
		(a.h==b.h)&&
		(a.y==b.y)
	;
}

uint8_t m2_equals(m2 a, m2 b){
	return
		(a.x1==b.x1)&&
		(a.x2==b.x2)&&
		(a.y1==b.y1)&&
		(a.y2==b.y2)
	;
}

uint8_t edge_collision_v4(v4 mask, m2 edge){
	v2 point_sentinel = {FLT_MAX, FLT_MAX};
	return !(
		v2_equals(edge_intersect(mask.x, mask.y, mask.w, mask.y, edge), point_sentinel) &&
		v2_equals(edge_intersect(mask.x, mask.y, mask.x, mask.h, edge), point_sentinel) &&
		v2_equals(edge_intersect(mask.w, mask.y, mask.w, mask.h, edge), point_sentinel) &&
		v2_equals(edge_intersect(mask.x, mask.h, mask.w, mask.h, edge), point_sentinel)
	);
}

v2 edge_intersect_v2(v2 a, v2 b, v2 c, v2 d){
	m2 edge = {c.x, c.y, d.x, d.y};
	return edge_intersect(a.x, a.y, b.x, b.y, edge);
}

v2 edge_intersect(float ax, float ay, float bx, float by, m2 edge){
	float a1 = by - ay;
	float b1 = ax - bx;
	float c1 = a1*(ax) + b1*(ay);
	float a2 = edge.y2 - edge.y1;
	float b2 = edge.x1 - edge.x2;
	float c2 = a2*(edge.x1) + b2*(edge.y1);
	float result = a1*b2 - a2*b1;
	if (result == 0){
		v2 point = {FLT_MAX, FLT_MAX};
		return point;
	}
	float x = (b2*c1 - b1*c2)/result;
	float y = (a1*c2 - a2*c1)/result;
	v2 point = {x, y};
	return point;
}

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
	return (yy > y) ? (360-res) : -res;
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

uint8_t rectCollides(v4 a, v4 b){
	v4 ab = {a.x, a.y, a.x + a.w, a.y + a.h};
	v4 bb = {b.x, b.y, b.x + b.w, b.y + b.h};
	return rectCollidesB(ab, bb);
}

uint8_t rectCollidesB(v4 a, v4 b){
	return !((a.x > b.w || a.w < b.x) || (a.y > b.h || a.h < b.y));
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
