
//******************************************************************************//
//  NAME
//     ACS_SSU_CommandHandler.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
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
//
//  AUTHOR
//     2010-10-27 by XNADNAR
//
//  SEE ALSO
//     -
//
//******************************************************************************
#include <iostream>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <acs_apgcc_oihandler_V2.h>
#include "acs_ssu_types.h"
#include "acs_ssu_param.h"
#include "acs_ssu_execute.h"
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_command_handler.h"

using namespace std;
ACS_SSU_Execute SSUErrorObj;
bool applyOperationInProgress = false;

//! Constructor
ACS_SSU_CommandHandler::ACS_SSU_CommandHandler( ACE_Event* poEvent,
		                   ACS_SSU_FolderQuotaMon* pFolderQuotaMon,
		                   string szimpName )
:acs_apgcc_objectimplementereventhandler_V2( szimpName ),
 m_phEndEvent( poEvent ),
 m_pFolderQuotaMon( pFolderQuotaMon )
{
	m_szRequestedConfig.clear();
	theOiHandlerPtr = 0;theOiHandlerPtr = new acs_apgcc_oihandler_V2();
	if( theOiHandlerPtr == 0 )
	{
		ERROR("%s", "Memory allocation failed for  acs_apgcc_oihandler_V2");
	}
}

//! Destructor
ACS_SSU_CommandHandler::~ACS_SSU_CommandHandler()
{
	if( theOiHandlerPtr != 0 )
	{
		theOiHandlerPtr->removeClassImpl(this,SSU_ROOT_CLASS_NAME);
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}
}

