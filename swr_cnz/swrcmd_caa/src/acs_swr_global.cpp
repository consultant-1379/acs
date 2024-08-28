//******************************************************************************
//
//  NAME
//  acs_swr_global.cpp
//
//  COPYRIGHT
//   Ericsson AB, Sweden 2014. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//
//  AUTHOR
//     2014-05-16 by XSAMECH
//
// RELEASE REVISION HISTORY
//      REV NO          DATE          NAME      DESCRIPTION
//      A               2014-05-16    XSAMECH   Product release
//      3.6 R1A05       2018-09-24    ZGXXNAV   adapated swm2.0 changes
//******************************************************************************


#include "acs_swr_global.h"
#include "fstream"

extern string g_errorText;
extern OmHandler g_theOmHandler;
extern bool is_swm_2_0;
const char *passiveblocks="/cluster/storage/system/config/apos/passive_blocks";
/*
 * getAposDetails
 * This method fetches information related to APOS
 */
bool getAposDetails(string &aposPackage, string &aposSecurity)
{
	bool status=true;
	string aposString;
	ifstream aposConFile;
	int line_num=0;
	aposConFile.open (swrDisk::aposConfFile);
	if (aposConFile.is_open())
	{
		while(!aposConFile.eof())
		{
			++line_num;
			getline(aposConFile,aposString);
			if (1==line_num)
			{
				int separator=aposString.find(":");
				separator++;
				int len=aposString.length();
				aposPackage=aposString.substr(separator,(len-separator));
			}
			else if (2==line_num)
			{
				int separator=aposString.find(":");
				separator++;
				int len=aposString.length();
				aposSecurity=aposString.substr(separator,(len-separator));
			}
		}
		aposConFile.close();
	}
	else
	{
		g_errorText=swrErrorStrings::aposFileOpenFail;
		status=false ;
	}
	return status;
}
/*
 *initBlockCxcName
 *This method creates a Map of CXC numbers and CXC names
 */
void initBlockCxcName(map<string, string>& cxcBlockNameMap)
{
	cxcBlockNameMap["LCTBIN"]="LCTBIN, Misc functions used in the AP";
	cxcBlockNameMap["AEHBIN"]="Event Handling, Executables";
	cxcBlockNameMap["APCONFBIN"]="AP Configuration Query API";
	cxcBlockNameMap["APGCCBIN"]="APGCC,Shared Library";
	cxcBlockNameMap["TRABIN"]="TRA , Executables" ;
	cxcBlockNameMap["MKTRBIN"]="MKTR, Executables";
	cxcBlockNameMap["GCCBIN"]="GCC GOH Common Classes, Shared Library";
	cxcBlockNameMap["APJTPBIN"]="AP Job Transfer Protocol, Executables";
	cxcBlockNameMap["PRCBIN"]="Process Control";
	cxcBlockNameMap["HWMBIN"]="AP HW Manager, Executables";
	cxcBlockNameMap["OHIBIN"]="OHI,Shared Library";
	cxcBlockNameMap["DSDBIN"]="Directory service, Executables";
	cxcBlockNameMap["CSBIN"]="CS, Executables";
	cxcBlockNameMap["SSUBIN"]="System Supervisor, Executables";
	cxcBlockNameMap["ASECBIN"]="ASEC , Executables";
	cxcBlockNameMap["MAINBIN"]="MAIN, Executables ";
	cxcBlockNameMap["CONVBIN"]="CONV , Executables";
	cxcBlockNameMap["PROVBIN"]="PROV , Executables";
	cxcBlockNameMap["STSCOUNTERSBIN"]="Statistics and Traffic measurement Subsystem, Counter Data";
	cxcBlockNameMap["STSCOUNT01BIN"]="Statistics and Traffic measurement Subsystem, Counter Data";
	cxcBlockNameMap["STSCOUNT02BIN"]="Statistics and Traffic measurement Subsystem, Counter Data";
	cxcBlockNameMap["STSCOUNT03BIN"]="Statistics and Traffic measurement Subsystem, Counter Data";
	cxcBlockNameMap["STSCOUNT04BIN"]="Statistics and Traffic measurement Subsystem, Counter Data";
	cxcBlockNameMap["HADEVMONBIN"]="HADEVMON, Executables";
	cxcBlockNameMap["ADHBIN"]="MCS MML Server, Executables";
	cxcBlockNameMap["CPFBIN"]="CP File Handling, Executables";
	cxcBlockNameMap["BUSRVBIN"]="CP BUSRV Restore System Backup Copy, executables";
	cxcBlockNameMap["BUFTPDBIN"]="CPS Relaod via FTP, Executables ";
	cxcBlockNameMap["LMBIN"]="License Manager, Executables";
	cxcBlockNameMap["CFETBIN"]="Configuration Editing Tool, Command";
	cxcBlockNameMap["TESRVBIN"]="TESRV , Executables";
	cxcBlockNameMap["CDHBIN"]="CDH, Executables";
	cxcBlockNameMap["CPTMP2DBIN"]="Central Processor Test, Executables";
	cxcBlockNameMap["IPNAADMBIN"]="IPNAADM service, Executable";
	cxcBlockNameMap["ALDABIN"]="Alarm Display Administrator, Executables";
	cxcBlockNameMap["ALECBIN"]="External Alarm";
	cxcBlockNameMap["CLHBIN"]="Central Log Handler, Commands";
	cxcBlockNameMap["SGSBIN"]="Statistics Gatherer Service";
	cxcBlockNameMap["ALOGBIN"]="ALOG Audit Log, Executables and Shared Library";
	cxcBlockNameMap["NSFBIN"]="Network Surveillance Function, Executables";
	cxcBlockNameMap["LOGMBIN"]="LOGM Service, Executables";
	cxcBlockNameMap["MLSBIN"]="MLS, Executable";
	cxcBlockNameMap["EMFBIN"]="EMF External Media Function, Executables";
	cxcBlockNameMap["ALHBIN"]="ALH Service, Executables";
	cxcBlockNameMap["TOCAPBIN"]="Directory service, Executables";
	cxcBlockNameMap["APBMBIN"]="AP Blade Manager, Executables";
	cxcBlockNameMap["AFPBIN"]="AFP AP File Processing, Executables and Shared Library";
	cxcBlockNameMap["AIAPBIN"]="Alphanumeric Information Transfer in AP, Executable";
	cxcBlockNameMap["CCHBIN"]="CCH Service, Executable";
	cxcBlockNameMap["BUAPBIN"]="CP BUAP Restore System Backup Copy, Executables";
	cxcBlockNameMap["CPTASPABIN"]="Central Processor Test, Executables";
	cxcBlockNameMap["CPTASPCBIN"]="Central Processor Test, Executables";
	cxcBlockNameMap["CPTASPCABIN"]="Central Processor Test, Executables";
	cxcBlockNameMap["CPTASPCBBIN"]="Central Processor Test, Executables";
	cxcBlockNameMap["CPTASPMBIN"]="Central Processor Test, Executables";
	cxcBlockNameMap["CSYNCBIN"]="CSYNC, Executables";
	cxcBlockNameMap["MTZBIN"]="MTZ , Executables";
	cxcBlockNameMap["HBEATBIN"]="HBEAT , Executables";
	cxcBlockNameMap["TREFCBIN"]="TREFC Service, Executable";
	cxcBlockNameMap["TRAPDSBIN"]="TRAP Dispatcher Service, Executables and Shared Library";
	cxcBlockNameMap["BURBIN"]="Backup and Restore, Executables";
	cxcBlockNameMap["SWRBIN"]="Software Record, Executables";
	cxcBlockNameMap["NCLIBBIN"]="NETCONF Library, Shared Library";
}

