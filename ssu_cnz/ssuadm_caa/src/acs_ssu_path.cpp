
//******************************************************************************
//
//  NAME
//     ACS_SSU_Path.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2006. All rights reserved.
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
//	    190 89-CAA 109 0529
//
//  AUTHOR 
//     2005-02-28 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <iostream>
#include <list>
#include <fstream>
#include "acs_ssu_path.h"
#include "acs_ssu_common.h"
//FileM changes
#include "acs_ssu_types.h"

using namespace std;

//!@todo This structure is created temporarily as LCTBIN is not available

#if 0
//List of supervised fileM path
#define cpPrintout "/data/opt/ap/internal_root/cp/printouts"
#define cpFiles  "/data/opt/ap/internal_root/cp/files"
#define mmlCommandsFile "/data/opt/ap/internal_root/cp/mml"
#define auditLog "/data/opt/ap/internal_root/audit_logs"
#define scrFiles "/data/opt/ap/internal_root/sts_scr"
#define dataTransferForResponding "/data/opt/ap/internal_root/data_transfer"
#define backupRestore "/data/opt/ap/internal_root/backup_restore"
#define swPackage "/data/opt/ap/internal_root/sw_package"
#define healthCheck "/data/opt/ap/internal_root/health_check"
#define licenseFile "/data/opt/ap/internal_root/license_file"
#define tools "/data/opt/ap/internal_root/tools"
#define supportData "/data/opt/ap/internal_root/support_data"
#endif

bool ACS_SSU_Path::createFileMMap()
{
	ACS_CC_ImmParameter paramToFind;
	ACS_CC_ReturnType enResult;
	std::string apzDN(SSU_FILEM_DN);

	OmHandler* omHandlerPtr = new OmHandler();
	if( omHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		DEBUG("%s","Error occurred while initializing APGCC OmHandler.");
		DEBUG("%s","Exiting ACS_SSU_Path::createFileMMap");
		return false;
	}

	map<string,string> pathMap;
	ACE_TCHAR szTemp[512];
	ACE_OS::strncpy(szTemp, ACE_TEXT(SSU_FILEM_QUOTAS_FILE), 511);
	std::string pszReadLine ="\0";
	std::list<string> symNameList;
	std::pair<map<string,string>::iterator,bool> status;

	std::ifstream *m_hQuotaFile = new ifstream(szTemp,ios::in);
	if ( m_hQuotaFile == 0 )
	{
		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Failed to open the FileM Folder Quota file \"%s\". \n%m"),
				szTemp
		);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		ERROR("%s",szData);
		delete m_hQuotaFile;
		return false;
	}

	while((!(std::getline(*m_hQuotaFile, pszReadLine)<=0)))
	{
		ACE_TCHAR* pszToken;
		ACE_TCHAR pszCharBuff[200] = {0};

		//size_t pos1=pszReadLine.find(',');
		//std::string myStrIn=pszReadLine.substr(0,pos1);

		ACE_OS::strcpy( pszCharBuff , pszReadLine.c_str());
		pszToken = ACE_OS::strtok( pszCharBuff, ", ");
		
		//DEBUG("myStrIn = %s", myStrIn.c_str());

		if ((pszReadLine.size()!=0))
		{
			//size_t pos2=pszReadLine.find(',');
			//std::string myStrIn=pszReadLine.substr(pos1+1,pos2);
			//DEBUG("myStrIn = %s", myStrIn.c_str());

			pszToken = ACE_OS::strtok(0,";, ");

			//symNameList.push_back(myStrIn);
			symNameList.push_back(pszToken);
			symNameList.sort();
			symNameList.unique();
		}
	}
	delete m_hQuotaFile;
	std::list<string>::iterator it=symNameList.begin();
	for(;it!=symNameList.end(); ++it)
	{
		paramToFind.attrName = new char [strlen((*it).c_str())+1];
		ACE_OS::strcpy( paramToFind.attrName, (*it).c_str());
		std::string fileMPath("");
		if(!strcmp(paramToFind.attrName,"dataTransferForResponding"))
		{
			fileMPath.append(NBI_ROOT_PATH);
			fileMPath.append("/data_transfer");
		}
		else
		{
			if( ( enResult = omHandlerPtr->getAttribute( apzDN.c_str(), &paramToFind ) ) != ACS_CC_SUCCESS )
			{
				ERROR("%s Result status is  %d", " Error occured while getting FileMPath from IMM",enResult);
				delete []paramToFind.attrName;
				return false;
			}
			
			std::string attrValue(reinterpret_cast<char*>(*(paramToFind.attrValues)));

			fileMPath.append(NBI_ROOT_PATH);
			fileMPath.append(attrValue);
		}

		delete []paramToFind.attrName;
		status=pathMap.insert(pair<string,string>(*it,fileMPath));

		if(status.second==false)
		{
			INFO("%s","Data path already present for this symbolic name");
		}
		else
		{
			DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",fileMPath.c_str());
		}
	}

	if(omHandlerPtr->Finalize() == ACS_CC_FAILURE )
	{
		ERROR("%s", "Error occurred while finalizing Omhandler");
	}

	delete omHandlerPtr;
	omHandlerPtr = NULL;

	m_mapInternalFileMPaths = pathMap;
	return true;

