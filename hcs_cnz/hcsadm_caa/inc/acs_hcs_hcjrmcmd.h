//======================================================================
//
// NAME
//      HcjrmCmd.h
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
//      Implementation of the HC Job Removal of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by XCHVKUM
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_HcjrmCmd_h
#define AcsHcs_HcjrmCmd_h

#include "acs_hcs_healthcheckcmd.h"

using namespace std;

namespace AcsHcs
{
	/**
    *Command Handler for hcjrm inside the HC engine:  
	* stop ongoing or remove a scheduled health check job
	*/
	class HcjrmCmd : public HealthCheckCmd
	{
	public:
		HcjrmCmd();
		HcjrmCmd(char* cmdMsg, ACE_HANDLE pipe);
		~HcjrmCmd();

		/**
		* Execute this command.
		* @return 0 on success, -1 otherwise.
		*/
		int execute();
	};
}

#endif // AcsHcs_HcjrmCmd_h