/*
 * setCxcName
 * This method sets the CXC name for a give CXC number.
 */
void getCxcName(const string& blockcxcname, map<string, string>cxcBlockNameMap)
{
	map<string,string>::const_iterator cxcBlockNameiter;
	//TR HP94040
	//Checking if a block is present or not in the Rule table so that
	//it can be displayed with BLOCK NAME incase of absence.
	cxcBlockNameiter = cxcBlockNameMap.find(blockcxcname);
	if(cxcBlockNameiter != cxcBlockNameMap.end())
	{
		cout<<cxcBlockNameiter->second<<endl;
	}
	else
	{
		cout<<blockcxcname<<", Executables"<<endl;
	}
}

/*
 * createAndInitialiseOMHandler
 * This method initializes omHandler
 */
bool createAndInitialiseOMHandler()
{
	if(ACS_CC_FAILURE == g_theOmHandler.Init())
		return false;
	else
		return true;
}

/*
 * finaliseOMHandler
 * This method finalizes omhandler
 */
bool finaliseOMHandler()
{
	if(ACS_CC_FAILURE == g_theOmHandler.Finalize())
		return false;
	else
		return true;
}
/*
 * getSwInventory
 * This method gets software Inventory details.
 */
ACS_CC_ReturnType getSwInventory(vector<string>& dnSwIList)
{
	ACS_CC_ReturnType status = ACS_CC_FAILURE;
	if(!createAndInitialiseOMHandler())
	{
		g_errorText=swrErrorStrings::swINotFound;
		return status;
	}

	//CoreMW 3.3 or later
	status = g_theOmHandler.getClassInstances(swrImm::CmwSwIMInventory,dnSwIList);

	if( (ACS_CC_SUCCESS!=status) || (0==dnSwIList.size()) )
	{
		dnSwIList.clear();

		//CoreMW 3.2 or earlier
		status = g_theOmHandler.getClassInstances(swrImm::SwInventory,dnSwIList);

		if( (ACS_CC_SUCCESS!=status) || (0==dnSwIList.size()) )
		{
			g_errorText=swrErrorStrings::swINotFound;
		}
	}

	finaliseOMHandler ();
	return status;
}

/*
 * getActiveVersion
 * This method fetches the active versions
 */
