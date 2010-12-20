#include "vector2.h"
#include <math.h>

vector2 v2Add(vector2 a, vector2 b) {
	vector2 c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}
vector2 v2Sub(vector2 a, vector2 b) {
	vector2 c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}
vector2 v2Neg(vector2 a) {
	a.x = -a.x;
	a.y = -a.y;
	return a;
}	
vector2 v2Unit(vector2 v) {
	vector2 a;
	a.x = v.x/v2Len(v);
	a.y = v.y/v2Len(v);
	return a;
}
vector2 v2sMul(float f, vector2 v) {
	v.x = f*v.x;
	v.y = f*v.y;
	return v;
}
vector2 v2Polar(float len, float angle) {
	vector2 a;
	a.x = len*cos(angle);
	a.y = len*sin(angle);
	return a;
}

float v2Dot(vector2 a, vector2 b) {
	return a.x*b.x + a.y*b.y;
}
float v2Len(vector2 v) {
	return sqrt(v.x*v.x + v.y*v.y);
}
float v2SPow(vector2 v) {
	return v.x*v.x + v.y*v.y;
}
float v2Arg(vector2 v) {
	return atan2(v.y,v.x);
}