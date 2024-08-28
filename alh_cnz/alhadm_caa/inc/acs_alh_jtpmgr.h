/*
 * acs_alh_jtpmgr.h
 *
 *  Created on: Nov 03, 2011
 *      Author: xgiopap
 */

#ifndef ACS_ALH_JTPMGR_H_
#define ACS_ALH_JTPMGR_H_


/** @file acs_alh_jtpmgr
 *	@brief
 *	@author xgiopap (Giovanni Papale)
 *	@date 2011-11-03
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 * DESCRIPTION
 *	The acs_alh_jtpmgr class is responsible for
 *
 *		- handling connect and disconnect to ALCO, the alarm handler
 *		  in CP. The principle is to always be connected.
 *
 *		- in connect state: handling data traffic between ALH and ALCO
 *
 * ERROR HANDLING
 *		The acs_alh_jtpmgr class will report error event to the
 *		AP Event Handler when JTP connection is disconnected.
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-03 | xgiopap      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "acs_alh_util.h"
#include "acs_alh_log.h"
#include "acs_alh_eventmgr.h"
#include "acs_alh_common.h"
#include "acs_alh_error.h"
#include <ACS_JTP.h>

//========================================================================================
//  THE JTP_Msg stuff (start)
//========================================================================================

const time_t ACS_ALH_timelimit = 600;

typedef struct
{
    time_t lasttime;
	unsigned long count;
} JTP_event;


// Used to specify the mode for receiving messages.
typedef enum
{
    JTP_NO_BLOCK = 0,
	JTP_BLOCK
} JTP_BlockType;

// Holds two bytes of data inside some variants of JTP_Msg.
typedef unsigned short JTP_BytePair;


// Identifies the type of a message.
typedef enum
{
    JTP_JEXINITREQ,
	JTP_JEXINITIND,
	JTP_JEXINITRSP,
	JTP_JEXINITCONF,
	JTP_JEXDATAREQ,
	JTP_JEXDATAIND,
	JTP_JEXDISCREQ,
	JTP_JEXDISCIND
} JTP_MsgType;


// ---------------------------------------------------
// Description the variants of JTP_Msg.
// ---------------------------------------------------

// Describes the message JEXINITREQ
// Request to start conversation
typedef struct
{
	struct JTP_Node*       node;
    JTP_BytePair		   data1;
    JTP_BytePair		   data2;
    unsigned short		   maxBufferSize;
} JTP_Jexinitreq;

// Describes the message JEXINITCONF
// Confirmation to start conversation
typedef struct
{
    JTP_BytePair	data1;
    JTP_BytePair    data2;
    short           resultCode;
} JTP_Jexinitconf;

// Describes the message JEXINITIND
// Indication to start conversation
typedef struct
{
	struct JTP_Node*	node;
    JTP_BytePair		data1;
    JTP_BytePair		data2;
    unsigned short		maxBufferSize;
} JTP_Jexinitind;

// Describes the message JEXINITRSP
// Response to start conversation
typedef struct
{
    JTP_BytePair	data1;
    JTP_BytePair	data2;
    short			resultCode;
} JTP_Jexinitrsp;

// Describes the message JEXDATAREQ
// Request to transfer data
typedef struct
{
    JTP_BytePair	data1;
    JTP_BytePair	data2;
    short           buflen;
    unsigned char*  buffer;
} JTP_Jexdatareq;

// Describes the message JEXDATAIND
// Data received
typedef struct
{
    JTP_BytePair	data1;
    JTP_BytePair	data2;
    short           buflen;
    unsigned char*  buffer;
} JTP_Jexdataind;

// Describes the message JEXDISCREQ
// Request to disconnect conversation
typedef struct
{
    JTP_BytePair	data1;
    JTP_BytePair	data2;
    short			reasonCode;
} JTP_Jexdiscreq;

// Describes the message JEXDISCIND
// Indication to disconnect conversation
typedef struct
{
    JTP_BytePair	data1;
    JTP_BytePair	data2;
    short           reasonCode;
} JTP_Jexdiscind;


// Holds a message received or a message to be sent.
typedef struct
{
    unsigned short  handle;
    JTP_MsgType msgType;
    union
    {
        JTP_Jexinitreq  jexinitreq;
        JTP_Jexinitconf jexinitconf;
        JTP_Jexinitind  jexinitind;
        JTP_Jexinitrsp  jexinitrsp;
        JTP_Jexdatareq  jexdatareq;
        JTP_Jexdataind  jexdataind;
        JTP_Jexdiscreq  jexdiscreq;
        JTP_Jexdiscind  jexdiscind;
    };
} JTP_Msg;

//  THE JTP_Msg stuff (end)
typedef enum
{
    JTP_CONN_LOST,
	JTP_MSG_FAULT,
	JTP_NOT_CONN,
	JTP_NO_MESSAGE,
	JTP_OK,
	JTP_PROT_FAULT,
	JTP_SIG_INTR
} JTP_ReturnType;


//========================================================================================
//	Type declarations for return type
//========================================================================================

typedef unsigned long JtpMgrReturnType;		// Return type of the
											// ACS_ALH_JtpMgr class

//========================================================================================
//	Constants returned by ACS_ALH_JtpMgr methods
//========================================================================================

const JtpMgrReturnType JtpMgr_Error = 0;
const JtpMgrReturnType JtpMgr_OK = 1;
const JtpMgrReturnType JtpMgr_DISC = 2;
const JtpMgrReturnType JtpMgr_RES = 3;
const JtpMgrReturnType JtpMgr_NORMAL = 4;
const JtpMgrReturnType JtpMgr_DATA_REC = 5;


//========================================================================================
//	Internal states for JtpMgr (jtpMgrState)
//========================================================================================

const short DISC = 0;
const short INITREQ = 1;
const short STSREQ = 2;
const short UPDINI = 3;
const short NORMAL = 4;
const short DATAREQ = 5;
const short SHUTDOWN = 6;

//
// The meaning of the ALH states:
//
// DISC
//	ALH is not connected to JTP.
//	The next step is to send "connectAppl" to get connected.
//	If JTP_OK, send "JEXINITREQ" to ALCO application
//
// INITREQ
//	ALH has sent "JEXINITREQ" to ALCO
//	The next step is to receive "JEXINITCONF" from ALCO.
//	Send "JEXDATAREQ" with STS_RES/STS_UNC telling the status
//	of ALH.
//
// STSREQ
//	Waiting for JEXDATAIND from ALCO telling status of ALCO:
//	if ALCOstatus == STS_RES and ALHstatus == STS_UNC
//		send JEXDATAREQ with UPDINI to ALCO
//		set ALHstatus = UPDINI
//	else
//		set ALHstatus = NORMAL
//
// UPDINI
//	Waiting for JEXDATAIND with UPD_ACK from ALCO
//	Set ALHstatus = NORMAL
//
// NORMAL
//	ALH is connected to ALCO and has no unacknowledged alarms
//	sent to ALCO.
//	It is allowed here to send ALARM (ALA_INI) or
//	ALARM CEASING (ALA_CSG) to ALCO.
//	If message sent
//		set ALHstatus = DATAREQ
//
// DATAREQ
//	ALH has sent ALARM or ALARM CEASING to ALCO and waits for
//	JEXDATAIND containing ALA_ACK with acknowledge for sent message.
//	When ALA_ACK arrived
//		set ALHstatus = NORMAL
//
// SHUTDOWN
//	ALH shall send "disconnectAppl" to JTP to get disconnected
//	After this, set ALHstatus = DISC
//

//========================================================================================
//	Protocol element identifiers for ALH - ALCO communication
//========================================================================================

const short STS_RES = 0;	// ALH restarted since last connect
const short STS_UNC = 1;	// ALH not restarted since last connect
const short UPD_INI = 2;
const short UPD_ACK = 3;
const short ALA_INI = 4;
const short ALA_CSG = 5;
const short ALA_ACK = 6;



//========================================================================================
//	Struct for PHA parameter reading
//========================================================================================

const unsigned short SIZE_OF_CP_NAME_STRING = 16;
struct string16{ char str[SIZE_OF_CP_NAME_STRING]; };

//========================================================================================
//	Class declarations for acs_alh_jtpmgr
//========================================================================================
class acs_alh_jtpmgr : public virtual acs_aeh_error
{
public:

	acs_alh_jtpmgr();
	// Description:
	//		Constructor - create an empty JTP API object
	// Parameters:
	//		None
	// Return value:
	//	None

	acs_alh_jtpmgr(const acs_alh_eventmgr& eventMgr, short apNodeNumber);
	// Description:
	//		Constructor - create a JTP API object and initiate apNodeNumber
	// Parameters:
	//		eventMgr		Event manager
	//		apNodeNo		The number of the AP [129.. ]
	// Return value:
	//		None

	acs_alh_jtpmgr(const acs_alh_eventmgr& eventMgr, short apNodeNumber, const char alcoName[], ACS_JTP_Conversation::JTP_NodeState NS);
	// Description:
	//		Constructor - create a JTP API object and initiate apNodeNumber
	//		and alcoName
	// Parameters:
	//		eventMgr		Event manager
	//		apNodeNumber	The number of the AP [129.. ]
	//		alcoName		Name of ALCO appl.
	// Return value:
	//		None

	virtual ~acs_alh_jtpmgr();
	// Description:
	//		Destructor - send disconnect to JTP and delete JTP API object
	// Parameters:
	//		None
	// Return value:
	//		None

	JtpMgrReturnType getStatus(short alhStatus, unsigned short* cpAlarmRef);
	// Description:
	//		getStatus takes the elementary steps to
	//
	//		- if ALCO disconnected, initiate connect
	//
	//		- if ALCO connected, check if message has received
	//
	//		- if message previously sent
	//			if response received
	//			return this to ALH
	//
	//		- if circumstances on the line indicates diconnect
	//			set JtpMgr in disconnect state to re-establish
	//			connection
	//
	// Parameters:
	//		alhStatus		Current status of ALH:
	//						STS_RES	ALH restarted since last connect
	//						STS_UNC	ALH unchanged since last connect
	//		cpAlarmRef		Returned if JtpMgr_DATA_REC.
	//
	// Return value:
	//		JtpMgr_DISC		ALCO disconnected
	//		JtpMgr_RES		ALCO reported STS_RES
	//						all alarms must be resent
	//		JtpMgr_NORMAL	ALCO connected
	//		JtpMgr_DATA_REC	Message arrived from ALCO

	JtpMgrReturnType sendData(unsigned char* buffer, short bufLen, short elemId, short retransInd);
	// Description:
	//		Prepares and makes a send request for message
	// Parameters:
	//		buffer			Buffer to be sent to ALCO
	//		bufLen			Nr of characters in buffer
	//		elemId			Type of protocol element to be sent
	//						ALA_INI
	//						ALA_CSG
	//		retransInd		Indicates if retransmission of buffer
	// Return value:
	//		JtpMgr_DISC		ALCO disconnected
	//		JtpMgr_NORMAL	ALCO connected

	void fetchJtpMgrState(short &jtpMgrStatus);
	// Description:
	//		Fetches the value of jtpMgrState
	// Parameters:
	//		jtpMgrStatus	Used to fetch jtpMgrState
	// Return value:
	//		None

	JtpMgrReturnType checkALCOConnection();
	// Description:
	//		Check that connection available towards CP by sending empty
	//		messages to ALCO.
	// Parameters:
	//		None
	// Return value:
	//		True			Connection request sent.
	//		False			Connection request failed.

private:

	JtpMgrReturnType sendToJtpBuffer();
	// Description:
	//		Sends buffer via JTP API. All bad return codes result in DISC or SHUTDOWN state
	// Parameters:
	//		None
	// Return value:
	//		JtpMgr_OK		Buffer sent to JTP API OK
	//		JtpMgr_DISC		Problem sending the buffer

	JtpMgrReturnType handleDiscState();
	// Description:
	//		Send connectAppl to JTP API
	//		if OK send JEXINITREQ to CP application
	//		According to result above, jtpMgrState is set
	//		to INITREQ
	// Parameters:
	//		None
	// Return value:
	//		JtpMgr_OK		connectAppl sendt OK
	//						JEXINITREQ sent OK
	//		JtpMgr_DISC		Problem sending messages above

	void handleShutDownState();
	// Description:
	//		Send disconnectAppl to JTP API
	//		jtpMgrState is set to DISQ
	// Parameters:
	//		None
	// Return value:
	//		JtpMgr_DISC		disconnectAppl sent OK

	JtpMgrReturnType handleInitreqState(short alhStatus);
	// Description:
	//		Send STS_XXX to ALCO expecting ALCO status back
	//		If sending OK, jtpMgrState is set to STSREQ
	// Parameters:
	//		alhStatus		Status of ALH
	// Return value:
	//		JtpMgr_	DISC

	JtpMgrReturnType handleStatusreqState(short alhStatus);
	// Description:
	//		If ALCO expects UPD_INI, send it, else return
	// Parameters:
	//		alhStatus
	// Return value:
	//		JtpMgr_NORMAL	If ALH - ALCO connected
	//						and JtpMgrState is changed to NORMAL (normal)
	//		JtpMgr_	DISC	if still waiting for ALCO status
	//		JtpMgr_RES		if ALH must resend all the alarms
	//						and jtpMgrState is changed to UPDINI

	JtpMgrReturnType handleUpdiniState();
	// Description:
	//		If UPD_ACK received from ALCO, set jtpMgrState == NORMAL (connected)
	//		else return
	// Parameters:
	//		None
	// Return value:
	//		JtpMgr_NORMAL	If ALH - ALCO connected
	//						and JtpMgrState is changed to NORMAL
	//		JtpMgr_DISC		if still waiting for UPD_ACKi from ALCO

	JtpMgrReturnType handleDatareqState(unsigned short* cpAlarmRef);
	// Description:
	//		If received ALA_ACK for previous ALA_INI, return msg to caller
	// Parameters:
	//		cpAlarmRef		Ptr to area to be returned
	// Return value:
	//		JtpMgr_NORMAL	If no valid msg received
	//		JtpMgr_DATA_REC	if valid msg arrived

	JtpMgrReturnType reportEvent(const char problemData[], long specificProblem);
	// Description:
	//		Common reportimg of events to Event Mgr
	// Parameters:
	//		problemData
	//		specificProblem
	// Return value:
	//		JtpMgr_OK

	JtpMgrReturnType handleDataReceived(short alhStatus, unsigned short* cpAlarmRef);
	// Description:
	//		Data is received from JTP. Now analyze
	// Parameters:
	//		None
	// Return value:
	//		JtpMgr_OK		connectAppl sent OK
	//						JEXINITREQ sent OK
	//		JtpMgr_DISC		Problem sending messages above

	JTP_ReturnType receiveMsg(const JTP_BlockType blockFlag, JTP_Msg* msg);
	// Description:
	//		Receives a message.
	// Parameters:
	//		blockFlag		JTP_NO_BLOCK causes the method to
	//						return whether a message has arrived or not
	//		msg				A struct where the message will be placed
	// Return values:
	//		JTP_OK
	//		JTP_NO_MESSAGE
	//		JTP_CONN_LOST
	//		JTP_SIG_INTR

	JTP_ReturnType sendMsg(const JTP_Msg& msg);
	// Description:
	//		Sends the message
	// Parameters:
	//		msg				The message to be sent
	// Return value:
	//		JTP_OK
	//		JTP_CONN_LOST
	//		JTP_PROT_FAULT

	int checkTimeSinceLastEvent(JTP_event&, time_t limit);
	// Description:
	//		Checks that the event is not sent more often than 1/limit
	// Parameters:
	//		JTP_event
	//		limit
	// Return value:
	//		1				OK to send event
	//		0				Not OK


	void formatTrace(char* traceBuf, JTP_Msg* ptrMsgSend, JTP_Msg* ptrMsgRec);

	ACS_JTP_Conversation *jtpApi_;
	ACS_JTP_Conversation::JTP_Node *pNode;
	JTP_Msg sendToJtp_;
	JTP_Msg getFrJtp_;
	acs_alh_eventmgr eventMgr_;
	short jtpMgrState_;
	short alcoStatus_;
	short reportAllowed_;
	short apNodeNumber_;
	//char siteNameCP_[SIZE_OF_CP_NAME_STRING];
	const char* alcoName_;
	ACS_JTP_Conversation::JTP_NodeState NodeSt;
	JTP_event disconn_;
	JTP_event connlost_;
	acs_alh_util util; //Trace object
	acs_alh_log log; //Log object
};


#endif /* ACS_ALH_JTPMGR_H_ */
