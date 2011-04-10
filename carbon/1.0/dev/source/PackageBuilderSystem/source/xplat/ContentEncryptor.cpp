
#include "ContentEncryptor.h"
#include "carbonSteganography.h"

#include "Log.h"

bool ContentEncryptor::run(const std::string &xmlPath)
{
	init(xmlPath);

	runEncryptor();

	runDecryptor();

	generateOutputXml();

	return true;
}

bool ContentEncryptor::init(const std::string &xmlPath, enCodingMode inMode)
{
	objMode = inMode;

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!xmlObj.initWithXMLPath(xmlPath))
	{
		CARBONLOG_ERROR(logger, "[init] : Failed to init xml with path" <<xmlPath.c_str());
		return false;
	}

	inXmlPath = xmlPath;

	return (loadConfigurations() && populateContentDetails());

}

bool ContentEncryptor::populateContentDetails()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	int numContentNodes;
	XMLNodeList contentNodeList = xmlObj.getNodeListForXQuery(contentInfoNodesXquery , &numContentNodes);
	if(numContentNodes > 0)
	{
		OSString contentQuery( contentInfoNodesXquery );
		contentQuery.append(OSConst("["));
		OSString curContentQuery;
		OSString indexStr;
		
		std::string contentIdentifier, contentType;
		unsigned int cIndex;
		for(unsigned int i =0; i < numContentNodes; i++)
		{
#ifdef MAC
			cIndex = i+1;
#else
			cIndex = i;
#endif
			curContentQuery.assign( contentQuery );
			carbonUtilities::cuGetOSStringFromInt( cIndex, indexStr );
			curContentQuery += indexStr + OSConst("]/");

			contentInfo _tempInfo;

			if( !xmlObj.stringValueForXQuery(curContentQuery + identifierTagName, _tempInfo.identifier) )
			{		
				CARBONLOG_ERROR(logger, "[populateContentDetails] : Failed to get the identifier tag in the xml at path" <<inXmlPath.c_str());
				return false;
			}

			if( !xmlObj.stringValueForXQuery(curContentQuery + inFilePathTagName, _tempInfo.inPath) )
			{		
				CARBONLOG_ERROR(logger, "[populateContentDetails] : Failed to get the infilepath information from the xml at path" <<inXmlPath.c_str());
				return false;
			}

			if( !xmlObj.stringValueForXQuery(curContentQuery + outFilePathTagName, _tempInfo.outPath) )
			{		
				CARBONLOG_INFO(logger, "[populateContentDetails] : Failed to get the outfilepath information from the xml at path" <<inXmlPath.c_str());
			}

			contentData.push_back(_tempInfo);
		}
	}
			
	return true;
}

bool ContentEncryptor::loadConfigurations()
{

	xmlObj.stringValueForXQuery(InBmpFileXquery, this->inputBmpPath);
	xmlObj.stringValueForXQuery(OutBmpFileXquery, this->outBmpPath);
	xmlObj.stringValueForXQuery(OutDirXquery, this->outDir);

	if(objMode == encryptionMode)
	{
		return initContentEncoder();
	}
	else
	{
		return initContentDecoder();
	}

}

//it initialized the encryptor using a random key
bool ContentEncryptor::initContentEncoder()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	carboncipherUtilities::AESSecretKeyContainer _keyContainer;
	_keyContainer.initWithRandomKeys();

	std::string keyStream;

	if(!_keyContainer.writeKeysIntoStream(keyStream))
	{
		CARBONLOG_ERROR(logger, "[initContentEncoder] : Failed to get keystream from key container");
		return false;
	}
	
	CARBONLOG_INFO(logger, "[initContentEncoder] : The key stream is - "<< keyStream.c_str());

	if(!carbonStegano::hideDataInBmp(keyStream, inputBmpPath, outBmpPath))
	{
		CARBONLOG_ERROR(logger, "[initContentEncoder] : Failed to write keystream in the bmp file at path" <<outBmpPath.c_str());
		return false;
	}

	_contentEncryptor.initializeCiphersFromKeys(_keyContainer);

	return true;

}

