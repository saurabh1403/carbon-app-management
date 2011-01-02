

#include "CipherUtilities.h"

namespace carboncipherUtilities
{

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

	//safe coding
	if(key)
		delete [] key;

	if(iv)
		delete [] iv;

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


}