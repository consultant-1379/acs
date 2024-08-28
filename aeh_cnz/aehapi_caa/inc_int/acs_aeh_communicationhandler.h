//========================================================================================
/** @file acs_aeh_communicationhandler.h
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-24
 *	@version 1.1.1
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
 *	DESCRIPTION
 *	-
 *
 *
 *  ERROR HANDLING
 *	-
 *
 *
 *	SEE ALSO
 *	-
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2011-05-24 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-08 | xfabron      | Released for ITSTEAM2 sprint 12     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 */
//========================================================================================

#ifndef ACS_AEH_COMMUNICATIONHANDLER_H_
#define ACS_AEH_COMMUNICATIONHANDLER_H_

#include "ace/ACE.h"
#include "string"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include "ace/SOCK_Connector.h"
#include "ace/LSOCK_Dgram.h"


class acs_aeh_eventmsg;

class acs_aeh_communicationhandler{

public:
	//==============//
	// Constructors //
	//==============//

	/** @brief Default constructor
	 *
	 *	Class constructor
	 *
	 *	@remarks Remarks
	 */
	acs_aeh_communicationhandler();


	/** @brief destructor.
	 *
	 *	Class destructor.
	 *
	 *	@remarks -
	 */
	~acs_aeh_communicationhandler();


	//===========//
	// Functions //
	//===========//

	/**	@brief open method
	 *	Opens the communication channel.
	 *
	 *	@return int:	0 on success -1 on failure.
	 *
	 */
	int open();


	/**	@brief sendMsg method
	 *	Sends the event message on the channel
	 *
	 *	@return int:	0 on success -1 on failure.
	 *
	 */
	int sendMsg(acs_aeh_eventmsg p_eventMsg);


	/**	@brief close method
	 *	Close the communication channel.
	 *
	 *	@return int:	0 on success -1 on failure.
	 *
	 */
	int close();


	/**	@brief getErrorCode method
	 *	get the error code.
	 *
	 *	@return int:	error code.
	 *
	 */
	int getErrorCode();


	/**	@brief getErrorString method
	 *	get the error string.
	 *
	 *	@return int:	error string.
	 *
	 */
	std::string getErrorString();

private:

	/**	@brief setHandle method
	 *	set the file descriptor.
	 *
	 *	@param int:		file descriptor.
	 *
	 */
	void setHandle(int p_handle);


	/**	@brief setErrorCode method
	 *	set the error code.
	 *
	 *	@param int:		error code.
	 *
	 */
	void setErrorCode(int p_errorCode);


	/**	@brief setErrorString method
	 *	set the error string.
	 *
	 *	@param std::string:		error string.
	 *
	 */
	void setErrorString(std::string p_errorText);

	// Pipe handle
	ACE_HANDLE handle;

	// error code
	int errorCode;

	// error string
	std::string errorText;

	/* Because of rsyslog adoption in SLES12 it has been necessary to write events
	 *  on a unix socket instead of a pipe.
	 *  the following lines are added for this purpose
	 * */
	// Connector to write on unix socket instead of pipe

	//ACE_UNIX_Addr addr_aeh;
	ACE_LSOCK_Dgram Connector;


};


#endif /* ACS_AEH_COMMUNICATIONHANDLER_H_ */
