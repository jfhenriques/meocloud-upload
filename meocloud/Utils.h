#ifndef _UTILS_H_
#define _UTILS_H_


#include <string>

#define MEOCLOUD_OK 0
#define MEOCLOUD_ERROR_HTTP 1
#define MEOCLOUD_EXCEPTION_HTTP 2

typedef const char* c_str;
typedef char* str;


typedef struct FileParts {
	std::string directory;
	std::string filename;

	std::string GetFullName()
	{
		return ( this->directory == "/" ? "/" : ( this->directory + "/" ) ) + this->filename;
	}
} FileParts;

str StrCopy(c_str in);

void GetParts(std::string file, FileParts& parts);
void GetParts(c_str file, FileParts& parts);


#endif /* _UTILS_H_ */