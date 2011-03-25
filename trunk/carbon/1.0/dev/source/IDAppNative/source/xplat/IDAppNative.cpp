
//conatins the main functionality of the IDApp Native interactivity with the UI

#include "IDAppNative.h"
#include "IDAppConstants.h"
#include "CarbonXMLParser.h"

typedef void (PackageSession::*FNMETHOD) ( IDAppNativeJob &job, std::string &resStr);

void processJob(const std::string &jobMsg, std::string &resStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	//get the session id. if it is null or empty, process it itself. else pass it to process to the asked session
	IDAppNativeJob obj;
	obj.initJob(jobMsg);

	IDAppGlobalContext::getInstance().executeFunctionWithParameters(obj, resStr);

	//here do all the work. Cases are:

	//1. get universal information

	//2. create a package session for IDApp

	//3. close all the sessions

}


void startPackageSession(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	outMsg = "startPackageSession function routine"; 
	CARBONLOG_TRACE(logger,"in startPackageSession routine");

	//*****get the package information
	CarbonXMLParser xmlArg;

	if(!xmlArg.initWithXMLString(inJob.targetArgument))
	{
		CARBONLOG_ERROR(logger, "[startPackageSession] : Failed to init xml obj with target argument string");
		inJob.getErrorXmlString(outMsg);
		return;
	}

	std::string pkgId;
	if(!xmlArg.stringValueForXQuery(PackageIdNodeXquery, pkgId))
	{
		CARBONLOG_ERROR(logger, "[startPackageSession] : Failed to get package id with target argument string");
		inJob.getErrorXmlString(outMsg);
		return;
	}

	OSString pkgPath;
	IDAppGlobalContext::getInstance().getProdDbHandle().getPackageDirectory(pkgId, pkgPath);	//this is the absolute path to the package directory

	//check whether the session is already opened
	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(inJob.sessionId);

	if(pkgSession != NULL)		//already opened. call the init again. thats all
	{
		pkgSession->initSession(pkgPath);
		inJob.getOutputXmlString(OutputXmlNode, outMsg);
		return;
	}

	else
	{
		//*****open the package session and make an entry in the session manager
		pkgSession = new PackageSession();

		if(!pkgSession->initSession(pkgPath))
		{
			CARBONLOG_ERROR(logger, "[startPackageSession] : Failed to initialize the package session");
			inJob.getErrorXmlString(outMsg, "Error Initializing Package");
			return;
		}

		//pkg Id would be the key for a session
		SessionMgr<PackageSession>::getInstance().createSession(pkgId, pkgSession);

		//********create the success output xml**********
		inJob.getOutputXmlString(OutputXmlNode, outMsg);
	}

}


void getContent(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	outMsg = "processSessionJob function routine"; 
	CARBONLOG_TRACE(logger,"in processSessionJob routine");

	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(inJob.sessionId);

	if(pkgSession != NULL)
		pkgSession->processJob(inJob, outMsg);

	else
		CARBONLOG_ERROR(logger, "No Session exists for the session id "<<inJob.sessionId);

}


//closing a particular package session
void closePackageSession(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	outMsg = "closePackageSession function routine"; 
	CARBONLOG_TRACE(logger,"in closePackageSession routine");

	//make the proper entries in license state

}

//closing of whole IDApp
void closeIDAppSession(IDAppNativeJob &inJob, std::string &outMsg)
{
	IDAppGlobalContext::getInstance().handleIDAppCloseSignal();

}

void getAvailablePackages(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	outMsg = "getAvailablePackages function routine"; 
	CARBONLOG_TRACE(logger,"in getAvailablePackages routine");

	vector<packageInfo> pkgPresent;

	IDAppGlobalContext::getInstance().getProdDbHandle().getInstalledPackages(pkgPresent);

	//TODO: construct the xml to be sent here
	std::string outXmlTemp = OutputXmlNode;
	CarbonXMLParser xmlObj;

	if(!xmlObj.initWithXMLString(outXmlTemp))
	{
		CARBONLOG_ERROR(logger, "[getAvailablePackages] : Failed to init the xml obj");
		inJob.getErrorXmlString(outMsg);
		return;
	}

	XMLNode pkgListNode;
	xmlObj.createNodeWithNameAndValue("packageList","",pkgListNode);

	vector<packageInfo>::const_iterator itr = pkgPresent.begin();
	for(; itr != pkgPresent.end(); itr++)
	{
		XMLNode pkgNode;
		xmlObj.createNodeWithNameAndValue("package","",pkgNode);
		xmlObj.addChildToNode("Name", itr->name, pkgNode);
		xmlObj.addChildToNode("packageId", itr->pkgId, pkgNode);
		xmlObj.addChildToNode("titleText", itr->titleText, pkgNode);
		xmlObj.addChildToNode("imageIconPath", itr->imageIconPath, pkgNode);

		if(!xmlObj.addChildToNode(pkgNode, pkgListNode))
		{
			CARBONLOG_ERROR(logger, "[getAvailablePackages] : Failed to add child to xml obj");
			inJob.getErrorXmlString(outMsg);
			return;
		}
	}

	XMLNode rootNode;
	xmlObj.getRootNode(rootNode);
	xmlObj.addChildToNode(pkgListNode, rootNode);

	string tempStr;
	xmlObj.getXMLString(tempStr);

	inJob.getOutputXmlString(tempStr, outMsg);

	CARBONLOG_DEBUG(logger, "[getAvailablePackages] : The xml constructed is "<<tempStr.c_str());
}

