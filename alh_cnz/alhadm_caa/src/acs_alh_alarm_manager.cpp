/*
 * acs_alh_alarm_manager.cpp
 *
 *  Created on: Nov 11, 2011
 *      Author: efabron
 *  Modified on: Jan 20, 20202
 *      Authors: TEISMA, TEIMON
 */

#include "acs_alh_alarm_manager.h"
#include <acs_aeh_evreport.h>
#include "acs_alh_imm_runtime_owner.h"
#define ACS_USAFM_ValidCertUnavailable 8771
#define ACS_USAFM_CertAboutToExpire 8772  // Id the Trusted Certificate Alarm 
static unsigned char alarmIndexRecord_[MAX_ALARMS_IN_LIST];

//========================================================================================
//	Constructor
//========================================================================================

acs_alh_alarm_manager::acs_alh_alarm_manager(acs_alh_imm_connection_handler *connHandler)
: log_(), util_("acs_alh_alarm_manager"), ahtMgr_(), eventMgr_(),immHandler_(connHandler), currentAlarmsInList_()
{
	exOrsb = 0;
	separatedCP_ = false;
	connHandler_ = connHandler;
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::acs_alh_alarm_manager() - Constructor for EX-SIDE, exOrsb = %i",exOrsb);
}

//========================================================================================
//	Copy Constructor for cpSide SB
//========================================================================================
acs_alh_alarm_manager::acs_alh_alarm_manager(const acs_alh_alarm_manager* execPtr)
: log_(), util_("acs_alh_alarm_manager"), ahtMgr_(), eventMgr_(execPtr->eventMgr_),immHandler_(execPtr->connHandler_), currentAlarmsInList_()
{
	exOrsb = 1;
	separatedCP_ = true;
	currentAlarmsInList_ = execPtr->currentAlarmsInList_;
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::acs_alh_alarm_manager() - Constructor for SB-SIDE, exOrsb = %i",exOrsb);
}


acs_alh_alarm_manager::acs_alh_alarm_manager(acs_alh_imm_connection_handler *connHandler, const acs_alh_eventmgr& eventMgr)
: log_(), util_("acs_alh_alarm_manager"), ahtMgr_(), eventMgr_(eventMgr),immHandler_(connHandler)
{

}


acs_alh_alarm_manager::~acs_alh_alarm_manager(){}



//========================================================================================
//	Treat event
//========================================================================================

bool acs_alh_alarm_manager::treatEvent(ACS_ALH_AlarmMessage& eventMessage)
{
	util_.trace("TRACE: acs_alh_alarm_manager::treatEvent() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatEvent() - enter");

	// Since the getMatch method is called each time an event in handled by ALH server
	// it's necessary to avoid frequently IMM access for each event handled.
	//If the boolean is true this means that the alarm table has been already checked when
	// the first event has been received by ALH; the next check will be made when the object acs_alh_ahtmgr
	// is destroyed and then recreated

	if ( !ahtMgr_.Alarm_Table_Already_Loaded() ){
		ahtMgr_.getParameters();
		ahtMgr_.setAlarmTableFlag( true );
	}

	// Consult Alarm Handler Table for substitution of Perceived Severity
	ahtMgr_.getMatch(eventMessage);

	// If resulting Perceived Severity attribute == EVENT then skip it...
	if (!strcmp(eventMessage.event.percSeverity, STRING_EVENT))
	{
		util_.trace("TRACE: acs_alh_alarm_manager::treatEvent() - exit");
		log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatEvent() - exit");
		return true;
	}

	// If percSeverity == ALARM then store it into the Alarm List
	// further on to be sent to CP
	if (strcmp(eventMessage.event.percSeverity, STRING_CEASING))
	{
//		printf("Severity: %s\n", eventMessage.event.percSeverity);

		if (!treatAlarm(eventMessage))
		{
			util_.trace("TRACE: acs_alh_alarm_manager::treatEvent() - exit - treatAlarm Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatEvent() - exit - treatAlarm  Returning false!!!");
			return false;
		}
	}
	else
	{
		// CEASING! Store it into the Alarm List further on to be sent to CP
		if (!treatCeasing(eventMessage))
		{
			util_.trace("TRACE: acs_alh_alarm_manager::treatEvent() - exit - treatCeasing Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatEvent() - exit - treatCeasing Returning false!!!");
			return false;
		}
	}

	util_.trace("TRACE: acs_alh_alarm_manager::treatEvent() - exit ");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatEvent() - exit ");
	return true;
}



//========================================================================================
//	Treat alarm
//========================================================================================

