//=============================================================================
/**
*  @file    ssucfg.cpp
*
*  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
*
*  @author 2010-10-27 by XNADNAR
*
*  @documentno
*
*  @brief  It acts as OM for modifying Configuration type of Folder quotas.
*
*/
//=============================================================================
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <ace/ACE.h>
#include <ace/Get_Opt.h>
#include <ACS_CC_Types.h>
#include <acs_apgcc_omhandler.h>
#include <acs_prc_api.h>
#include "acs_ssu_types.h"
#include "acs_ssu_common.h"
#include "acs_ssu_param.h"
#include "acs_ssu_execute.h"

using namespace std;
ACS_SSU_Execute SSUErrorObj;
static void PrintUsage(ostream& outs);
#define SSU_MAX_CONFIG_LENGTH 80

std::string ACS_SSU_Common::dnOfSSURoot = "";
// TRA Objects
ACS_TRA_Logging *ACS_SSU_Common::ACS_SSU_Logging=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_InformationTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_DebugTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_WarningTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_ErrorTrace=0;
ACS_TRA_trace * ACS_SSU_Common::ACS_SSU_FatalTrace=0;

/**
* @brief
* vToUpper: this method is used to Converts ACE char data to uppercase.
* @param  pszData      :	ACE_TCHAR
* @return void
*/
void vToUpper( ACE_TCHAR * pszData )
{
	ACE_TCHAR *tmp = pszData;

	while( *tmp != 0 )
	{
		if( *tmp>=97 && *tmp <= 122 )
		{
			*tmp = 65+(*tmp-97);
		}

		tmp++;
	}
}
/**
* @brief
* PrintUsage: this method is used to print the correct usage of ssucfg command
* @param  outs      :	ostream object
* @return void
*/
void PrintUsage(ostream& outs)
{
	outs << "Usage: ssucfg -c config [-f]\n" << endl;
}

