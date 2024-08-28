//======================================================================
//
// NAME
//      HealthCheckCmdFactory.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-04-30 by EGINSAN
// CHANGES
//     
//======================================================================

//#include <windows.h>
#include <iostream>
#include <algorithm>

#include "acs_hcs_healthcheckcmdfactory.h"
#include "acs_hcs_healthcheckcmd.h"
#include "acs_hcs_hcrflscmd.h"
#include "acs_hcs_hcrslscmd.h"
#include "acs_hcs_hcjlscmd.h"
#include "acs_hcs_hcjdefcmd.h"
#include "acs_hcs_hcjrmcmd.h"
#include "acs_hcs_tracer.h"

using namespace std;

//=======================================================================================

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//=======================================================================================

	HealthCheckCmd* HealthCheckCmdFactory::Create(ACE_HANDLE hPipe) 
	{
		//ACS_HCS_TRACE("HealthCheckCmdFactory::Create Entered");

		const int BUFSIZE = 512;
		char pchRequest[BUFSIZE];
		ACE_UINT32 cbBytesRead = 0; 
		bool fSuccess = false /*FALSE*/;

		// Read client requests from the pipe. 
		/* fSuccess = ReadFile( 
			hPipe,                  // handle to pipe 
			(void*)pchRequest,    // buffer to receive data 
			BUFSIZE,              // size of buffer 
			&cbBytesRead,         // number of bytes read 
			NULL);       

		if (!fSuccess || cbBytesRead == 0)
		{   
			if ( ACE_OS::last_error () == ERROR_BROKEN_PIPE)
			{
				ACS_HCS_TRACE("Client disconnected. Error Code: " <<  ACE_OS::last_error () << ".");
				return NULL;
			}
			else
			{
				ACS_HCS_TRACE("ReadFile failed. Error Code: " <<  ACE_OS::last_error () << ".");
				return NULL;
			}
		}*/

		string mesgRcvd = (string)pchRequest;
		size_t found;
		found = mesgRcvd.find_first_of(" ");

		string commandName = mesgRcvd.substr(0,found);

		transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);


		cout <<"The string to be compared is:"<<commandName<<endl;

		if ( strcmp(commandName.c_str(),"hcrfls") == 0)
		{	
			return new HcrflsCmd(pchRequest, hPipe);
		}
		else if (strcmp(commandName.c_str(),"hcrsls") == 0)
		{
			return new HcrslsCmd(pchRequest, hPipe);
		}
		else if (strcmp(commandName.c_str(),"hcjls") == 0)
		{
			return new HcjlsCmd(pchRequest, hPipe);
		}
		else if (strcmp(commandName.c_str(),"hcjdef") == 0)
		{
			return new HcjdefCmd(pchRequest, hPipe);
		}
		else if (strcmp(commandName.c_str(),"hcjrm") == 0)
		{
			return new HcjrmCmd(pchRequest, hPipe);
		}

		return NULL;
	}
	//=======================================================================================
}
//=======================================================================================

