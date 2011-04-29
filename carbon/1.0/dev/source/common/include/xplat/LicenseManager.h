
#pragma once

#include "Constants.h"
#include "Log.h"
#include "MachineIdManager.h"
#include "Utilities.h"
#include "SqliteWrapper.h"


//constants related to apd
#define keyFilePathConst			"KeyFilePath"


//this struct contains all the information about the timing of the package license
struct pkgTimeInfo
{
	//TODO: pass time information also. like expiry time, days remaining etc.
	long int closeTime;
	long int startTime;

	long int activationTime;
	long int expiryTime;

};


//this structure contains all the information about the package.
struct PkgInfo
{
	string pkgId;
	OSString pkgPath;
	OSString pkgLicensePath;
	OSString licenseDbPath;

	pkgTimeInfo _pkgTimeInfo;

};

typedef enum LicenseState
{
	LicenseOk = 0,
	LicenseNotValid,
	LicenseExpired,
	LicenseNotActivated,		//not supported now
	LicenseNotValidForPackage,
	LicenseTempered,
	UnknownError

}LicenseState;


class LicenseManager
{

private:
	PkgInfo _pkgInfo;
	bool LoadLicense();
	DMM _dbObj;
	bool isInitialized;
	MachineIdMgr _macIdMgr;

	//license parameters
	OSString keyFilePath;
	OSString machineLicenseFilePath;
	bool isExpiredStatus;
	long int lastLaunchTime;
	long int expiryTime;
	long int activationTime;
	string SN;

	//private functions
	bool validateMachineLicense(const OSString &keyFilePath, const OSString &machineLicensePath);
	bool populateLicenseFields();
	LicenseState validateLicenseTiming();
	bool copyKeyImageFile(const OSString &destFilePath);

public:

	LicenseManager():isInitialized(false)
	{
	}

	~LicenseManager()
	{
	}

	bool init(const PkgInfo &inPkg);

	//done at installation/activation time
	static bool createLicense(const PkgInfo _inPkgInfo);

	//it marks the entry in the db also to signify any license expiry and other cases. 
	LicenseState validateLicense();

	//package calls this to update the license when it is closed
	bool updateLicense(const pkgTimeInfo _inPkgInfo);

};

