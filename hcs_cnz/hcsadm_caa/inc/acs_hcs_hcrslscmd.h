//======================================================================
//
// NAME
//      HcrslsCmd.h
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
//      Implementation of the HC Rule set file listing.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by XCHVKUM
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_HcrslsCmd_h
#define AcsHcs_HcrslsCmd_h

#include "acs_hcs_healthcheckcmd.h"

using namespace std;

namespace AcsHcs
{
	/**
	  *Command Handler for hcrsls inside the HC engine:  
	  *list or view health check rule sets
	  */
	class HcrslsCmd : public HealthCheckCmd
	{
	public:
		HcrslsCmd(char* cmdMsg, ACE_HANDLE pipe);
		~HcrslsCmd();

		/**
		* Execute this command.
		* @return 0 on success, -1 otherwise.
		*/
		int execute();
	};
}

#endif // AcsHcs_HcrslsCmd_h
