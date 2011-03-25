
#include "ProductDbWrapper.h"
#include "Log.h"


bool PDbWrapper::initProductDb()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	OSString dbPath = _T("");
	if(!carbonUtilities::getCarbonCommonFolderPath(dbPath))
	{
		CARBONLOG_FATAL(logger, "[initProductDb] : Failed to get the common folder path. Really Weired Error");
		return false;
	}

	dbPath += kUProductDataBaseName;

	if(_dbObj.init(dbPath))
	{
		CARBONLOG_FATAL(logger, "[initProductDb] : Failed to get the common folder path. Really Weired Error");
		return false;
	}

	isInitialized = true;

	return true;
}

//TODO:
bool PDbWrapper::getAvailablePackages(vector<packageInfo> &outVec)
{

	packageInfo temp;
	temp.name = "hello world";
	temp.pkgId = "dsdfdff";
	temp.titleText = "10 classs";
	temp.imageIconPath = "d drive";

	outVec.push_back(temp);

	return true;

}


//TODO:
bool PDbWrapper::getPackageDirectory(const string &pkgId, OSString &outPath )
{
	outPath = OSConst("d:\\test_carbon");

	return true;
}


//TODO
bool PDbWrapper::getInstalledPackages(vector<packageInfo> &outVec)
{

	packageInfo temp;
	temp.name = "hello world";
	temp.pkgId = "dsdfdff";
	temp.titleText = "10 classs";
	temp.imageIconPath = "d drive";

	outVec.push_back(temp);


	return true;

}

