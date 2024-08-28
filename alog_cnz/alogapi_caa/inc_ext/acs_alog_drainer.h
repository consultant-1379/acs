/** @file ACS_ALOG_Drainer.h
 *	@brief
 *	@author xgencol/xanttro/
 *	@date 2011-05-23
 *	@version 1.0.0
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
 *	The class ACS_ALOG_Drainer is used for logging event from Ericsson Application.
 *
 *	For a description of all method, see below.
 *
 *
 *	REVISION INFO
 *	+========+============+===============+=====================================+
 *	| REV    | DATE       | AUTHOR        | DESCRIPTION                         |
 *	+========+============+===============+=====================================+
 *	| 		 | 08/02/2011 |xgencol/xgaeerr| File created.                       |
 *	+--------+------------+---------------+-------------------------------------+
 *	|        | 20/05/2011 |xgencol/xanttro|									    |
 *	+--------+------------+---------------+-------------------------------------+
 *	|        |            |               |                                     |
 *	+========+============+===============+=====================================+
 */
//========================================================================================


#ifndef ACS_ALOG_DRAINER_H_
#define ACS_ALOG_DRAINER_H_

#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#include <fcntl.h>
#include <string.h>
#include <queue>
#include <syslog.h>

#include "acs_alog_types.h"
#include "acs_alog_communicationhandler.h"

//========================================================================================
// Constant Definition
//========================================================================================

const char tag_[]= "alogapi.notice";		// Used to compose output string
const char cmd_[]= "cmd=";					// Used to compose output string
const char user_[]= "user=";				// Used to compose output string
const char block_[]= "block =";				// Used to compose output string
const char pid_[]= "pid=";					// Used to compose output string
const char logdata_[] = "logdata=";			// Used to compose output string
const char logtime_[] = "logtime=";			// Used to compose output string
const char type_[] = "type=";				// Used to compose output string
const char device_[] = "device=";			// Used to compose output string
const char remotehost_[] = "remotehost=";	// Used to compose output string
const char size_[] = "size=";				// Used to compose output string
const char sessionid_[] = "ses=";	// Used to compose output string
const char prca_[] = "prca=";				// Used to compose output string
const char cmdid_[] = "cmdid=";				// Used to compose output string
const char cpnamegroup_[] = "cpnamegroup=";	// Used to compose output string
const char cpidlist_[] = "cpidlist=";		// Used to compose output string
const char cmdseqnum_[] = "cmdseqnum=";		// Used to compose output string
const char localhostInfo_[] = "localhostinfo=";// Used to compose output string
const char cpidlistSeparator_[] = "+";		// Used to compose output string
const char separator_[] = ", ";				// Used to compose output string
const char newline_[]= "\n";				// Used to compose output string
const char dash_[]= " - ";					// Used to compose output string
const char colon_[]= ": ";					// Used to compose output string
const char voidChar_[]= "";					// void char string
const int voidInt_= 0;						// void int number


//========================================================================================
// Class declaration
//========================================================================================


class acs_alog_Drainer
{

private:

	/**@brief
	 * buffer queue (FIFO) declaration.<br>
	 *
	 */
	std::queue<const char *> acs_alog_queue;


public:


	/**@brief
	 * This is the class constructor.<br>
	 */
	acs_alog_Drainer(){};


	/**@brief
	 *	This method search pattern in the audit event record.<br>
	 *	This method, after determining ifstd:: the search string exists in the record,<br>
	 *	it returns a substring derived based on the parameters offset and end.<br>
	 *
	 *	@param[in]    type				Type of event to be logged.
	 *  @param[in]    data				The command to logging.
	 *  @param[in]    remoteHost		The name of remoteHost.
	 *  @param[in]    remoteUser   		The user with write command.
	 *  @oaram[in]    dataLength		The exact length of the data field.
	 *  @param[in]    logdate			The date of executed command.
	 *  @param[in]    logTime			The time of executed command.
	 *  @param[in]    applicationName	the application who execute the command.
	 *  @param[in]    success			the returned value of the command
	 *  @param[in]	  pid				The pid of process.
	 *  @param[in]    device			The name of the device.
	 *  @param[in]	  sessId			Log Session ID.
	 *  @param[in]	  cmdId				MML command ID.
	 *  @param[in]    CPnameGroup 		The CP name or CP group name in an MSC-S BC system.
	 *  @param[in]	  CPIDlist			The CP identity of the CP or the CPs in CPnameGroup in an MSC-S BC system
	 *  @param[in]	  CPIDlistLength	The number of CP identities in CPIDlist in an MSC-S BC system.
	 *  @param[in]    cmdSeqNum			Used to indicate the command sequence number associated with the logged event
	 *
	 *
	 *  @return acs_alog_returnType
	 *
	 */

