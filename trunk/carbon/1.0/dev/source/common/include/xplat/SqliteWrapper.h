
//whole library would be written in UTF8 format. 

//Required: sqlite source code or dll or static lib should be included

#pragma once

#include "Constants.h"
#include <map>
#include <vector>

#include "sqlite3.h"


static const int aBusyTimeoutMs = 2000;
using namespace std;


//TODO: make a memory pool and associate the clean up of that memory pool with the destructor
class DMM
{

public:
	DMM();
	~DMM();

	bool isInitialized;

	int init(const string &dbPath);

#ifdef WIN32
	int init(const OSString &dbPath);
#endif

	virtual bool insertOrReplaceQuery(const aMapStr &keyValue, const string &tableName);
	virtual bool insertQuery(const aMapStr &keyValue, const string &tableName);

	virtual bool insertOrReplaceQuery(const aVectorStr &values, const string &tableName);
	virtual bool insertQuery(const aVectorStr &values, const string &tableName);

	//fields are the comma separated fields or "*" in case of everything
	virtual bool getQueryResult(const string &fields, const aMapStr &keyValues, const string &tableName, const char *** outList, const string &predicate = " = ");
	virtual bool getQueryResult(const string &fields, const aMapStr &keyValues, const string &tableName, vector<string> &outList, const string &predicate = " = ");

	virtual bool deleteRecordquery(const aMapStr &keyValue, const string &tableName, const string &predicate = " = ");

	virtual bool updateQueryWithEqualPredicate(const aMapStr &keyValuesToSet, const aMapStr &keyValueToSearch, const string &tableName, const string &predicate = " = ");

	bool dbClose();

	int queryForList(const char ***outList, const char *inQuery ...);
	int queryNoReturn(const char *inQuery ...);
	int queryForSingle(const char **outValue, const char *inQuery ...);

	bool geterrMsg(string &outErrMsg);

	int commit();
	//TODO: add functions for beginning and closing transactions

	string errMsg;

private:

	sqlite3 *db;
	OSString _dbPath;

	bool prepareAndInsertQuery(const aMapStr &keyValue, const string &tableName, const string &insertString);
	bool prepareAndInsertQuery(const aVectorStr &values, const string &tableName, const string &insertString);

	int prepare(sqlite3_stmt **outStatement, const char *inQuery, va_list args);


};





