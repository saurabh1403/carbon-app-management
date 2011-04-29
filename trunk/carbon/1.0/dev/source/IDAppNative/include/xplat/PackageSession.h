
#pragma once

#include "Constants.h"
#include "NativeJob.h"
#include "NativeSession.h"
#include "TempManager.h"
#include "EDMM.h"
#include "SqliteWrapper.h"
#include "AESEncryption.h"
#include <map>

#include "LicenseManager.h"


//************some constants related to package session and package data db*****************

#define GenericContentTypeString			"Generic-content"
#define ZeroLevelContentTypeString			"0-Level-content"
#define OneLevelContentTypeString			"1-Level-content"
#define TwoLevelContentTypeString			"2-Level-content"
#define ThreeLevelContentTypeString			"3-Level-content"

#define ContentDetailKeyName				"contentDetail"

#define ContentIdentifierTagName			OSConst("identifier")
#define ContentTypeTagName					OSConst("type")

#define SecretInfoFileKeyName				"KeyFile"

#define kResourceDataKeyName				"resourceData"
#define kUIDataKeyName						"uiData"


//**********************************************************************

//*****************constants related to xml (argument as well as present in db)*******************

#define ContentFileXquery					OSConst("/argumentXml/contentFile")
#define ContentTypeXquery					OSConst("/argumentXml/type")
#define contentInfoNodesXquery				OSConst("/contents/content")



#define ResourceFileNodeName				"resourceFiles"
#define UIDataNodeName						"uiData"
#define ContentTypeNodeName					"contentType"
#define InfoXmlNodeName						OSConst("infoXmlPath")
#define ResourceBaseFolderConst				"resourceBaseFolder"
//***************************************************************************

struct contentInfo
{
	string contentType;
	OSString contentInfoXmlPath;
};

//_TODO: in all functions of this class, return only the data to the caller and donot contruct the output xml. This introduced coupling between this class and the protocol
class PackageSession
{

private:

	OSString packagePath;
	std::string sPackagePath;
	std::string sPackageId;

	//********************Session related members*************
	volatile bool isSessionInitialized;
	std::map<string, string> contentLocation;			// a map of content name and their path (stored in utf8 format)
	TempManager tempMgr;

	//*******************package related data**********
	std::string pkgDataPath;		//_TODO: populate this to be used as data path instead of just appending the content path to the package path
	std::vector<contentInfo> contentNodes;
	contentInfo resourcesInfo;
	contentInfo uiDataInfo;

	//******************security(encryption) related members************************
	DMM _pkgDb;		//this is not an encrypted db. only base 64 encoded xml would be present here
	AESWrapper _contentDecoder;			//_TODO: in future make this a dynamic logic to detect the type of encryption also

	//******************license related members***************************
	long int startTime;				//this is the time at which the package session is closed. 
	LicenseManager _pkgLicense;

#ifdef WIN32
	CRITICAL_SECTION PackageSessionCritSec;
#else

#endif

private:

	//************private internal function utilities*****************
	//returns true if content is present and its location
	bool isContentPresent(const std::string &contentName, OSString contentPath);

	//adds one more content
	void addContentInfo(const std::string &contentName, const std::string &contentPath);

	//removes content information from the map
	void removeContentInfo(const std::string &contentName);

	//fetches all content information from db and stores in files. 
	bool populatePackageContentFromDb();

	//initializes the aes wrapper by getting the decryption keys from the db
	bool initializeContentDecryptor();

	bool getSecretInfoFileFromDb(std::string &filePath);

	//it fetches the data from the packageContent table. thats all 
	bool populateContentInfoStructFromdb(const string &keyName, contentInfo &outVar);

	//it fetches the xml 
	bool populatePackageContentXmlFromdb();

	bool extractXmlFromDb(const string &keyName, OSString &xmlPath);

public:

	PackageSession():isSessionInitialized(false)
	{

	}

	~PackageSession()
	{
		closeSession();
	}

	//here will be all functions will be present inside this class and will be called from UI. //all output string will be utf8 string

	//implement this function in a thread safe manner
	//clears all the temp content
	bool clearCachedContent(std::string &resStr);

	//it decrypts the content and pass the path to the UI. The extracted output file is also stored in a hidden manner
	bool getContent(const std::string &argXml, std::string &resStr);

	//it simply returns the package content information
	bool getPackageContentInfo(std::string &resStr);		

	//no need of this function now.
	bool processJob(IDAppNativeJob &job, std::string &resStr);

	bool initSession(const OSString &pkgPath, const std::string pkgId, std::string &resStr);

	bool closeSession();

};

