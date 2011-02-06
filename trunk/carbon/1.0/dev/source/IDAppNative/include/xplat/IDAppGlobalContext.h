
#pragma once

#include "Constants.h"
#include "TempManager.h"
#include "BridgeInterface.h"
#include "ThreadPool.h"
#include "Log.h"
#include "NativeJob.h"

#include <map>

class BridgeInterface;
class TempManager;

typedef void(*msgTarget)(const std::string &inMsg, std::string &outMsg);

class IDAppGlobalContext
{

private:
	IDAppGlobalContext();
	~IDAppGlobalContext();

	BridgeInterface biObj;
	TempManager tempMgr;
	ThreadPool tPool;

	bool isIDAppInitialized;
	
	std::map<std::string, msgTarget> targetFnMap;

public:

	static IDAppGlobalContext &getInstance(void);
	bool initIDApp();

	BridgeInterface &getBIObject();
	TempManager &getTempMgr();
	ThreadPool &getThreadPool();

	bool closeIDApp();

	void executeFunctionWithParameters(const IDAppNativeJob &inJob, std::string &outStr);

	void handleIDAppCloseSignal();

	BridgeInterfaceStatus writePktToBi(const std::string &pktMsg);

public:
	volatile bool shouldContinue;

};