/**
* @brief
* ACE_TMAIN: Main Method
* @return int
*/

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	ACE_INT32 nExitCode = 0;
	ACS_SSU_Common::ACS_SSU_ErrorTrace = new ACS_TRA_trace("SSUERROR","C1024");
	ACS_SSU_Common::ACS_SSU_Logging = new ACS_TRA_Logging();
	ACS_SSU_Common::ACS_SSU_Logging->Open("SSU");

	// This command can only be executed on active node.
	ACS_PRC_API prcObj;
    if((prcObj.askForNodeState())!=1)
	{
    	ERROR("%s","SSUCFG: PRC API is returning NODE STATE as PASSIVE");
		cerr << SSUErrorObj.GetResultCodeText(SSU_RC_NOTACTIVENODE, &nExitCode)<< endl << endl;
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;
	}

	// This command can only be executed on APG43 (it's the same in BC)
	if (ACS_SSU_Common::GetHWVersion() != SSU_HW_VERSION_APG43 )
	{
		ERROR("%s","SSUCFG: GetHWVersion is not APG43");
		cerr << SSUErrorObj.GetResultCodeText(SSU_RC_ONAPG43ONLY, &nExitCode) << endl<< endl;
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;
	}

	ACE_INT32  c_flag = 0;
	ACE_INT32  f_flag = 0;
	ACE_TCHAR* pConfigType = NULL;

	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("c:f"));
	ACE_INT32 c;

	while ((c = get_opt()) != -1)
	{
		switch (c)
		{
		case 'c':
			c_flag++;
			pConfigType = get_opt.opt_arg();
			break;

		case 'f':
			f_flag++;
			break;

		case '?':
			cerr << SSUErrorObj.GetResultCodeText(SSU_RC_UNKNOWNCMDTYPE, &nExitCode) << endl;
			PrintUsage(cerr);
			ACS_SSU_Common::ACS_SSU_Logging->Close();
			return nExitCode;

		default:
			break;
		}
	}

	// Parsing command options
	if (c_flag != 1 || !pConfigType || f_flag > 1)
	{
		cerr << SSUErrorObj.GetResultCodeText(SSU_RC_UNKNOWNCMDTYPE, &nExitCode) << endl;
		PrintUsage(cerr);
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;

	}

	//! This is a special case when the <config type> was not given and the
	//! following argument "-f" is counted as the <config type>
	if (f_flag == 0 && (ACE_OS::strcmp(pConfigType, ACE_TEXT("-f")) == 0))
	{
		cerr << SSUErrorObj.GetResultCodeText(SSU_RC_UNKNOWNCMDTYPE, &nExitCode) << endl;
		PrintUsage(cerr);
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;
	}

	if (f_flag == 0)
	{
		// Issue check printout
		cout << "\nAre you sure you want to change AP Configuration Type for the SSU\n"
				<< "folder quota supervision? Enter y or n [default: n]\03: " << flush;

		ACE_TCHAR szResponce[64];
		cin.getline(szResponce, 63);

		// Quit here in case of no response
		if (!(*szResponce) || ACE_OS::strcasecmp(szResponce, "no") == 0 || ACE_OS::strcasecmp(szResponce, "n") == 0)
		{
			ACS_SSU_Common::ACS_SSU_Logging->Close();
			return 0;
		}

		if (ACE_OS::strcasecmp(szResponce, "yes") != 0 && ACE_OS::strcasecmp(szResponce, "y") != 0)
		{
			cerr << SSUErrorObj.GetResultCodeText(SSU_RC_UNKNOWNCMDTYPE, &nExitCode) << endl<< endl;
			ACS_SSU_Common::ACS_SSU_Logging->Close();
			return nExitCode;
		}
	}

	OmHandler omHandler;
	if( omHandler.Init(REGISTERED_OI)== ACS_CC_FAILURE )
	{
		ERROR("%s","SSUCFG: OmHandler Init is failed as acs_ssumonitord is not running");
		cerr << SSUErrorObj.GetResultCodeText(SSU_RC_SERVERNOTRESPONDING, &nExitCode) << endl<< endl;
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;
	}

	if (ACS_SSU_Common::fetchDnOfRootObjFromIMM(&omHandler) == -1)
	{
		ERROR("%s","SSUCFG: SSU related XML files are not configured");
		cerr << SSUErrorObj.GetResultCodeText(SSU_RC_SERVERNOTRESPONDING, &nExitCode) << endl<< endl;
		omHandler.Finalize();
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;
	}
	ACE_TCHAR pszConfigType[SSU_MAX_CONFIG_LENGTH];
	ACE_OS::strcpy(pszConfigType,pConfigType);
	vToUpper(pszConfigType);
	ACE_TCHAR errorHandlerdnName[150];
	ACE_OS::sprintf(errorHandlerdnName,"%s,%s",ACE_TEXT(SSU_ERRORHANDLER_RDNOBJECTNAME),ACS_SSU_Common::dnOfSSURoot.c_str());
	ACS_CC_ImmParameter paramToChange;

	paramToChange.attrName = new ACE_TCHAR [30];
	ACE_OS::strcpy( paramToChange.attrName ,SSU_IMM_CONFIG_ATTR_NAME);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(pszConfigType);

	std::string dnOfSSUConfigObj(SSU_CONFIG_OBJ_RDN);
	dnOfSSUConfigObj.append(",");
	dnOfSSUConfigObj.append(ACS_SSU_Common::dnOfSSURoot);

	ACE_INT32 returnCode = omHandler.modifyAttribute( dnOfSSUConfigObj.c_str() , &paramToChange);

	if(returnCode != ACS_CC_SUCCESS)
	{
		string errorText;
		ERROR("SSUCFG: modify attribute is failed due to %s",omHandler.getInternalLastErrorText());
		nExitCode = omHandler.getInternalLastError();
		if(nExitCode == -12)
		{
			cerr << SSUErrorObj.GetResultCodeText(SSU_RC_SERVERNOTRESPONDING, &nExitCode) << endl<< endl;
			nExitCode = 117;
		}
		else
		{
			omHandler.getExitCode(nExitCode, errorText);
			cerr<<errorText<<endl<< endl;
		}

		omHandler.Finalize();
		delete [] paramToChange.attrName;
		ACS_SSU_Common::ACS_SSU_Logging->Close();
		return nExitCode;
	}

	omHandler.Finalize();
	delete [] paramToChange.attrName;
	ACS_SSU_Common::ACS_SSU_Logging->Close();
	return nExitCode;
}