ACE_INT32 ACS_SSU_CommandHandler::svc( ACE_Reactor*& poReactor )
{
	DEBUG("%s","Entering ACS_SSU_CommandHandler::svc");

	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,SSU_ROOT_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		NOTICE( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s","Exiting ACS_SSU_CommandHandler::svc");
		return -1;
	}
	else
	{
		NOTICE( "Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
	DEBUG("%s","Exiting ACS_SSU_CommandHandler::svc");
	return 0;
}


ACS_CC_ReturnType ACS_SSU_CommandHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("oiHandle %ld ccbId %ld className %s parentName %s ACS_APGCC_AttrValues %u \n",oiHandle,ccbId,className,parentName,attr);

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_SSU_CommandHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_SSU_CommandHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_CC_ReturnType enResult=ACS_CC_FAILURE;
	ACE_INT32 nExitCode;
	if (!applyOperationInProgress)
	{
		DEBUG ("%s","\n---------------------------------------------------\n");
		DEBUG ("%s","            ObjectModifyCallback invocated           \n");
		DEBUG ("%s","-----------------------------------------------------\n");
		DEBUG ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
		DEBUG ("Object Name = %s ", objName);

		if (ACS_SSU_Common::GetHWVersion()!=0)
		{
			char* errorText = SSUErrorObj.GetResultCodeText(SSU_RC_ONAPG43ONLY, &nExitCode);
			std::string myErrStr(errorText);
			if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
			{
				DEBUG("%s ","Error in setting error text");
			}
			return enResult;
		}

		if (!ACS_SSU_Common::CheckIfActiveNode())
		{
			char* errorText = SSUErrorObj.GetResultCodeText(SSU_RC_NOTACTIVENODE, &nExitCode);
			std::string myErrStr(errorText);
			if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
			{
				DEBUG("%s ","Error in setting error text");
			}
			return enResult;
		}

		if (!ACS_SSU_Common::CheckFileExists(ACS_SSU_QUOTA_CHECK_BIN_PATH))
		{
			char* errorText = SSUErrorObj.GetResultCodeText(SSU_RC_QUOTACHECKERROR, &nExitCode);
			std::string myErrStr(errorText);
			if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
			{
				DEBUG("%s ","Error in setting error text");
			}
			return enResult;
		}
		int i = 0;
		if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,SSU_SYSTEM_CONFIGURATION) == 0 ) //&& attrMods[i]->modType == ACS_APGCC_ATTR_VALUES_REPLACE )
		{
			int* pszConfig = reinterpret_cast<int*> ( attrMods[i]->modAttr.attrValues[0]);

			ACE_INT32 nExitCode = 0;
			// For Checking config type//Updated for vMSC-300 and vMSC-147
                        // Updated for WIRELINE
			if (*pszConfig != SSU_BSC_CONFIG_VALUE &&
				*pszConfig != SSU_MSC_CONFIG_VALUE &&
				*pszConfig != SSU_MSC_BC_AP1_CONFIG_VALUE &&
				*pszConfig != SSU_MSC_BC_AP2_CONFIG_VALUE &&
				*pszConfig != SSU_MSC_400_AP1_CONFIG_VALUE &&
				*pszConfig != SSU_MSC_400_AP2_CONFIG_VALUE &&
				*pszConfig != SSU_MSC_147_AP2_CONFIG_VALUE &&
				*pszConfig != SSU_HLR_CONFIG_VALUE &&
				*pszConfig != SSU_HLR_SS_CONFIG_VALUE &&
				*pszConfig != SSU_HLR_BS_CONFIG_VALUE &&
				*pszConfig != SSU_MSCBC_AP1_8M450_CONFIG_VALUE &&
				*pszConfig != SSU_MSCBC_AP2_8M450_CONFIG_VALUE &&
				*pszConfig != SSU_MSCBC_AP1_8M600_CONFIG_VALUE &&
				*pszConfig != SSU_MSCBC_AP2_8M600_CONFIG_VALUE &&
				*pszConfig != SSU_HLR_TSC_CONFIG_VALUE &&
				*pszConfig != SSU_BSC_400_CONFIG_VALUE &&
				*pszConfig != SSU_BSP_MSC_BC_AP1_1200_CONFIG_VALUE &&
				*pszConfig != SSU_BSP_MSC_BC_AP2_1200_CONFIG_VALUE &&
				*pszConfig != SSU_HLR_SS_1200_CONFIG_VALUE &&
				*pszConfig != SSU_BSP_HLR_BS_1200_CONFIG_VALUE &&
				*pszConfig != SSU_HLR_TSC_1200_CONFIG_VALUE &&
				*pszConfig != SSU_vMSC_AP1_300_CONFIG_VALUE &&
				*pszConfig != SSU_vMSC_AP1_147_CONFIG_VALUE &&
				*pszConfig != SSU_vIPSTP_AP1_147_CONFIG_VALUE &&
				*pszConfig != SSU_IPSTP_CONFIG_VALUE &&
                                *pszConfig != SSU_WLN_147_AP1_CONFIG_VALUE
				*pszConfig != SSU_vHLR_BS_250_CONFIG_VALUE
			)
			{
				char* errorText =  SSUErrorObj.GetResultCodeText(SSU_RC_CONFIGCHANGEERROR, &nExitCode);
				std::string myErrStr(errorText);
				myErrStr = myErrStr + getConfigString(*pszConfig);
				if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
				{
					DEBUG("%s ","Error in setting error text");
				}
				return enResult;
			}

			bool isMultCPSys = ACS_SSU_Common::getIsMultCPSysValue();
			if ((*pszConfig == SSU_MSC_BC_AP1_CONFIG_VALUE ||
					*pszConfig == SSU_MSC_BC_AP2_CONFIG_VALUE ||
					*pszConfig == SSU_BSP_MSC_BC_AP1_1200_CONFIG_VALUE  ||
					*pszConfig == SSU_BSP_MSC_BC_AP2_1200_CONFIG_VALUE ||
					*pszConfig == SSU_MSCBC_AP1_8M450_CONFIG_VALUE ||
					*pszConfig == SSU_MSCBC_AP2_8M450_CONFIG_VALUE ||
					*pszConfig == SSU_MSCBC_AP1_8M600_CONFIG_VALUE ||
					*pszConfig == SSU_MSCBC_AP2_8M600_CONFIG_VALUE ||
                                        *pszConfig == SSU_BSP_HLR_BS_1200_CONFIG_VALUE ||
					*pszConfig == SSU_HLR_BS_CONFIG_VALUE ||
					*pszConfig == SSU_vHLR_BS_250_CONFIG_VALUE)
					&& (!isMultCPSys))
			{
				char* errorText = SSUErrorObj.GetResultCodeText(SSU_RC_ISNOTMULTCP, &nExitCode);
				std::string myErrStr(errorText);
				if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
				{
					DEBUG("%s ","Error in setting error text");
				}
				return enResult;
			}

			// The configuration msc-147-ap2 is valid only in a single CP system
			// if applyied in a multy CP system the error code 12 is returned this error code means that
			// The argument given as configuration type is not supported in this system configuration
			// Updated for vMSC-300 and vMSC-147
                        // Updated for WIRELINE
			if (*pszConfig == SSU_MSC_147_AP2_CONFIG_VALUE ||
				 *pszConfig == SSU_MSC_CONFIG_VALUE ||
				 *pszConfig == SSU_IPSTP_CONFIG_VALUE ||
                                 *pszConfig == SSU_WLN_147_AP1_CONFIG_VALUE ||
				 *pszConfig == SSU_HLR_SS_CONFIG_VALUE ||
				 *pszConfig == SSU_vMSC_300_AP1_CONFIG_VALUE ||
				 *pszConfig == SSU_vMSC_147_AP1_CONFIG_VALUE ||
				 *pszConfig == SSU_vIPSTP_147_AP1_CONFIG_VALUE ||
				 *pszConfig != SSU_MSC_400_AP1_CONFIG_VALUE &&
				 *pszConfig != SSU_MSC_400_AP2_CONFIG_VALUE &&
				 *pszConfig != SSU_HLR_CONFIG_VALUE
					&& isMultCPSys )
			{
				char* errorText = SSUErrorObj.GetResultCodeText(SSU_RC_ISNOTMULTCP, &nExitCode);
				std::string myErrStr(errorText);
				if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
				{
					DEBUG("%s ","Error in setting error text");
				}
				return enResult;
			}

			ACE_TCHAR* configString = getConfigString(*pszConfig);
			ACE_INT32 s32Result = m_pFolderQuotaMon->enCheckConfigChange( configString );

			if(s32Result  == SSU_RC_OK)
			{
				m_szRequestedConfig = configString;
				enResult = ACS_CC_SUCCESS;
			}
			else
			{
				char* errorText = SSUErrorObj.GetResultCodeText(s32Result, &nExitCode);
				std::string myErrStr(errorText);
				if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
				{
					DEBUG("%s ","Error in setting error text");
				}
				return enResult;
			}
		}
	}
	else
	{
		// apply callback execution is in progress
		char* errorText = SSUErrorObj.GetResultCodeText(SSU_RC_SERVICEISBUSY, &nExitCode);
		std::string myErrStr(errorText);
		if( setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
		{
			DEBUG("%s ","Error in setting error text");
		}
		return enResult;
	}


	return enResult;
}

