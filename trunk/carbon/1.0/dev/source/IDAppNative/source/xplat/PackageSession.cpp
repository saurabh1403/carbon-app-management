
#include "PackageSession.h"
#include "IDAppConstants.h"
#include "CarbonXMLParser.h"

//TODO:
bool PackageSession::initSession(const OSString &pkgPath)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(isSessionInitialized)
	{
		CARBONLOG_INFO(logger, "[initSession] : Session is already initialized");
		return true;
	}

	//check the licensing and the trial expiry

	//mark entry in db for its start time

	//open the db and check its validity

	//fetch all the information from the db and store in the class. call populate function

	isSessionInitialized = true;

	return true;

}


bool PackageSession::closeSession()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(!isSessionInitialized)
	{
		CARBONLOG_INFO(logger, "[closeSession] : Package is already closed. Quiting now");
		return true;
	}

	//mark time in db and the license

	//free all memory and clear all temp


	isSessionInitialized = false;


	return true;
}


bool PackageSession::processJob(IDAppNativeJob &job, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isSessionInitialized)
	{
		CARBONLOG_ERROR(logger, "Session is not initialized. Invalid call");
		return false;
	}

	//std::map<std::string, pkgSessionTarget>::const_iterator itr = targetFnMap.find(job.targetObject);
	//if( itr != targetFnMap.end())
	//{
	//	CARBONLOG_DEBUG(logger, "[PackageSession::processJob] : Executing target function "<<job.targetObject);
	//	itr->second(job,resStr);
	//}

	//else
	//	CARBONLOG_ERROR(logger, "[PackageSession::processJob] : target function doesn't exist"<<job.targetObject);

	//ugly.. need to correct it.



	return true;
}


//in case of error, it returns the error xml string node. i.e. <errorCode>...</errorCode><errorMsg>...</errorMsg>
bool PackageSession::getPackageContentInfo(std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	//construct an xml from the info from all the data stored in class
	
	std::string outXmlTemp = OutputXmlNode;
	CarbonXMLParser xmlObj;

	if(!xmlObj.initWithXMLString(outXmlTemp))
	{
		CARBONLOG_ERROR(logger, "[getPackageContentInfo] : Failed to init the xml obj");
		IDAppNativeJob::getErrorXmlNode(resStr, "Failed to init xml object");
		return false;
	}

	XMLNode rootNode;
	if(!xmlObj.getRootNode(rootNode))
	{
		CARBONLOG_ERROR(logger, "[getPackageContentInfo] : Failed to get root node in the xml");
		IDAppNativeJob::getErrorXmlNode(resStr, "Failed to get root node xml object");
		return false;
	}

	//***********adding resource file node**********
	XMLNode resourcesNode;
	xmlObj.createNodeWithNameAndValue(ResourceFileNodeName,"",resourcesNode);
	xmlObj.addChildToNode(ContentTypeNodeName, resourcesInfo.contentType, resourcesNode);
	xmlObj.addChildToNode(InfoXmlNodeName, resourcesInfo.contentInfoXmlPath, resourcesNode);
	xmlObj.addChildToNode(resourcesNode, rootNode);

	//***********adding ui data node***********
	XMLNode uiDataNode;
	xmlObj.createNodeWithNameAndValue(UIDataNodeName,"",uiDataNode);
	xmlObj.addChildToNode(ContentTypeNodeName, uiDataInfo.contentType, uiDataNode);
	xmlObj.addChildToNode(InfoXmlNodeName, uiDataInfo.contentInfoXmlPath, uiDataNode);
	xmlObj.addChildToNode(uiDataNode, rootNode);

	//******adding content nodes***********
	XMLNode contentListNode;
	xmlObj.createNodeWithNameAndValue("contentNodes","",contentListNode);

	std::vector<contentInfo>::const_iterator itr = contentNodes.begin();
	for(; itr != contentNodes.end(); itr++)
	{
		XMLNode contentNode;
		xmlObj.createNodeWithNameAndValue("content","",contentNode);
		xmlObj.addChildToNode(ContentTypeNodeName, itr->contentType, contentNode);
		xmlObj.addChildToNode(InfoXmlNodeName, itr->contentInfoXmlPath, contentNode);

		if(!xmlObj.addChildToNode(contentNode, contentListNode))
		{
			CARBONLOG_ERROR(logger, "[getPackageContentInfo] : Failed to add child to xml obj");
			IDAppNativeJob::getErrorXmlNode(resStr,"Failed to process Xml");
			return false;
		}
	}

	xmlObj.addChildToNode(contentListNode, rootNode);
	xmlObj.getXMLString(resStr);

	return true;

}

//TODO:
bool PackageSession::initializeContentDecryptor()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//db initialize
	if(_pkgDb.isInitialized)
	{
		CARBONLOG_ERROR(logger, "[initializeContentDecryptor] : package db is not initialized");
		return false;
	}

	//make the bmp image file in temp folder
//	string 
//	if(!getSecretInfoFileFromDb(
	
	//get the keys and make key container

	//initialize the aes wrapper with the key container

	return true;

}


//TODO:
bool PackageSession::getContent(const std::string &argXml, std::string &resStr)
{

	resStr = "<outputXml/>";

	return true;
}

//TODO:
bool PackageSession::clearCachedContent(std::string &resStr)
{

	resStr = "<outputXml/>";
	return true;
}

