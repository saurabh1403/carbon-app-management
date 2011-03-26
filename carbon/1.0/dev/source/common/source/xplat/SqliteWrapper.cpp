
#include "SqliteWrapper.h"

/* A blank list and string.  Return these on various query errors to
minimize crashiness when clients ignore the capsStatus return codes. */
static const char* empty_list = NULL;
static const char empty_string = NULL;

DMM::DMM():db(NULL),isInitialized(false)
{

}

DMM::~DMM()
{
	if(db!= NULL)
		dbClose();
}

//TODO:
#ifdef WIN32
int DMM::init(const OSString &dbPath)
{
	std::string sDbPath;

	// DONOT use utilities function. This class should remain very generic
	int len=WideCharToMultiByte(CP_UTF8,0,dbPath.c_str(),-1,NULL,NULL,NULL,NULL);
	char* path =new char[len+1];
	if(WideCharToMultiByte(CP_UTF8,0,dbPath.c_str(),-1,path,len+1,NULL,NULL)<=0)
	{
		delete [] path;
		return false;
	}

	sDbPath.assign(path);
	delete [] path;
	
	return init(sDbPath);

}

#endif

int DMM::init(const string &dbPath)
{
	// Open the database.
	int sqlrv = sqlite3_open(dbPath.c_str(), &db);
	if (SQLITE_OK != sqlrv)
	{
		errMsg = "Error opening the database - ";
		errMsg += sqlite3_errmsg(db);
		this->db = NULL;
		return sqlrv;
	}

	// Setup the standard timeout busy handler.
	sqlrv = sqlite3_busy_timeout(this->db, aBusyTimeoutMs);
	if (SQLITE_OK != sqlrv)
	{
		errMsg = "Error setting busy timeout:";
		errMsg += sqlite3_errmsg(db);
	}

	isInitialized = true;

	return SQLITE_OK;
}

bool DMM::insertOrReplaceQuery(const aMapStr &keyValue, const string &tableName)
{
	return prepareAndInsertQuery(keyValue, tableName, "INSERT OR REPLACE INTO ");
}

bool DMM::insertQuery(const aMapStr &keyValue, const string &tableName)
{
	return prepareAndInsertQuery(keyValue, tableName, "INSERT INTO ");
}

bool DMM::insertOrReplaceQuery(const aVectorStr &values, const string &tableName)
{
	return prepareAndInsertQuery(values, tableName, "INSERT OR REPLACE INTO ");	
}

bool DMM::insertQuery(const aVectorStr &values, const string &tableName)
{
	return prepareAndInsertQuery(values, tableName, "INSERT INTO ");	
}


bool DMM::prepareAndInsertQuery(const aMapStr &keyValue, const string &tableName, const string &insertString)
{
	if(db==NULL)
	{
		errMsg = "db is not initialized";
		return false;
	}

	string query;
	string column_str, values_str;

	column_str = "(";
	values_str = "VALUES(";

	aMapStr::const_iterator itr = keyValue.begin();
	for(int count=0; itr!= keyValue.end(); itr++, count++)
	{
		if(count > 0)
		{
			values_str += (" , ");
			column_str += (" , ");
		}

		column_str += ("`");
		column_str += itr->first;
		column_str += ("`");

		values_str += ("'");
		values_str += itr->second;
		values_str += ("'");
	}

	column_str += ")";
	values_str += ")";

	query = insertString + " " + tableName + column_str + " " + values_str;

	return (queryNoReturn(query.c_str()) == SQLITE_OK);

}


bool DMM::prepareAndInsertQuery(const aVectorStr &values, const string &tableName, const string &insertString)
{	
	if(db==NULL)
	{
		errMsg = "db is not initialized";
		return false;
	}
	string query;
	string values_str;

	values_str = "VALUES(";

	aVectorStr::const_iterator itr = values.begin();
	for(int count=0; itr!= values.end(); itr++, count++)
	{
		if(count > 0)
		{
			values_str += (" , ");
		}

		values_str += ("'");
		values_str += *itr;
		values_str += ("'");
	}

	values_str += ")";

	query = insertString + " " + tableName + " " + values_str;

	return (queryNoReturn(query.c_str()) == SQLITE_OK);

}