#if 0
	//Implementation using hardcoded filem paths
	map<string,string> pathMap;
	ACE_TCHAR szTemp[512];
	ACE_OS::strncpy(szTemp, ACE_TEXT(SSU_FILEM_QUOTAS_FILE), 511);
	std::string pszReadLine ="\0";
	std::list<string> symNameList;
	std::pair<map<string,string>::iterator,bool> status;

	std::ifstream *m_hQuotaFile = new ifstream(szTemp,ios::in);
	if ( m_hQuotaFile == 0 )
	{
		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Failed to open the Folder Quota file \"%s\". \n%m"),
				szTemp
		);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		ERROR("%s",szData);
		return false;
	}
	while((!(std::getline(*m_hQuotaFile, pszReadLine)<=0)))
	{
		if ((pszReadLine.size()!=0))
		{
			size_t pos=pszReadLine.find(',');
			//int pos2=pszReadLine.find(',',pos1+1);
			std::string myStrIn=pszReadLine.substr(0,pos);
			symNameList.push_back(myStrIn);
			//symNameList.sort();
			symNameList.unique();
		}
	}
	//ACS_APGCC_CommonLib objCommonLib;
	//ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;
	//int folderPathLen = 1024;
	//char folderPath[1024] = { 0 };
	std::list<string>::iterator it=symNameList.begin();

	//for(;it!=symNameList.end(); ++it)
	//{
	//	DEBUG("%s","************************************");
	//std::string myStrOut = "";// folderPath;
	//char folderPath[1024] = {0};
	//int folderPathLen = 1024;
	//char* folderPath;
	//	DEBUG("symNameList = <--%s-->",(*it).c_str());
	//folderPath = "";
	//returnType = objCommonLib.GetFileMPath( (*it).c_str(),
	//		        /*folderPath*/const_cast<char *>(myStrOut.c_str()),
	//                folderPathLen );

	/*ACS_APGCC_CommonLib objCommonLib;
       	ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;
       	std::string str = (*it);

    	returnType = objCommonLib.GetFileMPath( const_cast<char*>(str.c_str()),
                      folderPath,
                      folderPathLen);

		if( returnType != ACS_APGCC_DNFPATH_SUCCESS )
    	{
    		DEBUG("%s", " Error occured while getting the FileMPath.");
        	return false;
    	}
	 */
	//DEBUG("Key = %s",(*it).c_str());
	//DEBUG("Value = %s",cpPrintout);

	//DEBUG("Value = %s",myStrOut.c_str());
	status=pathMap.insert(pair<string,string>("cpPrintout",cpPrintout));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",cpPrintout);
	}

	status=pathMap.insert(pair<string,string>("cpFiles",cpFiles));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",cpFiles);
	}

	status=pathMap.insert(pair<string,string>("mmlCommandsFile",mmlCommandsFile));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",mmlCommandsFile);
	}
	status=pathMap.insert(pair<string,string>("auditLog",auditLog));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",auditLog);
	}
	status=pathMap.insert(pair<string,string>("scrFiles",scrFiles));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",scrFiles);
	}
	status=pathMap.insert(pair<string,string>("dataTransferForResponding",dataTransferForResponding));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",dataTransferForResponding);
	}
	status=pathMap.insert(pair<string,string>("backupRestore",backupRestore));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",backupRestore);
	}
	status=pathMap.insert(pair<string,string>("swPackage",swPackage));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",swPackage);
	}
	status=pathMap.insert(pair<string,string>("healthCheck",healthCheck));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",healthCheck);
	}
	status=pathMap.insert(pair<string,string>("licenseFile",licenseFile));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",licenseFile);
	}
	status=pathMap.insert(pair<string,string>("tools",tools));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",tools);
	}
	status=pathMap.insert(pair<string,string>("supportData",supportData));

	//status=pathMap.insert(pair<string,string>((*it),folderPath));
	if(status.second==false)
	{
		INFO("%s","Data path already present for this symbolic name");
	}
	else
	{
		//DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
		DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",supportData);
	}
	//}
	std::map<std::string,std::string>::iterator itMap;
	itMap = pathMap.begin();
	DEBUG("%s","Iteration using iterator ");
	for(;itMap!=pathMap.end();itMap++)
	{
		DEBUG("%s","***********************FileM Folders***************************");
		DEBUG("Key =  %s",((*itMap).first).c_str());
		DEBUG("Velue = %s",((*itMap).second).c_str());
	}
	m_mapInternalFileMPaths = pathMap;  
	return true;