bool acs_alh_alarm_manager::treatAlarm(ACS_ALH_AlarmMessage& eventMessage)
{
	util_.trace("TRACE: acs_alh_alarm_manager::treatAlarm() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatAlarm() - enter");

	// AEH object to report an Event
	acs_aeh_evreport EvReport;

	// Allocate record for alarm
	AllRecord alarm;
	alarm.event = eventMessage.event;
	alarm.identity[0] = (unsigned short)eventMessage.event.specificProblem;
	alarm.identity[1] = 0;
	alarm.identity[2] = getAlarmIndex();
	alarm.cpAlarmRef = 0;
	alarm.retransCnt = 0;
	alarm.ceasePending = false;
	alarm.manualCease = eventMessage.manualCease;

	string percSeverity(alarm.event.percSeverity);
	if		(percSeverity == STRING_CEASING) alarm.sendPriority = 0;
	else if (percSeverity == STRING_A1)		 alarm.sendPriority = 1;
	else if (percSeverity == STRING_A2)		 alarm.sendPriority = 2;
	else if (percSeverity == STRING_A3)		 alarm.sendPriority = 3;
	else if (percSeverity == STRING_O1)		 alarm.sendPriority = 4;
	else if (percSeverity == STRING_O2)		 alarm.sendPriority = 5;
	else return false;

	alarm.acknowledge = false;
	alarm.time = eventMessage.eventTime;


	// Find out whether item with matching "specificProblem" and
	// "objectOfReference" already is present in the list.
	std::vector<AllRecord> alarmList;
	bool checkSendEvent(true);
	int call_res;

	while(1)
	{
		call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
		if(call_res == 0)
		{
			for(int i=0; i<(int)alarmList.size(); i++)
                        {
	                        if((alarmList[i].event.specificProblem == alarm.event.specificProblem) &&
                                    (!strcmp(alarmList[i].event.objectOfReference, alarm.event.objectOfReference)) &&
                                    (!alarmList[i].ceasePending) &&
                                    (!strcmp(alarmList[i].event.percSeverity, alarm.event.percSeverity))){
                                if(((alarm.event.specificProblem == ACS_USAFM_ValidCertUnavailable) || (alarm.event.specificProblem == ACS_USAFM_CertAboutToExpire)) && !(strcmp(alarmList[i].event.problemText,alarm.event.problemText)==0))   // In case of Node Credential and Trusted Certificate, a che is is done on problemtext
					continue;

                                    util_.trace("TRACE: acs_alh_alarm_manager::treatAlarm() - Alarm already present, dropping...");
                                    log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatAlarm() - Alarm already present, dropping...");
                                    util_.trace("TRACE: acs_alh_alarm_manager::treatAlarm() - exit");
                                    log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatAlarm() - exit");
                                    return true;
                                }//end if
                        }//end for
			break;
		}
		else
		{
			if(checkSendEvent)
			{
				EvReport.sendEventMessage(
										"acs_alhd",
										8300,
										"EVENT",
										"Impossible check if the Alarm is already present into IMM.",
										"EVENT",
										"-",
										"An error occurred reading the Alarms from the IMM.",
										"-"
										);

				checkSendEvent = false;
			}

			util_.trace("TRACE: acs_alh_alarm_manager::treatAlarm() exit - getAlarmInToList error - Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatAlarm() exit - Impossible check if the Alarm is already present into IMM. EroorCode: %d", call_res);
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatAlarm() exit - getAlarmInToList error - Returning false!!!");

			sleep(2);
			//return false;
		}
	}

//	//=====================================================================================================
//	//ONLY TO TEST
//	//=====================================================================================================
//	std::vector<AllRecord> alarmList;
//	int test_res = immHandler_.getAlarmInToList(&alarmList, 0);
//	if(test_res!=0){
//		printf("ERROR: %d\n", immHandler_.getError());
//		printf("ERROR CODE: %s\n", immHandler_.getErrorText());
//	}else{
//		for(int i=0; i<(int)alarmList.size(); i++){
//			printf("Identity %d:%d\n", alarmList[i].identity[0], alarmList[i].identity[2]);
//			printf("processName: %s\n", alarmList[i].event.processName);
//
//			printf("Severity of Alarm: %s\n", alarmList[i].event.percSeverity);
//			printf("Specific Problem of Alarm: %ld\n", alarmList[i].event.specificProblem);
//			printf("Probable cause of Alarm: %s\n", alarmList[i].event.probableCause);
//			printf("Category of Alarm: %s\n", alarmList[i].event.objClassOfReference);
//			printf("Object of reference of Alarm: %s\n", alarmList[i].event.objectOfReference);
//			printf("Problem data of Alarm: %s\n", alarmList[i].event.problemData);
//			printf("Problem text of Alarm: %s\n", alarmList[i].event.problemText);
//			printf("cpAlarmRef of Alarm: %u\n", alarmList[i].cpAlarmRef);
//			printf("Retransmission counter of Alarm: %u\n", alarmList[i].retransCnt);
//			printf("Send Priority of Alarm: %u\n", alarmList[i].sendPriority);
//			printf("Cease pending of Alarm: %d\n", alarmList[i].ceasePending);
//			printf("Manual ceasing of Alarm: %d\n", alarmList[i].manualCease);
//			printf("Ack of Alarm: %d\n", alarmList[i].acknowledge);
//
//		}
//	}
//
//	//=====================================================================================================
//	//=====================================================================================================




	if (storeToIMM(&alarm) != true){ // Does always return true

		// If error, report it and exit
		char problemData[DEF_MESSIZE];

		snprintf(problemData, sizeof(problemData) - 1, "acs_alhd - %s", getErrorText());

		eventMgr_.sendEvent(
				ACS_ALH_alarmListWriteError,
				STRING_EVENT,
				"acs_alhd",
				problemData,
				ACS_ALH_Text_AlarmListWriting_problem);


		util_.trace("TRACE: acs_alh_alarm_manager::treatAlarm() - exit - storeToIMM Returning false!!!");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatAlarm() - exit - storeToIMM Returning false!!!");
		return false;
	}


	util_.trace("TRACE: acs_alh_alarm_manager::treatAlarm() - exit");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatAlarm() - exit");
	return true;
}



//========================================================================================
//	Treat alarm ceasing
//========================================================================================

