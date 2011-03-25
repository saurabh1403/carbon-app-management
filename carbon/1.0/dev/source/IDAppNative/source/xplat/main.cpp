
#include <iostream>

#include "BridgeInterface.h"
#include "BIConstants.h"
#include "TempManager.h"
#include "IDAppGlobalContext.h"
#include "IDAppNative.h"

using namespace std;

void test_tempMgr()
{
	TempManager obj(2);
	OSString filePath, folderPath;

	obj.getNewTempFilePath(folderPath, filePath);


}


int main(int argc, char *argv[])
{
	//many things to do:
	//1. initialize the global context

//	test_tempMgr();
/*
	BridgeInterface biObj;

	biObj.initBI();

	BIPacket pkt;

	while(true)
	{
		if(biObj.readAsyncPkt(pkt) == kBridgeInterfaceErrorNone)
		{
//			cout<<"buffer is "<<pkt.buffer;
			biObj.writePkt(pkt);
		}
	}

	cout<<"hello world";

	biObj.closeBI();

	return 0;
*/

	CoInitialize(NULL);

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(!IDAppGlobalContext::getInstance().initIDApp())
	{
		CARBONLOG_FATAL(logger, "[IDApp Main] : Failed to initialize IDApp Native. Fatal Error. Quitting Now");
		//TODO: show a native dialogue here saying the package is corrupt
		return -1;
	}

	bool toContinue = true;

	while(toContinue && IDAppGlobalContext::getInstance().shouldContinue)
	{
		BIPacket pkt;
		string resStr;

		if(IDAppGlobalContext::getInstance().getBIObject().readSyncPkt(pkt) == kBridgeInterfaceErrorNone)
		{
			CARBONLOG_DEBUG(logger, "[IDApp Main] : Processing sync job ");
		}
		else if(IDAppGlobalContext::getInstance().getBIObject().readAsyncPkt(pkt) == kBridgeInterfaceErrorNone)
		{
			CARBONLOG_DEBUG(logger, "[IDApp Main] : Processing async job ");
		}
		else
		{
			Sleep(10);
			continue;
		}

		//processing of job 
		processJob(pkt.buffer, resStr);
		if(!resStr.empty())
		{
			CARBONLOG_TRACE(logger, "[IDApp Main] : writing job msg - "<<resStr.c_str());
			if(IDAppGlobalContext::getInstance().writePktToBi(resStr) != kBridgeInterfaceErrorNone)
			{
				CARBONLOG_ERROR(logger, "[IDApp Main] : Failed to write packet through bridge interface. Error Code - ");
				toContinue = false;
			}
		}

	}	//end of while loop

	IDAppGlobalContext::getInstance().closeIDApp();

	CoUninitialize();

	return 0;
}

