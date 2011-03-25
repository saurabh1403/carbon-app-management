
#pragma once

#include "Constants.h"
#include "EDMM.h"


//a struct for getting the package info in this form.
struct packageInfo
{
	string name;
	string pkgId;
	string titleText;
	string imageIconPath;
};




//provides all the functionality for accessing the information from the db about the product.
//this db is the one which contains infomration about the whole product
class PDbWrapper
{

private:

	DMM _dbObj;
	bool isInitialized;

public:

	PDbWrapper():isInitialized(false)
	{

	}

	~PDbWrapper()
	{
		isInitialized = false;

	}

	bool initProductDb();

	//gets Maps having key as pkg Code and value as the package Branding File
	bool getAvailablePackages(vector<packageInfo> &outMap);

	bool getInstalledPackages(vector<packageInfo> &outMap);

	bool isPackageInstalled(const string &pkgId);

	//removes the package content which are present only in the common files location. This API should be called in elevated mode only.
	bool cleanPackageInformationAndAssets(const string &pkgId);

	//outPath should be unicode since it is a path 
	bool getPackageDirectory(const string &pkgId, OSString &outPath );

};