bool acs_alh_alarm_manager::treatCeasing(ACS_ALH_AlarmMessage& eventMessage)
{
	AllRecord ar;
	AllRecord cease;


	util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatCeasing() - enter");


	// Try to find corresponding Alarm record already stored in Alarm List:
	// Search Alarm List for alarm item with same
	// <specProb/objOfRef> as the Cease and with
	// ceasePending == false
	// if found
	//   append Ceasing to Alarm List

	// First check if alarm ceased from acease command (Awful solution!).
	// The awful solution is due to the fact that the acease command
	// uses specificProblem and index to specify which alarm to cease.
	// The index is not possible to specify in the event-API used to
	// cease an alarm. To circumvent that problem, the index is delivered
	// in the problemData. The index is then extracted from the
	// problemData and used in this routine.

	int index;
	if (sscanf(
			eventMessage.event.problemData,
			"Ceased by acease command index %d",
			&index
			) != 1)
		index = -1;

	cease.event = eventMessage.event;
	cease.cpAlarmRef = 0;
	cease.retransCnt = 0;
	cease.ceasePending = false;
	cease.sendPriority = 0;
	cease.time = eventMessage.eventTime;


	if (index >= 0)	{	// Try to find the specified index

		// The index is specified. That means the cease is ordered by the command
		// acease. Thus search for an alarm with specified index.

		std::vector<AllRecord> alarmList;
		int counter=0;
		do{
		int call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
		if(call_res == 0) {
			for(int i=0; i<(int)alarmList.size(); i++){

				if ((alarmList[i].event.specificProblem == cease.event.specificProblem) &&
					(strcmp(alarmList[i].event.objectOfReference, cease.event.objectOfReference) == 0) &&
					(alarmList[i].identity[2] == index) &&
					(alarmList[i].ceasePending == false) ){

					for (int j = 0; j < 3; j++)
					{
						ar.identity[j] = alarmList[i].identity[j];
					}
					ar.event = alarmList[i].event;
					cease.ceasePending = true;
					break;
				}//end if
			}//end for
			if(cease.ceasePending == false)
			{
			    std::string nodeNameLocal = util_.getlocalNodeName();
			    bool server_running = util_.isAlhServerRunning( nodeNameLocal );
			    if ( !server_running )
			    { // ALH server is not running
			    	log_.write(LOG_LEVEL_TRACE, "value od server running in  treatCeasing is %d",server_running);
			        acs_alh_imm_runtime_owner temp_OI(connHandler_);
			        sleep(1);
			        temp_OI.finalize_IMM();
			        temp_OI.init_IMM( false );
			     }
			 }
			 else
			 break;

			}//end if
		else{
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatCeasing() - Impossible to access IMM ");
			util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() - exit - Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatCeasing() - exit - Returning false!!!");
			break;
		}
		counter++;
		}while(counter<=1);


	}//end if (index)

	if ((index < 0) || (cease.ceasePending == false)) // Try to find a matching alarm
	{
		// The index is not specified (that is, the cease is not ordered from the
		// command acease) or the index is specified but corresponding alarm is
		// not found. (A rather small risk for that but it might happen if the
		// alarm is ceased after the acease command is issued but before the
		// cease from acease arrived to alhexec).
		// If you have two (or several) alarms with the same SpecificProblem and
		// objectOfReference but with different severity, this solution might cause
		// the wrong alarm to be ceased. But in that case you have not followed the
		// design rules. So dont blame me!

		std::vector<AllRecord> alarmList;
		int counter=0;
		do{
		int call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
		if(call_res == 0) {
                        for (int i = 0; i <(int)alarmList.size(); i++) {
				if ((alarmList[i].event.specificProblem == cease.event.specificProblem) &&
                                    (strcmp(alarmList[i].event.objectOfReference, cease.event.objectOfReference) == 0) &&
                                    (alarmList[i].ceasePending == false)){
					if (((cease.event.specificProblem == ACS_USAFM_ValidCertUnavailable) || (cease.event.specificProblem == ACS_USAFM_CertAboutToExpire))  && !(strcmp(alarmList[i].event.problemText,cease.event.problemText)==0)) // In case of Node Credential and Trusted Certificate, a che is is done on problemtext
						continue;

                                        for (int j = 0; j < 3; j++)
                                        {
                                            ar.identity[j] = alarmList[i].identity[j];
                                        }
                                        ar.event = alarmList[i].event;
                                        cease.ceasePending = true;
                                        break;
				}
                        }//end for
			if(cease.ceasePending == false)
			{
			     std::string nodeNameLocal = util_.getlocalNodeName();
			     bool server_running = util_.isAlhServerRunning( nodeNameLocal );
			     if ( !server_running )
			     { // ALH server is not running
			    	 	 log_.write(LOG_LEVEL_TRACE, "value od server running in  treatCeasing is %d",server_running);
			             acs_alh_imm_runtime_owner temp_OI(connHandler_);
			             sleep(1);
			             temp_OI.finalize_IMM();
			             temp_OI.init_IMM( false );
			     }
			}
			else
			break;
	    }//end if
		else{
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatCeasing() - Impossible to access IMM ");
			util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() exit - Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatCeasing() exit - Returning false!!!");
			break;
		}
		counter++;
		}while(counter<=1);

	}

	if (cease.ceasePending)
	{
		// Old alarm found in Alarm List
		// indicate cease pending for found alarm item and update


		bool retValue = appendCeasingToList(&ar, &cease);
		if(!retValue){
			// If error, report it and exit
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "acs_alhd - %s", getErrorText());
			eventMgr_.sendEvent(
					ACS_ALH_alarmListWriteError,
					STRING_EVENT,
					"acs_alhd",
					problemData,
					ACS_ALH_Text_AlarmListWriting_problem);

			util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() - exit - Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatCeasing() - exit - Returning false!!!");
		}
		util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() - exit ");
		log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatCeasing() - exit ");

		return retValue;
	}
	else
	{
		// cease for this alarm already stored in Alarm List!

		util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() - Ceasing already stored or Alarm not present");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::treatCeasing() - Ceasing already stored or Alarm not present");
	}

	util_.trace("TRACE: acs_alh_alarm_manager::treatCeasing() - exit ");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::treatCeasing() - exit ");

	return true;
}






//========================================================================================
//	store to IMM
//========================================================================================

bool acs_alh_alarm_manager::storeToIMM(const AllRecord* allPtr)
{
	//int counter = getRecordCounter();

	util_.trace("TRACE: acs_alh_alarm_manager::storeToIMM() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::storeToIMM() - enter");

	int ret = 0;

	ret = immHandler_.addAlarmInfo(allPtr);
	if( ret != 0 ){
		setError(immHandler_.getError());
		setErrorText(immHandler_.getErrorText());

		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::storeToIMM() - addAlarmInfo(...) Failed. ErrorCode: %d - %s", immHandler_.getError(), immHandler_.getErrorText());
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::storeToIMM() - exit - Returning false!!!");
		util_.trace("TRACE: acs_alh_alarm_manager::storeToIMM() - exit - Returning false!!!");

		return false;
	}

//	setRecordCounter(counter+1);
	if (strcmp(STRING_CEASING, allPtr->event.percSeverity) != 0)
	{
//		printf("SONO in STORE IMM\n");
		setAlarmIndex(allPtr->identity[2], 1);
		currentAlarmsInList_++;
	}

	util_.trace("TRACE: acs_alh_alarm_manager::storeToIMM() - exit");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::storeToIMM() - exit");

	return true;
}



//========================================================================================
//	Append ceasing to list
//========================================================================================

