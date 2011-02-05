
#include "ThreadPool.h"
#include "Utilities.h"
#include "IDAppNative.h"
#include "IDAppGlobalContext.h"

ThreadPool::ThreadPool():shouldContinue(false),m_hThreads(NULL), nThreadsRunning(0)
{

}


ThreadPool::~ThreadPool()
{
	//blocking here
	closeAllThreads();
}


bool ThreadPool::initWithThreads(unsigned int nthreads)
{
	//in case, previously threads are opened.
	closeAllThreads();

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	unsigned int nCores = carbonUtilities::cuGetNumberOfProcessors();
	nthreads = (nthreads > nCores) ? nCores: nthreads;

	//fallback in case of any error.
	nthreads = (nthreads <=0)? 2: nthreads;

	shouldContinue = true;
	CARBONLOG_INFO(logger, "[initWithThreads] : Number of threads to spawn are "<<nthreads);

	m_hThreads = new HANDLE[nthreads];
	for(unsigned int i =0; i< nthreads ;i++)
	{
		DWORD threadID = 0;
		m_hThreads[i] = CreateThread(NULL, 0, &(ThreadPool::WorkerThread), this, 0, &threadID);
		if(m_hThreads[i] != NULL)
			nThreadsRunning++;

		else
			CARBONLOG_ERROR(logger, "[initWithThreads] : Threads "<<i<<" spawing failed");

	}

	CARBONLOG_INFO(logger, "[initWithThreads] : Threads spawned completed");

	return (nThreadsRunning == 0)? false : true;
}


//a synchronous blocking call call
bool ThreadPool::closeAllThreads()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//wait for all threads closure
	if(m_hThreads == NULL || nThreadsRunning == 0)
	{
		CARBONLOG_INFO(logger, "[closeAllThreads] : There is no thread to close. ");
		return true;
	}

	shouldContinue = false;
	WaitForMultipleObjects(nThreadsRunning, m_hThreads, true, INFINITE);
	CARBONLOG_INFO(logger, "[closeAllThreads] : Threads are now closed.");

	delete [] m_hThreads;
	m_hThreads = NULL;

	return true;
}


bool ThreadPool::getJobMsg(std::string &jobMsg)
{
	//it will only return the async job messages
	BIPacket pkt;

	if(IDAppGlobalContext::getInstance().getBIObject().readAsyncPkt(pkt) == kBridgeInterfaceErrorNoDataPresent)
		return false;

	else
	{
		jobMsg.assign(pkt.buffer);
		return true;
	}

	return true;

}

void ThreadPool::writeJobOutput(const std::string &outStr)
{
	//this check is provided so that any empty length message should not be sent. 
	//It is not necessary that every job should return some message. 
	//Either the job can sent the msg itself or there is no need to send any msg. The design is flexible.

	if(outStr.empty())
		return;

	BIPacket pkt;
	pkt.buffer = outStr;
	pkt.pktSize = outStr.length();

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(IDAppGlobalContext::getInstance().getBIObject().writePkt(pkt) != kBridgeInterfaceErrorNone)
	{
		CARBONLOG_ERROR(logger, "[writeJobOutput] : Failed to send packet to UI");
	}

}


DWORD WINAPI ThreadPool::WorkerThread(LPVOID lpParam)
{
	ThreadPool *obj = (ThreadPool *)lpParam;
	std::string jobMsg, jobOutput;
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CARBONLOG_INFO(logger, "[WorkerThread] : starting worker thread now..");

	while(obj->shouldContinue)
	{
		if(obj->getJobMsg(jobMsg))
		{
			CARBONLOG_DEBUG(logger, "[WorkerThread] : Processing job ..");
			processJob(jobMsg,jobOutput);
			obj->writeJobOutput(jobOutput);
		}

		else
			Sleep(10);

	}

	CARBONLOG_INFO(logger, "[WorkerThread] : exiting worker thread now..");

	return 0;

}

