
#pragma once

#include "Constants.h"
#include "string"

using namespace std;

class INativeJob
{

private:

public:

	// it populates the job parameters and variables using the string coming from UI
	virtual bool initJob(const std::string &inMsgString) = 0;

};


//two types of job. Universal or for a particular session
class IDAppNativeJob:public INativeJob
{
private:

public:

	string targetObject;
	string sessionId;
	bool isJobForSession;

	//parses the xml and populates the parameters
	bool initJob(const std::string &inMsgStr);
};

