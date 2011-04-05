//Add the copyright information

#pragma once

#include "carbonConfigurations.h"

#ifdef WIN32
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#endif


//TODO: write proper functions in carbonDefaultLogger class
class carbonDefaultLogger
{

};

#pragma comment(lib,"log4cxx.lib")

#if USE_LOG_4CXX_LIBRARY

	#include "log4cxx/logger.h"
	#include "log4cxx/basicconfigurator.h"
	#include "log4cxx/helpers/exception.h"
	#include <log4cxx/simplelayout.h>
	#include <log4cxx/fileappender.h>
	#include <log4cxx/patternlayout.h>
	#include <log4cxx/helpers/transcoder.h>

#define CARBONLOG_FATAL			LOG4CXX_FATAL
#define	CARBONLOG_DEBUG			LOG4CXX_DEBUG
#define	CARBONLOG_TRACE			LOG4CXX_TRACE
#define	CARBONLOG_WARN			LOG4CXX_WARN
#define	CARBONLOG_INFO			LOG4CXX_INFO
#define	CARBONLOG_ERROR			LOG4CXX_ERROR

#define CARBONLOG_CLASS_PTR		LoggerPtr

using namespace log4cxx;
using namespace log4cxx::helpers;


#else


#define CARBONLOG_FATAL(logger, message)
#define	CARBONLOG_DEBUG(logger, message)
#define	CARBONLOG_TRACE(logger, message)
#define	CARBONLOG_WARN(logger, message)
#define	CARBONLOG_INFO(logger, message)
#define	CARBONLOG_ERROR(logger, message)

#define CARBONLOG_CLASS_PTR		carbonDefaultLogger*


#endif


#include "Constants.h"
#include <iostream>

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
	const carbonLogLevel DefaultLogLevel = LOG_DEBUG;

	const OSString LogLevelCheckFileName = OSConst("carbon_trace.txt");		//keep it txt file so that user don't get confused about how to create it. 

}


//A singleton class to create the logger instances depending on the options. 
//_TODO: extend this class to return multiple loggerPtr objects depending on the loggername. 
//It will be a kind of singleton + factory design pattern
//just like session manager class. it also stores the information about the current active session
class carbonLogger
{

public:

	static bool init(carbonLog::carbonLogLevel logLevel = carbonLog::DefaultLogLevel, 
		const OSString &logModuleName = carbonLog::DefaultLogModuleName, 
		const OSString &logPath = carbonLog::DefaultLogPath,
		const OSString &logFileName = carbonLog::DefaultLogFileName);

	static void setLogLevel(carbonLog::carbonLogLevel logLevel);

	static CARBONLOG_CLASS_PTR getLoggerPtr();

private:

	carbonLogger();
	~carbonLogger();

	static CARBONLOG_CLASS_PTR _logger;

};