ACS_CC_ReturnType ACS_SSU_CommandHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);

	return ACS_CC_SUCCESS;

}

void ACS_SSU_CommandHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);

}

void ACS_SSU_CommandHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	applyOperationInProgress = true;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            CcbApplyCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("oiHandle %ld ccbId %ld \n",oiHandle,ccbId);

	ACE_INT32 s32Result;
	ACE_INT32 nExitCode;
	if( !m_szRequestedConfig.empty() )
	{
	   s32Result = m_pFolderQuotaMon->SetConfigType(m_szRequestedConfig.c_str());
	   char* errorText = SSUErrorObj.GetResultCodeText(s32Result, &nExitCode);
	   std::string myErrStr(errorText);
	   if(setErrorText(ccbId,(unsigned int)nExitCode,myErrStr) == false)
	   {
		DEBUG("%s ","Error in setting error text");
	   }

	   m_szRequestedConfig.clear();
	   m_pFolderQuotaMon->ListFolderQuotaConfig(true);
	}
	applyOperationInProgress = false;
}

ACS_CC_ReturnType ACS_SSU_CommandHandler::updateRuntime(const char *objName, const char *attrName)
{
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("Object Name %s \t Attr Name %s \n", objName , attrName);

	return ACS_CC_SUCCESS;
}

void ACS_SSU_CommandHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("Object Name %s \t \n",  p_objName);

}
bool ACS_SSU_CommandHandler::setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText)
{
	(void) ccbId;
	DEBUG("%s","SSU Reporting Following Error while serving SSU Commands");
	DEBUG("Error ID = %d",aErrorId);
	DEBUG("ERROR TEXT = %s",ErrorText.c_str());
	setExitCode(aErrorId,ErrorText);
	return true;
}

