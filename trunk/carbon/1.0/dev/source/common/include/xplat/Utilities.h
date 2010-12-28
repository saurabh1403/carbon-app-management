
#pragma once

#include "Constants.h"

#ifdef WIN32
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#endif
namespace carbonUtilities
{

	bool cuFileExists(const OSString &filePath);




}


