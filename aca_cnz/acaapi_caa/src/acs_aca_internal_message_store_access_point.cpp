// ******************************************************************************
//
//  NAME
//    ACS_ACA_InternalMessageStoreAccessPoint.C
//
//  COPYRIGHT
//    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//    All rights reserved.
//
//    The Copyright to the computer program(s) herein 
//    is the property of Ericsson Utvecklings AB, Sweden.
//    The program(s) may be used and/or copied only with 
//    the written permission from Ericsson Utvecklings AB or in 
//    accordance with the terms and conditions stipulated in the 
//    agreement/contract under which the program(s) have been 
//    supplied.

//  DOCUMENT NO
//    CAA 109 0313

//  AUTHOR 
//    UAB/I/GD Carl Johannesson

//  REVISION
//    A
//    2000-05-10

//  DESCRIPTION
//    This class is the real implementation of ACS_ACA_MessageStoreAccessPoint, 
//	  and has the same public interface.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO       DATE         NAME      DESCRIPTION
//    A            2000-05-10   uabcajn   product release
//    B            2007-01-15   qnicmut   Blade Cluster adaptations
//******************************************************************************
#include <functional>

#include "acs_aca_message_store_access_point.h"
#include "acs_aca_message_store_message.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_internal_message_store_access_point.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_InternalMessageStoreAccessPoint)

// -----------------------------------------------------------------------------
ACS_ACA_InternalMessageStoreAccessPoint::ACS_ACA_InternalMessageStoreAccessPoint()
: stateM(ACS_ACA_MessageStoreAccessPoint::Disconnected),
  errorCodeM(ACS_ACA_MessageStoreAccessPoint::Undefined), anyMessageRead(false) {
	ACS_ACA_TRACE_FUNCTION;
}

// -----------------------------------------------------------------------------
ACS_ACA_InternalMessageStoreAccessPoint::~ACS_ACA_InternalMessageStoreAccessPoint() {
	ACS_ACA_TRACE_FUNCTION;
}

// -----------------------------------------------------------------------------
ACS_ACA_MessageStoreAccessPoint::State ACS_ACA_InternalMessageStoreAccessPoint::getState() const {
	ACS_ACA_TRACE_FUNCTION;
	return stateM;
}

// -----------------------------------------------------------------------------
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACS_ACA_InternalMessageStoreAccessPoint::getLastError() const {
	ACS_ACA_TRACE_FUNCTION;
	return errorCodeM;
}

// -----------------------------------------------------------------------------
void ACS_ACA_InternalMessageStoreAccessPoint::clearLastError() {
	ACS_ACA_TRACE_FUNCTION;
	errorCodeM = ACS_ACA_MessageStoreAccessPoint::Undefined;
	ACS_ACA_TRACE_MESSAGE("Setting errorCodeM to 'Undefined'");
}

// -----------------------------------------------------------------------------
void ACS_ACA_InternalMessageStoreAccessPoint::setMessageData(
		ACS_ACA_MessageStoreMessage & message,
		ULONGLONG msgNumber,
		unsigned char * msgData,
		int msgLength,
		ACS_ACA_MessageStoreMessage::State	state)
{
	ACS_ACA_TRACE_FUNCTION;
	if (state == ACS_ACA_MessageStoreMessage::Empty) {
		ACS_ACA_TRACE_MESSAGE("MessageStore Empty");
		message.clear();
	}
	else {
		message.setData(msgNumber, msgData, msgLength, state);
		ACS_ACA_TRACE_DUMP(msgData, msgLength, ACS_ACA_TRACE_DEFAULT_DUMP_SIZE);
	}
}
