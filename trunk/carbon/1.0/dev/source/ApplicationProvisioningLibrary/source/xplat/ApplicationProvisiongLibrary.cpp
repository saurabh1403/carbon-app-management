// ApplicationProvisiongLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "Log.h"

void testLog()
{
//	carbonLogger::init(carbonLog::LOG_DEBUG, L"my_test_log", L"d:\\test1", L"cardfd.log");
	LoggerPtr logger(carbonLogger::getLoggerPtr());

	carbonLogger::setLogLevel(carbonLog::LOG_INFO);

	LOG4CXX_FATAL(logger, L"fatal .  ");
	LOG4CXX_DEBUG(logger, L"debug logging.  ");

	LOG4CXX_TRACE(logger, L" trace loggigng .  ");

}

int main()
{

	std::cout<<"hello world\n";

	testLog();

	return 0;
}

