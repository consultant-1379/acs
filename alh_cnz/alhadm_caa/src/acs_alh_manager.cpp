/*
 * acs_alh_manager.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: xgiopap
 */

#include "acs_alh_manager.h"
#include "syslog.h"
#include "ace/Synch.h"
#include "acs_prc_api.h"
#include "acs_alh_imm_runtime_owner.h"
using namespace std;
#define IMM_RETRY_INTERVAL 50000 // wait 50ms

// Logical names for ALCO applications on CP
// both on active and passive side
const char *ALCOName[] = {"ALCOEX", "ALCOSB"};
const char exitStr[]="EXIT\n";
bool isOsafimmndRestarted = false;

const char *CP_STATE_STR[]={"WORKING",
		"SEPARATED",
		"UNDEFINED",
		"NO_CONTACT"
};

const char* ALH_STATUS_STR[] = {
		"STS_RES",
		"STS_UNC",
		"UPD_INI",
		"UPD_ACK",
		"ALA_INI",
		"ALA_CSG",
		"ALA_ACK"
};


const char* JTP_MGR_STATUS_STR[] = {
		"DISC",
		"INIT_REQ",
		"STS_REQ",
		"UPD_INI",
		"NORMAL",
		"DATA_REQ",
		"SHUTDOWN"
};

ACS_JTP_Conversation::JTP_NodeState NodeSt[] = {ACS_JTP_Conversation::NODE_STATE_ACTIVE, ACS_JTP_Conversation::NODE_STATE_PASSIVE};

ACE_Thread_Mutex mutex;
ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> _condition_attributes;
ACE_Condition<ACE_Thread_Mutex> cond_Thread(mutex,_condition_attributes);

ACE_Recursive_Thread_Mutex _acs_alh_Mutex_getEvent_Thread;

const short sizeOfOcorTransTab = 5;

struct tmp
{
    short numeric;
    char  alfa[10];
};

tmp ocorTransTab[sizeOfOcorTransTab]= {
	{2,  "APZ"},
	{7,  "APT"},
	{8,  "PWR"},
	{15, "EXT"},
	{25, "EVENT"}
};



//========================================================================================
//	Constructor
//========================================================================================
acs_alh_manager::acs_alh_manager(acs_alh_imm_connection_handler *connHandler, ACE_Condition<ACE_Thread_Mutex>* cond) :
util("CPNOTIFIER_CLASS"),
apNodeNumber_(0),
kapNodeNumber_(0),
cluster_(),
clusterName_(),
nodename_(),
cpAlarmRef_(0),
CP_state_(CP_UNDEFINED),
eventMgr_(),
problemData_(),
exeMgr_(),
jtpMgrState_(),
apNodeSite_(),
alhStatus_(),
allRecordToAlco_(),
jtpMgr_(),
term_cond_(cond),
startServiceTime_()
{

    memset(cluster_, 0, sizeof(cluster_));
    exeMgr_[EX_SIDE] = 0;
    exeMgr_[STANDBY_SIDE] = 0;
    jtpMgr_[EX_SIDE] = 0;
    jtpMgr_[STANDBY_SIDE] = 0;
    allRecordToAlco_[EX_SIDE] = 0;
    allRecordToAlco_[STANDBY_SIDE] = 0;
    alhStatus_[EX_SIDE] = STS_UNC;
    alhStatus_[STANDBY_SIDE] = STS_UNC;
    jtpMgrState_[EX_SIDE] = DISC;
    jtpMgrState_[STANDBY_SIDE] = DISC;


    connHandler_ = connHandler;
	signalTermination_ = false;
	alhManagerTermination_ = false;
	alh_event_reader_work_thread_id = 0;
	alh_cpNotifier_work_thread_id = 0;
	reboot_ = false;
	is_working_ = false;
	event_reader_thread_failed = false;
	immnd_monitoring_thread_failed = false;
	stopIMMNDMonitoringThread= false;
	waitTermination_ = false;
	synch_Imm_Access_ = false;
}


//========================================================================================
//	Destructor
//========================================================================================
acs_alh_manager::~acs_alh_manager()
{
	//printf("~acs_alh_manager() enter\n");

	for (int i = 0; i < NO_OF_CP_SIDES; ++i)
	{
		if (jtpMgr_[i]) { delete jtpMgr_[i]; }
		if (exeMgr_[i]) { delete exeMgr_[i]; }
	}

}




//========================================================================================
// 	Get AP Node Name
//========================================================================================
int acs_alh_manager::getAPNodeName()
{

	util.trace("acs_alh_manager::getAPNodeName - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::getAPNodeName - enter");

	ifstream ifs;
	string myNodeName;

	ifs.open("/etc/cluster/nodes/this/hostname");

	if (ifs.good())
		getline(ifs, myNodeName);
	else{
		util.trace("acs_alh_manager::getAPNodeName - exit - Returning -1");
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::getAPNodeName - exit - Returning -1");
		return -1;
	}

	ifs.close();

	strcpy(nodename_, myNodeName.c_str());


	util.trace("acs_alh_manager::getAPNodeName - nodename_: %s", nodename_);
	log.write(LOG_LEVEL_INFO, "acs_alh_manager::getAPNodeName - nodename_: %s", nodename_);


	util.trace("acs_alh_manager::getAPNodeName - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::getAPNodeName - exit");

	return 0;
}


//========================================================================================
// 	Create managers
//========================================================================================
bool acs_alh_manager::create_managers()
{

	util.trace("acs_alh_manager::create_managers - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::create_managers - enter");


	exeMgr_[EX_SIDE] = new (std::nothrow) acs_alh_alarm_manager(connHandler_);
	if(!exeMgr_[EX_SIDE]){
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::create_managers - Memory error allocating object: acs_alh_alarm_manager exeMgr_[%d] !",  EX_SIDE);
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::create_managers - Method: create_managers - exit - Returning -1");
		util.trace("acs_alh_manager::create_managers - exit - Returning -1");
		return false;
	}else{
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::create_managers - exeMgr_[%d] successfully created ", EX_SIDE);
	}

	exeMgr_[EX_SIDE]->setCpState(false);

	if(exeMgr_[EX_SIDE]->countAlarmsInList()==-1){
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::create_managers - call countAlarmsInList() failed");
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::create_managers - exit - Returning false");
		util.trace("TRACE: acs_alh_manager::create_managers - exit - Returning false");
		return false;
	}



	util.trace("acs_alh_manager::create_managers - Method: create_managers - exit");
	log.write(LOG_LEVEL_INFO, "acs_alh_manager::create_managers - Method: create_managers - exit");

	return true;
}