bool DMM::getQueryResult(const string &fields, const aMapStr &keyValues, const string &tableName, const char *** outList, const string &predicate)
{
	if(db==NULL)
	{
		errMsg = "db is not initialized";
		return false;
	}

	string query;

	string condition_str = " WHERE ";
	aMapStr::const_iterator itr = keyValues.begin();
	for(int count = 0;itr != keyValues.end(); itr++, count++)
	{
		if(count > 0)
		{
			condition_str += " AND ";
		}

		condition_str += itr->first;
		condition_str += predicate + "'" + itr->second + "' ";

	}

	query += "SELECT " + fields + " FROM " + tableName + condition_str;

	return (queryForList(outList, query.c_str()) == SQLITE_OK);

}


bool DMM::deleteRecordquery(const aMapStr &keyValue, const string &tableName, const string &predicate)
{
	if(db==NULL)
	{
		errMsg = "db is not initialized";
		return false;
	}

	string query;

	string field_str = " WHERE ";
	aMapStr::const_iterator itr = keyValue.begin();
	for(int count = 0;itr != keyValue.end(); itr++, count++)
	{

		if(count > 0)
		{
			field_str += " AND ";
		}

		field_str += itr->first;
		field_str += predicate + "'" + itr->second + "' ";

	}

	query += "DELETE FROM " + tableName + field_str;

	return (queryNoReturn(query.c_str()) == SQLITE_OK);

}


bool DMM::updateQueryWithEqualPredicate(const aMapStr &keyValuesToSet, const aMapStr &conditionPair, const string &tableName, const string &predicate)
{

	string query;
	string condition_str, field_str;

	field_str = " SET ";
	aMapStr::const_iterator pItr = keyValuesToSet.begin();
	for(int count = 0; pItr != keyValuesToSet.end(); pItr++, count++)
	{

		if(count > 0)
		{
			field_str += " , ";
		}

		field_str += pItr->first;
		field_str += "='" + pItr->second + "'";

	}

	condition_str = " WHERE ";
	aMapStr::const_iterator itr = conditionPair.begin();
	for(int count = 0;itr != conditionPair.end(); itr++, count++)
	{
		if(count > 0)
		{
			condition_str += " AND ";
		}

		condition_str += itr->first;
		condition_str += predicate + "'" + itr->second + "' ";

	}

	query += "UPDATE " + tableName + field_str + condition_str;

	return (queryNoReturn(query.c_str()) == SQLITE_OK);

}


int DMM::queryForList(const char ***outList, const char *inQuery ...)
{
	if (NULL == outList)
		return -1;
	*outList = &empty_list;
	
	int sqlrv = SQLITE_OK;
	sqlite3_stmt *statement = NULL;

	va_list args;
	va_start(args, inQuery);
	int rv = prepare(&statement, inQuery, args);
	va_end(args);
	if (SQLITE_OK != rv)
		return rv;
	
	// Process..
	std::vector<const char*> accumulator;
	bool done = false;
	while (!done)
	{
		sqlrv = sqlite3_step(statement);
		switch (sqlrv)
		{
			case SQLITE_ROW:
				{
					int numColumns = sqlite3_column_count(statement);
					for (int col = 0; col < numColumns; ++col)
					{
						const char *p = reinterpret_cast<const char *>(sqlite3_column_text(statement, col));
						if (p)
						{
							char *np= (char *) malloc((strlen(p) + 1) * sizeof(char));
							strcpy(np, p);
							accumulator.push_back(np);
						}
						else
						{
							accumulator.push_back(NULL);
						}
					}
				}
				break;

			case SQLITE_DONE:
				if (accumulator.size() >= 1)
					rv = SQLITE_OK;
				else
				{
					rv = -1;
				}

				done = true;
				break;
				
			case SQLITE_ERROR:
				// Meaningful errors are reported at finalize, not here.
				break;

			default:
				done = true;
				break;
		}
	}

	// Slap together an array to return.
	if (SQLITE_OK == rv)
	{
		size_t elements = accumulator.size();
		*outList = (const char **) malloc((elements + 1) * sizeof(char*));
		if (NULL == *outList)
		{
			rv = -1;
		}
		else
		{
			for (int i = 0; i < static_cast<int>(elements); i++)
			{			
				(*outList)[i] = accumulator[i];
			}
			(*outList)[elements] = NULL;
		}
	}

	// Cleanup...
	sqlrv = sqlite3_finalize(statement);

	return rv;
}


