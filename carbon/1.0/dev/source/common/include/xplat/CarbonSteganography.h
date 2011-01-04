
#pragma once

#include "Constants.h"

#include "EasyBMP.h"
using namespace std;
#include <cstdio>
#include <cstdlib>


namespace carbonStegano
{
	//it will overwrite the file at the output location of the file is already present.
	bool hideFileInBmp(const std::string &inFilePath, const std::string &inBmpFilePath, const std::string &outBmpFilePath);
	bool retriveFileFromBmp(const std::string &inBmpFilePath, const std::string &outFilePath);
	bool retriveFileFromBmpStream(const std::string &inBmpFileDataStream, const std::string &outFilePath);
	bool retrieveDataFromBmpStream(const std::string &inBmpFileDatastream, const std::string &outData);
	bool retrieveDataFromBmp(const std::string &inBmpFilePath, std::string &outData);
	bool hideDataInBmp(const std::string &inData, const std::string &inBmpFilePath, const std::string &outBmpFilePath);

#ifdef WIN32
	bool hideFileInBmp(const OSString &infilePath, const OSString &bmpFilePath, const OSString &outBmpFilePath);
	bool retriveFileFromBmp(const OSString &inBmpFilePath, const OSString &outFilePath);

#endif

};

