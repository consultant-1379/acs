/*
 * acs_alh_eventreader.cpp
 *
 *  Created on: Nov 4, 2011
 *      Author: efabron
 */


#include "acs_alh_eventreader.h"
#include "acs_alh_macroconfig.h"
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <iostream>


#include "ace/Condition_T.h"
#include "ace/Task.h"

char *ALCOName[] = {"ALCOEX", "ALCOSB"};

ACE_thread_t alog_read_work_thread_id = 0;

//========================================================================================
//	Constructors
//========================================================================================

acs_alh_eventreader::acs_alh_eventreader(acs_alh_imm_connection_handler *connHandler): util_("acs_alh_eventreader"), log_(), eventMgr_(){

	connHandler_ = connHandler;
	signalTermination_ = false;
	alh_event_reader_work_thread_id = 0;
	CP_state_ = 0;
}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_eventreader::~acs_alh_eventreader(){

}



//========================================================================================
// 	Get event thread
//========================================================================================

static ACE_THR_FUNC_RETURN getEventThread(void* mcp){

	//util_.trace("TRACE: getEventThread() entered");
	//log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.getEventThread() entered");


	if (((acs_alh_eventreader *)mcp)->getEvent() == -1)
	{

		//log_.write(LOG_LEVEL_TRACE, "getEventThread() mcp->getEvent returned -1.");
		//util_.trace("TRACE: getEventThread() mcp->getEvent returned -1.");

		return (ACE_THR_FUNC_RETURN)-1;
	}

	return (ACE_THR_FUNC_RETURN)0;
}




int acs_alh_eventreader::startEventReadThread()
{

	util_.trace("TRACE: startEventReadThread() entered");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.startEventReadThread() entered");

	/* spawn thread */
	const ACE_TCHAR* thread_name = "get_Event_Thread";

	int call_result = ACE_Thread_Manager::instance()->spawn(&getEventThread,
										(void *)this,
										THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
										&alh_event_reader_work_thread_id,
										0,
										ACE_DEFAULT_THREAD_PRIORITY,
										-1,
										0,
										ACE_DEFAULT_THREAD_STACKSIZE,
										&thread_name);
	if (call_result == -1)
	{

		log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.startEventReadThread() Start %s failure. Error code: %d",thread_name, call_result );
		log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.startEventReadThread() - exit - Returning -1!!!");
		util_.trace("TRACE: startEventReadThread() - exit - Returning -1!!!");
		return -1;

	}

	return 0;
}







int acs_alh_eventreader::getEvent(){

	util_.trace("TRACE: getEvent() enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.getEvent() enter");

	ACS_ALH_AlarmMessage eventMessage;
	char buffer[MAX_BUF_SIZE_ALMSG];
	std::string recordEvent;

	int countEvent = 0;

	FILE *pipe;

	//Open the pipe e return a pointer to the stream
	pipe = fopen(ALH_FIFO_NAME, "r");
	if( pipe == 0){
		log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.getEvent() Cannot open read only alhfifo. Returning -1");
		setError(acs_alh::ERR_SYSTEM_OPEN);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "object eventreader.getEvent problem\nCannot open read only alhfifo. Returning -1");
		setErrorText(problemData);
		return -1;
	}


	while (1) {

		if (signalTermination_) {
			util_.trace("TRACE: acs_alh_eventreader.getEvent() signal termination. Returning -1");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.getEvent() signal termination. Returning -1");
			return -1;
		}


		//Read new event message from alhpipe
		if ( fgets(buffer, sizeof(buffer), pipe) != 0 ){
			countEvent++;
			log_.write(LOG_LEVEL_DEBUG, "acs_alh_eventreader.getEvent(). Read new even message. Number of event message read: %d", countEvent);
		}else{
			log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.getEvent(). Cannot read event from alhfifo. Returning -1");
			setError(acs_alh::ERR_SYSTEM_READ);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "object eventreader.getEvent problem\nCannot read event from alhfifo. Returning -1");
			setErrorText(problemData);
			util_.trace("TRACE: acs_alh_eventreader.getEvent() exit - Cannot read event from alhfifo. Returning -1");
			return -1;
		}


		//TODO
		//controllare correttezza del buffer
		if(buffer != 0){

			recordEvent =  buffer;

			//Check if the recordEvent is an APEvent
			if (checkEventMessage(recordEvent)){
				log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.getEvent(). Read event: %s", recordEvent.c_str());

				getEventMessage(eventMessage, recordEvent);

				if (treatEvent(eventMessage) == false)
				{
					util_.trace("TRACE: acs_alh_eventreader.getEvent() treatEvent failed. Returning -1");
					log_.write(LOG_LEVEL_ERROR, "acs_alh_eventreader.getEvent() treatEvent failed. Returning -1");

					return -1;
				}

			}else{
				log_.write(LOG_LEVEL_WARN, "acs_alh_eventreader.getEvent(). Event discarded. It isn't an APEvent");
			}
		}

