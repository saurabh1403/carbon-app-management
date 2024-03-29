
//conatins the main functionality of the IDApp Native interactivity with the UI

#include "IDAppNative.h"
#include "IDAppConstants.h"
#include "CarbonXMLParser.h"
#include <vector>

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

void closeAllPackageSessions()
{
	std::vector<string> sessionIds;

	SessionMgr<PackageSession>::getInstance().getAllSessions(sessionIds);
	
	std::vector<string>::const_iterator itr = sessionIds.begin();

	for(; itr!= sessionIds.end(); itr++)
		closePackageSessionWithId(*itr);

}

bool closePackageSessionWithId(const std::string &pkgSessionId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	bool retVal = true;
	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(pkgSessionId);

	if(pkgSession != NULL)
	{
		if(!pkgSession->closeSession())
		{
			//no logging. already must have been done
			retVal = false;			
		}
		else
		{
			retVal = true;			
		}

		//delete session as well as remove entry
		SessionMgr<PackageSession>::getInstance().removeSessionWithId(pkgSessionId);
		delete pkgSession;

	}

	else
	{
		CARBONLOG_ERROR(logger, "[closePackageSession] : No Session exists for the session id "<<pkgSessionId);
		retVal = false;
	}

	return retVal;

}

void startPackageSession(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
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
		CARBONLOG_ERROR(logger, "[startPackageSession] : Failed to get package id from target argument string");
		inJob.getErrorXmlString(outMsg);
		return;
	}

	OSString pkgPath;
	if(!IDAppGlobalContext::getInstance().getProdDbHandle().getPackageDirectory(pkgId, pkgPath))	//this is the absolute path to the package directory
	{
		string _temp;
		CARBONLOG_ERROR(logger, "[startPackageSession] : Failed to get package path");
		IDAppNativeJob::getErrorXmlNode(_temp, "Error getting package directory");
		inJob.getErrorXmlString(outMsg, _temp);
		return;
	}

	std::string resStr = "";

	//check whether the session is already opened
	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(inJob.sessionId);

	if(pkgSession != NULL)		//already opened. call the init again. thats all
	{
		if(!pkgSession->initSession(pkgPath, pkgId, resStr))
		{
			string _temp;
//			IDAppNativeJob::getErrorXmlNode(_temp, "Failed to init the session");
			inJob.getErrorXmlString(outMsg, resStr);
		}
		else
			inJob.getOutputXmlString(OutputXmlNode, outMsg);
		return;	
	}

	else
	{
		//*****open the package session and make an entry in the session manager
		pkgSession = new PackageSession();

		if(!pkgSession->initSession(pkgPath, pkgId, resStr ))
		{
			string temp;
			CARBONLOG_ERROR(logger, "[startPackageSession] : Failed to initialize the package session");
//			IDAppNativeJob::getErrorXmlNode(temp,"Error Initializing Package Session");
			inJob.getErrorXmlString(outMsg,resStr);
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
	CARBONLOG_TRACE(logger,"in getContent routine");

	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(inJob.sessionId);

	if(pkgSession != NULL)
	{
		std::string outputXml;
		if(!pkgSession->getContent(inJob.targetArgument, outputXml))
		{
			//no logging. already must have been done
			inJob.getErrorXmlString(outputXml, outMsg);
			return;
		}
		else
		{
			inJob.getOutputXmlString(outputXml, outMsg);
			return;
		}
	}

	else
	{
		CARBONLOG_ERROR(logger, "[getContent] : No Session exists for the session id "<<inJob.sessionId);
		inJob.getErrorXmlString(outMsg, "The Package Session id is invaid");
	}

}


//closing a particular package session
void closePackageSession(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	outMsg = "closePackageSession function routine"; 
	CARBONLOG_TRACE(logger,"in closePackageSession routine");

	if(!closePackageSessionWithId(inJob.sessionId))
	{
		inJob.getErrorXmlString(outMsg, "Failed to close the package session or invalid session");
	}
	else
	{
		inJob.getOutputXmlString(OutputXmlNode, outMsg);
	}

	return;

}

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
		xmlObj.createNodeWithNameAndValue("pkg","",pkgNode);
		xmlObj.addChildToNode("Name", itr->name, pkgNode);
		xmlObj.addChildToNode("packageId", itr->pkgId, pkgNode);
		xmlObj.addChildToNode("titleText", itr->titleText, pkgNode);
		xmlObj.addChildToNode("mainIconPath", itr->coverPageIconPath, pkgNode);
		xmlObj.addChildToNode("thumbnailIconPath", itr->thumbnailIconPath, pkgNode);
		xmlObj.addChildToNode("description", itr->description, pkgNode);
		xmlObj.addChildToNode("pkgInstalledDir", itr->pkgPath, pkgNode);

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

	CARBONLOG_TRACE(logger,"out of getAvailablePackages routine");
//	CARBONLOG_DEBUG(logger, "[getAvailablePackages] : The xml constructed is "<<tempStr.c_str());
}


void getPackageSessionData(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CARBONLOG_TRACE(logger,"in getPackageSessionData routine");

	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(inJob.sessionId);

	if(pkgSession != NULL)
	{
		std::string outputXml;

		if(pkgSession->getPackageContentInfo(outputXml))
			inJob.getOutputXmlString(outputXml, outMsg);
		else
			inJob.getErrorXmlString(outputXml, outMsg);

		return;
	}

	else
	{
		CARBONLOG_ERROR(logger, "[getPackageSessionData] : No Session exists for the session id "<<inJob.sessionId);
		inJob.getErrorXmlString(outMsg, "The Package Session id is invaid");
	}

}



void clearCachedContent(IDAppNativeJob &inJob, std::string &outMsg)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CARBONLOG_TRACE(logger,"in clearCachedContent routine");

	PackageSession *pkgSession = SessionMgr<PackageSession>::getInstance().getSessionWithId(inJob.sessionId);

	if(pkgSession != NULL)
	{
		std::string outputXml;
		if(!pkgSession->clearCachedContent(outputXml))
		{
			//no logging. already must have been done
			inJob.getErrorXmlString(outputXml, outMsg);
			return;
		}
		else
		{
			inJob.getOutputXmlString(outputXml, outMsg);
			return;
		}
	}

	else
	{
		CARBONLOG_ERROR(logger, "[clearCachedContent] : No Session exists for the session id "<<inJob.sessionId);
		inJob.getErrorXmlString(outMsg, "The Package Session id is invaid");
	}


}
