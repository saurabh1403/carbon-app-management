
#pragma once

#include <iostream>

using namespace std;

namespace carbonConfigurations
{

//Logger Settings
#define USE_LOG_4CXX_LIBRARY	1
#define DEFAULT_LOG_LEVEL		LOG_DEBUG

#define ProductName			"Carbon"
#define kUProductName		_T("Carbon")

#define ProductDataBaseName		"pdb.db"
#define kUProductDataBaseName		_T("pdb.db")


#ifdef WIN32
#define kUCarbonCommonFolderRelPath			_T("\\Carbon\\")
#define CarbonCommonFolderRelPath			"\\Carbon\\"

#endif


}

