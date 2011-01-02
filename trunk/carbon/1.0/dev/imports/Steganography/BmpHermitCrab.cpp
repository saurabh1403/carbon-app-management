
#include "bmphermitcrab.h"

char* CBmpHermitCrab::m_binaryMap[]={
	"00000000","00000001","00000010","00000011","00000100","00000101","00000110","00000111",
	"00001000","00001001","00001010","00001011","00001100","00001101","00001110","00001111",
	"00010000","00010001","00010010","00010011","00010100","00010101","00010110","00010111",
	"00011000","00011001","00011010","00011011","00011100","00011101","00011110","00011111",
	"00100000","00100001","00100010","00100011","00100100","00100101","00100110","00100111",
	"00101000","00101001","00101010","00101011","00101100","00101101","00101110","00101111",
	"00110000","00110001","00110010","00110011","00110100","00110101","00110110","00110111",
	"00111000","00111001","00111010","00111011","00111100","00111101","00111110","00111111",
	"01000000","01000001","01000010","01000011","01000100","01000101","01000110","01000111",
	"01001000","01001001","01001010","01001011","01001100","01001101","01001110","01001111",
	"01010000","01010001","01010010","01010011","01010100","01010101","01010110","01010111",
	"01011000","01011001","01011010","01011011","01011100","01011101","01011110","01011111",
	"01100000","01100001","01100010","01100011","01100100","01100101","01100110","01100111",
	"01101000","01101001","01101010","01101011","01101100","01101101","01101110","01101111",
	"01110000","01110001","01110010","01110011","01110100","01110101","01110110","01110111",
	"01111000","01111001","01111010","01111011","01111100","01111101","01111110","01111111",
	"10000000","10000001","10000010","10000011","10000100","10000101","10000110","10000111",
	"10001000","10001001","10001010","10001011","10001100","10001101","10001110","10001111",
	"10010000","10010001","10010010","10010011","10010100","10010101","10010110","10010111",
	"10011000","10011001","10011010","10011011","10011100","10011101","10011110","10011111",
	"10100000","10100001","10100010","10100011","10100100","10100101","10100110","10100111",
	"10101000","10101001","10101010","10101011","10101100","10101101","10101110","10101111",
	"10110000","10110001","10110010","10110011","10110100","10110101","10110110","10110111",
	"10111000","10111001","10111010","10111011","10111100","10111101","10111110","10111111",
	"11000000","11000001","11000010","11000011","11000100","11000101","11000110","11000111",
	"11001000","11001001","11001010","11001011","11001100","11001101","11001110","11001111",
	"11010000","11010001","11010010","11010011","11010100","11010101","11010110","11010111",
	"11011000","11011001","11011010","11011011","11011100","11011101","11011110","11011111",
	"11100000","11100001","11100010","11100011","11100100","11100101","11100110","11100111",
	"11101000","11101001","11101010","11101011","11101100","11101101","11101110","11101111",
	"11110000","11110001","11110010","11110011","11110100","11110101","11110110","11110111",
	"11111000","11111001","11111010","11111011","11111100","11111101","11111110","11111111"
};
WORD CBmpHermitCrab::m_wHermitCrabFlag=MAKEWORD('C','H');

CBmpHermitCrab::CBmpHermitCrab(void)
{
}

CBmpHermitCrab::~CBmpHermitCrab(void)
{
}

