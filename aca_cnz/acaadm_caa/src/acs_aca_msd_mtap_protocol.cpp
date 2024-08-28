//****************************************************************
//
//  NAME
//    ACAMSD_MTAP_Protocol.C
//
//  COPYRIGHT
//	  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
//    CAA 109 0312

//  AUTHOR 
//    EAB/UZ/DG Carl Johannesson


//  DESCRIPTION
//    This class contains functions for sending and receiving MTAP
//    protocol primitives.
//    The class is pure static. 

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO     DATE        NAME       DESCRIPTION
//    A          2000-05-10  uabcajn    product release
//    B          2004-03-12  uabcajn    rogue wave replaced by STL.
//****************************************************************
#include "ace/OS_NS_poll.h"
#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Event_Handler.h"
#include "ace/Handle_Set.h"

#include "acs_aca_defs.h"
#include "acs_aca_logger.h"
#include "acs_aca_msd_mtap_protocol.h"

//****************************************************************
// Read the whole buffer from DSDLIB and return the MTAP-primitive.
//****************************************************************
ACAMSD_MTAP_Protocol::Primitive ACAMSD_MTAP_Protocol::getPrimitive (
		ACS_DSD_Session & dsdptr,
		unsigned long & primitiveSize,
		unsigned char * buffer) {

	const int BUFFER_SIZE = ACS_DSD_MAXBUFFER;
	unsigned char tempBuf[BUFFER_SIZE] = {0};
	int readResult = 0;

	ACAMSD_MTAP_Protocol::receive(dsdptr, reinterpret_cast<char *> (tempBuf), sizeof(tempBuf), &readResult);

	if ((readResult <= 0) || (readResult > BUFFER_SIZE)) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Error while receiving a primitive from the remote peer, return_code = %d, error_message: '%s'",
				readResult, dsdptr.last_error_text());

		ACS_DSD_Node remote_node;
		if (dsdptr.get_remote_node(remote_node)) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'get_remote_node' failed, returning None");
			return None;
		}

		if (readResult == acs_dsd::ERR_PEER_CLOSED_CONNECTION) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "The remote peer (system_id = %u) closed the connection!", remote_node.system_id);
			return PeerClosedConnection;
		}

		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while receiving a primitive from the remote peer (system_id = %u), returning None", remote_node.system_id);
		return None;
	}

	ACE_OS::memcpy(buffer, tempBuf, primitiveSize = readResult);
	return static_cast<Primitive>(buffer[0]);
}

//****************************************************************
// Build a response to the MTAP 'Echo-check' primitive in buffer 
// and send it.
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendEchoCheckResponse (ACS_DSD_Session & dsdptr, const unsigned char * buffer) {

	unsigned char responseBuf[4] = {0};
	unsigned char * tmpBuf = responseBuf;

	// Copy the contents of the 'Echo-check' primitive to the buffer
	ACE_OS::memcpy(tmpBuf, buffer, 4);

	return ACAMSD_MTAP_Protocol::send(dsdptr, responseBuf, 4) == 0;
}

//****************************************************************
// Build an 'Error' primitive and send it.
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendError (ACS_DSD_Session & dsdptr, ErrorCode errorCode) {

	unsigned char sendBuf[8] = {0};
	unsigned char * tmpBuf = sendBuf;

	// Primitive indicator
	*tmpBuf++ = Error;

	// And three zero bytes (can't use memcpy)
	*tmpBuf++ = 0;
	*tmpBuf++ = 0;
	*tmpBuf++ = 0;

	// Error code
	*tmpBuf = errorCode;
	return ACAMSD_MTAP_Protocol::send(dsdptr, sendBuf, 5) == 0;
}

