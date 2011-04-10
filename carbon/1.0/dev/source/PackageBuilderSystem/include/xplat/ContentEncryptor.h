
#pragma once

#include "Constants.h"
#include "TempManager.h"
#include "AESEncryption.h"
#include "CarbonXMLParser.h"
#include <map>

#define InBmpFileXquery				OSConst("/contentData/configuration/inputBmpFile")
#define OutBmpFileXquery			OSConst("/contentData/configuration/outBmpFile")
#define OutDirXquery				OSConst("/contentData/configuration/outputDir")
#define contentInfoNodesXquery		OSConst("/contentData/contents/content")

#define identifierTagName			OSConst("identifier")
#define sIdentifierTagName			"identifier"
#define inFilePathTagName				OSConst("inPath")
#define outFilePathTagName			OSConst("outPath")


#define resultXmlTag				OSConst("<contentData></contentData>")

struct contentInfo
{
	std::string identifier;
	std::string inPath;
	std::string outPath;
};

typedef enum mode
{
	encryptionMode,
	decryptionMode

}enCodingMode;


class ContentEncryptor
{

private:

	vector<contentInfo>  contentData;
	AESWrapper _contentEncryptor;
	AESWrapper _contentDecryptor;
	CarbonXMLParser xmlObj;
	enCodingMode objMode;

	//these are absolute paths
	std::string inputBmpPath;
	std::string outDir;
	std::string outBmpPath;

	std::string inXmlPath;

	bool loadConfigurations();
	bool populateContentDetails();
	bool initContentEncoder();
	bool initContentDecoder();


public:

	ContentEncryptor()
	{
	}

	~ContentEncryptor()
	{

	}

	bool run(const std::string &inXmlPath);
	
	bool init(const std::string &xmlPath, enCodingMode = encryptionMode);

	bool runEncryptor();

	bool runDecryptor();

	bool generateOutputXml();


};

