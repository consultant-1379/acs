//******************************************************************************
//
//  NAME
//     ACS_EMF_Execute.cpp
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

#ifndef ACS_EMF_EXECUTE_H
#define ACS_EMF_EXECUTE_H

#include <acs_emf_defs.h>
//#include "acs_emf_param.h"

class ACS_EMF_Execute
{
public:
   ACS_EMF_Execute( );
   ~ACS_EMF_Execute( );

public:
   //const ACE_TCHAR* get_Data() const;
   //const ACE_INT32   get_LastError() const;

public:
   static const ACE_TCHAR* GetResultCodeText(const ACE_INT32 dwResultCode,
											   ACE_INT32* pExitCode = NULL);

   static const ACE_TCHAR* GetErrorText(const ACE_INT32 errorCode);

private:
   ACE_INT32        m_dwLastError;

};

#endif //ACS_EMF_EXECUTE_H
