
#pragma once

#include "Constants.h"
#include "BIConstants.h"
#include "MsgQueue.h"

//TODO: disable this logging.
#define DEBUG_MODE_LOGGING_ENABLE		FALSE
#define DEBUG_FILE_PATH				"D:\\carbon_bi_log.log"

#define DISALLOW_COPY_AND_ASSIGN(Type) \
  Type(const Type&);               \
  void operator=(const Type&)

class MsgQueue;

//_TODO: add the error string in the class for storing all the error messages. 
class BridgeInterface
{

private:

	MsgQueue asyncQueue;
	MsgQueue syncQueue;

	msgQueueSelector queueSelectorFn;

#ifdef WIN32
	mutable CRITICAL_SECTION carbonBridgeCritSec;
	static DWORD WINAPI listenerThread(LPVOID lpParam);
#endif

	void insertInQueue(const BIPacket &inPkt);
	DISALLOW_COPY_AND_ASSIGN(BridgeInterface);

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

