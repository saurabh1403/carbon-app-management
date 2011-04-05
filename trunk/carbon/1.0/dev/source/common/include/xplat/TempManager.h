
#pragma once

#include "Constants.h"
#include <set>
#include <vector>

#include "Log.h"
#include "Utilities.h"
using namespace std;

//_TODO: make a templatized class to allow string as well as osstring capability
class TempManager
{

private:
	vector<OSString> tempFolder;
	volatile unsigned int nTempLocations;

#ifdef WIN32
	CRITICAL_SECTION cTempMgrCrit;

	vector <std::string> sTempFolder;

#else

#endif

public:

	TempManager(unsigned int nCountLocation  = 1);
	~TempManager();

	//it just returns the file name and not creates the file. fileName is just a guid
	bool getNewTempFilePath(OSString &tempfolderPath, OSString &tempFileName);

	//it returns the temp folder location only
	bool getTempFolderPath(OSString &tempPath);

	//it deletes all the content but not the folders
	bool deleteAllTempContent();

	//deletes every temp folder
	bool clearTemp();

#ifdef WIN32	

	//it just returns the file name and not creates the file. fileName is just a guid
	bool getNewTempFilePath(std::string &tempfolderPath, std::string &tempFileName);

	//it returns the temp folder location only
	bool getTempFolderPath(std::string &tempPath);

#endif

};