bool ContentEncryptor::initContentDecoder()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	
	string outKeyStream;
	//TODO: take inBmpPath and not outBmpPath
	bool ret = carbonStegano::retrieveDataFromBmp(outBmpPath, outKeyStream);

	carboncipherUtilities::AESSecretKeyContainer keyStorage2;
	keyStorage2.initKeysFromKeyStream(outKeyStream.c_str());
	
	_contentDecryptor.initializeCiphersFromKeys(keyStorage2);

	return true;
}

bool ContentEncryptor::runEncryptor()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	//loop through the contents

	string tempOutDir = outDir + Slash;

	vector<contentInfo>::iterator itr = contentData.begin();
	for(; itr!= contentData.end(); itr++)
	{
		string guid;
		carbonUtilities::cuGenerateGUID(guid);

		std::string outFilePath = tempOutDir + guid;

		if(!_contentEncryptor.encryptFile(itr->inPath, outFilePath))
		{
			CARBONLOG_ERROR(logger, "[runEncryptor] : Failed to encrypt the file " << itr->inPath.c_str());
			continue;
		}

		itr->outPath  = outFilePath;
	}

	return true;
}

bool ContentEncryptor::runDecryptor()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//TODO: testing purpose
	vector<contentInfo>::const_reverse_iterator itr = contentData.rbegin();
//	vector<contentInfo>::iterator itr = contentData.begin();
	for(; itr!= contentData.rend(); itr++)
	{
		//TODO: dont seek outPath but inPath. This is just for testing
		std::string outFilePath = itr->outPath;
		outFilePath += "_test.flv";

		_contentDecryptor.decryptFile(itr->outPath, outFilePath);
	}

	return true;

}


bool ContentEncryptor::generateOutputXml()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CarbonXMLParser _xmlObj;

	if(!_xmlObj.initWithXMLString(resultXmlTag))
	{
		CARBONLOG_ERROR(logger, "[generateOutputXml] : failed to init with result xml string");
		return false;
	}

	XMLNode rootNode;
	if(!_xmlObj.getRootNode(rootNode))
	{
		CARBONLOG_ERROR(logger, "[generateOutputXml] : Failed to get root node in the xml");
		return false;
	}

	//generate the configuration part
	XMLNode configNode;
	_xmlObj.createNodeWithNameAndValue("configuration", "", configNode);
	_xmlObj.addChildToNode("outBmpFile", outBmpPath, configNode);
	_xmlObj.addChildToNode("outputDir", outDir, configNode);
	_xmlObj.addChildToNode("inputBmpFile", inputBmpPath, configNode);
	_xmlObj.addChildToNode(configNode, rootNode);

	//******adding content nodes***********
	XMLNode contentListNode;
	_xmlObj.createNodeWithNameAndValue("contents","",contentListNode);

	std::vector<contentInfo>::const_iterator itr = contentData.begin();
	for(; itr != contentData.end(); itr++)
	{
		XMLNode contentNode;
		_xmlObj.createNodeWithNameAndValue("content","",contentNode);
		_xmlObj.addChildToNode(sIdentifierTagName, itr->identifier, contentNode);
		_xmlObj.addChildToNode("inPath", itr->inPath, contentNode);
		_xmlObj.addChildToNode("outPath", itr->outPath, contentNode);

		if(!_xmlObj.addChildToNode(contentNode, contentListNode))
		{
			CARBONLOG_ERROR(logger, "[generateOutputXml] : Failed to add child to xml obj");
			return false;
		}
	}

	_xmlObj.addChildToNode(contentListNode, rootNode);

	string sOutXmlPath = outDir;
	sOutXmlPath += Slash;
	sOutXmlPath += "outputXml.xml";

	OSString outXmlPath;
	carbonUtilities::cuConvertStringToOSString(sOutXmlPath, outXmlPath);

	if(!_xmlObj.writeXMLToFilePathInUTF8(outXmlPath))
	{
		CARBONLOG_ERROR(logger, "[generateOutputXml] : Failed to write xml to the file" <<sOutXmlPath.c_str());
		return false;
	}

	return true;
}




