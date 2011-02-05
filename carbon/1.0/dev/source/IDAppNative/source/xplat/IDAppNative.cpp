
//conatins the main functionality of the IDApp Native interactivity with the UI

#include "IDAppNative.h"

void processJob(const std::string &jobMsg, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//get the session id. if it is null or empty, process it itself. else pass it to process to the asked session
	IDAppNativeJob obj;
	obj.initJob(jobMsg);
	if(obj.isJobForSession)
	{
		PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(obj.sessionId);
		if(pkgSession != NULL)
			pkgSession->processJob(obj, resStr);
		else
			CARBONLOG_ERROR(logger, "No Session exists for the session id "<<obj.sessionId);
	}

	else
	{
		IDAppGlobalContext::getInstance().executeFunctionWithParameters(obj, resStr);

		//here do all the work. Cases are:

		//1. get universal information

		//2. create a package session for IDApp

		//close all the sessions

	}

}


void startPackageSession(const std::string &inMsg, std::string &outMsg)
{

}

void closePackageSession(const std::string &inMsg, std::string &outMsg)
{


}

void getAvailablePackages(const std::string &inMsg, std::string &outMsg)
{

}

