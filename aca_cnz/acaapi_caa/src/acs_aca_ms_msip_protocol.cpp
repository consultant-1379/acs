/*=================================================================== */
   /**
   @file acs_aca_ms_msip_protocol.cpp

   Class method implementation for MSA API.

   This module contains the implementation of class declared in
   the acs_aca_ms_msip_protocol.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       18/08/2011     XTANAGG       Initial Release            **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <sys/types.h>
#include <sys/socket.h>

#include "acs_aca_message_store_access_point.h"
#include "acs_aca_utility.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_ms_msip_protocol.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_ACAMS_MSIP_Protocol);


/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: getPrimitive
=================================================================== */
ACAMS_MSIP_Protocol::Primitive ACAMS_MSIP_Protocol::getPrimitive(ACE_HANDLE msip_socket, unsigned int & size, unsigned char * buffer) {
//	ACS_ACA_TRACE_FUNCTION;

	// First there is a size indicator (unsigned int)
	errno = 0;
	ssize_t recv_result = ACE_OS::recv(msip_socket, reinterpret_cast<char *>(&size), sizeof(size), MSG_DONTWAIT);
	int errno_save = errno;

	if ((recv_result <= 0) && ((errno_save == EAGAIN) || (errno_save == EWOULDBLOCK))) {
		ACS_ACA_TRACE_MESSAGE("No data was present on the socket (recv call returned %d), waiting...", errno_save);

		fd_set readFds;
		FD_ZERO(&readFds);
		FD_SET(msip_socket, &readFds);
		ACE_Time_Value timeout;
		timeout.sec(3);

		errno = 0;
		const int select_result = ACE_OS::select((msip_socket + 1), &readFds, 0, 0, &timeout);
		errno_save = errno;

		if (select_result < 0) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'select' failed: returning None - "
					"select_result == %d, errno == %d", select_result, errno_save);
			return None;
		}
		else if (select_result == 0) {
			ACS_ACA_TRACE_MESSAGE("WARNING: Timeout expired while waiting data on the socket:"
					" returning None - errno == %d", errno_save);
			return None;
		}
		else {
			ACS_ACA_TRACE_MESSAGE("On the socket there is data to be read, calling 'recv'!");

			errno = 0;
			recv_result = ACE_OS::recv(msip_socket, reinterpret_cast<char *>(&size), sizeof(size), 0);
			errno_save = errno;

			if (recv_result <= 0) {
				ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' failed: returning None - "
						"recv_result == %zd, errno == %d", recv_result, errno_save);
				return None;
			}
		}
	}
	else if (recv_result <= 0) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' failed: returning None - errno == %d", errno_save);
		return None;
	}

	// After getting the size, check that the value is the expected one
	if (recv_result != sizeof(size)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' returned %zd bytes instead of %zu:"
				" returning None", recv_result, sizeof(size));
		return None;
	}
 
	// Now read the rest into the buffer
	errno = 0;
	recv_result = ACE_OS::recv(msip_socket, reinterpret_cast<char *>(buffer), size, 0);
	errno_save = errno;

	if (recv_result <= 0) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' failed: returning None - errno == %d", errno_save);
		return None;
	}

	if (recv_result != size) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' returned %zd bytes instead of %u:"
				" returning None", recv_result, size);
		return None;
	}

	// First in the buffer is the primitive indicator
	return *((Primitive*)buffer);
}

/*===================================================================
   ROUTINE: sendConnectionAccepted
=================================================================== */
bool ACAMS_MSIP_Protocol::sendConnectionAccepted(ACE_HANDLE msip_socket) {
	ACS_ACA_TRACE_FUNCTION;

	char sendBuf[sizeof(unsigned int) + sizeof(Primitive)];
	unsigned int * currentPos = (unsigned int *)sendBuf;

	// First a length indicator
	unsigned int length = sizeof(Primitive);
	*currentPos = length;
	++currentPos;

	// Then the primitive identifier
	*currentPos = ConnectionAccepted;

	// Now send it
	unsigned int totalLength = sizeof(unsigned int) + length;
	int rc = ACE_OS::send(msip_socket, sendBuf, totalLength, MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1))	{
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}

	return true;
}

