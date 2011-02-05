
#include <iostream>

#include "BridgeInterface.h"
#include "BIConstants.h"
#include "TempManager.h"
#include "IDAppGlobalContext.h"

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

	int i;
	cin>>i;


//	IDAppGlobalContext::getInstance().initIDApp();

//	IDAppGlobalContext::getInstance().closeIDApp();

	return 0;

}

