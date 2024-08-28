//========================================================================================
/** @file  acs_aeh_evreportimpl.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-09
 *	@version 0.9.1
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
 *	| P0.9.1 | 2011-05-09 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2011-05-13 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-27 | xfabron      | Released for ITSTEAM2 sprint 13     |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 * SyntaxCheck class, Error base class
 *
 */
//========================================================================================

#include "acs_aeh_communicationhandler.h"
#include "acs_aeh_evreportimpl.h"
#include "acs_aeh_exception.h"
#include "acs_aeh_types.h"

#include <acs_prc_api.h>

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include "ace/OS.h"


#ifndef ACS_AEH_INTERVAL
#define ACS_AEH_INTERVAL	5
#endif

namespace ACS_AEH {

std::vector<eventData> acs_aeh_evreportimpl::eventVector;


// The allowed values for perceived severity
const char A1_[] = 		"A1";
const char A2_[] = 		"A2";
const char A3_[] = 		"A3";
const char O1_[] = 		"O1";
const char O2_[] = 		"O2";
const char Ceasing_[] =	"CEASING";
const char Event_[] =	"EVENT";


// The allowed values for object class of reference
const char APZ_[] =		"APZ";
const char APT_[] =		"APT";
const char PWR_[] =		"PWR";
const char EXT_[] =		"EXT";
// Note. for "EVENT", see above


// Event parameters
parameter param[] = {
	{"process name",				PROCESS_NAME_MAX_LEN},
	{"specific problem",			0},
	{"perceived severity",			PERC_SEVERITY_MAX_LEN},
	{"probable cause",				PROBABLE_CAUSE_MAX_LEN},
	{"object class of reference",	OBJ_CLASS_OF_REF_MAX_LEN},
	{"object of reference",			OBJECT_OF_REF_MAX_LEN},
	{"problem data",				PROBLEM_DATA_MAX_LEN},
	{"problem text",				PROBLEM_TEXT_MAX_LEN},
	{"manual cease",				0}
};


enum parindex_t {
	PROCESSNAME,
	SPECIFICPROBLEM,
	PERCSEVERITY,
	PROBABLECAUSE,
	OBJCLASSOFREFERENCE,
	OBJECTOFREFERENCE,
	PROBLEMDATA,
	PROBLEMTEXT,
	MANUALCEASE
};

//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_evreportimpl::acs_aeh_evreportimpl()
{
}


//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_evreportimpl::~acs_aeh_evreportimpl()
{
}


//========================================================================================
//	Check string for valid characters
//========================================================================================

void acs_aeh_evreportimpl::checkParameter(parameter &par, std::string p_paramValue)const{

	int paramSize = p_paramValue.length();

	if( paramSize >= par.maxsize){
		acs_aeh_exception ex(PARAMERROR);
		ex()<<"Max length exceeded for parameter '"<<par.name<<"'";
		throw ex;
	}

	for(int i=0; i<paramSize; i++){
		char c = p_paramValue.c_str()[i];
		if( !isascii(c)){
			//Not an ascii character
			acs_aeh_exception ex(PARAMERROR);
			ex()<<"Non ascii character found in parameter '"<<par.name<<"'";
			throw ex;
		}else if (!isprint(c) && !isspace(c)){
			//Not a printable character or a white space
			acs_aeh_exception ex(PARAMERROR);
			ex() << "Illegal ascii charcter (0x" << std::hex << int(c)
				 << ") found in parameter '" << par.name << "'";
			throw ex;
		}
	}

}

//========================================================================================
//	Check Severity value
//========================================================================================

void acs_aeh_evreportimpl::checkSeverity(std::string p_severity, acs_aeh_eventmsg &msg)const{

	if( strcmp(p_severity.c_str(), A1_ ) == 0){
		msg.setPriorityMsg("error");
		return;
	}else if ( strcmp(p_severity.c_str(), A2_) == 0 ){
		msg.setPriorityMsg("error");
		return;
	}else if( strcmp(p_severity.c_str(), A3_) == 0 ){
		msg.setPriorityMsg("error");
		return;
	}else if ( strcmp(p_severity.c_str(), O1_) == 0 ){
		msg.setPriorityMsg("error");
		return;
	}else if ( strcmp(p_severity.c_str(), O2_) == 0 ){
		msg.setPriorityMsg("error");
		return;
	}else if ( strcmp(p_severity.c_str(), Ceasing_) == 0 ){
		msg.setPriorityMsg("error");
		return;
	}else if ( strcmp(p_severity.c_str(), Event_) == 0 ){
		msg.setPriorityMsg("warning");
		return;
	}else{
		acs_aeh_exception ex(PARAMERROR);
		ex() << "Illegal value for parameter 'perceived severity" << "'";
		throw ex;
	}
}

//========================================================================================
//	Check Class of Reference value
//========================================================================================

void acs_aeh_evreportimpl::checkClassOfReference(std::string p_classOfReference)const{

	if( strcmp(p_classOfReference.c_str(), APZ_ ) == 0){
		return;
	}else if ( strcmp(p_classOfReference.c_str(), APT_) == 0 ){
		return;
	}else if( strcmp(p_classOfReference.c_str(), PWR_) == 0 ){
		return;
	}else if ( strcmp(p_classOfReference.c_str(), EXT_) == 0 ){
		return;
	}else if ( strcmp(p_classOfReference.c_str(), Event_) == 0 ){
		return;
	}else{
		acs_aeh_exception ex(PARAMERROR);
		ex() << "Illegal value for parameter 'object class of reference" << "'";
		throw ex;
	}
}


//========================================================================================
//	Check Class of Reference value
//========================================================================================

bool acs_aeh_evreportimpl::eventToSend(acs_aeh_eventmsg &msg) const{


	double elapsed_time = 0;
	time_t now;
	eventData newEvent;

	time( &now );

	if( eventVector.size() > 0 ){
		//Event present in the vector
		for(unsigned int i=0; i<eventVector.size(); i++){
			if(eventVector[i].event == msg){
				//The event is already present in the vector
				elapsed_time = difftime(now, eventVector[i].lastSent);
				//Check when the event is send
				if(elapsed_time < ACS_AEH_INTERVAL){

					return false;
				}else{
					//Event send more than 30 seconds ago
					time(&(eventVector[i].lastSent));
					return true;
				}
			}else{
				//Event doesn't match, test if the elapsed_time is > than 30 seconds
				elapsed_time = difftime(now, eventVector[i].lastSent);
				//Remove the event to vector
				if(elapsed_time > ACS_AEH_INTERVAL){
					eventVector.erase(eventVector.begin()+i);
					i--;
				}

			}
		}//end for
		//The Event doesn't present in the vector so add it to the vector
		time(&(newEvent.lastSent));
		newEvent.event=msg;
		eventVector.push_back(newEvent);
		return true;

	}else{
		//Vector is empty send the event and add it to the vector
		time(&(newEvent.lastSent));
		newEvent.event=msg;
		eventVector.push_back(newEvent);
		return true;
	}

	return true;

}

//========================================================================================
//	Send event message
//========================================================================================

void acs_aeh_evreportimpl::sendEventMessage(
						acs_aeh_processName processName,
						acs_aeh_specificProblem specificProblem,
						acs_aeh_percSeverity percSeverity,
						acs_aeh_probableCause probableCause,
						acs_aeh_objClassOfReference objClassOfReference,
						acs_aeh_objectOfReference objectOfReference,
						acs_aeh_problemData problemData,
						acs_aeh_problemText problemText,
						bool manualCease)const
{


	parindex_t index;
	bool send_event = true;
	int retVal = 0;
	acs_aeh_communicationhandler logEvent;

	acs_aeh_eventmsg eventMessage(processName, specificProblem, percSeverity, probableCause,
			objClassOfReference, objectOfReference, problemData, problemText,  manualCease);

	try{

		//Process name
		index = PROCESSNAME;

		if(eventMessage.getProcessName().empty()){
			acs_aeh_exception ex(PARAMERROR);
			ex() << "Parameter "<< "'"<<param[index].name <<"'"<< " must be initialized ";
			throw ex;
		}
		checkParameter(param[index], eventMessage.getProcessName());

		//Perceived severity
		index = PERCSEVERITY;
		checkParameter(param[index], eventMessage.getPercSeverity());
		checkSeverity(eventMessage.getPercSeverity(), eventMessage);

		// Probable cause
		index = PROBABLECAUSE;
		if (eventMessage.getProbableCause().empty()){
			eventMessage.setProbableCause("-");
		}
		checkParameter(param[index], eventMessage.getProbableCause());

		// Object class of reference
		index = OBJCLASSOFREFERENCE;
		checkParameter(param[index], eventMessage.getObjClassOfReference());
		checkClassOfReference(eventMessage.getObjClassOfReference());

		// Object of reference
		index = OBJECTOFREFERENCE;
		if (eventMessage.getObjectOfReference().empty()){
			eventMessage.setObjectOfReference("-");
		}
		checkParameter(param[index], eventMessage.getObjectOfReference());

		// problem data
		index = PROBLEMDATA;
		if (eventMessage.getProblemData().empty()){
			eventMessage.setProblemData("-");
		}
		checkParameter(param[index], eventMessage.getProblemData());

		// problem text
		index = PROBLEMTEXT;
		if (eventMessage.getProblemText().empty()){
			eventMessage.setProblemText("-");
		}
		checkParameter(param[index], eventMessage.getProblemText());

		// user name
		uid_t userUID;
		passwd *userInfo = 0;

		userUID = getuid();

		userInfo = getpwuid(userUID);
		if(userInfo == 0){
			eventMessage.setUserName("-");
		}else{
			eventMessage.setUserName(userInfo->pw_name);
		}

		//Node Name
//		char *nodeName = 0;
//		nodeName = getenv("HOST");

		char nodeName[32] = {0};
		gethostname(nodeName,sizeof(nodeName));

		if( nodeName == 0){
			eventMessage.setNodeName("-");
		}else{
			eventMessage.setNodeName(nodeName);
		}

		// Node Status
		ACS_PRC_API test;
		int nodeState;

		nodeState = test.askForNodeState();

		if ( nodeState <= 0 ){
			eventMessage.setNodeStatus("-");
		}else{
			switch(nodeState){
			case 1:
				eventMessage.setNodeStatus("ACTIVE");
				break;
			case 2:
				eventMessage.setNodeStatus("PASSIVE");
				break;
			}
		}

		if( strcmp(percSeverity, Event_) == 0 ){
			if(eventToSend(eventMessage) == false){
				send_event = false;
			}
		}


		if( send_event ){
			retVal = logEvent.open();
			if ( retVal == -1 ){
				acs_aeh_exception ex(SYSTEMERROR);
				ex() << "Open communication channel failure" << "'";
				throw ex;
			}

			retVal = logEvent.sendMsg(eventMessage);
			if ( retVal == -1 ){
				acs_aeh_exception ex(SYSTEMERROR);
				ex() << "Send event message failure" << "'";
				throw ex;
			}

			retVal = logEvent.close();
			if ( retVal == -1 ){
				acs_aeh_exception ex(SYSTEMERROR);
				ex() << "Close communication channel failure" << "'";
				throw ex;
			}
		}


	}catch(acs_aeh_exception &ex){

		std::stringstream s;
		s << ex;
		throw ex;

	}

}//end function sendEventMessage

};