ACS_CC_ReturnType getActiveVersion(vector<string> dnSwList,string &activeVersion,list<string>& activeVersions)
{
	ACS_CC_ReturnType status = ACS_CC_FAILURE;
	ACS_CC_ImmParameter paramToFind;
	if(!createAndInitialiseOMHandler())
	{
		g_errorText=swrErrorStrings::activeNotFound;
		return status;
	}
	paramToFind.attrName = const_cast<char*>(swrImm::activeAttrName);
	status = g_theOmHandler.getAttribute (dnSwList[0].c_str(), &paramToFind );
	if(ACS_CC_SUCCESS != status )
	{
		g_errorText=swrErrorStrings::activeNotFound;
	}
	else
	{
		//--TR HQ48327 fix starts here
		//Checking the number of active versions and taking up the
		//Initially created one!!

		int count=paramToFind.attrValuesNum;
		//for swrprint -l option
		while(count>0)
		{
			string version(reinterpret_cast<char*>(paramToFind.attrValues[--count]));
			if(string::npos!=version.find(swrUtil::APG43LswVersion))
			{
				activeVersions.push_front(version);
				activeVersion=version;
			}
			else
			{
				activeVersions.push_back(version);
			}
		}
		//--TR HQ48327--ends here
	}
	finaliseOMHandler ();
	return status;
}


/*
 * getSystemName
 * This method gets the system name from APG active software Version
 */

void getSystemName(string& sysName,string activeVersion)
{
	size_t posSep,posSystemName;
	string seprator="_";
        if ( is_swm_2_0 == true )
        {
           posSystemName=activeVersion.find(swrParse::atrate);
        }
	else
	{
	   posSystemName=activeVersion.find(swrParse::equal);
	}
	size_t posSystemNameEnd=activeVersion.find(swrParse::minus);
	size_t posSystemNameEnds=activeVersion.find("-",posSystemNameEnd+1,1);
	posSystemNameEnd=posSystemNameEnd-posSystemName;
	posSystemNameEnds=posSystemNameEnds-posSystemName;
	string systemName=activeVersion.substr(++posSystemName,--posSystemNameEnd);
	while((posSep = systemName.find(seprator))!=string::npos)
	{
		int len = seprator.size();

		systemName.replace(posSep,len," ");

		posSep +=seprator.size();
	}
	sysName = systemName;
}

/*
 * getInitialParams
 * This method fetches information used later on.
 */
int getInitialParams(vector<string>& dnSwInventory,string & activeVersion,string& sysName,string &aposPackage,string &aposSecurity,list<string>& activeVersions)
{
	ACS_CC_ReturnType acsRet = ACS_CC_FAILURE;
	acsRet = getSwInventory(dnSwInventory);
	if( ACS_CC_SUCCESS != acsRet  )
	{
		return 3;
	}

	acsRet = getActiveVersion(dnSwInventory,activeVersion,activeVersions);
	if( ACS_CC_SUCCESS != acsRet )
	{

		return 4;
	}

	getSystemName(sysName,activeVersion);
	if (!getAposDetails(aposPackage,aposSecurity))
	{
		return 13;

	};
	return 0;
}
/*
 * parseSwItem
 * This method parses Software Bundle name to get version , subsystem information etc.
 */
void parseSwItem(string swItem, string &prodName, string &prodVer)
{
	int positNamestart=swItem.find(swrParse::equal);
	size_t positNameEnd=swItem.find_last_of(swrParse::comma);
	positNamestart++;
	string swName=swItem.substr(positNamestart,(positNameEnd-positNamestart));

	if(string::npos != swName.find(swrUtil::secException) || 
           string::npos != swName.find(swrUtil::ldescriptException))
	{
		size_t pos1 = swName.find_last_of(swrParse::minus);
		size_t posVer = swName.find_last_of(swrParse::minus, pos1-1);
		if(string::npos != posVer)
		{
			prodName=swName.substr(0,posVer);
			prodVer=swName.substr(posVer+1);
			prodVer=prodVer.substr(0,prodVer.find("-"));
		}
	}
	else
	{
		size_t posNameEnd=swName.find_last_of(swrParse::minus);

		if(string::npos != posNameEnd)
		{
			prodName=swName.substr(0,posNameEnd);
			prodVer=swName.substr(posNameEnd+1);
		}
	}
}

/*
 * parseSwBundle
 * This method parses Software Bundle name to get version , subsystem information etc.
 */

void parseSwBundle(string swBundle, string &bundlName, string &bundlVer)
{

	int posbundleName=swBundle.find(swrImm::safSmfBundle);
	int posbundleNameEnd=swBundle.find(swrParse::comma);
	swBundle=swBundle.substr(posbundleName,posbundleNameEnd-posbundleName);
	int posBundName=swBundle.find(swrParse::equal);
	posBundName++;
	swBundle=swBundle.substr(posBundName,posbundleNameEnd-posBundName);
	if(string::npos != swBundle.find(swrUtil::secException) || 
           string::npos != swBundle.find(swrUtil::ldescriptException))
	{
		size_t pos1 = swBundle.find_last_of(swrParse::minus);
		size_t posVer = swBundle.find_last_of(swrParse::minus, pos1-1);
		if(string::npos != posVer)
		{
			bundlName=swBundle.substr(0,posVer);
			bundlVer=swBundle.substr(posVer+1);
			bundlVer=bundlVer.substr(0,bundlVer.find("-"));
		}
	}
	else
	{
		size_t posAmfBundName=swBundle.find_last_of(swrParse::minus);

		if(string::npos != posAmfBundName)
		{
			bundlName=swBundle.substr(0,posAmfBundName);
			bundlVer=swBundle.substr(posAmfBundName+1);
		}

	}
}