//****************************************************************
// Unpack the MTAP 'Get-parameter' primitive. The list of para-
// meters should be empty when this method is called, and the 
// parameters will be added to it.
// MTAP indicates parameters of interest with this primitive.
//****************************************************************
ACAMSD_MTAP_Protocol::ErrorCode ACAMSD_MTAP_Protocol::unpackGetParameter (
		const unsigned char * buffer,
		unsigned long size,
		std::list<unsigned char> & parameters) {

	unsigned char numberOfParameters = buffer[4];
	unsigned short value = 5;

	if (size != (value + numberOfParameters)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The value of size is different from the expected one, returning UnspecifiedError");
		return UnspecifiedError;
	}
   
	for (int i = 0; i < numberOfParameters; ++i) parameters.push_back(buffer[5 + i]);

	return NoError;
}

//****************************************************************
// Build a response to the MTAP 'Get_parameter' primitive in 
// buffer and send it.
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendGetParameterResponse (
		ACS_DSD_Session & dsdptr,
		const unsigned char * buffer,
		std::list<unsigned char> & parameters,
		std::list<unsigned long> & parameterValues,
		ErrorCode result) {

	unsigned char responseBuf[64] = {0};
	unsigned char * tmpBuf = responseBuf;

	int numberOfParameters = static_cast<int>(parameters.size());

	// Copy the contents of the 'Get-parameter' primitive to the buffer
	ACE_OS::memcpy(tmpBuf, buffer, 4);

	// Error code
	responseBuf[4] = result;

	// Number of parameters
	responseBuf[5] = static_cast<unsigned char>(numberOfParameters);
	tmpBuf = &responseBuf[6];

	// For each parameter, the parameter code and value
	for (int i = 0; i < numberOfParameters; ++i) {
		*tmpBuf++ = parameters.front();
		parameters.pop_front();
		put4bytes(tmpBuf, parameterValues.front());
		parameterValues.pop_front();
	}
	return ACAMSD_MTAP_Protocol::send(dsdptr, responseBuf, (6 + 5 * numberOfParameters)) == 0;
}

//****************************************************************
// Unpack the MTAP 'Option-negotiation' primitive.
//****************************************************************
ACAMSD_MTAP_Protocol::ErrorCode ACAMSD_MTAP_Protocol::unpackOptionNegotiation (
		const unsigned char * buffer,
		unsigned long size,
		Option & option,
		unsigned char & optionValue) {

	if (size != 5) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The value of size is different from the expected one, returning UnspecifiedError");
		return UnspecifiedError;
	}

	option = static_cast<Option>(buffer[1]);
	optionValue = buffer[4];

	return NoError;
}

//****************************************************************
// Build a response to the MTAP 'Option-negotiation' primitive in 
// buffer and send it. Can only handle 1-byte option data.
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendOptionNegotiationResponse (
		ACS_DSD_Session & dsdptr,
		const unsigned char * buffer,
		unsigned char optionValue,
		ErrorCode result) {
	unsigned char responseBuf[8] = {0};
	unsigned char * tmpBuf = responseBuf;

	// Will/Will not response
	*tmpBuf++ = (result == NoError ? Do_Will : DoNot_WillNot);


	// Copy the next 3 bytes (Option code and MTAP individual) from the request.
	*tmpBuf++ = buffer[1];
	*tmpBuf++ = buffer[2];
	*tmpBuf++ = buffer[3];

	// Option data
	*tmpBuf = optionValue;

	return ACAMSD_MTAP_Protocol::send(dsdptr, responseBuf, 5) == 0;
}

//****************************************************************
// Build a 'Stop' primitive and send it.
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendStop (ACS_DSD_Session & dsdptr) {

	unsigned char sendBuf[4] = {0};
	unsigned char * tmpBuf = sendBuf;

	// Primitive indicator
	*tmpBuf++ = Stop;

	// And three zero bytes (can't use memcpy)
	*tmpBuf++ = 0;
	*tmpBuf++ = 0;
	*tmpBuf = 0;

	return ACAMSD_MTAP_Protocol::send(dsdptr, sendBuf, 4) == 0;
}

//****************************************************************
// Unpack the MTAP 'Synchronize' primitive.
//****************************************************************
ACAMSD_MTAP_Protocol::ErrorCode ACAMSD_MTAP_Protocol::unpackSynchronize (
		const unsigned char * buffer,
		unsigned long size,
		unsigned char & fileNumber,
		unsigned long & recordNumber) {
	if (size != 8) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The value of size is different from the expected one, returning UnspecifiedError");
		return UnspecifiedError;
	}

	fileNumber = buffer[1];
	recordNumber = get4bytes(&buffer[4]);

	return NoError;
}