ACE_TCHAR* ACS_SSU_CommandHandler::getConfigString(ACE_INT32 configValue)
{
	switch(configValue)
	{
	case SSU_BSC_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_BSC_CONFIG);
		return (char *) SSU_BSC_CONFIG;
	case SSU_MSC_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSC_CONFIG);
		return (char *) SSU_MSC_CONFIG;
	case SSU_HLR_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_HLR_CONFIG);
		return (char *) SSU_HLR_CONFIG;
	case SSU_HLR_SS_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_HLR_SS_CONFIG);
		return (char *) SSU_HLR_SS_CONFIG;
	case SSU_MSC_BC_AP1_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSC_BC_AP1_CONFIG);
		return (char *) SSU_MSC_BC_AP1_CONFIG;
	case SSU_MSC_BC_AP2_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSC_BC_AP2_CONFIG);
		return (char *) SSU_MSC_BC_AP2_CONFIG;
	case SSU_MSC_400_AP1_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSC_400_AP1_CONFIG);
		return (char *) SSU_MSC_400_AP1_CONFIG;
	case SSU_MSC_400_AP2_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSC_400_AP2_CONFIG);
		return (char *) SSU_MSC_400_AP2_CONFIG;
	case SSU_MSCBC_AP1_8M450_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSCBC_AP1_8M450_CONFIG);
		return (char *) SSU_MSCBC_AP1_8M450_CONFIG;
	case SSU_MSCBC_AP2_8M450_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSCBC_AP2_8M450_CONFIG);
		return (char *) SSU_MSCBC_AP2_8M450_CONFIG;
	case SSU_MSCBC_AP1_8M600_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSCBC_AP1_8M600_CONFIG);
		return (char *) SSU_MSCBC_AP1_8M600_CONFIG;
	case SSU_MSCBC_AP2_8M600_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSCBC_AP2_8M600_CONFIG);
		return (char *) SSU_MSCBC_AP2_8M600_CONFIG;
	case SSU_MSC_147_AP2_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_MSC_147_AP2_CONFIG);
		return (char *) SSU_MSC_147_AP2_CONFIG;
	case SSU_HLR_BS_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_HLR_BS_CONFIG);
		return (char *) SSU_HLR_BS_CONFIG;
	case SSU_HLR_TSC_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_HLR_TSC_CONFIG);
		return (char *) SSU_HLR_TSC_CONFIG;
	case SSU_BSC_400_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_BSC_400_CONFIG);
		return (char *) SSU_BSC_400_CONFIG;
	case SSU_BSP_MSC_BC_AP1_1.2_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_BSP_MSC_BC_AP1_1.2_CONFIG);
		return (char *) SSU_BSP_MSC_BC_AP1_1.2_CONFIG;
	case SSU_BSP_MSC_BC_AP2_1.2_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_BSP_MSC_BC_AP2_1.2_CONFIG);
		return (char *) SSU_BSP_MSC_BC_AP2_1.2_CONFIG;
	case SSU_HLR_SS_1200_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_HLR_SS_1200_CONFIG);
		return (char *) SSU_HLR_SS_1200_CONFIG_VALUE;
	case SSU_BSP_HLR_BS_1200_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_BSP_HLR_BS_1200_CONFIG);
		return (char *) SSU_BSP_HLR_BS_1200_CONFIG;
	case SSU_HLR_TSC_1200_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_HLR_TSC_1200_CONFIG);
		return (char *) SSU_HLR_TSC_1200_CONFIG;
		//Updated for vMSC-300 and vMSC-147
	case SSU_vMSC_300_AP1_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_vMSC_300_AP1_CONFIG);
		return (char *) SSU_vMSC_300_AP1_CONFIG;
	case SSU_vMSC_147_AP1_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_vMSC_147_AP1_CONFIG);
		return (char *) SSU_vMSC_147_AP1_CONFIG;
	case SSU_vIPSTP_147_AP1_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_vIPSTP_147_AP1_CONFIG);
		return (char *) SSU_vIPSTP_147_AP1_CONFIG;
	case SSU_vIPSTP_CONFIG_VALUE:
		DEBUG("User given Configuration is %s",SSU_IPSTP_CONFIG);
		return (char *) SSU_IPSTP_CONFIG;
	case SSU_vHLR_BS_250_CONFIG_VALUE:
    DEBUG("User given Configuration is %s",SSU_vHLR_BS_250_CONFIG_VALUE);
    return (char *) SSU_vHLR_BS_250_CONFIG;
        //Updated for WIRELINE
        case SSU_WLN_147_AP1_CONFIG_VALUE:
                DEBUG("User given Configuration is %s",SSU_WLN_147_AP1_CONFIG);
	default:
		DEBUG("%s","User given Configuration is NONE");
		return (char *) "NONE" ;
	}
}