/*
 * getInstalledBundle
 * This method checks for the installed products and verifies them with the swInventory
 */
ACS_CC_ReturnType getInstalledBundle(map<string,string> &AmfSC1bundle,map<string,string> &AmfSC2bundle)
{
	ACS_CC_ReturnType status = ACS_CC_FAILURE;
	vector<string> dnlistSwBundle;
	vector<string>::const_iterator dnlistsw;
	string installedBundle;
	if(!createAndInitialiseOMHandler())
		{
		g_errorText=swrErrorStrings::instSoftNotFound;
		return status;
		}
	status = g_theOmHandler.getClassInstances(swrImm::nodeSwBundleClassName,dnlistSwBundle);
	if(ACS_CC_SUCCESS != status || (0==dnlistSwBundle.size()) )
	{
		g_errorText=swrErrorStrings::instSoftNotFound;
		return status;
	}
	else
	{
            if ( is_swm_2_0 == true )
	    {	
	        bool isDummy=false;
		list<string> dummyBlocks;
		std::string temp;
    	        string blkName;  
		ifstream passivefile(passiveblocks,ios::in);
	        if (passivefile.good())
		{
 		   while (passivefile >> blkName) {
		      dummyBlocks.push_back(blkName);
                   }
		}
		//Close the file stream
		passivefile.close();	
                for(dnlistsw=dnlistSwBundle.begin();dnlistsw!=dnlistSwBundle.end();++dnlistsw)
		{
	           isDummy=false;
		   for (std::list<string>::iterator it=dummyBlocks.begin(); it != dummyBlocks.end() && !isDummy; ++it)
		   {
		  	 std::size_t pos = (*dnlistsw).find((*it)); 
		 	 if (pos != string::npos)
		 	 {
		   	    isDummy=true;
			 }
		    }
		    if(!isDummy)
		    { 
		  	installedBundle.assign(*dnlistsw);
			size_t posclusterOne=installedBundle.find(swrUtil::nodeA);
			size_t posclusterTwo=installedBundle.find(swrUtil::nodeB);
			if (posclusterOne!=string::npos)
		 	{
		           string bundlName;
			   string bundlVer;
			   parseSwBundle(installedBundle, bundlName, bundlVer);
			   if( (0 != bundlName.length()) && (0 != bundlVer.length()) )
				AmfSC1bundle[bundlName]=bundlVer;
			}
			else if (posclusterTwo!=string::npos)
			{
			   string bundlName;
			   string bundlVer;
			   parseSwBundle(installedBundle, bundlName, bundlVer);
			   if( (0 != bundlName.length()) && (0 != bundlVer.length()) )
					AmfSC2bundle[bundlName]=bundlVer;
			}
		    }
		}
	    }
	    else
	    {
               for(dnlistsw=dnlistSwBundle.begin();dnlistsw!=dnlistSwBundle.end();++dnlistsw)
               {
                     installedBundle.assign(*dnlistsw);
                     size_t posclusterOne=installedBundle.find(swrUtil::nodeA);
                     size_t posclusterTwo=installedBundle.find(swrUtil::nodeB);
                     if (posclusterOne!=string::npos)
                     {
                        string bundlName;
                        string bundlVer;
                        parseSwBundle(installedBundle, bundlName, bundlVer);
                       if( (0 != bundlName.length()) && (0 != bundlVer.length()) )
                                AmfSC1bundle[bundlName]=bundlVer;
                     }
                     else if (posclusterTwo!=string::npos)
                     {
                         string bundlName;
                         string bundlVer;
                         parseSwBundle(installedBundle, bundlName, bundlVer);
                         if( (0 != bundlName.length()) && (0 != bundlVer.length()) )
                                        AmfSC2bundle[bundlName]=bundlVer;
                     }
	       }
	    }			
	}
	finaliseOMHandler ();
	return status;
}

/*
 * getProductStats
 * This method prints Information of the Software Domain.(APG/Non APG)
 */
