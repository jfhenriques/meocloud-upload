
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"


using namespace std;


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



void GetParts(c_str file, FileParts& parts)
{
	GetParts(file == NULL ? string() : string(file), parts);
}

void GetParts(string file, FileParts& parts)
{
	size_t lPos = file.find_last_of("/\\");

	if( lPos == string::npos )
	{
		parts.filename = file;
		parts.directory = "/";
	}
	else
	{
		if( (lPos+1) < file.size() )
			parts.filename = file.substr(lPos+1);

		if( lPos == 1 )
			parts.directory = "/";

		else
		{
			string directory = file.substr(0, lPos);

			size_t start = 0,
					end, tSize = directory.length();

			parts.directory = "";

			if( directory[0] == '/' || directory[0] == '\\' )
				start++;

			do
			{
				end = directory.find_first_of("/\\", start);

				if( end == string::npos )
					end = tSize;

				if( (end - start) == 0 )
				{
					start++;
					continue;
				}

				parts.directory += "/" + string(directory, start, end-start);
				start = end + 1;

			} while(end < tSize);
		}
	}
}