int DMM::queryForSingle(const char **outValue, const char *inQuery ...)
{
	if (NULL == outValue)
	{
		return -1;
	}

	*outValue = &empty_string;

	int sqlrv = SQLITE_OK;
	sqlite3_stmt *statement = NULL;

	va_list args;
	va_start(args, inQuery);
	int rv = prepare(&statement, inQuery, args);
	va_end(args);
	if (SQLITE_OK != rv)
	{
		return rv;
	}

	// Process..
	*outValue = NULL;

	sqlrv = sqlite3_step(statement);

	switch (sqlrv)
	{
		case SQLITE_ROW:
			{
				const char *p = reinterpret_cast<const char *>(sqlite3_column_text(statement, 0));
				char *np = (char *) malloc((strlen(p) + 1) * sizeof(char));
				strcpy(np, p);
				*outValue = np;
			}
			
			break;
			
		case SQLITE_DONE:
			
			break;
			
		case SQLITE_ERROR:
			// Meaningful errors are reported at finalize, not here.
			break;

		default:
			break;
	}

	// Cleanup...
	sqlrv = sqlite3_finalize(statement);
	
	return sqlrv;
}




int DMM::queryNoReturn(const char *inQuery ...)
{
	int sqlrv = SQLITE_OK;
	sqlite3_stmt *statement = NULL;

	va_list args;
	va_start(args, inQuery);
	int rv = prepare(&statement, inQuery, args);
	va_end(args);
	if (SQLITE_OK != rv)
	{
		return rv;
	}

	// Execute...
	bool done = false;
	while (!done)
	{
		sqlrv = sqlite3_step(statement);
		switch (sqlrv)
		{
			case SQLITE_DONE:
				done = true;
				break;

			case SQLITE_ROW:
				break;

			case SQLITE_ERROR:
				// Meaningful errors are reported at finalize, not here.
				break;

			default:
				done = true;
				break;
		}
	}

	// Cleanup...
	sqlrv = sqlite3_finalize(statement);

	return sqlrv;
}

int DMM::commit()
{
	if (sqlite3_get_autocommit(db))
		return -1; // NO-OP in auto-commit mode

	return queryNoReturn("COMMIT");

}

int DMM::prepare(sqlite3_stmt **outStatement, const char *inQuery, va_list args)
{
	int rv = SQLITE_OK;
	char *q = sqlite3_vmprintf(inQuery, args);

	// If we are in auto-commit mode and this is a mutating operation, start a transaction
	if (sqlite3_get_autocommit(db))
	{
		// Ain't this sophisticated!
		if ((strstr(q, "INSERT ") == q) || (strstr(q, "UPDATE ") == q) || (strstr(q, "DELETE ") == q)) 
		{
			rv = queryNoReturn("BEGIN");
		}
	}

	const char *tail = NULL;
	int sqlrv = sqlite3_prepare(db, q, static_cast<int>(strlen(q)), outStatement, &tail);

	sqlite3_free(q);

	return sqlrv;
}

bool DMM::dbClose()
{
	if (db)
	{
		int rv = sqlite3_close(db);

		db = NULL;
		return (rv == SQLITE_OK);
	}
	return true;
}