void getProductStats(string activeVersion)
{
	string productName,productIdentity,productRevision;
	size_t posSystemName,posSystemNameEnd,posSystemNameEnds,posRevisionNameEnd,posSystemNameMid1,posSystemNameMid2,posRstateindex;
        if ( is_swm_2_0 == true )	
        {
           posSystemName=activeVersion.find(swrParse::atrate);
        }
	else
	{
	   posSystemName=activeVersion.find(swrParse::equal);
        }
	posSystemNameEnd=activeVersion.find(swrParse::minus);
	posSystemNameEnds=activeVersion.find("-",posSystemNameEnd+1,1);
	posRevisionNameEnd=activeVersion.find(swrParse::comma);

	posSystemNameMid1=posSystemNameEnd-posSystemName;
	posSystemNameMid2=posSystemNameEnds-posSystemNameEnd;
	posRevisionNameEnd=posRevisionNameEnd-posSystemNameEnds;
	productName=activeVersion.substr(++posSystemName,--posSystemNameMid1);
	productIdentity=activeVersion.substr(++posSystemNameEnd,--posSystemNameMid2);
	productRevision=activeVersion.substr(++posSystemNameEnds,--posRevisionNameEnd);



	cout.setf(ios::left, ios::adjustfield);
	cout <<setw(16)<<"PRODUCTNAME" << setw(16) << "IDENTITY" << setw(10) << "REVISION"<<endl;
        if ( is_swm_2_0 == true )
        {
           posRstateindex = productIdentity.find_last_of("R");
           if (std::string::npos != posRstateindex)
           {
             productIdentity = productIdentity.substr(posRstateindex, productIdentity.size());
           }
           cout <<setw(16)<<productName<<setw(16)<<productRevision<<setw(16)<<productIdentity<<endl;
        }
	else
        {
           cout <<setw(16)<<productName<<setw(16)<<productIdentity<<setw(16)<<productRevision<<endl;
        }
	cout <<endl;
	cout <<endl;
}


/*
 * header
 * This method prints the header part of swrprint
 */
bool header(string sysName,string aposPackage,string aposSecurity)
{
	bool retStatus=true;
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	char nodeName[32];
     	int stat = gethostname(nodeName,sizeof nodeName);

	if (!stat)
	{
	cout << "\nAP SOFTWARE CHARACTERISTICS    ";
	cout << asctime (timeinfo)<<endl ;
	cout << "SYSTEM:        "<<sysName<<endl;
	cout << "NODE:          "<<nodeName<<endl;
	cout << "APOS PACKAGE:  "<<aposPackage<<endl;
	cout << "APOS SECURITY: "<<aposSecurity<<endl;
	cout <<endl;
	cout <<endl;
	}
	else
	{
	  retStatus=false;

	}
	return retStatus;
}

/* 				getProductData
 * This method retrieves data related to all the products in the software Inventory *
 */


