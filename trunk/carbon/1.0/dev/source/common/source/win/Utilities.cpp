

#include "Utilities.h"

namespace carbonUtilities
{

bool cuFileExists(const OSString &filePath)
{
	if(!PathFileExists(filePath.c_str()))
	{
		return false;
	}
	return true;
}

}