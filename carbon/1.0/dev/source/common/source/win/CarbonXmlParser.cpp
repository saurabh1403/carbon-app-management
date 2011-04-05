
#include "CarbonXMLParser.h"
#include "Utilities.h"
using namespace std;



CarbonXMLParser::CarbonXMLParser()
{
	docRef = NULL;
}

CarbonXMLParser::~CarbonXMLParser()
{
	//((MSXML2::IXMLDOMDocument2 *)docRef)->Release();

	//docRef->Release();
	//docRef=NULL;
}

bool CarbonXMLParser::initWithXMLPath(const std::string &xmlFilePath)
{
	OSString tempPath;
	carbonUtilities::cuConvertStringToOSString(xmlFilePath, tempPath);
	return initWithXMLPath(tempPath);

}

bool CarbonXMLParser::initWithXMLPath(const OSString &xmlFilePath)
{
	MSXML2::IXMLDOMDocument2Ptr     pXMLDoc = NULL;
	HRESULT hr=NULL;
	try
	{
		hr=pXMLDoc.CreateInstance(__uuidof(DOMDocument));
   		if(pXMLDoc)
		{
			pXMLDoc->async = VARIANT_FALSE;
		}
		else
		{
			return false;
		}
		if(pXMLDoc->load(xmlFilePath.c_str()) != VARIANT_TRUE)
		{
			return false;
		}
	}
	catch(_com_error & e)
	{
		return false;
	}
	docRef=pXMLDoc;
	return true;
}

bool CarbonXMLParser::initWithXMLString( const OSString & xmlString )
{
	MSXML2::IXMLDOMDocument2Ptr     pXMLDoc = NULL;
	pXMLDoc.CreateInstance("MSXML2.DOMDocument.3.0");
	TCHAR temp[MAX_PATH];
	try
	{
		if(pXMLDoc)
		{
			pXMLDoc->async = VARIANT_FALSE;
		}
		else
		{
			return false;
		}

		if(pXMLDoc->loadXML(xmlString.c_str()) != VARIANT_TRUE)
		{
			return false;
		}
	}
	catch(_com_error & e)
	{
		TCHAR mssg[MAX_PATH];
		_stprintf(mssg,L"Error Occured%s\n",e.Description());
		////Logger::getInstance()->logError(kLogModule,"Error Occured%s\n",e.Description());
		return false;
	}
	docRef=pXMLDoc;
	return true;
}

bool CarbonXMLParser::isInitialized()
{
	if(docRef)
	{
		return true;
	}
	
	return false;
}

bool CarbonXMLParser::nodeExistsXQuery( const OSString & xQuery )
{
	MSXML2::IXMLDOMDocument2Ptr     pXMLDoc = NULL;
	MSXML2::IXMLDOMNodeListPtr      listPtr = NULL;
	long nodeCount=0;
	if(isInitialized())
	{
		listPtr=docRef->selectNodes(xQuery.c_str());
		listPtr->get_length(&nodeCount);
		if(nodeCount)
		{
			return true;
		}
		else
		{
//			fprintf(stderr,"Failed to find Node ");
			return false;
		}
	}
	else
	{
		return true;
	}
	return true;
}


