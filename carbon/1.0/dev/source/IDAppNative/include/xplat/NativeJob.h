
#pragma once

#include "Constants.h"
#include "string"

//******************xml related constants**********

#define CallIdentifierXquery			OSConst("//message/callIdentifier")
#define MessageTypeXquery				OSConst("//message/messagetype")
#define SessionBaseXquery				OSConst("//message/sessionbased")
#define SessionIdXquery					OSConst("//message/sessionid")
#define ArgumentXmlNodeXquery			OSConst("//message/argumentXml")
#define TargetFunctionXquery			OSConst("//message/targetobject")

//***********************************

using namespace std;

class INativeJob
{

private:

public:

	// it populates the job parameters and variables using the string coming from UI
	virtual bool initJob(const std::string &inMsgString) = 0;

};

//complete this functionality
//two types of job. Universal or for a particular session
//TODO: rename this class. This can be used at many places
class IDAppNativeJob:public INativeJob
{
private:

	
public:

	IDAppNativeJob();

	std::string callIdentifier;		//unique identifier to identify the job request
	std::string targetObject;
	std::string sessionId;
	std::string targetArgument;
	std::string targetResult;
	bool isJobForSession;

	//parses the xml and populates the parameters
	bool initJob(const std::string &inMsgStr);

	//this constructs the output xml using the callIdentifier and the targetResult string and put the result in outStr
	void getOutputXmlString(std::string &outStr);

	//this constructs the output xml using the callIdentifier and the targetResultStr. The output is stored in outStr
	void getOutputXmlString(const std::string &targetResultStr, std::string &outStr);

	//this is similar to getOutputXmlString except that it doesn't use the xml parser. Since the error might have occured bcoz of xmlparser also
	//it uses the string concatenation to make the output message xml
	void getErrorXmlString(string &outXml, string errMsg = "");

	static void getErrorXmlNode(std::string &outStr, const std::string errTitle = "", const std::string errMsg = "", const std::string errCode = "");

};

