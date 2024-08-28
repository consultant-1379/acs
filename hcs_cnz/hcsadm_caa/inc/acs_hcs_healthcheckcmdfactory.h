//======================================================================
//
// NAME
//      HealthCheckCmdFactory.h
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
//      Healt Check Command Factory for HCS administration commands.
//      Based on 190 89-CAA xxxxx.
//
// DOCUMENT NO
//      190 89-CAA xxxxx
//
// AUTHOR
//      2011-04-30 by EGINSAN
// CHANGES
//     
//======================================================================


#ifndef HealthCheckCmdFactory_h
#define HealthCheckCmdFactory_h

#include "acs_hcs_healthcheckcmd.h"
#include <iostream>
#include <string>
using namespace std;

namespace AcsHcs {

// Health Check Command Factory Class
class HealthCheckCmdFactory {

	public:
       // Static class to create HCCmd objects
       // Change is required only in this function to create a new object type
       static HealthCheckCmd* Create(ACE_HANDLE hPipe);

};

}

#endif
