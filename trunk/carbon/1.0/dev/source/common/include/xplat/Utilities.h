
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

	bool getLongFromCharArray(const char * inStr, long int &outNum);
	bool getStringFromLong(const long int inNum, string &outStr);

	//chars_count denotes how many chars should be there in the array. if 0 or smaller than required, it would create of appropriate length.
	bool getCharArrayFromInt(char **outArr, unsigned int i, unsigned int chars_count = 0);

	bool cuCreateFileWithData( const void *data, unsigned int length, OSString &filePath);

	bool cuGetOSStringFromInt(unsigned int input, OSString &outNum);

	bool readFromFile(const OSString &infilePath, char ** fileData, unsigned int &dataLen);

#ifdef WIN32
	bool cuGenerateGUID(std::string &outGuid);
	bool cuCopyFile(const std::string &infilePath, const std::string &outPath);
#endif

	bool cuGenerateGUID(OSString &outGuid);

	//returns temp folder path
	bool cuGetTempPath(OSString &outStr);

	bool cuCreateTempFolder(OSString &outPath, OSString &errMsg);

	bool cuConvertStringToOSString(const std::string inStr, OSString &outStr);
	bool cuConvertOSStringToString(const OSString &inStr, std::string &outStr);

	bool cuCreateDirectory(const OSString &path);
	bool cuDeleteDirectory(const OSString &path);
	bool cuDeleteFile(const OSString &filePath);

	bool cuDeleteDirectoryContent(const OSString &dirPath);
	bool cuCopyFile(const OSString &infilePath, const OSString &outPath);

	unsigned int cuGetNumberOfProcessors();

	bool getCarbonCommonFolderPath(OSString &outPath);

	//it gets the special folder according to FolderIdentifier. http://msdn.microsoft.com/en-us/library/bb762494(v=vs.85).aspx
	bool cuGetWinSpecificFolderPath(OSString & outPath,int FolderIdentifier );

	bool cuCreateCarbonLicenseDirectory();

	//TODO
	bool cuEncodeFileIntoBase32(const OSString &inFilePath, std::string &outData);
	bool cuDecodeBase32DataIntoFile(const std::string inData, const OSString &outFilePath);

}