bool acs_alh_alarm_manager::appendCeasingToList(AllRecord* arPtr, AllRecord* ceasePtr)
{
    // A Ceasing item shall be inserted into Alarm List
    // The Alarm corresponding to this has to be updated first.
    // Indicate cease pending for found alarm item and update Alarm List

	util_.trace("TRACE: acs_alh_alarm_manager::appendCeasingToList() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::appendCeasingToList() - enter");

	int ret_code = 0;
	char rdnValue[20] = { 0 };

	snprintf(rdnValue, sizeof(rdnValue) - 1, "%hu:%hd", arPtr->identity[0], arPtr->identity[2]);

	int counter = 0;
	do{
		ret_code = immHandler_.updateCeasePending(rdnValue, 1, exOrsb);
		if(ret_code == 0){
			//NO_ERROR exit from do while
			break;
		}else if(ret_code == acs_alh::ERR_SAF_IMM_RETRY){
			//Method failed because IMM is not available. Try to repeat the operation
			counter++;
			sleep(2);
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::appendCeasingToList() - method updateCeasePending failed. - IMM Error Code: %d - Retry the operation. counter: %d",
					immHandler_.getError(),
					counter);
			util_.trace("ERROR: acs_alh_alarm_manager::appendCeasingToList() - method updateCeasePending failed. - IMM Error Code: %d - Retry the operation. counter: %d",
					immHandler_.getError(),
					counter);
		}else{
			//Method failed. exit from do while and handle the error after
			break;
		}

	}while((ret_code != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

	counter = 0;

	//ret_code = immHandler_.updateCeasePending(rdnValue, 1, exOrsb);
	if(ret_code == 0){

		// Update the info in cease record
	    ceasePtr->event = arPtr->event;
	    for (int i = 0; i < 3; i++)
	    {
			ceasePtr->identity[i] = arPtr->identity[i];
	    }

	    strncpy(ceasePtr->event.percSeverity, STRING_CEASING, sizeof(ceasePtr->event.percSeverity) - 1);
	    ceasePtr->cpAlarmRef = 0;
	    ceasePtr->retransCnt = 0;
	    ceasePtr->sendPriority = 0;
	    ceasePtr->ceasePending = true;
	    ceasePtr->acknowledge = false;


	    // Link the Ceasing item to Alarm List File
	    if(storeToIMM(ceasePtr)){
	    	return true;
	    }else{
	    	//Store of ceasing item into Alarm List failed
	    	log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::appendCeasingToList() - Impossible to create the cease alarm info object for alarm %s ", rdnValue);
	    	util_.trace("TRACE: acs_alh_alarm_manager::appendCeasingToList() - Impossible to create the cease alarm info object for alarm %s ", rdnValue);

	    	//Try to set the cease pending attribute with the old value.
	    	do{
	    		ret_code = immHandler_.updateCeasePending(rdnValue, 0, exOrsb);
	    		if(ret_code == 0){
	    			//NO_ERROR exit from do while
	    			break;
	    		}else if(ret_code == acs_alh::ERR_SAF_IMM_RETRY){
	    			//Method failed because IMM is not available. Try to repeat the operation
	    			counter++;
	    			sleep(2);
	    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::appendCeasingToList() - method updateCeasePending failed. - IMM Error Code: %d - Retry the operation. counter: %d",
						immHandler_.getError(),
						counter);
	    			util_.trace("ERROR: acs_alh_alarm_manager::appendCeasingToList() - method updateCeasePending failed. - IMM Error Code: %d - Retry the operation. counter: %d",
						immHandler_.getError(),
						counter);
	    		}else{
	    			//Method failed. exit from do while and handle the error after
	    			break;
	    		}

	    	}while((ret_code != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

			// If error, report it and exit
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "acs_alhd - Impossible to store the cease item for alarm %hd:%hd into Alarm List",
					arPtr->identity[0],
					arPtr->identity[2]);

			eventMgr_.sendEvent(
					ACS_ALH_alarmListWriteError,
					STRING_EVENT,
					"acs_alhd",
					problemData,
					ACS_ALH_Text_AlarmListWriting_problem);


			util_.trace("TRACE: acs_alh_alarm_manager::appendCeasingToList() - exit - Returning false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::appendCeasingToList() - exit - Returning false!!!");

	    	return false;
	    }
	}else{

		// If error, report it and exit
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "acs_alhd - Impossible to update the cease pending attribute of alarm %hu:%hd",
				arPtr->identity[0],
				arPtr->identity[2]);
		eventMgr_.sendEvent(
				ACS_ALH_alarmListWriteError,
				STRING_EVENT,
				"acs_alhd",
				problemData,
				ACS_ALH_Text_AlarmListUpdating_problem);

		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::appendCeasingToList() - Impossible to update the cease pending attribute of alarm object %s "
				"- ErrorMsg: %s - ErrorCode: %d", rdnValue, immHandler_.getErrorText(), immHandler_.getError());
		util_.trace("ERROR: acs_alh_alarm_manager::appendCeasingToList() - Impossible to update the cease pending attribute of alarm object %s "
				"- ErrorMsg: %s - ErrorCode: %d", rdnValue, immHandler_.getErrorText(), immHandler_.getError());

		util_.trace("TRACE: acs_alh_alarm_manager::appendCeasingToList() - exit - Returning false!!!");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::appendCeasingToList() - exit - Returning false!!!");

		return false;
	}

}




//========================================================================================
//	Set alarm index
//========================================================================================

bool acs_alh_alarm_manager::setAlarmIndex(short index, unsigned char oneOrZero)
{

	// 0 -> free index
	// 1 -> busy index

	if ((index >= 0) && (index < MAX_ALARMS_IN_LIST))
	{
		alarmIndexRecord_[index] = oneOrZero;
//		printf("SET INDEX: %d ALARMINDEX_RECORD: %d\n",index, alarmIndexRecord_[index]);
		return true;
	}
	return false;
}


//========================================================================================
//	Get alarm index
//========================================================================================

short acs_alh_alarm_manager::getAlarmIndex()
{
//	printf("getAlarmIndex\n");
	for (short i = 0; i < MAX_ALARMS_IN_LIST; i++)
	{
	//	printf("getAlarmIndex %d\n",i);
		if (alarmIndexRecord_[i] == 0){
		//	printf("index: %d\n",i);
			return i;
		}
	}
	return MAX_ALARMS_IN_LIST;
}


//========================================================================================
//	Acknowledge alarm
//========================================================================================

