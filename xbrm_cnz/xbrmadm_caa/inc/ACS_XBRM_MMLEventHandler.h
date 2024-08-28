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

#ifndef ACS_XBRM_MMLEVENTHANDLER_H
#define ACS_XBRM_MMLEVENTHANDLER_H

#include <ace/Task.h>
#include "MCS_MML_Defs.h"
#include "mcs_mmlapi_eventHandler.h"
#include "ACS_XBRM_MMLMessageHandler.h"

class ACS_XBRM_MMLMessageHandler;
typedef enum {
   MML_PROMPT = 900,
   MML_PRINTOUT = 901,
   MML_EVENT = 902,
   MML_CLOSE = 1000,
   MML_SESSIONREADY = 1001,
   MML_STARTTEST = 1002,
   MML_EXIT = 1003
} MML_Messages;

class ACS_XBRM_MMLEventHandler: public MCS_MML_Event_Handler{

public:

	ACS_XBRM_MMLEventHandler(ACS_XBRM_MMLMessageHandler *messageHandler);
	// MML API Callbacks
	//-------------------

	// Reception of CP read order
	virtual int handle_read(MCS_MML_READORDER readOrder);

	// Indication of new printout data available
	virtual int handle_printout();

	// Reception of event such as, for example, CP restart
	virtual int handle_event(MCS_MML_EVENTTYPE event);
	virtual int handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::RelDeviceData const & relDeviceData);
	virtual int handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::LockEventData const & lockEventData);
	virtual int handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::CloseData const & closeData);
	virtual int handle_event(MCS_MML_EVENTTYPE const event, MCS_MML_Protocol::AvailabilityEventData const & availabilityEventData);

	// MML session forced closed by the MML API
	virtual int handle_close();

private:
   
ACS_XBRM_MMLMessageHandler *m_messageHandler;
	int sendSimpleMessage(const int message, char data);
	int sendEvent(MCS_MML_EVENTTYPE const event, char* data, long unsigned int sizeOfData);
};

#endif /*ACS_XBRM_MMLEVENTHANDLER_H*/
