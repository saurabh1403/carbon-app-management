// ApplicationProvisiongLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "Log.h"
#include "SqliteWrapper.h"
#include "AESEncryption.h"

CFB_Mode<AES>::Encryption cfbEncryption;
CFB_Mode<AES>::Decryption cfbDecryption;

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
	tempMap["name"] = "UWA";

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

	//UPDATE QUERY 

	aMapStr upMap, conMap;

	upMap["version"] = "jshdjshds";
	upMap["display_name"] = "retest";

	conMap["name"] = "DWA";
	conMap["type"] = "UI";

	ret = obj.updateQueryWithEqualPredicate(upMap, conMap, "applet_registration_info");
	obj.commit();

}


void test_aes()
{

	AESSecretKeyContainer keyStorage1;
	keyStorage1.initWithRandomKeys();

	AESSecretKeyContainer keyStorage = keyStorage1;

	std::string inStr = "this is such a big and new work and i can live with it";
	std::string cStr, oStr;

	cfbEncryption.SetKeyWithIV(keyStorage.getKey(), keyStorage.getKeyLength(), keyStorage.getIV());

/*	StringSource(
		inStr,
		true,
		new StreamTransformationFilter(
		cfbEncryption,
		new StringSink( cStr )
		) // StreamTransformationFilter
		); // StringSource
*/

	cout<<"\n"<<inStr<<endl;

	cfbDecryption.SetKeyWithIV(keyStorage.getKey(), keyStorage.getKeyLength(), keyStorage.getIV());

	AESWrapper aes_obj(keyStorage1);
//	AESWrapper aes_obj;
	aes_obj.encryptString(inStr, cStr);
	cout<<"\ncipher text is \t"<<cStr;

/*	StringSource(
		cStr,
		true,
		new StreamTransformationFilter(
		cfbDecryption,
		new StringSink( oStr )
		) // StreamTransformationFilter
		); // StringSource
*/

	aes_obj.decryptString(cStr, oStr);
	cout<<"\n\n\nrecovered text is \t"<<oStr<<endl;

}


void test_local_aes()
{
	std::string cipherText;
	std::string recoveredText;

	AutoSeededRandomPool rnd;

	// Generate a random key
	byte * key = new byte[AES::DEFAULT_KEYLENGTH];
	rnd.GenerateBlock(key, AES::DEFAULT_KEYLENGTH);

	// Generate a random IV
	byte * iv = new byte[AES::BLOCKSIZE];
	rnd.GenerateBlock(iv, AES::BLOCKSIZE);

	std::string plainText = "This is such a long and huge hello world and i am going so big now skjfdhk fjd fkjd kfd\nd fhdgf jdhfdj fdf\tjskd skjf \n";
	cout<<"\n plain text is \t"<<plainText<<endl;
	//////////////////////////////////////////////////////////////////////////
	// Encrypt streaming

	CFB_Mode<AES>::Encryption cfbEncryption;
	cfbEncryption.SetKeyWithIV(key, AES::DEFAULT_KEYLENGTH, iv);

	StringSource(
		plainText,
		true,
		new StreamTransformationFilter(
		cfbEncryption,
		new StringSink( cipherText )
		) // StreamTransformationFilter
		); // StringSource

	cout<<"\nencrypted is \t"<<cipherText.c_str()<<endl;
	cout<<"\n encrypted length is \t"<<cipherText.length();

	CFB_Mode<AES>::Decryption cfbDecryption;
	cfbDecryption.SetKeyWithIV(key, AES::DEFAULT_KEYLENGTH, iv);

	StringSource(
		cipherText,
		true,
		new StreamTransformationFilter(
		cfbDecryption,
		new StringSink( recoveredText )
		) // StreamTransformationFilter
		); // StringSource

	cout<<"\ndecrypted is \t"<<recoveredText.c_str()<<endl;

	if(plainText != recoveredText)
		cout<<"\n\n theh text are different\n";

	else
		cout<<"\n\n the texts are same.. Hurray!!!!!!!!!!\n";

	delete [] key;
	delete [] iv;

}


int main()
{

	//std::cout<<"hello world\n";

//	testLog();
//	testDb();
	test_aes();
//	test_local_aes();

//	int i;
//	cin>>i;

	return 0;
}