//========================================================================================
// 	Get AP Node Number
//========================================================================================
int acs_alh_manager::getAPNodeNumber()
{

	util.trace("acs_alh_manager::getAPNodeNumber - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::getAPNodeNumber - enter");

	ifstream ifs;
	string id_name;


	int apNode = 0;
	int ret_code = util.get_AP_node_number(apNode);
	if(ret_code != 0){
		util.trace("acs_alh_manager::getAPNodeNumber Impossible retrieve the apNodeNumber value from IMM - errorCode: %d", ret_code);
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::getAPNodeNumber - Impossible retrieve the apNodeNumber value from IMM - errorCode: %d", ret_code);
		apNodeNumber_ = 1;
		util.trace("acs_alh_manager::getAPNodeNumber - apNodeNumber_: %d", apNodeNumber_ );
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumber - apNodeNumber_: %d", apNodeNumber_ );
	}else{
		apNodeNumber_ = apNode;
		util.trace("acs_alh_manager::getAPNodeNumber - apNodeNumber_ fetch by IMM: %d", apNodeNumber_ );
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumber - apNodeNumber_ fetch by IMM: %d", apNodeNumber_ );
	}


//	apNodeNumber_ = 1;	// DA CAMBIARE CON IL VALORE PRESO DA IMM
	kapNodeNumber_ = 2*(apNodeNumber_-1);

	util.trace("acs_alh_manager::getAPNodeNumbe - apNodeNumber_: %i", apNodeNumber_);
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumbe - apNodeNumber_: %i", apNodeNumber_);



	ifs.open("/etc/cluster/nodes/this/id");

	if (ifs.good())
		getline(ifs, id_name);
	else{
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::getAPNodeName - Impossible to read information from file /etc/cluster/nodes/this/id");
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::getAPNodeName - exit - Returning -1");
		util.trace("acs_alh_manager::getAPNodeName - exit - Returning -1");
		return -1;
	}

	ifs.close();

	if(strcmp(id_name.c_str(), "1") == 0)
		strcpy(cluster_, "A");

	if(strcmp(id_name.c_str(), "2") == 0)
		strcpy(cluster_, "B");

	//Get the cluster information
	char clusterName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = {0};
	ret_code = util.get_cluster_name(clusterName);
	if(ret_code != 0){
		util.trace("acs_alh_manager::getAPNodeNumber - Impossible retrieve the clusterName value from IMM - errorCode: %d", ret_code);
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::getAPNodeNumber - Impossible retrieve the clusterName value from IMM - errorCode: %d", ret_code);
		strcpy(clusterName_, "-");
		util.trace("acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
	}else{
		if(clusterName != 0){
			strncpy(clusterName_, clusterName, ACS_ALH_CONFIG_CLUSTER_NAME_SIZE_MAX);
		}else{
			strcpy(clusterName_, "-");
		}
		util.trace("acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
	}


	//printf("getAPNodeNumber: %s\n", clusterName_);

	//strcpy(clusterName_, "EV1WSERI0198");

	if(strcmp(cluster_, "B") != 0)
		++kapNodeNumber_;


	util.trace("acs_alh_manager::getAPNodeNumbe - kapNodeNumber_: %u", kapNodeNumber_);
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumbe - kapNodeNumber_: %u", kapNodeNumber_);

	util.trace("acs_alh_manager::getAPNodeName - clusterName_: %s", clusterName_);
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeName - clusterName_: %s", clusterName_);

	util.trace("acs_alh_manager::getAPNodeName - cluster_: %s", cluster_);
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeName - cluster_: %s", cluster_);

	util.trace("acs_alh_manager::getAPNodeName - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::getAPNodeName - exit");


	return 0;
}


//========================================================================================
// 	Create JTP manager
//========================================================================================
bool acs_alh_manager::createJtp()
{

	util.trace("acs_alh_manager::createJtp - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::createJtp - enter");


	for (int i = 0; i < NO_OF_CP_SIDES; ++i)
	{
		jtpMgr_[i] = new (std::nothrow) acs_alh_jtpmgr(eventMgr_, OFFSET_AP_NODE + kapNodeNumber_, ALCOName[i], NodeSt[i]);
		if(!jtpMgr_[i]){
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::createJtp - Memory error allocating object: acs_alh_jtpmgr_[%d] !",  i);
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::createJtp - exit - Returning -1");
			util.trace("acs_alh_manager::createJtp - Method: create_managers - exit - Returning -1");
			return false;
		}else{
			log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::createJtp - acs_alh_jtpmgr_[%d] successfully created ", i);
		}
	}

	util.trace("acs_alh_manager::createJtp - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::createJtp - exit");

	return true;
}



//========================================================================================
// 	Send to CP
//========================================================================================
int acs_alh_manager::sendToCP()
{

	util.trace("acs_alh_manager::sendToCP - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::sendToCP - enter");

	int retCode = 0;
	int numOfLoops(LIMIT_FOR_CONNECT_ATTEMPT);
	AlaRecordStruct alaRecordToAlco;

	for (int i = 0; i < NO_OF_CP_SIDES; ++i)
	{
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.sendToCP() iterazione numero %i - waiting...", i);
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.sendToCP() NO_OF_CP_SIDES %i - waiting...", NO_OF_CP_SIDES);
		// Skip attempt to send alarm to other side in case sides not separated.
		if (i == STANDBY_SIDE && CP_state_ != CP_SEPARATED) {
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager.sendToCP() continue selected");
			continue;
		}

		jtpMgr_[i]->fetchJtpMgrState(jtpMgrState_[i]);

		util.trace("acs_alh_manager::sendToCP - CP_state_= %d - jtpMgrState[%d]: %s",
				CP_state_,
				i,
				JTP_MGR_STATUS_STR[jtpMgrState_[i]]);

		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::sendToCP - CP_state_= %d - jtpMgrState[%d]: %s",
				CP_state_,
				i,
				JTP_MGR_STATUS_STR[jtpMgrState_[i]]);

		if ((jtpMgrState_[i] == DISC) && (numOfLoops < LIMIT_FOR_CONNECT_ATTEMPT))
		{
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager.sendToCP() jtpMgrState_[i] == DISC) && (numOfLoops < LIMIT_FOR_CONNECT_ATTEMPT)");
			numOfLoops++;
		}
		else
		{
			if (i != 0)
				numOfLoops = 1;

			// Check status for ALCO connection and react accordingly
			//ACE_Guard<ACE_Recursive_Thread_Mutex> guard_alarmManager(_acs_alh_Mutex_);

			while ( synch_Imm_Access_ == true ){
				usleep ( IMM_RETRY_INTERVAL ); // wait 0.2 sec
				log.write(LOG_LEVEL_ERROR, "acs_alh_manager.sendToCP() synch_Imm_Access = true - waiting...");
			}

			synch_Imm_Access_ = true;

			switch (jtpMgr_[i]->getStatus(alhStatus_[i], &cpAlarmRef_))
			{
				case JtpMgr_DISC:
					//Call trace function
					util.trace("acs_alh_manager::sendToCP - handling JtmMgrState: DISC");
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - handling JtmMgrState: DISC");

					// Line disconnected, clear the current send request
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i] = 0 after DISC",i);
					allRecordToAlco_[i] = 0;
					retCode = 1;
					sleep(3);
					break;

				case JtpMgr_RES:
					//Call trace function
					util.trace("acs_alh_manager::sendToCP - handling JtmMgrState: RES");
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - handling JtmMgrState: RES");

					// Force resend of all items in Alarm List
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i] = 0 after RES",i);
					allRecordToAlco_[i] = 0;
					exeMgr_[i]->unAckAllItem();
					retCode = 1;
					break;

				case JtpMgr_NORMAL:{
					//Call trace function
					util.trace("acs_alh_manager::sendToCP - handling JtmMgrState: NORMAL");
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - handling JtmMgrState: NORMAL");

					// Possible to send alarm or alarm ceasing
					// Check if old one waiting
					/*if (alhStatus_[i] != STS_UNC)
					{
						exeMgr_[i]->setRebootFlag(0);
					}*/

					alhStatus_[i] = STS_UNC;


					if (!allRecordToAlco_[i])
					{
						if (!clearAlarmTemp(i)) {
							log.write(LOG_LEVEL_ERROR, "acs_alh_manager::sendToCP - clearAlarmTemp(%i) failed", i);
							return 1;
						}

						if (exeMgr_[i]->getUnAckItem(alarmTemp[i], CP_state_) == 0)
						{
							allRecordToAlco_[i] = &(alarmTemp[i]);

							log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i] = %u",i,allRecordToAlco_[i]);
							log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].sendpriority = %u",i,allRecordToAlco_[i]->sendPriority);
							log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].identity[0] = %u",i,allRecordToAlco_[i]->identity[0]);
							log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].identity[1] = %u",i,allRecordToAlco_[i]->identity[1]);

							retCode = 1;

							if (allRecordToAlco_[i]->sendPriority)
							{
								// Alarm to be sent
								alaRecordToAlco.alaIni.apNode[1]     = (unsigned char)(OFFSET_AP_NODE + kapNodeNumber_);
								alaRecordToAlco.alaIni.apNode[0]     = 0;
								alaRecordToAlco.alaIni.alarmId[0][0] = (unsigned char)allRecordToAlco_[i]->identity[0];
								alaRecordToAlco.alaIni.alarmId[0][1] = allRecordToAlco_[i]->identity[0] >> 8;
								alaRecordToAlco.alaIni.alarmId[1][0] = 0;
								alaRecordToAlco.alaIni.alarmId[1][1] = 0;
								alaRecordToAlco.alaIni.alarmId[2][0] = (unsigned char)allRecordToAlco_[i]->identity[2];
								alaRecordToAlco.alaIni.alarmId[2][1] = allRecordToAlco_[i]->identity[2] >> 8;
								alaRecordToAlco.alaIni.alarmClass    = allRecordToAlco_[i]->sendPriority - 1;

								for (int l = 0; l < sizeOfOcorTransTab; l++)
								{
									if (!strcmp(ocorTransTab[l].alfa, allRecordToAlco_[i]->event.objClassOfReference))
									{
										alaRecordToAlco.alaIni.alarmCategory = (unsigned char)ocorTransTab[l].numeric;
										break;
									}
								} // End of for-loop

								strcpy(alaRecordToAlco.alaIni.buffer, allRecordToAlco_[i]->event.probableCause);

								//Get the cluster information
								int ret_code = 0;
								char clusterName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = {0};
								ret_code = util.get_cluster_name(clusterName);
								if(ret_code != 0){
									util.trace("acs_alh_manager::getAPNodeNumber - Impossible retrieve the clusterName value from IMM - errorCode: %d", ret_code);
									log.write(LOG_LEVEL_ERROR, "acs_alh_manager::getAPNodeNumber - Impossible retrieve the clusterName value from IMM - errorCode: %d", ret_code);
									strcpy(clusterName_, "-");
									util.trace("acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
									log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
								}else{
									if(clusterName != 0){
										strncpy(clusterName_, clusterName, ACS_ALH_CONFIG_CLUSTER_NAME_SIZE_MAX);
									}else{
										strcpy(clusterName_, "-");
									}
									util.trace("acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
									log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::getAPNodeNumber - clusterName set to value: %s", clusterName_);
								}

								snprintf(apNodeSite_, sizeof(apNodeSite_) - 1, (apNodeNumber_ > 9)? "\n\n%-6s%-20s%-10s%s\n%-6u%-20s%-10s%s\n":"\n\n%-6s%-20s%-10s%s\n %-5u%-20s%-10s%s\n",
											"AP", "APNAME", "NODE", "NODENAME", apNodeNumber_, clusterName_, cluster_, nodename_);

								strcat(alaRecordToAlco.alaIni.buffer, apNodeSite_);

								if (strlen(allRecordToAlco_[i]->event.problemText))
								{
									strcat(alaRecordToAlco.alaIni.buffer, "\n");
									strcat(alaRecordToAlco.alaIni.buffer, allRecordToAlco_[i]->event.problemText);
								}

								unsigned short bufSize = strlen(alaRecordToAlco.alaIni.buffer);
								alaRecordToAlco.alaIni.bufSize[1] = (unsigned char)bufSize;
								alaRecordToAlco.alaIni.bufSize[0] = bufSize >> 8;



								if ((jtpMgr_[i]->sendData((unsigned char*)&alaRecordToAlco.alaIni, sizeOfNonBufferAlaIni + bufSize, ALA_INI, allRecordToAlco_[i]->retransCnt)) == JtpMgr_NORMAL)
								{
									allRecordToAlco_[i]->retransCnt = 1;

									util.trace("acs_alh_manager::sendToCP - handling JtmMgrState: NORMAL - send Alarm to CP: %d:%d",
											allRecordToAlco_[i]->identity[0],
											allRecordToAlco_[i]->identity[2]);
									log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - handling JtmMgrState: NORMAL - send Alarm to CP: %d:%d",
											allRecordToAlco_[i]->identity[0],
											allRecordToAlco_[i]->identity[2]);

								}else{
									log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i] = 0 after NORMAL - ALARM",i);
									allRecordToAlco_[i] = 0;
								}
							}
							else
							{
								log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - ceasing part : Alarm Ceasing to be sent");
								log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - ceasing part : Alarm t ocease below: ");
								log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].identity[0] = %u",i,allRecordToAlco_[i]->identity[0]);
								log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].identity[1] = %u",i,allRecordToAlco_[i]->identity[1]);
								// Alarm Ceasing to be sent
								alaRecordToAlco.alaCsg.apNode[1] = (unsigned char)(OFFSET_AP_NODE + kapNodeNumber_);
								alaRecordToAlco.alaCsg.apNode[0] = 0;
								alaRecordToAlco.alaCsg.alarmId[0][0] = (unsigned char)allRecordToAlco_[i]->identity[0];
								alaRecordToAlco.alaCsg.alarmId[0][1] = allRecordToAlco_[i]->identity[0] >> 8;
								alaRecordToAlco.alaCsg.alarmId[1][0] = 0;
								alaRecordToAlco.alaCsg.alarmId[1][1] = 0;
								alaRecordToAlco.alaCsg.alarmId[2][0] = (unsigned char)allRecordToAlco_[i]->identity[2];
								alaRecordToAlco.alaCsg.alarmId[2][1] = allRecordToAlco_[i]->identity[2] >> 8;
								alaRecordToAlco.alaCsg.cpAlarmRef[0] = (unsigned char)allRecordToAlco_[i]->cpAlarmRef;
								alaRecordToAlco.alaCsg.cpAlarmRef[1] = allRecordToAlco_[i]->cpAlarmRef >> 8;

								if ((jtpMgr_[i]->sendData((unsigned char*)&alaRecordToAlco.alaCsg, sizeof(struct AlaCsgStruct), ALA_CSG, allRecordToAlco_[i]->retransCnt)) == JtpMgr_NORMAL)
								{
									allRecordToAlco_[i]->retransCnt = 1;

									util.trace("acs_alh_manager::sendToCP - handling JtmMgrState: NORMAL - send ceasing to CP: %d:%d",
											allRecordToAlco_[i]->identity[0],
											allRecordToAlco_[i]->identity[2]);
									log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - handling JtmMgrState: NORMAL - send ceasing to CP: %d:%d",
											allRecordToAlco_[i]->identity[0],
											allRecordToAlco_[i]->identity[2]);
								}
								else {
									log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i] = 0 after NORMAL - CEASING",i);
									allRecordToAlco_[i] = 0;
								}
							}
						}
					}
					else
						retCode = 1;
				}
					break;

				case JtpMgr_DATA_REC:

					util.trace("acs_alh_manager::sendToCP - handling JtmMgrState: DATA_REQ");
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - handling JtmMgrState: DATA_REQ");

					if ( exeMgr_[EX_SIDE] )
						log.write(LOG_LEVEL_TRACE, "acs_alh_manager.sendToCP() - exeMgt_[EX_SIDE].get_exOrsb = %i",exeMgr_[EX_SIDE]->get_exOrsb());
					if ( exeMgr_[STANDBY_SIDE] )
						log.write(LOG_LEVEL_TRACE, "acs_alh_manager.sendToCP() - exeMgt_[STANDBY_SIDE].get_exOrsb = %i",exeMgr_[STANDBY_SIDE]->get_exOrsb());

					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].sendpriority = %u",i,allRecordToAlco_[i]->sendPriority);
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].identity[0] = %u",i,allRecordToAlco_[i]->identity[0]);
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i].identity[1] = %u",i,allRecordToAlco_[i]->identity[1]);
					// ALA_ACK received. Tell Alarm List and update Alarm List file
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - invio dell ACKALARM exeMgr_[%i]->ackAlarm",i);
					if (exeMgr_[i]->ackAlarm(allRecordToAlco_[i], cpAlarmRef_) == false)
					{
						eventMgr_.sendEvent(
								exeMgr_[i]->getError(),
								STRING_EVENT,
								ALCOName[i],
								exeMgr_[i]->getErrorText(),
								ACS_ALH_Text_EventTreating_problem);
						// Continue. Ignore the error right now. Take care of it later
					}

					exeMgr_[i]->cleanAlarmIMM_slowCP();
					log.write(LOG_LEVEL_INFO, "acs_alh_manager::sendToCP - allRecordToAlco_[%i] = 0 after DATA_REC",i);
					allRecordToAlco_[i] = 0;
					retCode = 1;
					break;

				default:
					break;
			}

			synch_Imm_Access_ = false;
		}
	}

	util.trace("acs_alh_manager::sendToCP - exit - Returning %d", retCode);
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::sendToCP - exit - Returning %d", retCode);

	return retCode;
}





