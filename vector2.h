typedef struct {
	double x,y;
} vector2;

vector2 v2Add(vector2 a, vector2 b);
vector2 v2Sub(vector2 a, vector2 b);
vector2 v2Neg(vector2 a);
vector2 v2Unit(vector2 v);
vector2 v2sMul(float f, vector2 v);
vector2 v2Polar(float len, float angle);
vector2 v2Rotate(float ang, vector2 a);

float v2Dot(vector2 a, vector2 b);
float v2Cross(vector2 a, vector2 b); 
float v2Len(vector2 v);
float v2SPow(vector2 v);
float v2Arg(vector2 v);

