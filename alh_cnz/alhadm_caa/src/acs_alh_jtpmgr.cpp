/*
 * acs_alh_jtpmgr.cpp
 *
 *  Created on: Nov 03, 2011
 *      Author: xgiopap
 */


#include "acs_alh_jtpmgr.h"
#include "ace/OS.h"
#include <stdio.h>
#include <iostream>
#include <poll.h>

using namespace std;

short tmpreason = -1;

const char *ALCOName_[] = {"ALCOEX", "ALCOSB"};

const char* ALH_STATUS_[] = {
		"STS_RES",
		"STS_UNC",
		"UPD_INI",
		"UPD_ACK",
		"ALA_INI",
		"ALA_CSG",
		"ALA_ACK"
};

//========================================================================================
//	Constructor
//========================================================================================
acs_alh_jtpmgr::acs_alh_jtpmgr() :
jtpApi_(),
pNode(0),
sendToJtp_(),
getFrJtp_(),
eventMgr_(),
jtpMgrState_(DISC),
alcoStatus_(STS_UNC),
reportAllowed_(true),
apNodeNumber_(0),
alcoName_(""),
disconn_(),
connlost_(),
util("JTPMGR_CLASS")
{


	jtpApi_ = new ACS_JTP_Conversation();
    memset(&sendToJtp_, 0, sizeof(sendToJtp_));
	disconn_.lasttime = 0;
	disconn_.count = 1;
	connlost_.lasttime = 0;
	connlost_.count = 1;
}


//========================================================================================
//	Constructor
//========================================================================================
acs_alh_jtpmgr::acs_alh_jtpmgr(const acs_alh_eventmgr& eventMgr, short apNodeNumber) :
jtpApi_(),
pNode(0),
sendToJtp_(),
getFrJtp_(),
eventMgr_(eventMgr),
jtpMgrState_(DISC),
alcoStatus_(STS_UNC),
reportAllowed_(true),
apNodeNumber_(apNodeNumber),
alcoName_(""),
disconn_(),
connlost_(),
util("JTPMGR_CLASS")
{


	jtpApi_ = new ACS_JTP_Conversation();
    memset(&sendToJtp_, 0, sizeof(sendToJtp_));
	disconn_.lasttime = 0;
	disconn_.count = 1;
	connlost_.lasttime = 0;
	connlost_.count = 1;
}


//========================================================================================
//	Constructor
//========================================================================================
acs_alh_jtpmgr::acs_alh_jtpmgr(const acs_alh_eventmgr& eventMgr, short apNodeNumber, const char alcoName[], ACS_JTP_Conversation::JTP_NodeState NS) :
jtpApi_(),
pNode(0),
sendToJtp_(),
getFrJtp_(),
eventMgr_(eventMgr),
jtpMgrState_(DISC),
alcoStatus_(STS_UNC),
reportAllowed_(true),
apNodeNumber_(apNodeNumber),
alcoName_(alcoName),
NodeSt(NS),
disconn_(),
connlost_(),
util("JTPMGR_CLASS")
{


	jtpApi_ = new ACS_JTP_Conversation();
    memset(&sendToJtp_, 0, sizeof(sendToJtp_));
	disconn_.lasttime = 0;
	disconn_.count = 1;
	connlost_.lasttime = 0;
	connlost_.count = 1;
}


//========================================================================================
//	Destructor
//========================================================================================
acs_alh_jtpmgr::~acs_alh_jtpmgr()
{

	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::~acs_alh_jtpmgr - enter");

    // Disconnect to JTP
    handleShutDownState();

    delete jtpApi_;
    delete pNode;

	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::~acs_alh_jtpmgr - exit");
}


