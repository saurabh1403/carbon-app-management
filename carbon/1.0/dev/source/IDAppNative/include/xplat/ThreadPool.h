
#pragma once

#include "Constants.h"
#include "Log.h"
#include <list>

//_TODO: to pass the vector of funtion pointers here to start the thread with. Currently, it uses the fixed funtions for the thread
class ThreadPool
{

private:
	HANDLE *m_hThreads;
	unsigned int nThreadsRunning;

public:
	ThreadPool();
	~ThreadPool();
	volatile bool shouldContinue;

#ifdef WIN32
	static DWORD WINAPI WorkerThread(LPVOID lpParam);
#endif

	bool initWithThreads();
	bool initWithThreads(unsigned int nthreads);

	//a synchronous blocking call.
	bool closeAllThreads();

	//logic is here to fetch the proper job from proper queue. if job present, it will return true else false
	bool getJobMsg(std::string &jobMsg);

	bool writeJobOutput(const std::string &outStr);

};