//****************************************************************
// Build a response to the MTAP 'Synchronize' primitive in buffer 
// and send it.
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendSynchronizeResponse (
		ACS_DSD_Session & dsdptr,
		const unsigned char * buffer,
		std::list<unsigned long> & missingList,
		ErrorCode result) {
	unsigned char responseBuf[140] = {0};
	int numberOfRecordsLeft = static_cast<int>(missingList.size());
	unsigned char * tmpBuf = responseBuf;
	unsigned long elementLength = 7 + (4 * numberOfRecordsLeft);
 
	// Copy the contents of the synchronize primitive to the buffer
	ACE_OS::memcpy(tmpBuf, buffer, 4);
	tmpBuf += 4;

	// Error code
	responseBuf[4] = result;
	tmpBuf += 1;

	// Continued (set to one if there will be more responses)
	*(tmpBuf++) = 0;

	// number of records
	*(tmpBuf++) = numberOfRecordsLeft;

	// And finally the record numbers
	while (numberOfRecordsLeft--) {
		put4bytes(tmpBuf, missingList.front());
		missingList.pop_front();
	}

	return ACAMSD_MTAP_Protocol::send(dsdptr, responseBuf, static_cast<unsigned short>(elementLength)) == 0;
}

//****************************************************************
// Unpack the MTAP 'Unlink-file' primitive.
//****************************************************************
ACAMSD_MTAP_Protocol::ErrorCode ACAMSD_MTAP_Protocol::unpackUnlinkFile (
		const unsigned char * buffer,
		unsigned long size,
		unsigned char & fileNumber) {
	if (size != 4) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The value of size is different from the expected one, returning UnspecifiedError");
		return UnspecifiedError;
	}

	fileNumber = buffer[1];

	return NoError;
}

//****************************************************************
// Acknowledge the MTAP-primitive contained in buffer. 
// (either unlink-all-files or unlink-file).
//****************************************************************
bool ACAMSD_MTAP_Protocol::sendUnlink_X_Response (ACS_DSD_Session & dsdptr, const unsigned char * buffer, ErrorCode result) {

	unsigned char responseBuf[8] = {0};
	unsigned char * tmpBuf = responseBuf;

	// Copy the contents of the unlink-all-files or unlink-file primitive to the buffer
	ACE_OS::memcpy(tmpBuf, buffer, 4);

	// Error code
	responseBuf[4] = result;

	return ACAMSD_MTAP_Protocol::send(dsdptr, responseBuf, 5) == 0;
}

//****************************************************************
//                 Private help-functions
//****************************************************************

//****************************************************************
// Interpret the Element Length value in an MTAP-primitive.
// The most significant bit is always set, and must thus be 
// masked away.
//****************************************************************
unsigned long ACAMSD_MTAP_Protocol::getElementLength (const unsigned char * buf) {
	unsigned long len = *(reinterpret_cast<const unsigned long *>(buf));

	if ((len & 0x000000FF) != 0x00000080) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while reading the element length, exiting");
		return 0;
	} else { len &= 0xFFFFFF7F; }

	// switch from big to little endian.
	unsigned char * res = reinterpret_cast<unsigned char *>(&len);
	char temp;
	temp = *(res + 3);
	*(res + 3) = *res;
	*res = temp;
	temp = *(res + 2);
	*(res + 2) = *(res + 1);
	*(res + 1) = temp;

	return len;
}