bool acs_alh_alarm_manager::ackAlarm(AllRecord* allPtr, const unsigned short cpAlarmRef)
{

	util_.trace("TRACE: acs_alh_alarm_manager::ackAlarm() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackAlarm() - enter");

	int ret_code = 0;
	int counter = 0;

	// Check if acknowledged message was ALARM or alarm CEASING
	if (allPtr->sendPriority)
	{
		// ALARM (A1 - O2) acknowledged
		// copy cpAlarmReference from ALA_ACK to Alarm List field
		// allPtr->cpAlarmRef = cpAlarmRef;
		// allPtr->acknowledge = true;
		char rdnValue[20] = { 0 };

		snprintf(rdnValue, sizeof(rdnValue) - 1, "%hu:%hd", allPtr->identity[0], allPtr->identity[2]);

		/*OLD VERSION*/
//		int ret_code = immHandler_.ackAlarm(rdnValue, cpAlarmRef, 1, exOrsb);
//		if(ret_code != 0){
//			char problemData[DEF_MESSIZE];
//			setError(ACS_ALH_alarmListWriteError);
//			snprintf(problemData, sizeof(problemData) - 1, "Error when writing AlarmList. ErrCode = %d", ret_code);
//			setErrorText(problemData);
//			util_.trace("TRACE: acs_alh_alarm_manager::ackAlarm() - exit - ackAlarm in IMM Failed Returning - false!!!");
//			log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackAlarm() - exit - ackAlarm in IMM Failed - Returning false!!!");
//			return false;
//		}

		/*NEW VERSION*/
		do{
			ret_code = immHandler_.ackAlarm(rdnValue, cpAlarmRef, 1, exOrsb);
    		if(ret_code == 0){
    			//NO_ERROR exit from do while
    			break;
    		}else if(ret_code == acs_alh::ERR_SAF_IMM_RETRY){
    			//Method failed because IMM is not available. Try to repeat the operation
    			counter++;
    			//sleep(2);
    			sleep(5);
    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackAlarm() - method ackAlarm failed. - IMM Error Code: %d - Retry the operation. counter: %d",
					immHandler_.getError(),
					counter);
    			util_.trace("ERROR: acs_alh_alarm_manager::ackAlarm() - method ackAlarm failed. - IMM Error Code: %d - Retry the operation. counter: %d",
					immHandler_.getError(),
					counter);
    		}else{
    			//Method failed. exit from do while and handle the error after
    			//The calling method report the error as EVENT in messenger file.
    			char problemData[DEF_MESSIZE];
    			setError(ACS_ALH_alarmListWriteError);
    			snprintf(problemData, sizeof(problemData) - 1, "Error when writing AlarmList. ErrCode = %d", ret_code);
    			setErrorText(problemData);

    			util_.trace("TRACE: acs_alh_alarm_manager::ackAlarm() - exit - ackAlarm in IMM Failed Returning - false!!!");
    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackAlarm() - exit - ackAlarm in IMM Failed - Returning false!!!");
    			return false;
    		}

		}while((ret_code != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

		if(ret_code != 0){
			char problemData[DEF_MESSIZE];
			setError(ACS_ALH_alarmListWriteError);
			snprintf(problemData, sizeof(problemData) - 1, "Error when writing AlarmList. ErrCode = %d", ret_code);
			setErrorText(problemData);

			util_.trace("TRACE: acs_alh_alarm_manager::ackAlarm() - exit - ackAlarm in IMM Failed Returning - false!!!");
			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackAlarm() - exit - ackAlarm in IMM Failed - Returning false!!!");
			return false;
		}

	}
	else
	{
		// CEASING acknowledged!
		if (ackCeasing(allPtr) != true)
		{
			char problemData[DEF_MESSIZE];
			setError(ACS_ALH_alarmListWriteError);
			snprintf(problemData, sizeof(problemData) - 1, "Error when writing AlarmList.");
			setErrorText(problemData);
			util_.trace("TRACE: acs_alh_alarm_manager::ackAlarm() - exit - Returning false!!!");
			log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackAlarm() - exit - Returning false!!!");
			return false;
		}
	}

	util_.trace("TRACE: acs_alh_alarm_manager::ackAlarm() - exit");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackAlarm() - exit");
	return true;

}


//========================================================================================
//	Acknowledge alarm ceasing
//========================================================================================

bool acs_alh_alarm_manager::ackCeasing(const AllRecord* allPtr)
{

	util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - enter", exOrsb);

	int counter = 0;
	// Find corresponding alarm
	// Find out whether item with matching "specificProblem" and
	// "objectOfReference" already is present in the list.

	std::vector<AllRecord> alarmList;
	int call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
	if(call_res == 0) {
		for(int i=0; i<(int)alarmList.size(); i++){
			if ((alarmList[i].identity[0] == allPtr->identity[0]) &&
				(alarmList[i].identity[1] == allPtr->identity[1]) &&
				(alarmList[i].identity[2] == allPtr->identity[2]) &&
				(!strcmp(alarmList[i].event.percSeverity, STRING_CEASING)))
			{

				char rdnValueCease[20] = { 0 };
				char rdnValueAl[20] = { 0 };

				snprintf(rdnValueCease, sizeof(rdnValueCease) - 1, "%hu:%hd:C", allPtr->identity[0], allPtr->identity[2]);
				snprintf(rdnValueAl, sizeof(rdnValueAl) - 1, "%hu:%hd", allPtr->identity[0], allPtr->identity[2]);

				//Set the acknowledge flag for the Alarm Ceasing object
				//OLD VERSION
//				call_res = immHandler_.ackAlarm(rdnValueCease, 9999, 1, exOrsb);
//				if(call_res != 0){
//					util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() - exit - ackAlarm in IMM Failed Returning - false!!!");
//					log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - ackAlarm in IMM Failed - Returning false!!!", exOrsb);
//					return false;
//				}
				//NEW VERSION
				do{
					call_res = immHandler_.ackAlarm(rdnValueCease, 9999, 1, exOrsb);
					if(call_res == 0){
		    			//NO_ERROR exit from do while
		    			break;
					}else if(call_res == acs_alh::ERR_SAF_IMM_RETRY){
		    			//Method failed because IMM is not available. Try to repeat the operation
		    			counter++;
		    			sleep(2);
		    			log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::ackCeasing() - method ackAlarm failed. - IMM Error Code: %d - Retry the operation. counter: %d",
							immHandler_.getError(),
							counter);
		    			util_.trace("ERROR: acs_alh_alarm_manager::ackCeasing() - method ackAlarm failed. - IMM Error Code: %d - Retry the operation. counter: %d",
							immHandler_.getError(),
							counter);
					}else{
		    			//Method failed. exit from do while and handle the error after
		    			//The calling method report the error as EVENT in messenger file.
		    			util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() - exit - ackAlarm in IMM Failed Returning - false!!!");
		    			log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - ackAlarm in IMM Failed - Returning false!!!", exOrsb);
		    			return false;
					}

				}while((call_res != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

				if(call_res != 0){
	    			util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() - exit - ackAlarm in IMM Failed Returning - false!!!");
	    			log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - ackAlarm in IMM Failed - Returning false!!!", exOrsb);
	    			return false;
				}

				counter = 0;

				//Remove the alarm and alarm ceasing object from IMM
				do{
					call_res = immHandler_.removeAlarmFromIMM(rdnValueAl, separatedCP_);
					if(call_res >= 0){
		    			//NO_ERROR exit from do while
		    			break;
					}else if(call_res == acs_alh::ERR_SAF_IMM_RETRY){
		    			//Method failed because IMM is not available. Try to repeat the operation
		    			counter++;
		    			sleep(2);
					}else{
						//Error! Impossible remove object from IMM
						setError(ACS_ALH_alarmListWriteError);
						char problemData[DEF_MESSIZE];
						snprintf(problemData, sizeof(problemData) - 1,
								"Error when update the AlarmList. ErrorCode = %d", call_res );
						setErrorText(problemData);

						log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Impossible remove the alarm from IMM", rdnValueAl, exOrsb );
						log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - IMM Failed - Returning false!!!", exOrsb);
						return false;
					}

				}while((call_res != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

				log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - removeAlarmFromIMM exits with %d", rdnValueAl, exOrsb, call_res );

				//call_res = immHandler_.removeAlarmFromIMM(rdnValueAl, separatedCP_);
				if(call_res < 0){
					//Error! Impossible remove object from IMM
					setError(ACS_ALH_alarmListWriteError);
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1,
							"Error when update the AlarmList. ErrorCode = %d", call_res );
					setErrorText(problemData);

					log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Impossible remove the alarm from IMM", rdnValueAl, exOrsb );
					log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - IMM Failed - Returning false!!!", exOrsb);
					return false;
				}else{

					if(call_res == 2){
						log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Alarm definitely removed from IMM", rdnValueAl, exOrsb);
						setAlarmIndex(allPtr->identity[2], 0);
						currentAlarmsInList_--;
					}else{
						log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Alarm removed from IMM for cpSide %d", rdnValueAl, exOrsb, exOrsb );
					}

					counter = 0;

					do{
						call_res = immHandler_.removeAlarmFromIMM(rdnValueCease, separatedCP_);
						if(call_res >= 0){
			    			//NO_ERROR exit from do while
			    			break;
						}else if(call_res == acs_alh::ERR_SAF_IMM_RETRY){
			    			//Method failed because IMM is not available. Try to repeat the operation
			    			counter++;
			    			sleep(2);
						}else{
							//Error! Impossible remove object from IMM
							setError(ACS_ALH_alarmListWriteError);
							char problemData[DEF_MESSIZE];
							snprintf(problemData, sizeof(problemData) - 1,
									"Error when update the AlarmList. ErrorCode = %d", call_res );
							setErrorText(problemData);

							log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Impossible remove the alarm from IMM", rdnValueCease, exOrsb );
							log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - IMM Failed - Returning false!!!", exOrsb);
							return false;
						}

					}while((call_res != 0) && (counter<RETRY_COUNTER_MAX_VALUE));


					//call_res = immHandler_.removeAlarmFromIMM(rdnValueCease, separatedCP_);

					if(call_res < 0){
						//Error! Impossible remove object from IMM
						setError(ACS_ALH_alarmListWriteError);
						char problemData[DEF_MESSIZE];
						snprintf(problemData, sizeof(problemData) - 1,
								"Error when update the AlarmList. ErrorCode = %d", call_res );
						setErrorText(problemData);


						log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Impossible remove the alarm ceasing from IMM", rdnValueCease, exOrsb );
						log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing() cpSide: %d  - exit - IMM Failed - Returning false!!!", exOrsb);
						return false;
					}else{
						if(call_res == 2){
							log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Alarm ceasing definitely removed from IMM", rdnValueCease, exOrsb, exOrsb );
						}else{
							log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing(%s) cpSide: %d - Alarm ceasing removed from IMM for cpSide: %d", rdnValueCease, exOrsb, exOrsb );
						}

					}
				}


				util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit", exOrsb);
				log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() - exit");

				return true;

			}//end if
		}//end for
	}else{

		// Error! Alarm not found
		setError(ACS_ALH_alarmListRemoveError);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1,
			"object EXE.ackCeasing problem.Alarm not found %ld %s",
            allPtr->event.specificProblem,
			allPtr->event.objectOfReference
			);
		setErrorText(problemData);

		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - Impossible to find corresponding alarm into IMM ", exOrsb);
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit - Returning false!!!", exOrsb);
		util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() exit - Returning false!!!");
		return false;
	}


	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::ackCeasing() cpSide: %d - exit", exOrsb);
	util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() - exit");

	return true;
}



//========================================================================================
//	Get first not acknowledged item
//========================================================================================

int acs_alh_alarm_manager::getUnAckItem(AllRecord &alarm, int cpstate)
{

	//Return value:
	// 0	OK
	// <0	ERROR
	//1		NO ALARM TO ACK


	util_.trace("TRACE: acs_alh_alarm_manager::getUnAckItem() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::getUnAckItem() - enter");


	std::vector<AllRecord> alarmList;
	int call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
	if(call_res == 0) {
		int numOfAlarmIntoList = (int)alarmList.size();

		for(int i=0; i<numOfAlarmIntoList; i++){
			if(alarmList[i].acknowledge == false){

				// Set record for alarm
				alarm.event = alarmList[i].event;

				alarm.identity[0] = alarmList[i].identity[0];
				alarm.identity[1] = alarmList[i].identity[1];
				alarm.identity[2] = alarmList[i].identity[2];
				alarm.cpAlarmRef = alarmList[i].cpAlarmRef;
				alarm.retransCnt = alarmList[i].retransCnt;
				alarm.ceasePending = alarmList[i].ceasePending;
				alarm.manualCease = alarmList[i].manualCease;
				alarm.sendPriority = alarmList[i].sendPriority;
				alarm.acknowledge = alarmList[i].acknowledge;
				alarm.time = alarmList[i].time;


				log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::getUnAckItem() for cpSide %d- Alarm(%ld:%ld) acknowledged find ",
						exOrsb,
						alarmList[i].identity[0],
						alarmList[i].identity[2]);
				log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::getUnAckItem() - exit");
				util_.trace("TRACE: acs_alh_alarm_manager::getUnAckItem() - exit");
				log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::getUnAckItem() - identity[0] = %u", alarm.identity[0]);
				log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::getUnAckItem() - identity[1] = %u", alarm.identity[1]);
				log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::getUnAckItem() - send priority = %u", alarm.sendPriority);
				return call_res;
			}
			else
			{
				if (cpstate == CP_WORKING)
				{			
					log_.write(LOG_LEVEL_WARN, "acs_alh_alarm_manager::cp state == CP_WORKING");
					if ( ( strcmp( alarmList[i].event.percSeverity, "CEASING") == 0 ) && ( alarmList[i].acknowledge == true ) ) {
						// An alarm is still stored in IMM even if it has been ceased and both raising / ceasing events have been
						// sent to the CP, and this one has already acknowledge both of them
						// If the CP status is NORMAL this alarm should be removed from IMM
								
						char rdnValueCease[20] = {0};
						char rdnValueAl[20] = {0};

						snprintf(rdnValueCease, sizeof(rdnValueCease) - 1, "%hu:%hd:C", alarmList[i].identity[0], alarmList[i].identity[2]);
						snprintf(rdnValueAl, sizeof(rdnValueAl) - 1, "%hu:%hd", alarmList[i].identity[0], alarmList[i].identity[2]);

						immHandler_.removeAlarmFromIMM(rdnValueCease,EX_SIDE);

						immHandler_.removeAlarmFromIMM(rdnValueAl,EX_SIDE);

						log_.write(LOG_LEVEL_WARN, "acs_alh_alarm_manager::new fix working removeAlarmFromIMM that have notification");
					}
				}				
			}
		}

	}else{
		// Error! Impossible to retrieve the alarm list from IMM
		setError(ACS_ALH_RetrievalDataFromIMMError);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1,
			"object EXE.getUnAckItem problem. Impossible to retrieve data from IMM. ErrorCode: %d ",
			call_res
		);
		setErrorText(problemData);

		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::getUnAckItem() exit - Impossible to access into IMM ");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::getUnAckItem() exit - Returning %d !!!", call_res);
		util_.trace("TRACE: acs_alh_alarm_manager::ackCeasing() exit - Returning %d !!!", call_res);
		return call_res;
	}


	log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::getUnAckItem() - No alarm to acknowledge");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::getUnAckItem() - exit - No alarm to acknowledge");
	util_.trace("TRACE: acs_alh_alarm_manager::getUnAckItem() - exit - No alarm to acknowledge");

	return 1;

}


