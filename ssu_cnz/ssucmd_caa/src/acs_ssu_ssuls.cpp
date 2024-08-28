//=============================================================================
/**
 *  @file    ssuls.cpp
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *
 *  @author 2010-11-18 by XNADNAR
 *
 *  @documentno
 *
 *  @brief  It acts as OM for listing the information about the current folder
 *          quota usage that is supervised by SSU.
 *
 */
//=============================================================================


#include <stdio.h>
#include <string>
#include <iostream>
#include <ace/Get_Opt.h>
#include <acs_apgcc_omhandler.h>
#include <acs_prc_api.h>
#include "acs_ssu_types.h"
#include "acs_ssu_common.h"
#include "acs_ssu_param.h"
#include "acs_ssu_execute.h"

#define SSU_HW_VERSION_APG43 0

using namespace std;

ACS_SSU_Execute objSsuLs;

char *nameClass = const_cast<char *>("FolderQuotaInfo");
char saImmSSULSFName[]        	=     "ssuFolderName";
char attrLSRtQLimit[]			=	  "ssuFolderQuotaLimit";
char attrLSRtCSize[]			=	  "ssuCurrentFolderSize";
char attrLSRtA1ALimit[]			=	  "ssuFolderA1AlarmLimit";
char attrLSRtA1CLimit[]			=	  "ssuFolderA1CeaseLimit";
char attrLSRtA2ALimit[]			=	  "ssuFolderA2AlarmLimit";
char attrLSRtA2CLimit[]			=	  "ssuFolderA2CeaseLimit";
char attrRtCfg[]   				= 	  "ssuCurrConfiguration";

#define ONE_KB                0x400
#define ONE_MB                0x100000
#define ONE_GB                0x40000000

std::string ACS_SSU_Common::dnOfSSURoot = "";
ACS_TRA_Logging *ACS_SSU_Common::ACS_SSU_Logging=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_InformationTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_DebugTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_WarningTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_ErrorTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_FatalTrace=0;
ACS_SSU_Execute SSUErrorObj;

static void PrintUsage(ostream& outs);

// Command name
//static char*  g_lpszName = "ssuls";

/**
 * @brief
 * PrintUsage: this method is used to print the correct usage of ssuls command
 * @param  outs      :	ostream object
 * @return void
 */

void PrintUsage(ostream& outs)
{
   outs << "Usage: ssuls [-l]\n "<<endl;
}

/**
 * @brief
 * GetListOfExistingRDNs: this method is used to fetch existing ssuls runtime objects.
 * @param  existingList      :	vector<string>
 * @return void
 */
void GetListOfExistingRDNs(vector<string>& existingList)
{

	FILE *fp = NULL;
	std::string cmd("immfind -c ");
	ACE_TCHAR data[200];
	cmd.append(nameClass);

	fp = popen(cmd.c_str(),"r");

	if (fp == NULL)
	{
		return;
	}

	while( fgets(data, 200, fp) != NULL)
	{
		ACE_INT32 len = strlen(data);
		if( data[len-1] == '\n' )
			data[len-1] = '\0';
		existingList.push_back(data);
	}

	pclose(fp);

}
/**
 * @brief
 * isSSUServiceisRunning: this method is used to identify ssu service is running or not.
 * @param  none
 * @return bool
 */
