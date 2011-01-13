
#pragma once

#include "Constants.h"
#include "BIConstants.h"
#include "MsgQueue.h"

class MsgQueue;

//TODO: add the error string the class for storing all the error messages. 
class BridgeInterface
{

private:

	MsgQueue asyncQueue;
	MsgQueue syncQueue;

	msgQueueSelector queueSelectorFn;

#ifdef WIN32
	CRITICAL_SECTION carbonBridgeCritSec;
	static DWORD WINAPI listenerThread(LPVOID lpParam);
#endif

	void insertInQueue(const BIPacket &inPkt);

public:

	HANDLE readerThread;
	bool shouldContinue;

	BridgeInterface(msgQueueSelector inQueueSelectorFn = NULL);
	~BridgeInterface();

	//for external world
	BridgeInterfaceStatus initBI();
	BridgeInterfaceStatus readAsyncPkt(BIPacket &outPkt);
	BridgeInterfaceStatus readSyncPkt(BIPacket &outPkt);
	BridgeInterfaceStatus writePkt(const BIPacket &inPkt);
	BridgeInterfaceStatus closeBI();

};

