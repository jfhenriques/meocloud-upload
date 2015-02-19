#ifndef _UTILS_H_
#define _UTILS_H_



#ifdef __unix__

#define OS_UNIX

#define DEFAULT_CONF_PATH	"/etc/meocloud.conf"
#define CONF_TXT_R "rb"
#define CONF_TXT_W "wb"

#elif defined(_WIN32) || defined(WIN32) 

#define OS_WINDOWS

#include <Windows.h>

#define DEFAULT_CONF_DIR	"\\MeocloudUpload"
#define DEFAULT_CONF_FILE	"\\meocloud.conf"

#define CONF_TXT_R "r"
#define CONF_TXT_W "w"


#endif


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
FILE* GetConfFilePtr(c_str in, bool isWrite = false);


#endif /* _UTILS_H_ */