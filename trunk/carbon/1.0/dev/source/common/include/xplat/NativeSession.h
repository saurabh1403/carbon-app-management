
#pragma once
#include "Constants.h"

class NativeSession
{

private:

public:

	virtual bool processJob(INativeJob *job, std::string resStr) = 0;
	virtual bool initSession() = 0;
	virtual bool closeSession() = 0;
};

