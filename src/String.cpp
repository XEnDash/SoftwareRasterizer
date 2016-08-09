#include "String.h"

bool StringCopy(char *dest, char *src)
{
    if(!dest || !src)
		return false;

    int count = 0;

    while(*src != 0)
    {
		*dest = *src;
		dest++;
		src++;

		count++;
		if(count >= 65536)
			break;
    }

	*dest = 0;

    return true;
}
