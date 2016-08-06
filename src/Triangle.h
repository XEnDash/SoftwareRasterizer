#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4.h"

class CTriangle
{
public:

	bool Create(V3 a, V3 b, V3 c);
	bool Create(V3 a, V3 b, V3 c, V2 a_tex, V2 b_tex, V2 c_tex);

	void Transform(CMatrix4 &m);

	float Area();

private:
	
public:
	V3 a, b, c;
	V2 a_tex, b_tex, c_tex;

private:
};

void BarycentricCoords(V2 a, V2 b, V2 c, V2 p, float *u, float *v, float *w);