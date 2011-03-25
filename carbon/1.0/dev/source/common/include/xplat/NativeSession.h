
#pragma once
#include "Constants.h"

class NativeSession
{

private:

public:

	virtual bool processJob(IDAppNativeJob &job, std::string &resStr) = 0;
	virtual bool initSession(const OSString &pkgPath) = 0;
	virtual bool closeSession() = 0;
};

