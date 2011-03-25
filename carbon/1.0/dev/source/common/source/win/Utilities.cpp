

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

bool getIntFromCharArray(int &outVal, const char *inArr,unsigned int start_index , unsigned int size_chars)
{
	if(start_index > strlen(inArr) || start_index + size_chars > strlen(inArr))
		return false;

	std::string charBuf;

	if(size_chars == 0)
		charBuf.assign(inArr, start_index, strlen(inArr) - start_index);

	else
		charBuf.assign(inArr, start_index, size_chars);

	outVal = atoi(charBuf.c_str());

	return true;

}


bool getCharArrayFromInt(char **outArr, unsigned int i, unsigned int chars_count)
{
	if(outArr == NULL)
		return false;

	char val[20];
	itoa(i, val, 10);
	unsigned int len_val = strlen(val);

	if(chars_count !=0 && chars_count > len_val)
	{
		*outArr = new char[chars_count + 1];
		memset(*outArr, '0', chars_count);
		memcpy(*outArr + chars_count - len_val, val, len_val);
		*(*outArr + chars_count) = '\0';
	}

	else
	{
		*outArr = new char[len_val + 1];
		memcpy(*outArr, val, len_val);
		*(*outArr + len_val) = '\0';
	}

	return true;
}


bool utilCreateFileWithData( const void *data, int length, OSString &filePath )
{
	HANDLE hOutputFile=::CreateFile(filePath.c_str(),GENERIC_WRITE,FILE_SHARE_READ ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if (INVALID_HANDLE_VALUE == hOutputFile)
	{
		return false;
	}

	DWORD bytesWritten = 0;
	if(WriteFile(hOutputFile,data, length,&bytesWritten,NULL))
	{
//		utilFlushFileBuffers(filePath);
	}
	else
	{
		CloseHandle(hOutputFile);
		return false;
	}
	CloseHandle(hOutputFile);

	return true;
}


bool cuGenerateGUID(std::string &outGuid)
{
	OSString tGUID;
	if(!cuGenerateGUID(tGUID))
		return false;

	if(!cuConvertOSStringToString(tGUID, outGuid))
		return false;

	return true;

}

bool cuGenerateGUID(OSString &outGuid)
{
	long lRetVal = 0;
	GUID *pguid = 0x00;
	pguid = new GUID;
	lRetVal=CoCreateGuid(pguid);
	HRESULT hr = NOERROR;
	OLECHAR szCLSID[MAX_PATH];
    hr = StringFromGUID2((*pguid), szCLSID, MAX_PATH);
	delete pguid;
	outGuid = szCLSID;
	return true;
}

bool cuConvertStringToOSString(const std::string inStr, OSString &outStr)
{
	int length= MultiByteToWideChar(CP_UTF8,0,inStr.c_str(),-1,NULL,NULL);
	TCHAR * temp = (TCHAR*)malloc(sizeof(TCHAR) * (length+1));
	if(!temp)
	{
		return false;
	}

	if(MultiByteToWideChar(CP_UTF8,0,inStr.c_str(),-1,temp,length)<=0)
	{
		free(temp);
		return false;
	}
	temp[length] = L'\0';
	outStr.assign(temp);
	free(temp);
	return true;
}


bool cuConvertOSStringToString(const OSString &inStr, std::string &outStr)
{
	int len=WideCharToMultiByte(CP_UTF8,0,inStr.c_str(),-1,NULL,NULL,NULL,NULL);
	char* path =new char[len+1];
	if(WideCharToMultiByte(CP_UTF8,0,inStr.c_str(),-1,path,len+1,NULL,NULL)<=0)
	{
		delete [] path;
		return false;
	}

	outStr.assign(path);
	delete [] path;
	return true;
}

bool cuGetTempPath(OSString &outStr)
{
	OSChar *tempFolder = new OSChar[1024];
	GetTempPath(1024,tempFolder);
	OSString temp;
	outStr.assign(tempFolder);
	delete []tempFolder;
	return true;
}


bool cuCreateTempFolder(OSString &outPath, OSString &errMsg)
{
	cuGetTempPath(outPath);

	OSString guid;
	if(!cuGenerateGUID(guid))
		return false;

	outPath.append(guid);
    int retVal=SHCreateDirectoryEx(NULL,outPath.c_str(),NULL);
	if((retVal==ERROR_SUCCESS )||(retVal== ERROR_ALREADY_EXISTS)|| (retVal==ERROR_FILE_EXISTS))
	{
			//return true;
	}
	else
    {
		errMsg = OSConst("Unable to create the temporary directory");
		return false;
    }

	if(!cuCreateDirectory(outPath))
		return false;

	return true;

}


bool cuCreateDirectory(const OSString &path)
{
	int retVal=0;
	retVal=SHCreateDirectoryEx(NULL,path.c_str(),NULL);
	if((retVal==ERROR_SUCCESS )||(retVal== ERROR_ALREADY_EXISTS)|| (retVal==ERROR_FILE_EXISTS))
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
}

//TODO:
bool cuDeleteDirectory(const OSString &path)
{

	return true;
}


//TODO: 
bool cuDeleteDirectoryContent(const OSString &dirPath)
{

	return true;
}


unsigned int cuGetNumberOfProcessors()
{
	SYSTEM_INFO sys_Info;

	GetSystemInfo(&sys_Info);

	return sys_Info.dwNumberOfProcessors;
}

bool getCarbonCommonFolderPath(OSString &outPath)
{
	TCHAR commonPath[MAX_PATH];
	if(!SHGetSpecialFolderPath(NULL,commonPath,CSIDL_PROGRAM_FILES_COMMON,false))
	{
		return false;
	}

	outPath=commonPath;
	outPath += kUCarbonCommonFolderRelPath;

	return true;
}

bool testfn(vector<OSString> temp)
{

	return true;
}

}