int getProductData(map<string,string> AmfSC1bundle, map<string,string> AmfSC2bundle, list<string>& subSys,
					string activeVersion, string SwInventoryId, swData& data)
{
	string ProductData,ProductName,ProductRevision,ProductNumber,Subsystem;
	ACS_CC_ImmParameter paramToFind;
	ACS_CC_ReturnType status;
	vector<string> swItemId;
	vector<string>::const_iterator it1;
	map<string,string>::const_iterator AmfSC1Bund;
	map<string,string>::const_iterator AmfSC2Bund;
	list<string>::iterator pName;

	if(!createAndInitialiseOMHandler())
		{
		g_errorText=swrErrorStrings::swVersionNotFound;
		return 5;
		}

	paramToFind.attrName = const_cast<char*>(swrImm::consistsOfAttrName);
		status = g_theOmHandler.getAttribute (activeVersion.c_str(), &paramToFind );

	if( ACS_CC_SUCCESS != status)
	{
		g_errorText=swrErrorStrings::swVersionNotFound;
		finaliseOMHandler();
		return 5;

	}
	else
	{
		int count=paramToFind.attrValuesNum;
		for(int active=0;active<count;++active)
		{
			swItemId.push_back(string(reinterpret_cast<char*>(paramToFind.attrValues[active])));
		}
		for(it1=swItemId.begin();it1!=swItemId.end();++it1)
		{
			string switemid(*it1);
			string prodNameNum;
			string prodVer;
			parseSwItem(switemid, prodNameNum, prodVer);
			if ( AmfSC1bundle.count(prodNameNum)> 0 && AmfSC2bundle.count(prodNameNum)> 0)
			{
				AmfSC1Bund=AmfSC1bundle.find(prodNameNum);
				string checkRevSC_1,checkRevSC_2;
				checkRevSC_1=(*AmfSC1Bund).second;
				AmfSC2Bund=AmfSC2bundle.find(prodNameNum);
				checkRevSC_2=(*AmfSC2Bund).second;
				if ( (checkRevSC_1 == checkRevSC_2) && (checkRevSC_1 != prodVer) )
				{
					switemid="swItemId="+prodNameNum+swrParse::minus+checkRevSC_1+swrParse::comma+SwInventoryId;
				}

			}
			else
			{
				continue;
			}
			paramToFind.attrName = const_cast<char*>(swrImm::administrativeDataAttrName);

			status = g_theOmHandler.getAttribute (switemid.c_str(),&paramToFind);

			if(ACS_CC_SUCCESS != status)
			{
				g_errorText=swrErrorStrings::adminDataNotFound;
				finaliseOMHandler();
				return 6;
			}
			else
			{
				ProductData=string(reinterpret_cast<char*>(paramToFind.attrValues[0]));
				paramToFind.attrName = const_cast<char*>(swrImm::prodNameAttrName);
				status = g_theOmHandler.getAttribute (ProductData.c_str(), &paramToFind );

				if(ACS_CC_SUCCESS != status)
				{
							g_errorText=swrErrorStrings::prodNameNotFound;
							finaliseOMHandler();
							return 14;
				}
				else
				{

					ProductName=string(reinterpret_cast<char*>(paramToFind.attrValues[0]));
					string fullProductName=ProductName;
					if(string::npos != ProductName.find(swrUtil::secException) || 
					   string::npos != ProductName.find(swrUtil::ldescriptException))
					{
						size_t pos1 = ProductName.find_last_of(swrParse::minus);
						size_t posPrdName = ProductName.find(swrParse::minus);
						string cbaComp;
						if(string::npos != pos1)
						{
							ProductNumber = ProductName.substr(pos1+1);
							transform(ProductNumber.begin(), ProductNumber.end(),ProductNumber.begin(), ::toupper);
							cbaComp=ProductNumber.substr(0,3);
						}

						if (swrUtil::CBANum==cbaComp)
						{
							Subsystem=swrUtil::cbaSubSys;
						}

						if((string::npos != pos1) && (string::npos != posPrdName))
						{
							ProductName = ProductName.substr(posPrdName+1, pos1-posPrdName-1);
							/* Limit lenght to 15 */
							ProductName = ProductName.substr(0,15);
						}

						data.packageToProduct[ProductName]=fullProductName;
						paramToFind.attrName = const_cast<char*>(swrImm::prodRevAttrName);
						status = g_theOmHandler.getAttribute (ProductData.c_str(), &paramToFind );
						if( ACS_CC_SUCCESS != status)
						{
							g_errorText=swrErrorStrings::prodRevNotFound;
							finaliseOMHandler();
							return 12;
						}
						ProductRevision=string(reinterpret_cast<char*>(paramToFind.attrValues[0]));
						ProductRevision=ProductRevision.substr(0,ProductRevision.find("-"));
					}
					else
					{
						int posSub=ProductName.find(swrParse::minus);
						int posName=ProductName.find(swrParse::underScore);
						string nameEnd="\n";
						int posNameEnd=ProductName.find(nameEnd);
						int len=posName-posSub;

						paramToFind.attrName = const_cast<char*>(swrImm::prodNumAttrName);
						status = g_theOmHandler.getAttribute (ProductData.c_str(), &paramToFind );
						if( ACS_CC_SUCCESS != status)
						{
							g_errorText=swrErrorStrings::prodNumNotFound;
							finaliseOMHandler();
							return 7;
						}
						ProductNumber=string(reinterpret_cast<char*>(paramToFind.attrValues[0]));

						size_t pos_seperator=ProductNumber.find(swrParse::underScore);
						string cbaComp=ProductNumber.substr(0,3);
						if (string::npos!=pos_seperator)
						{
							ProductNumber.replace(pos_seperator--,1,"/");
						}

						if (swrUtil::CBANum==cbaComp && (activeVersion.find(swrUtil::APG43LswVersion)!=string::npos ))
						{
							Subsystem=swrUtil::cbaSubSys;
							++posSub;
							ProductName=ProductName.substr(posSub,ProductName.length()-posSub);
						}
						else
						{
							Subsystem=ProductName.substr(posSub+1,--len);
							ProductName=ProductName.substr(posName+1,posNameEnd);
						}
						data.packageToProduct[ProductName]=fullProductName;
						paramToFind.attrName = const_cast<char*>(swrImm::prodRevAttrName);
						status = g_theOmHandler.getAttribute (ProductData.c_str(), &paramToFind );
						if( ACS_CC_SUCCESS != status)
						{
							g_errorText=swrErrorStrings::prodRevNotFound;
							finaliseOMHandler();
							return 12;
						}
						ProductRevision=string(reinterpret_cast<char*>(paramToFind.attrValues[0]));
					}

					data.packageName.push_back(ProductName);
					data.packageMap[toUpperString(ProductName)]=ProductName;
					subSys.push_back(Subsystem);
					data.subsysMap[toUpperString(Subsystem)]=Subsystem;
					data.nameToNumber[ProductName]=ProductNumber;
					data.nameToRevision[ProductName]=ProductRevision;
					data.subsysName[ProductName]=Subsystem;
				}
			}
		}
		subSys.sort();
		subSys.unique();
	}
	finaliseOMHandler();
	return 0;
}

/*
 * printData
 * This method prints the details of software products in general format.
 */