//========================================================================================
// 	Check ALCO connection
//========================================================================================
void acs_alh_manager::checkALCO()
{

	util.trace("acs_alh_manager::checkALCO - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::checkALCO - enter");

	//int alco_connection_check_index = 0;

	if (allRecordToAlco_[EX_SIDE] == 0) //&& !(++alco_connection_check_index % 60) ) // Don't know why this %60 check, but leave it says uablmgr. UABMAHA
	{
		//alco_connection_check_index = 1;	// Don't want to get outside range

		if (!jtpMgr_[EX_SIDE]->checkALCOConnection()){
			jtpMgrState_[EX_SIDE] = DISC;
			util.trace("acs_alh_manager::checkALCO - jtpMgrState_[EX_SIDE]: %i ", jtpMgrState_[EX_SIDE]);
			log.write(LOG_LEVEL_INFO, "acs_alh_manager::checkALCO - jtpMgrState_[EX_SIDE]: %i ", jtpMgrState_[EX_SIDE]);
		}

		if (!jtpMgr_[STANDBY_SIDE]->checkALCOConnection()){
			jtpMgrState_[STANDBY_SIDE] = DISC;
			util.trace("acs_alh_manager::checkALCO - jtpMgrState_[STANDBY_SIDE]: %i ", jtpMgrState_[STANDBY_SIDE]);
			log.write(LOG_LEVEL_INFO, "acs_alh_manager::checkALCO - jtpMgrState_[STANDBY_SIDE]: %i ", jtpMgrState_[STANDBY_SIDE]);
		}
	}

	util.trace("acs_alh_manager::checkALCO - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::checkALCO - exit");
}


//========================================================================================
// 	Method isApReboot
//========================================================================================
bool acs_alh_manager::isApReboot(){

	util.trace("acs_alh_manager::isApReboot - enter ");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::isApReboot - enter");

	ACS_PRC_API prchandler;

	time_t startTime = 0;
	time_t lastRebootTime = 0;
	time_t lastStartTime = 0;

	//Time of last start of service
	char buffLastStartTime[20] = { 0 };
	memset(buffLastStartTime, 0, sizeof(buffLastStartTime));
	util.get_last_alh_start_time(buffLastStartTime);

	if(buffLastStartTime != 0){
		lastStartTime = convertEventTime(buffLastStartTime);
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - last start time: %s", buffLastStartTime);
		util.trace("acs_alh_manager::isApReboot - last start time: %s", buffLastStartTime);
	}else{
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - last start time not available");
		util.trace("acs_alh_manager::isApReboot - last start time not available");
	}


	//Time of last reboot
	lastRebootTime = prchandler.lastRebootTime();
	char buffRebootTime[20];
	strftime(buffRebootTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&lastRebootTime));
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - last reboot time: %s", buffRebootTime);

	//MODIFY for TR HP54702
	//Start time
