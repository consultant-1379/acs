//******************************************************************************
//
// NAME
//      acs_prc_brfImplementerThread.h
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// DOCUMENT NO
//      19089-CAA 109 0520

// AUTHOR
//      2010-12-29 by XLUCPET

// REVISION
//  -
//

// CHANGES

//
//      REV NO          DATE            NAME            DESCRIPTION
//                      2014-04-23      XSARSES         Fix for HS53134

// SEE ALSO
//
//
//******************************************************************************

#ifndef ACS_PRC_BRFIMPLEMENTERTHREAD_H_
#define ACS_PRC_BRFIMPLEMENTERTHREAD_H_

#include "ace/Task.h"
#include <sys/poll.h>
#include <stdio.h>
#include <iostream>
#include "acs_prc_brfImplementer.h"
#include "acs_apgcc_oihandler_V3.h"
#include "ACS_TRA_Logging.h"
#include "acs_apgcc_omhandler.h"


class acs_prc_brfImplementerThread : public ACE_Task_Base{

public:
	acs_prc_brfImplementerThread ( std::string local_node_id, std::string local_node_name );//HV92897
	virtual ~acs_prc_brfImplementerThread();

	virtual int svc ( void );

	void stop(){ sleep = true; };

	void start(){ sleep = false; };

private:

	int createParticipant ( std::string local_node_id, std::string local_node_name );//HV92897
	int removePartecipant_brf ( std::string local_node_id, std::string local_node_name );//HV92897

    acs_apgcc_oihandler_V3 oiHandler;
    acs_prc_brfImplementer BRFImplementer;
    ACS_TRA_Logging Logging;

	bool sleep;
	std::string p_local_node_id;
        std::string p_local_node_name;//HV92897

};

#endif /* ACS_PRC_BRFIMPLEMENTERTHREAD_H_ */
