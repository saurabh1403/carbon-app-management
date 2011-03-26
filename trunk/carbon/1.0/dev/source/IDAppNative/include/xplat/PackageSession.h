
#pragma once

#include "Constants.h"
#include "NativeJob.h"
#include "NativeSession.h"
#include "TempManager.h"
#include "EDMM.h"
#include "SqliteWrapper.h"
#include "AESEncryption.h"
#include <map>


//************some constants related to package session and package data*****************

#define GenericContentTypeString			"Generic-content"
#define ZeroLevelContentTypeString			"0-Level-content"
#define OneLevelContentTypeString			"1-Level-content"
#define TwoLevelContentTypeString			"2-Level-content"
#define ThreeLevelContentTypeString			"3-Level-content"


#define ResourceFileNodeName				"resourceFiles"
#define UIDataNodeName						"uiData"
#define ContentTypeNodeName					"contentType"
#define InfoXmlNodeName						"infoXmlPath"
//**********************************************************************

struct contentInfo
{
	string contentType;
	string contentInfoXmlPath;
};

class PackageSession:public NativeSession
{

private:

	OSString packagePath;

	//********************Session related members*************
	volatile bool isSessionInitialized;
	std::string sPackagePath;		//in utf-8 format
	volatile std::map<string, string> contentLocation;			// a map of content name and their path (stored in utf8 format)
	TempManager tempMgr;

	//*******************package related data**********
	std::string pkgDataPath; 
	std::vector<contentInfo> contentNodes;
	contentInfo resourcesInfo;
	contentInfo uiDataInfo;

	//******************security(encryption) related members************************
	DMM _pkgDb;		//this is not an encrypted db. only base 64 encoded xml would be present here
	AESWrapper _contentDecoder;			//TODO: in future make this a dynamic logic to detect the type of encryption also



#ifdef WIN32
	CRITICAL_SECTION IDAppCritSec;
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

public:

	PackageSession():isSessionInitialized(false)
	{

	}

	~PackageSession()
	{
		closeSession();
	}

	//here will be all functions will be present inside this class and will be called from UI. //all output string will be utf8 string

	//TODO: implement this function in a thread safe manner
	//clears all the temp content
	bool clearCachedContent(std::string &resStr);

	//it decrypts the content and pass the path to the UI. The extracted output file is also stored in a hidden manner
	bool getContent(const std::string &argXml, std::string &resStr);

	//it simply returns the package content information
	bool getPackageContentInfo(std::string &resStr);		

	//no need of this function now.
	bool processJob(IDAppNativeJob &job, std::string &resStr);

	bool initSession(const OSString &pkgPath);

	bool closeSession();

};

