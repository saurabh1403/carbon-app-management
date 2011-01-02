
#pragma once

#include "Constants.h"

namespace carbonCipherConfiguration
{
	//this algo code is put in the db and it should be in sync with this.
	enum AESAlgoCode
	{
		AES_ECB = 0,
		AES_CFB
	};


	enum EncryptionAlgoCode
	{
		AES_Symmetric = 0,
		RSA_Assymetric = 1,
	};



}