//========================================================================================
//	Get status
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::getStatus(short alhStatus, unsigned short* cpAlarmRef)
{

	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::getStatus - enter ");
	//util.trace("acs_alh_jtpmgr::getStatus - enter ");


	//Call logging function
	log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::getStatus - alhStatus %d, cpAlarmRef = %d", alhStatus, *cpAlarmRef);

	switch (jtpMgrState_)
    {
		// ALH disconnected. Try to connect
		case DISC:
			if (handleDiscState() == JtpMgr_OK)
			{
				reportAllowed_ = true;
				break;
			}
			return JtpMgr_DISC;

		// SHUTDOWN state. Disconnect must be done before next connect.
		case SHUTDOWN:
			handleShutDownState();
			return JtpMgr_DISC;
		default:
			break;
    }

    // Line is in an intermediate state where DATA MSG can be expected
    // Therefore do a tentative read
    switch (receiveMsg(JTP_NO_BLOCK, &getFrJtp_))
    {
		// DATA received. Handle it according to content and state of JtpMgr
		case JTP_OK:
			reportAllowed_ = true;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: JTP_OK");
			return handleDataReceived(alhStatus, cpAlarmRef);

		case JTP_NO_MESSAGE:
			reportAllowed_ = true;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: JTP_NO_MESSAGE");
			break;

		case JTP_SIG_INTR:
			reportEvent("object JtpMgr.getStatus\nJTP_SIG_INTR from JTP", ACS_ALH_JtpMgrBadReturnCodeProblem);
			jtpMgrState_ = SHUTDOWN;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: JTP_SIG_INTR");
			return JtpMgr_DISC;

		case JTP_CONN_LOST:
			// Don't want bursts of events, uabmaha
			if (checkTimeSinceLastEvent(connlost_, ACS_ALH_timelimit))
			{
				char EventPhrase[100] = {0};
				snprintf(EventPhrase, sizeof(EventPhrase) - 1, "object JtpMgr.getStatus\nJTP_CONN_LOST from JTP, Count = %lu", connlost_.count);
				reportEvent(EventPhrase, ACS_ALH_JtpMgrBadReturnCodeProblem);
				connlost_.count = 0;
			}
			jtpMgrState_ = SHUTDOWN;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: JTP_CONN_LOST");
			return JtpMgr_DISC;

		case JTP_NOT_CONN:
			reportEvent("object JtpMgr.getStatus\nJTP_NOT_CONN from JTP", ACS_ALH_JtpMgrBadReturnCodeProblem);
			jtpMgrState_ = SHUTDOWN;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: JTP_NOT_CONN");
			return JtpMgr_DISC;

		case JTP_PROT_FAULT:
			reportEvent("object JtpMgr.getStatus\nJTP_PROT_FAULT from JTP", ACS_ALH_JtpMgrBadReturnCodeProblem);
			sendToJtp_.msgType = JTP_JEXDISCREQ;
			sendToJtp_.jexdatareq.data1 = 0;
			sendToJtp_.jexdatareq.buflen = 0;
			sendToJtpBuffer();
			jtpMgrState_ = SHUTDOWN;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: JTP_PROT_FAULT");
			return JtpMgr_DISC;

		default:
			reportEvent("object JtpMgr.getStatus\nunspecified Ret Code from JTP", ACS_ALH_JtpMgrBadReturnCodeProblem);
			jtpMgrState_ = SHUTDOWN;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - Receive Message: DEFAULT");
			return JtpMgr_DISC;
    }

    //Call logging function
    log.write(LOG_LEVEL_INFO, "JTPMGR - Method: getStatus - jtpMgrState_:%d", jtpMgrState_);

    if (jtpMgrState_ == NORMAL || jtpMgrState_ == DATAREQ)
		return JtpMgr_NORMAL;
    else
		return JtpMgr_DISC;
}


//========================================================================================
//	Send data
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::sendData(unsigned char* buffer, short bufLen, short elemId, short retransInd)
{
	//Call trace function
	//util.trace("acs_alh_jtpmgr::sendData");

	// Prepare and send buffer to ALCO
	sendToJtp_.jexdatareq.data1 = elemId;
	sendToJtp_.jexdatareq.data2 = retransInd;
	sendToJtp_.jexdatareq.buflen = bufLen;
	sendToJtp_.jexdatareq.buffer = buffer;

	if (sendToJtpBuffer() == JtpMgr_OK)
	{
		jtpMgrState_ = DATAREQ;
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: sendData - JtpMgr_NORMAL");
		return JtpMgr_NORMAL;
	}

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: sendData - JtpMgr_DISC");

	return JtpMgr_DISC;
}


//========================================================================================
//	Fetch JTP Manager state
//========================================================================================
void acs_alh_jtpmgr::fetchJtpMgrState(short &jtpMgrStatus)
{
	// Fetch jtpMgrState
	jtpMgrStatus = jtpMgrState_;
}


//========================================================================================
//	Send to JTP buffer
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::sendToJtpBuffer()
{
	//Call trace function
	//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendToJtpBuffer - enter");

	//ADD FOR INCREMENT TRACE INFORMATION
	char traceBuf2[DEF_LOGSIZE] = {0};
	snprintf(traceBuf2, sizeof(traceBuf2) - 1, "msg to ALCO");
	formatTrace(traceBuf2, &sendToJtp_, 0);
	//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - %s", traceBuf2);
	//END

	switch (sendMsg(sendToJtp_))
	{
		case JTP_OK:
			reportAllowed_ = true;
			//Call logging function
			//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JTP_OK");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_OK");
			return JtpMgr_OK;

		case JTP_NOT_CONN:
			jtpMgrState_ = DISC;
			reportEvent("object JtpMgr.sendToJtpBuffer\nJTP_NOT_CONN from JTP",	ACS_ALH_JtpMgrBadReturnCodeProblem);
			//Call logging function
			log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::sendToJtpBuffer - JTP_NOT_CONN");
			//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			return JtpMgr_DISC;

		case JTP_CONN_LOST:
			jtpMgrState_ = SHUTDOWN;
			reportEvent("object JtpMgr.sendToJtpBuffer\nJTP_CONN_LOST from JTP", ACS_ALH_JtpMgrBadReturnCodeProblem);
			//Call logging function
			log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::sendToJtpBuffer - JTP_CONN_LOST");
			//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			return JtpMgr_DISC;

		case JTP_PROT_FAULT:
			jtpMgrState_ = SHUTDOWN;
			//Call logging function
			log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::sendToJtpBuffer - JTP_PROT_FAULT");
			//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			return JtpMgr_DISC;

		default:
			jtpMgrState_ = SHUTDOWN;
			reportEvent("object JtpMgr.sendToJtpBuffer\nUnspecified Ret Code from JTP",	ACS_ALH_JtpMgrBadReturnCodeProblem);
			//Call logging function
			log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::sendToJtpBuffer - DEFAULT");
			//util.trace("acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendToJtpBuffer - exit - Returning JtpMgr_DISC");
			return JtpMgr_DISC;
	}
}


