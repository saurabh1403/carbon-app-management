// ApplicationProvisiongLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "Log.h"
#include "SqliteWrapper.h"


void testLog()
{
//	carbonLogger::init(carbonLog::LOG_DEBUG, L"my_test_log", L"d:\\test1", L"cardfd.log");
	LoggerPtr logger(carbonLogger::getLoggerPtr());

	carbonLogger::setLogLevel(carbonLog::LOG_INFO);

	LOG4CXX_FATAL(logger, L"fatal .  ");
	LOG4CXX_DEBUG(logger, L"debug logging.  ");

	LOG4CXX_TRACE(logger, L" trace loggigng .  ");

}


void testDb()
{

	DMM obj;
	int retVal = obj.init("d:\\pim.db");

	//insert
	//aVectorStr temp;
	//temp.push_back("2");
	//temp.push_back("name");
	//temp.push_back("saurabh");
	//bool ret = obj.insertOrReplaceQuery(temp, "table1");
	//obj.commit();

	////delete
	//aMapStr tempMap;
	//tempMap["value"] = "zareen";
	//ret = obj.deleteRecordquery(tempMap, "table1");
	//obj.commit();

	//select query

	aMapStr tempMap;
	tempMap["name"] = "UiWA";

	string field = " * ";

	const char **outlist;
	bool ret = obj.getQueryResult(field, tempMap, "applet_registration_info", &outlist);

	int i  = 0;

	while(outlist[i] != NULL)
	{
		cout<<outlist[i];
		cout<<"\n";
		i++;
	}
	obj.commit();

}

int main()
{

	std::cout<<"hello world\n";

//	testLog();
	testDb();

	return 0;
}

