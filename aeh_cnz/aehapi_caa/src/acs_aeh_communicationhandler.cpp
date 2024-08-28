//========================================================================================
/** @file  acs_aeh_communicationhandler.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-23
 *	@version 1.0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2011-05-24 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-08 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 * -
 *
 */
//========================================================================================


#include "acs_aeh_communicationhandler.h"
#include "acs_aeh_eventmsg.h"

#include <cassert>
#include <ace/OS_NS_unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define PIPE_NAME "/var/run/ap/acs_aehfifo"
#define SOCK_NAME "/var/run/ap/acs_aehusock"


#define MAX_BUF_SIZE	4096

//========================================================================================
//	Default Constructor
//========================================================================================

acs_aeh_communicationhandler::acs_aeh_communicationhandler() : errorText(){
	handle = ACE_INVALID_HANDLE;
	errorCode = 0;
}


//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_communicationhandler::~acs_aeh_communicationhandler()
{
}


//========================================================================================
//	Open method
//========================================================================================

int acs_aeh_communicationhandler::open(){
//	int fd;
//
//	fd = ::open(PIPE_NAME, O_RDWR);
//	if( fd == -1 ){
//		setErrorCode(errno);
//		setErrorString(std::string(strerror(errno)));
//		return -1;
//	}
//
//	setHandle(fd);
//
//	return 0;

//Because of rsyslog introduction in SLES12 events are written on a unix socket instead of a pipe

//	if ( Connector.open ( addr) < 0 ){
//		std::string errorString1 = std::string(strerror(errno));
//		printf("acs_aeh_communicationhandler::open ERROR errno=%d message=%s", errno, errorString1.c_str() );
//		setErrorCode(errno);
//		setErrorString(std::string(strerror(errno)));
//		return -1;
//	}

	return 0;
}


//========================================================================================
//	Close method
//========================================================================================

int acs_aeh_communicationhandler::close(){
//	int fd;
//
//	fd = ::close(handle);
//	if( fd == -1 ){
//		setErrorCode(errno);
//		setErrorString(std::string(strerror(errno)));
//		return -1;
//	}
//
//	setHandle(ACE_INVALID_HANDLE);
//
//	return 0;

//Because of rsyslog introduction in SLES12 events are written on a unix socket instead of a pipe

//	int ret;
//	ret = Stream.close();
//	if( ret == -1 ){
//		setErrorCode(errno);
//		setErrorString(std::string(strerror(errno)));
//		return -1;
//	}
	return 0;
}


int acs_aeh_communicationhandler::sendMsg(acs_aeh_eventmsg p_eventMsg){

	//size_t written;
	int sentOnSock = 0;


	std::string message = p_eventMsg.getEventMessage();

	int dimBuff = message.length();

	if ( dimBuff > MAX_BUF_SIZE ){
		setErrorCode(-200);
		setErrorString(std::string("Event message exceeds the maximum size allowed"));
		return -1;
	}

	char buf[dimBuff];

	memset(buf, 0, dimBuff+1);

	::sprintf(buf, "%s\n", message.c_str());
//Because of rsyslog adoption in SLES12 a unix socketis used instead of a pipe the following lines commented
//and substitued by the next
//	if ( handle == ACE_INVALID_HANDLE  ){
//		setErrorCode(-99);
//		setErrorString(std::string("Invalid file descriptor"));
//		return -1;
//	}

//	if( !ACE::write_n( handle, &buf, strlen(buf), &written ) ){
//		setErrorCode(errno);
//		setErrorString(std::string(strerror(errno)));
//		return -1;
//	}

//1	if ( Stream.get_handle() == ACE_INVALID_HANDLE  ){
//1		setErrorCode(-99);
//1		setErrorString(std::string("Invalid file descriptor"));
//1		return -1;
//1	}

//	if ( ( byte_sent = Stream.send_n ( my_buffer, byte_to_send )) < 0 ){
//	if ( (sentOnSock = Stream.send_n ( &buf, strlen(buf)) ) < 0 ){


	char *sock_client = ACE_OS::tempnam ();

	ACE_LSOCK_Dgram Connector1 ((ACE_UNIX_Addr) (sock_client));

	chmod(SOCK_NAME,0666);

	if ( (sentOnSock = Connector1.send ( &buf, strlen(buf), (ACE_UNIX_Addr)SOCK_NAME ) ) < 0 ){
		//printf("%s Connector.send , errno = %d (%s)\n", __PRETTY_FUNCTION__, errno, strerror(errno));
		setErrorCode(errno);
		setErrorString(std::string(strerror(errno)));
//		Stream.close();
		Connector1.close();
		unlink(sock_client);
		free(sock_client);

		return -1;
	}
	//printf("acs_aeh_communicationhandler::sendMsg send on socket ok sentOnSock = %d \n dimBuff = %d\n strlen(buf) = %lu \n", sentOnSock, dimBuff, strlen(buf) );

	//if( sentOnSock != strlen(buf) ){
	if( sentOnSock != dimBuff+1 ){

		setErrorCode(-201);
		setErrorString(std::string("Write not complete "));
		Connector1.close();
		unlink(sock_client);
		free(sock_client);
		return -1;
	}

	Connector1.close();
	unlink(sock_client);
	free(sock_client);
	return 0;

}


//========================================================================================
//	setHandle method
//========================================================================================

void acs_aeh_communicationhandler::setHandle(int p_handle){

	handle = p_handle;
}


//========================================================================================
//	setErrorCode method
//========================================================================================

void acs_aeh_communicationhandler::setErrorCode(int p_errorCode){

	errorCode = p_errorCode;

}


//========================================================================================
//	setErrorString method
//========================================================================================

void acs_aeh_communicationhandler::setErrorString(std::string p_errorText){

	errorText = p_errorText;

}


//========================================================================================
//	getErrorCode method
//========================================================================================

int acs_aeh_communicationhandler::getErrorCode(){

	return errorCode;
}


//========================================================================================
//	getErrorCode method
//========================================================================================

std::string acs_aeh_communicationhandler::getErrorString(){

	return errorText;
}
