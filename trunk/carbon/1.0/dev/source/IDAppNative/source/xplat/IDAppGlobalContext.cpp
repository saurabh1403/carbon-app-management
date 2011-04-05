
#include "IDAppGlobalContext.h"
#include "IDAppNative.h"

pktType msgSelecter(const std::string &inMsg)
{

	//TODO: write the logic for detecting the type of packet. Just search for the tag sync and async
	return asyncPkt;
}


IDAppGlobalContext::IDAppGlobalContext():isIDAppInitialized(false), biObj(msgSelecter), shouldContinue(true)
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


void IDAppGlobalContext::handleIDAppCloseSignal()
{
	shouldContinue = false;
}

bool IDAppGlobalContext::initIDApp()
{
	//init logger
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	carbonLogger::setLogLevel(carbonLog::LOG_TRACE);

	targetFnMap.clear();
	targetFnMap["startPackageSession"] = startPackageSession;
	targetFnMap["closePackageSession"] = closePackageSession;
	targetFnMap["getAvailablePackages"] = getAvailablePackages;
	targetFnMap["getPackageSessionData"] = getPackageSessionData;
	targetFnMap["getContent"] = getContent;
	targetFnMap["clearCachedContent"] = clearCachedContent;

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

	//init the product database
	if(!_prodObj.initProductDb())
	{
		CARBONLOG_ERROR(logger, "[initIDApp] : Failed to initialize product Database");
		return false;
	}

	isIDAppInitialized = true;

	CARBONLOG_INFO(logger, "[initIDApp] : IDApp successfully initializaed");

	return true;
}

ThreadPool &IDAppGlobalContext::getThreadPool()
{
	return tPool;
}

PDbWrapper &IDAppGlobalContext::getProdDbHandle()
{
	return _prodObj;

}

//TODO: send the termination packet to UI
bool IDAppGlobalContext::closeIDApp()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	CARBONLOG_TRACE(logger, "Closing thread pool.......");
	tPool.closeAllThreads();

	CARBONLOG_TRACE(logger, "Closing temp manager.......");
	tempMgr.clearTemp();

	CARBONLOG_TRACE(logger, "Closing bridge interface .......");
	biObj.closeBI();

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


void IDAppGlobalContext::executeFunctionWithParameters(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isIDAppInitialized)
		CARBONLOG_WARN(logger, "[executeFunctionWithParameters] : Global Context yet not initialized");

	std::map<std::string, msgTarget>::const_iterator itr = targetFnMap.find(inJob.targetObject);
	if( itr != targetFnMap.end())
	{
		CARBONLOG_DEBUG(logger, "[executeFunctionWithParameters] : Executing target function "<<inJob.targetObject);
		itr->second(inJob,outMsg);
	}

	else
	{
		CARBONLOG_ERROR(logger, "[executeFunctionWithParameters] : target function doesn't exist"<<inJob.targetObject);
		inJob.getErrorXmlString(outMsg, "The target function name doesn't exist.");
	}

}

BridgeInterfaceStatus IDAppGlobalContext::writePktToBi(const std::string &pktMsg)
{
	//this check is provided so that any empty length message should not be sent. 
	//It is not necessary that every job should return some message. 
	//Either the job can sent the msg itself or there is no need to send any msg. The design is flexible.

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isIDAppInitialized)
		CARBONLOG_WARN(logger, "[writePktToBi] : Global Context yet not initialized");

	if(pktMsg.empty())
		return kBridgeInterfaceErrorNone;

	BIPacket pkt;
	pkt.buffer = pktMsg;
	pkt.pktSize = pktMsg.length();

	BridgeInterfaceStatus biStatus = biObj.writePkt(pkt);
	if( biStatus != kBridgeInterfaceErrorNone)
	{
		CARBONLOG_ERROR(logger, "[writePktToBi] : Failed to send packet to UI. Error core is "<<biStatus);
	}

	return biStatus;

}

