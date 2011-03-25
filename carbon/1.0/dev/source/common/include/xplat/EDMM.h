
#pragma once

#include "Constants.h"
#include "SqliteWrapper.h"
#include "CipherConfigurations.h"
#include "CipherUtilities.h"
#include "Log.h"

class AESWrapper;
class AESSecretKeyContainer;

class EDMM
{

public:

	EDMM():aesCipher(NULL)
	{

	}

	~EDMM()
	{

	}

	//force encrypt ensures whether this class should be used only with encrypted data and not with normal data in any case. 
	//in case, it fails to get the keys from the db and forceEncrypt is false, it will operate in normal mode.
	bool init(const OSString &dbPath, bool forceEncrypt = false);
	bool init(const OSString &dbPath, const AESSecretKeyContainer &inKeyContainer);

	bool insertOrReplaceQuery(const aMapStr &keyValue, const string &tableName);
	bool insertQuery(const aMapStr &keyValue, const string &tableName);

	bool insertOrReplaceQuery(const aVectorStr &values, const string &tableName);
	bool insertQuery(const aVectorStr &values, const string &tableName);

	bool getQueryResult(const string &fields, const aMapStr &keyValues, const string &tableName, const char *** outList, const string &predicate = " = ");

	bool deleteRecordquery(const aMapStr &keyValue, const string &tableName, const string &predicate = " = ");

	bool updateQueryWithEqualPredicate(const aMapStr &keyValuesToSet, const aMapStr &keyValueToSearch, const string &tableName, const string &predicate = " = ");

private:

	DMM _dmmObj;
	OSString dbPath;
	bool isInitialized;
	bool isInEncryptedMode;
	AESWrapper *aesCipher;

	//TODO: make  a fn for converting map and vectors into encrypted format.
	bool initializeAESWrapper(string &errMsg, bool forceEncrypt = false);

};

