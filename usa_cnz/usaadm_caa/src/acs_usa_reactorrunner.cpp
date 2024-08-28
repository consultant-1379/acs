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
 * @file mcs_adh_reactorRunner.cpp
 *
 * @brief
 * Implements the reactorRunner
 *
 * @details
 * When a reactor is stopped it need to be reset in order to be able to
 * be started again. This is handled by this class.
 * When a reactor is restarted from another thread it need to be assigned
 * this no thread as its owner. This is handled by this class.
 *
 * @author 
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 *
 ****************************************************************************/
#include <ace/Reactor.h>
#include <syslog.h>
#include "acs_usa_tratrace.h"
#include "acs_usa_reactorrunner.h"

ACS_USA_Trace traReactR ("ACS_USA_ReactorRunner  ");
//                      "MCS_ADH_XxxxXxxxXxxxXxx"


//----------------------------------------------------------------------------
ACS_USA_ReactorRunner::ACS_USA_ReactorRunner(ACE_Reactor* reactor, const string& name):
   m_reactor(reactor),
   m_name(name)
{
   if (0 == m_reactor) {
      traReactR.traceAndLogFmt(ERROR, "%s() No reactor provided", __func__);
   }
   syslog(LOG_INFO, "ACS_USA_ReactorRunner invoked");
}



//----------------------------------------------------------------------------
int ACS_USA_ReactorRunner::open() {

  syslog(LOG_INFO, "ACS_USA_ReactorRunner::open invoked");	
   return this->activate( THR_JOINABLE | THR_NEW_LWP );
}

//----------------------------------------------------------------------------
int ACS_USA_ReactorRunner::svc() {

   syslog(LOG_INFO, "ACS_USA_ReactorRunner:svc() invoked");
   if (0 == m_reactor) {
      traReactR.traceAndLogFmt(ERROR, "%s() No reactor defined", __func__);
      return -1;
   }

   if (m_reactor->reactor_event_loop_done() != 0) {
      traReactR.traceAndLogFmt(WARNING, "%s() '%s' REACTOR is already running!!!", __func__, m_name.c_str());
   }
   usa_trace(traReactR, "%s() starting '%s' REACTOR ", __func__, m_name.c_str());
   syslog(LOG_INFO, "%s() starting '%s' REACTOR ", __func__, m_name.c_str());

   m_reactor->owner(ACE_OS::thr_self()); // Must change owner otherwise reactor cannot be restarted
   m_reactor->run_reactor_event_loop(); // Will hang until someone ends the loop

   usa_trace(traReactR, "%s() '%s' REACTOR is stopped", __func__, m_name.c_str());
   syslog(LOG_INFO, "%s() '%s' REACTOR is stopped", __func__, m_name.c_str());
   m_reactor->reset_reactor_event_loop(); 
   return 0;
}

//----------------------------------------------------------------------------
void ACS_USA_ReactorRunner::stop() {
   if (0 == m_reactor) {
      traReactR.traceAndLogFmt(ERROR, "%s() No reactor defined", __func__);
      return;
   }
   usa_trace(traReactR, "%s() stopping '%s' REACTOR", __func__, m_name.c_str());
   m_reactor->end_reactor_event_loop();
}

