#include "Log.h"

#include <stdarg.h>

CLog CLog::singleton;

bool32 CLog::Init(char *filename)
{
	if (fopen_s(&singleton.f, filename, "w") != 0)
		return false;

	fprintf(singleton.f, "=== Log Initialized ===\n\n");

	return true;
}

void CLog::Log(const char *s)
{
	fprintf(singleton.f, s);
	fprintf(singleton.f, "\n");
}

void CLog::FLog(const char *s, ...)
{	
	va_list ap;
	va_start(ap, s);
	
	vfprintf(singleton.f, s, ap);

	va_end(ap);

	fprintf(singleton.f, "\n");
}