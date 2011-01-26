
#pragma once

#include "Constants.h"
#include <set>
#include <vector>

#include "Log.h"
#include "Utilities.h"
using namespace std;

class TempManager
{

private:
	vector<OSString> tempFolder;
	volatile unsigned int nTempLocations;

#ifdef WIN32
	CRITICAL_SECTION cTempMgrCrit;

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

};

