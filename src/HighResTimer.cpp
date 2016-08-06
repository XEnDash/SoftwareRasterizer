#include "HighResTimer.h"

#include "Assert.h"
#include <Windows.h>

bool32 CHighResTimer::Init()
{
	LARGE_INTEGER qpf_freq;
	if (!QueryPerformanceFrequency(&qpf_freq))
		return false;

	freq = (double)(qpf_freq.QuadPart);

	running = false;
}

void CHighResTimer::Start()
{
	LARGE_INTEGER qpf_timestamp;
	QueryPerformanceCounter(&qpf_timestamp);

	beginning = (double)(qpf_timestamp.QuadPart);

	running = true;
}

void CHighResTimer::Stop()
{
	Assert(running == false, "HighResTimer isn't running")

	LARGE_INTEGER qpf_timestamp;
	QueryPerformanceCounter(&qpf_timestamp);

	end = (double)(qpf_timestamp.QuadPart);

	running = false;
}

double CHighResTimer::GetMicroseconds()
{
	double diff = (end - beginning) * 1000000.0;
	return diff / freq;
}

double CHighResTimer::GetMilliseconds()
{
	double diff = (end - beginning) * 1000.0;
	return diff / freq;
}

double CHighResTimer::GetSeconds()
{
	double diff = (end - beginning);
	return diff / freq;
}