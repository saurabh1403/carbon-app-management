

#include "LicenseManager.h"
#include "carbonSteganography.h"
#include "AESEncryption.h"

//TOOD: remove all damn logs from here

bool LicenseManager::init(const PkgInfo &inPkg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//TODO: for testing
	PkgInfo dummyPkg;
	dummyPkg.licenseDbPath = inPkg.licenseDbPath;
	dummyPkg.pkgId = inPkg.pkgId;
	dummyPkg._pkgTimeInfo.activationTime = 245232;
	dummyPkg._pkgTimeInfo.expiryTime = 4334232;
	dummyPkg._pkgTimeInfo.startTime = 11245232;
	dummyPkg._pkgTimeInfo.closeTime = 245232;
	LicenseManager::createLicense(dummyPkg);
	
	_pkgInfo = inPkg;

	//open the db file
	std::string sDbPath;
	carbonUtilities::cuConvertOSStringToString(inPkg.licenseDbPath, sDbPath);
	if(_dbObj.init(sDbPath))
	{
		CARBONLOG_ERROR(logger, "[LicenseManager::init] : Failed to init the license db");
		return false;
	}

	if(!populateLicenseFields())
	{
		CARBONLOG_ERROR(logger, "[LicenseManager::init] : License is corrupt");
		return false;
	}

	if(!_macIdMgr.Initialize())
	{
		CARBONLOG_ERROR(logger, "[LicenseManager::init] : mac id init failed");
		return false;
	}

	isInitialized = true;

	return true;

}