//	startTime = time(0);
//	char buffStartTime[20];
//	strftime(buffStartTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&startTime));
//	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - start time: %s", buffStartTime);
//	util.set_last_alh_start_time(buffStartTime);
	//END

	//ADD for TR HP54702
	startTime = time(0);
	strftime(startServiceTime_, 20, "%Y-%m-%d %H:%M:%S", localtime(&startTime));
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - start time: %s", startServiceTime_);
	//END

	if(lastStartTime > lastRebootTime){
		reboot_ = false;
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - No reboot occurs from last start of ALH service");
	}else{
		reboot_ = true;
		alhStatus_[EX_SIDE] = STS_RES;
		alhStatus_[STANDBY_SIDE] = STS_RES;
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::isApReboot - Node is reboot");

	}

	util.trace("acs_alh_manager::isApReboot - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::isApReboot - exit");

	return reboot_;

}

//========================================================================================
// 	Store timestamp information into IMM - ADD for TR HP54702
//========================================================================================
bool acs_alh_manager::storeTimestampInformation(){

	bool res = false;
	int ret_code = 0;
	int flags[2] = {1, 1};

	util.trace("acs_alh_manager::storeTimestampInformation - enter ");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::storeTimestampInformation - enter");


	ret_code = util.set_last_alh_start_time(startServiceTime_);
	if(ret_code == 0){
		res = true;
		if(reboot_){
			do{
				ret_code = util.set_cp_alignment_flag(flags);
				if (ret_code == 0){
					res = true;
					log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::storeTimestampInformation - set the cpAlignment flags to 1");
					break;
				}else{
					res = false;
					log.write(LOG_LEVEL_ERROR, "acs_alh_manager::storeTimestampInformation - Impossible to set the cpAlignment flags to 1. Retry...");
					sleep(1);
				}
			}while(ret_code != 0);

		}else{
			log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::storeTimestampInformation - No AP node Reboot occurred.");
		}
	}else{
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::storeTimestampInformation - Impossible to store start service time indication "
						"into IMM. StartServiceTime_: %s", startServiceTime_ );
	}

	util.trace("acs_alh_manager::storeTimestampInformation - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::storeTimestampInformation - exit");

	return res;
}


//========================================================================================
// 	Get event thread
//========================================================================================

static ACE_THR_FUNC_RETURN getEventThread(void* mcp){


	if (((acs_alh_manager *)mcp)->getEvent() == -1)
	{

		//cout<<"THREAD EVENT READER FALLITO\n"<<endl;

		((acs_alh_manager *)mcp)->set_event_reader_thread_failed_flag(true);
		ACE_Thread_Manager::instance()->exit((void*)-1, 0);
		return (ACE_THR_FUNC_RETURN)-1;
	}

	//cout<<"USCITO DAL THREAD EVENT READER\n"<<endl;
	ACE_Thread_Manager::instance()->exit(0, 0);

	return (ACE_THR_FUNC_RETURN)0;
}



//========================================================================================
// 	getCPNotifierThread thread
//========================================================================================

static ACE_THR_FUNC_RETURN getCPNotifierThread(void* mcp){

	if (((acs_alh_cpnotifier *)mcp)->getCpState() == -1)
	{

		//cout<<"THREAD CP NOTIFIER READER FALLITO\n"<<endl;

		return (ACE_THR_FUNC_RETURN)-1;
	}

	//cout<<"USCITO DAL THREAD CP NOTIFIER\n"<<endl;
	ACE_Thread_Manager::instance()->exit(0, 0);

	return (ACE_THR_FUNC_RETURN)0;
}

//========================================================================================
// 	getIMMNDStateThread thread
//========================================================================================
static ACE_THR_FUNC_RETURN getIMMNDStateThread(void* mcp){

		if (((acs_alh_manager *)mcp)->getIMMNDState() == -1)
		{
			((acs_alh_manager *)mcp)->set_immnd_monitoring_thread_failed(true);
			return (ACE_THR_FUNC_RETURN)-1;
		}
		ACE_Thread_Manager::instance()->exit(0, 0);
		return (ACE_THR_FUNC_RETURN)0;
}

//========================================================================================
// 	getrunALHtThread thread
//========================================================================================

static ACE_THR_FUNC_RETURN getrunALHThread(void* mcp){

	if (((acs_alh_manager *)mcp)->runALH() == -1) {
		return (ACE_THR_FUNC_RETURN)-1;
	}

	ACE_Thread_Manager::instance()->exit(0, 0);

	return (ACE_THR_FUNC_RETURN)0;
}





//========================================================================================
// 	start_activity thread
//========================================================================================

int acs_alh_manager::start_activity(){
	util.trace("TRACE: start_activity() - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.start_activity() - enter");

	/* spawn thread */
	const ACE_TCHAR* thread_name = "ALH_Manager_Thread";

	int call_result = ACE_Thread_Manager::instance()->spawn(&getrunALHThread,
										(void *)this,
										THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
										&alh_manager_work_thread_id,
										0,
										ACE_DEFAULT_THREAD_PRIORITY,
										-1,
										0,
										ACE_DEFAULT_THREAD_STACKSIZE,
										&thread_name);
	if (call_result == -1)
	{

		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.start_activity() Start %s failure. Error code: %d",thread_name, call_result );
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.start_activity() - exit - Returning -1!!!");
		util.trace("TRACE: start_activity() - exit - Returning -1!!!");
		return -1;

	}

	util.trace("TRACE: start_activity() - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.start_activity() - exit");

	return 0;
}




//========================================================================================
// 	stop_activity thread
//========================================================================================

int acs_alh_manager::stop_activity(){

	util.trace("TRACE: stop_activity() - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.stop_activity() - enter");

	alhManagerTermination_ = true;

	util.trace("TRACE: stop_activity() - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.stop_activity() - exit");

	return 0;
}

//========================================================================================
// 	startEventReadThread thread
//========================================================================================

int acs_alh_manager::startEventReadThread()
{

	util.trace("TRACE: startEventReadThread() - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.startEventReadThread() - enter");

	/* spawn thread */
	const ACE_TCHAR* thread_name = "Event_Reader_Thread";

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
	if ((event_reader_thread_failed = (call_result == -1)))
	{
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.startEventReadThread() Start %s failure. Error code: %d",thread_name, call_result );
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.startEventReadThread() - exit - Returning -1!!!");
		util.trace("TRACE: startEventReadThread() - exit - Returning -1!!!");
		return -1;

	}

	util.trace("TRACE: startEventReadThread() - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.startEventReadThread() - exit");

	return 0;
}


//========================================================================================
// 	stopEventReadThread Method
//========================================================================================

int acs_alh_manager::stopEventReadThread(){

	util.trace("TRACE: stopEventReadThread() - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.stopEventReadThread() - enter");

	FILE *pipe;

	//Open the pipe e return a pointer to the stream
	pipe = fopen(ALH_FIFO_NAME, "w");
	if( pipe == 0){
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.stopEventReadThread() Cannot open write only alhfifo. Returning -1");
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "object acs_alh_manager.stopEventReadThread problem\nCannot open read only alhfifo. Returning -1");
		return -1;
	}

	signalTermination_ = true;
	fwrite(exitStr, strlen(exitStr), 1, pipe);

	//printf("Numero of byte written: %d\n", n);
	log.write(LOG_LEVEL_WARN, "acs_alh_manager.stopEventReadThread() - Write in the pipe the termination string");
	util.trace("TRACE: stopEventReadThread() - Write in the pipe the termination string");

	fclose(pipe);

	util.trace("TRACE: stopEventReadThread() - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.stopEventReadThread() - exit");

	return 0;
}


//========================================================================================
// 	startCpNotifierThread Method
//========================================================================================

int acs_alh_manager::startCpNotifierThread(acs_alh_cpnotifier *cpNotifier_ptr){

	util.trace("TRACE: startCpNotifierThread() - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.startCpNotifierThread() - enter");


	/* spawn thread */
	const ACE_TCHAR* thread_name = "CP_Notifier_Thread";

	int call_result = ACE_Thread_Manager::instance()->spawn(&getCPNotifierThread,
										(void *)cpNotifier_ptr,
										THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
										&alh_cpNotifier_work_thread_id,
										0,
										ACE_DEFAULT_THREAD_PRIORITY,
										-1,
										0,
										ACE_DEFAULT_THREAD_STACKSIZE,
										&thread_name);

	if (call_result == -1)
	{

		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.startCpNotifierThread() Start %s failure. Error code: %d",thread_name, call_result );
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.startCpNotifierThread() - exit - Returning -1!!!");
		util.trace("TRACE: startCpNotifierThread() - exit - Returning -1!!!");
		return -1;

	}

	util.trace("TRACE: startCpNotifierThread() - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.startCpNotifierThread() - exit");

	return 0;
}


int acs_alh_manager::startIMMNDMonitoringThread()
{

	util.trace("TRACE: startEventReadThread() - enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.startEventReadThread() - enter");

	/* spawn thread */
	const ACE_TCHAR* thread_name = "IMMND_Monitoring_Thread";

	int call_result = ACE_Thread_Manager::instance()->spawn(&getIMMNDStateThread,
										(void *)this,
										THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
										&alh_immnd_monitoring_thread_id,
										0,
										ACE_DEFAULT_THREAD_PRIORITY,
										-1,
										0,
										ACE_DEFAULT_THREAD_STACKSIZE,
										&thread_name);
	if (call_result == -1)
	{
		immnd_monitoring_thread_failed = true;
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.startIMMNDMonitoringThread() Start %s failure. Error code: %d",thread_name, call_result );
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.startIMMNDMonitoringThread() - exit - Returning -1!!!");
		util.trace("TRACE: startIMMNDMonitoringThread() - exit - Returning -1!!!");
		return -1;

	}

	util.trace("TRACE: startIMMNDMonitoringThread() - exit");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.startIMMNDMonitoringThread() - exit");

	return 0;
}

