
#pragma once

#include <string>
#include "Constants.h"
#include "CipherConfigurations.h"
#include "CipherUtilities.h"

#include "aes.h"
using CryptoPP::AES;
using CryptoPP::SimpleKeyingInterface;

#include "ccm.h"
using CryptoPP::CCM;

#include "cryptlib.h"
using CryptoPP::BufferedTransformation;
using CryptoPP::AuthenticatedSymmetricCipher;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include "files.h"
using CryptoPP::FileSink;
using CryptoPP::FileSource;

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "SecBlock.h"
using CryptoPP::SecByteBlock;

#include "cryptlib.h"
using CryptoPP::Exception;
using CryptoPP::DecodingResult;

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

#include <assert.h>

class carboncipherUtilities::AESSecretKeyContainer;

//currently it supports only CFB mode
//_TODO: add exception to every cipher operation
class AESWrapper
{

public:

	//it makes a default key and then uses it.
	AESWrapper(carbonCipherConfiguration::AESAlgoCode aesAlgo = carbonCipherConfiguration::AES_CFB);

	//this is the most preferred and recommended way to use it.
	AESWrapper(const carboncipherUtilities::AESSecretKeyContainer &inKeyMgr, carbonCipherConfiguration::AESAlgoCode aesAlgo = carbonCipherConfiguration::AES_CFB);

	void initializeCiphersFromKeys(const carboncipherUtilities::AESSecretKeyContainer &aes_key);

	~AESWrapper();

	bool encryptBytes(const byte *inStr, size_t byteSize, std::string &outStr);
	//this function writes the whole encrypted string and sets the length accordingly so that the string doesn't terminate with any null character
	bool encryptString(const std::string &inStr, std::string &outStr);
	bool encryptFile(const OSString &inFilePath, const OSString &outFilePath);

	

	//TODO: decrypt string is a dangerous function. We should deal in char pointers since in string, the charactes are parsed until a null comes.
	// and it is highly probable that in encrypted string null characters are present.
	//before using this function, use another functin which takes byte * as input
	bool decryptBytes(const byte *inStr, size_t byteSize, std::string &outStr);
	bool decryptString(const std::string &inStr, std::string &outStr);
	bool decryptFile(const OSString &inFilePath, const OSString &outFilePath);

#ifdef WIN32
	bool encryptFile(const std::string &inFilePath, const std::string &outFilePath);
	bool decryptFile(const std::string &inFilePath, const std::string &outFilePath);
#endif

private:

	//initialize ciphers from the given key
	void initializeCiphersFromKeys();

	carboncipherUtilities::AESSecretKeyContainer *keyStorage;
	bool isAESinitialized;

	CFB_Mode<AES>::Encryption cfbEncryptor;
	CFB_Mode<AES>::Decryption cfbDecryptor;

};

