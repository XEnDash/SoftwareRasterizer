#include "MemoryManager.h"

#include "Platform.h"
#include "Log.h"
#include "Assert.h"

char *__file__ = (char *)(-1);
uint32 __line__ = -1;

void *CMemoryManager::Allocate(uint64 size, char *file, uint32 line)
{
	void *output = CPlatform::Allocate(size);
	CLog::FLog("Allocation [ptr=0x%x, size=%i, file=%s, line=%i]\n", output, size, file, line);
	return output;
}

void *CMemoryManager::Reallocate(void *ptr, uint64 size, char *file, uint32 line)
{
	void *output = CPlatform::Reallocate(ptr, size);
	CLog::FLog("Rellocation [ptr=0x%x, size=%i, file=%s, line=%i]\n", output, size, file, line);
	return output;
}

void CMemoryManager::Free(void *ptr, char *file, uint32 line)
{
	CLog::FLog("Free [ptr=0x%x, file=%s, line=%i]\n", ptr, file, line);
	CPlatform::Free(ptr);
}

#ifdef REDIRECT_NEW_DELETE

#undef new
void * operator new(uint64 size, char *file, uint32 line) 
{ 
	Assert(size < 0, "size is negative");
	Assert((file == (char *)(-1) || line == -1), "file or line are wrong")

		void *output = CMemoryManager::Allocate(size, file, line);

	__file__ = (char *)(-1);
	__line__ = -1;

	return output;
};

#undef delete
void operator delete(void *ptr)
{ 
	Assert(ptr == 0, "ptr is zero");
	Assert((__file__ == (char *)(-1) || __line__ == -1), "file or line are wrong")

	CMemoryManager::Free(ptr, __file__, __line__);

	__file__ = (char *)(-1);
	__line__ = -1;
}

/*#undef new[]
void *operator new[](uint64 size, char *file, uint32 line)
{
	Assert(size < 0, "size is negative");
	Assert((file == (char *)(-1) || line == -1), "file or line are wrong")

	void *output = CMemoryManager::Allocate(size, file, line);

	return output;
}

#undef delete[]
void operator delete(void *ptr)
{
	delete(ptr);
}*/

#else

#endif