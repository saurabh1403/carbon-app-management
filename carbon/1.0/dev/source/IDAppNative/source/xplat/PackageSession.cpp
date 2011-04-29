
#include "PackageSession.h"
#include "IDAppConstants.h"
#include "CarbonXMLParser.h"
#include "carbonSteganography.h"
#include <string>

/********************************
Here is the method to add any new API in this module.

1. add a function. 
2. add its function pointer in global context
3. in the function, if any error comes, make an error node string 
3a.	Make an output node from the error node using the functions available in IdappNativeJob class 
4. if the job is for session, get the session id and get the session pointer
4a. in the session functions, call that function and those functions should return an xml node having the outputXml content
4b. the caller wil make the ouptut xml from this node and then send it to UI
4c. the called function in the session class can also write the output to the UI directly. 


********************************/

//TODO:
bool PackageSession::initSession(const OSString &pkgPath, const std::string pkgId, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	if(isSessionInitialized)
	{
		CARBONLOG_INFO(logger, "[initSession] : Session is already initialized");
		return true;
	}

	
#ifdef WIN32
	InitializeCriticalSection(&PackageSessionCritSec);
#endif

	packagePath = pkgPath;
	carbonUtilities::cuConvertOSStringToString(pkgPath, sPackagePath);
	sPackageId = pkgId;

	//check the licensing and the trial expiry
	PkgInfo _pkgInfo;
	_pkgInfo.pkgId = sPackageId;

	OSString apdDbPath = _T("");
	if(!carbonUtilities::getCarbonCommonFolderPath(apdDbPath))
	{
		CARBONLOG_FATAL(logger, "[initSession] : Failed to get the common folder path. Really Weired Error");
		IDAppNativeJob::getErrorXmlNode(resStr,"Product is Corrupted. Reinstalling it may fix the problem");
		return false;
	}
	apdDbPath += kUAPDRelativeFolderPath;
	apdDbPath += OSSlash;
	apdDbPath += kUApplicationProvisioningDataBaseName;

	_pkgInfo.licenseDbPath = apdDbPath;
	_pkgInfo.pkgPath = packagePath;

	//_TODO: for testing only
	if(!_pkgLicense.init(_pkgInfo))
	{
		CARBONLOG_FATAL(logger, "[initSession] : Failed to init APD library");
		IDAppNativeJob::getErrorXmlNode(resStr,"Product is Corrupted. Reinstalling it may fix the problem");
		return false;
	}

	if(_pkgLicense.validateLicense() != LicenseOk)
	{
		CARBONLOG_FATAL(logger, "[initSession] : License is not valid for this package");
		IDAppNativeJob::getErrorXmlNode(resStr,"Product is Expired. Contact the Support team to renew the license");
		return false;
	}

	//mark entry in db for its start time
	startTime = time(NULL);

	//open the db and check its validity
	string pkgDbPath;
	pkgDbPath = sPackagePath;
	pkgDbPath += "\\pkg.db";
	if(_pkgDb.init(pkgDbPath) != SQLITE_OK)
	{
		CARBONLOG_ERROR(logger, "[initSession] : Failed to init the package db");
		IDAppNativeJob::getErrorXmlNode(resStr,"Product is Corrupted. Reinstalling it may fix the problem");
		return false;
	}

	//fetch all the information from the db and store in the class. call populate function
	//store the absolute path here and not only the relative path. UI should get all paths as absolute
	if(!populatePackageContentFromDb())
	{
		CARBONLOG_ERROR(logger, "[initSession] : Failed to populate the content from package db");
		IDAppNativeJob::getErrorXmlNode(resStr,"Product is Corrupted. Reinstalling it may fix the problem");
		return false;
	}

	//initialize the content decoder here
	if(!initializeContentDecryptor())
	{
		CARBONLOG_ERROR(logger, "[initSession] : Failed to initialize the content decoder");
		IDAppNativeJob::getErrorXmlNode(resStr,"Product is Corrupted. Reinstalling it may fix the problem");
		return false;
	}

	tempMgr.initWithTempLocations(4);

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
	pkgTimeInfo _pkgTime;
	_pkgTime.startTime = this->startTime;
	_pkgTime.closeTime = time(NULL);

	//mark the package end time
	if(!_pkgLicense.updateLicense(_pkgTime))
	{
		CARBONLOG_FATAL(logger, "[closeSession] : Error in making db changes");
	}

	//free all memory and clear all temp
	tempMgr.clearTemp();

	carbonUtilities::cuDeleteFile(resourcesInfo.contentInfoXmlPath);

	carbonUtilities::cuDeleteFile(uiDataInfo.contentInfoXmlPath);
	
	std::vector<contentInfo>::const_iterator itr = contentNodes.begin();
	for(; itr != contentNodes.end(); itr++)
	{
		carbonUtilities::cuDeleteFile(itr->contentInfoXmlPath);
	}	

	isSessionInitialized = false;

	return true;
}

