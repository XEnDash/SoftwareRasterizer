#pragma once

#include <stdint.h>
#include <math.h>

typedef int8_t int8;
typedef uint8_t uint8;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

typedef uint32 bool32;

class Color
{
public:
	int r, g, b;

	static const Color white;
	static const Color black;
	static const Color red;
	static const Color green;
	static const Color blue;
};

#define RGBtob32(r, g, b) ((r<<16) | (g<<8) | (b))

#define Shiftb32tob8(p) ((p[0]) | (p[1]) << 8 | (p[2] << 8) | (p[3] << 24))

#define min3(a, b, c) (fmin(fmin(a, b), c))
#define max3(a, b, c) (fmax(fmax(a, b), c))

#define Clamp(c, min, max) { if(c < min) c = min; if(c > max) c = max; }

#include "MemoryManager.h"
#include "Assert.h"