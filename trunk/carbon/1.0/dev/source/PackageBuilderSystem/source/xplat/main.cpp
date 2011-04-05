

#include "Constants.h"
#include "ContentEncryptor.h"

#include <iostream>

bool initializePBS()
{
	//make a new log file with new name 
	//init logger
	carbonLogger::init(carbonLog::LOG_TRACE, carbonLog::DefaultLogModuleName, carbonLog::DefaultLogPath, OSConst("carbon_PBS.log"));
	carbonLogger::setLogLevel(carbonLog::LOG_TRACE);

	return true;
}


int main(int argc, char *argv[] )
{

	::CoInitialize(NULL);
	initializePBS();

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(argc <2)
	{
		CARBONLOG_ERROR(logger, "[main] : invalid number of arguments");
		return -1;
	}

	std::string xmlPath = argv[1];
	ContentEncryptor _obj;

	bool ret = _obj.run(xmlPath);

//	CoUninitialize();
	return 0;
}

