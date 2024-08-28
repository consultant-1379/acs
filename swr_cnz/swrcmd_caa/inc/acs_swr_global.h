/*=================================================================== */
/**
   @file  acs_swr_global.h

   @brief Header file for swr module.

          This module contains all the declarations useful for swrprint.


   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       16/05/2014   XSAMECH   Initial Release
==================================================================== */
/*==================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_SWR_GLOBAL_H_
#define _ACS_SWR_GLOBAL_H_

#include<string>
#include <stdio.h>
#include <ctype.h>
#include <sys/unistd.h>
#include <string>
#include <time.h>
#include <iostream>
#include <ace/ACE.h>
#include <stdlib.h>
#include <ace/Get_Opt.h>
#include <ACS_CC_Types.h>
#include <acs_apgcc_omhandler.h>
#include <iomanip>
#include <map>
#include <list>
#include <fstream>
using namespace std;

/*
 * swrParse- contains characters used for parsing strings.
 */
namespace swrParse{
	const char minus = '-';
	const char plus = '+';
	const char comma = ',';
	const char equal = '=';
	const char underScore = '_';
	const char dot = '.';
	const char colon = ':';
	const char forwardSlash ='/';
	const char space=' ';
	const char atrate='@';
}
/*
 * swrImm: contains strings referring classes,objects and attribute names in IMM.
 */

namespace swrImm{

	const char* const SwInventory = "SwInventory";
	const char* const CmwSwIMInventory = "CmwSwIMSwInventory";
	const char* const activeAttrName="active";
	const char* const consistsOfAttrName="consistsOf";
	const char* const administrativeDataAttrName="administrativeData";
	const char* const prodNumAttrName="productNumber";
	const char* const prodNameAttrName="productName";
	const char* const prodRevAttrName="productRevision";
	const char* const nodeSwBundleClassName ="SaAmfNodeSwBundle";
	const char* const safSmfBundle="safSmfBundle";

}
/*
 * swrDisk: contains strings referring to physical location of files.
 */
namespace swrDisk{
	const char * const aposConfFile="/opt/ap/apos/conf/apos_ver.conf";
	const char * const bundleLocation="/storage/system/software/coremw/repository/";
}

/*
 * swrUtil: contains strings commonly used by swrprint.
 */
namespace swrUtil
{
	const unsigned int nameKeyLen=255;
	const char* const nodeA= "SC-1";
	const char* const nodeB= "SC-2";
	const char* const cbaSubSys="CBA-Component";
	const char* const ERIC="ERIC";
	const char* const CBANum="CXP";
	const char* const secException ="ERIC-sec-";
	const char* const ldescriptException ="ERIC-lde-";
	const char* const APG43LswVersion="APG43L";
	const char* const thirdPartyString="Third party products";
	const char* const Info3pp="_info.txt";

}

/*
 * swrErrorStrings: contains strings related to error messages.
 */
namespace swrErrorStrings{

	const string nodeNameNotFound("Node Name Not Found");
	const string swINotFound("Could Not Find Swinventory");
	const string activeNotFound("Could Not Find Active Version");
	const string aposFileOpenFail("Error Opening Apos version conf file");
	const string instSoftNotFound("Could not find installed software bundle");
	const string swVersionNotFound("Could Not find Software Version");
	const string adminDataNotFound("Could Not find  Administrative Data");
	const string prodNameNotFound("Could Not find PRODUCT NAME");
	const string prodRevNotFound("Could Not find PRODUCT REVISION");
	const string prodNumNotFound("Could Not find PRODUCT NUMBER");
	const string catNotFound("Category not found/valid.");
	const string packageNotFound("The package(s) could not be found.");
	const string illegalIn3pp("No packages can be specified when using category '3pp'.");
	const string illegalUsage("Incorrect Usage\nUsage: swrprint [ -l ][ -c category ][ package ... ]\n");

}
/*
 *  parameterID : contains enumeration of possible options for swrprint
 */
enum parameterID
{
 c_category,
 l_longform
};

/*
 * parseDetails: contains information related parsing , to be used in main logic.
 */
struct parseDetails
{
	int parPosition[2];
	int packageCount;
	bool l_opt;
	bool c_opt;

};

/*
 * swData: contains data used by printing functions.
 */
struct swData{
	list<string>packageName;
	map<string,string>nameToNumber;
	map<string,string>nameToRevision;
	map<string,string>packageToProduct;
	map<string,string>subsysName;
	map<string,string>subsysMap;
	map<string,string>packageMap;

};
/**
   @brief        getAposDetails
   @param        aposPackage
   @param        aposSecurity
   @return       bool
*/
bool getAposDetails(string &aposPackage, string &aposSecurity);

