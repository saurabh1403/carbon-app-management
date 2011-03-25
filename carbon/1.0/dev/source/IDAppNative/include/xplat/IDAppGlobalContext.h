
#pragma once

#include "Constants.h"
#include "TempManager.h"
#include "BridgeInterface.h"
#include "ThreadPool.h"
#include "Log.h"
#include "NativeJob.h"
#include "ProductDbWrapper.h"

#include <map>

class BridgeInterface;
class TempManager;

typedef void(*msgTarget)(IDAppNativeJob &inJob, std::string &outMsg);

//whole product level information is stored here.
class IDAppGlobalContext
{

private:
	IDAppGlobalContext();
	~IDAppGlobalContext();

	BridgeInterface biObj;
	TempManager tempMgr;
	ThreadPool tPool;
	PDbWrapper _prodObj;

	bool isIDAppInitialized;
	
	std::map<std::string, msgTarget> targetFnMap;

public:

	static IDAppGlobalContext &getInstance(void);
	bool initIDApp();

	BridgeInterface &getBIObject();
	TempManager &getTempMgr();
	ThreadPool &getThreadPool();
	PDbWrapper &getProdDbHandle();

	bool closeIDApp();

	void executeFunctionWithParameters(IDAppNativeJob &inJob, std::string &outMsg);

	void handleIDAppCloseSignal();

	BridgeInterfaceStatus writePktToBi(const std::string &pktMsg);

public:
	volatile bool shouldContinue;

};