int printData(string var,bool category, list<string>activeVersions, map<string,string> AmfSC1bundle, map<string,string> AmfSC2bundle,
		      string sysName, string aposPackage, string aposSecurity, string SwInventoryId, swData& data, string activeVersion)
{
	bool printHeader=true;
	int productStat=0;
	unsigned int checkCat=0;
	list<string>subSys;
	map<string,string>::const_iterator name_Num;
	map<string,string>::const_iterator name_Rev;
	map<string,string>::const_iterator subsysNameIt;
	list<string>::const_iterator pName;

	//clean up the global structures
	data.packageName.clear();
	data.nameToNumber.clear();
	data.nameToRevision.clear();
	data.packageToProduct.clear();
	data.subsysName.clear();
	data.subsysMap.clear();
	data.packageMap.clear();
	subSys.clear();

    for (list<string>::const_iterator activeIt=activeVersions.begin();activeIt!=activeVersions.end();++activeIt)
    {
    	productStat=getProductData(AmfSC1bundle,AmfSC2bundle,subSys,(*activeIt),SwInventoryId,data);
    	if (productStat)
    		return productStat;
    }
	
	//Check for category
    bool catFound= false;
    if( category && data.subsysMap.count(toUpperString(var)))
    {
    	map<string,string>::iterator subsysIter;
    	subsysIter=data.subsysMap.find(toUpperString(var));
    	if (data.subsysMap.end() != subsysIter)
    	{
    		var=(*subsysIter).second;
    		catFound=true;
    	}

    }

    if (!category || catFound)
    {
    	if (printHeader)
    	{
    		if(!header(sysName,aposPackage,aposSecurity))
    		{
    			g_errorText=swrErrorStrings::nodeNameNotFound;
    			return 1;
    		}
    		getProductStats(activeVersion);
    		printHeader = false;
    		if(category)
    		{
    			cout << "Category:   " << var<< endl << endl;
    		}
    		cout << setw(16) << "PACKAGE" << setw(16) << "IDENTITY" << setw(10) << "VERSION"
    				<< setw(16) << "CNI" << "STATE" << endl;
    	}

    	data.packageName.sort();

    	string Subsystemstr;
    	for (pName=data.packageName.begin();pName!=data.packageName.end();++pName)
    	{
    		string ProductName(*pName);
    		string ProductNumber;
    		string ProductRevision;

    		if (data.nameToNumber.count(ProductName)> 0 )
    		{
    			name_Num=data.nameToNumber.find(ProductName);
    			ProductNumber=(*name_Num).second;
    			name_Rev=data.nameToRevision.find(ProductName);
    			ProductRevision=(*name_Rev).second;
    			subsysNameIt=data.subsysName.find(ProductName);
    			Subsystemstr=(*subsysNameIt).second;
        
                        if(ProductName.length()>=16)
                        {
                                ProductName =  ProductName.erase(15,ProductName.length());
                        }

    			if (Subsystemstr==var||!category )
    				cout<< setw(16) << ProductName << setw(16)<< ProductNumber << setw(10)<<ProductRevision <<setw(16) << " " <<"Active"<<endl;
    		}
    	}
    }
    else
    {
    	checkCat++;

    }

    if (checkCat != 0)
    {
    	g_errorText=swrErrorStrings::catNotFound;
    	return 9;
    }

	return 0;
}

/*
 * printPart
 * This method prints software product data in long format (-l option)
 */
int  printPart(string var, bool category, list<string> activeVersions, map<string,string> AmfSC1bundle, map<string,string> AmfSC2bundle,
		       string sysName,string aposPackage, string aposSecurity, list<string>& subSys, string SwInventoryId,
		       map<string,string> cxcBlockNameMap,swData& data, string activeVersion)
{
	bool printHeader=true;
	int productStat=0;
	unsigned int checkCat=0;
	map<string,string>::const_iterator name_Num;
	map<string,string>::const_iterator name_Rev;
	map<string,string>::const_iterator subsysNameIt;
	list<string>::const_iterator pName;

	//clean up the global structures
	data.packageName.clear();
	data.nameToNumber.clear();
	data.nameToRevision.clear();
	data.packageToProduct.clear();
	subSys.clear();

	for (list<string>::const_iterator activeIt=activeVersions.begin();activeIt!=activeVersions.end();++activeIt)
	{
		//if(category && (*activeIt).find(swrUtil::APG43LswVersion)==string::npos)
		//{continue;}
		productStat=getProductData(AmfSC1bundle,AmfSC2bundle,subSys,*activeIt,SwInventoryId,data);
		if (productStat)
			return productStat;
	}

	bool catFound= false;
	if( category && data.subsysMap.count(toUpperString(var)))
	{
		map<string,string>::iterator subsysIter;
		subsysIter=data.subsysMap.find(toUpperString(var));
		if (data.subsysMap.end() != subsysIter)
		{
			var=(*subsysIter).second;
			catFound=true;
		}

	}
	if (!category|| catFound)
	{
		if (printHeader)
		{
			if(!header(sysName,aposPackage,aposSecurity))
			{
				g_errorText=swrErrorStrings::nodeNameNotFound;
				return 1;
			}
			getProductStats(activeVersion);
			printHeader=false;
			if(category)
			{
				cout << "Category:   " << var << endl << endl;
			}
		}

		data.packageName.sort();
		string Subsystemstr;
		for (pName=data.packageName.begin();pName!=data.packageName.end();++pName)
		{
			string ProductName;
			string ProductNumber;
			string ProductRevision;
			ProductName.assign(*pName);

			if (data.nameToNumber.count(ProductName)> 0 )
			{
				name_Num=data.nameToNumber.find(ProductName);
				ProductNumber=(*name_Num).second;
				name_Rev=data.nameToRevision.find(ProductName);
				ProductRevision=(*name_Rev).second;
				subsysNameIt=data.subsysName.find(ProductName);
				Subsystemstr=(*subsysNameIt).second;

				if (Subsystemstr==var||!category )
				{
					cout<<"SUBSYSTEM:\t\t"    <<Subsystemstr<<endl;
					cout<<"MODULE NAME:\t\t"   <<ProductName<<endl;
					cout<<"CXC NAME:\t\t";getCxcName(ProductName,cxcBlockNameMap);
					cout<<"CXC NUMBER:\t\t"<<ProductNumber<<endl;
					cout<<"CXC VERSION:\t\t" <<ProductRevision<<endl;
					cout<<"SUPPLIER:\t\t" <<"ERICSSON AB"<<endl;
					cout<<endl;
					cout<<endl;
				}
			}
		}
	}
	else
	{
		checkCat++;
	}

	if (checkCat != 0)
	{
		g_errorText=swrErrorStrings::catNotFound;
		return 9;
	}

	return 0;
}

