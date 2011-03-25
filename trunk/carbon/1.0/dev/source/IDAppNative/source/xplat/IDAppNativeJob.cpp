
#include "NativeJob.h"
#include "IDAppConstants.h"
#include "CarbonXMLParser.h"

IDAppNativeJob::IDAppNativeJob():isJobForSession(false)
{


}

void IDAppNativeJob::getErrorXmlString(string &outXml, string errMsg)
{
	outXml = "<result>";

	outXml += "<CallIdentifier>";
	outXml += this->callIdentifier;
	outXml += "</CallIdentifier>";

	outXml += "<error>";
	if(errMsg.empty())
		outXml += "Failed to process xml";
	else
		outXml += errMsg;
	outXml += "</error>";

	outXml += "</result>";

}

//TODO: parse the xml and populate the fields
bool IDAppNativeJob::initJob(const std::string &inMsgStr)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	CarbonXMLParser xmlObj;
	if(!xmlObj.initWithXMLString(inMsgStr))
	{
		CARBONLOG_ERROR(logger, "[IDAppNativeJob::initJob] : Failed to init the xml obj");
		return false;
	}

	xmlObj.stringValueForXQuery(CallIdentifierXquery, this->callIdentifier);

	string temp = "";
	xmlObj.stringValueForXQuery(SessionBaseXquery, temp);
	if(temp == "true")
	{
		this->isJobForSession = true;
		xmlObj.stringValueForXQuery(SessionIdXquery, this->sessionId);
	}
	else
		this->isJobForSession = false;

	XMLNode argNode = NULL;
	argNode = xmlObj.getNodeForXQuery(ArgumentXmlNodeXquery);
	if(argNode != NULL)
	{
		xmlObj.getXMLStringOfNode(argNode, this->targetArgument);
	}

	xmlObj.stringValueForXQuery(TargetFunctionXquery, this->targetObject);
	return true;
}

void IDAppNativeJob::getOutputXmlString(const std::string &targetResultStr, std::string &outStr)
{	
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CarbonXMLParser xmlObj;

	if(!xmlObj.initWithXMLString(ResultXmlNode))
	{
		CARBONLOG_ERROR(logger, "[getOutputXmlString] : Failed to init the xml obj");
		getErrorXmlString(outStr);
		return;
	}

	XMLNode rootNode;
	xmlObj.getRootNode(rootNode);

	//**********adding call identifier node*********
	XMLNode callIdNode;
	xmlObj.createNodeWithNameAndValue(CallIdentifierNodeName, this->callIdentifier, callIdNode);
	xmlObj.addChildToNode(callIdNode, rootNode);

	//**********adding output xml****************
	CarbonXMLParser outputXmlParser;
	XMLNode outputXmlSubNode;

	if(!outputXmlParser.initWithXMLString(targetResultStr))
	{
		CARBONLOG_ERROR(logger, "[getOutputXmlString] : Failed to init the xml obj for outputXml node");
		getErrorXmlString(outStr);
		return;
	}

	outputXmlParser.getRootNode(outputXmlSubNode);
	xmlObj.addChildToNode(outputXmlSubNode, rootNode);

	xmlObj.getXMLString(outStr);

}



