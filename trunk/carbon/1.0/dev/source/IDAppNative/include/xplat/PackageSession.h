
#pragma once

#include "Constants.h"
#include "NativeJob.h"
#include "NativeSession.h"
#include "TempManager.h"
#include <map>

//TODO: leave this method
class PackageSession;
typedef void(PackageSession::*pkgSessionTarget)(IDAppNativeJob &inJob, std::string &outMsg);


class PackageSession:public NativeSession
{

private:

	volatile bool isSessionInitialized;
	OSString packagePath;
	volatile std::map<string, string> contentLocation;			// a map of content name and their path (stored in utf8 format)
//	std::map<std::string, pkgSessionTarget> targetFnMap;		// map of function names and their pointers
	TempManager tempMgr;

#ifdef WIN32
	CRITICAL_SECTION IDAppCritSec;

#else

#endif


private:
	//here will be all functions will be present inside this class and will be called from UI. //all output string will be utf8 string

	//TODO: implement this function in a thread safe manner
	//clears all the temp content
	void clearCachedContent(IDAppNativeJob &job, std::string &resStr);

	//it decrypts the content and pass the path to the UI. The extracted output file is also stored in a hidden manner
	void getContent(IDAppNativeJob &job, std::string &resStr);

	//************private internal function utilities*****************
	//returns true if content is present and its location
	bool isContentPresent(const std::string &contentName, OSString contentPath);

	//adds one more content
	void addContentInfo(const std::string &contentName, const std::string &contentPath);

	//removes content information from the map
	void removeContentInfo(const std::string &contentName);

public:

	PackageSession()
	{

	}

	~PackageSession()
	{
		closeSession();
	}

	//fetches all content information from db and stores in files. 
	void getPackageContentInfo(IDAppNativeJob &job, std::string &resStr);		

	//no need of this function now.
	bool processJob(IDAppNativeJob &job, std::string &resStr);

	bool initSession(const OSString &pkgPath);

	bool closeSession();


};

