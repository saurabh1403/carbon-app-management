
#pragma once

#include "Constants.h"

#ifdef WIN32
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#endif
namespace carbonUtilities
{

	bool cuFileExists(const OSString &filePath);
	bool getIntFromCharArray(int &outVal, const char *in, unsigned int start_index = 0, unsigned int size_chars = 0);

	//chars_count denotes how many chars should be there in the array. if 0 or smaller than required, it would create of appropriate length.
	bool getCharArrayFromInt(char **outArr, unsigned int i, unsigned int chars_count = 0);

	bool cuCreateFileWithData( const void *data, unsigned int length, OSString &filePath);

//	bool readFromFile(const string &infilePath, char *outArr);


}


