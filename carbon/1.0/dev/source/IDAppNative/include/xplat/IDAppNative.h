
#pragma once

#include "PackageSession.h"
#include "NativeJob.h"
#include "NativeSession.h"
#include "SessionMgr.h"
#include "IDAppGlobalContext.h"

void processJob(const std::string &jobMsg, std::string &resStr);

//These functions can also send the message to UI directly without returning in the last. 
void startPackageSession(IDAppNativeJob &inJob, std::string &outMsg);

void closePackageSession(IDAppNativeJob &inJob, std::string &outMsg);

void getAvailablePackages(IDAppNativeJob &inJob, std::string &outMsg);

