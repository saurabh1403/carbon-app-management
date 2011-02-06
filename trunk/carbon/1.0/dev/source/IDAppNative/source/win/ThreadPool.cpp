
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


bool ThreadPool::initWithThreads()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	unsigned int nCores = carbonUtilities::cuGetNumberOfProcessors();

	shouldContinue = true;
	CARBONLOG_INFO(logger, "[initWithThreads] : Number of threads to spawn are "<<nCores);

	return initWithThreads(nCores);

}

bool ThreadPool::initWithThreads(unsigned int nthreads)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(nthreads > 10)
	{
		CARBONLOG_ERROR(logger, "[initWithThreads] : Too many threads asked to start - " <<nthreads);
		return false;
	}

	if(nthreads == 0)
	{
		CARBONLOG_WARN(logger, "[initWithThreads] : 0 threads asked to spawn");
		return true;
	}

	//in case, previously threads are opened.
	closeAllThreads();

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
		CARBONLOG_TRACE(logger, "[closeAllThreads] : There is no thread to close. ");
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

bool ThreadPool::writeJobOutput(const std::string &outStr)
{
	return (IDAppGlobalContext::getInstance().writePktToBi(outStr) == kBridgeInterfaceErrorNone);
}


DWORD WINAPI ThreadPool::WorkerThread(LPVOID lpParam)
{
	ThreadPool *obj = (ThreadPool *)lpParam;

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CARBONLOG_INFO(logger, "[WorkerThread] : starting worker thread now..");

	while(obj->shouldContinue)
	{
		std::string jobMsg, jobOutput;
		if(obj->getJobMsg(jobMsg))
		{
			CARBONLOG_DEBUG(logger, "[WorkerThread] : Processing job ..");
			processJob(jobMsg,jobOutput);
			if(!obj->writeJobOutput(jobOutput))
				CARBONLOG_ERROR(logger, "[WorkerThread] : Failed to write packet to BI");
		}

		else
			Sleep(10);
	}

	CARBONLOG_INFO(logger, "[WorkerThread] : exiting worker thread now..");

	return 0;

}

