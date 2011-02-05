
#include "TempManager.h"

using namespace carbonUtilities;

TempManager::TempManager(unsigned int nCountLocation)
{

#ifdef WIN32
	InitializeCriticalSection(&cTempMgrCrit);
#endif

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(nCountLocation > 10)
		nCountLocation = 10;

	OSString tempLoc, errMsg;
	nTempLocations = 0;

	for(unsigned int i = 0; i < nCountLocation; i++)
	{
		if(!cuCreateTempFolder(tempLoc, errMsg))
			CARBONLOG_FATAL(logger, errMsg);

		tempFolder.push_back(tempLoc);
		nTempLocations++;

	}

}

TempManager::~TempManager()
{
	clearTemp();
	nTempLocations = 0;
}

//it just returns the file name and not creates the file. fileName is just a guid
bool TempManager::getNewTempFilePath(OSString &tempfolderPath, OSString &tempFileName)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(!getTempFolderPath(tempfolderPath))
		return false;

	if(!cuGenerateGUID(tempFileName))
	{
		CARBONLOG_WARN(logger, L"[getNewTempFilePath] : Failed to generate guid for temp file name.");
		return false;
	}

	return true;
}


bool TempManager::getTempFolderPath(OSString &tempPath)
{
	EnterCriticalSection(&cTempMgrCrit);
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(nTempLocations == 0)
	{
		CARBONLOG_WARN(logger, L"[getTempFolderPath] : The number of temp folder is zero.");
		return false;
	}

	else 
		tempPath = tempFolder.at(rand()%nTempLocations);

	LeaveCriticalSection(&cTempMgrCrit);
	return true;
}

//it deletes all the content but not the folders
bool TempManager::deleteAllTempContent()
{
	EnterCriticalSection(&cTempMgrCrit);

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	vector<OSString>::const_iterator itr = tempFolder.begin();

	for(; itr!= tempFolder.end(); itr++)
	{
		if(!cuDeleteDirectoryContent(*itr))
			CARBONLOG_WARN(logger, OSConst("[deleteAllTempContent] : Failed to delete the content of directory - ")<<*itr);
	}

	LeaveCriticalSection(&cTempMgrCrit);
	return true;
}

bool TempManager::clearTemp()
{
	EnterCriticalSection(&cTempMgrCrit);

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	vector<OSString>::const_iterator itr = tempFolder.begin();

	for(; itr!= tempFolder.end(); itr++)
	{
		nTempLocations--;
		if(!cuDeleteDirectory(*itr))
			CARBONLOG_WARN(logger, OSConst("[clearTemp] : Failed to delete the directory - ")<<*itr);
	}
	nTempLocations = 0;

	LeaveCriticalSection(&cTempMgrCrit);

	return true;
}

