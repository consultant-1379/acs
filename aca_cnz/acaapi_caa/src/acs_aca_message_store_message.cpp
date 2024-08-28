// ******************************************************************************
//
//  NAME
//    ACS_ACA_MessageStoreMessage.C
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
//    This class represents an MTAP message as seen from the client's
//    perspective.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO       DATE         NAME      DESCRIPTION
//    A            2000-05-10   uabcajn   product release
//    B            2007-01-15   qnicmut   Blade Cluster adaptations
//******************************************************************************
#include <ctype.h>

#include <iostream>
#include <string>

#include "acs_aca_api_tracer.h"
#include "acs_aca_message_store_message.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_MessageStoreMessage)


// -----------------------------------------------------------------------------
ACS_ACA_MessageStoreMessage::ACS_ACA_MessageStoreMessage()
	: messageIdM(0), stateM(Empty), dataLengthM(0), dataM(0), allocatedMemorySizeM(0), allocatedMemoryMd(0)
{
	ACS_ACA_TRACE_FUNCTION;
}

// -----------------------------------------------------------------------------
// Copy constructor.
ACS_ACA_MessageStoreMessage::ACS_ACA_MessageStoreMessage(const ACS_ACA_MessageStoreMessage & anotherMSM)
{
	ACS_ACA_TRACE_FUNCTION;
	messageIdM = anotherMSM.messageIdM;
	stateM = anotherMSM.stateM;
    
	if ((dataLengthM = anotherMSM.dataLengthM) > 0) {
		// Must allocate space and copy data.
		ACS_ACA_TRACE_MESSAGE("Allocating Space and Copying Data");
		allocatedMemorySizeM = dataLengthM;
		allocatedMemoryMd = new (std::nothrow) unsigned char [allocatedMemorySizeM];
		memcpy(allocatedMemoryMd, anotherMSM.dataM, allocatedMemorySizeM);
		dataM = allocatedMemoryMd;

	}
	else {
		allocatedMemorySizeM = 0;
		allocatedMemoryMd = 0;
		dataM = 0;
	}
}

// -----------------------------------------------------------------------------
ACS_ACA_MessageStoreMessage::~ACS_ACA_MessageStoreMessage()
{
	ACS_ACA_TRACE_FUNCTION;
	if (allocatedMemorySizeM > 0) {
		delete[] allocatedMemoryMd;
		allocatedMemoryMd = 0;
	}
}

// -----------------------------------------------------------------------------
ULONGLONG ACS_ACA_MessageStoreMessage::getMessageId() const
{
	ACS_ACA_TRACE_FUNCTION;
	return messageIdM;
}

// -----------------------------------------------------------------------------
ACS_ACA_MessageStoreMessage::State ACS_ACA_MessageStoreMessage::getState() const
{
	ACS_ACA_TRACE_FUNCTION;
	return stateM;
}

// -----------------------------------------------------------------------------
int ACS_ACA_MessageStoreMessage::getDataLength() const
{
	ACS_ACA_TRACE_FUNCTION;
	return dataLengthM;
}

// -----------------------------------------------------------------------------
const unsigned char * ACS_ACA_MessageStoreMessage::getData() const
{
	ACS_ACA_TRACE_FUNCTION;
	return dataM;
}

// -----------------------------------------------------------------------------
int ACS_ACA_MessageStoreMessage::clear()
{
	ACS_ACA_TRACE_FUNCTION;
	int returnValue = (stateM != Empty);
    
	messageIdM = 0;
	stateM = Empty;
	dataLengthM = 0;
	dataM = 0;

	return returnValue;
}

// -----------------------------------------------------------------------------
void ACS_ACA_MessageStoreMessage::setData(
			ULONGLONG														messageId,
			const unsigned char *								data,
			int																	dataLength,
			ACS_ACA_MessageStoreMessage::State	newState)
{
	ACS_ACA_TRACE_FUNCTION;

	// Assign a new message number.
	messageIdM = messageId;
	stateM = newState;

	if ((dataLengthM = dataLength) > allocatedMemorySizeM) {
		// Must allocate more space.
		delete[] allocatedMemoryMd;

		allocatedMemoryMd = new (std::nothrow) unsigned char [(allocatedMemorySizeM = dataLengthM)];
		if ( !allocatedMemoryMd ) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Failed to allocate memory, returning");
			return;
		}
		ACS_ACA_TRACE_MESSAGE("Allocating Space of %d", dataLengthM);
	}

	if (dataLengthM > 0) {
		if (allocatedMemoryMd) {
			memcpy(allocatedMemoryMd, data, dataLengthM);
			dataM = allocatedMemoryMd;
		}
	}
}