//========================================================================================
//	Handle disconnect state
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::handleDiscState()
{

	//util.trace("acs_alh_jtpmgr::handleDiscState - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleDiscState - enter");

	// Prepare and send buffer to ALCO
	jtpMgrState_ = INITREQ;
	sendToJtp_.msgType = JTP_JEXINITREQ;
	sendToJtp_.jexinitreq.data1 = apNodeNumber_;
	sendToJtp_.jexinitreq.maxBufferSize = sizeof(struct AlaIniStruct);

	if (sendToJtpBuffer() == JtpMgr_DISC)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDiscState - exit - JtpMgr_DISC");
		//util.trace("acs_alh_jtpmgr::handleDiscState - exit - Returning JtpMgr_DISC");
		return JtpMgr_DISC;
	}


	//util.trace("acs_alh_jtpmgr::handleDiscState - exit - Returning JtpMgr_OK");
	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleDiscState - exit - Returning JtpMgr_OK");

	return JtpMgr_OK;
}


//========================================================================================
//	Handle shut down state
//========================================================================================
void acs_alh_jtpmgr::handleShutDownState()
{
	//Call trace function
	//util.trace("JTPMGR - Method: handleShutdownState");

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleShutdownState");

	delete jtpApi_;

	jtpApi_ = 0;
	jtpMgrState_ = DISC;
}


//========================================================================================
//	Handle init request state
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::handleInitreqState(short alhStatus)
{

    //util.trace("acs_alh_jtpmgr::handleInitreqState - enter");
    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState - enter");


	if (getFrJtp_.msgType != JTP_JEXINITCONF)
	{
		jtpMgrState_ = SHUTDOWN;
		//Call logging function
		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::handleInitreqState - jtpMgrState_ = SHUTDOWN");
	    //util.trace("acs_alh_jtpmgr::handleInitreqState - exit - Returning JtpMgr_DISC");
	    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState - exit - Returning JtpMgr_DISC");
		return JtpMgr_DISC;	// wait for timeout
	}


	// Prepare and send JEXDATAREQ (ALH_status) to ALCO
	sendToJtp_.msgType = JTP_JEXDATAREQ;

	//========================================================================================
	// ADD for TR HP54702
	//========================================================================================
	int flags[2] = {0, 0};
	int newFlags[2] = {0, 0};
	int ret_code = 0;
	int index = 0;
	ret_code = util.get_cp_alignment_flag(flags);

	if(strcmp(alcoName_, ALCOName_[0]) == 0){
		index = 0;
		//util.trace("acs_alh_jtpmgr::handleInitreqState Prepare message for ALCOEX. Index value %d.", index);
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState Prepare message for ALCOEX. Index value %d.", index);
	}else{
		index = 1;
		//util.trace("acs_alh_jtpmgr::handleInitreqState Prepare message for ALCOSB.");
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState Prepare message for ALCOSB.");
	}

	if(ret_code == 0){
		if( flags[index] == 1 ){
		    //util.trace("acs_alh_jtpmgr::handleInitreqState CP flag alignment for %s read. Value: %d ", ALCOName_[index], flags[index]);
		    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState CP alignment flag for %s read. Value: %d ", ALCOName_[index], flags[index]);

		    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState send to CP STS_RES indication");
		    sendToJtp_.jexdatareq.data1 = STS_RES;  // Because the CP is not yet aligned

		    do{
		    	newFlags[0] = flags[0];
		    	newFlags[1] = flags[1];
		    	newFlags[index] = 0;
		    	ret_code = util.set_cp_alignment_flag(newFlags);
				if (ret_code == 0){
				    //util.trace("acs_alh_jtpmgr::handleInitreqState CP flag alignment for %s set to 0", ALCOName_[index]);
				    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState CP flag alignment for %s set to 0", ALCOName_[index]);
				    break;
				}else{
				    //util.trace("acs_alh_jtpmgr::handleInitreqState CP flag alignment for %s set to 0 FAILED. Retry...", ALCOName_[index]);
				    log.write(LOG_LEVEL_ERROR, "acs_alh_jtpmgr::handleInitreqState CP flag alignment for %s set to 0 FAILED. Retry...", ALCOName_[index]);
				    sleep(1);
				}
		    }while(ret_code != 0);


		}else{
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState send to CP %s indication on ALHStatus: %d", ALCOName_[index], alhStatus);
			sendToJtp_.jexdatareq.data1 = alhStatus;  // Alarmlist reset if STS_RES
		}
	}else{
	    //util.trace("acs_alh_jtpmgr::handleInitreqState CP flag alignment read for %s failed", ALCOName_[index]);
	    log.write(LOG_LEVEL_ERROR, "acs_alh_jtpmgr::handleInitreqState CP flag alignment read for %s failed", ALCOName_[index]);

	    log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::handleInitreqState send to CP indication on ALHStatus: %d", alhStatus);

		sendToJtp_.jexdatareq.data1 = alhStatus;  // Alarmlist reset if STS_RES
	}
	//========================================================================================
	// END
	//========================================================================================

	sendToJtp_.jexdatareq.buflen = 0;

	if (sendToJtpBuffer() == JtpMgr_OK)
		jtpMgrState_ = STSREQ;


	//Call logging function
	log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::handleInitreqState - jtpMgrState_: %d", jtpMgrState_);

    //util.trace("acs_alh_jtpmgr::handleInitreqState - exit - Returning JtpMgr_DISC");
    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleInitreqState - exit - Returning JtpMgr_DISC");

	return JtpMgr_DISC;
}


