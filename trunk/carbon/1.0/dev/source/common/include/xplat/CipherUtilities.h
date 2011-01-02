
#pragma once

#include "Constants.h"
#include "CipherConfigurations.h"

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

	//copy constructor
	AESSecretKeyContainer(const AESSecretKeyContainer &);

	bool isKeyValid() const
	{
		if(key == NULL || iv == NULL || key_length < 1 || block_length < 1)
			return false;

		return true;
	}

	void initWithRandomKeys();

	void setKey(byte *inKey, size_t keySize = AES::DEFAULT_KEYLENGTH);
	void setIV(byte *inIV, size_t ivSize = AES::BLOCKSIZE);
	void setAlgoCode();


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

//init with the stream of encoded key
//currently, the supported algorithm is reading the key xml file which is embedded in an image file using a password protection.
bool getAESKeysFromHermitCrab(const std::string &HermitStream, AESSecretKeyContainer &outKeyContainer );

//generate a key container from a db key stream.
//this db key stream can be anything - key in a hermit crab with password, key in some encoded manner etc. 
//it follows a particular protocol in case of retrieving keys from the db stream
bool getAESKeysFromDbKeyStream(std::string dbKeyStream, AESSecretKeyContainer &outKeyContainer);

//bool initAESKeysWithValues(byte* key, byte*iv, int key_length = AES::DEFAULT_KEYLENGTH, int iv_length = AES::BLOCKSIZE, AESSecretKeyContainer &outKeyContainer);

}