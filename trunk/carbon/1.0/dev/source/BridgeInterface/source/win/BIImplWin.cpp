
#include "BridgeInterface.h"
#include <io.h>
#include <fcntl.h>
#include <fstream>

#if DEBUG_MODE_LOGGING_ENABLE == TRUE

std::fstream _fWrite(DEBUG_FILE_PATH, std::ios::out);
#define writeLog(msg)		_fWrite.write(msg, sizeof(msg));	\
	_fWrite.flush();

#else

#define writeLog(msg)			

#endif

//mSizeBuff is the number of objects which is to be read. so, its basically the number of bytes to read
static bool BIRawReadTry(BridgeInterface *obj, byte * buffer, int mSizeBuff)
{
	DWORD dwRead = 0;
	size_t nCount = 0;
	int nReadCount = 0;

	int numBytesRead = 0;
	while(obj->shouldContinue && mSizeBuff)
	{	
		nCount = fread(buffer + numBytesRead, sizeof(byte), mSizeBuff, stdin);

		if ( ferror( stdin ))
		{
//			file<<"some error in stream reading\r\n";
//			perror("read failed");
//			exit(1);
			return false;
		}

		if (nCount == 0)				//No data available on the pipe
		{
			Sleep(10);
			continue;
		}
		else if (nCount != mSizeBuff)
		{
			numBytesRead += nCount;
			mSizeBuff -= nCount;

			//the object is so big
			if(nReadCount > 100)
			{
				return false;
				//return 0;
			}
			nReadCount++;
//			Sleep(10);
			continue;
		}

		break;
	}

	return true;
}

DWORD WINAPI BridgeInterface::listenerThread(LPVOID lpParam)
{
	BridgeInterface * obj = (BridgeInterface*)lpParam;

	union BIPacketMeta
	{
		unsigned short int size;
		byte size_buf[sizeof(short int)];
	}pktMeta;

	while(obj->shouldContinue)
	{
		DWORD dwRead = 0;
		bool isDataRead = false;

		while(obj->shouldContinue && !isDataRead)
		{
			if(!BIRawReadTry(obj,(byte *)(pktMeta.size_buf), sizeof(pktMeta.size_buf)))
				return 0;
			isDataRead = true;
		}

		isDataRead = false;
		char * buffer = NULL;

		while(obj->shouldContinue && !isDataRead)
		{
			buffer = new char[pktMeta.size + 1];
			if(!BIRawReadTry(obj,(byte *)buffer, pktMeta.size))
			{
				delete [] buffer;
				return 0;
			}
			buffer[pktMeta.size] = '\0';
			isDataRead = true;
		}

		BIPacket mPacket;
		mPacket.pktSize = pktMeta.size;
		mPacket.buffer = buffer;

		if(buffer)
			delete [] buffer;

//		memcpy(&mPacket.pktInfo.PacketID,(const void*)&mPacketInfo.PacketID,sizeof(mPacketInfo.PacketID));	
//		memcpy(&mPacket.pktInfo.PacketType,(const void*)&mPacketInfo.PacketType,sizeof(mPacketInfo.PacketType));	
//		mPacket.pktInfo.size = (szBuffer.size() + 1) * sizeof(NativeChar);						//size() returns length in chars

		obj->insertInQueue(mPacket);
	}

	return 0;
}



void BridgeInterface::insertInQueue(const BIPacket &inPkt)
{
	if(queueSelectorFn == NULL)
	{
		//put in Async queue by default
		asyncQueue.push(inPkt);
		return;
	}

	int inPktType = (*queueSelectorFn)(inPkt.buffer);

	switch(inPktType)
	{
	case invalidPkt:
		break;
	case syncPkt:
		syncQueue.push(inPkt);
		break;
	case asyncPkt:
		asyncQueue.push(inPkt);
		break;
	default:
		break;
	}

}


BridgeInterface::BridgeInterface(msgQueueSelector inQueueSelectorFn):queueSelectorFn(inQueueSelectorFn)
{
	InitializeCriticalSection(&carbonBridgeCritSec);
	readerThread = NULL;
	shouldContinue = false;
}