//========================================================================================
//	Handle status request state
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::handleStatusreqState(short alhStatus)
{
	//Call trace function
	//util.trace("JTPMGR - Method: handleStatusReqState");

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleStatusReqState");

	if (getFrJtp_.msgType != JTP_JEXDATAIND)
	{
		jtpMgrState_ = SHUTDOWN;
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleStatusReqState - JtpMgr_DISC - jtpMgrState_ = SHUTDOWN");
		return JtpMgr_DISC;	// wait for timeout
	}

	alcoStatus_ = getFrJtp_.jexdataind.data1;

	if (!(alcoStatus_ == STS_RES && alhStatus == STS_UNC))
	{
		jtpMgrState_ = NORMAL;
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleStatusReqState - JtpMgr_NORMAL - jtpMgrState_ = NORMAL");
		return JtpMgr_NORMAL;
	}

	// Prepare and send UPD_INI to ALCO
	sendToJtp_.msgType = JTP_JEXDATAREQ;
	sendToJtp_.jexdatareq.data1 = UPD_INI;
	sendToJtp_.jexdatareq.buflen = 0;

	if (sendToJtpBuffer() == JtpMgr_OK)
		jtpMgrState_ = UPDINI;

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleStatusReqState - JtpMgr_RES - jtpMgrState_ = UPDINI");

	return JtpMgr_RES;
}


//========================================================================================
//	Handle updin state
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::handleUpdiniState()
{
	//Call trace function
	//util.trace("JTPMGR - Method: handleUpdiniState");

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleUpdiniState");

	if (getFrJtp_.msgType == JTP_JEXDATAIND)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleUpdiniState - JTP_JEXDATAIND");
		alcoStatus_ = getFrJtp_.jexdataind.data1;
		if (alcoStatus_ == UPD_ACK)
		{
			jtpMgrState_ = NORMAL;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleUpdiniState - JtpMgr_NORMAL - jtpMgrState_ = NORMAL");
			return JtpMgr_NORMAL;
		}
	}

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleUpdiniState - JtpMgr_DISC - jtpMgrState_ = NORMAL");

	return JtpMgr_DISC;
}


//========================================================================================
//	Handle data request state
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::handleDatareqState(unsigned short* cpAlarmRef)
{
	//Call trace function
	//util.trace("JTPMGR - Method: handleDatareqState");

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDatareqState");

	if (getFrJtp_.msgType == JTP_JEXDATAIND)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDatareqState - JTP_JEXDATAIND");
		alcoStatus_ = getFrJtp_.jexdataind.data1;
		if (alcoStatus_ == ALA_ACK)
		{
			*cpAlarmRef = getFrJtp_.jexdataind.data2;
			jtpMgrState_ = NORMAL;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDatareqState - jtpMgrState_ = NORMAL - JtpMgr_DATA_REC");
			return JtpMgr_DATA_REC;
		}
	}

	//Call logging function
	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDatareqState - jtpMgrState_ = NORMAL - JtpMgr_NORMAL");

	return JtpMgr_NORMAL;
}


//========================================================================================
//	Report event
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::reportEvent(const char problemData[], long specificProblem)
{
    if (reportAllowed_)
    {
		eventMgr_.sendEvent(specificProblem, STRING_EVENT, alcoName_, problemData, ACS_ALH_Text_JtpMgr_problem);
		reportAllowed_ = false;
    }

    return JtpMgr_OK;
}


//========================================================================================
//	Check time since last event
//========================================================================================
int acs_alh_jtpmgr::checkTimeSinceLastEvent(JTP_event &str, time_t limit)
{
	//Call logging function
	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::checkTimeSinceLastEvent - enter");

	time_t currtime = time(0);
	//cout << "currtime=" << currtime << endl;
	//cout << "lasttime=" << str.lasttime << endl;

	time_t t = static_cast<time_t>(difftime(currtime, str.lasttime));
	//cout << "t0=" << t << endl;

	if (t < 0 || t > limit+10 || str.lasttime == 0)
	{
		// Probable some change of daylight saving time
		// or much time has past since lasttime
		// or first new event session occurred
		// (seems unnecessary since ->t>125)
		str.lasttime = currtime;
		t = 0;
	}

	//cout << "t=" << t << endl;
	log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::checkTimeSinceLastEvent - time t %d", t);

	if (t == 0 || (t >= limit && t <= limit + 10))
	{
		//cout << "t=" << t << endl;
		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::checkTimeSinceLastEvent - time t %d", t);

		// It has been ~limit minutes since last event was sent
		if (t >= limit)
		{
			str.lasttime = currtime-1;
		}
		// To avoid diff==0->will send 2 events at same time
		else if (t == 0)
		{
			--str.lasttime;
		}
		// To avoid sending more than 1 event at this specific moment
		//cout << "returning 1" << endl;
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::checkTimeSinceLastEvent - exit - Returning 1");

		return 1;
	}

	//cout << "t=" << t << endl;
	log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::checkTimeSinceLastEvent - time t %d", t);

	++str.count;

	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::checkTimeSinceLastEvent - exit - Returning 0");
	return 0;
}