//		std::cout<<std::endl;
//		std::cout<<std::endl;
//
//		printf("EventTime: %s", ctime(&(eventMessage.eventTime)));
//		std::cout<<"eventType: "<<eventMessage.eventType<<std::endl;
//		std::cout<<"nodeName: "<<eventMessage.nodeName<<std::endl;
//		std::cout<<"nodeState: "<<eventMessage.nodeState<<std::endl;
//		std::cout<<"priority: "<<eventMessage.priority<<std::endl;
//		std::cout<<"user: "<<eventMessage.user<<std::endl;
//		std::cout<<"ProcessName: "<<eventMessage.event.processName<<std::endl;
//		std::cout<<"SpecificProblem: "<<eventMessage.event.specificProblem<<std::endl;
//		std::cout<<"Seveity: "<<eventMessage.event.percSeverity<<std::endl;
//		std::cout<<"ProbableCause: "<<eventMessage.event.probableCause<<std::endl;
//		std::cout<<"ObjClassOfRef: "<<eventMessage.event.objClassOfReference<<std::endl;
//		std::cout<<"ObjReference: "<<eventMessage.event.objectOfReference<<std::endl;
//		std::cout<<"ProblemData: "<<eventMessage.event.problemData<<std::endl;
//		std::cout<<"ProblemText: "<<eventMessage.event.problemText<<std::endl;;
//		std::cout<<"ManualCease: "<<eventMessage.manualCease<<std::endl;
//		std::cout<<std::endl;
//		std::cout<<std::endl;




	}//end while

	fclose(pipe);

	return 0;
}


//========================================================================================
//	getEventMessage method
//========================================================================================

void acs_alh_eventreader::getEventMessage(ACS_ALH_AlarmMessage& event_message, std::string record){

	std::string eventData;
	std::string eventTime;
	size_t pos;


	// eventTime
	pos = record.find(" ");
	eventData = record.substr(0, pos);

	record = record.substr(pos+1);
	pos = record.find(" ");
	eventTime = record.substr(0, pos);

	time_t timeEvent = convertEventTime(eventData, eventTime);
	if( timeEvent != -1 ){
		event_message.eventTime = timeEvent;
	}else{
		log_.write(LOG_LEVEL_WARN, "acs_alh_eventreader.getEventMessage(). Time conversions failed.");
	}


	//eventType
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.eventType, sizeof(event_message.eventType) - 1, "%s", record.substr(0, pos).c_str());


	// nodeName
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.nodeName, sizeof(event_message.nodeName) - 1, "%s", record.substr(0, pos).c_str());

	// nodeState
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.nodeState, sizeof(event_message.nodeState) - 1, "%s", record.substr(0, pos).c_str());

	// priority
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.priority, sizeof(event_message.priority) - 1, "%s", record.substr(0, pos).c_str());

	// user
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.user, sizeof(event_message.user) - 1, "%s", record.substr(0, pos).c_str());


	// Process Name
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.event.processName, sizeof(event_message.event.processName) - 1, "%s", record.substr(0, pos).c_str());


	// Specific Problem
	char tmp[1024] = {0};
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(tmp, sizeof(tmp) - 1, "%s", record.substr(0, pos).c_str());
	event_message.event.specificProblem = atoi(tmp);


	// Perceived Severity
	record = record.substr(pos+1);
	pos = record.find(" ");
	snprintf(event_message.event.percSeverity, sizeof(event_message.event.percSeverity) - 1, "%s", record.substr(0, pos).c_str());


	// Manual cease
	record = record.substr(pos+1);
	pos = record.find(" ");
	std::string cease = record.substr(0, pos);
	int value = atoi(cease.c_str());
	if(value > 0)
		event_message.manualCease = true;
	else
		event_message.manualCease = false;


	// Probable Cause
	record = record.substr(pos+10);
	pos = record.find("CLASS_REF");
	snprintf(event_message.event.probableCause, sizeof(event_message.event.probableCause) - 1, "%s", record.substr(0, pos-1).c_str());


	// Object Class of Reference
	record = record.substr(pos+11);
	pos = record.find("OBJ_REF");
	snprintf(event_message.event.objClassOfReference, sizeof(event_message.event.objClassOfReference) - 1, "%s", record.substr(0, pos-1).c_str());

	// Object of Reference
	record = record.substr(pos+9);
	pos = record.find("P_DATA");
	snprintf(event_message.event.objectOfReference, sizeof(event_message.event.objectOfReference) - 1, "%s", (record.substr(0, pos-1)).c_str());


	// Problem Data
	record = record.substr(pos+8);
	pos = record.find("P_TEXT");
	std::string p_data = record.substr(0, pos-1);
	p_data = getFormattedProblemDataText(p_data);
	snprintf(event_message.event.problemData, sizeof(event_message.event.problemData) - 1, "%s", p_data.c_str());


	// Problem Text
	std::string p_text = record.substr(pos+8);
	p_text = getFormattedProblemDataText(p_text);
	snprintf(event_message.event.problemText, sizeof(event_message.event.problemData) - 1, "%s", p_text.c_str());
	int len = strlen(event_message.event.problemText);
	if(event_message.event.problemText[len -1] == '\n'){
		event_message.event.problemText[len -1] = '\0';
	}


}


