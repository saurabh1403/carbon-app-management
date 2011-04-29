
#pragma once
#include "Constants.h"

class NativeSession
{

private:

public:

	virtual bool processJob(IDAppNativeJob &job, std::string &resStr);
	virtual bool initSession(const OSString &pkgPath) = 0;
};