//========================================================================================
//	Handle data received
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::handleDataReceived(short alhStatus, unsigned short* cpAlarmRef)
{

    //util.trace("acs_alh_jtpmgr::handleDataReceived - enter");
    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleDataReceived - enter");

	//Call trace function
	// JEXDISCIND can arrive anytime. Check if now.
    char traceBuf[DEF_LOGSIZE] = {0};
	snprintf(traceBuf, sizeof(traceBuf) - 1, "msg from JTP_API");
	//util.trace("acs_alh_jtpmgr::handleDataReceived - traceBuf: %s", traceBuf);

	//ADD FOR INCREMENT TRACE INFORMATION
	char traceBuf2[DEF_LOGSIZE] = {0};
	snprintf(traceBuf2, sizeof(traceBuf2) - 1, "msg from ALCO");
	formatTrace(traceBuf2, 0, &getFrJtp_);
	//util.trace("acs_alh_jtpmgr::handleDataReceived - getStatus %s", traceBuf2);
	//END

	//Call logging function
	log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::handleDataReceived - traceBuf: %s", traceBuf);

    if (getFrJtp_.msgType == JTP_JEXDISCIND)
    {
    	//Call logging function
    	log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDataReceived - JTP_JEXDISCIND");
		if (checkTimeSinceLastEvent(disconn_, ACS_ALH_timelimit))
		{
			char tmp[DEF_LOGSIZE] = {0};
			snprintf(tmp, sizeof(tmp) - 1, "object JtpMgr.getStatus\nJEXDISCIND from JTP, reason = %d, count = %lu", tmpreason, disconn_.count);
			tmpreason = -1;
			reportEvent(tmp, ACS_ALH_JtpMgrDiscRecProblem);
			disconn_.count = 0;
		}

		sleep(1);	// Sleep a while and wait for disconnect problem to go away

		//Call trace function
		//util.trace("JTPMGR - Method: handleDataReceived - JEXDISCIND received");

		jtpMgrState_ = SHUTDOWN;
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDataReceived - jtpMgrState_ = SHUTDOWN - JtpMgr_DISC");
		return JtpMgr_DISC;
    }

    //Call logging function
    log.write(LOG_LEVEL_INFO, "JTPMGR - Method: handleDataReceived - jtpMgrState_:%d - JtpMgr_NORMAL", jtpMgrState_);

    switch (jtpMgrState_)
    {
		case INITREQ:
			return handleInitreqState(alhStatus);

		case STSREQ:
			return handleStatusreqState(alhStatus);

		case UPDINI:
			return handleUpdiniState();

		case DATAREQ:
			return handleDatareqState(cpAlarmRef);

		default:	// Could only be NORMAL
			break;
    }

    //util.trace("acs_alh_jtpmgr::handleDataReceived - exit - Returning JtpMgr_NORMAL");
    log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::handleDataReceived - exit - Returning JtpMgr_NORMAL");
	return JtpMgr_NORMAL;
}


//========================================================================================
//	Check ALCO connection
//========================================================================================
JtpMgrReturnType acs_alh_jtpmgr::checkALCOConnection()
{
	int status = 0;
    unsigned short cpAlarmRef;

    //util.trace("acs_alh_jtpmgr::checkALCOConnection - enter");
    log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::checkALCOConnection - enter");

    // Send connection check request to ALCO.
    if (jtpMgrState_ == NORMAL)
	{
    	sendToJtp_.msgType = JTP_JEXDATAREQ;
		sendToJtp_.jexdatareq.data1 = STS_UNC;
		sendToJtp_.jexdatareq.buflen = 0;

		if (sendToJtpBuffer() == JtpMgr_OK)
		{
			jtpMgrState_ = STSREQ;
			// Get response.
			getStatus(STS_UNC, &cpAlarmRef);
			status = 1;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: checkALCOConnection - jtpMgrState_ = STSREQ");
		}
		else
		{
			jtpMgrState_ = DISC;
			status = 0;
			//Call logging function
			log.write(LOG_LEVEL_INFO, "JTPMGR - Method: checkALCOConnection - jtpMgrState_ = DISC");
		}
    }

    //Call trace function
     //util.trace("acs_alh_jtpmgr::checkALCOConnection - exit - Returning %d", status);

     //Call logging function
     log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::checkALCOConnection - exit - Returning %d", status);

    // Return status.
    return(status);
}


