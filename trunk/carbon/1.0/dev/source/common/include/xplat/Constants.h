
#pragma once

#include <string>
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