//it creates a license for the package
//it takes care for the existing package and it overwrites the information
bool LicenseManager::createLicense(const PkgInfo _inPkgInfo)
{
	carbonUtilities::cuCreateCarbonLicenseDirectory();

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//open the db file
	std::string sDbPath;
	carbonUtilities::cuConvertOSStringToString(_inPkgInfo.licenseDbPath, sDbPath);

	DMM _dbObj;
	if(_dbObj.init(sDbPath))
	{
		CARBONLOG_ERROR(logger, "[LicenseManager::init] : Failed to init the license db");
		return false;
	}

	bool shouldCreateNewLicense = true;

	//creates the key and machine id file
	const char **outData;
	OSString keyPath, machineLicensePath;
	
	aMapStr tempArg;
	tempArg["package_id"] = _inPkgInfo.pkgId;

	tempArg["key"] = "KeyFilePath";
	if(_dbObj.getQueryResult("value", tempArg, "l_Data", &outData) && *outData != NULL)
	{
		//TODO: make a check on the file existence of license data also
		shouldCreateNewLicense = false;

		CARBONLOG_INFO(logger, "[createLicense] : package information exists");
		carbonUtilities::cuConvertStringToOSString(*outData, keyPath);

		free((void*)*outData);
		free(outData);

		tempArg["key"] = "machineLicenseFile";
		//this should also exist
		if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData) || *outData == NULL)
		{
			//this is problematic
			//delete the file
			carbonUtilities::cuDeleteFile(keyPath);
			shouldCreateNewLicense = true;
		}
		else
		{
			carbonUtilities::cuConvertStringToOSString(*outData, machineLicensePath);

			if(!carbonUtilities::cuFileExists(machineLicensePath) || !carbonUtilities::cuFileExists(keyPath))
				shouldCreateNewLicense = true;

			free((void*)*outData);
			free(outData);
		}

	}

	//create a new license files here
	if(shouldCreateNewLicense)
	{
		//construct the license path
		carbonUtilities::cuGetWinSpecificFolderPath(keyPath, CSIDL_COMMON_APPDATA);
		keyPath += CarbonAppDataRelativeFolder;
		keyPath += OSSlash;
		
		machineLicensePath = keyPath;

		OSString _guid;
		carbonUtilities::cuGenerateGUID(_guid);
		keyPath += _guid;

		carbonUtilities::cuGenerateGUID(_guid);
		machineLicensePath += _guid;

		//store the key and encrypt it
		MachineIdMgr _mcIdMgr;
		if(!_mcIdMgr.Initialize())
		{
			CARBONLOG_ERROR(logger, "[createLicense] : mc id failed");
			return false;
		}

		std::string macLicXmlStr = "";

		if(!_mcIdMgr.generateMachineIdXmlFile(macLicXmlStr, _inPkgInfo.pkgId))
		{
			CARBONLOG_ERROR(logger, "[createLicense] : mc id 2 failed");
			return false;
		}

		carboncipherUtilities::AESSecretKeyContainer _keyContainer;
		_keyContainer.initWithRandomKeys();

		AESWrapper _encoder;
		_encoder.initializeCiphersFromKeys(_keyContainer);

		string encodedLicense;
		if(!_encoder.encryptString(macLicXmlStr, encodedLicense))
		{
			CARBONLOG_ERROR(logger, "[createLicense] : encode failed");
			return false;
		}

		//store the key
		OSString srcImageFilePath;
		carbonUtilities::getCarbonCommonFolderPath(srcImageFilePath);
		srcImageFilePath += OSSlash;
		srcImageFilePath += licenseImageFileName;

		std::string keyStream;
		_keyContainer.writeKeysIntoStream(keyStream);
		if(!carbonStegano::hideDataInBmp(keyStream, srcImageFilePath, keyPath))
		{
			CARBONLOG_ERROR(logger, "[createLicense] : stegny failed");
			return false;
		}

		//store the license
		if(!carbonUtilities::cuCreateFileWithData(encodedLicense.c_str(), encodedLicense.length(), machineLicensePath))
		{
			CARBONLOG_ERROR(logger, "[createLicense] : file creation failed");
			return false;
		}
	}

	//*********************************marks the state in db***********************

	aMapStr tempVal;
	tempVal["domain"] = "1";
	tempVal["package_id"] = _inPkgInfo.pkgId;

	//machine license
	std::string tempKeyPath, tempMacLicPath;
	carbonUtilities::cuConvertOSStringToString(keyPath, tempKeyPath);
	carbonUtilities::cuConvertOSStringToString(machineLicensePath, tempMacLicPath);

	tempVal["key"] = "KeyFilePath";
	tempVal["value"] = tempKeyPath;
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 1 failed");
		return false;
	}

	tempVal["key"] = "machineLicenseFile";
	tempVal["value"] = tempMacLicPath;
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 2 failed");
		return false;
	}

	//other parameters
	string tempPropVal;
	tempVal["key"] = "lastLaunch";
	carbonUtilities::getStringFromLong(_inPkgInfo._pkgTimeInfo.closeTime, tempPropVal);
	tempVal["value"] = tempPropVal;
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 3 failed");
		return false;
	}

	tempVal["key"] = "expiryTime";
	carbonUtilities::getStringFromLong(_inPkgInfo._pkgTimeInfo.expiryTime, tempPropVal);
	tempVal["value"] = tempPropVal;
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 4 failed");
		return false;
	}

	tempVal["key"] = "activationTime";
	carbonUtilities::getStringFromLong(_inPkgInfo._pkgTimeInfo.activationTime, tempPropVal);
	tempVal["value"] = tempPropVal;
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 5 failed");
		return false;
	}

	tempVal["key"] = "IsActivated";
	tempVal["value"] = "1";
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 6 failed");
		return false;
	}

	tempVal["key"] = "IsExpired";
	tempVal["value"] = "0";
	if(!_dbObj.insertOrReplaceQuery(tempVal, "l_Data"))
	{
		CARBONLOG_ERROR(logger, "[createLicense] : data insertion 7 failed");
		return false;
	}

	_dbObj.commit();
	_dbObj.dbClose();

	return true;

}

//it copies the file 
bool LicenseManager::copyKeyImageFile(const OSString &destFilePath)
{
	OSString srcFilePath;

	carbonUtilities::getCarbonCommonFolderPath(srcFilePath);
	srcFilePath += OSSlash;
	srcFilePath += licenseImageFileName;

	if(!carbonUtilities::cuCopyFile(srcFilePath, destFilePath))
	{
		return false;
	}

	return true;
}