//========================================================================================
//	Receive Message
//========================================================================================
JTP_ReturnType acs_alh_jtpmgr::receiveMsg(const JTP_BlockType blockFlag, JTP_Msg* pmsg)
{
	//Call logging function
	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - enter");
	//util.trace("acs_alh_jtpmgr::receiveMsg - enter");

	if (!jtpApi_){
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_CONN_LOST");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_CONN_LOST");
		//END

		return JTP_CONN_LOST;
	}
	// First check if an initconf message pending
	// Can not do poll here because the message is already cashed in the buffer.
	if (jtpApi_->State() == ACS_JTP_Conversation::StateConnected)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::receiveMsg - ACS_JTP_Conversation::StateConnected");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::receiveMsg - ACS_JTP_Conversation::StateConnected");
		//END

		unsigned short u1;
		unsigned short u2;
		unsigned short r;

		if (jtpApi_->jexinitconf(u1, u2, r) == false)
		{
			delete jtpApi_;
			jtpApi_ = 0;

			//ADD to trace TR HP2715
			//util.trace("acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_CONN_LOST");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_CONN_LOST");
			//END

			return JTP_CONN_LOST;
		}

		//util.trace("acs_alh_jtpmgr::receiveMsg - message type: JTP_JEXINITCONF, protocol element identifier: %d, %s", u1, ALH_STATUS_[u1]);
		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - message type: JTP_JEXINITCONF, protocol element identifier: %d, %s", u1, ALH_STATUS_[u1]);

		pmsg->msgType = JTP_JEXINITCONF;
		pmsg->jexinitconf.data1 = u1;
		pmsg->jexinitconf.data2 = u2;
		pmsg->jexinitconf.resultCode = r;
		//Call logging function
		//util.trace("acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_OK");
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_OK");
		return JTP_OK;
	}

	if ((jtpApi_->State() == ACS_JTP_Conversation::StateWaitForData) ||	(blockFlag == JTP_NO_BLOCK))
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: receiveMsg - ACS_JTP_Conversation::StateWaitForData");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::receiveMsg - ACS_JTP_Conversation::StateWaitForData");
		//END

		int conversationState = 0;



		//================================================================================================
        //Create a FD set.
		JTP_HANDLE fd = jtpApi_->getHandle();
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        fd_set readHandleSet;
        FD_ZERO(&readHandleSet);
        FD_SET (fd, &readHandleSet);

    	//ADD to trace TR HP2715
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - FD: %d", (int)(fd+1));
		//util.trace("acs_alh_jtpmgr::receiveMsg - FD: %d", (int)(fd+1));
		//END

        int ret = select ((int)(fd+1), &readHandleSet, NULL, NULL, &tv);

        //printf("value of select: %d\n", ret);

    	//ADD to trace TR HP2715
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - return of select: %d", ret);
		//util.trace("acs_alh_jtpmgr::receiveMsg  - return of select: %d", ret);
		//END

        conversationState = jtpApi_->State();
		//util.trace("acs_alh_jtpmgr::receiveMsg  - state of the conversation: %d", conversationState );
		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - state of the conversation: %d", conversationState );

        if(ret < 0){ //select FAILED
    		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_SIG_INTR");
    		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_SIG_INTR");
        	return JTP_SIG_INTR;
        }

        if(blockFlag == JTP_NO_BLOCK){
        	if(ret == 0 ){ //select timeout
        		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_NO_MESSAGE");
        		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_NO_MESSAGE");
        		return JTP_NO_MESSAGE;
        	}

//    		//ADD to trace TR HP2715
//    		conversationState = jtpApi_->State();
//    		if(conversationState == ACS_JTP_Conversation::StateJobRunning){
//    			util.trace("acs_alh_jtpmgr::receiveMsg  - state of the conversation: %d", conversationState );
//    			log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - state of the conversation: %d", conversationState );
//        		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_NO_MESSAGE");
//        		util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_NO_MESSAGE");
//        		return JTP_NO_MESSAGE;
//    		}
//    		//END



        	conversationState = jtpApi_->State();
    		//util.trace("acs_alh_jtpmgr::receiveMsg  - state of the conversation: %d", conversationState );
    		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - state of the conversation: %d", conversationState );
    		//sleep(1);
        	if(jtpApi_->State() != ACS_JTP_Conversation::StateWaitForData){
        		conversationState = jtpApi_->State();
        		//util.trace("acs_alh_jtpmgr::receiveMsg  - state of the conversation: %d", conversationState);
        		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - state of the conversation: %d", conversationState );
        		delete jtpApi_;
        		jtpApi_ = 0;
        		//printf("ret: %d, JTP_CONN_LOST\n", ret);
        		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - JTP_CONN_LOST");
        		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - exit - JTP_CONN_LOST");
        		return JTP_CONN_LOST;
        	}
        }

		//=============================================================================================

        //printf("Ricevuto messaggio da ALCO\n");


		unsigned short u1;
		unsigned short u2;
		unsigned short reason;
		unsigned short len;
		char* buf;

		if (jtpApi_->jexdataind(u1, u2, len, buf) == false)
		{
			if (jtpApi_->jexdiscind(u1, u2, reason) == false)
			{
				delete jtpApi_;
				jtpApi_ = 0;
        		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_CONN_LOST");
        		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_CONN_LOST");
				return JTP_CONN_LOST;
			}
			else
			{

				//util.trace("acs_alh_jtpmgr::receiveMsg - message type: JTP_JEXDISCIND, protocol element identifier: %d, %s\n", u1, ALH_STATUS_[u1]);
				log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg -  message type: JTP_JEXDISCIND, protocol element identifier: %d, %s", u1, ALH_STATUS_[u1]);

//				printf("Stiamo dentro if ramo else\n");
//				printf("U1 value: %d - %s\n", u1, ALH_STATUS_[u1]);
//				printf("U2 value: %d\n", u2);

				pmsg->msgType = JTP_JEXDISCIND;
				pmsg->jexdiscind.data1 = u1;
				pmsg->jexdiscind.data2 = u2;
				pmsg->jexdiscind.reasonCode  = reason;
				tmpreason = reason;
				delete jtpApi_;
				jtpApi_ = 0;

        		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_OK");
        		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_OK");

				return JTP_OK;
			}
		}

		//util.trace("acs_alh_jtpmgr::receiveMsg - message type: JTP_JEXDATAIND, protocol element identifier: %d, %s\n", u1, ALH_STATUS_[u1]);
		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::receiveMsg - message type: JTP_JEXDATAIND, protocol element identifier: %d, %s", u1, ALH_STATUS_[u1]);

//		printf("Stiamo fuori if\n");
//		printf("U1 value: %d - %s\n", u1, ALH_STATUS_[u1]);
//		printf("U2 value: %d\n", u2);

		pmsg->msgType = JTP_JEXDATAIND;
		pmsg->jexdataind.data1 = u1;
		pmsg->jexdataind.data2 = u2;
		pmsg->jexdataind.buflen  = len;
		pmsg->jexdataind.buffer = (unsigned char*)buf;

		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_OK");
		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_OK");

		return JTP_OK;
	}
	else
	{
		delete jtpApi_;
		jtpApi_ = 0;

		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::receiveMsg - exit - Returning JTP_CONN_LOST");
		//util.trace("acs_alh_jtpmgr::receiveMsg  - exit - Returning JTP_CONN_LOST");

		return JTP_CONN_LOST;
	}
}