//_TODO: useless function..remove it
bool PackageSession::processJob(IDAppNativeJob &job, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isSessionInitialized)
	{
		CARBONLOG_ERROR(logger, "Session is not initialized. Invalid call");
		return false;
	}

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
	xmlObj.addChildToNode(ResourceBaseFolderConst, sPackagePath, resourcesNode);
	xmlObj.addChildToNode(ContentTypeNodeName, resourcesInfo.contentType, resourcesNode);
	xmlObj.addChildToNode(InfoXmlNodeName, resourcesInfo.contentInfoXmlPath, resourcesNode);
	xmlObj.addChildToNode(resourcesNode, rootNode);

	//***********adding ui data node***********
	XMLNode uiDataNode;
	xmlObj.createNodeWithNameAndValue(UIDataNodeName,"",uiDataNode);
	xmlObj.addChildToNode(ResourceBaseFolderConst, sPackagePath, uiDataNode);
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

bool PackageSession::getSecretInfoFileFromDb(std::string &filePath)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	
	//db initialize
	if(!_pkgDb.isInitialized)
	{
		CARBONLOG_ERROR(logger, "[getSecretInfoFileFromDb] : package db is not initialized");
		return false;
	}

	const char ** outVal;
	aMapStr keyVal;
	keyVal["key"] = SecretInfoFileKeyName;
	if(!_pkgDb.getQueryResult("value", keyVal, "packageContent", &outVal))
	{
		CARBONLOG_ERROR(logger, "[getSecretInfoFileFromDb] : Failed to get the value from package db for key " <<SecretInfoFileKeyName);
		return false;
	}

	filePath = sPackagePath;

	filePath += *outVal;

	//TODO: remove this log
	CARBONLOG_INFO(logger, "[getSecretInfoFileFromDb] : The key file location is " << filePath.c_str());

	return true;
}


bool PackageSession::initializeContentDecryptor()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//db initialize
	if(!_pkgDb.isInitialized)
	{
		CARBONLOG_ERROR(logger, "[initializeContentDecryptor] : package db is not initialized");
		return false;
	}

	//make the bmp image file in temp folder
	string secretInfoFileLoc;
	if(!getSecretInfoFileFromDb(secretInfoFileLoc))
	{
		CARBONLOG_ERROR(logger, "[initializeContentDecryptor] : no info file present in package db");
		return false;
	}

	//get the keystream and make key container
	string keyStream;
	if(!carbonStegano::retrieveDataFromBmp(secretInfoFileLoc, keyStream))
	{
		CARBONLOG_ERROR(logger, "[initializeContentDecryptor] : Failed to get keystream from file");
		return false;
	}
	carboncipherUtilities::AESSecretKeyContainer _keyContainer;

	if(!_keyContainer.initKeysFromKeyStream(keyStream.c_str()))
	{
		CARBONLOG_ERROR(logger, "[initializeContentDecryptor] : Failed to init keys from keystream");
		return false;
	}

	//initialize the aes wrapper with the key container

	_contentDecoder.initializeCiphersFromKeys(_keyContainer);

	return true;

}