//****************************************************************
// Assign given value to the Element Length part of an MTAP-
// primitive. It is coded as an ordinary unsigned long with the 
// most significant bit set.
//****************************************************************
void ACAMSD_MTAP_Protocol::putElementLength (unsigned char * & buf, unsigned long value) {

	unsigned char * tmpBuf = reinterpret_cast<unsigned char *>(&value);
	unsigned char temp;
	// RPC format, Big endian.
	temp = tmpBuf[3];
	tmpBuf[3] = tmpBuf[0];
	tmpBuf[0] = temp;
	temp = tmpBuf[1];
	tmpBuf[1] = tmpBuf[2];
	tmpBuf[2] = temp;
	tmpBuf[0] |= 0x80;

	ACE_OS::memcpy(buf, tmpBuf, 4);
	buf += 4;

}

//****************************************************************
// Convert an AXE 4-bytes value to an unsigned long
// remove this function later on, since no conversion on NT is needed.
//****************************************************************
unsigned long ACAMSD_MTAP_Protocol::get4bytes (const unsigned char * buf) {
	unsigned long result;
	unsigned char * resBuf = reinterpret_cast<unsigned char *>(&result);

	resBuf[0] = buf[0];
	resBuf[1] = buf[1];
	resBuf[2] = buf[2];
	resBuf[3] = buf[3];

	return result;
}

//****************************************************************
//      send()
//****************************************************************
int ACAMSD_MTAP_Protocol::send (ACS_DSD_Session & dsdptr, unsigned char * buffer, unsigned short len) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Sending primitive:");
	ACS_ACA_LOG_DUMP(buffer, len, ACS_ACA_DEFAULT_DUMP_SIZE);

	int call_result = dsdptr.sendf(buffer, len, MSG_NOSIGNAL);
	if (call_result < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while sending the primitive to the remote peer, "
				"call_result = %d, error_message: '%s'", call_result, dsdptr.last_error_text());

		ACS_DSD_Node remote_node;
		if (dsdptr.get_remote_node(remote_node)) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'get_remote_node' failed, returning -1");
			return -1;
		}
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while sending the primitive to the remote peer"
				" (system_id = %u), returning %d!", remote_node.system_id, call_result);
		return call_result;
	}

	return 0;
}

//****************************************************************
//      receive()
//****************************************************************
bool ACAMSD_MTAP_Protocol::receive (ACS_DSD_Session & dsdptr, char * buffer, unsigned int /*len*/, int * bytesReceived) {
	ACE_HANDLE myHandle[4] = {0};
	int handleCount = 4;
	*bytesReceived = -1;

	if (dsdptr.get_handles(myHandle, handleCount)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'get_handles' failed, returning false");
		return false;
	}

	struct pollfd myPollfds[handleCount];
	for (ACE_INT32 i = 0; i < handleCount; i++) {
		myPollfds[i].fd = myHandle[i];
		myPollfds[i].events = POLLIN | POLLRDHUP | POLLHUP | POLLERR;
		myPollfds[i].revents = 0;
	}

	ACE_Time_Value selectTime;
	selectTime.set(10);

	ACE_INT32 waitResult = ACE_OS::poll(myPollfds, handleCount, &selectTime);

	if (waitResult == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Timeout expired while trying to receive a primitive, returning false");
		return false;
	} else if (waitResult < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'poll' failed while trying to receive a primitive (errno = %d), returning false", ACE_OS::last_error());
		return false;
	}

	if ((*bytesReceived = dsdptr.recv(buffer, ACS_DSD_MAXBUFFER)) <= 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'recv' failed, bytesReceived = %d, error_message: '%s' returning false",
				*bytesReceived, dsdptr.last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Received primitive:");
	ACS_ACA_LOG_DUMP(reinterpret_cast<unsigned char*> (buffer), *bytesReceived, ACS_ACA_DEFAULT_DUMP_SIZE);

	return true;
}


//****************************************************************
// Convert an unsigned long to an AXE 4-bytes value, and place it 
// in the buffer.
//****************************************************************
void ACAMSD_MTAP_Protocol::put4bytes (unsigned char * & buf, unsigned long value) {
	unsigned char * valBuf = reinterpret_cast<unsigned char *>(&value);

	buf[0] = valBuf[0];
	buf[1] = valBuf[1];
	buf[2] = valBuf[2];
	buf[3] = valBuf[3];
	buf += 4;
}
