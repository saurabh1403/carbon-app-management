
#pragma once

//rename this file. it will be used in other apps also

#include "Constants.h"

#define ResultXmlNodeName			"result"
#define ResultXmlNode				"<result/>"

#define OutputXmlNode				"<outputXml></outputXml>"
#define CallIdentifierNodeName		"CallIdentifier"
#define SessionIdNodeName			"SessionId"

#define XmlInitErrorNode			"<error>Failed to init Xml</error>"
#define XmlProcessingErrorNode		"<error>Failed to process error</error>"

#define PackageIdNodeXquery			OSConst("/argumentXml/packageId")		//this is w.r.t. argumentXml node


//*****package db related constants************

#define kInstalledState				"2"
#define kUninstalledState			"0"
#define kExpiredState				"3"
#define kNotActivatedState			"2"


//************pdb related constants*************


#define kPackageDirConst			"INSTALLDIR"
#define kPackageIconConst			"PackageIcon"












