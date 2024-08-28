//======================================================================
//
// NAME
//      Synchron.cpp
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
//      2011-06-14 by EEDSTL
// CHANGES
//     
//======================================================================

#include "acs_hcs_synchron.h"

using namespace std;

namespace AcsHcs
{
	/**
	* Static Varibles initialization for registering the threads.
	*/	

	map<ACE_hthread_t, bool>    Synchron::ThrExitHandler::theThrStatusMap;
	ACE_Recursive_Thread_Mutex  Synchron::ThrExitHandler::theThrMutex;
}