
#pragma once

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <vector>
#include <map>

#include "Constants.h"
#include "Utilities.h"

using namespace std;

#ifdef MAC

typedef std::string OSString;
typedef char NativeChar;

typedef void * XMLDoc;
typedef void * XMLNode;
typedef void * XMLNodeList;


#else

#include <msxml2.h>
#import "msxml3.dll"
#include "tchar.h"
#include "atlcomcli.h"

typedef TCHAR NativeChar;

typedef MSXML2::IXMLDOMDocument2Ptr XMLDoc;
typedef MSXML2::IXMLDOMElementPtr XMLNode;
typedef MSXML2::IXMLDOMNodeListPtr XMLNodeList;

#endif

typedef pair<OSString, OSString> StrPair;
typedef map<OSString, OSString> StrMap;

class CarbonXMLParser
	{
	public:
		CarbonXMLParser(void);
		~CarbonXMLParser(void);
		
		// Reading
		bool initWithXMLPath(const OSString &xmlFilePath);
		bool initWithXMLString( const OSString & xmlString);
		bool writeXMLToFilePathInUTF8( const OSString &xmlFilePath );

		bool isInitialized();
		bool nodeExistsXQuery( const OSString & xQuery);
		bool stringValueForXQuery( const OSString & xQuery, OSString & outValue);
		bool stringValueListForXQuery( const OSString & xQuery, aVectorOsStr &outValueList, int *valCount );

		XMLNode getNodeForXQuery( const OSString & xQuery );
		XMLNodeList getNodeListForXQuery(const OSString & xQuery, int* outCount);
		XMLNode getNodeAtIndexFromNodeList( XMLNodeList aList, int index );
		
		StrMap getMapFromNode( XMLNode aNode );
		StrMap getMapOfNode( XMLNode aNode );
		StrMap getAttrsMapOfNode( XMLNode aNode );
		// Writing
		bool getRootNode(XMLNode & outValue);
		bool createNodeWithNameAndValue(const OSString & nodeName, const OSString & nodeValue, XMLNode& outValue);
		bool addChildToNode(const OSString & nodeName, const OSString & nodeValue, XMLNode parentNode, XMLNode& outChildNode);
		bool addChildToNode(const OSString & nodeName, const OSString & nodeValue, XMLNode parentNode);
		bool addChildToNode(XMLNode newNode, XMLNode parentNode);
		bool addAttributeToNode(XMLNode node,const OSString& attributeName,const OSString& attributeVal);
		bool deleteNode( XMLNode node );
		bool getNodeXMLString(const XMLNode aNode, OSString & outvalue);
		bool getXMLString(OSString & outValue);
		bool getNodeFirstChildString( XMLNode node, OSString & outValue);
		bool getXMLStringOfNode(const XMLNode aNode, OSString & outvalue);

#ifdef WIN32
		bool initWithXMLPath(const std::string &xmlFilePath);
		bool initWithXMLString( const string & xmlString);
		bool stringValueForXQuery( const OSString & xQuery, string & outValue);
		bool createNodeWithNameAndValue(const string & nodeName, const string & nodeValue, XMLNode& outValue);
		bool addChildToNode(const string & nodeName, const string & nodeValue, XMLNode parentNode, XMLNode& outChildNode);
		bool addChildToNode(const string & nodeName, const string & nodeValue, XMLNode node);
		bool getNodeXMLString(const XMLNode aNode, string & outvalue);
		bool getXMLStringOfNode(const XMLNode aNode, string & outvalue);
		bool getXMLString(string &outVal);
#endif

	private:

#ifdef MAC
		void *docRef;
#else
		MSXML2::IXMLDOMDocument2Ptr     docRef;
#endif
		bool isInvalidAccess();

};

#endif
