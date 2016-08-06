#include "Triangle.h"

bool CTriangle::Create(V3 a, V3 b, V3 c)
{
	Create(a, b, c, V2::Zero, V2::Zero, V2::Zero);
	
	return true;
}

bool CTriangle::Create(V3 a, V3 b, V3 c, V2 a_tex, V2 b_tex, V2 c_tex)
{
	this->a = a;
	this->b = b;
	this->c = c;

	this->a_tex = a_tex;
	this->b_tex = b_tex;
	this->c_tex = c_tex;

	return true;
}

void CTriangle::Transform(CMatrix4 &m)
{
	this->a.Transform(m);
	this->b.Transform(m);
	this->c.Transform(m);
}

float CTriangle::Area()
{
	float ab = (b - a).Length();
	float bc = (c - b).Length();
	float ca = (a - c).Length();
	float s = (ab + bc + ca) / 2;

	return sqrt(s * (s - ab) * (s - bc) * (s - ca));
}

void BarycentricCoords(V2 a, V2 b, V2 c, V2 p, float *u, float *v, float *w)
{
	V2 v0 = b - a, v1 = c - a, v2 = p - a;

	float d00 = V2::Dot(v0, v0);
	float d01 = V2::Dot(v0, v1);
	float d11 = V2::Dot(v1, v1);
	float d20 = V2::Dot(v2, v0);
	float d21 = V2::Dot(v2, v1);

	float denom = d00 * d11 - d01 * d01;

	*v = (d11 * d20 - d01 * d21) / denom;
	*w = (d00 * d21 - d01 * d20) / denom;
	*u = 1.0f - *v - *w;
}