/*
 * printThirdPart
 * This method prints details of Third party products
 */
void printThirdPart(swData data)
{
	cout << "Category:   Third party products" << endl << endl;
	map<string,string>::const_iterator pack_prod;
	map<string,string>::const_iterator name_Num;
	map<string,string>::const_iterator name_Rev;
	list<string>::const_iterator pName;
	for (pName=data.packageName.begin();pName!=data.packageName.end();++pName)
	{
		string ProductName(*pName);

		string ProductRevision,ProductNumber,PackageName;
		if (data.nameToNumber.count(ProductName)> 0 )
		{
			name_Num=data.nameToNumber.find(ProductName);
			ProductNumber=(*name_Num).second;
			/* Replace char / with char * in order to match the bundle path */
			replace( ProductNumber.begin(), ProductNumber.end(),'/','_');
			name_Rev=data.nameToRevision.find(ProductName);
			ProductRevision=(*name_Rev).second;

			pack_prod=data.packageToProduct.find(ProductName);
			PackageName=(*pack_prod).second;

		string location=swrDisk::bundleLocation+PackageName+"-"+ProductNumber+"-"+ProductRevision+"/";
		string thirdPartyData;
		ifstream infoFile;
		int line_num=0;
		string fileTo3pp=location+swrParse::forwardSlash+ProductName+swrUtil::Info3pp;
		int tpp=0;
		infoFile.open (fileTo3pp.c_str());
		if (infoFile.is_open())
		{
			while(!infoFile.eof())
			{
				++line_num;
				getline(infoFile,thirdPartyData);
				if(line_num==1 && thirdPartyData==swrUtil::thirdPartyString)
				{
					tpp=1;
				}
				else if(tpp==1)
					cout<<thirdPartyData<<endl;
			}
			infoFile.close();
		}
	}
	}

}
/*
 * checkData
 * This method checks for valid arguments supplied
 */
bool CheckData(const char* Data)
{
	bool retStatus=false;
	string str(Data);
	size_t ind = 0;
	size_t invalidPos=0;
	//find char -
	ind = str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_-") ;
	invalidPos = str.find_first_of("_-");

	if(string::npos == ind &&  invalidPos!=(str.length()-1) && invalidPos!=0 && str.length()<= swrUtil::nameKeyLen)
	{
		retStatus=true;
	}
	else
	{
		g_errorText=swrErrorStrings::illegalUsage;
	}



		return retStatus;
}
/*
 * parse
 * This method parses the command line arguments
 */
bool parse(int argc,char *argv[], struct parseDetails & parData)
{
	bool parseStatus =true;
	int parIssued[2]={0};
	int argCnt = 1;
	int LastArg = argc - 1;
	parData.packageCount=0;
	while (argCnt <= (LastArg))
		{
			if (!strcmp(argv[argCnt], "-l"))
			{
				parIssued[l_longform] +=1;
				parData.parPosition[l_longform]=argCnt;
				parData.l_opt =true;
			}
			else if (!strcmp(argv[argCnt],"-c"))
			{
				parIssued[c_category] +=1;
				parData.parPosition[c_category]=argCnt;
				parData.c_opt =true;
			}
			else
			{
			   if (!CheckData(argv[argCnt]))
			   {
				   parseStatus =false;
				   g_errorText=swrErrorStrings::illegalUsage;
			   }

			   parData.packageCount++;

			}
			argCnt++;
		}
		if (parIssued[c_category]>1 || parIssued[l_longform]>1)
		{
			parseStatus =false;
			g_errorText=swrErrorStrings::illegalUsage;
		}
		else if (parIssued[c_category] == 1)
		{
			if (parData.l_opt && (parData.parPosition[c_category] == ( parData.parPosition[l_longform] -1)))
			{
				parseStatus =false;
				g_errorText=swrErrorStrings::illegalUsage;
			}
			else if (2 == argc  || parData.parPosition[c_category] == LastArg)
			{

				parseStatus =false;
				g_errorText=swrErrorStrings::illegalUsage;
			}
			else
			{
				parData.packageCount--;
			}
		}
return parseStatus;
}
/*
 * toUpperString
 * This method converts strings from lower case to upper case.
 */
string toUpperString(string str)
{
	for(unsigned int i=0; i<str.length();i++)
	{
		str[i]=toupper(str[i]);
	}
	return str;
}
