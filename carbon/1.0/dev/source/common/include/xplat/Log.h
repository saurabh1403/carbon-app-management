//Add the copyright information

#pragma once


#ifdef WIN32
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#endif

#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"
#include <log4cxx/simplelayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/helpers/transcoder.h>

#include "Constants.h"

#include <iostream>

using namespace log4cxx;
using namespace log4cxx::helpers;

//some carbon log related definitions and constants
namespace carbonLog{

	enum carbonLogLevel{
		LOG_FATAL = 0,
		LOG_ERROR,
		LOG_WARN,
		LOG_INFO,
		LOG_DEBUG,
		LOG_TRACE
	};

	OSString getTempPath();

	const OSString DefaultLogModuleName = OSConst("carbon");
	const OSString DefaultLogPath = getTempPath();
	const OSString DefaultLogFileName = OSConst("carbon.log");
	const carbonLogLevel DefaultLogLevel = LOG_INFO;

	const OSString LogLevelCheckFileName = OSConst("carbon_trace.txt");		//keep it txt file so that user don't get confused about how to create it. 

}


//A singleton class to create the logger instances depending on the options. 

//TO think: extend this class to return multiple loggerPtr objects depending on the loggername. 
//It will be a kind of singleton + factory design pattern

class carbonLogger
{

public:

	static bool init(carbonLog::carbonLogLevel logLevel = carbonLog::DefaultLogLevel, 
		const OSString &logModuleName = carbonLog::DefaultLogModuleName, 
		const OSString &logPath = carbonLog::DefaultLogPath,
		const OSString &logFileName = carbonLog::DefaultLogFileName);

	static void setLogLevel(carbonLog::carbonLogLevel logLevel);

	static LoggerPtr getLoggerPtr();

private:

	carbonLogger();
	~carbonLogger();

	static LoggerPtr _logger;

};

namespace carbonFileUtilities
{



}