#endif
}

bool ACS_SSU_Path::createMap()
{
	map<string,string> pathMap;
	ACE_TCHAR szTemp[512];
	ACE_OS::strncpy(szTemp, ACE_TEXT(SSU_QUOTAS_FILE), 511);
	std::string pszReadLine ="\0";
	std::list<string> symNameList;
	std::pair<map<string,string>::iterator,bool> status;

	std::ifstream * m_hQuotaFile = new ifstream(szTemp,ios::in);

	if ( m_hQuotaFile == 0 )
	{
		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Failed to open the Folder Quota file \"%s\". \n%m"),
				szTemp
		);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		ERROR("%s",szData);
		delete m_hQuotaFile;
		return false;
	}
	while((!(std::getline(*m_hQuotaFile, pszReadLine)<=0)))
	{
		if ((pszReadLine.size()!=0))
		{
			int pos1=pszReadLine.find(',');
			int pos2=pszReadLine.find(',',pos1+1);
			std::string myStrIn=pszReadLine.substr(pos1+1,(pos2-pos1)-1);
			symNameList.push_back(myStrIn);
			symNameList.sort();
			symNameList.unique();
		}
	}
	delete m_hQuotaFile;
#ifdef SSUIPTTEST
	/*! Changes done for CR693.These changes can be removed once the
	symbolic name for the ipt folder exists and can able to access the GetDataDiskPath.*/

	const char* foldIpt="/data/ipt/";
	const char* foldIptData="/data/ipt/data";
	const char* foldIptLogs="/data/ipt/logs";
	bool result=ACS_SSU_Common::CheckFileExists(foldIpt);

	if(!result)
	{
		if(mkdir("/data/ipt/",S_IRWXU|S_IROTH)== 0)
		{
			//cout<<"IPT folder created successfully"<<endl;
			DEBUG("Directory for /data/ipt folder is created successfully: %d",result);
		}else
		{
			//cout<<"Failed to create the ipt folder"<< endl;
			DEBUG("Failed to create /data/ipt folder: %d",result);
		}
	}

	result=ACS_SSU_Common::CheckFileExists(foldIptData);
	if(result)
	{
		pathMap.insert(pair<string,string>(("IPT_DATA"),"/data/ipt/data"));
	}
	else
	{
		if(mkdir("/data/ipt/data",S_IRWXU|S_IROTH)== 0)
		{
			pathMap.insert(pair<string,string>(("IPT_DATA"),"/data/ipt/data"));
			//cout<<"IPT folder for /data/ipt/data created succesfully" <<endl;
			DEBUG("%s","Directory for /data/ipt/data folder is created successfully");
		}
		else
		{
			//cout<<"Failed to create the /data/ipt/data folder"<< endl;
			DEBUG("%s","Failed to create /data/ipt/data folder");
		}
	}


	result=ACS_SSU_Common::CheckFileExists(foldIptLogs);
	if (result)
	{
		pathMap.insert(pair<string,string>(("IPT_LOGS"),"/data/ipt/logs"));
	}
	else
	{
		if(mkdir("/data/ipt/logs",S_IRWXU|S_IROTH)== 0)
		{
			pathMap.insert(pair<string,string>(("IPT_LOGS"),"/data/ipt/logs"));
			//cout<<"IPT folder for /data/ipt/logs created succesfully" <<endl;
			DEBUG("%s","Directory for /data/ipt/logs folder is created successfully");
		}
		else
		{
			//cout<<"Failed to create the /data/ipt/logs folder"<< endl;
			DEBUG("%s","Failed to create /data/ipt/logs folder");
		}

	}