/*===================================================================
   ROUTINE: sendConnectionRefused
=================================================================== */
bool ACAMS_MSIP_Protocol::sendConnectionRefused(ACE_HANDLE msip_socket, const char * message) {
	ACS_ACA_TRACE_FUNCTION;

	int messageLen = (message ? strlen(message) : 0);
	unsigned size = sizeof(Primitive) + messageLen + 1;
	char sendBuf [sizeof(unsigned int) + size];
	char * ptrBuf = sendBuf;

	*reinterpret_cast<unsigned *>(ptrBuf) = size;
	*reinterpret_cast<Primitive *>(ptrBuf += sizeof(unsigned)) = ConnectionRefused;
	ptrBuf += sizeof(Primitive);

	if (messageLen > 0)
		ACE_OS::strcpy(ptrBuf, message);

	*(ptrBuf + messageLen) = '\0';

	// Now send it
	unsigned totalLength = sizeof(unsigned) + size; //sizeof(unsigned int) + length;
	int rc = ACE_OS::send(msip_socket, sendBuf, totalLength, MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}

	return true;
}

/*===================================================================
   ROUTINE: unpackConnectionRefused
=================================================================== */
ACAMS_MSIP_Protocol::ErrorCode ACAMS_MSIP_Protocol::unpackConnectionRefused(	const unsigned char * buffer,	unsigned size, std::string & message) {
	ACS_ACA_TRACE_FUNCTION;

	if (size < (sizeof(Primitive) + 1))	{
		ACS_ACA_TRACE_MESSAGE("ERROR: [size < (sizeof(Primitive) + 1)], unspecified error");
		return UnspecifiedError;
	}

	int messageLen = size - sizeof(Primitive) - 1;
	message.clear();
	message.assign((messageLen > 0 ? reinterpret_cast<const char *>(buffer + sizeof(Primitive)) : ""), messageLen);

	return NoError;
}

/*===================================================================
   ROUTINE: sendDeleteFile
=================================================================== */
bool ACAMS_MSIP_Protocol::sendDeleteFile(ACE_HANDLE msip_socket, const std::string & fileName) {
	ACS_ACA_TRACE_FUNCTION;

	char sendBuf[sizeof(unsigned int) + sizeof(Primitive) + 32];
	unsigned int * currentPos = (unsigned int *)sendBuf;

	// First a length indicator
	unsigned int length = sizeof(Primitive) + fileName.size() + 1;
	*currentPos = length;
	++currentPos;

	// Then the primitive identifier
	*currentPos = DeleteFile;
	++currentPos;

	// The file name
	ACE_OS::strcpy((char *)currentPos, fileName.c_str());

	// Now send it
	unsigned int totalLength = sizeof(unsigned int) + length;
	int rc = ACE_OS::send(msip_socket, sendBuf, totalLength, MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}
	
	return true;
}

/*===================================================================
   ROUTINE: sendCPSystemId
=================================================================== */
bool ACAMS_MSIP_Protocol::sendCPSystemId(ACE_HANDLE socket, unsigned cpSystemId) {
	ACS_ACA_TRACE_FUNCTION;

	char buffer[sizeof(unsigned) + sizeof(Primitive) + sizeof(unsigned)] = {0};
	char * bufPtr = buffer;

	*reinterpret_cast<unsigned *>(bufPtr) = sizeof(Primitive) + sizeof(unsigned);
	bufPtr += sizeof(unsigned);

	*reinterpret_cast<Primitive *>(bufPtr) = CPSystemId;
	bufPtr += sizeof(Primitive);

	*reinterpret_cast<unsigned *>(bufPtr) = cpSystemId;

	unsigned int totalLength = sizeof(buffer);
	int rc = ACE_OS::send(socket, buffer, sizeof(buffer), MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}

	return true;
}

