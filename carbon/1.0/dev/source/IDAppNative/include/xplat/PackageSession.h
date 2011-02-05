
#pragma once

#include "Constants.h"
#include "NativeJob.h"
#include "NativeSession.h"

//TODO: there must be a default constructor
class PackageSession:public NativeSession
{

private:

#ifdef WIN32
	CRITICAL_SECTION IDAppCritSec;

#else

#endif

public:

	PackageSession()
	{

	}

	~PackageSession()
	{

	}

	bool processJob(const IDAppNativeJob &job, std::string resStr);
	bool initSession();
	bool closeSession();

};

