#pragma once

#include "Defines.h"

class CHighResTimer
{
public:
	bool32 Init();
	void Start();
	void Stop();

	double GetMicroseconds();
	double GetMilliseconds();
	double GetSeconds();

private:
	double freq;
	double beginning;
	double end;

	bool running;
};