/*===================================================================
   ROUTINE: unpackCPSystemId
=================================================================== */
ACAMS_MSIP_Protocol::ErrorCode ACAMS_MSIP_Protocol::unpackCPSystemId(	const unsigned char * buffer, unsigned size, unsigned & cpSystemId) {
	ACS_ACA_TRACE_FUNCTION;

	if (size != (sizeof(Primitive) + sizeof(unsigned)))	{
		ACS_ACA_TRACE_MESSAGE("ERROR: [size != (sizeof(Primitive) + sizeof(unsigned))], unspecified error");
		return UnspecifiedError;
	}
	cpSystemId = *reinterpret_cast<const unsigned *>(buffer + sizeof(Primitive));

	return NoError;
}

/*===================================================================
   ROUTINE: sendCPSystemIdRequest
=================================================================== */
bool ACAMS_MSIP_Protocol::sendCPSystemIdRequest(ACE_HANDLE msip_socket) {
	ACS_ACA_TRACE_FUNCTION;

	char buffer[sizeof(unsigned) + sizeof(Primitive)];
	*reinterpret_cast<unsigned *>(buffer) = sizeof(Primitive);
	*reinterpret_cast<Primitive *>(buffer + sizeof(unsigned)) = CPSystemIdRequest;

	unsigned int totalLength = sizeof(buffer);
	int rc = ACE_OS::send(msip_socket, buffer, sizeof(buffer), MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}

	return true;
}


/*===================================================================
   ROUTINE: sendCPSystemIdUnavailable
=================================================================== */
bool ACAMS_MSIP_Protocol::sendCPSystemIdUnavailable(ACE_HANDLE msip_socket) {
	ACS_ACA_TRACE_FUNCTION;

	char buffer[sizeof(unsigned) + sizeof(Primitive)];
	*reinterpret_cast<unsigned *>(buffer) = sizeof(Primitive);
	*reinterpret_cast<Primitive *>(buffer + sizeof(unsigned)) = CPSystemIdUnavailable;

	unsigned int totalLength = sizeof(buffer);
	int rc = ACE_OS::send(msip_socket, buffer, sizeof(buffer), MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}

	return true;
}

