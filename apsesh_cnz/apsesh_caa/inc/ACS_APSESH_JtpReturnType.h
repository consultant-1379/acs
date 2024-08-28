//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_JtpReturnType
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************
#ifndef ACS_APSESH_JtpReturnType_h
#define ACS_APSESH_JtpReturnType_h 1

#include <string>

struct ACS_APSESH_JtpReturnType 
{
	//	One of these return codes is returned every time an attempt is made to fetch
	//	data from SESH via JTP.  They give information regarding the handling of the
	//	JTP primitves used to communication with SESH.
	//	JTP_OK - Data has been received or sent successfully
	//	JTP_TIMEOUT - The conversation's handle was not signaled before the timeout
	//	expired.
	//	JTP_DISCONNECT - The JTP connection has been closed by SESH.
	//	JTP_APSESH_SHUTDOWN - The operator is intervening to shut down the APSESH
	//	service.
	//	JTP_WAIT_FAILED - A communication fault has has occured.  This may happen,
	//	for instance, if a handle is invalid or if the peer has reset.
	//	JTP_UNEXPECTED - An error has been encountered, which is not expected to
	//	ever occur.  It should be assumed that the connection needs to be reset.

	typedef enum {
		JTP_OK, JTP_NO_CONNECTION, JTP_NO_DATA,
		JTP_DISCONNECT, JTP_QUORUM_DISSOLVE_DISCONNECT,
		JTP_APSESH_SHUTDOWN, JTP_FAIL, JTP_FAIL_UNEXPECTED }
		ReturnCode;

	std::string returnMsg;
	ReturnCode returnCode;
};

// Class ACS_APSESH_JtpReturnType 

// Class ACS_APSESH_JtpReturnType 

#endif
