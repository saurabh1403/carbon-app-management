

#include "Utilities.h"
#include <string>

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

bool getLongFromCharArray(const char * inStr, long int &outNum)
{
	if(inStr == NULL)
		return false;

	outNum = atol(inStr);

	return true;

}

bool getStringFromLong(const long int inNum, string &outStr)
{
	char a[30];
	_ltoa_s(inNum, a, 30, 10);
	outStr = a;

	return true;
}


bool cuCreateCarbonLicenseDirectory()
{
	OSString folderPath;
	carbonUtilities::cuGetWinSpecificFolderPath(folderPath, CSIDL_COMMON_APPDATA);
	folderPath += CarbonAppDataRelativeFolder;
	folderPath += OSSlash;

	return cuCreateDirectory(folderPath);
}


bool cuGetWinSpecificFolderPath(OSString & outPath,int FolderIdentifier )
{
	TCHAR path[MAX_PATH];
	if(!SHGetSpecialFolderPath(NULL,path,FolderIdentifier,false))
	{
		return false;
	}
	outPath.assign(path);
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


bool cuCreateFileWithData(const void *data, unsigned int length, OSString &filePath)
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

bool cuGetOSStringFromInt(unsigned int input, OSString &outNum)
{
	char *num;
	if(!getCharArrayFromInt(&num, input))
	{
		return false;
	}

	cuConvertStringToOSString(num, outNum);
	return true;
}
	
bool readFromFile(const OSString &inFilePath, char ** fileData, unsigned int &dataLen)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	DWORD dwFileSize;
	DWORD dwBytesRead;
	char * fileBuff = NULL;
	HANDLE fileStream;

	fileStream = CreateFile(inFilePath.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(fileStream == INVALID_HANDLE_VALUE)
	{
		CARBONLOG_ERROR(logger, "[readFromFile] : Failed to create file handle for file present at " << inFilePath.c_str());
		return false;
	}

	dwFileSize = GetFileSize(fileStream, NULL);
	if(dwFileSize == INVALID_FILE_SIZE)
	{
		CARBONLOG_ERROR(logger, "[readFromFile] : Unable to get file size for file at location " << inFilePath.c_str() << " and the platform error is " << GetLastError());
		return false;
	}
	
	fileBuff = (char *)malloc(sizeof(char) * (dwFileSize+1));
	dataLen = dwFileSize;
	if(!fileBuff)
	{
		return false;
	}
	
	if(!ReadFile(fileStream, (LPVOID)fileBuff, dwFileSize, &dwBytesRead, NULL))
	{
		CARBONLOG_ERROR(logger, "[readFromFile] : Unable to read file at location - " << inFilePath.c_str() << " and the platform error is " << GetLastError());
		free(fileBuff);
		return false;
	}

	*fileData = fileBuff;

	return true;
}

//TODO:
bool cuDeleteDirectory(const OSString &path)
{
	if(PathIsDirectory(path.c_str()) == FALSE)
		return false;

	cuDeleteDirectoryContent(path);
	RemoveDirectory(path.c_str());

	return true;
}

bool cuDeleteFile(const OSString &filePath)
{
	DeleteFile(filePath.c_str());

	return true;
}


//_TODO: improve this. recursion is bad for health
bool cuDeleteDirectoryContent(const OSString &dirPath)
{	
	if(PathIsDirectory(dirPath.c_str()) == FALSE)
		return false;

	TCHAR fileFound[MAX_PATH];
	WIN32_FIND_DATA info;
	HANDLE hp; 
	_stprintf_s(fileFound,MAX_PATH, _T("%s\\*.*"), dirPath.c_str());
	hp = FindFirstFile(fileFound, &info);
	do
	{
		if (!((_tcscmp(info.cFileName, _T("."))==0)||
			(_tcscmp(info.cFileName, _T(".."))==0)))
		{
			if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==
				FILE_ATTRIBUTE_DIRECTORY)
			{
				OSString subFolder = dirPath;
				subFolder.append(OSSlash);
				subFolder.append(info.cFileName);
				cuDeleteDirectoryContent(subFolder);
				RemoveDirectory(subFolder.c_str());
			}
			else
			{
				_stprintf_s(fileFound, MAX_PATH, _T("%s\\%s"), dirPath.c_str(), info.cFileName);
				BOOL retVal = DeleteFile(fileFound);
			}
		}

	}while(FindNextFile(hp, &info)); 
	FindClose(hp);

	return true;
}

bool cuCopyFile(const OSString &infilePath, const OSString &outPath)
{
	return CopyFile(infilePath.c_str(), outPath.c_str(), false);

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

}
