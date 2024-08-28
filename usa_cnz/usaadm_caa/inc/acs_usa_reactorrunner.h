/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2012
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file acs_usa_reactorRunner.h
 *
 * @brief
 * Users an ACE_Task to run a reactor.
 *
 * @details
 * Create an instance of this class and assign it a reactor instance.
 * Call open() to start the reactor.
 * Call stop() to stop the reactor.
 *
 * @author XTBAKLU
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 *
 ****************************************************************************/

#ifndef ACS_USA_REACTORRUNNER_H_
#define ACS_USA_REACTORRUNNER_H_

#include <string>
#include <ace/Task.h>

class ACE_Reactor;

//----------------------------------------------------------------------------
class ACS_USA_ReactorRunner: public ACE_Task<ACE_SYNCH> {
	
public:
	
   ACS_USA_ReactorRunner(ACE_Reactor* reactor, const std::string& name);
   int open();
   int svc();
   void stop();
   
private:
   
   ACE_Reactor* m_reactor;
   std::string m_name;
};

#endif /* ACS_USA_REACTORRUNNER_H_ */