/*===================================================================
   ROUTINE: unpackDeleteFile
=================================================================== */
ACAMS_MSIP_Protocol::ErrorCode ACAMS_MSIP_Protocol::unpackDeleteFile(	const unsigned char * buffer, unsigned size, std::string & fileName) {
	ACS_ACA_TRACE_FUNCTION;

	fileName = (char *)(buffer + sizeof(Primitive));

	if (fileName.size() != (size - sizeof(Primitive) - 1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: [fileName.size() != (size - sizeof(Primitive) - 1)], unspecified error");
		return UnspecifiedError;
	}
	
	return NoError;
}

/*===================================================================
   ROUTINE: sendRequestMsg
=================================================================== */
bool ACAMS_MSIP_Protocol::sendRequestMsg(ACE_HANDLE msip_socket, unsigned long long msgNum) {
	ACS_ACA_TRACE_FUNCTION;

	char sendBuf[sizeof(unsigned int) + sizeof(Primitive) + sizeof(unsigned long long)];
	unsigned int * currentPos = (unsigned int *)sendBuf;

	// First a length indicator
	unsigned int length = sizeof(Primitive) + sizeof(unsigned long long);
	*currentPos = length;
	++currentPos;

	// Then the primitive identifier
	*currentPos = RequestMsg;
	++currentPos;

	// The message number
	setBufferFromUll(msgNum, reinterpret_cast<unsigned char *>(currentPos)); //*reinterpret_cast<ULONGLONG *>(currentPos) = msgNum;

	// Now send it
	unsigned int totalLength = sizeof(unsigned int) + length;
	int rc = ACE_OS::send(msip_socket, sendBuf, totalLength, MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}
	
	return true;
}

/*===================================================================
   ROUTINE: unpackRequestMsg
=================================================================== */
ACAMS_MSIP_Protocol::ErrorCode ACAMS_MSIP_Protocol::unpackRequestMsg(const unsigned char * buffer, unsigned size, unsigned long long &msgNumber) {
	ACS_ACA_TRACE_FUNCTION;

	if (size != (sizeof(Primitive) + sizeof(unsigned long long))) {
		ACS_ACA_TRACE_MESSAGE("ERROR: [size != (sizeof(Primitive) + sizeof(unsigned long long)))], unspecified error");
		return UnspecifiedError;
	}
	msgNumber = convertToUll(buffer + sizeof(Primitive)); //*reinterpret_cast<const ULONGLONG *>(buffer + sizeof(Primitive));

	return NoError;
}

/*===================================================================
   ROUTINE: sendMsgAvailable
=================================================================== */
bool ACAMS_MSIP_Protocol::sendMsgAvailable(ACE_HANDLE msip_socket) {
	ACS_ACA_TRACE_FUNCTION;

	char sendBuf[sizeof(unsigned int) + sizeof(Primitive)];
	unsigned int * currentPos = (unsigned int *)sendBuf;

	// First a length indicator
	unsigned int length = sizeof(Primitive);
	*currentPos = length;
	++currentPos;

	// Then the primitive identifier
	*currentPos = MsgAvailable;

	// Now send it
	unsigned int totalLength = sizeof(unsigned int) + length;
	int rc = ACE_OS::send(msip_socket, sendBuf, totalLength, MSG_NOSIGNAL);

	if (((unsigned)rc < totalLength) || (rc == -1))	{
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
		return false;
	}
	
	return true;
}

/*===================================================================
   ROUTINE: unpackMsgAvailable
=================================================================== */
ACAMS_MSIP_Protocol::ErrorCode ACAMS_MSIP_Protocol::unpackMsgAvailable(const unsigned char * /*buffer*/, unsigned size) {
	ACS_ACA_TRACE_FUNCTION;

	if (size != sizeof(Primitive)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: [size != sizeof(Primitive)], unspecified error");
		return UnspecifiedError;
	}
	
	return NoError;
}

/*===================================================================
   ROUTINE: sendApplDisc
=================================================================== */
void ACAMS_MSIP_Protocol::sendApplDisc(ACE_HANDLE msip_socket) {
	ACS_ACA_TRACE_FUNCTION;

	char sendBuf[sizeof(unsigned int) + sizeof(Primitive)];
	unsigned int * currentPos = (unsigned int *)sendBuf;

	// First a length indicator
	unsigned int length = sizeof(Primitive);
	*currentPos = length;
	++currentPos;

	// Then the primitive identifier
	*currentPos = ApplDisc;

	// Now send it
	unsigned int totalLength = sizeof(unsigned int) + length;
	int rc = ACE_OS::send(msip_socket, sendBuf, totalLength, MSG_NOSIGNAL);

	if ( ((unsigned)rc < totalLength) || (rc == -1) )
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'send' failed!, errno == %d", ACE_OS::last_error());
}

/*===================================================================
   ROUTINE: getMSIPportNo
=================================================================== */
int ACAMS_MSIP_Protocol::getMSIPportNo(const char * name) {
	ACS_ACA_TRACE_FUNCTION;

	char totstr[256] = {0};
	int MSIPport = 0;
	ACE_OS::strcpy(totstr, name);
	int totln = ACE_OS::strlen(totstr);

	while (totln--)
		MSIPport += totstr[totln];

	if (MSIPport < 15000)
		MSIPport += 15000;
	else if (MSIPport > 65000) {
		while (MSIPport > 65000)
			MSIPport -= 30000;
	}
	ACS_ACA_TRACE_MESSAGE("Value of MSIP port for MS '%s' is %d", name, MSIPport);
	
	return MSIPport;
}

/*===================================================================
   ROUTINE: getMSIPportNo
=================================================================== */
int ACAMS_MSIP_Protocol::getMSIPportNo(const char * site, const char * name) {
	ACS_ACA_TRACE_FUNCTION;

	char totstr[256] = {0};
	int MSIPport = 0;
	ACE_OS::strcpy(totstr, site);
	ACE_OS::strcat(totstr, name);
	int totln = ACE_OS::strlen(totstr);

	while (totln--)
		MSIPport += totstr[totln];

	if (MSIPport < 15000)
		MSIPport += 15000;
	else if (MSIPport > 65000) {
		while (MSIPport > 65000)
			MSIPport -= 30000;
	}
	ACS_ACA_TRACE_MESSAGE("Value of MSIP port for MS '%s' of CP %s is %d", name, site, MSIPport);
	
	return MSIPport;
}
