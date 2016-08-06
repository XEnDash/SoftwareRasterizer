#pragma once

#include "Defines.h"
#include "Triangle.h"

class CWavefrontOBJ
{
public:
	bool Load(char *file);

	CTriangle *tris;
	int tris_num;
};