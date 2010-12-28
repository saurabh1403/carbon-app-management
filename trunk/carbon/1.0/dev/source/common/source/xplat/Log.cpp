
#include "Log.h"

LoggerPtr carbonLogger::_logger = NULL;

using namespace carbonLog;

namespace carbonLog
{
	OSString getTempPath()
	{
		OSChar *tempFolder = new OSChar[1024];
		GetTempPath(1024,tempFolder);
		OSString temp;
		temp.assign(tempFolder);
		delete []tempFolder;
		return temp;
	}
}

carbonLogger::carbonLogger()
{

}


carbonLogger::~carbonLogger()
{
	_logger = NULL;

}

LoggerPtr carbonLogger::getLoggerPtr()
{
	if(_logger == NULL)
		carbonLogger::init();

	return _logger;
}

void carbonLogger::setLogLevel(carbonLogLevel logLevel)
{
	//check for the carbon_trace.txt file in the temp folder

	OSString carbonFileCheck = carbonLog::getTempPath();
	carbonFileCheck = carbonFileCheck + OSSlash;
	carbonFileCheck += carbonLog::LogLevelCheckFileName;

	bool traceFileExists = false;

#ifdef WIN32

	traceFileExists = (TRUE == PathFileExists(carbonFileCheck.c_str()));

#else

#endif

	if(traceFileExists)
	{
		_logger->setLevel(log4cxx::Level::getTrace());
	}

	else
	{

		switch(logLevel)
		{
		case LOG_FATAL:
			_logger->setLevel(log4cxx::Level::getFatal());
			break;
		case LOG_ERROR:
			_logger->setLevel(log4cxx::Level::getError());
			break;
		case LOG_WARN:
			_logger->setLevel(log4cxx::Level::getWarn());
			break;
		case LOG_INFO:
			_logger->setLevel(log4cxx::Level::getInfo());
			break;
		case LOG_DEBUG:
			_logger->setLevel(log4cxx::Level::getDebug());
			break;
		default:
			_logger->setLevel(log4cxx::Level::getTrace());
			break;

		}

	}

}


bool carbonLogger::init(carbonLog::carbonLogLevel logLevel, const OSString &logModuleName, 
						const OSString &logPath, const OSString &logFileName)
{
	try
	{
		_logger = Logger::getLogger(logModuleName);
		_logger->setAdditivity(false);
		OSString logfile = logPath + OSSlash + logFileName;

		std::string patternstring = "[%d{dd MMM yyyy HH:mm:ss}] %c [%-5p] - %m%n";

		log4cxx::LayoutPtr patLay(new log4cxx::PatternLayout(log4cxx::helpers::Transcoder::decode(patternstring.c_str())));
		log4cxx::FileAppenderPtr appender(new log4cxx::FileAppender(patLay, logfile, true, true, 1024));		//make a check to disable buffering

		_logger->setLevel(log4cxx::Level::getTrace());
		_logger->removeAllAppenders();
		_logger->addAppender(appender);			//only one appender is allowed as of now
		setLogLevel(logLevel);

		return true;
	}
	catch(Exception&)
	{
		//take some action and make a default basic log
	}

	return true;
}

