
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"




str StrCopy(c_str in)
{
	if( in == NULL )
		return NULL;

	size_t size = strlen(in);
	char *tmp = new char[size + 1];

	if( tmp != NULL )
	{
		strncpy(tmp, in, size);
		tmp[size] = 0;
	}
	return tmp;
}

