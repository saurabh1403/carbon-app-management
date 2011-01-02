// ApplicationProvisiongLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "Log.h"
#include "SqliteWrapper.h"
#include "AESEncryption.h"

#include <Windows.h>


using namespace carbonCipherConfiguration;
using namespace carboncipherUtilities;

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


void test_aes(const std::string inStr, std::string &oStr)
{

	AESSecretKeyContainer keyStorage1;
	keyStorage1.initWithRandomKeys();

	std::string cStr;	
	cout<<"\n"<<inStr<<endl;

	AESWrapper aes_obj(keyStorage1);
//	AESWrapper aes_obj;
	aes_obj.encryptString(inStr, cStr);
	cout<<"\ncipher text is \t"<<cStr;

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

	cout<<"\n\n the algorithm name is \t"<<cfbEncryption.AlgorithmName()<<endl;

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


void test_aes_file()
{
	SYSTEMTIME st;
	AESSecretKeyContainer keyStorage1;
	keyStorage1.initWithRandomKeys();

	AESWrapper aes_obj(keyStorage1);

	string inFile = "d:\\test1.avi";
	string outFile = "d:\\test1_enc";
	string recoverdFile = "d:\\test11-op.avi";
	//OSString inFile = L"D:\\test1.avi";
	//OSString outFile = L"D:\\test1_enc";
	//OSString recoverdFile = L"D:\\test11-op.avi";

	GetSystemTime(&st);
	printf("Encryptoin started at ********\n");
	printf("%d : %d : %d : %d \n" ,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

	aes_obj.encryptFile(inFile, outFile);

	GetSystemTime(&st);
	printf("Decryptoin started at ********  \n");
	printf("%d : %d : %d : %d \n" ,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

	aes_obj.decryptFile(outFile, recoverdFile);

	GetSystemTime(&st);
	printf("Decryptoin finished at ********  \n");
	printf("%d : %d : %d : %d \n" ,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
	
}

int main()
{

	//std::cout<<"hello world\n";

//	testLog();
//	testDb();
	std::string oStr;
//	test_aes("this is such a big and new work and i can live with it", oStr);
//	test_local_aes();
	test_aes_file();

//	int i;
//	cin>>i;

	return 0;
}

