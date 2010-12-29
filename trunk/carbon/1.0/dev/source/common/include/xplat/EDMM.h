
#pragma once

#include "SqliteWrapper.h"

typedef struct keyContainer
{
	byte *key;

}keyContainer;

class EDMM : public DMM
{

public:

	int init(const OSString &dbPath, bool forceEncrypt = false);

private:

	OSString dbPath;

};

