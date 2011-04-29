
#pragma once

#include "Constants.h"
#include "Log.h"
#include "MachineIdManager.h"

//This class is responsible for activating a package.
class PkgActivationMgr
{

private:

	bool isInitialized;
	std::string serialNumber;
	MachineIdMgr _macMgr;

public:

	bool init(const std::string &inSerialNumber);

	bool validateSerialNumber();
	bool generateChallengeString(std::string &challengeStr);
	bool validateResponseString(const std::string &inResponseStr);
	bool generateResponseString(std::string &responseStr);

};

