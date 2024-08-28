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

#ifndef ACS_SSU_EXECUTE_H
#define ACS_SSU_EXECUTE_H
#include <iostream>
#include <string>
#include "acs_ssu_types.h"
using std::string;
//#include "ACS_SSU_Param.h"

extern string mErrorChange, mErrorCurrentConfig;


class ACS_SSU_Execute
{
public:
   ACS_SSU_Execute();
   ~ACS_SSU_Execute();

public:

   const ACE_TCHAR* get_Data() const;
   ACE_INT32  get_LastError() const;


public:
   ACE_TCHAR* GetResultCodeText(const ACE_INT32 dwResultCode,
                                          ACE_INT32* pExitCode = NULL);

private:
   ACE_INT32        m_dwLastError;
};





#endif
