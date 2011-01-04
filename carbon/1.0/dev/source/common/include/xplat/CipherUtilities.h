
#pragma once

#include "Constants.h"
#include "CipherConfigurations.h"
#include "Utilities.h"

#include "SecBlock.h"
using CryptoPP::SecByteBlock;

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

#include <assert.h>

namespace carboncipherUtilities
{

using namespace carbonCipherConfiguration;

//this class reads the database secret encoded key and then constructs the symmetric AES key, IV vector and other parameters. 
//this class can be extended to any level to retrieve the key from an image and different methods for decoding. 
// the same reverse logic should be present in the db constructor to construct the encoded key
//this key manager should be used only for the db data and not for other encryption methods. 
class AESSecretKeyContainer
{
public:

	AESSecretKeyContainer():key(NULL), iv(NULL), key_length(0), block_length(0)
	{
		//yet to decide if it should generate random keys or not
		//initWithRandomKeys();
	}

	~AESSecretKeyContainer()
	{
		resetKeys();
	}

	//copy constructor
	AESSecretKeyContainer(const AESSecretKeyContainer &);

	void resetKeys()
	{
		if(key)
		{
			delete [] key;
			key = NULL;
		}
		if(iv)
		{
			delete [] iv;
			iv = NULL;
		}
	}

	bool isKeyValid() const
	{
		if(key == NULL || iv == NULL || key_length < 1 || block_length < 1)
			return false;

		return true;
	}

	void initWithRandomKeys();

	//it follows a particular protocol in case of retrieving keys from the stream
	bool initKeysFromKeyStream(const char *keyStream);

	//writing keys to the stream
	bool writeKeysIntoStream(std::string &outKeyStream);

	void setKey(byte *inKey, size_t keySize = AES::DEFAULT_KEYLENGTH);
	void setIV(byte *inIV, size_t ivSize = AES::BLOCKSIZE);
	void setAlgoCode();

	//bool initAESKeysWithValues(byte* key, byte*iv, int key_length = AES::DEFAULT_KEYLENGTH, int iv_length = AES::BLOCKSIZE, AESSecretKeyContainer &outKeyContainer);

	byte* AESSecretKeyContainer::getKey() const
	{
		return key;
	}

	byte *AESSecretKeyContainer::getIV() const
	{
		return iv;
	}

	size_t AESSecretKeyContainer::getKeyLength() const
	{
		return key_length;
	}

	size_t AESSecretKeyContainer::get_block_length() const
	{
		return block_length;
	}

private:

	byte *key;
	byte *iv;

	size_t key_length;
	size_t block_length;		//iv length is equal to block length

};


}