/**
   @brief        initBlockCxcName
   @return       void
*/
void initBlockCxcName(map<string, string>& cxcBlockNameMap);
/**
   @brief        getCxcName
   @param        blockcxcname
   @return       void
*/
void getCxcName(const string& blockcxcname, map<string, string>cxcBlockNameMap);

/**
   @brief        getSwInevntory
   @param        dnSwI
   @return       ACS_CC_ReturnType
*/
ACS_CC_ReturnType getSwInventory(vector<string>& dnSwIList);
/**
   @brief        getSwInevntory
   @param        dnSw
   @param        swInvId
   @param 		 activeVersion
   @param		 activeVersions
   @return       ACS_CC_ReturnType
*/
ACS_CC_ReturnType getActiveVersion(vector<string> dnSw, string &activeVersion,
									list<string>& activeVersions);

/**
   @brief        getSystemName
   @param        sysName
   @param        activeVersion
   @return       void
*/
void getSystemName(string& sysName, string activeVersion);
/**
   @brief        getInitialParams
   @param        dnSwInventory
   @param        dnSwI
   @param 		 activeVersion
   @param		 sysName
   @param		 aposPackage
   @param		 aposSecurity
   @param		 activeVersions
   @return       int
*/
int getInitialParams(vector<string>& dnSwInventory, string & activeVersion, string& sysName,
				     string &aposPackage, string &aposSecurity, list<string>& activeVersions);
/**
   @brief        parseSwItem
   @param        swItem
   @param        prodName
   @param 		 prodVer
   @return       void
*/
void parseSwItem(string swItem, string &prodName, string &prodVer);
/**
   @brief        parseSwBundle
   @param        swBundle
   @param        bundlName
   @param 		 bundlVer
   @return       void
*/
void parseSwBundle(string swBundle, string &bundlName, string &bundlVer);
/**
   @brief        getInstalledBundle
   @param        AmfSC1bundle
   @param        AmfSC2bundle
   @return       ACS_CC_ReturnType
*/
ACS_CC_ReturnType getInstalledBundle(map<string, string> &AmfSC1bundle, map<string,string> &AmfSC2bundle);
/**
   @brief        getProductStats
   @param        activeVersion
   @param        lopt
   @return       void
*/
void getProductStats(string activeVersion);
/**
   @brief        header
   @param        sysName
   @param        aposPackage
   @param        aposSecurity
   @return       bool
*/
bool header(string sysName,string aposPackage,string aposSecurity);
/**
   @brief        getProductData
   @param        AmfSC1bundle
   @param        AmfSC2bundle
   @param 		 subSys
   @param		 activeVersion
   @param		 SwInventoryId
   @return       int
*/
int getProductData(map<string, string> AmfSC1bundle, map<string,string> AmfSC2bundle, list<string>& subSys,
				   string activeVersion, string SwInventoryId, swData& data );
/**
   @brief        printData
   @param        var
   @param        cat
   @param 		 activeVersion
   @param		 AmfSC1bundle
   @param		 AmfSC2bundle
   @param  	     sysName
   @param 		 aposPackage
   @param 		 aposSecurity
   @param 		 SwInventoryId
   @param		 data
   @param        activeVersions
   @return       int
*/
int printData(string var, bool category, list<string> activeVersions, map<string,string> AmfSC1bundle, map<string,string> AmfSC2bundle,
		      string sysName, string aposPackage, string aposSecurity, string SwInventoryId, swData& data, string activeVersion );
/**
   @brief        printData
   @param        var
   @param  		 category
   @param        cat
   @param 		 activeVersion
   @param		 AmfSC1bundle
   @param		 AmfSC2bundle
   @param  	     sysName
   @param 		 aposPackage
   @param 		 aposSecurity
   @param 		 SwInventoryId
   @return       int
*/
int  printPart(string var, bool category, list<string> activeVersions, map<string,string> AmfSC1bundle, map<string,string> AmfSC2bundle,
		string sysName, string aposPackage, string aposSecurity, list<string>& subSys, string SwInventoryId, map<string,string> cxcBlockNameMap, swData& data, string activeVersion);
/**
@brief        printThirdPart
@return       void
 *
 */
void printThirdPart(swData data);

/*
 @brief 	  checkData
 @param       Data
 @return	  bool
 */

bool CheckData(const char* Data);

/*
 @brief 	parse
 @param     argc
 @param     argv
 @param     packageCount
 @return    bool
 */
bool parse(int argc,char *argv[], struct parseDetails & parData);

/*
 @brief		toUpperString
 @param		str
 @return 	string
 */
string toUpperString(string str);

#endif // _ACS_SWR_GLOBAL_H_