//========================================================================================
//	Undo acknowledge of all items
//========================================================================================

bool acs_alh_alarm_manager::unAckAllItem()
{


	// For every item in the Alarm List:
	// - clear indication of acknowledge (ackNowledge)
	//   and retransmission counter (retransCnt)
	// - update Alarm List file

	util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::unAckAllItem() - enter");

	int counter = 0;

	std::vector<AllRecord> alarmList;
	int call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
	if(call_res == 0) {
		int numOfAlarmIntoList = (int)alarmList.size();
		printf("NUMBER OF EVENT: %d\n", numOfAlarmIntoList);
		for(int i=0; i<numOfAlarmIntoList; i++){

			char rdnValue[20] = { 0 };
			if(!strcmp(alarmList[i].event.percSeverity, STRING_CEASING)){
				snprintf(rdnValue, sizeof(rdnValue) - 1, "%hu:%hd:C", alarmList[i].identity[0], alarmList[i].identity[2]);
			}else{
				snprintf(rdnValue, sizeof(rdnValue) - 1, "%hu:%hd", alarmList[i].identity[0], alarmList[i].identity[2]);
			}

			//OLD VERSION
//			call_res = immHandler_.unAckEvent(rdnValue, exOrsb);
//			if ( call_res != 0 ){
//				char problemData[DEF_MESSIZE];
//				snprintf(problemData, sizeof(problemData) - 1, "Error when update alarm in  AlarmList. Errno = %d", call_res);
////					eventMgr_.sendEvent(
////						ACS_ALH_alarmListFileWriteError,
////						STRING_EVENT,
////						"EX_",
////						problemData,
////						ACS_ALH_Text_EventTreating_problem
////						);
//
//				log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - Error impossible unAck the alarm %s", rdnValue);
//				log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
//				util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
//
//				return false;
//			}

			//NEW VERSION
			do{
				call_res = immHandler_.unAckEvent(rdnValue, exOrsb);

				if(call_res == 0){
	    			//NO_ERROR exit from do while
	    			break;
				}else if(call_res == acs_alh::ERR_SAF_IMM_RETRY){
	    			//Method failed because IMM is not available. Try to repeat the operation
	    			counter++;
	    			sleep(2);
	    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::ackCeasing() - method unAckEvent failed. - IMM Error Code: %d - Retry the operation. counter: %d",
						immHandler_.getError(),
						counter);
	    			util_.trace("ERROR: acs_alh_alarm_manager::ackCeasing() - method unAckEvent failed. - IMM Error Code: %d - Retry the operation. counter: %d",
						immHandler_.getError(),
						counter);
				}else{
	    			//Method failed. exit from do while and handle the error after
	    			//The calling method report the error as EVENT in messenger file.
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1, "Error when update alarm in  AlarmList (unAckEvent). Errno = %d", call_res);
					eventMgr_.sendEvent(
						ACS_ALH_alarmListWriteError,
						STRING_EVENT,
						"acs_alhd",
						problemData,
						ACS_ALH_Text_AlarmListUpdating_problem
					);
	    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - Error impossible unAck the alarm %s", rdnValue);
	    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
	    			util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
	    			return false;
				}

			}while((call_res != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

			if (call_res != 0){
				char problemData[DEF_MESSIZE];
				snprintf(problemData, sizeof(problemData) - 1, "Error when update alarm in  AlarmList (unAckEvent). Errno = %d", call_res);
				eventMgr_.sendEvent(
					ACS_ALH_alarmListWriteError,
					STRING_EVENT,
					"acs_alhd",
					problemData,
					ACS_ALH_Text_AlarmListUpdating_problem
				);
    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - Error impossible unAck the alarm %s", rdnValue);
    			log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
    			util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
    			return false;
			}else{
				log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::unAckAllItem() - Alarm %s unAck", rdnValue);
				util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - Alarm %s unAck", rdnValue);
			}

		}//end for

	}else{
		// Error! Impossible to retrieve the alarm list from IMM
		setError(ACS_ALH_RetrievalDataFromIMMError);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1,
			"object EXE.unAckAllItem problem. Impossible to retrieve data from IMM. ErrorCode: %d ",
			call_res
		);
		setErrorText(problemData);

		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - exit - Impossible to access into IMM ");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
		util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - exit - Returning false!!!");
		return false;
	}


	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::unAckAllItem() - exit");
	util_.trace("TRACE: acs_alh_alarm_manager::unAckAllItem() - exit");

	return true;

}


