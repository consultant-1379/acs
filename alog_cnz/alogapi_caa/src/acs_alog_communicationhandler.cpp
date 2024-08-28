/*
 * acs_alog_communicationhandler.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: xanttro
 */

#include <iostream>
#include <fstream>
#include <time.h>

#include "acs_alog_communicationhandler.h"
#include <fcntl.h>

#define PIPE_NAME		"/var/run/ap/ALOG_API_pipe"

using namespace std;

acs_alog_communicationhandler::acs_alog_communicationhandler(): errorText()
{
	handle = ACE_INVALID_HANDLE;
	errorCode = acs_alog_ok;
}

acs_alog_communicationhandler::~acs_alog_communicationhandler()
{
}

int acs_alog_communicationhandler::sendMsg (const char *eventMsg){

	ACE_LSOCK_Stream Stream;
	ACE_LSOCK_Connector Connector;
	ACE_UNIX_Addr addr(PIPE_NAME);
	int byte_sent = 0;
	int byte_to_send = strlen(eventMsg) + 4;

	if ( Connector.connect ( Stream, addr ) >= 0 ){
		char* my_buffer = new char[byte_to_send];
		*reinterpret_cast<uint32_t *>(my_buffer) = htonl ( byte_to_send  - 4);
		strncpy(my_buffer+4,eventMsg,byte_to_send - 4);

		if ( ( byte_sent = Stream.send_n ( my_buffer, byte_to_send )) < 0 ){
			//printf("%s write_n fails, errno = %d (%s)\n", __PRETTY_FUNCTION__, errno, strerror(errno));
			setErrorCode(acs_alog_write_error);
			setErrorText("Write not complete");
			Stream.close();
			delete []my_buffer;
			return -1;
		}
		delete []my_buffer;
	}
	else {
		//printf("%s write_n fails, errno = %d (%s)\n", __PRETTY_FUNCTION__, errno, strerror(errno));
		setErrorCode(acs_alog_write_error);
		setErrorText("Write not complete");
		Stream.close();
		return -1;
	}

	Stream.close();

	return 0;
}

acs_alog_returnType acs_alog_communicationhandler::getErrorCode() const
{
	return errorCode;
}

std::string acs_alog_communicationhandler::getErrorText() const
{
	return errorText;
}

ACE_HANDLE acs_alog_communicationhandler::getHandle() const
{
	return handle;
}

void acs_alog_communicationhandler::setErrorCode(acs_alog_returnType errorCode)
{
	this->errorCode = errorCode;
}

void acs_alog_communicationhandler::setErrorText(std::string errorText)
{
	this->errorText = errorText;
}

void acs_alog_communicationhandler::setHandle(ACE_HANDLE handle)
{
	this->handle = handle;
}
