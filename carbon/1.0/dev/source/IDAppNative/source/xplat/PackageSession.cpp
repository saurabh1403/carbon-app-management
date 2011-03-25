
#include "PackageSession.h"

//TODO:
bool PackageSession::initSession(const OSString &pkgPath)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(isSessionInitialized)
	{
		CARBONLOG_INFO(logger, "[initSession] : Session is already initialized");
		return true;
	}

	//check the licensing and the trial expiry

	//mark entry in db for its start time

	//open the db and check its validity

	//initializes all the function pointers
//	targetFnMap["getPackageContentInfo"] = this->getPackageContentInfo;
//	targetFnMap["clearCachedContent"] = this->clearCachedContent;
//	targetFnMap["getContent"] = this->getContent;

	isSessionInitialized = true;

	return true;

}


bool PackageSession::closeSession()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(!isSessionInitialized)
	{
		CARBONLOG_INFO(logger, "[closeSession] : Package is already closed. Quiting now");
		return true;
	}

	//mark time in db and the license

	//free all memory and clear all temp


	isSessionInitialized = false;


	return true;
}


bool PackageSession::processJob(IDAppNativeJob &job, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isSessionInitialized)
	{
		CARBONLOG_ERROR(logger, "Session is not initialized. Invalid call");
		return false;
	}

	//std::map<std::string, pkgSessionTarget>::const_iterator itr = targetFnMap.find(job.targetObject);
	//if( itr != targetFnMap.end())
	//{
	//	CARBONLOG_DEBUG(logger, "[PackageSession::processJob] : Executing target function "<<job.targetObject);
	//	itr->second(job,resStr);
	//}

	//else
	//	CARBONLOG_ERROR(logger, "[PackageSession::processJob] : target function doesn't exist"<<job.targetObject);

	//ugly.. need to correct it.



	return true;
}