//========================================================================================
//	Send message
//========================================================================================
JTP_ReturnType acs_alh_jtpmgr::sendMsg(const JTP_Msg& msg)
{

	//util.trace("acs_alh_jtpmgr::sendMsg - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - enter");

	if (msg.msgType == JTP_JEXINITREQ)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::sendMsg - msg.msgType = JTP_JEXINITREQ");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::sendMsg - msg.msgType = JTP_JEXINITREQ");
		//END

		delete jtpApi_;

		jtpApi_ = new (std::nothrow) ACS_JTP_Conversation(const_cast<char*>(alcoName_), msg.jexinitreq.maxBufferSize);

		/*Added to manage memory error of allocation*/
		if (jtpApi_ == 0 ){
			log.write(LOG_LEVEL_ERROR, "acs_alh_jtpmgr::sendMsg - Memory error allocating object: ACS_JTP_Conversation jtpApi_ !");
			log.write(LOG_LEVEL_ERROR, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			return JTP_CONN_LOST;
		}


		ACS_JTP_Conversation::JTP_Node pNode_new;

		pNode_new.system_id = ACS_JTP_Conversation::SYSTEM_ID_CP_ALARM_MASTER;
		pNode_new.node_state = NodeSt;

		//Call logging function
		log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::sendMsg - serv: %s - state= %d", alcoName_, pNode_new.node_state);

		if (jtpApi_->jexinitreq(&pNode_new, msg.jexinitreq.data1, msg.jexinitreq.data2) == false){

			delete jtpApi_;
			jtpApi_ = 0;

			//ADD to trace TR HP2715
			//util.trace("acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			//END

			return JTP_CONN_LOST;
		}

		//util.trace("acs_alh_jtpmgr::sendMsg - exit - - Returning JTP_OK");
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_OK");

		return JTP_OK;
	}
	else if (msg.msgType == JTP_JEXDATAREQ)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "acs_alh_jtpmgr::sendMsg - message type: JTP_JEXDATAREQ");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::sendMsg - msg.msgType = JTP_JEXDATAREQ");
		//END

		if (!jtpApi_){
			//ADD to trace TR HP2715
			//util.trace("acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			//END
			return JTP_CONN_LOST;
		}

		if (jtpApi_->jexdatareq(msg.jexdatareq.data1, msg.jexdatareq.data2, msg.jexdatareq.buflen, (char*)msg.jexdatareq.buffer) == false)
		{
			delete jtpApi_;
			jtpApi_ = 0;
			return JTP_CONN_LOST;
		}

		//util.trace("acs_alh_jtpmgr::sendMsg - message type: JTP_JEXDATAREQ, protocol element identifier: %d, %s\n", msg.jexdatareq.data1, ALH_STATUS_[msg.jexdatareq.data1]);
		log.write(LOG_LEVEL_DEBUG, "acs_alh_jtpmgr::sendMsg - message type: JTP_JEXDATAREQ, protocol element identifier: %d, %s", msg.jexdatareq.data1, ALH_STATUS_[msg.jexdatareq.data1]);

		//util.trace("acs_alh_jtpmgr::sendMsg - exit - Returning JTP_OK");
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_OK");
		return JTP_OK;

	}
	else if (msg.msgType == JTP_JEXDISCREQ)
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: sendMsg - msg.msgType = JTP_JEXDISCREQ");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::sendMsg - msg.msgType = JTP_JEXDISCREQ");
		//END

		if (!jtpApi_){
			//ADD to trace TR HP2715
			//util.trace("acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			//END
			return JTP_CONN_LOST;
		}

		if (jtpApi_->jexdiscreq(msg.jexdiscreq.data1, msg.jexdiscreq.data2,	msg.jexdiscreq.reasonCode) == false)
		{
			delete jtpApi_;
			jtpApi_ = 0;

			//ADD to trace TR HP2715
			//util.trace("acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_CONN_LOST");
			//END

			return JTP_CONN_LOST;
		}

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::sendMsg - exit - Returning JTP_OK");
		log.write(LOG_LEVEL_TRACE, "acs_alh_jtpmgr::sendMsg - exit - Returning JTP_OK");
		//END

		return JTP_OK;
	}
	else
	{
		//Call logging function
		log.write(LOG_LEVEL_INFO, "JTPMGR - Method: sendMsg - msg.msgType = JTP_PROT_FAULT");

		//ADD to trace TR HP2715
		//util.trace("acs_alh_jtpmgr::sendMsg - msg.msgType = JTP_PROT_FAULT");
		//END

		return JTP_PROT_FAULT;
	}
}


//========================================================================================
//	Format trace
//========================================================================================

