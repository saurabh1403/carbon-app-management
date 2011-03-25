
#pragma once

#include <string>
#include <map>
#include <vector>
#include "carbonConfigurations.h"


#ifdef WIN32

	#include <tchar.h>
	#include <Windows.h>
	#define OSConst(x)		_T(x)	
	#define OSSlash		OSConst("\\")

	typedef TCHAR OSChar;
	typedef std::wstring OSString;

#else

	#define OSConst(x)		x			
	#define OSSlash		OSConst("/")

	typedef char OSChar;
	tyepdef std::string OSString

#endif


typedef std::map<OSString, OSString> aMapOsStr;
typedef std::map<std::string,std::string> aMapStr;
typedef std::vector<std::string> aVectorStr;
typedef std::vector<OSString> aVectorOsStr;





