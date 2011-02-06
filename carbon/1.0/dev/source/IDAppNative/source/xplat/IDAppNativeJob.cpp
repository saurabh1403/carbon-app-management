
#include "NativeJob.h"

IDAppNativeJob::IDAppNativeJob():isJobForSession(false)
{


}

//TODO: parse the xml and populate the fields
bool IDAppNativeJob::initJob(const std::string &inMsgStr)
{
	this->targetObject = inMsgStr;
	return true;
}