int acs_alh_manager::getIMMNDState(){
	log.write(LOG_LEVEL_TRACE,"Entry in getIMMNDState()");
	int pidValue=-1,retcode=0;
	FILE *fp;
	char buff[10];
	struct stat buf;
	const char *filepath;
	string str;
	memset(buff,'\0',10);
	while(!get_IMMND_thread_termination())
	{
	       retcode = system("pidof osafimmnd > /tmp/pidof");
	       if(retcode == 0)
	       {
	           fp = fopen("/tmp/pidof", "r");
	           if(fp!= NULL)
	           {
	        	  fscanf(fp, "%d", &pidValue);
	        	  fclose(fp);
	        	  log.write(LOG_LEVEL_TRACE,"Pid Value of osafimmnd %d",pidValue);
	        	  sprintf(buff,"%d",pidValue);
	        	  str=std::string("/proc/")+std::string(buff)+std::string("/status");
	        	  filepath=str.c_str();

	        	  log.write(LOG_LEVEL_TRACE, "filepath = %s",filepath);
	        	  log.write(LOG_LEVEL_TRACE, "isOsafimmndRestarted set to %d",isOsafimmndRestarted);

	        	  while(!isOsafimmndRestarted && !get_IMMND_thread_termination())
	        	  {
	        		if(stat(filepath, &buf) != 0)
	        		{
	                    isOsafimmndRestarted = true;
	                    log.write(LOG_LEVEL_WARN, "isOsafimmndRestarted set to true");
	                    filepath =NULL;
	        		}
	        		sleep(3);
	        	  }
	        	}
	        }//end of if
	       else{
	    	   log.write(LOG_LEVEL_TRACE,"Pid not exists for osafimmnd");
	    	   sleep(3);
	       }
	 }//end of while
	return  0;
	}


//========================================================================================
// 	getEvent Method
//========================================================================================

int acs_alh_manager::getEvent(){

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_alarmManager(_acs_alh_Mutex_getEvent_Thread);

	util.trace("TRACE: getEvent() enter");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() enter");

	event_reader_thread_failed = false;

	ACS_ALH_AlarmMessage eventMessage;
	char buffer[MAX_BUF_SIZE_ALMSG] = {0};
	std::string recordEvent;

	bool exit_event_received = false;

	int countEvent = 0;

	//Open the pipe e return a pointer to the stream
	pipe = fopen(ALH_FIFO_NAME, "r+");		//TR HU50325

	if( pipe == 0){
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.getEvent() exit - Cannot open (read mode) alhfifo. Returning -1");
		util.trace("TRACE: acs_alh_manager.getEvent() exit - Cannot open (read mode) alhfifo. Returning -1");
		//setError(acs_alh::ERR_SYSTEM_OPEN);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "Cannot open alhfifo. (read mode)");
		eventMgr_.sendEvent(
				ACS_ALH_EventLogInitializationError,
				STRING_EVENT,
				"ALH-EventLog",
				problemData,
				ACS_ALH_Text_EventLog_problem
					);

		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.getEvent() - Cannot open (read mode) alhfifo - Try to acquire mutex");
		cond_Thread.mutex().acquire();
		event_reader_thread_failed = true;
		cond_Thread.signal();
		cond_Thread.mutex().release();
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.getEvent() - Cannot open (read mode) alhfifo - Mutex released");

		return -1;
	}

	while (1) {

		if (signalTermination_) {
			if( exit_event_received ){
				util.trace("TRACE: acs_alh_manager.getEvent() - Event_Reader_Thread - start termination process");
				log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() - Event_Reader_Thread - start termination process");
			}
			else {
				util.trace("TRACE: acs_alh_manager.getEvent() - Event_Reader_Thread - signalTeminination flag is true but not EXIT event received");
				log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() - Event_Reader_Thread - signalTeminination flag is true but not EXIT event received");
			}
			waitTermination_ = true;
			log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() - waitTermination_ = true");
			break;
		}

		char * fgets_result = fgets(buffer, sizeof(buffer), pipe);

		//Read new event message from alhpipe
		if (fgets_result) {
			countEvent++;
			log.write(LOG_LEVEL_DEBUG, "acs_alh_manager.getEvent(). Read new even message. Number of event message read: %d", countEvent);
		} else {
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager.getEvent() exit - Cannot read event from alhfifo. Returning -1");
			util.trace("TRACE: acs_alh_manager.getEvent() exit - Cannot read event from alhfifo. Returning -1");
			//setError(acs_alh::ERR_SYSTEM_READ);
			char problemData[DEF_MESSIZE] = {0};
			snprintf(problemData, sizeof(problemData) - 1, "Problem during the reading of event from alhfifo.");
			eventMgr_.sendEvent(
				ACS_ALH_EventReadingError,
				STRING_EVENT,
				"ALH-EventLog",
				problemData,
				ACS_ALH_Text_EventReading_problem
				);

			fclose(pipe);
			log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() - Cannot read event from alhfifo - Try to acquire mutex cond_Thread");
			cond_Thread.mutex().acquire();
			log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() - Cannot read event from alhfifo - Mutex cond_Thread acquired");
			event_reader_thread_failed = true;
			cond_Thread.signal();
			cond_Thread.mutex().release();
			log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() - Cannot read event from alhfifo - Mutex cond_Thread released");
			return -1;
		}

		//Check if the buffer is empty
		if(buffer != 0){

			recordEvent = buffer;

			if(strcmp(recordEvent.c_str(), exitStr) == 0){
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager.getEvent(). Received termination signal");
				util.trace("TRACE: acs_alh_manager.getEvent(). Received termination signal");
				exit_event_received = true;
				continue;
			}

			//Check if the recordEvent is an APEvent
			switch ( checkEventMessage(recordEvent) )
			{
				case ACS_ALH_EVENT_OK :
					log.write(LOG_LEVEL_WARN, "acs_alh_manager.getEvent(). Read event: %s", recordEvent.c_str());

					getEventMessage(eventMessage, recordEvent);

					//ACE_Guard<ACE_Recursive_Thread_Mutex> guard_alarmManager(_acs_alh_Mutex_);

					while ( synch_Imm_Access_ == true ){
						usleep ( IMM_RETRY_INTERVAL ); // wait 0.2 sec
						log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() synch_Imm_Access = true - waiting...");
					}

					synch_Imm_Access_ = true;

					if (treatEvent(eventMessage) == false){
						util.trace("TRACE: acs_alh_manager.getEvent() treatEvent failed. The event is skipped");
						log.write(LOG_LEVEL_ERROR, "acs_alh_manager.getEvent() treatEvent failed. The event is not stored in IMM, the event is skipped");
					}

					synch_Imm_Access_ = false;
					break;

				case ACS_ALH_EVENT_DISCARDED :
					log.write(LOG_LEVEL_WARN, "acs_alh_manager.getEvent(). Event discarded. It isn't an APEvent");
					break;

				case ACS_ALH_EVENT_CORRUPTED :
					log.write(LOG_LEVEL_ERROR, "acs_alh_manager.getEvent(). The event read from alhfifo is corrupted ( some fields are missing )");
					util.trace("TRACE: acs_alh_manager.getEvent() exit - The event read from alhfifo is corrupted ( some fields are missing )");

					char problemData[DEF_MESSIZE] = {0};
					snprintf(problemData, sizeof(problemData) - 1, "Problem during the reading of event from alhfifo.");
					eventMgr_.sendEvent(
						ACS_ALH_EventCorrupted,
						STRING_EVENT,
						"ALH-EventLog",
						problemData,
						ACS_ALH_Text_Event_corrupted
						);
					break;
			}

			//Erase the buffer
			memset(buffer, 0, sizeof(buffer));
		}

	}//end while

	fclose(pipe);

	util.trace("TRACE: acs_alh_manager.getEvent() - exit - Returning: 0");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.getEvent() exit - Returning: 0");

	return 0;
}

//========================================================================================
//	getEventMessage method
//========================================================================================