//_TODO: modify this function. take nativejob as input so that it can send the data from here only and then proceed to decrypt some more video files.
bool PackageSession::getContent(const std::string &argXml, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//see if it is initialized
	if(!isSessionInitialized)
	{
		CARBONLOG_ERROR(logger, "[PackageSession::getContent] : session is not a valid one");
		IDAppNativeJob::getErrorXmlNode(resStr, "Invalid Session");
		return false;
	}

	//get content name
	CarbonXMLParser xmlObj;
	if(!xmlObj.initWithXMLString(argXml))
	{
		CARBONLOG_ERROR(logger, "[PackageSession::getContent] : Failed to init with argument xml");
		return false;
	}

	string contentName;
	if(!xmlObj.stringValueForXQuery(ContentFileXquery, contentName))
	{
		CARBONLOG_ERROR(logger, "[PackageSession::getContent] : Failed to get content name from argument xml");
		return false;
	}

	string contentType;
	if(!xmlObj.stringValueForXQuery(ContentTypeXquery, contentType))
	{
		CARBONLOG_WARN(logger, "[PackageSession::getContent] : Failed to get content type from argument xml");
	}

	string outputContentPath;
	//is content already decoded
	std::map<string, string>::const_iterator itr = contentLocation.find(contentName);

	if( itr != contentLocation.end())
	{
		outputContentPath = itr->second;
	}

	else		//decode the content here
	{
		std::string contentPath = sPackagePath;
		contentPath  +=  contentName;

		string tempPath, tempFile;
		tempMgr.getNewTempFilePath(tempPath, tempFile);
		
		outputContentPath = tempPath;
		outputContentPath += Slash;
		outputContentPath += tempFile;

		OSString osOutPath, osInPath;
		carbonUtilities::cuConvertStringToOSString(outputContentPath, osOutPath);
		carbonUtilities::cuConvertStringToOSString(contentPath, osInPath);

		//TOOD: remove this bloody log from here
		CARBONLOG_TRACE(logger, "the output dir for content is " << outputContentPath.c_str());

		//TODO: only for testing
/*		if(!_contentDecoder.decryptFile(osInPath, osOutPath))
		{
			CARBONLOG_FATAL(logger, "[PackageSession::getContent] : Failed to decode the content ");
			IDAppNativeJob::getErrorXmlNode(resStr, "Failed to decode content");
			return false;
		}*/
		
		if(!carbonUtilities::cuCopyFile(osInPath, osOutPath))
		{
			CARBONLOG_FATAL(logger, "[PackageSession::getContent] : Failed to copy the content ");
			IDAppNativeJob::getErrorXmlNode(resStr, "Failed to copy content");
			return false;
		}


		//only this part should be there in critical section

		EnterPlatformCritSection(&PackageSessionCritSec);

		contentLocation[contentName] = outputContentPath;

		LeavePlatformCritSection(&PackageSessionCritSec);
	}

	//create content output location
	string outputXmlStr = "<outputXml/>";
	CarbonXMLParser outXmlObj;

	if(!outXmlObj.initWithXMLString(outputXmlStr))
	{
		CARBONLOG_ERROR(logger, "[PackageSession::getContent] : Failed to init xml object");
		return false;
	}

	XMLNode rootnode;
	outXmlObj.getRootNode(rootnode);

	outXmlObj.addChildToNode("contentFile", contentName, rootnode);
	outXmlObj.addChildToNode("type", contentType, rootnode);
	outXmlObj.addChildToNode("contentPath", outputContentPath, rootnode);

	outXmlObj.getXMLString(resStr);
	return true;
}

bool PackageSession::clearCachedContent(std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	EnterPlatformCritSection(&PackageSessionCritSec);

	if(!tempMgr.deleteAllTempContent())
	{
		CARBONLOG_WARN(logger, "[PackageSession::clearCachedContent] : Failed to clean temp folders");
	}

	contentLocation.clear();

	LeavePlatformCritSection(&PackageSessionCritSec);

	//output xml
	resStr = OutputXmlNode;

	return true;
}



bool PackageSession::populatePackageContentFromDb()
{
	//get resource data from db
	populateContentInfoStructFromdb(kResourceDataKeyName, this->resourcesInfo);

	populateContentInfoStructFromdb(kUIDataKeyName, this->uiDataInfo);

	//get content xmls here

	populatePackageContentXmlFromdb();

	return true;
}