BridgeInterface::~BridgeInterface()
{
	closeBI();

}

BridgeInterfaceStatus BridgeInterface::initBI()
{
	EnterCriticalSection(&carbonBridgeCritSec);

	if(readerThread != NULL)
	{
		LeaveCriticalSection(&carbonBridgeCritSec);
		return kBridgeInterfaceErrorPipeAlreadyInitialized;
	}

	_setmode( _fileno( stdin ), _O_BINARY );
	_setmode( _fileno( stdout ), _O_BINARY );

	shouldContinue = true;

	asyncQueue.clearQueue();
	syncQueue.clearQueue();

	DWORD threadID = 0;
	readerThread = CreateThread(NULL, 0, &(BridgeInterface::listenerThread), this, 0, &threadID);

	if(readerThread == NULL)
	{
		shouldContinue = false;
		closeBI();
		LeaveCriticalSection(&carbonBridgeCritSec);
		return kBridgeInterfaceErrorInitChannel;
	}

	writeLog("bi got init properly\n");
	LeaveCriticalSection(&carbonBridgeCritSec);
	return kBridgeInterfaceErrorNone;
}


BridgeInterfaceStatus BridgeInterface::readAsyncPkt(BIPacket &outPkt)
{
	if(!asyncQueue.pop(outPkt))
	{
		return kBridgeInterfaceErrorNoDataPresent;
	}

	return kBridgeInterfaceErrorNone;
}


BridgeInterfaceStatus BridgeInterface::readSyncPkt(BIPacket &outPkt)
{
	if(!syncQueue.pop(outPkt))
	{
		return kBridgeInterfaceErrorNoDataPresent;
	}

	return kBridgeInterfaceErrorNone;
}

static BridgeInterfaceStatus BIRawWrite(BridgeInterface *obj,const byte * buffer, int mBufSize)
{
	int mByteWritten = 0;
	int nCount = 0;
	while(obj->shouldContinue)
	{
		int dwSent;

		dwSent = fwrite(buffer + mByteWritten, sizeof(byte), mBufSize, stdout);
		fflush(stdout);
		if(ferror( stdout )|| (dwSent > mBufSize))
		{
			writeLog("pipe corrupt for writing\n");
			return kBridgeInterfaceErrorPipeWriteCorrupt;
		}

		if(dwSent < mBufSize)
		{
			mByteWritten += dwSent;
			mBufSize -= dwSent;
			if(nCount > 5000)
			{
				writeLog("Enough tries to write. 5000 +. Failed to write\n");
				return kBridgeInterfaceErrorUnableToSendData;
			}
			nCount++;
			Sleep(10);
			continue;
		}
		break;
	}

	return kBridgeInterfaceErrorNone;
}


BridgeInterfaceStatus BridgeInterface::writePkt(const BIPacket &inPkt)
{
//	writeLog("before entering crit section\n");

	EnterCriticalSection(&carbonBridgeCritSec);
	writeLog("Entered critical section for writing pkt\n");
	Sleep(1);						//Adding a delay of 1 ms to prevent flooding of pipe with data

	//TODO: construct the whole logic for constructing the sending buffer string with length prepended
	std::string bufToSend;
	bufToSend = inPkt.buffer;

	BridgeInterfaceStatus biStatus = BIRawWrite(this, (byte *)bufToSend.c_str(), bufToSend.length());
	writeLog("exiting crit section for writing. successful write\n");

	LeaveCriticalSection(&carbonBridgeCritSec);

	return biStatus;
}

//TODO: really optimize it. terminate thread is so ugly. 
// try to use async stdin read using ReadFile api of windows. 
BridgeInterfaceStatus BridgeInterface::closeBI()
{
	if(readerThread)
	{
		shouldContinue = false;
		TerminateThread(readerThread, 0);
		WaitForSingleObject(readerThread, INFINITE);
		CloseHandle(readerThread);
	}
	readerThread = NULL;

	return kBridgeInterfaceErrorNone;
}
