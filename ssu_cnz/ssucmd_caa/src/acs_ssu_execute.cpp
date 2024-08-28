//******************************************************************************
//
//  NAME
//     ACS_SSU_Execute.cpp
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
//	    190 89-CAA 109 0260
//
//  AUTHOR 
//     2005-04-01 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#include <ace/ACE.h>
#include <stdio.h>
#include "acs_ssu_execute.h"

using namespace std;

string mErrorChange ="";
string mErrorCurrentConfig = "";

//============================================================================
// Constructor
//============================================================================
ACS_SSU_Execute::ACS_SSU_Execute() : m_dwLastError(0)
{

}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_Execute::~ACS_SSU_Execute()
{
}


//============================================================================
// GetResultCodeText
// Returns the text from a result code
//============================================================================
ACE_TCHAR* ACS_SSU_Execute::GetResultCodeText(const ACE_INT32 dwResultCode,
		                                                ACE_INT32* pExitCode)
{
   static ACE_TCHAR szText[128] = ACE_TEXT("");
   ACE_INT32 nExitCode = -1;

   switch (dwResultCode)
   {
   case SSU_RC_OK:
	   ACE_OS::strcpy(szText, ACE_TEXT(""));
	   nExitCode = 0;
	   break;

   case SSU_RC_UNKNOWNCMDTYPE:
      ACE_OS::strcpy(szText, ACE_TEXT("Incorrect Usage"));
      nExitCode = 2;
      break;

   case SSU_RC_SERVERNOTRESPONDING:
	  ACE_OS::strcpy(szText, ACE_TEXT("Unable to connect to server"));
      nExitCode = 117;
      break;

   case SSU_RC_NOTACTIVENODE:
	  ACE_OS::strcpy(szText, ACE_TEXT("The command can not be executed on the passive node"));
      nExitCode = 5;
      break;

   case SSU_RC_ONAPG43ONLY:
	  ACE_OS::strcpy(szText, ACE_TEXT("Illegal command in this system configuration"));
      nExitCode = 115;//6;
      break;

   case SSU_RC_CONFIGTYPEALREADYEXIST:
	  ACE_OS::strcpy(szText, mErrorCurrentConfig.c_str());
      nExitCode = 7;
      break;

   case SSU_RC_NEWQUOTAEXCEEDED:
	  ACE_OS::strcpy(szText, ACE_TEXT( mErrorChange.c_str() ));
      nExitCode = 8;
      break;

   case SSU_RC_QUOTAFILEERROR:
	  ACE_OS::strcpy(szText, ACE_TEXT("SSU folder quotas file error"));
      nExitCode = 9;
      break;

   case SSU_RC_CONFIGFILEERROR:
	  ACE_OS::strcpy(szText, ACE_TEXT("SSU configuration file error"));
      nExitCode = 10;
      break;

   case SSU_RC_QUOTACHECKERROR:
	   ACE_OS::strcpy(szText, ACE_TEXT("Quotacheck binary is not existing"));
	   nExitCode = 11;
	   break;

   case SSU_RC_CONFIGCHANGEERROR:
	   ACE_OS::strcpy(szText, ACE_TEXT("Unreasonable value "));
   	   nExitCode = 3;
   	   break;

   case SSU_RC_OPERATIONINPROGRESS:
	  ACE_OS::strcpy(szText, ACE_TEXT("Operation already in progress"));
      nExitCode = 14;
      break;

   case SSU_RC_PERMISSIONSDENIED:
	  ACE_OS::strcpy(szText, ACE_TEXT("Permissions denied"));
      nExitCode = 15;
      break;

   case SSU_RC_ENDEVENTSIGNALED:
	  ACE_OS::strcpy(szText, ACE_TEXT("Operation cancelled by ACS_SSU_Server"));
      nExitCode = 21;
      break;

	case SSU_RC_ISNOTMULTCP:
	  ACE_OS::strcpy(szText, ACE_TEXT("Illegal value in this system configuration"));
      nExitCode = 12;
      break;

	case SSU_RC_SERVICEISBUSY:
		ACE_OS::strcpy(szText, ACE_TEXT("Service is busy"));
		nExitCode = 16;
		break;
   default:
	  ACE_OS::strcpy(szText, ACE_TEXT("Other error"));
   }

   if (pExitCode)
      *pExitCode = nExitCode;

   return szText;
}

//============================================================================
//============================================================================
// get_LastError
// Return the win32 error code of the last failed operation
//============================================================================
ACE_INT32 ACS_SSU_Execute::get_LastError() const
{
   return m_dwLastError;
}


