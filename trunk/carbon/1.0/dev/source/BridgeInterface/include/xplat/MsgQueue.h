
#pragma once

#include "BIConstants.h"
#include <iostream>
#include <vector>
#include <queue> 

struct BIPacket;

class MsgQueue
{

private:
	std::queue<BIPacket> pktQueue;

#ifdef WIN32
	CRITICAL_SECTION carbonCritSec;
#endif

public:
	MsgQueue();	
	~MsgQueue();
	bool push(const BIPacket &pkt);
	bool pop(BIPacket &pkt);
	bool clearQueue();
};


