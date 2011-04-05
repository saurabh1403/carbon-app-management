
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
#define filePathTagName				OSConst("FilePath")

#define resultXmlTag				OSConst("<result></result>")

struct contentInfo
{
	std::string identifier;
	std::string inPath;
	std::string outPath;
};

class ContentEncryptor
{

private:

	vector<contentInfo>  contentData;
	AESWrapper _contentEncryptor;
	CarbonXMLParser xmlObj;

	//these are absolute paths
	std::string inputBmpPath;
	std::string outDir;
	std::string outBmpPath;

	std::string inXmlPath;

	bool loadConfigurations();
	bool populateContentDetails();
	bool initContentEncoder();

	bool init(const std::string &xmlPath);

	bool runEncryptor();

	bool generateOutputXml();


public:

	ContentEncryptor()
	{
	}

	~ContentEncryptor()
	{

	}

	bool run(const std::string &inXmlPath);


};

