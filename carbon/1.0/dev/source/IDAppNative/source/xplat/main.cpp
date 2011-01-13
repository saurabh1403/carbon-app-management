

#include <iostream>

#include "BridgeInterface.h"
#include "BIConstants.h"

using namespace std;


int main(int argc, char *argv[])
{

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