//========================================================================================
//	Count ALarms in the List
//========================================================================================

int acs_alh_alarm_manager::countAlarmsInList()
{

	// Calculate currentAlarmsInList
	util_.trace("TRACE: acs_alh_alarm_manager::countAlarmsInList() - enter");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::countAlarmsInList() - enter");

	currentAlarmsInList_ = 0;

	std::vector<AllRecord> alarmList;
	int call_res = immHandler_.getAlarmInToList(&alarmList, exOrsb);	//secondo parametro per il cpSide
	if(call_res == 0) {
		int numOfAlarmIntoList = 0;
		numOfAlarmIntoList = (int)alarmList.size();
		log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::countAlarmsInList() - alarmList.size(): %d\n", numOfAlarmIntoList);
		//printf("countAlarmsInList: alarmList.size(): %d\n", numOfAlarmIntoList);
		for(int i=0; i<numOfAlarmIntoList; i++){
			setAlarmIndex(alarmList[i].identity[2], 1);
			currentAlarmsInList_++;
			log_.write(LOG_LEVEL_DEBUG,"acs_alh_alarm_manager::countAlarmsInList() - Value of variable currentAlarmsInList_: %d, alarmIndexRecord_[%d]: %d\n ",
					currentAlarmsInList_,
					alarmList[i].identity[2],
					alarmIndexRecord_[alarmList[i].identity[2]]);
		}
	}else{
		// Error! Impossible to retrieve the alarm list from IMM
		setError(ACS_ALH_RetrievalDataFromIMMError);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1,
			"object EXE.unAckAllItem problem. Impossible to retrieve data from IMM. ErrorCode: %d ",
			call_res
		);
		setErrorText(problemData);

		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::countAlarmsInList() - Impossible to access into IMM ");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_alarm_manager::countAlarmsInList() - exit - Returning -1");
		util_.trace("TRACE: acs_alh_alarm_manager::countAlarmsInList() - exit - Returning -1");
		return -1;
	}

	log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::countAlarmsInList() - exit");
	util_.trace("TRACE: acs_alh_alarm_manager::countAlarmsInList() - exit");

	return currentAlarmsInList_;
}


