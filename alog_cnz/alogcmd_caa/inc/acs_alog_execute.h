//****************************************************************************
//
//  NAME
//     acs_alog_execute.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2004. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  PR           DATE      INITIALS    DESCRIPTION
//  -----------------------------------------------------------
//  N/A       26/07/2012     xbhakat       Initial Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************

#ifndef ACS_ALOG_EXECUTE_H
#define ACS_ALOG_EXECUTE_H

#include <acs_alog_defs.h>
#include <ace/ACE.h>


class acs_alog_execute
{
public:
	acs_alog_execute();
   ~acs_alog_execute();

public:
   enumCmdExecuteResult Execute(const enumCmdType           CmdType,
                                      const acs_alog_dataUnitType DataUnitType,
                                       const ACE_TCHAR *                lpszData1 = (""),
                                      const ACE_TCHAR *               lpszData2 = (""));

private:
   CmdData m_srctCmdData;

};
#endif
