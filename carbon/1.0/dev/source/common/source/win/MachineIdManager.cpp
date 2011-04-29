
#include "MachineIdManager.h"
#include "Utilities.h"
#include "CarbonXMLParser.h"

#include <iostream>
using namespace std;

#include "Log.h"

int countThreshold = 3;			//at least these many properties should match to validate a machine id
int propertiesIncluded = 4;

//this generates the id from the processor. In any case, it will either return empty string or a valid string.
//bool MachineIdMgr::return is just for fun... 
bool MachineIdMgr::generateProcessorId(std::string &inId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	inId = "";

	IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT  hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_Processor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger,"Query for operating system name failed."
            << " Error code = 0x" 
            << hex << hres);
        return false;               // Program has failed.
    }

    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
	//get only the first instance
	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

	if(0 == uReturn)
	{
		return false;
	}

	VARIANT vtProp;

	// Get the value of the ProcessorId property
	hr = pclsObj->Get(L"ProcessorId", 0, &vtProp, 0, 0);
	string tempStr;

	if (SUCCEEDED(hr) && (V_VT(&vtProp) == VT_BSTR))
	{
		carbonUtilities::cuConvertOSStringToString(vtProp.bstrVal, tempStr);
		inId += tempStr;
	}
	else
	{
		CARBONLOG_ERROR(logger,"[generateProcessorId] : Error in getting specified object\n");
	}

	VariantClear(&vtProp);

    pEnumerator->Release();
    pclsObj->Release();
	return true;

}

//this validates the id 
bool MachineIdMgr::validateProcessorId(const std::string &inId)
{
	if(!isInitialized)
		return false;

	std::string tempId;
	generateProcessorId(tempId);

	return (inId == tempId);

}

bool MachineIdMgr::generateBaseBoardId(std::string &inId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	inId = "";

	IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT  hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_BaseBoard"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger,"Query for operating system name failed."
            << " Error code = 0x" 
            << hex << hres);
        return false;               // Program has failed.
    }

    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
	//get only the first instance
	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

	if(0 == uReturn)
	{
		return false;
	}

	VARIANT vtProp;

	// Get the value of the ProcessorId property
	hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
	string tempStr;

	if (SUCCEEDED(hr) && (V_VT(&vtProp) == VT_BSTR))
	{
		carbonUtilities::cuConvertOSStringToString(vtProp.bstrVal, tempStr);
		inId += tempStr;
	}
	else
	{
		CARBONLOG_ERROR(logger,"[generateBaseBoardId] : Error in getting specified object\n");
	}

	VariantClear(&vtProp);

    pEnumerator->Release();
    pclsObj->Release();
	return true;

}

bool MachineIdMgr::validateBaseBoardId(const std::string &inId)
{
	if(!isInitialized)
	return false;

	std::string tempId;
	generateBaseBoardId(tempId);

	return (inId == tempId);
}

bool MachineIdMgr::generateDiskDriveId(std::string &inId)
{
		CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	inId = "";

	IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT  hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_DiskDrive"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger,"Query for operating system name failed."
            << " Error code = 0x" 
            << hex << hres);
        return false;               // Program has failed.
    }

    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
	//get only the first instance
	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

	if(0 == uReturn)
	{
		return false;
	}

	VARIANT vtProp;

	// Get the value of the ProcessorId property
	hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
	string tempStr;

	if (SUCCEEDED(hr) && (V_VT(&vtProp) == VT_BSTR))
	{
		carbonUtilities::cuConvertOSStringToString(vtProp.bstrVal, tempStr);
		inId += tempStr;
	}
	else
	{
		CARBONLOG_ERROR(logger,"[generateBaseBoardId] : Error in getting specified object\n");
	}

	VariantClear(&vtProp);

    pEnumerator->Release();
    pclsObj->Release();
	return true;

}

bool MachineIdMgr::validateDiskDriveId(const std::string &inId)
{
	if(!isInitialized)
	return false;

	std::string tempId;
	generateDiskDriveId(tempId);

	return (inId == tempId);

}

