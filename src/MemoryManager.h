#pragma once

#include "Defines.h"

// TODO(dShamir): add leak check

class CMemoryManager
{
public:
	static void *Allocate(uint64 size, char *file, uint32 line);
	static void *Reallocate(void *ptr, uint64 size, char *file, uint32 line);
	//void *AllocateAndInit(uint64 size);
	static void Free(void *ptr, char *file, uint32 line);
private:
};

#define lnew(s) CMemoryManager::Allocate(s, __FILE__, __LINE__)
#define lrealloc(p, s) CMemoryManager::Reallocate(p, s, __FILE__, __LINE__)
#define ldelete(ptr) CMemoryManager::Free(ptr, __FILE__, __LINE__)

#ifdef DISALLOW_NEW_AND_DELETE
#define new Assert("new is disallowed..");
#define delete Assert("delete is disallowed..");
#endif