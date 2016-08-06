#pragma once

#include "Defines.h"
#include <stdio.h>

class CLog
{
public:
	static bool32 Init(char *filename);
	static void Log(const char *s);
	static void FLog(const char *s, ...);
private:
	char *filename;
	FILE *f;

	static CLog singleton; // NOTE(dShamir): new and delete wants logging, but we don't have logging yet, so the class is a global variable
};
