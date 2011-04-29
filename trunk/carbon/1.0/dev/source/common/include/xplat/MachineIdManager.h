
#pragma once
#include "Constants.h"
#include <comdef.h>
#include <Wbemidl.h>
#define _WIN32_DCOM

# pragma comment(lib, "wbemuuid.lib")

#define MachineIdDelimiter				"|@|"

#define machineIdRootNodeTag			"<machineId/>"

#define packageIdTagName				"PackageId"
#define processorIdTagName				"ProcessorId"
#define BaseBoardIdTagName				"BaseBoardId"
#define DiskDriveIdTagName				"DiskDriveId"
#define BiosIdTagName					"BiosId"


#define packageIdXquery					OSConst("/machineId/PackageId")
#define processorIdXquery				OSConst("/machineId/ProcessorId")
#define BaseBoardIdXquery				OSConst("/machineId/BaseBoardId")
#define DiskDriveIdXquery				OSConst("/machineId/DiskDriveId")
#define BiosIdXquery					OSConst("/machineId/BiosId")





class MachineIdMgr
{

private:

	bool isInitialized;
	IWbemLocator *pLoc;
	IWbemServices *pSvc;

	//this generates the id from the processor
	bool generateProcessorId(std::string &inId);
	//this validates the id 
	bool validateProcessorId(const std::string &inId);

	bool generateBaseBoardId(std::string &inId);
	bool validateBaseBoardId(const std::string &inId);

	bool generateDiskDriveId(std::string &inId);
	bool validateDiskDriveId(const std::string &inId);

	bool generateBiosId(std::string &inId);
	bool validateBiosId(const std::string &inId);

public:

	MachineIdMgr():isInitialized(false)
	{
	}

	~MachineIdMgr()
	{

	}

	bool Initialize();

	bool generateMachineId(std::string &macId);

	//_TODO: improve this algorithm
	bool validateMachineId(const std::string &inMacId);

	//inPkgId specifies whether to bind this machine id xml with the package id or not. Its for the robust design. Always use this.
	bool generateMachineIdXmlFile(std::string &outXml, const std::string inPkgId = "");
	bool ValidateMachineIdXmlFile(const std::string &inXml, const std::string inPkgId = "");

};