bool CBmpHermitCrab::Hermit(const char* pszDestFileName,const char* pszSrcFileName,const char* pszPassword,const char* pszResultFile)
{
	ASSERT(pszDestFileName!=NULL && pszSrcFileName!=NULL);
	
	CFile fDestFile,fSrcFile;
	VERIFY(fDestFile.Open(pszDestFileName,CFile::modeRead|CFile::typeBinary));
	VERIFY( fSrcFile.Open(pszSrcFileName,CFile::modeRead|CFile::typeBinary));

	int	iDestFileLength =fDestFile.GetLength();
	int	iSrcFileLength  =fSrcFile.GetLength();

	CString strSrcFileName(pszSrcFileName);
	int nIndex=strSrcFileName.ReverseFind('\\');
	if(nIndex>0){
		strSrcFileName=strSrcFileName.Right(strSrcFileName.GetLength()-nIndex-1);
	}

	HERMITCRAB_HEAD hermitCrabHead;
	hermitCrabHead.wFlag		= m_wHermitCrabFlag;
	hermitCrabHead.bPasswordLen	= ((pszPassword==NULL)? 0:strlen(pszPassword));
	hermitCrabHead.iFileLength  = iSrcFileLength;
	hermitCrabHead.bFileNameLen = (BYTE)strSrcFileName.GetLength();

	int  iBinaryLen = (sizeof(hermitCrabHead) + hermitCrabHead.bPasswordLen +hermitCrabHead.bFileNameLen+hermitCrabHead.iFileLength)*8;
	if (iDestFileLength<iBinaryLen){
		fDestFile.Close();
		fSrcFile.Close();
		return false;
	}

	char *buffer = new char [iSrcFileLength];	
	fSrcFile.Read(buffer,iSrcFileLength);
	fSrcFile.Close();

	char *bmpBuffer=new char [iDestFileLength];
	fDestFile.Read(bmpBuffer,iDestFileLength);
	fDestFile.Close();

	char *srcBinary = new char [iBinaryLen];
	
	int iBufferAllLen=sizeof(hermitCrabHead)+hermitCrabHead.bPasswordLen+hermitCrabHead.bFileNameLen+hermitCrabHead.iFileLength;
	unsigned char * strBufferAll =new unsigned char[iBufferAllLen];
	unsigned char * p= strBufferAll;
	memcpy(p,(void*)&hermitCrabHead,sizeof(hermitCrabHead));
	p= p+sizeof(HERMITCRAB_HEAD) ;
	if(hermitCrabHead.bPasswordLen>0){
		memcpy(p,(void*)pszPassword,hermitCrabHead.bPasswordLen);
		p=p+hermitCrabHead.bPasswordLen;
	}
	memcpy(p,(void*)(LPCTSTR)strSrcFileName,hermitCrabHead.bFileNameLen);
	p=p+hermitCrabHead.bFileNameLen;
	memcpy(p,(void*)buffer,hermitCrabHead.iFileLength);

	delete [] buffer;

	CString temp;
	nIndex=0;
	for(int i=0;i<(iBinaryLen/8); i++){
		temp= m_binaryMap[(unsigned char) strBufferAll[i]];
		srcBinary[nIndex]	= temp[0];
		srcBinary[nIndex+1]	=temp[1];
		srcBinary[nIndex+2]	=temp[2];
		srcBinary[nIndex+3]	=temp[3];
		srcBinary[nIndex+4]	=temp[4];
		srcBinary[nIndex+5]	=temp[5];
		srcBinary[nIndex+6]	=temp[6];
		srcBinary[nIndex+7]	=temp[7];
		nIndex=nIndex+8; 
	}
	

	int mask=254;	
	for (int i=0 ; i<iBinaryLen ; i++){
		bmpBuffer[i+55]=bmpBuffer[i+55] & mask;
		if (srcBinary[i]=='1')  
			bmpBuffer[i+55]=bmpBuffer[i+55]+1;
	}
	delete [] srcBinary;

	CFile fResultFile;
	CString strResultFile;
	if(pszResultFile==NULL){
		strResultFile.Format("%s",pszDestFileName);
	}else{
		strResultFile.Format("%s",pszResultFile);
	}
	
	VERIFY(fResultFile.Open(strResultFile,CFile::modeReadWrite|CFile::modeCreate|CFile::typeBinary));
	fResultFile.Write(bmpBuffer,iDestFileLength);	
	fResultFile.Close();
	
	delete [] bmpBuffer;
	delete [] strBufferAll;

	return true;
}
bool CBmpHermitCrab::IsHermitCrabBMPFile(const char* pszFileName)
{
	CFile fSrcFile;
	VERIFY(fSrcFile.Open(pszFileName,CFile::modeRead|CFile::typeBinary ));
	int	iSrcFileLength=fSrcFile.GetLength();
	char *buffer = new char [iSrcFileLength] ; 
	fSrcFile.Read(buffer,iSrcFileLength);
	fSrcFile.Close();
	int *recBin=new int [iSrcFileLength]; 
	for(int j=55 ; j<55+8*sizeof(HERMITCRAB_HEAD);j++){
		recBin[j]=(unsigned char) ((unsigned char)buffer[j] & 1);
	}
	char temp;
	unsigned char *p =new unsigned char [sizeof(HERMITCRAB_HEAD)];
	int nLoop=0;
	for (int j=55 ; j<55+8*sizeof(HERMITCRAB_HEAD);j=j+8) {
		temp=128*recBin[j]+64*recBin[j+1]+32*recBin[j+2]+16*recBin[j+3]+8*recBin[j+4]+4*recBin[j+5]+2*recBin[j+6]+1*recBin[j+7];
		p[nLoop++]=temp;
	}
	HERMITCRAB_HEAD * pHermitCrabHead=(HERMITCRAB_HEAD *)(void*)p; 
	bool bRet = (pHermitCrabHead->wFlag==m_wHermitCrabFlag);
	delete [] p;
	delete [] buffer;
	delete [] recBin;
	return  bRet;
}
bool CBmpHermitCrab::RecoverCrabFile(const char* pszFileName,const char* pszPassword,const char* pszPath)
{
	CFile fSrcFile;
	VERIFY(fSrcFile.Open(pszFileName,CFile::modeRead|CFile::typeBinary ));
	int	iSrcFileLength=fSrcFile.GetLength();
	char *buffer = new char [iSrcFileLength] ; 
	fSrcFile.Read(buffer,iSrcFileLength);
	fSrcFile.Close();
	
	int *recBin=new int [iSrcFileLength]; 

	int nHeadSize=sizeof(HERMITCRAB_HEAD);

	for(int j=55 ; j<55+8*nHeadSize;j++){
		recBin[j]=(unsigned char) ((unsigned char)buffer[j] & 1);
	}
	char temp;

	unsigned char *p =new unsigned char [sizeof(HERMITCRAB_HEAD)];
	int nLoop=0;
	for (int j=55 ; j<55+8*nHeadSize;j=j+8) {
		temp=128*recBin[j]+64*recBin[j+1]+32*recBin[j+2]+16*recBin[j+3]+8*recBin[j+4]+4*recBin[j+5]+2*recBin[j+6]+1*recBin[j+7];
		p[nLoop++]=temp;
	}
	HERMITCRAB_HEAD * pHermitCrabHead=(HERMITCRAB_HEAD *)(void*)p; 

	if(pHermitCrabHead->wFlag!=m_wHermitCrabFlag){
		delete [] buffer;
		delete [] recBin;
		return false;
	}
	if(pHermitCrabHead->bPasswordLen>0){
		if(pszPassword==NULL){
			delete [] buffer;
			delete [] recBin;
			return false;
		}
	}
	for(int j=55+8*nHeadSize;j<55+8*nHeadSize+pHermitCrabHead->bPasswordLen*8+pHermitCrabHead->bFileNameLen*8;j++){
		recBin[j]=(unsigned char) ((unsigned char)buffer[j] & 1);
	}
	CString strTemp;
	for(int j=55+8*nHeadSize;j<55+8*nHeadSize+pHermitCrabHead->bPasswordLen*8+pHermitCrabHead->bFileNameLen*8;j=j+8){
		temp=128*recBin[j]+64*recBin[j+1]+32*recBin[j+2]+16*recBin[j+3]+8*recBin[j+4]+4*recBin[j+5]+2*recBin[j+6]+1*recBin[j+7];
		strTemp+=temp;
	}
	if(pHermitCrabHead->bPasswordLen>0){
		CString strPassword=strTemp.Left(pHermitCrabHead->bPasswordLen);
		if(strPassword!=pszPassword){
			delete [] buffer;
			delete [] recBin;
			return false;
		}
	}
	CString strFileName=strTemp.Right(pHermitCrabHead->bFileNameLen);
	CString strPath="";
	if(pszPath!=NULL){
		strPath=pszPath;
		int nIndex= strPath.ReverseFind('\\');
		if(nIndex<0 || nIndex!=(strPath.GetLength()-1) ){
			strPath+="\\";
		}
	}
	strPath+=strFileName;
	CFile fResultFile;
	VERIFY(fResultFile.Open(strPath,CFile::modeReadWrite|CFile::modeCreate|CFile::typeBinary));

	for(int j=55+8*nHeadSize+pHermitCrabHead->bPasswordLen*8+pHermitCrabHead->bFileNameLen*8;
		j<55+8*nHeadSize+pHermitCrabHead->bPasswordLen*8+pHermitCrabHead->bFileNameLen*8+pHermitCrabHead->iFileLength*8;j++){
		recBin[j]=(unsigned char) ((unsigned char)buffer[j] & 1);
	}
	strTemp="";
	for(int j=55+8*nHeadSize+pHermitCrabHead->bPasswordLen*8+pHermitCrabHead->bFileNameLen*8;
		j<55+8*nHeadSize+pHermitCrabHead->bPasswordLen*8+pHermitCrabHead->bFileNameLen*8+pHermitCrabHead->iFileLength*8;j=j+8){
		temp=128*recBin[j]+64*recBin[j+1]+32*recBin[j+2]+16*recBin[j+3]+8*recBin[j+4]+4*recBin[j+5]+2*recBin[j+6]+1*recBin[j+7];
		strTemp+=temp;
	}
	fResultFile.Write(strTemp,pHermitCrabHead->iFileLength);
	fResultFile.Close();
	delete [] p;
	delete [] buffer;
	delete [] recBin;
	
	return true;
}