bool MachineIdMgr::generateBiosId(std::string &inId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	inId = "";

	IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT  hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_BIOS"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger,"Query for operating system name failed."
            << " Error code = 0x" 
            << hex << hres);
        return false;               // Program has failed.
    }

    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
	//get only the first instance
	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

	if(0 == uReturn)
	{
		return false;
	}

	VARIANT vtProp;

	// Get the value of the ProcessorId property
	hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
	string tempStr;

	if (SUCCEEDED(hr) && (V_VT(&vtProp) == VT_BSTR))
	{
		carbonUtilities::cuConvertOSStringToString(vtProp.bstrVal, tempStr);
		inId += tempStr;
	}
	else
	{
		CARBONLOG_ERROR(logger,"[generateBaseBoardId] : Error in getting specified object\n");
	}

	VariantClear(&vtProp);

    pEnumerator->Release();
    pclsObj->Release();
	return true;
}

bool MachineIdMgr::validateBiosId(const std::string &inId)
{
	if(!isInitialized)
	return false;

	std::string tempId;
	generateBiosId(tempId);

	return (inId == tempId);
}


//this initializes the COM 
bool MachineIdMgr::Initialize()
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    //hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    //if (FAILED(hres))
    //{
    //    cout << "Failed to initialize COM library. Error code = 0x" 
    //        << hex << hres << endl;
    //    return 1;                  // Program has failed.
    //}

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

/*    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger, ("Failed to initialize security. Error code = 0x")
            << hex << hres);
        return false;                    // Program has failed.
    }
 */   
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger, "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres);
      
        return false;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    pSvc = NULL;
	
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger, "Could not connect. Error code = 0x" 
             << hex << hres);
        pLoc->Release();     
		return false;                // Program has failed.
    }



    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        CARBONLOG_ERROR(logger, "Could not set proxy blanket. Error code = 0x" 
            << hex << hres);
        pSvc->Release();
        pLoc->Release();     
        return false;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

	isInitialized = true;


	return true;
}


//The machine id consists of various Ids which are separated by the delimited ------> |@| <------. 
//Hopefully this delimited is not present in any other id
//_TODO: make it more robust by taking more parameters into account. see this article : http://stackoverflow.com/questions/99880/generating-a-unique-machine-id
bool MachineIdMgr::generateMachineId(std::string &macId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isInitialized  && !Initialize())
	{
		CARBONLOG_ERROR(logger, L"[generateMachineId] : Failed to initialize COM.");
		return false;
	}

	std::string temp = "";

	macId = "";
	generateProcessorId(temp);
	macId += temp;
	
	temp = "";
	macId += MachineIdDelimiter;
	generateBaseBoardId(temp);
	macId += temp;

	temp = "";
	macId += MachineIdDelimiter;
	generateDiskDriveId(temp);
	macId += temp;
	
	temp = "";
	macId += MachineIdDelimiter;
	generateBiosId(temp);
	macId += temp;
	
	return true;

}

//it seperates the string using the delimited. and then it 
bool MachineIdMgr::validateMachineId(const std::string &inMacId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	
	if(!isInitialized  && !Initialize())
	{
		CARBONLOG_ERROR(logger, L"[validateMachineId] : Failed to initialize COM.");
		return false;
	}

	std::string tempSubStr = inMacId;
	size_t pos;

	std::string tempId = "";
	int countMatched = 0;

	//get processor id
	pos = tempSubStr.find_first_of(MachineIdDelimiter);
	tempId = tempSubStr.substr(0, pos);
	tempSubStr = tempSubStr.substr(pos);
	if(validateProcessorId(tempId))
		countMatched ++;

	//get BaseBoard Id
	pos = tempSubStr.find_first_of(MachineIdDelimiter);
	tempId = tempSubStr.substr(0, pos);
	tempSubStr = tempSubStr.substr(pos);
	if(validateBaseBoardId(tempId))
		countMatched ++;


	//get disk drive Id
	pos = tempSubStr.find_first_of(MachineIdDelimiter);
	tempId = tempSubStr.substr(0, pos);
	tempSubStr = tempSubStr.substr(pos);
	if(validateDiskDriveId(tempId))
		countMatched ++;
	

	//get bios id
	pos = tempSubStr.find_first_of(MachineIdDelimiter);
	tempId = tempSubStr.substr(0, pos);
	tempSubStr = tempSubStr.substr(pos);
	if(validateBiosId(tempId))
		countMatched ++;

	if(countMatched >= countThreshold)
		return true;

	return false;

}