bool  CarbonXMLParser::stringValueForXQuery( const OSString & xQuery, OSString & outValue )
{
	MSXML2::IXMLDOMDocument2Ptr     pXMLDoc = NULL;
	MSXML2::IXMLDOMNodePtr          nodePtr = NULL;
	MSXML2::IXMLDOMNodeListPtr      listPtr = NULL;
	long nodeCount=0;
	//NativeChar *outVal;
	_bstr_t outVal;
	if(isInitialized())
	{
		//pXMLDoc=(MSXML2::IXMLDOMDocument2 *)docRef;
		if(nodeExistsXQuery(xQuery))
		{
			listPtr = docRef->selectNodes(xQuery.c_str());
			listPtr->get_length(&nodeCount);

			if(nodeCount)
			{
				listPtr->get_item(0, &nodePtr);
				if(nodePtr)
				{
					//nodePtr->get_text(&outVal);
					outVal = nodePtr->Gettext();
					outValue = (LPCTSTR)outVal;
					//SysFreeString(outVal);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
	return true;
}

bool CarbonXMLParser::getNodeFirstChildString(XMLNode node, OSString & outValue)
{
	if( !node )
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeFirstChildString ... node is null."));
		return false;
	}
 
	MSXML2::IXMLDOMNodePtr          nodePtr = NULL;
	MSXML2::IXMLDOMNodeListPtr      listPtr = NULL;
	long nodeCount=0;
	if(isInitialized())
	{
		listPtr = node->GetchildNodes();
		listPtr->get_length(&nodeCount);
		if(nodeCount)
		{
			listPtr->get_item(0, &nodePtr);
		}
		else
			return false;
		BSTR val;
		HRESULT hr = nodePtr->get_xml(&val);
		if(!FAILED(hr))
		{
			outValue = (LPCTSTR)val;
			SysFreeString(val);
			return true;
		}
		else
			return false;
	}

	//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
	return false;

}

XMLNode CarbonXMLParser::getNodeForXQuery( const OSString & xQuery )
{
	MSXML2::IXMLDOMNodePtr          nodePtr = NULL;
	MSXML2::IXMLDOMNodeListPtr      listPtr = NULL;
	long nodeCount=0;
	if(isInitialized())
	{
		if(nodeExistsXQuery(xQuery))
		{
			listPtr = docRef->selectNodes(xQuery.c_str());
			listPtr->get_length(&nodeCount);
			if(nodeCount)
			{
				listPtr->get_item(0, &nodePtr);
			}
		}
	}

	//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
	return nodePtr;
}
XMLNodeList CarbonXMLParser::getNodeListForXQuery(const OSString & xQuery, int* outCount)
{
	MSXML2::IXMLDOMNodeListPtr      listPtr = NULL;
	long nodeCount=0;
	if(isInitialized())
	{
		if(nodeExistsXQuery(xQuery))
		{
			listPtr = docRef->selectNodes(xQuery.c_str());
			listPtr->get_length(&nodeCount);
			if( outCount != NULL )
				*outCount=nodeCount;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeListForXQuery ... CarbonXMLParser not Initialized ..."));
	}
	return listPtr;
}
XMLNode CarbonXMLParser::getNodeAtIndexFromNodeList( XMLNodeList aList, int index )
{
	if( !aList )
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeAtIndexFromNodeList ... NodeList is null."));
		return NULL;
	}

	MSXML2::IXMLDOMNodeListPtr listPtr=(MSXML2::IXMLDOMNodeList *) aList;
	MSXML2::IXMLDOMNodePtr nodePtr=NULL;
	if(isInitialized())
	{
		if(listPtr)
		{
			listPtr->get_item(index, &nodePtr);
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeListForXQuery ... CarbonXMLParser not Initialized ..."));
	}
	return nodePtr;
}

StrMap CarbonXMLParser::getAttrsMapOfNode(XMLNode aNode)
{
	StrMap resMap;
	if( !aNode )
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getMapFromNode ... Node is null."));
		return resMap;
	}
	
	MSXML2::IXMLDOMNodePtr nodePtr=(MSXML2::IXMLDOMNode *) aNode;
	MSXML2::IXMLDOMNamedNodeMapPtr aNodeMap;
	if(isInitialized())
	{
		HRESULT  hr = nodePtr->get_attributes(&aNodeMap);
		if( FAILED( hr ) )
		{
			//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("Failed to get Attributes from the Node :%d"),GetLastError());
			return resMap;
		}

		long length=0;
		hr = aNodeMap->get_length( &length );
		if( FAILED( hr ) )
		{
			//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("Failed to number of Attributes for the Node :%d"),GetLastError());
			return resMap;
		}
		int iter =0;
		BSTR bptr;
		while( iter < length )
		{
			MSXML2::IXMLDOMNodePtr attributeNode = aNodeMap->Getitem(iter);
			bptr = attributeNode->GetnodeName();
			if(!bptr)
			{
              //Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("Failed to get Node name :%d"),GetLastError());
			  return resMap;
			}
			OSString nodeName = (LPCTSTR) bptr;
			SysFreeString(bptr);
			
			VARIANT vText; 
			hr = attributeNode->get_nodeValue(&vText);
			if( FAILED(hr) )
			{
			   //Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("Failed to get node Value :%d"),GetLastError());
			   return resMap;
			}
			bptr = vText.bstrVal;
			OSString  baseValue = (LPCTSTR) bptr;
			SysFreeString(bptr);

			resMap.insert(pair<OSString,OSString>(nodeName, baseValue));
			iter++;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getAttrsMapOfNode ... CarbonXMLParser not Initialized ..."));
	}

	return resMap;
}
StrMap CarbonXMLParser::getMapFromNode( XMLNode aNode )
{
	StrMap res;
	if( !aNode )
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getMapFromNode ... Node is null."));
		return res;
	}

	MSXML2::IXMLDOMNodePtr nodePtr=(MSXML2::IXMLDOMNode *) aNode;
	MSXML2::IXMLDOMNodePtr childNodePtr=NULL;
	MSXML2::IXMLDOMNodeListPtr listPtr=NULL;
	long nodeCount=0;
	int iter=0;
	if(isInitialized())
	{
		if(nodePtr->hasChildNodes())
		{
			nodePtr->get_childNodes(&listPtr);
			listPtr->get_length(&nodeCount);
			if(nodeCount)
			{
				while(iter<nodeCount)
				{

					//TCHAR* outValue=NULL;
					_bstr_t outValue;
					TCHAR baseName[MAX_PATH]={'\0'};

					childNodePtr = NULL;
					listPtr->get_item(iter, &childNodePtr);

					_tcscpy(baseName,childNodePtr->nodeName);
					//childNodePtr->get_text(&outValue);
					outValue = childNodePtr->Gettext();
					if((baseName)&&(outValue.length()))
					{
						res.insert(pair<OSString,OSString>(baseName, (LPCTSTR)outValue));
					}
					iter++;
				}
			}
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getMapFromNode ... CarbonXMLParser not Initialized ..."));
	}
	return res;
}

StrMap CarbonXMLParser::getMapOfNode( XMLNode aNode )
{
	StrMap res;
	if( !aNode )
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getMapOfNode ... Node is null."));
		return res;
	}

	MSXML2::IXMLDOMNodePtr nodePtr=(MSXML2::IXMLDOMNode *) aNode;
	if(isInitialized())
	{
		//TCHAR* outValue=NULL;
		//BSTR outValue;
		_bstr_t outValue;
		TCHAR baseName[MAX_PATH]={'\0'};

		_tcscpy(baseName,nodePtr->nodeName);
		//nodePtr->get_text(&outValue);
		outValue = nodePtr->Gettext();
		if((baseName)&&(outValue.length()))
		{
			res.insert(pair<OSString,OSString>(baseName, (LPCTSTR)outValue));
		}
		//SysFreeString(outValue);
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getMapOfNode ... CarbonXMLParser not Initialized ..."));
	}
	return res;
}


bool CarbonXMLParser::getRootNode(XMLNode & root)
{
	if(isInitialized())
	{
		root = docRef->documentElement;
		return true;
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getRootNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}



bool CarbonXMLParser::createNodeWithNameAndValue(const OSString & nodeName, const OSString & nodeValue, XMLNode& outValue)
{
	if(isInitialized())
	{
		try
		{
			MSXML2::IXMLDOMNodePtr newNode = NULL;
			CComVariant vType(NODE_ELEMENT);
			newNode = docRef->createNode(vType,nodeName.c_str(),L"");
			newNode->nodeTypedValue = nodeValue.c_str();
			outValue = newNode;
			return true;
		}
		catch (_com_error & e)
		{
			return false;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getRootNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}

bool CarbonXMLParser::addChildToNode(XMLNode newNode, XMLNode node)
{
	if (!node || !newNode)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... newNode or node is null!"));
		return false;
	}

	if(isInitialized())
	{
		try
		{
			node->appendChild(newNode);
			return true;
		}
		catch (_com_error & e)
		{
			return false;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}

bool CarbonXMLParser::addChildToNode(const OSString & nodeName, const OSString & nodeValue, XMLNode node, XMLNode& outChildNode)
{
	if(node == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... node is null!"));
		return false;
	}
	
	if(isInitialized())
	{
		try
		{
			MSXML2::IXMLDOMNodePtr newNode = NULL;
			CComVariant vType(NODE_ELEMENT);
			newNode = docRef->createNode(vType,nodeName.c_str(),L"");
			newNode->nodeTypedValue = nodeValue.c_str();
			node->appendChild(newNode);
			outChildNode = newNode;
			return true;
		}
		catch (_com_error & e)
		{
			return false;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}


}

bool CarbonXMLParser::deleteNode(XMLNode node)
{
	if(node == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("Error in deleteNode node is null!"));
		return false;
	}

	if(isInitialized())
	{
		try
		{
			MSXML2::IXMLDOMNodePtr pParent = node->parentNode;

			if(pParent != NULL)
			{
				pParent->removeChild(node);
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (_com_error & e)
		{
			return false;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}


bool CarbonXMLParser::addChildToNode(const OSString & nodeName, const OSString & nodeValue, XMLNode node)
{
	if(node == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... node is null!"));
		return false;
	}
	
	if(isInitialized())
	{
		try
		{
			MSXML2::IXMLDOMNodePtr newNode = NULL;
			CComVariant vType(NODE_ELEMENT);
			newNode = docRef->createNode(vType,nodeName.c_str(),L"");
			//newNode = docRef->createElement(nodeName.c_str());
			newNode->nodeTypedValue = nodeValue.c_str();
			node->appendChild(newNode);
			return true;
		}
		catch (_com_error & e)
		{
			//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("Exception :%s"),e.Description());
			return false;
		}
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}

bool CarbonXMLParser::getXMLStringOfNode(const XMLNode aNode, string & outvalue)
{
	if(aNode == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeXMLString ... node is null!"));
		return false;
	}

	OSString temp;
	if(getXMLStringOfNode(aNode, temp))
	{
		if(carbonUtilities::cuConvertOSStringToString(temp.c_str(), outvalue))
		{
			return true;
		}
	}
	
	return false;}

bool CarbonXMLParser::getXMLStringOfNode(const XMLNode aNode, OSString & outvalue)
{
	if(aNode == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeXMLString ... node is null!"));
		return false;
	}

	if(isInitialized())
	{
		BSTR bptr;
		HRESULT hr = aNode->get_xml(&bptr);
		if(!FAILED(hr))
		{
			outvalue = (LPCTSTR)bptr;
			SysFreeString(bptr);
			return true;
		}
		else
			return false;
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}

	return true;
}

bool CarbonXMLParser::getNodeXMLString(XMLNode aNode, string & outvalue)
{
	if(aNode == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeXMLString ... node is null!"));
		return false;
	}

	OSString temp;

	if(getNodeXMLString(aNode, temp))
	{
		if(carbonUtilities::cuConvertOSStringToString(temp.c_str(), outvalue))
		{
			return true;
		}
	}
	
	return false;
}

bool CarbonXMLParser::getNodeXMLString(XMLNode aNode, OSString & outvalue)
{
	if(aNode == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("getNodeXMLString ... node is null!"));
		return false;
	}

	if(isInitialized())
	{
		if(!aNode->hasChildNodes())
			return false;
		MSXML2::IXMLDOMNodePtr firstChild;
		aNode->get_firstChild(&firstChild);

		BSTR bptr;
		HRESULT hr = firstChild->get_xml(&bptr);
		if(!FAILED(hr))
		{
			outvalue = (LPCTSTR)bptr;
			SysFreeString(bptr);
			return true;
		}
		else
			return false;
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}


bool CarbonXMLParser::getXMLString(OSString & outvalue)
{
	if(isInitialized())
	{
		BSTR bptr;
		HRESULT hr = docRef->get_xml(&bptr);
		if(!FAILED(hr))
		{
			outvalue = (LPCTSTR)bptr;
			SysFreeString(bptr);
			return true;
		}
		else
			return false;
	}
	else
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... CarbonXMLParser not Initialized ..."));
		return false;
	}
}


bool CarbonXMLParser::getXMLString(string &outVal)
{
	OSString tempStr;
	getXMLString(tempStr);

	carbonUtilities::cuConvertOSStringToString(tempStr, outVal);
	return true;

}


bool CarbonXMLParser::isInvalidAccess()
{
	if( !isInitialized() )
	{
		return true;
	}
	return false;
}


bool CarbonXMLParser::initWithXMLString( const string & xmlString)
{
	OSString wideXmlString;
	string tempStr=xmlString;
	if(!carbonUtilities::cuConvertStringToOSString(tempStr,wideXmlString))
	{
		return false;
	}

	return initWithXMLString(wideXmlString);
}
bool CarbonXMLParser::stringValueForXQuery( const OSString & xQuery, string & outValue)
{
	OSString wideOutValue;
	if(!stringValueForXQuery(xQuery,wideOutValue))
	{
		return false;
	}
	if(!carbonUtilities::cuConvertOSStringToString(wideOutValue.c_str(),outValue))
	{
		return false;
	}

	return true;
}
#if 0
bool createNodeWithNameAndValue(const OSString & nodeName, const string & nodeValue, XMLNode& outValue)
{
	OSString wideNodeValue;
	if(!carbonUtilities::cuConvertStringToOSString(nodeValue,wideNodeValue))
	{
		return false;
	}

	return createNodeWithNameAndValue(nodeName,wideNodeValue,outValue);
}
#endif
bool CarbonXMLParser::createNodeWithNameAndValue(const string & nodeName, const string & nodeValue, XMLNode& outValue)
{
	OSString wideNodeName;
	OSString wideNodeValue;
	if(!carbonUtilities::cuConvertStringToOSString(nodeName,wideNodeName))
	{
		return false;
	}

	if(!carbonUtilities::cuConvertStringToOSString(nodeValue,wideNodeValue))
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("Failed in string to OSString conversion"));
		return false;
	}

	return createNodeWithNameAndValue(wideNodeName,wideNodeValue,outValue);
}
bool CarbonXMLParser::addChildToNode(const string & nodeName, const string & nodeValue, XMLNode node)
{
	if(node == NULL)
	{
		//Logger::getInstance()->logDebug(kSTR_XMLPARSER, UniConst("addChildToNode ... node is null!"));
		return false;
	}

	OSString wideNodeName;
	OSString wideNodeValue;
	if(!carbonUtilities::cuConvertStringToOSString(nodeName,wideNodeName))
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("Failed in string to OSString conversion"));
		return false;
	}

	if(!carbonUtilities::cuConvertStringToOSString(nodeValue,wideNodeValue))
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("Failed in string to OSString conversion"));
		return false;
	}
	return addChildToNode(wideNodeName,wideNodeValue,node);
}

bool CarbonXMLParser::writeXMLToFilePathInUTF8( const OSString &xmlFilePath )
{
	HRESULT hr = docRef->save(xmlFilePath.c_str());
	if(SUCCEEDED(hr))
	{
		// do nothing
	}
	else
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("Failed to write XML to file"));
		return false;
	}
	return true;
}

bool CarbonXMLParser::stringValueListForXQuery( const OSString & xQuery, aVectorOsStr &outValueList, int *valCount )
{
	MSXML2::IXMLDOMNodeListPtr listPtr=NULL;
    MSXML2::IXMLDOMNodePtr nodePtr=NULL;
	if(!isInitialized())
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("xmldoc not initialized"));
		return false;
	}
	int outCount=0;
	listPtr=getNodeListForXQuery(xQuery,&outCount);
	int size=outCount;
	
	int aVal = 0;
	for(int iter=0;iter<size;iter++)
	{
		TCHAR temp[128];
		_stprintf(temp,L"iter =>%d",iter);
		if(listPtr)
		{
			listPtr->get_item( iter , &nodePtr );   
			_bstr_t value;
			value = nodePtr->Gettext();
			outValueList.push_back((LPWSTR)value);
			aVal++;
		}
	}
	
	if(valCount !=NULL)
	{
		*valCount=aVal;
	}

	if(size<=0)
		//Logger::getInstance()->logTrace(kSTR_XMLPARSER, UniConst("stringValueListForXQuery does not exists"));

	return true;
}
bool CarbonXMLParser::addAttributeToNode(XMLNode node, const OSString &attributeName,const OSString &attributeVal)
{
	if (!isInitialized())
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("xmldoc not initialized"));
		return false;
	}

	try
	{
		HRESULT  hr = node->setAttribute(attributeName.c_str(),_variant_t(attributeVal.c_str()));
		if(SUCCEEDED(hr))
		{
			return true;
		}
		else
		{
			//Logger::getInstance()->logError(kSTR_XMLPARSER, UniConst("Failed to add Attribute to XML error :%d"),GetLastError());
			return false;
		}
	}
	catch (_com_error & e)
	{
		//Logger::getInstance()->logError(kSTR_XMLPARSER,UniConst("Error Occured%s\n"),e.Description());
		return false;
	}

	return true;
}