int acs_alh_alarm_manager::cleanAlarmIMM(){

	std::vector<AllRecord> alarmList_EX;

	int call_res_ex = immHandler_.getAlarmInToList( &alarmList_EX, EX_SIDE);
	if (call_res_ex != 0) {
		log_.write(LOG_LEVEL_TRACE, "acs_alh_alarm_manager::cleanAlarmIMM() - getAlarmInToList error");
		return call_res_ex;
	}

	// char strToLog[256] = {0};
	char rdnValueCease[20] = {0};
	char rdnValueAl[20] = {0};

	for ( int i = 0; i < (int)alarmList_EX.size(); i++){
		if ( ( strcmp( alarmList_EX[i].event.percSeverity, "CEASING") == 0 ) && ( alarmList_EX[i].acknowledge == true ) ) {
			// An alarm is still stored in IMM even if it has been ceased and both raising / ceasing events have been
			// sent to the CP, and this one has already acknowledge both of them
			// If the CP status is NORMAL this alarm should be removed from IMM

			snprintf(rdnValueCease, sizeof(rdnValueCease) - 1, "%hu:%hd:C", alarmList_EX[i].identity[0], alarmList_EX[i].identity[2]);
			snprintf(rdnValueAl, sizeof(rdnValueAl) - 1, "%hu:%hd", alarmList_EX[i].identity[0], alarmList_EX[i].identity[2]);

			immHandler_.removeAlarmFromIMM(rdnValueCease,EX_SIDE);

			immHandler_.removeAlarmFromIMM(rdnValueAl,EX_SIDE);

			memset(rdnValueCease,0,sizeof(rdnValueCease));
			memset(rdnValueAl,0,sizeof(rdnValueAl));
		}
	}

	return 0;
}

int acs_alh_alarm_manager::cleanAlarmIMM_slowCP(){

	log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::cleanAlarmIMM_slowCP() Start");

	std::vector<AllRecord> alarmList_EX;
	std::vector<AllRecord> alarmList_SB;

	immHandler_.getAlarmInToList( &alarmList_EX, EX_SIDE);
	immHandler_.getAlarmInToList( &alarmList_SB, STANDBY_SIDE);

	char rdnValueAl[20] = {0};

	log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::cleanAlarmIMM_slowCP() search for ceasePending on EX");

	for ( int i = 0; i < (int)alarmList_EX.size(); i++){
		if (( alarmList_EX[i].ceasePending == true ) && ( strcmp(alarmList_EX[i].event.percSeverity,"CEASING") != 0 )) {

			snprintf(rdnValueAl, sizeof(rdnValueAl) - 1, "%hu:%hd", alarmList_EX[i].identity[0], alarmList_EX[i].identity[2]);

			log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::cleanAlarmIMM_slowCP() ceasePending found on EX - Alarm %s", rdnValueAl);

			int j = 0;
			for ( j = 0; j < (int)alarmList_EX.size(); j++){
				if (( alarmList_EX[i].identity[0] == alarmList_EX[j].identity[0] ) &&
				    ( alarmList_EX[i].identity[2] == alarmList_EX[j].identity[2] ) &&
				    ( strcmp(alarmList_EX[j].event.percSeverity,"CEASING") == 0 )){

					break;
				}
			}

			if ( j >= (int)alarmList_EX.size()) {
				log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::cleanAlarmIMM_slowCP() No CEASE alarm found on EX for alarm %s ", rdnValueAl);

				immHandler_.removeAlarmFromIMM(rdnValueAl,EX_SIDE);

				log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::cleanAlarmIMM_slowCP() Alarm %s has been definitively removed from IMM", rdnValueAl);
			}

			memset(rdnValueAl,0,sizeof(rdnValueAl));
		}
	}

	log_.write(LOG_LEVEL_DEBUG, "acs_alh_alarm_manager::cleanAlarmIMM_slowCP() exit");

	return 0;
}