	acs_alog_returnType logData(
			const ACS_ALOG_DataUnitType type,
			const char * data,
			const char * remoteHost,
			const char * remoteUser,
			const int dataLength = voidInt_,
			const char * logDate = voidChar_,
			const char * logTime = voidChar_,
			const char * applicationName = voidChar_,
			const char * success = voidChar_,
			const char * pid = voidChar_,
			const char * device = voidChar_,
			const char * sessId = voidChar_,
			const char * cmdId = voidChar_,
			const char* CPnameGroup = voidChar_,
			const int* CPIDlist = NULL,
			const int CPIDlistLength = voidInt_,
			const unsigned cmdSeqNum = voidInt_  );


	/**@brief
		 *	This method search pattern in the audit event record.<br>
		 *	This method, after determining ifstd:: the search string exists in the record,<br>
		 *	it returns a substring derived based on the parameters offset and end.<br>
		 *
		 *	@param[in]    type				Type of event to be logged.
		 *  @param[in]    data				The command to logging.
		 *  @param[in]    remoteHost		The name of remoteHost.
		 *  @param[in]    remoteUser   		The user with write command.
		 *  @oaram[in]    dataLength		The exact length of the data field.
		 *  @param[in]    logdate			The date of executed command.
		 *  @param[in]    logTime			The time of executed command.
		 *  @param[in]    applicationName	the application who execute the command.
		 *  @param[in]    success			the returned value of the command
		 *  @param[in]	  pid				The pid of process.
		 *  @param[in]    device			The name of the device.
		 *  @param[in]	  sessId			Log Session ID.
		 *  @param[in]	  cmdId				MML command ID.
		 *  @param[in]    CPnameGroup 		The CP name or CP group name in an MSC-S BC system.
		 *  @param[in]	  CPIDlist			The CP identity of the CP or the CPs in CPnameGroup in an MSC-S BC system
		 *  @param[in]	  CPIDlistLength	The number of CP identities in CPIDlist in an MSC-S BC system.
		 *  @param[in]    cmdSeqNum			Used to indicate the command sequence number associated with the logged event
		 *  @param[in]	  prca
		 *  @param[in]    localHostInfo
		 *
		 *  @return acs_alog_returnType
		 *
		 */

		acs_alog_returnType logData(
				const ACS_ALOG_DataUnitType type,
				const char * data,
				const char * remoteHost,
				const char * remoteUser,
				const int dataLength,
				const char * logDate,
				const char * logTime,
				const char * applicationName,
				const char * success,
				const char * pid,
				const char * device,
				const char * sessId,
				const char * cmdId,
				const char* CPnameGroup,
				const int* CPIDlist,
				const int CPIDlistLength,
				const unsigned cmdSeqNum,
				const int prca,
				const char * localHostInfo);


	/**@brief
	 * This method open new session for logging.<br>
	 * This method, reset buffer and restart append of new session log message.<br>
	 *
	 */
	void newLoggingSession();


	/**@brief
	 * This method put the events into big buffer.<br>
	 * This method, put the events in buffer for follow sent at ALOG.<br>
	 *
	 *  @param[in] 	 data				This parameter contains the command.
	 *	@oaram[in]   dataLength			The exact length of the data field.
	 */
	void appendData(const char * data,const int dataLength = voidInt_);

