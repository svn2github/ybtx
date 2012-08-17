
#include <stdlib.h>

unsigned int HashCode(unsigned char* data, unsigned int size)
{
	unsigned int uHash = 0;
	unsigned char* ptr = data;

	unsigned int i = 0;
	for (i=0; i<size; ++i)
	{
		uHash = uHash * 33 + *ptr;
		++ptr;
	}

	return uHash;
}