#endif


	std::list<string>::iterator it=symNameList.begin();
	std::string myStrOut=" ";
	for(;it!=symNameList.end(); ++it)
	{
		bool myResult = ACS_SSU_Common::GetDataDiskPath((*it), myStrOut);
		if (myResult)
		{
			status=pathMap.insert(pair<string,string>((*it),myStrOut));
			if(status.second==false)
			{
				INFO("%s","Data path already present for this symbolic name");
			}
			else
			{
				DEBUG("Data path obtained from Common APGCC LIB Successfully :%s",myStrOut.c_str());
			}

		}
		else
		{
			DEBUG(" Error in Obtaining Datapath for  %s from Common APGCC LIB",(*it).c_str());
#ifdef SSUIPTTEST
			// In this case it should not return any thing in case of failure
			if((*it).compare("IPT_LOGS") != 0 && (*it).compare("IPT_DATA") != 0)
			return false;
#else
			
			return false;
#endif
		}
	}

	m_mapInternalPaths=pathMap;
	return true;
}

ACS_SSU_Path::ACS_SSU_Path()
{
	Clear();
}

ACS_SSU_Path::ACS_SSU_Path(const ACE_TCHAR* lpszPath)
{
	Clear();
	Expand(lpszPath);
}

ACS_SSU_Path::~ACS_SSU_Path()
{
}

void ACS_SSU_Path::Clear()
{
	ACE_OS::memset((void*)&m_szPath,'\0', FILE_MAX_PATH);
}

const ACE_TCHAR* ACS_SSU_Path::Path() const
{
	return m_szPath;
}

ACS_SSU_Path::operator const ACE_TCHAR*() const
				{
	return m_szPath;
				}

ACS_SSU_Path::operator ACE_TCHAR*() const
				{
	return (ACE_TCHAR*)m_szPath;
				}

const ACE_TCHAR* ACS_SSU_Path::operator*() const
{
	return m_szPath;
}

void ACS_SSU_Path::SetPath(const ACE_TCHAR* lpszNewPath)
{
	Clear();
	Expand(lpszNewPath);
}

void ACS_SSU_Path::SetPath(const ACS_SSU_Path& NewPath)
{
	Clear();
	Expand(NewPath.Path());
}

void ACS_SSU_Path::vGetInternalPath( std::string &szPathSrc, std::string &szPathDest )
{
	std::map<std::string,std::string>::iterator it= m_mapInternalPaths.find(szPathSrc);

	if( it != m_mapInternalPaths.end())
	{
		szPathDest = it->second;
	}
	else
	{
		szPathDest = "";
	}
}

//FileM change
bool ACS_SSU_Path::ExpandFileMPath(const ACE_TCHAR* pszPath)
{
    std::string szSource(pszPath);
    std::string szDest("");
	std::map<std::string,std::string>::iterator it= m_mapInternalFileMPaths.find(szSource);
    if( it != m_mapInternalFileMPaths.end())
    {
        szDest = it->second;
    }
    else
    {
        szDest = "";
    }
    
	ACE_OS::strncpy(m_szPath, szDest.c_str(),szDest.length());
    return true;
}


bool ACS_SSU_Path::Expand(const ACE_TCHAR* pszPath)
{
	std::string szSource(pszPath);

	std::string szDest("");

	vGetInternalPath(szSource,szDest);

	ACE_OS::strncpy(m_szPath, szDest.c_str(),szDest.length());

	return true;
}


