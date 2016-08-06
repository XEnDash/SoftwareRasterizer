#pragma once

#include "Log.h"
#include <assert.h>

#define Assert(s, t) if(s) { CLog::FLog("ASSERT: %s [FILE: '%s' LINE: %i]", t, __FILE__, __LINE__); assert(0); }