bool PackageSession::populatePackageContentXmlFromdb()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	OSString contentInfoXmlPath;
	if(!extractXmlFromDb(ContentDetailKeyName, contentInfoXmlPath))
	{
		CARBONLOG_ERROR(logger, "[populatePackageContentXmlFromdb] : Failed to get the content xml from db for key " <<ContentDetailKeyName);
		return false;
	}

	CarbonXMLParser xmlObj;
	if(!xmlObj.initWithXMLPath(contentInfoXmlPath))
	{
		CARBONLOG_ERROR(logger, "[populatePackageContentXmlFromdb] : Failed to init the content info xml at path" <<contentInfoXmlPath.c_str());
		carbonUtilities::cuDeleteFile(contentInfoXmlPath);
		return false;
	}

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
			
			if( !xmlObj.stringValueForXQuery(curContentQuery + ContentIdentifierTagName, contentIdentifier) )
			{		
				CARBONLOG_ERROR(logger, "[populatePackageContentXmlFromdb] : Failed to get the identifier tag in the xml at path" <<contentInfoXmlPath.c_str());
				carbonUtilities::cuDeleteFile(contentInfoXmlPath);
				return false;
			}

			if( !xmlObj.stringValueForXQuery(curContentQuery + ContentTypeTagName, contentType) )
			{		
				CARBONLOG_ERROR(logger, "[populatePackageContentXmlFromdb] : Failed to get the type information from the xml at path" <<contentInfoXmlPath.c_str());
				carbonUtilities::cuDeleteFile(contentInfoXmlPath);
				return false;
			}

			//get the corresponding contentinfo
			contentInfo tempInfo;
			if(!populateContentInfoStructFromdb(contentIdentifier, tempInfo))
			{
				carbonUtilities::cuDeleteFile(contentInfoXmlPath);
				return false;
			}

			tempInfo.contentType = contentType;
			contentNodes.push_back(tempInfo);
		}
	}

	carbonUtilities::cuDeleteFile(contentInfoXmlPath);
	return true;

}


//it fetches the data from the packageContent table. thats all
bool PackageSession::populateContentInfoStructFromdb(const string &keyName, contentInfo &outVar)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	bool retVal;
	OSString outXmlPath;
	if(!extractXmlFromDb(keyName, outXmlPath))
	{
		CARBONLOG_ERROR(logger, "[populateContentInfoStructFromdb] : Failed to extract xml for key name - " <<keyName.c_str());
		outVar.contentInfoXmlPath = OSConst("");
		outVar.contentType = "error";
		retVal = false;
	}
	else
	{
		outVar.contentInfoXmlPath = outXmlPath;
		outVar.contentType = GenericContentTypeString;
		retVal = true;
	}

	return retVal;
}

//it extracts the xml only from the packageContent table in package db
bool PackageSession::extractXmlFromDb(const string &keyName, OSString &xmlPath)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	const char **Data;
	aMapStr keyVal;
	keyVal["key"] = keyName;
	if(!_pkgDb.getQueryResult("value", keyVal, "packageContent", &Data))
	{
		CARBONLOG_ERROR(logger, "[extractXmlFromDb] : Failed to get the value from package db for key " <<keyName);
		return false;
	}

	OSString tempFolder;
	carbonUtilities::cuGetTempPath(tempFolder);				//get the temp path

	bool retVal = false;

	if(*Data != NULL)
	{
		//TODO:
		//decode the xml. It should be present in the base64 manner

		//store in an entirely different place in temp folder. it should be erased only at the time of close

		OSString tempResourceFileName;
		carbonUtilities::cuGenerateGUID(tempResourceFileName);
		tempResourceFileName = tempFolder + tempResourceFileName;

		if(!carbonUtilities::cuCreateFileWithData((const void*)*Data, strlen(*Data), tempResourceFileName))
		{
			CARBONLOG_ERROR(logger, "[extractXmlFromDb] : Failed to write xml in the temp folder - " <<tempResourceFileName.c_str());
			retVal = false;
		}
		else
		{
//			carbonUtilities::cuConvertOSStringToString(tempResourceFileName, xmlPath);
			xmlPath = tempResourceFileName;
			retVal = true;
		}

		free((void*)*Data);
		free(Data);

	}

	return true;
}

