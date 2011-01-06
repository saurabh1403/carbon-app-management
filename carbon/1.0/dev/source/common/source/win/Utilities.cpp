

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

}


	
bool getCharArrayFromInt(char **outArr, unsigned int i, unsigned int chars_count)
{
	if(outArr == NULL)
		return false;

	char val[20];
	itoa(i, val, 10);
	int len_val = strlen(val);

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


}