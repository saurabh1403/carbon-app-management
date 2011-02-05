
#include "IDAppGlobalContext.h"
#include "IDAppNative.h"

pktType msgSelecter(const std::string &inMsg)
{

	//TODO: write the logic for detecting the type of packet. Just search for the tag sync and async
	return asyncPkt;
}


IDAppGlobalContext::IDAppGlobalContext():isIDAppInitialized(false), biObj(msgSelecter)
{

}


IDAppGlobalContext::~IDAppGlobalContext()
{

}


IDAppGlobalContext &IDAppGlobalContext::getInstance(void)
{
	static IDAppGlobalContext _instance;
	return _instance;
}


bool IDAppGlobalContext::initIDApp()
{
	//init logger
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	carbonLogger::setLogLevel(carbonLog::LOG_INFO);

	//init BI
	if(biObj.initBI()!=kBridgeInterfaceErrorNone)
	{
		CARBONLOG_ERROR(logger, "[initIDApp] : Failed to initialize bridge interface");
		return false;
	}

	//init thread pool
	if(!tPool.initWithThreads())
	{
		CARBONLOG_ERROR(logger, "[initIDApp] : Failed to initialize thread pool");
		return false;
	}

	targetFnMap.clear();
	targetFnMap["startPackageSession"] = startPackageSession;
	targetFnMap["closePackageSession"] = closePackageSession;
	targetFnMap["getAvailablePackages"] = getAvailablePackages;

	isIDAppInitialized = true;

	CARBONLOG_INFO(logger, "[initIDApp] : IDApp successfully initializaed");

	return true;
}

ThreadPool &IDAppGlobalContext::getThreadPool()
{
	return tPool;
}

bool IDAppGlobalContext::closeIDApp()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	CARBONLOG_TRACE(logger, "Closing bridge interface .......");
	biObj.closeBI();

	CARBONLOG_TRACE(logger, "Closing temp manager.......");
	tempMgr.clearTemp();

	CARBONLOG_TRACE(logger, "Closing thread pool.......");
	tPool.closeAllThreads();

	return true;
}

BridgeInterface &IDAppGlobalContext::getBIObject()
{

	return biObj;
}


TempManager &IDAppGlobalContext::getTempMgr()
{

	return tempMgr;
}


void IDAppGlobalContext::executeFunctionWithParameters(const IDAppNativeJob &inJob, std::string &outStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	std::map<std::string, msgTarget>::const_iterator itr = targetFnMap.find(inJob.targetObject);
	if( itr != targetFnMap.end())
	{
		CARBONLOG_DEBUG(logger, "Executing target function "<<inJob.targetObject);
		itr->second(inJob.targetArgument, outStr);
	}

	else
		CARBONLOG_ERROR(logger, "target function doesn't exist"<<inJob.targetObject);

}

