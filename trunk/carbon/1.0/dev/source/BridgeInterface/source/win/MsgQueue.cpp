
#include "MsgQueue.h"

MsgQueue::MsgQueue()
{
	InitializeCriticalSection(&carbonCritSec);
}

MsgQueue::~MsgQueue()
{
	DeleteCriticalSection(&carbonCritSec);
}


bool MsgQueue::push(const BIPacket &pkt)
{
	EnterCriticalSection(&carbonCritSec);

	pktQueue.push(pkt);

	LeaveCriticalSection(&carbonCritSec);

	return true;
}


bool MsgQueue::pop(BIPacket &pkt)
{

	EnterCriticalSection(&carbonCritSec);

	if(pktQueue.empty())
	{
		LeaveCriticalSection(&carbonCritSec);
		return false;
	}

	else
	{
		pkt = pktQueue.front();
		pktQueue.pop();
	}

	LeaveCriticalSection(&carbonCritSec);

	return true;
}

bool MsgQueue::clearQueue()
{
	EnterCriticalSection(&carbonCritSec);

	while(!pktQueue.empty())
	{
		pktQueue.pop();
	}

	LeaveCriticalSection(&carbonCritSec);
	return true;
}

