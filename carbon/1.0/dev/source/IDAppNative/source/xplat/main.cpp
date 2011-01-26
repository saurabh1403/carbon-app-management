
#include <iostream>

#include "BridgeInterface.h"
#include "BIConstants.h"
#include "TempManager.h"

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

	test_tempMgr();

	BridgeInterface biObj;

	biObj.initBI();

	BIPacket pkt;

	while(true)
	{
		if(biObj.readAsyncPkt(pkt) == kBridgeInterfaceErrorNone)
		{
			biObj.writePkt(pkt);
		}
	}

	cout<<"hello world";

	biObj.closeBI();

	return 0;

}

