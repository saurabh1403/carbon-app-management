// ApplicationProvisiongLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "Log.h"
#include "SqliteWrapper.h"
#include "AESEncryption.h"
#include "CarbonSteganography.h"
#include <Windows.h>
#include "CarbonXmlParser.h"


using namespace carbonCipherConfiguration;
using namespace carboncipherUtilities;
using namespace carbonUtilities;

void testLog()
{
//	carbonLogger::init(carbonLog::LOG_DEBUG, L"my_test_log", L"d:\\test1", L"cardfd.log");
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	carbonLogger::setLogLevel(carbonLog::LOG_INFO);

	CARBONLOG_FATAL(logger, L"fatal .  ");
	CARBONLOG_DEBUG(logger, L"debug logging.  ");
	CARBONLOG_TRACE(logger, L" trace loggigng .  ");

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
	bool ret = obj.getQueryResult(field, tempMap, "package_installation_info", &outlist);

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

	std::string keyStream;
	keyStorage1.writeKeysIntoStream(keyStream);

	AESSecretKeyContainer keyStorage2;
	keyStorage2.initKeysFromKeyStream(keyStream.c_str());

	std::string cStr;	
	cout<<"\n"<<inStr<<endl;

	AESWrapper aes_obj1(keyStorage1);
//	AESWrapper aes_obj;
	aes_obj1.encryptString(inStr, cStr);
	cout<<"\ncipher text is \t"<<cStr;

	AESWrapper aes_obj2(keyStorage2);

	aes_obj2.decryptString(cStr, oStr);
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


void test_aes_file_encrypt()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	SYSTEMTIME st;
	AESSecretKeyContainer keyStorage1;
	keyStorage1.initWithRandomKeys();

	std::string keyStream;
	keyStorage1.writeKeysIntoStream(keyStream);

	CARBONLOG_ERROR(logger, "[test_aes_file_encrypt] : key stream is " << keyStream.c_str());

	string inBmp = "d:\\o1.bmp";
	string outBmp = "d:\\op.bmp";

	bool ret = carbonStegano::hideDataInBmp(keyStream, inBmp, outBmp);

	AESWrapper aes_enc;
	aes_enc.initializeCiphersFromKeys(keyStorage1);

	string inFile = "d:\\test.flv";
	string outFile = "d:\\11";
	//OSString inFile = L"D:\\test1.avi";
	//OSString outFile = L"D:\\test1_enc";
	//OSString recoverdFile = L"D:\\test11-op.avi";

	GetSystemTime(&st);
	printf("Encryptoin started at ********\n");
	printf("%d : %d : %d : %d \n" ,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

	aes_enc.encryptFile(inFile, outFile);

}



void test_aes_file_decrypt()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	SYSTEMTIME st;
	string outBmp = "D:\\test_carbon\\resources\\op.bmp";
//	string outBmp = "D:\\op.bmp";

	string outKeyStream;
	bool ret = carbonStegano::retrieveDataFromBmp(outBmp, outKeyStream);

	CARBONLOG_ERROR(logger, "[test_aes_file_decrypt] : key stream is " << outKeyStream.c_str());

	AESSecretKeyContainer keyStorage2;
	keyStorage2.initKeysFromKeyStream(outKeyStream.c_str());
	
	AESWrapper aes_dec;
	aes_dec.initializeCiphersFromKeys(keyStorage2);

	string outFile = "d:\\test_carbon\\data\\3232";
//	string outFile = "D:\\11";

	string recoverdFile = "d:\\test11-op.flv";
	GetSystemTime(&st);
	printf("Decryptoin started at ********  \n");
	printf("%d : %d : %d : %d \n" ,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

	aes_dec.decryptFile(outFile, recoverdFile);

	GetSystemTime(&st);
	printf("Decryptoin finished at ********  \n");
	printf("%d : %d : %d : %d \n" ,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

}

void test_stegano()
{

	string inFile = "d:\\pim.db";
	string inBmp = "d:\\o1.bmp";
	string outBmp = "d:\\o1-hide.bmp";
	string outFile = "d:\\pim-op.db";

	bool ret;

	//ret = carbonStegano::hideFileInBmp(inFile, inBmp, outBmp);

	//ret = carbonStegano::retriveFileFromBmp(outBmp, outFile);

	string inData = "world sprawiła, że znalazłis \ngood";
	ret = carbonStegano::hideDataInBmp(inData, inBmp, outBmp);

	string outData;
	//ret = carbonStegano::retriveFileFromBmp(outBmp, outFile);

	ret = carbonStegano::retrieveDataFromBmp(outBmp, outData);
	cout<<"\nthe output data is \n"<<outData<<endl;

	if(outData.compare(inData))
		cout<<"\n\ndata differ\n";

	else
		cout<<"data is same\n";

}



void test_utilties()
{
	char *val;
	getCharArrayFromInt(&val, 14031988, 7);
	int i;
	getIntFromCharArray(i, val, 4, 2);
	cout<<"\n val is \t"<<val;
	cout<<"\n\n and int value is \t"<<i<<endl;
	delete val;
}

//TODO: complete the EDMM functionalities
void test_EDMM()
{

	string inBmp = "d:\\o1.bmp";
	string outbmp = "d:\\o1-op.bmp";
	string dbPath = "d:\\test.db";

	//encode the key in the bmp
	AESSecretKeyContainer keyStorage1;
	keyStorage1.initWithRandomKeys();

	std::string keyStream;
	keyStorage1.writeKeysIntoStream(keyStream);
	carbonStegano::hideDataInBmp(keyStream, inBmp, outbmp);

	std::string cStr;	

	string inStr = "thi is a such a really long long f******g word and this is working fine";
	string oStr;
	cout<<"\n"<<inStr<<endl;

	AESWrapper aes_obj1(keyStorage1);
	aes_obj1.encryptString(inStr, cStr);
	cout<<"\ncipher text is \t"<<cStr;

	//decode the key from bmp
	string oukeyStream = "";
	carbonStegano::retrieveDataFromBmp(outbmp, oukeyStream );
//	oukeyStream = keyStream;
	AESSecretKeyContainer keyStorage2;
	keyStorage2.initKeysFromKeyStream(oukeyStream .c_str());

	if(keyStream != oukeyStream)
		cout<<"\n\n keys differ";
	else
		cout<<"\n\n keys are same";

	AESWrapper aes_obj2(keyStorage2);

	aes_obj2.decryptString(cStr, oStr);
	cout<<"\n\n\nrecovered text is \t"<<oStr<<endl;


}

int main()
{

	//std::cout<<"hello world\n";

	::CoInitialize(NULL);

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CarbonXMLParser _xmlObj;
	CARBONLOG_INFO(logger, "[generateOutputXml] : hahah string");

	if(!_xmlObj.initWithXMLString("<xml/>"))
	{
		CARBONLOG_ERROR(logger, "[generateOutputXml] : failed to init with result xml string");
		return false;
	}

//	testLog();
//	testDb();
	std::string oStr;
//	test_aes("this is such a big and new work and i can live with it", oStr);
//	test_local_aes();
//	test_aes_file_encrypt();
	test_aes_file_decrypt();
//	test_stegano();


//	test_utilties();
//	test_EDMM();

//	CoUninitialize();

	return 0;
}

