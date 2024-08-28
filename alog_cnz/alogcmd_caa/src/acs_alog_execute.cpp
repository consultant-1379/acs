//****************************************************************************
//
//  NAME
//     acs_alog_execute.cpp
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
//  N/A       07/08/2012     xbhakat        Final Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************



#include <stdio.h>
#include <acs_alog_execute.h>
#include <string.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include "iostream"
//#include "fstream"


using namespace std;
const char pipeName[] = "/var/run/ap/ACS_ALOG_MainCmdPipe";


acs_alog_execute::acs_alog_execute() {

}
acs_alog_execute::~acs_alog_execute() {

}

enumCmdExecuteResult acs_alog_execute::Execute(const enumCmdType CmdType,
		const acs_alog_dataUnitType DataUnitType, const ACE_TCHAR * lpszData1,
		const ACE_TCHAR * lpszData2) {
	m_srctCmdData.CmdType = CmdType;

	if ( lpszData1 != NULL )
		strcpy(m_srctCmdData.Data1, lpszData1);

	if ( lpszData2 != NULL )
		strcpy(m_srctCmdData.Data2, lpszData2);

	m_srctCmdData.DataUnitType = DataUnitType;

	char buffer[1024];
	ACE_LSOCK_Stream Stream;
	ACE_LSOCK_Connector Connector;
	ACE_UNIX_Addr addr(pipeName);
	//	size_t byte_read = 0;

	if (Connector.connect(Stream, addr) == -1) {
		return cerServerNotResponding;

	}

	void *dest;
	dest = malloc(sizeof(m_srctCmdData));

	CmdData *m_srctCmdDataPtr;
	m_srctCmdDataPtr = &m_srctCmdData;

	memcpy(dest, m_srctCmdDataPtr, sizeof(m_srctCmdData));

	if (Stream.send_n(dest, sizeof(m_srctCmdData)) == -1) {
		cout << "Unable to connect to server" << endl;
		Stream.close();
		return cerOtherError;
	}

	/*int Error = 1;
	  int byte_sent = 0;*/
	Stream.recv (buffer, 1024);
	enumCmdExecuteResult ExecResult;
	ExecResult = *(reinterpret_cast<enumCmdExecuteResult*>(buffer));

	Stream.close();
	return ExecResult;

}
