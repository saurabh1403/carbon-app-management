
#pragma once

#include "PackageSession.h"
#include "NativeJob.h"
#include "NativeSession.h"
#include "SessionMgr.h"
#include "IDAppGlobalContext.h"



void processJob(const std::string &jobMsg, std::string &resStr);

void startPackageSession(const std::string &inMsg, std::string &outMsg);

void closePackageSession(const std::string &inMsg, std::string &outMsg);

void getAvailablePackages(const std::string &inMsg, std::string &outMsg);





