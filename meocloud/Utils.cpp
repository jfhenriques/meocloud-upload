
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"
#include <sstream>

#ifdef OS_WINDOWS
#include <tchar.h>
#include <Shlobj.h>

#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif


using namespace std;


str StrCopy(c_str in)
{
	if( in == NULL )
		return NULL;

	size_t size = 1 + strlen(in);
	char *tmp = new char[size ];

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

#ifdef OS_WINDOWS

static BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		   (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
#endif

#ifdef OS_UNIX

c_str GetUnixDefaultConfFile()
{
	stringstream ss;
	string path;
	uid_t  uid = getuid();

	if (uid == 0)
		ss << "/etc/";

	else
	{
		struct passwd *pw = getpwuid(uid);
		ss << pw->pw_dir << "/.";
	}

	ss << DEFAULT_CONF_FILE;
	path = ss.str();

	return StrCopy(path.c_str());
}

#endif

FILE* GetConfFilePtr(c_str file, bool isWrite)
{

#if defined(OS_WINDOWS)

	if (file != NULL)
		return fopen(file, isWrite ? CONF_TXT_W : CONF_TXT_R);

	FILE *filePtr = NULL;
	wchar_t *localAppData = NULL;
	LPCTSTR wpath = NULL;
	wstringstream ss;
	wstring wspath;

	// Fetch Local App Data folder path.
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &localAppData);

	ss << localAppData << _T(DEFAULT_CONF_DIR);
	wspath = ss.str();
	wpath = wspath.c_str();

	CoTaskMemFree(static_cast<void*>(localAppData));

	if (DirectoryExists(wpath) || CreateDirectory(wpath, NULL) != 0)
	{
		ss << "\\" << _T(DEFAULT_CONF_FILE);

		wspath = ss.str();
		wpath = wspath.c_str();

		filePtr = _wfopen(wpath, isWrite ? _T(CONF_TXT_W) : _T(CONF_TXT_R));
	}

	return filePtr;

#elif defined(OS_UNIX)

	c_str defPath = NULL;

	if( file == NULL )
		defPath = GetUnixDefaultConfFile();

	FILE filePtr = fopen(file == NULL ? defPath : file, isWrite ? CONF_TXT_W : CONF_TXT_R);

	if( defPath != NULL )
		delete[] defPath;

	return filePtr;

#else

	return NULL;
#endif

}