bool isSSUServiceisRunning()
{

	FILE *fp = NULL;
	std::string cmd = "ps -ea | grep acs_ssumonitord | grep -v \"grep\" 1>/dev/null 2>/dev/null";
	int ret = -1;
	int status = -1;

	fp = popen(cmd.c_str(),"r");

	if (fp == NULL)
	{
		return false;
	}

	status = pclose(fp);

	if (WIFEXITED(status))
	{
		ret=WEXITSTATUS(status);
	}

	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief
 * ACE_TMAIN: Main Method
 * @return int
 */

int ACE_TMAIN( ACE_INT32 argc, ACE_TCHAR* argv[] )
{
   ACE_INT32 nExitCode = 0;
   ACS_SSU_Common::ACS_SSU_ErrorTrace = new ACS_TRA_trace("SSUERROR","C1024");
   ACS_SSU_Common::ACS_SSU_Logging = new ACS_TRA_Logging();
   ACS_SSU_Common::ACS_SSU_Logging->Open("SSU");
   // This command can only be executed on active node.
   ACS_PRC_API prcObj;
   if ((prcObj.askForNodeState())!=1)
   {
	   ERROR("%s","SSULS: PRC API is returning NODE STATE as PASSIVE");
	   cerr << objSsuLs.GetResultCodeText(SSU_RC_NOTACTIVENODE, &nExitCode) << endl<<endl;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return nExitCode;
   }

   // This command can only be executed on APG43
   if (ACS_SSU_Common::GetHWVersion() != SSU_HW_VERSION_APG43)
   {
	   ERROR("%s","SSULS: GetHWVersion is not APG43");
	   cerr << objSsuLs.GetResultCodeText(SSU_RC_ONAPG43ONLY, &nExitCode) << endl<<endl;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return nExitCode;
   }

   ACE_INT32 l_flag = 0;
   ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("l"));
   ACE_INT32 c;

   while ((c = get_opt()) != -1)
   {
	   switch (c)
	   {
	   case 'l':
		   l_flag++;
		   break;

	   case '?':
		   cerr << "Incorrect usage\n";
		   PrintUsage(cerr);
		   ACS_SSU_Common::ACS_SSU_Logging->Close();
		   return 2;
		   //break;

	   default:
		   break;
	   }
   }

   // Option parsing
   if (l_flag > 1)
   {
	   cerr << "Incorrect usage\n";
	   PrintUsage(cerr);
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return 2;
   }

   // Check ssu service is running or not?
   if(!isSSUServiceisRunning())
   {
	   ERROR("%s","SSULS: ssumonitord is not running");
	   cout<<"Unable to connect to server\n"<<endl;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return 117;
   }

   ACS_CC_ImmParameter paramToFind;
   OmHandler* omHandlerPtr = new OmHandler ();

   if( omHandlerPtr->Init() == ACS_CC_FAILURE )
   {
	   ERROR("%s","SSULS: OmHandler Init failure !!!");
	   cout<<"Unable to connect to server\n"<<endl;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return 117;
   }

   std::vector<std::string> p_dnList;
   omHandlerPtr->getClassInstances(nameClass,p_dnList);
   if(p_dnList.size()==0)
   {
	   ERROR("%s","SSULS: Existing RDN List is zero");
	   cout<<"Unable to connect to server\n"<<endl;
	   omHandlerPtr->Finalize();
	   delete omHandlerPtr;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return 117;
   }

   // Check SSU conf XML files are configured or not?
   if (ACS_SSU_Common::fetchDnOfRootObjFromIMM(omHandlerPtr) == -1)
   {
	   ERROR("%s","SSULS: SSU related XML files are not configured");
	   cerr << SSUErrorObj.GetResultCodeText(SSU_RC_SERVERNOTRESPONDING, &nExitCode) << endl;
	   omHandlerPtr->Finalize();
	   delete omHandlerPtr;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return nExitCode;
   }

   paramToFind.attrName = new char [30];
   ACS_CC_ReturnType enResult;
   char* pszAttrValue;
   char dnName[150];

   ACE_OS::strcpy(dnName,SSU_CONFIG_OBJ_RDN);
   ACE_OS::strcat(dnName,",");
   ACE_OS::strcat(dnName,ACS_SSU_Common::dnOfSSURoot.c_str());

   ACE_OS::strcpy( paramToFind.attrName , SSU_IMM_CONFIG_ATTR_NAME);

   if((enResult = omHandlerPtr->getAttribute(dnName,&paramToFind)) == ACS_CC_SUCCESS)
   {
	   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
	   if(ACE_OS::strcmp(pszAttrValue , "") == 0 )
	   {
		   ERROR("%s","SSULS: system configuration received from IMM object is null");
		   cout<<"Unable to connect to server\n"<<endl;
		   omHandlerPtr->Finalize();
		   delete omHandlerPtr;
		   delete [] paramToFind.attrName;
		   ACS_SSU_Common::ACS_SSU_Logging->Close();
		   return 117;
	   }
   }
   else
   {
	   ERROR("%s","SSULS: unable to fetch system configuration from IMM object");
	   cout<<"Unable to connect to server\n"<<endl;
	   omHandlerPtr->Finalize();
	   delete omHandlerPtr;
	   delete [] paramToFind.attrName;
	   ACS_SSU_Common::ACS_SSU_Logging->Close();
	   return 117;
   }

   if (l_flag == 1)
   {
	   std::string configType(pszAttrValue);
	   ACE_OS::strcpy(dnName,p_dnList[0].c_str());
	   ACE_OS::strcpy(paramToFind.attrName , attrRtCfg);
	   if((enResult = omHandlerPtr->getAttribute(dnName, &paramToFind)) == ACS_CC_SUCCESS )
	   {
		   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
		   if(ACE_OS::strcmp(pszAttrValue , "") == 0 )
		   {
			   ERROR("%s","SSULS: system configuration received from SSULS Runtime IMM object is null");
			   cout<<"Unable to connect to server\n"<<endl;
			   omHandlerPtr->Finalize();
			   delete omHandlerPtr;
			   delete [] paramToFind.attrName;
			   ACS_SSU_Common::ACS_SSU_Logging->Close();
			   return 117;
		   }
	   }
	   else
	   {
		   ERROR("%s","SSULS: unable to fetch system configuration from SSULS Runtime object");
		   cout<<"Unable to connect to server\n"<<endl;
		   omHandlerPtr->Finalize();
		   delete omHandlerPtr;
		   delete [] paramToFind.attrName;
		   ACS_SSU_Common::ACS_SSU_Logging->Close();
		   return 117;
	   }
	   if(ACE_OS::strcmp(configType.c_str() , "") != 0 )
	   {
		   cout << endl << "AP CONFIGURATION TYPE" << endl << endl;
		   cout << configType << endl;
	   }

	   ACE_UINT64 quotaLimit;
	   for( unsigned int i = 0; i <  p_dnList.size(); i++)
	   {
		   ACE_OS::strcpy(dnName,p_dnList[i].c_str());
		   char dataBuffer[1024];

		   // For Fetching Folder Name from Runtime objects
		   ACE_OS::strcpy( paramToFind.attrName , saImmSSULSFName);
		   if((enResult = omHandlerPtr->getAttribute(dnName, &paramToFind)) == ACS_CC_SUCCESS)
		   {
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-30s%s\n"), ACE_TEXT("Folder name:"), pszAttrValue);
			   cout << endl << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder name from SSULS Runtime object");
			   break;
		   }

		   // For Fetching Folder Quota Limit from Runtime objects
		   ACE_OS::strcpy( paramToFind.attrName , attrLSRtQLimit);
		   if((enResult = omHandlerPtr->getAttribute(dnName,&paramToFind)) == ACS_CC_SUCCESS)
		   {
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   quotaLimit = atoi(pszAttrValue);
			   ACE_OS::sprintf(dataBuffer,
					   ACE_TEXT("%-30s%0.2f%s\n"),
					   ACE_TEXT("Quota limit:"),
					   (quotaLimit >= 1000 ? (quotaLimit/1000.0) : (DOUBLE) quotaLimit),
					   (ACE_TCHAR*)(quotaLimit >= 1000 ? ACE_TEXT("GB") : ACE_TEXT("MB")));

			   cout << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder quota limit from SSULS Runtime object");
			   break;
		   }

		   // For Fetching Folder Current size from Runtime objects
		   ACE_OS::strcpy(paramToFind.attrName,attrLSRtCSize);
		   if((enResult = omHandlerPtr->getAttribute(dnName,&paramToFind)) == ACS_CC_SUCCESS)
		   {
			   ACE_UINT64 curSize;
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   curSize = atol(pszAttrValue);
			   ACE_UINT64 u64LimitQuota;
			   if (quotaLimit < 1000)
			   {
				   u64LimitQuota = (ACE_UINT64)quotaLimit * (ACE_UINT64)ONE_MB;
			   }
			   else
			   {
				   u64LimitQuota = (ACE_UINT64)(((DOUBLE)quotaLimit / 1000.0) * (DOUBLE)ONE_GB);
			   }

			   double nSize;
			   ACE_INT32 k;
			   ACE_UINT64 nPerc = (ACE_UINT64)(((DOUBLE)curSize/(DOUBLE)u64LimitQuota)*1000.0);
			   k= nPerc%10;
			   if(k>=5)
			   {
				   nPerc=(nPerc/10)+1;
			   }
			   else
			   {
				   nPerc=nPerc/10;
			   }

			   if (curSize < (ACE_UINT64)ONE_MB)
			   {
				   nSize = ((DOUBLE)curSize / (DOUBLE)ONE_KB);
				   ACE_OS::sprintf(dataBuffer,
						   ACE_TEXT("%-30s%0.2f%s (%d%% of quota limit)\n"),
						   ACE_TEXT("Current folder size:"),
						   nSize,
						   (ACE_TCHAR*)ACE_TEXT("KB"),
						   nPerc);
			   }
			   else if (curSize < (ACE_UINT64)ONE_GB)
			   {
				   nSize = ((DOUBLE)curSize / (DOUBLE)ONE_MB);
				   ACE_OS::sprintf(dataBuffer,
						   ACE_TEXT("%-30s%0.2f%s (%d%% of quota limit)\n"),
						   ACE_TEXT("Current folder size:"),
						   nSize,
						   (ACE_TCHAR*)ACE_TEXT("MB"),
						   nPerc);
			   }
			   else
			   {
				   nSize = ((long double)curSize / (DOUBLE)ONE_GB);
				   ACE_OS::sprintf(dataBuffer,
						   ACE_TEXT("%-30s%0.2f%s (%d%% of quota limit)\n"),
						   ACE_TEXT("Current folder size:"),
						   nSize,
						   (ACE_TCHAR*)ACE_TEXT("GB"),
						   nPerc);
			   }
			   cout << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder current size from SSULS Runtime object");
			   break;
		   }

		   // For Fetching Folder A2 Alarm Limit from Runtime objects
		   ACE_OS::strcpy( paramToFind.attrName , attrLSRtA2ALimit);
		   if( ( enResult = omHandlerPtr->getAttribute( dnName, &paramToFind ) ) == ACS_CC_SUCCESS )
		   {
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   ACE_UINT32 nA2AlarmLevel = atoi(pszAttrValue);
			   ACE_OS::sprintf(dataBuffer,
					   ACE_TEXT("%-30s%d%% free space\n"),
					   ACE_TEXT("A2 alarm level:"),
					   nA2AlarmLevel);
			   cout << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder A2 Alarm limit from SSULS Runtime object");
			   break;
		   }

		   // For Fetching Folder A2 Cease Limit from Runtime objects
		   ACE_OS::strcpy( paramToFind.attrName , attrLSRtA2CLimit);

		   if( ( enResult = omHandlerPtr->getAttribute( dnName, &paramToFind ) ) == ACS_CC_SUCCESS )
		   {
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   ACE_UINT32 nA2CeaseLevel = atoi(pszAttrValue);
			   ACE_OS::sprintf(dataBuffer,
					   ACE_TEXT("%-30s%d%% free space\n"),
					   ACE_TEXT("A2 cease level:"),
					   nA2CeaseLevel);
			   cout << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder A2 cease limit from SSULS Runtime object");
			   break;
		   }

		   // For Fetching Folder A1 Alarm Limit from Runtime objects
		   ACE_OS::strcpy( paramToFind.attrName , attrLSRtA1ALimit);

		   if( ( enResult = omHandlerPtr->getAttribute( dnName, &paramToFind ) ) == ACS_CC_SUCCESS )
		   {
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   ACE_UINT32 nA1AlarmLevel = atoi(pszAttrValue);
			   ACE_OS::sprintf(dataBuffer,
					   ACE_TEXT("%-30s%d%% free space\n"),
					   ACE_TEXT("A1 alarm level:"),
					   nA1AlarmLevel);
			   cout << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder A1 alarm limit from SSULS Runtime object");
			   break;
		   }

		   // For Fetching Folder A1 Cease Limit from Runtime objects
		   ACE_OS::strcpy( paramToFind.attrName , attrLSRtA1CLimit);

		   if( ( enResult = omHandlerPtr->getAttribute( dnName, &paramToFind ) ) == ACS_CC_SUCCESS )
		   {
			   pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			   ACE_UINT32 nA1CeaseLevel = atoi(pszAttrValue);
			   ACE_OS::sprintf(dataBuffer,
					   ACE_TEXT("%-30s%d%% free space\n"),
					   ACE_TEXT("A1 cease level:"),
					   nA1CeaseLevel);
			   cout << dataBuffer;
		   }
		   else
		   {
			   ERROR("%s","SSULS: unable to fetch folder A1 cease limit from SSULS Runtime object");
			   break;
		   }
	   }
   }
   else
   {
	   if(ACE_OS::strcmp(pszAttrValue , "") != 0 )
	   {
		   cout << endl << "AP CONFIGURATION TYPE" << endl << endl;
		   cout << pszAttrValue << endl;
	   }
   }
   cout<<endl;
   delete [] paramToFind.attrName;
   omHandlerPtr->Finalize();
   delete omHandlerPtr;
   ACS_SSU_Common::ACS_SSU_Logging->Close();
   return nExitCode;
}


