

#include "CipherUtilities.h"

namespace carboncipherUtilities
{

	using namespace carbonUtilities;
	AESSecretKeyContainer::AESSecretKeyContainer(const AESSecretKeyContainer &aes_obj)
	{
		if(aes_obj.isKeyValid())
		{
			size_t keyLength = aes_obj.getKeyLength();
			key = new byte[keyLength];
			memcpy(key, aes_obj.getKey(), keyLength);
			key_length =keyLength;

			size_t ivLength = aes_obj.get_block_length();
			iv = new byte[ivLength];
			memcpy(iv, aes_obj.getIV(),ivLength );
			block_length =ivLength ;

		}

	}

	void AESSecretKeyContainer::setKey(byte *inKey, size_t keySize)
	{
		key = inKey;
		key_length = keySize;
	}

	void AESSecretKeyContainer::setIV(byte *inIV, size_t ivSize)
	{
		iv = inIV;
		block_length = AES::BLOCKSIZE;
	}


	void AESSecretKeyContainer::initWithRandomKeys()
	{
		resetKeys();

		AutoSeededRandomPool rnd;

		// Generate a random key
		key = new byte[AES::DEFAULT_KEYLENGTH];
		rnd.GenerateBlock(key, AES::DEFAULT_KEYLENGTH);
		key_length = AES::DEFAULT_KEYLENGTH;

		// Generate a random IV
		iv = new byte[AES::BLOCKSIZE];
		rnd.GenerateBlock(iv, AES::BLOCKSIZE);
		block_length = AES::BLOCKSIZE;
	}


	bool AESSecretKeyContainer::initKeysFromKeyStream(const char *keyStream)
	{

		resetKeys();
		int index_iterator = 0;
		int algo_code;

		getIntFromCharArray(algo_code, keyStream, index_iterator, 3);
		index_iterator += 3;

		//here is the protocol
		//first three bytes: 

		int keyLength, ivLength;

		getIntFromCharArray(keyLength, keyStream, index_iterator, 3);
		index_iterator += 3;

		getIntFromCharArray(ivLength, keyStream, index_iterator, 3);
		index_iterator += 3;

		string keyStr64, ivStr64;

		keyStr64.assign(keyStream,index_iterator, keyLength);
		index_iterator += keyLength;

		ivStr64.assign(keyStream, index_iterator, ivLength);
		index_iterator += ivLength;

		//_TODO:
		//put the logic for decoding keys and iv
		string keyStr, ivStr;
		keyStr = keyStr64;
		ivStr = ivStr64;

		byte *key_temp = new byte[keyStr.length()];
		byte *iv_temp = new byte[ivStr.length()];

		memcpy(key_temp, keyStr.c_str(), keyStr.length());
		memcpy(iv_temp, ivStr.c_str(), ivStr.length());

		setKey(key_temp, keyStr.length());
		setIV(iv_temp, ivStr.length());

		return true;

	}


	bool AESSecretKeyContainer::writeKeysIntoStream(std::string &outKeyStream)
	{

		outKeyStream = "";
		char *val;

		//write algo code
		if(!getCharArrayFromInt(&val, AES_CFB, 3))
		{
			delete val;
			return false;
		}
		outKeyStream.append(val);
		delete val;
		val = NULL;

		//write key length
		if(!getCharArrayFromInt(&val, key_length, 3))
		{
			delete val;
			return false;
		}
		outKeyStream.append(val);
		delete val;
		val = NULL;

		//write iv length
		if(!getCharArrayFromInt(&val, block_length, 3))
		{
			delete val;
			return false;
		}
		outKeyStream.append(val);
		delete val;
		val = NULL;

		//_TODO: put the logic to encode the keys
		outKeyStream.append((char*)key, key_length);
		outKeyStream.append((char*)iv, block_length);

		return true;
	}

}