
#pragma once

#include "Constants.h"
#include "SqliteWrapper.h"
#include "CipherConfigurations.h"
#include "CipherUtilities.h"

class AESWrapper;
class AESSecretKeyContainer;

class EDMM : public DMM
{

public:

	EDMM():isForcedEncryption(false), aesCipher(NULL)
	{

	}

	~EDMM()
	{

	}

	//force encrypt ensures whether this class should be used only with encrypted data and not with normal data in any case. 
	//in case, it fails to get the keys from the db and forceEncrypt is false, it will operate in normal mode.
	int init(const OSString &dbPath, bool forceEncrypt = false);
	int init(const OSString &dbPath, const AESSecretKeyContainer &inKeyContainer);

	virtual bool insertOrReplaceQuery(const aMapStr &keyValue, const string &tableName);
	virtual bool insertQuery(const aMapStr &keyValue, const string &tableName);

	virtual bool insertOrReplaceQuery(const aVectorStr &values, const string &tableName);
	virtual bool insertQuery(const aVectorStr &values, const string &tableName);

	virtual bool getQueryResult(const string &fields, const aMapStr &keyValues, const string &tableName, const char *** outList, const string &predicate = " = ");

	virtual bool deleteRecordquery(const aMapStr &keyValue, const string &tableName, const string &predicate = " = ");

	virtual bool updateQueryWithEqualPredicate(const aMapStr &keyValuesToSet, const aMapStr &keyValueToSearch, const string &tableName, const string &predicate = " = ");

private:

	OSString dbPath;
	bool isForcedEncryption;
	AESWrapper *aesCipher;

	//make  a fn for converting map and vectors into encrypted format.

};