void acs_alh_manager::getEventMessage(ACS_ALH_AlarmMessage& event_message, std::string record){

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
		log.write(LOG_LEVEL_WARN, "acs_alh_eventreader.getEventMessage(). Time conversions failed.");
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

int acs_alh_manager::checkEventMessage(std::string record){

	size_t found;

	found = record.find("APEvent");

	//Check that the "APEvent" tag is located in right position.
	if( (found != string::npos) && ((int)found == 20) ){
		if ( ( record.find("CLASS_REF") != string::npos ) &&
			 ( record.find("OBJ_REF") != string::npos ) &&
			 ( record.find("P_DATA") != string::npos ) &&
			 ( record.find("P_TEXT") != string::npos ) ){
			return ACS_ALH_EVENT_OK;
		}
		else {
			return ACS_ALH_EVENT_CORRUPTED;
		}
	}
	else {
		return ACS_ALH_EVENT_DISCARDED;
	}
}


//-------------------------------------------------------------------------------------------------------------------------------------------------
//convertEventTime method
//-------------------------------------------------------------------------------------------------------------------------------------------------

time_t acs_alh_manager::convertEventTime(std::string eventData, std::string eventTime){

	struct tm when;

	time_t t;

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

	t = mktime(&when);

	return t;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//convertEventTime method
//-------------------------------------------------------------------------------------------------------------------------------------------------
time_t acs_alh_manager::convertEventTime(const char *timeStr){

	struct tm when;

	int yy = 0;
	int mm = 0;
	int dd = 0;
	int hh = 0;
	int m = 0;
	int ss = 0;

	sscanf(timeStr, "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hh, &m, &ss);
	// years since 1900
	when.tm_year = yy - 1900;
	// months since January
	when.tm_mon = mm - 1;
	// day of the month
	when.tm_mday = dd;

	when.tm_sec = ss;

	when.tm_min = m;

	when.tm_hour = hh;

	when.tm_isdst = -1; // autodetect DST

	return mktime(&when);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//getFormattedProblemDataText method
//-------------------------------------------------------------------------------------------------------------------------------------------------
std::string acs_alh_manager::getFormattedProblemDataText(std::string p_text){
	std::string newProblem;

	newProblem = p_text;
	util.myReplace(newProblem, "##", "\n");

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
bool  acs_alh_manager::treatEvent(ACS_ALH_AlarmMessage& alarmMessage)
{

	util.trace("TRACE: acs_alh_manager::treatEvent() entered");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager.treatEvent() entered");

	for (int i = 0; i < NO_OF_CP_SIDES; ++i)
	{

		log.write(LOG_LEVEL_TRACE, "acs_alh_manager.treatEvent() - i = %d , CP_state_ = %d", i, CP_state_);

		// Skip connect to other side in case sides not separated.
		if ((i == STANDBY_SIDE) /*&& (CP_state_ != CP_SEPARATED)*/)
		{
			continue;
		}

		// Put the event into Alarm List
		if (exeMgr_[i]->treatEvent(alarmMessage) == false)
		{

//			// If error, report it and exit
//			char problemData[DEF_MESSIZE];
//
//			snprintf(problemData, sizeof(problemData) - 1, "acs_alhd\n%s", exeMgr_[i]->getErrorText());
//
//			eventMgr_.sendEvent(
//					exeMgr_[i]->getError(),
//					STRING_EVENT,
//					ALCOName[i],
//					problemData,
//					ACS_ALH_Text_EventTreating_problem);

			return false;
		}//end if

	}//end for

	return true; // Ok

}


//========================================================================================
// handleCpSeparatedState Method
//========================================================================================

bool acs_alh_manager::handleCpSeparatedState(){

	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::handleCpSeparatedState - enter");
	util.trace("TRACE: acs_alh_manager::handleCpSeparatedState - enter");

	//ACE_Guard<ACE_Recursive_Thread_Mutex> guard_alarmManager(_acs_alh_Mutex_);

	while ( synch_Imm_Access_ == true ){
		usleep ( IMM_RETRY_INTERVAL ); // wait 0.2 sec
		log.write(LOG_LEVEL_TRACE, "acs_alh_manager.handleCpSeparatedState() synch_Imm_Access = true - waiting...");
	}

	synch_Imm_Access_ = true;

	// Re-create manager for standby side.
	if (exeMgr_[STANDBY_SIDE])
	{
		delete exeMgr_[STANDBY_SIDE];
	}
	//Change the state of the exeMgr_[EX_SIDE]
	exeMgr_[EX_SIDE]->setCpState(true);  //true = separated

	//If the AP has been rebooted
	if(reboot_){
		if(exeMgr_[EX_SIDE]->countAlarmsInList()==-1){
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::handleCpSeparatedState - call countAlarmsInList() failed");
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::handleCpSeparatedState - exit - Returning false");
			util.trace("TRACE: acs_alh_manager::handleCpSeparatedState - exit - Returning false");
			synch_Imm_Access_ = false;
			return false;
		}
	}

	//Create the exeMgr_[STANDBY_SIDE]
	exeMgr_[STANDBY_SIDE] = new (std::nothrow) acs_alh_alarm_manager(exeMgr_[EX_SIDE]);
	if(!exeMgr_[STANDBY_SIDE]){
		log.write(LOG_LEVEL_ERROR, "Memory error allocating object: acs_alh_alarm_manager exeMgr_[%d] !",  STANDBY_SIDE);
		util.trace("TRACE: acs_alh_manager::handleCpSeparatedState - exit - Returning false");
		synch_Imm_Access_ = false;
		return false;
	}
	exeMgr_[STANDBY_SIDE]->setCpState(true); //true = separated


	if(!exeMgr_[STANDBY_SIDE]->unAckAllItem()){
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::handleCpSeparatedState - call unAckAllItem() failed");
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::handleCpSeparatedState - exit - Returning false");
		util.trace("TRACE: acs_alh_manager::handleCpSeparatedState - exit - Returning false");
		synch_Imm_Access_ = false;
		return false;
	}

	//added paola

	prev_CP_state = CP_state_;
	CP_state_ = CP_SEPARATED;

	if ( exeMgr_[EX_SIDE] )
		log.write(LOG_LEVEL_TRACE, "acs_alh_manager::handleCpSeparatedState - exeMgt_[EX_SIDE].get_exOrsb = %i",exeMgr_[EX_SIDE]->get_exOrsb());

	if ( exeMgr_[STANDBY_SIDE] )
		log.write(LOG_LEVEL_TRACE, "acs_alh_manager::handleCpSeparatedState - exeMgt_[STANDBY_SIDE].get_exOrsb = %i",exeMgr_[STANDBY_SIDE]->get_exOrsb());

	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::handleCpSeparatedState - exit ");
	util.trace("TRACE: acs_alh_manager::handleCpSeparatedState - exit");

	synch_Imm_Access_ = false;

	return true;
}


//========================================================================================
// handleCpNormal Method
//========================================================================================

bool acs_alh_manager::handleCpNormal(){

	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::handleCpNormal - enter");
	util.trace("TRACE: acs_alh_manager::handleCpNormal - enter");

	while ( synch_Imm_Access_ == true ){
		usleep ( IMM_RETRY_INTERVAL ); // wait 0.2 sec
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager.handleCpNormal() synch_Imm_Access = true - waiting...");
	}

	synch_Imm_Access_ = true;

	// Delete manager for standby side, if exists.
	if (exeMgr_[STANDBY_SIDE]) {
		delete exeMgr_[STANDBY_SIDE];
	}

	exeMgr_[STANDBY_SIDE] = 0;

	exeMgr_[EX_SIDE]->setCpState(false);  //true = separated /false = working

	if ( CP_state_ == CP_SEPARATED ){
		exeMgr_[EX_SIDE]->cleanAlarmIMM();
	}

	prev_CP_state = CP_state_;
	CP_state_ = CP_WORKING;

	log.write(LOG_LEVEL_ERROR, "acs_alh_manager::handleCpNormal - CP_state: %d ", CP_state_);
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::handleCpNormal - exit ");
	util.trace("TRACE: acs_alh_manager::handleCpNormal - exit");

	synch_Imm_Access_ = false;

	return true;
}

//========================================================================================
// clearAlarmTemp Method
//========================================================================================

bool acs_alh_manager::clearAlarmTemp(int i){

	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::clearAlarmTemp - enter");
	util.trace("TRACE: acs_alh_manager::clearAlarmTemp - enter");

	try {
		memset(alarmTemp[i].event.processName,'\0',sizeof(char)*32);
		alarmTemp[i].event.specificProblem=0;
		memset(alarmTemp[i].event.percSeverity,'\0',sizeof(char)*9);
		memset(alarmTemp[i].event.probableCause,'\0',sizeof(char)*128);
		memset(alarmTemp[i].event.objClassOfReference,'\0',sizeof(char)*8);
		memset(alarmTemp[i].event.objectOfReference,'\0',sizeof(char)*64);
		memset(alarmTemp[i].event.problemData,'\0',sizeof(char)*1024);
		memset(alarmTemp[i].event.problemText,'\0',sizeof(char)*1024);

		alarmTemp[i].identity[0]=0;
		alarmTemp[i].identity[1]=0;
		alarmTemp[i].identity[2]=0;
		alarmTemp[i].cpAlarmRef=0;
		alarmTemp[i].retransCnt=0;
		alarmTemp[i].ceasePending=false;
		alarmTemp[i].manualCease=false;
		alarmTemp[i].sendPriority=0;
		alarmTemp[i].acknowledge=false;
		alarmTemp[i].dummy=0;
		alarmTemp[i].time=0;
	} catch (int j) {
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::clearAlarmTemp - try - catch error");
		util.trace("TRACE: acs_alh_manager::clearAlarmTemp - try - catch error");
		return false;
	}


	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::clearAlarmTemp - exit ");
	util.trace("TRACE: acs_alh_manager::clearAlarmTemp - exit");
	return true;
}

//========================================================================================
// runALH
//========================================================================================

int acs_alh_manager::runALH(){

	//Call trace function
	util.trace("acs_alh_manager::runALH - enter ");
	log.write(LOG_LEVEL_INFO, "acs_alh_manager::runALH - enter");

	is_working_ = true;
	int sendActive = 0;
	int exit_code = 0;
	int ret_code = 0;

	prev_CP_state = CP_NOCONTACT;

	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH_STATUS[EX_SIDE]: %s", ALH_STATUS_STR[alhStatus_[EX_SIDE]]);
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH_STATUS[STANDBY_SIDE]: %s", ALH_STATUS_STR[alhStatus_[STANDBY_SIDE]]);

	ACE_Thread_Mutex stop_mutex;
	ACE_Condition<ACE_Thread_Mutex> cond_stop_Thread(stop_mutex);

	struct timeval timeInt;
	timeInt.tv_sec = 3;
	timeInt.tv_usec = 0;
	ACE_Time_Value tv(timeInt.tv_sec, timeInt.tv_usec);

	char tmp[32] = {0};
	snprintf(tmp, sizeof(tmp) - 1, "%s:%d", ACS_ALHEXEC_NAME, getpid());
	eventMgr_.setProcessName(tmp);			// Set static info

	acs_alh_cpnotifier cpNotifier(&cond_Thread, &cond_stop_Thread);

	// Read the AP Node Name
	if ( (alhManagerTermination_) || ( getAPNodeName() != 0)){
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH in shutdown phase...");
		is_working_ = false;
		return -1; // Terminate program
	}

	if ( (alhManagerTermination_) || ( create_managers() == false)){ // ExeMgr
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH in shutdown phase...");
		is_working_ = false;
		return -1; // Terminate program
	}

	// Read APNodeNumber from IMM
	if ( (alhManagerTermination_) || ( getAPNodeNumber() != 0)){
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH in shutdown phase...");
		is_working_ = false;
		return -1; // Terminate program
	}

	// Create JtpMgr
	if ( (alhManagerTermination_) || ( createJtp() == false)){
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH in shutdown phase...");
		is_working_ = false;
		return -1; // Terminate program
	}

	// It should be an empty line between Probable Cause and apNodeSite
	// and between apNodeSite and Problem text. Therefore the extra LineFeed \n
	// at the beginning and the end of apNodeSite.
	snprintf(apNodeSite_, sizeof(apNodeSite_) - 1, (apNodeNumber_ > 9)? "\n\n%-6s%-20s%-10s%s\n%-6u%-20s%-10s%s\n":"\n\n%-6s%-20s%-10s%s\n %-5u%-20s%-10s%s\n",
			"AP", "APNAME", "NODE", "NODENAME", apNodeNumber_, clusterName_, cluster_, nodename_);


	//========================================================================================
	// Main loop
	//========================================================================================

	CP_state_ = CP_NOCONTACT;

	if(cpNotifier.CP_Connect() == true)
	{
		switch (cpNotifier.current_CP_state_)
		{
			case acs_dsd::CP_STATE_NORMAL:
				{
					CP_state_ = CP_WORKING;
					log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_state: %d - %s", CP_state_, CP_STATE_STR[CP_state_]);
					break;
				}

			case acs_dsd::CP_STATE_SEPARATED:
				{
					CP_state_ = CP_SEPARATED;

					if(!handleCpSeparatedState()){
						log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - error - Transition to CP SEPARATED state FAILED!!!");
						util.trace("TRACE: axs_alh_manager::runALH - error - Transition to CP SEPARATED state FAILED!!!");
					}

					log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_state: %d - %s", CP_state_, CP_STATE_STR[CP_state_]);
					break;
				}

			case acs_dsd::CP_STATE_UNDEFINED:
				{
					CP_state_ = CP_UNDEFINED;
					log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_state: %d - %s", CP_state_, CP_STATE_STR[CP_state_]);
					break;
				}

			default:
				{
					CP_state_ = CP_UNDEFINED;
					log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_state: %d - %s", CP_state_, CP_STATE_STR[CP_state_]);
					break;
				}
		}//end switch
	}else{
		CP_state_ = CP_NOCONTACT;
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - call CP_Connect() returned false - CP_state %d - %s", CP_state_, CP_STATE_STR[3]);
	}

	if ( alhManagerTermination_ ) {
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH in shutdown phase...");
		is_working_ = false;
		return -1;
	}

	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - Try to start CP_Notifier_Thread...");
	ret_code = startCpNotifierThread(&cpNotifier);
	if( ret_code != 0 ){
		exit_code = -1;
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - Fail to start CP_Notifier_Thread...");
		return exit_code;
	}else{
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_Notifier_Thread started success!!!");
	}

	if ( alhManagerTermination_ ) {
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - ALH in shutdown phase...");
		is_working_ = false;
		return -1;
	}

	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - Try to start IMMNDMonitoringThread...");
		ret_code = startIMMNDMonitoringThread();
		if( ret_code != 0 ){
			exit_code = -1;
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - Fail to start IMMNDMonitoringThread ...");
			return exit_code;
		}else{
			log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - IMMNDMonitoringThread started success!!!");
		}


	//Start the eventRederThread
	log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - Try to start Event_Reader_Thread...");
	ret_code = startEventReadThread();
	if( ret_code != 0 ){
		exit_code = -1;
		log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - Fail to start Event_Reader_Thread...");
		return exit_code;
	}else{
		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - Event_Reader_Thread started success!!!");
	}

	int tryTerminate = 0;

	for(;;){
		if(alhManagerTermination_)
		{
			log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH -  ENTER IF alhManagerTermination_");
			if(allRecordToAlco_[EX_SIDE] == 0 && allRecordToAlco_[STANDBY_SIDE] == 0)
			{
				util.trace("TRACE: acs_alh_manager::runALH() - ALH_Manager_Thread - start termination process");
				log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH() - ALH_Manager_Thread - start termination process-1");

				if(!event_reader_thread_failed){
					stopEventReadThread();

					while(waitTermination_ == false){
						usleep(200000);
					}

					util.trace("acs_alh_manager::runALH - Event_Reader_Thread stopped!!!");
					log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH - Event_Reader_Thread stopped!!!-1");
				}

				util.trace("acs_alh_manager::runALH - EVENT_THREAD_TERMINATED!!!");
				log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH - EVENT_THREAD_TERMINATED!!!-1");

				if(cpNotifier.get_cpNotifier_thread_terminated() == false)
				{
					util.trace("acs_alh_manager::runALH - CP_NOTIFIER_THREAD start termination process...");
					log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH - CP_NOTIFIER_THREAD start termination process...-1");

					cpNotifier.stop_cpNotifier_activity();

					while ( cpNotifier.get() == false ){
						usleep(200000);
					}

					//printf("wait effettuata\n");
					util.trace("acs_alh_manager::runALH - CP_NOTIFIER_THREAD stopped!!!");
					log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH - CP_NOTIFIER_THREAD stopped!!!-1");
					//cond_stop_Thread.mutex().release();
				}

				if(!get_IMMND_thread_termination())
				{
					set_IMMND_thread_termination(true);
				   util.trace("acs_alh_manager::runALH - IMMND_MONITORING_THREAD stopped!!!");
				   log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH - IMMND_MONITORING_THREAD stopped!!!-1");
				}
				break;
			}
			else
			{
				if(tryTerminate < 1)
					tryTerminate++;
				else
				{
					util.trace("TRACE: acs_alh_manager::runALH() - ALH_Manager_Thread - start termination process");
					log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH() - ALH_Manager_Thread - start termination process-2");

					if(!event_reader_thread_failed){
						stopEventReadThread();

						while(waitTermination_ == false){
							usleep(200000);
						}

						//cond_stop_event_Thread.mutex().release();
						util.trace("acs_alh_manager::runALH - Event_Reader_Thread stopped!!!");
						log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH - Event_Reader_Thread stopped!!!-2");
					}

					util.trace("acs_alh_manager::runALH - EVENT_THREAD_TERMINATED!!!");
					log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH - EVENT_THREAD_TERMINATED!!!-2");

					if(cpNotifier.get_cpNotifier_thread_terminated() == false)
					{
						util.trace("acs_alh_manager::runALH - CP_NOTIFIER_THREAD start termination process...");
						log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH - CP_NOTIFIER_THREAD start termination process...-2");
						cpNotifier.stop_cpNotifier_activity();

						while ( cpNotifier.get() == false) {
							usleep(200000);
						}

						util.trace("acs_alh_manager::runALH - CP_NOTIFIER_THREAD stopped!!!");
						log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH - CP_NOTIFIER_THREAD stopped!!!-2");
					}
					if(!get_IMMND_thread_termination())
					{
						set_IMMND_thread_termination(true);
						util.trace("acs_alh_manager::runALH - IMMND_MONITORING_THREAD stopped!!!");
						log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH - IMMND_MONITORING_THREAD stopped!!!-2");
					}

					break;
				}
			}
		}

		int ext = 0;

		util.trace("TRACE: acs_alh_manager::runALH - Try to acquire mutex");
		log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH - Try to acquire mutex");
		cond_Thread.mutex().acquire();

		if ( event_reader_thread_failed ){
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - Event_Reader_Thread failed");
			util.trace("TRACE: acs_alh_manager::runALH - Event_Reader_Thread failed");
			sleep(3);
			ret_code = startEventReadThread();
			if( ret_code == 0 ){
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - Event_Reader_Thread started success!!!");
			}
		}

		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - Attends new events...");
		util.trace("TRACE: acs_alh_manager::runALH - Attends new events...");
		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
		timeout = timeout.now() + tv;
		ext = cond_Thread.wait(&timeout);

		if((ext == -1) && (errno == ETIME)){

			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH -  TIMEOUT expired ");
			util.trace("TRACE: axs_alh_manager::runALH - TIMEOUT expired");

			if( (CP_state_ == CP_UNDEFINED) || (CP_state_ == CP_NOCONTACT) || CP_state_ != cpNotifier.get_cpstate_value()){

				if(cpNotifier.CP_Connect() == true){

					switch(cpNotifier.get_cpstate_value()){
					case acs_dsd::CP_STATE_NORMAL:
						log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - CP_STATE = NORMAL");
						util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = NORMAL");

						if(!handleCpNormal()){
							log.write(LOG_LEVEL_ERROR, "Transition to CP NORMAL state FAILED!!!");
							util.trace("TRACE: acs_alh_manager::runALH - error - Transition to CP NORMAL state FAILED!!!");
						}
						break;
					case acs_dsd::CP_STATE_SEPARATED:
						log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - CP_STATE = SEPARATED");
						util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = SEPARATED");
						//TODO
						//Gestire errore
						if(!handleCpSeparatedState()){
							log.write(LOG_LEVEL_ERROR, "Transition to CP SEPARATED state FAILED!!!");
							util.trace("TRACE: axs_alh_manager::runALH - error - Transition to CP SEPARATED state FAILED!!!");
						}
						break;
					case acs_dsd::CP_STATE_UNDEFINED:
						log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - CP_STATE = UNDEFINED");
						util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = UNDEFINED");
						prev_CP_state = CP_state_;
						CP_state_ = CP_UNDEFINED;

						break;

					default:
						log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - CP_STATE = UNDEFINED ( default switch branch )");
						util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = UNDEFINED ( default switch branch )");
						prev_CP_state = CP_state_;
						CP_state_ = CP_UNDEFINED;
						sleep(1);

					}//end switch

				}else{
					//connect failed
					log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - CP_STATE = UNDEFINED ( cpNotifier.CP_Connect() return false )");
					util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = UNDEFINED ( cpNotifier.CP_Connect() return false )");
					prev_CP_state = CP_state_;
					CP_state_ = CP_UNDEFINED;

				}//end if CP_connect()

			}//end if CP_state
			else {
				prev_CP_state = CP_state_;
			}

			if(CP_state_ != CP_UNDEFINED){
				if (prev_CP_state == CP_UNDEFINED){
					checkALCO();
				}

				// Send alarms to the other side.
				sendActive = sendToCP();
				// 0 nothing to do
				// 1 OK

				if ( sendActive == 0 )
					sleep(1);
			}

			cond_Thread.mutex().release();

			util.trace("TRACE: acs_alh_manager::runALH - Mutex released");
			log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH - Mutex released");

			char logBuffer[512] = {0};
			snprintf(logBuffer,512,"acs_alh_manager::runALH - TIMEOUT EXPIRED - CP_state = %i / prev_CP_state = %i", CP_state_, prev_CP_state);
			log.write(LOG_LEVEL_ERROR, logBuffer);
			//Start of TR HV31154

			if(isOsafimmndRestarted)
			{
				isOsafimmndRestarted=false;
				std::string nodeNameLocal = util.getlocalNodeName();
				bool server_running = util.isAlhServerRunning( nodeNameLocal );
				log.write(LOG_LEVEL_TRACE, "value of server runnning is %d",server_running);
				if ( !server_running ){
					// ALH server is not running
					log.write(LOG_LEVEL_TRACE, "IMMND was Restarted so the IMM handle is reinitialized.");
					acs_alh_imm_runtime_owner temp_OI(connHandler_);
					sleep(1);
					temp_OI.finalize_IMM();
					temp_OI.init_IMM( false );
				}
				}
			 //End of TR HV31154
			 continue;
		}

		log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - A signal has been received on mutex cond_Thread");
		util.trace("TRACE: acs_alh_manager::runALH - A signal has been received on mutex cond_Thread");
		sleep(1);

		if(cpNotifier.get_cpNotifier_thread_terminated() == true && cpNotifier.get_cpNotifier_thread_exit_code() != 0 && !alhManagerTermination_){

			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - CP_Notifier_Thread failed");
			util.trace("TRACE: acs_alh_manager::runALH - CP_Notifier_Thread failed");
			sleep(3);
			ACE_Thread_Manager::instance()->join(get_cpNotifier_thread_handle());
			if(!alhManagerTermination_){
				startCpNotifierThread(&cpNotifier);
			}

		}else if (event_reader_thread_failed){
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - Event_Reader_Thread failed");
			util.trace("TRACE: acs_alh_manager::runALH - Event_Reader_Thread failed");
			sleep(3);
			ACE_Thread_Manager::instance()->join(get_application_thread_handle());
			ret_code = startEventReadThread();
			if( ret_code == 0 ){
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - error - Event_Reader_Thread started success!!!");
			}

		}

		else if(immnd_monitoring_thread_failed){
			immnd_monitoring_thread_failed=false;
			log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - immnd_monitoring_thread failed");
			util.trace("TRACE: acs_alh_manager::runALH - immnd_monitoring_thread failed");
			sleep(3);
			ACE_Thread_Manager::instance()->join(get_immnd_thread_handle());
			ret_code = startIMMNDMonitoringThread();
			if( ret_code == 0 ){
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - error - immnd_monitoring_thread started success!!!");
			}

		}

		else{
			//CP_THREAD_NOTIFIER STATE CHANGE

			log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - handling CP_EVENT");
			util.trace("TRACE: acs_alh_manager::runALH - handling CP_EVENT");

			switch(cpNotifier.get_cpstate_value()){

			case acs_dsd::CP_STATE_UNKNOWN:
				//cout<<"CP STATE UNKNOWN"<<endl;
				break;
			case acs_dsd::CP_STATE_NORMAL:
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_STATE = NORMAL");
				util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = NORMAL");

				if(!handleCpNormal()){
					log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - error - Transition to CP NORMAL state FAILED!!!");
					util.trace("TRACE: acs_alh_manager::runALH - error - Transition to CP NORMAL state FAILED!!!");
				}
				break;

			case acs_dsd::CP_STATE_SEPARATED:
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_STATE = SEPARATED");
				util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = SEPARATED");

				if(!handleCpSeparatedState()){
					log.write(LOG_LEVEL_ERROR, "acs_alh_manager::runALH - error - Transition to CP SEPARATED state FAILED!!!");
					util.trace("TRACE: acs_alh_manager::runALH - error - Transition to CP SEPARATED state FAILED!!!");
				}

				break;

			case acs_dsd::CP_STATE_UNDEFINED:
				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_STATE = UNDEFINED");
				util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = UNDEFINED");
				prev_CP_state = CP_state_;
				CP_state_ = CP_UNDEFINED;

				break;
			default:

				log.write(LOG_LEVEL_DEBUG, "acs_alh_manager::runALH - CP_STATE = UNDEFINED");
				util.trace("TRACE: acs_alh_manager::runALH - CP_STATE = UNDEFINED");
				prev_CP_state = CP_state_;
				CP_state_ = CP_UNDEFINED;
			}

		}

		cond_Thread.mutex().release();

		char logBuffer[512] = {0};
		snprintf(logBuffer,512,"CP_state = %i / prev_CP_state = %i", CP_state_, prev_CP_state);
		log.write(LOG_LEVEL_ERROR, logBuffer);

		//sleep(1);

	}

	// wait for Event Reader Thread termination
	ACE_Thread_Manager::instance()->join(get_application_thread_handle());
	log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH() - getEvent Threads stopped");
	ACE_Thread_Manager::instance()->join(get_cpNotifier_thread_handle());
	log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH() - CP Notifier Threads stopped");
	ACE_Thread_Manager::instance()->join(get_immnd_thread_handle());
	log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH() - IMMND Monitoring Thread stopped");
	pthread_detach(get_application_thread_handle());
	pthread_detach(get_cpNotifier_thread_handle());
	pthread_detach(get_immnd_thread_handle());
	term_cond_->signal();

	log.write(LOG_LEVEL_WARN, "acs_alh_manager::runALH() - CP Notifier and getEvent and IMMND Monitoring Threads stopped");

	is_working_ = false;

	util.trace("TRACE: acs_alh_manager::runALH() - exit - Returning: 0");
	log.write(LOG_LEVEL_TRACE, "acs_alh_manager::runALH() - exit - Returning: 0");

	return 0;
}


