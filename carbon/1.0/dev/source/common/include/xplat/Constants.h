
#pragma once

#include <string>
#include <map>
#include <vector>
#include "carbonConfigurations.h"


#ifdef WIN32

	#include <tchar.h>
	#include <Windows.h>
	#define OSConst(x)		_T(x)	
	#define OSSlash			OSConst("\\")
	#define Slash			"\\"

	typedef TCHAR OSChar;
	typedef std::wstring OSString;

#define EnterPlatformCritSection(x)					EnterCriticalSection(x)
#define	LeavePlatformCritSection(x)					LeaveCriticalSection(x)
						
#else

	#define OSConst(x)		x			
	#define OSSlash		OSConst("/")

#define EnterPlatformCritSection(x)					
#define	LeavePlatformCritSection(x)					
		
	typedef char OSChar;
	tyepdef std::string OSString

#endif


typedef std::map<OSString, OSString> aMapOsStr;
typedef std::map<std::string,std::string> aMapStr;
typedef std::vector<std::string> aVectorStr;
typedef std::vector<OSString> aVectorOsStr;





