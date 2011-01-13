
#pragma once

#include "iostream"


#ifdef WIN32

#define CUSTOMAPI WINAPI
#include <windows.h>

#else


#define CUSTOMAPI

#endif

typedef enum pktType
{
	invalidPkt = -1,
	genericPkt = 0,
	syncPkt = 1,
	asyncPkt = 2,

}pktType;


struct BIPacket
{
	unsigned short int pktSize;
	std::string buffer;		//this will be simply a utf string 

	BIPacket():buffer(""), pktSize(0)
	{
	}
};

//a function which returns true or false depending upon whether the msg should be pushed in Async or Sync Queue.
//sync or async are virtual in case of stdin and stdout communication
typedef pktType(*msgQueueSelector)(const std::string &messageData);

typedef class BridgeInterface BILibrary;
typedef BILibrary* BILibraryRef;

typedef enum kBridgeInterfaceStatus {

	kBridgeInterfaceErrorSentinel=-999,
	kBridgeInterfaceErrorInitInvalidParam = -7,		// Inavlid parameters
	kBridgeInterfaceErrorInitChannel = -6,			// Error in initializing the pipe
	kBridgeInterfaceErrorWrite = -5,					// Error in writing the data in the pipe
	kBridgeInterfaceErrorPipeNotValid = -4,			// if the other process has closed the pipe or the pipe handle is not invalid
	kBridgeInterfaceErrorUnknown = -3,				// some unknown error has occured
	kBridgeInterfaceErrorChannelCorrupt = -2,		// the data on pipe is corrupted and improper
	kBridgeInterfaceErrorPipeWriteCorrupt = -1,		// write buffer value to write failed. Need to close the connection in this case

	kBridgeInterfaceErrorNone = 0,					// Success

	kBridgeInterfaceErrorNoDataPresent = 1,			// no data is present in the Message Queue 
	kBridgeInterfaceErrorDataTooLarge = 2,			// if string buffer is larger than MAX_BUFF_SIZE
	kBridgeInterfaceErrorUnableToSendData = 3,		// sendpacket fails to deliver packet and error is EAGAIN. User can try again sending the data
	kBridgeInterfaceErrorTimeout = 4,				// timeout happens while trying for action
	kBridgeInterfaceErrorPipeAlreadyInitialized = 5	// When init call is made and the pipe was already initialized

}BridgeInterfaceStatus;