void acs_alh_jtpmgr::formatTrace(char* traceBuf,
							JTP_Msg* ptrMsgSend,
							JTP_Msg* ptrMsgRec)
{
	int k = 0;
	int l = 0;

	const char *states[] =
	{
		"DISC",
		"INITREQ",
		"STSREQ",
		"UPDINI",
		"NORMAL",
		"DATAREQ",
		"SHUTDOWN"
	};

	const char *msgTypes[] =
	{
		"JTP_JEXINITREQ",
		"JTP_JEXINITIND",
		"JTP_JEXINITRSP",
		"JTP_JEXINITCONF",
		"JTP_JEXDATAREQ",
		"JTP_JEXDATAIND",
		"JTP_JEXDISCREQ",
		"JTP_JEXDISCIND"
	};

	//printf("SONO IN FORMAT TRACE: \n");

	sprintf(traceBuf, "\n\tjtpMgrState: \t%s\n", states[jtpMgrState_]);
	if (ptrMsgSend)
	{
		strcat(traceBuf, "\tJTP_Msg Send buffer to JTP_API:\n");
		sprintf(traceBuf, "%s\t\thandle:\t%d\n",
			traceBuf, ptrMsgSend->handle);
		sprintf(traceBuf, "%s\t\tmsgType:\t%s\n",
			traceBuf, msgTypes[ptrMsgSend->msgType]);

		//printf("TIPO MESSAGGIO: %d\n", ptrMsgSend->msgType);

		if(ptrMsgSend->msgType == JTP_JEXINITREQ)  {
			//trace something?
			sprintf(traceBuf, "%s\t\tnodeNr:\t%d\n",
			traceBuf, ptrMsgSend->jexinitreq.data1);
		}
		else {
			k = ptrMsgSend->jexdatareq.data1;
			sprintf(traceBuf, "%s\t\t\tdata1:\t[HB]%d\t[LB]%d\t(k=%d)\n", traceBuf,
				(ptrMsgSend->jexdatareq.data1)>>8,
				(ptrMsgSend->jexdatareq.data1)&0x0ff,
				k);
			sprintf(traceBuf, "%s\t\t\tdata2:\t[HB]%d\t[LB]%d\n", traceBuf,
				(ptrMsgSend->jexdatareq.data2)>>8,
				(ptrMsgSend->jexdatareq.data2)&0xff);
			if (k == ALA_INI)
			{
				sprintf(traceBuf, "%s\t\t\tbuflen:\t%d\n",
					traceBuf, ptrMsgSend->jexdatareq.buflen);
				sprintf(traceBuf, "%s\t\t\t\tAP NODE:\t%d, %d\n", traceBuf,
					ptrMsgSend->jexdatareq.buffer[0], ptrMsgSend->jexdatareq.buffer[1]);
				strcat(traceBuf, "\t\t\t\tALARM ID:\t");
				for(l = 0; l < 3; l++)
				{
					sprintf(traceBuf,"%s[%d, %d] ", traceBuf,
						ptrMsgSend->jexdatareq.buffer[2+2*(2-l)+1],
						ptrMsgSend->jexdatareq.buffer[2+2*(2-l)]);
				}
				strcat(traceBuf, "\n");
				sprintf(traceBuf, "%s\t\t\t\tALARM CLASS:\t%d\n",
					traceBuf, ptrMsgSend->jexdatareq.buffer[8]);
				sprintf(traceBuf, "%s\t\t\t\tALARM CAT:\t%d\n",
					traceBuf, ptrMsgSend->jexdatareq.buffer[9]);
				sprintf(traceBuf, "%s\t\t\t\tSIZE OF BUF:\t%d, %d\n", traceBuf,
					ptrMsgSend->jexdatareq.buffer[10], ptrMsgSend->jexdatareq.buffer[11]);
				sprintf(traceBuf, "%s\t\t\t\tBUFFER:\t%s\n",
					traceBuf, &ptrMsgSend->jexdatareq.buffer[12]);
			}
			if (k == ALA_CSG)
			{
				sprintf(traceBuf, "%s\t\t\tbuflen:\t%d\n",
					traceBuf, ptrMsgSend->jexdatareq.buflen);
				sprintf(traceBuf, "%s\t\t\t\tAP NODE:\t%d, %d\n",
					traceBuf, ptrMsgSend->jexdatareq.buffer[0],
					ptrMsgSend->jexdatareq.buffer[1]);
				strcat(traceBuf, "\t\t\t\tALARM ID:\t");
				for(l = 0; l < 3; l++)
				{
					sprintf(traceBuf,"%s[%d, %d] ",
						traceBuf, ptrMsgSend->jexdatareq.buffer[2+2*(2-l)+1],
						ptrMsgSend->jexdatareq.buffer[2+2*(2-l)]);
				}
				strcat(traceBuf, "\n");
				sprintf(traceBuf, "%s\t\t\t\tCP ALARM REF:\t%d, %d\n", traceBuf,
					ptrMsgSend->jexdatareq.buffer[9], ptrMsgSend->jexdatareq.buffer[8]);
			}
		}
	}

	if (ptrMsgRec)
	{
		strcat(traceBuf, "\tJTP_Msg Received buffer from JTP_API:\n");
		sprintf(traceBuf, "%s\t\thandle:\t%d\n",
			traceBuf, ptrMsgRec->handle);
		sprintf(traceBuf, "%s\t\tmsgType:\t%s\n",
			traceBuf, msgTypes[ptrMsgRec->msgType]);
		k = ptrMsgRec->jexdatareq.data1;
		sprintf(traceBuf, "%s\t\t\tdata1:\t[HB]%d\t[LB]%d\t(k=%d)\n", traceBuf,
			(ptrMsgRec->jexdatareq.data1)>>8,
			(ptrMsgRec->jexdatareq.data1)&&0xff,
			k);
		sprintf(traceBuf, "%s\t\t\tdata2:\t[HB]%d\t[LB]%d\n", traceBuf,
			(ptrMsgRec->jexdatareq.data2)>>8,
			(ptrMsgRec->jexdatareq.data2)&&0xff);
	}
}

