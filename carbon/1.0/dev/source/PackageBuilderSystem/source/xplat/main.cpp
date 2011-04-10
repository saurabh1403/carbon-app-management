

#include "Constants.h"
#include "ContentEncryptor.h"

#include <iostream>

bool initializePBS()
{
	//make a new log file with new name 
	//init logger
	carbonLogger::init(carbonLog::LOG_TRACE, carbonLog::DefaultLogModuleName, carbonLog::DefaultLogPath, OSConst("carbon_PBS.log"));
	carbonLogger::setLogLevel(carbonLog::LOG_TRACE);
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CARBONLOG_ERROR(logger, "[main] : testing the logging");

	return true;
}


int main(int argc, char *argv[] )
{

	::CoInitialize(NULL);
	initializePBS();

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	std::string xmlPath ;
	if(argc <3)
	{
		xmlPath = "D:\\in.xml";
		CARBONLOG_ERROR(logger, "[main] : invalid number of arguments");
	}
	else
		xmlPath = argv[1];

	CARBONLOG_INFO(logger, "[main] : xml path is " << xmlPath.c_str());
	ContentEncryptor _obj;

	if(argc > 2 && strcmp(argv[2], "-e") == 0)
	{
		CARBONLOG_INFO(logger, "[main] : in encryption mode");
		_obj.init(xmlPath, encryptionMode);
		_obj.runEncryptor();
		_obj.generateOutputXml();
	}

	else
	{
		CARBONLOG_INFO(logger, "[main] : in decryption mode");
//		DebugBreak();
//		MessageBoxA(NULL, "erer", "erer", MB_OK);
		_obj.init(xmlPath, decryptionMode);
		_obj.runDecryptor();
	}

	//	CoUninitialize();
	return 0;
}