bool LicenseManager::populateLicenseFields()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	aMapStr tempArg;
	tempArg["package_id"] = _pkgInfo.pkgId;

	const char **outData;
	string dataVal;

	//*********** check expiry status*****************
	tempArg["key"] = "IsExpired";
	
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_ERROR(logger, "[populateLicenseFields] : Failed to get data 1 from license db");
		return LicenseNotValid;
	}
	string expiryStatus = *outData;
	isExpiredStatus = (expiryStatus == "1");

	free((void*)*outData);
	free(outData);

	//********** get machine license *******************

	tempArg["key"] = "KeyFilePath";
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_ERROR(logger, "[populateLicenseFields] : Failed to get data 2 from license db");
		return false;
	}

	carbonUtilities::cuConvertStringToOSString(*outData, keyFilePath);
	
	free((void*)*outData);
	free(outData);

	tempArg["key"] = "machineLicenseFile";
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_ERROR(logger, "[populateLicenseFields] : Failed to get data 3 from license db");
		return false;
	}
	
	carbonUtilities::cuConvertStringToOSString(*outData, machineLicenseFilePath);

	free((void*)*outData);
	free(outData);

	//get timing information
	tempArg["key"] = "lastLaunch";
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_ERROR(logger, "[populateLicenseFields] : Failed to get data 4 from license db");
		return false;
	}
	carbonUtilities::getLongFromCharArray(*outData, lastLaunchTime);

	free((void*)*outData);
	free(outData);

	tempArg["key"] = "expiryTime";
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_ERROR(logger, "[populateLicenseFields] : Failed to get data 5 from license db");
		return false;
	}
	carbonUtilities::getLongFromCharArray(*outData, expiryTime);

	free((void*)*outData);
	free(outData);
	
	 
	tempArg["key"] = "activationTime";
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_ERROR(logger, "[populateLicenseFields] : Failed to get data 6 from license db");
		return false;
	}
	carbonUtilities::getLongFromCharArray(*outData, activationTime);

	free((void*)*outData);
	free(outData);

	//get serial number
	tempArg["key"] = "SN";
	if(!_dbObj.getQueryResult("value", tempArg, "l_Data", &outData))
	{
		CARBONLOG_WARN(logger, "[populateLicenseFields] : Failed to get data 7 from license db");
	}
	
	else
	{
		SN = *outData;

		free((void*)*outData);
		free(outData);
	}

	return true;
}

//TODO: mark the expiry state in db also
LicenseState LicenseManager::validateLicense()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//get the different parameters from the db. update the license in case of expiry
	if(!isInitialized)
	{
		CARBONLOG_ERROR(logger, "[validateLicense] : not initialized");
		return LicenseNotValid;
	}

	if(isExpiredStatus)
	{
		CARBONLOG_ERROR(logger, "[validateLicense] : expired");
		return LicenseExpired;
	}

	//check machine id
	if(!validateMachineLicense(keyFilePath, machineLicenseFilePath))
	{
		return LicenseNotValid;
	}

	return validateLicenseTiming();

}

LicenseState LicenseManager::validateLicenseTiming()
{
	return LicenseOk;
}

bool LicenseManager::validateMachineLicense(const OSString &keyFilePath, const OSString &machineLicensePath)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//get the keystream and make key container
	string keyStream;
	string sFilePath;

	carbonUtilities::cuConvertOSStringToString(keyFilePath, sFilePath);
	if(!carbonStegano::retrieveDataFromBmp(sFilePath, keyStream))
	{
		CARBONLOG_ERROR(logger, "[validateMachineLicense] : Failed to get keystream from file");
		return false;
	}
	
	carboncipherUtilities::AESSecretKeyContainer _keyContainer;

	if(!_keyContainer.initKeysFromKeyStream(keyStream.c_str()))
	{
		CARBONLOG_ERROR(logger, "[validateMachineLicense] : Failed to init keys from keystream");
		return false;
	}

	//initialize the aes wrapper with the key container
	AESWrapper _keyDecoder;
	_keyDecoder.initializeCiphersFromKeys(_keyContainer);

	std::string licenceData;
	char *fileData;
	unsigned int num;

	if(!carbonUtilities::readFromFile(machineLicenseFilePath, &fileData, num))
	{
		CARBONLOG_ERROR(logger, "[validateMachineLicense] : Failed to get machine license data.");
		return false;
	}

	std::string decodedLicenseData;

	if(!_keyDecoder.decryptBytes((byte*)fileData, num, decodedLicenseData))
	{
		CARBONLOG_ERROR(logger, "[validateMachineLicense] : Failed to decode.");
		return false;
	}

	free(fileData);

	if(!_macIdMgr.ValidateMachineIdXmlFile(decodedLicenseData, _pkgInfo.pkgId))
	{
		CARBONLOG_ERROR(logger, "[validateMachineLicense] : Failed to validate the machine id xml.");
		return false;
	}

	return true;

}

bool LicenseManager::updateLicense(const pkgTimeInfo _inPkgInfo)
{

	return true;
}