//-------------------------------------------------------------------------------------------------------------------------------------------------
//getFormattedProblemDataText method
//-------------------------------------------------------------------------------------------------------------------------------------------------

bool acs_alh_eventreader::checkEventMessage(std::string record){

	size_t found;

	found = record.find("APEvent");

	//Check that the "APEvent" tag is located in right position.
	if( (found != string::npos) && ((int)found == 20) )
		return true;

	else

		return false;
}



//-------------------------------------------------------------------------------------------------------------------------------------------------
//convertEventTime method
//-------------------------------------------------------------------------------------------------------------------------------------------------

time_t acs_alh_eventreader::convertEventTime(std::string eventData, std::string eventTime){

	struct tm when;

	int yy = 0;
	int mm = 0;
	int dd = 0;
	int hh = 0;
	int m = 0;
	int ss = 0;


	sscanf(eventData.c_str(),"%d-%d-%d", &yy, &mm, &dd);
	// years since 1900
	when.tm_year = yy - 1900;
	// months since January
	when.tm_mon = mm - 1;
	// day of the month
	when.tm_mday = dd;


	sscanf(eventTime.c_str(),"%d:%d:%d", &hh, &m, &ss);
	when.tm_sec = ss;
	when.tm_min = m;
	when.tm_hour = hh;

	when.tm_isdst = -1; // autodetect DST

	return mktime(&when);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//getFormattedProblemDataText method
//-------------------------------------------------------------------------------------------------------------------------------------------------
std::string acs_alh_eventreader::getFormattedProblemDataText(std::string p_text){
	std::string newProblem;

	newProblem = p_text;
	util_.myReplace(newProblem, "##", "\n");

//	char str[p_text.length()];
//	::sprintf(str, "%s",p_text.c_str());
//
//	char *pch = 0;
//	pch = strtok(str, "##");
//
//	while (pch != 0){
//
//		newProblem += pch;
//		pch = strtok(0, "##");
//		if(pch !=0 ){
//			newProblem += "\n";
//		}
//	}

	return newProblem;
}



//========================================================================================
//	Store the Alarm/Ceasing in the alarm-list and the corresponding alarm-list-file
//========================================================================================

bool  acs_alh_eventreader::treatEvent(ACS_ALH_AlarmMessage& alarmMessage)
{

	util_.trace("TRACE: acs_alh_eventreader::treatEvent() entered");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.treatEvent() entered");

	for (int i = 0; i < NO_OF_CP_SIDES; ++i)
	{

		log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.treatEvent() - i = %d , CP_state_ = %d \n", i, CP_state_);

		// Skip connect to other side in case sides not separated.
		if ((i == STANDBY_SIDE) /*&& (CP_state_ != CP_SEPARATED)*/)
		{
			continue;
		}

		// Put the event into Alarm List
		if (exeMgr_[i]->treatEvent(alarmMessage) == false)
		{

			// If error, report it and exit
			char problemData[DEF_MESSIZE];

			snprintf(problemData, sizeof(problemData) - 1, "ACS_ALHD\n%s", exeMgr_[i]->getErrorText());

			eventMgr_.sendEvent(
					exeMgr_[i]->getError(),
					STRING_EVENT,
					ALCOName[i],
					problemData,
					ACS_ALH_Text_EventTreating_problem);

			return false;
		}//end if

	}//end for

	//TODO
	//Da Eleiminare
	//testAck();

	return true; // Ok

}



bool  acs_alh_eventreader::createAlarmManager(bool separated){

	util_.trace("TRACE: acs_alh_eventreader::createAlarmManager() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.createAlarmManager() - enter");

	if(!separated){
		exeMgr_[EX_SIDE] = new (std::nothrow) acs_alh_alarm_manager(connHandler_);
		if(!exeMgr_[EX_SIDE]){
			log_.write(LOG_LEVEL_ERROR, "Memory error allocating object: acs_alh_alarm_manager exeMgr_[%d] !",  EX_SIDE);
			return false;
		}
		exeMgr_[EX_SIDE]->setCpState(separated);
		//TODO
		//Da fare solo se non stiamo runnando al seguito di un rebbot
		exeMgr_[EX_SIDE]->countAlarmsInList();
	}else{
		exeMgr_[EX_SIDE] = new (std::nothrow) acs_alh_alarm_manager(connHandler_);
		if(!exeMgr_[EX_SIDE]){
			log_.write(LOG_LEVEL_ERROR, "Memory error allocating object: acs_alh_alarm_manager exeMgr_[%d] !",  EX_SIDE);
			return false;
		}
		exeMgr_[EX_SIDE]->setCpState(separated);
		//TODO
		//Da fare solo se non stiamo runnando al seguito di un rebbot
		exeMgr_[EX_SIDE]->countAlarmsInList();

		exeMgr_[STANDBY_SIDE] = new (std::nothrow) acs_alh_alarm_manager(exeMgr_[EX_SIDE]);
		if(!exeMgr_[STANDBY_SIDE]){
			log_.write(LOG_LEVEL_ERROR, "Memory error allocating object: acs_alh_alarm_manager exeMgr_[%d] !",  STANDBY_SIDE);
			return false;
		}
		exeMgr_[STANDBY_SIDE]->setCpState(separated);

		exeMgr_[STANDBY_SIDE]->unAckAllItem();
	}

	util_.trace("TRACE: acs_alh_eventreader::createAlarmManager() - exit");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_eventreader.createAlarmManager() - exit");

	return true;

}




void acs_alh_eventreader::testAck(){

	log_.write(LOG_LEVEL_DEBUG, "acs_alh_eventreader::testAck() enter ");

	printf("acs_alh_eventreader::testAck() enter\n ");
	acs_alh_imm_data_handler immHandler_(connHandler_);

	int call_res = 0;

	for(int i=0; i < NO_OF_CP_SIDES; ++i){

		unsigned short cpAlarmRef = 1234;

		AllRecord alarm;

		call_res = exeMgr_[i]->getUnAckItem(alarm);

		if(call_res == 0){

			if(!strcmp(alarm.event.percSeverity, STRING_CEASING) ){
				printf("try to ack alarm ceasing for %s: %d:%d \n", ALCOName[i], alarm.identity[0], alarm.identity[2]);
			}else{
				printf("try to ack alarm for %s: %d:%d \n", ALCOName[i], alarm.identity[0], alarm.identity[2]);
			}

			if( exeMgr_[i]->ackAlarm(&alarm, cpAlarmRef) ){
				printf("ACK OK!!!\n");
			}else{
				printf("ACK NOT OK!!!\n");
			}

		}//end if

	}//end for

	log_.write(LOG_LEVEL_DEBUG, "acs_alh_eventreader::testAck() exit ");

}

