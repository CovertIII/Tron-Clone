#include <stdlib.h>
#include <stdio.h>
#include "vector2.h"
#include <math.h>
#include "collison.h"

int line_collison(vector2 p, vector2 bp, vector2 q, vector2 bq){
	vector2 r = v2Sub(bp, p);
	vector2 s = v2Sub(bq, q);
	
	if(v2Cross(r, s) > -0.1 && v2Cross(r, s) < 0.1)
		{return 0;}
	
	float t = v2Cross(v2Sub(q, p), s) / v2Cross(r, s);
	float u = v2Cross(v2Sub(q, p), r) / v2Cross(r, s);
	
	if((t >= 0 && t <=1) && (u >= 0 && u <=1) )
		{return 1;}
	
	return 0;
}