//the xml structure is like:
/*
<machineId>
	<pkgId>....</pkgId>
	<ProcessorId>...</ProcessorId>
	<Property2>...</Property2>
</machineId>

*/
bool MachineIdMgr::generateMachineIdXmlFile(std::string &outXml, const std::string inPkgId)
{

	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());

	if(!isInitialized)
	{
		return false;
	}

	CarbonXMLParser xmlObj;

	xmlObj.initWithXMLString(machineIdRootNodeTag);

	XMLNode rootNode;
	if(!xmlObj.getRootNode(rootNode))
	{
		CARBONLOG_ERROR(logger, "[generateMachineIdXmlFile] : Failed to get root node");
		return false;
	}

	if(!inPkgId.empty())
	{
		xmlObj.addChildToNode(packageIdTagName, inPkgId, rootNode);
	}

	std::string propVal = "";
	generateProcessorId(propVal);
	xmlObj.addChildToNode(processorIdTagName, propVal, rootNode);

	propVal = "";
	generateBaseBoardId(propVal);
	xmlObj.addChildToNode(BaseBoardIdTagName, propVal, rootNode);

	propVal = "";
	generateDiskDriveId(propVal);
	xmlObj.addChildToNode(DiskDriveIdTagName, propVal, rootNode);

	propVal = "";
	generateBiosId(propVal);
	xmlObj.addChildToNode(BiosIdTagName, propVal, rootNode);

	if(!xmlObj.getXMLString(outXml))
	{
		CARBONLOG_ERROR(logger, "[generateMachineIdXmlFile] : Failed to get xml string");
		return false;
	}

	return true;
}

bool MachineIdMgr::ValidateMachineIdXmlFile(const std::string &inXml, const std::string inPkgId)
{
	CARBONLOG_CLASS_PTR logger(carbonLogger::getLoggerPtr());
	CarbonXMLParser xmlObj;

	int countMatched = 0;

	if(!xmlObj.initWithXMLString(inXml))
	{
		CARBONLOG_ERROR(logger, "[ValidateMachineIdXmlFile] : Failed to init with xml string");
		return false;
	}

	string propVal = "";
	if(!inPkgId.empty())
	{
		if(!xmlObj.stringValueForXQuery(packageIdXquery,propVal) || (inPkgId.compare(propVal)!=0))
		{
			CARBONLOG_ERROR(logger, "[ValidateMachineIdXmlFile] : package id");
			return false;
		}
	}

	propVal = "";
	if(!xmlObj.stringValueForXQuery(processorIdXquery,propVal))
	{
		CARBONLOG_ERROR(logger, "[ValidateMachineIdXmlFile] : first prop");
		return false;
	}
	if(validateProcessorId(propVal))
		countMatched ++;

	propVal = "";
	if(!xmlObj.stringValueForXQuery(BaseBoardIdXquery,propVal))
	{
		CARBONLOG_ERROR(logger, "[ValidateMachineIdXmlFile] : second prop");
		return false;
	}
	if(validateBaseBoardId(propVal))
		countMatched ++;

	propVal = "";
	if(!xmlObj.stringValueForXQuery(DiskDriveIdXquery,propVal))
	{
		CARBONLOG_ERROR(logger, "[ValidateMachineIdXmlFile] : third prop");
		return false;
	}
	if(validateDiskDriveId(propVal))
		countMatched ++;

	propVal = "";
	if(!xmlObj.stringValueForXQuery(BiosIdXquery,propVal))
	{
		CARBONLOG_ERROR(logger, "[ValidateMachineIdXmlFile] : forth prop");
		return false;
	}
	if(validateBiosId(propVal))
		countMatched ++;

	return (countMatched >= countThreshold);

}

