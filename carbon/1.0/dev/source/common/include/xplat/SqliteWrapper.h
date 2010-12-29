
//whole library would be written in UTF8 format. 

//Required: sqlite source code or dll or static lib should be included

#pragma once

#include "Constants.h"
#include <map>
#include <vector>

#include "sqlite3.h"

typedef std::map<OSString, OSString> aMapOsStr;
typedef std::map<std::string,std::string> aMapStr;
typedef std::vector<std::string> aVectorStr;
typedef std::vector<OSString> aVectorOsStr;

static const int aBusyTimeoutMs = 2000;
using namespace std;

class DMM
{

public:
	DMM();
	~DMM();

	//TODO: make it a wstring for the path
	int init(const string &dbPath);

	virtual bool insertOrReplaceQuery(const aMapStr &keyValue, const string &tableName);
	virtual bool insertQuery(const aMapStr &keyValue, const string &tableName);

	virtual bool insertOrReplaceQuery(const aVectorStr &values, const string &tableName);
	virtual bool insertQuery(const aVectorStr &values, const string &tableName);

	virtual bool getQueryResult(const string &fields, const aMapStr &keyValues, const string &tableName, const char *** outList, const string &predicate = " = ");

	virtual bool deleteRecordquery(const aMapStr &keyValue, const string &tableName, const string &predicate = " = ");

	virtual bool updateQueryWithEqualPredicate(const aMapStr &keyValuesToSet, const aMapStr &keyValueToSearch, const string &tableName, const string &predicate = " = ");

	virtual bool dbClose();

	int queryForList(const char ***outList, const char *inQuery ...);
	int queryNoReturn(const char *inQuery ...);
	int queryForSingle(const char **outValue, const char *inQuery ...);

	bool geterrMsg(string &outErrMsg);

	int commit();
	//TODO: add functions for beginning and closing transactions

private:

	sqlite3 *db;
	OSString _dbPath;
	string errMsg;

	bool prepareAndInsertQuery(const aMapStr &keyValue, const string &tableName, const string &insertString);
	bool prepareAndInsertQuery(const aVectorStr &values, const string &tableName, const string &insertString);

	int prepare(sqlite3_stmt **outStatement, const char *inQuery, va_list args);


};





