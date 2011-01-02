

#include "AESEncryption.h"

using namespace carbonCipherConfiguration;
using namespace carboncipherUtilities;

//it makes a default key and then uses it.
AESWrapper::AESWrapper(AESAlgoCode aesAlgo)
{
	keyStorage = new AESSecretKeyContainer();
	keyStorage->initWithRandomKeys();
	initializeCiphersFromKeys();
	isAESinitialized = true;
}


//this is the most preferred and recommended way to use it.
AESWrapper::AESWrapper(const AESSecretKeyContainer &inKeyMgr, AESAlgoCode aesAlgo)
{
	if(inKeyMgr.isKeyValid())
	{
		keyStorage = new AESSecretKeyContainer(inKeyMgr);
		initializeCiphersFromKeys();
		isAESinitialized = true;	
	}

	else
	{
		isAESinitialized = false;
		keyStorage = NULL;
	}
}


AESWrapper::~AESWrapper()
{
	if(keyStorage && isAESinitialized)
	{
		delete keyStorage;
		keyStorage = NULL;
		isAESinitialized = false;
	}
}

void AESWrapper::initializeCiphersFromKeys()
{
	//keys must be proper, otherwise, it will throw exception later
	cfbEncryptor.SetKeyWithIV(this->keyStorage->getKey(), this->keyStorage->getKeyLength(), this->keyStorage->getIV());
	cfbDecryptor.SetKeyWithIV(this->keyStorage->getKey(), this->keyStorage->getKeyLength(), this->keyStorage->getIV());
}

bool AESWrapper::encryptBytes(const byte *inStr, size_t byteSize, std::string &outStr)
{
	if(!isAESinitialized)
		return false;

	StringSource(
		inStr,
		byteSize,
		true,
		new StreamTransformationFilter(
		cfbEncryptor,
		new StringSink( outStr )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}

bool AESWrapper::encryptString(const std::string &inStr, std::string &outStr)
{
	if(!isAESinitialized)
		return false;

	StringSource(
		inStr,
		true,
		new StreamTransformationFilter(
		cfbEncryptor,
		new StringSink( outStr )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}


bool AESWrapper::encryptFile(const OSString &inFilePath, const OSString &outFilePath)
{
	if(!isAESinitialized)
		return false;

	FileSource(
		inFilePath.c_str(),
		true,
		new StreamTransformationFilter(
		cfbEncryptor,
		new FileSink( outFilePath.c_str() )
		) // StreamTransformationFilter
		); // StringSource


	return true;
}

bool AESWrapper::decryptBytes(const byte *inStr, size_t byteSize, std::string &outStr)
{ 
	if(!isAESinitialized)
		return false;

	StringSource(
		inStr,
		byteSize,
		true,
		new StreamTransformationFilter(
		cfbDecryptor,
		new StringSink( outStr )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}

bool AESWrapper::decryptString(const std::string &inStr, std::string &outStr)
{
	if(!isAESinitialized)
		return false;

	StringSource(
		inStr,
		true,
		new StreamTransformationFilter(
		cfbDecryptor,
		new StringSink( outStr )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}

bool AESWrapper::decryptFile(const OSString &inFilePath, const OSString &outFilePath)
{
	if(!isAESinitialized)
		return false;

	FileSource(
		inFilePath.c_str(),
		true,
		new StreamTransformationFilter(
		cfbDecryptor,
		new FileSink( outFilePath.c_str() )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}

#ifdef WIN32

bool AESWrapper::encryptFile(const std::string &inFilePath, const std::string &outFilePath)
{
	if(!isAESinitialized)
		return false;

	FileSource(
		inFilePath.c_str(),
		true,
		new StreamTransformationFilter(
		cfbEncryptor,
		new FileSink( outFilePath.c_str() )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}

bool AESWrapper::decryptFile(const std::string &inFilePath, const std::string &outFilePath)
{
	if(!isAESinitialized)
		return false;

	FileSource(
		inFilePath.c_str(),
		true,
		new StreamTransformationFilter(
		cfbDecryptor,
		new FileSink( outFilePath.c_str() )
		) // StreamTransformationFilter
		); // StringSource

	return true;
}

#endif



