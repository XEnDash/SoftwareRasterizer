#pragma once

#include "Defines.h"

class CTexture
{
public:
	bool Load(char *file);

private:

public:
	Color *pixels;
	uint32 w, h;
	int32 bits_per_pixel;

private:
};