
#include "ProductDbWrapper.h"
#include "Log.h"
#include "IDAppConstants.h"
#include "CarbonXMLParser.h"


bool PDbWrapper::initProductDb()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	OSString dbPath = _T("");
	if(!carbonUtilities::getCarbonCommonFolderPath(dbPath))
	{
		CARBONLOG_FATAL(logger, "[initProductDb] : Failed to get the common folder path. Really Weired Error");
		return false;
	}

	dbPath += OSSlash;
	dbPath += kUProductDataBaseName;

	if(_dbObj.init(dbPath))
	{
		CARBONLOG_FATAL(logger, "[initProductDb] : Failed to get the common folder path. Really Weired Error");
		return false;
	}

	isInitialized = true;

	return true;
}

//_TODO:
bool PDbWrapper::getAvailablePackages(vector<packageInfo> &outVec)
{
	//no need of this functoin as of now

	packageInfo temp;
	temp.name = "hello world";
	temp.pkgId = "dsdfdff";
	temp.titleText = "10 classs";
	temp.imageIconPath = "d drive";

	outVec.push_back(temp);

	return true;

}


bool PDbWrapper::getPackageDirectory(const string &pkgId, OSString &outPath )
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isInitialized)
	{
		return false;
	}

	const char **pkgDir;
	aMapStr keyVal;
	keyVal["package_id"] = pkgId;
	keyVal["key"] = kPackageDirConst;
	if(!_dbObj.getQueryResult("value", keyVal, "package_data", &pkgDir) || *pkgDir== NULL)
	{
		CARBONLOG_ERROR(logger, "Failed to get the installed dir for package from pdb" <<pkgId);
		return false;
	}

	string sOutPath = *pkgDir;

	carbonUtilities::cuConvertStringToOSString(sOutPath, outPath);

	free((void*)*pkgDir);
	free(pkgDir);

	return true;
}


//_TODO
bool PDbWrapper::getInstalledPackages(vector<packageInfo> &outVec)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isInitialized)
	{
		return false;
	}

	vector<string> installedPkg;
	aMapStr keyVal;
	keyVal["install_state"] = kInstalledState;
	if(!_dbObj.getQueryResult("package_id", keyVal, "packages", installedPkg))
	{
		CARBONLOG_ERROR(logger, "Failed to get the installed packages from pdb");
		return false;
	}

	vector<string>::const_iterator itr = installedPkg.begin();
	for(; itr != installedPkg.end(); itr++)
	{
		packageInfo temp;
		//get other information here

		vector<string> pkgInfo;
		aMapStr _keyVal;
		_keyVal["package_id"] = *itr;
		_keyVal["key"] = "PACKAGEINFO";
		
		if(!_dbObj.getQueryResult("value", _keyVal, "package_data", pkgInfo))
		{
			CARBONLOG_ERROR(logger, "Failed to get the package info from pdb for package - " << itr->c_str());
			continue;
		}

		//TODO: we have got the xml from the db. now process it
		CarbonXMLParser pkgInfoXml;
		if(pkgInfo.size()==0 || !pkgInfoXml.initWithXMLString(pkgInfo[0]))
		{
			CARBONLOG_ERROR(logger, "Failed to init the package info string xml for package - " << itr->c_str());
			continue;
		}

		//******get the package icon information
		pkgInfo.clear();
		_keyVal.clear();
		_keyVal["package_id"] = *itr;
		_keyVal["key"] = kPackageIconConst;

		string pkgIconPath = "";

		if(!_dbObj.getQueryResult("value", _keyVal, "package_data", pkgInfo) || pkgInfo.size()==0 )
		{
			CARBONLOG_ERROR(logger, "Failed to get the package icon info from pdb for package - " << itr->c_str());
		}
		else
			pkgIconPath = pkgInfo[0];

		temp.name = "hello world";
		temp.pkgId = *itr;
		temp.titleText = "hello world";
		temp.imageIconPath = pkgIconPath;			//if icon path is empty, it should show the default icon

		outVec.push_back(temp);

	}

	return true;

}

