
#include "EDMM.h"

bool EDMM::init(const OSString &dbPath, bool forceEncrypt)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	int retVal;

	retVal = _dmmObj.init(dbPath);

	if(retVal != SQLITE_OK)
	{
		CARBONLOG_FATAL(logger, _T("[EDMM::init] : Failed to initialized pdb present at ")<<dbPath.c_str());
		CARBONLOG_FATAL(logger, _T("[EDMM::init] : Error returned to open pdb is ")<<retVal<<"  and error message is "<<_dmmObj.errMsg.c_str());
		return retVal;
	}

	isInitialized = true;

	std::string errMsg;

	if(!initializeAESWrapper( errMsg, forceEncrypt))
	{
//		CARBONLOG_FATAL(logger, "[EDMM::init] : db is not properly encrypted with the proper keys - "<<errMsg.c_str());
		isInitialized = false;
		return false;
	}

	return true;

}

bool EDMM::initializeAESWrapper(string &errMsg, bool forceEncrypt)
{
	if(!isInitialized)
	{
		errMsg = "db is not initialized";
		return false;
	}

	//TODO: get the keys stream and then initialize the aes wrapper

	return true;

}

