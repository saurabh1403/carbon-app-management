
//conatins the main functionality of the IDApp Native interactivity with the UI

#include "IDAppNative.h"

void processJob(const std::string &jobMsg, std::string &resStr)
{
	//get the session id. if it is null or empty, process it itself. else pass it to process to the asked session
	IDAppNativeJob *obj = new IDAppNativeJob();
	obj->initJob(jobMsg);
	if(obj->isJobForSession)
	{
		SessionMgr<IDAppSession>::getInstance().getSessionWithId(obj->sessionId)->processJob(obj, resStr);
	}

	else
	{

		//here do all the work. Cases are:

		//1. get universal information

		//2. create a package session for IDApp

		//close all the sessions

	}

}


