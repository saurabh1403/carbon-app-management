
#pragma once

#include "Constants.h"
#include "Log.h"

#ifdef WIN32
#include <shlwapi.h>
#include<Shlobj.h>
//#pragma comment(lib,"shlwapi.lib")
//#pragma comment(lib,"Shell32.lib")

#endif
namespace carbonUtilities
{

	bool cuFileExists(const OSString &filePath);
	bool getIntFromCharArray(int &outVal, const char *in, unsigned int start_index = 0, unsigned int size_chars = 0);

	//chars_count denotes how many chars should be there in the array. if 0 or smaller than required, it would create of appropriate length.
	bool getCharArrayFromInt(char **outArr, unsigned int i, unsigned int chars_count = 0);

	bool cuCreateFileWithData( const void *data, unsigned int length, OSString &filePath);

	bool cuGetOSStringFromInt(unsigned int input, OSString &outNum);

//	bool readFromFile(const string &infilePath, char *outArr);

#ifdef WIN32
	bool cuGenerateGUID(std::string &outGuid);
#endif

	bool cuGenerateGUID(OSString &outGuid);

	//returns temp folder path
	bool cuGetTempPath(OSString &outStr);

	bool cuCreateTempFolder(OSString &outPath, OSString &errMsg);

	bool cuConvertStringToOSString(const std::string inStr, OSString &outStr);
	bool cuConvertOSStringToString(const OSString &inStr, std::string &outStr);

	bool cuCreateDirectory(const OSString &path);
	bool cuDeleteDirectory(const OSString &path);

	bool cuDeleteDirectoryContent(const OSString &dirPath);

	unsigned int cuGetNumberOfProcessors();

	bool getCarbonCommonFolderPath(OSString &outPath);

	bool testfn(aVectorOsStr temp);

}