	/**@brief
	 * This method confirm sent events.<br>
	 * This method, at session end, confirm sent events.<br>
	 *
	 *	@param[in]    type				Type of event to be logged.
	 *  @param[in]    remoteHost		The name of remoteHost.
	 *  @param[in]    remoteUser   		The user with write command.
	 *  @param[in]    logdate			The date of executed command.
	 *  @param[in]    logTime			The time of executed command.
	 *  @param[in]    applicationName	the application who execute the command.
	 *  @param[in]    success			the returned value of the command
	 *  @param[in]	  pid				The pid of process.
	 *  @param[in]    device			The name of the device.
	 *  @param[in]	  sessId			Log Session ID.
	 *  @param[in]	  cmdId				MML command ID.
	 *  @param[in]    CPnameGroup 		The CP name or CP group name in an MSC-S BC system.
	 *  @param[in]	  CPIDlist			The CP identity of the CP or the CPs in CPnameGroup in an MSC-S BC system
	 *  @param[in]	  CPIDlistLength	The number of CP identities in CPIDlist in an MSC-S BC system.
	 *  @param[in]    cmdSeqNum			Used to indicate the command sequence number associated with the logged event
	 *
	 *  @return acs_alog_returnType
	 */


	acs_alog_returnType commit(
			const ACS_ALOG_DataUnitType type,
			const char * remoteHost,
			const char * remoteUser,
			const char * logDate = voidChar_,
			const char * logTime = voidChar_,
			const char * applicationName = voidChar_,
			const char * success = voidChar_,
			const char * pid = voidChar_,
			const char * device = voidChar_,
			const char * sessId = voidChar_,
			const char * cmdId = voidChar_,
			const char* CPnameGroup = voidChar_,
			const int* CPIDlist = NULL,
			const int CPIDlistLength = voidInt_,
			const unsigned cmdSeqNum = voidInt_  );

	/**@brief
		 * This method confirm sent events.<br>
		 * This method, at session end, confirm sent events.<br>
		 *
		 *	@param[in]    type				Type of event to be logged.
		 *  @param[in]    remoteHost		The name of remoteHost.
		 *  @param[in]    remoteUser   		The user with write command.
		 *  @param[in]    logdate			The date of executed command.
		 *  @param[in]    logTime			The time of executed command.
		 *  @param[in]    applicationName	the application who execute the command.
		 *  @param[in]    success			the returned value of the command
		 *  @param[in]	  pid				The pid of process.
		 *  @param[in]    device			The name of the device.
		 *  @param[in]	  sessId			Log Session ID.
		 *  @param[in]	  cmdId				MML command ID.
		 *  @param[in]    CPnameGroup 		The CP name or CP group name in an MSC-S BC system.
		 *  @param[in]	  CPIDlist			The CP identity of the CP or the CPs in CPnameGroup in an MSC-S BC system
		 *  @param[in]	  CPIDlistLength	The number of CP identities in CPIDlist in an MSC-S BC system.
		 *  @param[in]    cmdSeqNum			Used to indicate the command sequence number associated with the logged event
		 *  @param[in]	  prca
		 *  @param[in]    localHostInfo
		 *
		 *  @return acs_alog_returnType
		 */


		acs_alog_returnType commit(
				const ACS_ALOG_DataUnitType type,
				const char * remoteHost,
				const char * remoteUser,
				const char * logDate,
				const char * logTime,
				const char * applicationName,
				const char * success,
				const char * pid,
				const char * device,
				const char * sessId,
				const char * cmdId,
				const char* CPnameGroup,
				const int* CPIDlist,
				const int CPIDlistLength,
				const unsigned cmdSeqNum,
				const int prca,
				const char * localHostInfo);

	/**@brief
	 * This method not confirm sent events.<br>
	 * This method, at session end, not confirm	sent events.<br>
	 * @return -
	 */
	acs_alog_returnType rollback();

	/**@brief
	 * This is the class destructor.<br>
	 *
	 */

	virtual ~acs_alog_Drainer(){};

};

/**@brief
 * This method verify the date time format<br>
 * @return int
 */
int checkDateTime(std::string data, std::string time);

#endif /* ACS_ALOG_DRAINER_H_ */
