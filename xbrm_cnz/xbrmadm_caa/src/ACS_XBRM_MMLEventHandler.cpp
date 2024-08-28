/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_MMLEventHandler.cpp
 *
 * @brief
 * ACS_XBRM_MMLEventHandler Class for MML session event handler
 *
 * @details
 * Implementation of ACS_XBRM_MMLEventHandler class to handle events for MML commands
 *
 * @author XHARBAV
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-07-11  XHARBAV  Created First Revision
 *
 ****************************************************************************/
#include "ACS_XBRM_MMLEventHandler.h"
#include <ACS_APGCC_InitFile.H>
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "mcs_cc_util.h"
#include <string>

using namespace std;
ACS_XBRM_TRACE_DEFINE(ACS_XBRM_MMLEventHandler);

ACS_XBRM_MMLEventHandler::ACS_XBRM_MMLEventHandler(ACS_XBRM_MMLMessageHandler *messageHandler):
MCS_MML_Event_Handler(),
m_messageHandler(messageHandler)
{
  // Empty
}


//-------------------------------------------------------
// The CP is ready to receive a command
//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_read(MCS_MML_READORDER readOrder)
{   
   return this->sendSimpleMessage(MML_PROMPT, readOrder);
}


//-------------------------------------------------------
// CP reports a new printout is available
//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_printout() {
   return this->sendSimpleMessage(MML_PRINTOUT, 0);
}

//-------------------------------------------------------
// Reception of event such as, for example, CP restart
//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_event(MCS_MML_EVENTTYPE event) {
   return this->sendSimpleMessage(MML_EVENT, event);
}

//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::RelDeviceData const & relDeviceData) {
   return this->sendEvent(event, (char*) &relDeviceData, sizeof(relDeviceData));
}

//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::LockEventData const & lockEventData) {
   return this->sendEvent(event, (char*) &lockEventData, sizeof(lockEventData));
}

//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::CloseData const & closeData){
   return this->sendEvent(event, (char*) &closeData, sizeof(closeData));
}

//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::AvailabilityEventData const & availabilityEventData){
   return this->sendEvent(event, (char*) &availabilityEventData, sizeof(availabilityEventData));
}


// MML session forced closed by the MML API
//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::handle_close() {
		// Send ME_SHUTDOWN to messageHandler
    this->sendSimpleMessage(MML_EVENT, ME_SHUTDOWN);
    return 0;
}



//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::sendSimpleMessage(const int message, char data){
   ACE_Message_Block* mb;

   // If new fails, return with -1
   ACE_NEW_RETURN( mb, ACE_Message_Block(1), -1 );

   mb->wr_ptr()[0] = data;
   mb->wr_ptr(1);
   mb->msg_type(message);

   if ( m_messageHandler->putq(mb) < 0 ) {
      // Failed to forward event
      (void) mb->release();
      return -1;
   }
   return 0;
}

//-------------------------------------------------------
int ACS_XBRM_MMLEventHandler::sendEvent(MCS_MML_EVENTTYPE const /*event*/, char* data, long unsigned int sizeOfData){
   ACE_Message_Block* mb;

   // If new fails, return with -1
   ACE_NEW_RETURN( mb, ACE_Message_Block(sizeOfData), -1 );

   mb->copy(data, sizeOfData );
   mb->msg_type( MML_EVENT );

   if ( m_messageHandler->putq(mb) < 0 ) {
      // Failed to forward event
      (void) mb->release();
      return -1;
   }
   return 0;
}
