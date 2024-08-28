//******************************************************************************
//
// NAME
//      ACS_PRC_Prcman.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// DOCUMENT NO
//	19089-CAA 109 0520

// AUTHOR 
//	2010-12-29 by XLUCPET

// REVISION
//  -	
// 

// CHANGES

//
//	REV NO		DATE		NAME 		DESCRIPTION
//			2014-04-23	XSARSES	 	Fix for HS53134

// SEE ALSO 
// 	
//
//******************************************************************************

#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <syslog.h>
#include <sys/stat.h>
#include <grp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>

#include "acs_prc_api.h"
#include "ACS_PRC_suInfo.h"
#include "ACS_PRC_RunTimeOwner.h"
#include "ACS_PRC_RunTimeOwnerThread.h"
#include "ACS_PRC_RunTimeOwnerInitializationThread.h"
#include "ACS_PRC_NotificationThread.h"
#include "acs_prc_brfImplementer.h"
#include "acs_prc_brfImplementerThread.h"
#include "ACS_PRC_ispapi.h"
#include "ACS_TRA_Logging.h"
#include "ACS_PRC_prcmanThread.h"
#include "ACS_PRC_SignalHandler.h"
#include "acs_prc_eventmatrix_pipe_handler.h"
#include "acs_apgcc_omhandler.h"
#include <ACS_APGCC_Util.H>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/Thread.h>
#include "acs_apgcc_oihandler_V3.h"
#include "acs_prc_reporting.h"
#include "acs_prc_observerImplementer.h"
#include "acs_prc_api_impl.h"
#include "acs_prc_runlevel.h"
#include "ACS_APGCC_CommonLib.h"
#define LOCK_FILE "/var/run/ap/prcmand.lck"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif
using namespace std;
bool  is_swm_2_0 = true;
const char pipeName[] = "/var/run/ap/PrcManServer";
const char reboot_file[] = "/var/log/prc_reboot";
const char reboot_file_eva[] = "/var/log/prc_reboot_eva";
const char reboot_file_fc[] = "/var/log/prc_reboot_fc";
const char lotc_restore_path_clear[] = "/usr/share/pso/storage-paths/clear";
const char apg_backup_not_done_file[] = "/var/log/apg_backup_not_done";
const char apg_backup_not_done_file_cluster[] = "/cluster/etc/ap/acs/prc/conf/apg_backup_not_done";
const char prc_status1[] = "/cluster/etc/ap/acs/prc/conf/remote_prc_status_1";//HV92897
const char prc_status2[] = "/cluster/etc/ap/acs/prc/conf/remote_prc_status_2";//HV92897
const char prc_restore_path[] = "/acs-prc/";

const char dnObjName_node1[] = "brfPersistentDataOwnerId=ACS-PRC-1,brfParticipantContainerId=1";
const char dnObjName_node2[] = "brfPersistentDataOwnerId=ACS-PRC-2,brfParticipantContainerId=1";

const char dnManagedElement[] = "managedElementId=1";

const char MODEL_CHANGE_COMMITTED[] = "MODEL_CHANGE_COMMITTED";
const char LOG_FROM_PRC_API_COM_COMPONENT[] = "#LOG_FROM_PRC_API_COM_COMPONENT#";

//Moved creation of /cluster/etc/ap/acs/prc/conf to service level
const char PRC_CONFIG_DIR_PRC_NAME[] = "/cluster/etc/ap/acs/prc/";
const char PRC_CONFIG_DIR_CONF_NAME[] = "/cluster/etc/ap/acs/prc/conf/";


static int siglist[] = {SIGINT, SIGTERM, SIGPIPE, SIGABRT, SIGTSTP};
char strErr_no[1024] = {0};

int removePartecipant ( string local_node_id, string local_node_host_name ) {

	ACS_CC_ReturnType returnCode;

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	char* rdnValue;

	OmHandler immHandler;

	int retry = 10;
	do {
		returnCode=immHandler.Init();

		if (returnCode != ACS_CC_SUCCESS ){
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant:immHandler.Init() - Init failed", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			usleep(500000); // wait 0,5 seconds before try again
			retry--;
		} else {
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant:immHandler.Init() - Init failed", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		}
	}while(( returnCode != ACS_CC_SUCCESS) && ( retry >= 0 ));

	if (returnCode != ACS_CC_SUCCESS){
		Logging.Close();
		return returnCode;
	}

	if ( local_node_id == "1" )
		rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-1,brfParticipantContainerId=1");
	else if (local_node_id == "2")
		rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-2,brfParticipantContainerId=1");


	else{
		if(strcmp(local_node_host_name.c_str(), "SC-2-1") == 0)
		{
			rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-1,brfParticipantContainerId=1");
			snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant - RDN ACS-PRC-1 fetched", PRCBIN_REVISION );
			Logging.Write(strErr_no, LOG_LEVEL_TRACE);
		}

		else if(strcmp(local_node_host_name.c_str(), "SC-2-2") == 0)
		{
			rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-2,brfParticipantContainerId=1");
			snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant - RDN ACS-PRC-2 fetched", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_TRACE);
		}

		else
		{
			snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant: Wrong node information - Failure in delete BRF object", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			Logging.Close();

			return -1;
		}

	}

	retry = 10;
	do {
		returnCode=immHandler.deleteObject( rdnValue );

		if (returnCode != ACS_CC_SUCCESS ){
			if (returnCode == -12){ // object doesn't exist
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant:deleteObject() - BRF object [ %s ] doesn't exist - End", PRCBIN_REVISION, rdnValue);
				immHandler.Finalize();
				Logging.Close();
				return 0;
			} else {
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant:deleteObject() - BRF object [ %s ] remove fails", PRCBIN_REVISION, rdnValue);
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			}
			usleep(500000); // wait 0,5 seconds before try again
			retry--;
		} else {
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - removePartecipant:deleteObject() - BRF object [ %s ] remove success", PRCBIN_REVISION, rdnValue);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		}
	}while(( returnCode != ACS_CC_SUCCESS) && ( retry >= 0 ));

	if( returnCode != ACS_CC_SUCCESS){
		immHandler.Finalize();
		Logging.Close();
		return -1;
	}

	returnCode=immHandler.Finalize();
	if( returnCode != ACS_CC_SUCCESS){
		Logging.Close();
		return -1;
	}

	Logging.Close();
	return 0;
}

int checkAndRemovePeerPartecipant ( string remote_node_id, string remote_node_hostname, string local_hostname ) {
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - Entry in checkAndRemovePeerPartecipant(), remote node is:  %s", PRCBIN_REVISION, remote_node_hostname.c_str() );
	Logging.Write(strErr_no, LOG_LEVEL_ERROR);


	ACE_SOCK_Connector serverConnector;
	ACE_SOCK_Stream serverStream;
	ACE_INET_Addr server_addr(8881,remote_node_hostname.c_str());
	ACE_Time_Value val(3);

	if (serverConnector.connect (serverStream, server_addr, &val  ) == -1){
		// other node is down, then remove the backup object on the other node
		//removePartecipant( remote_node_id );

		//changes HV92897 start
		if(strcmp(remote_node_hostname.c_str(), "SC-2-1") == 0)
		{

			if (access(prc_status1, F_OK) == 0){
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - Remote PRC  process instance is running Exit from checkAndRemovePeerPartecipant()- Return Code 1", PRCBIN_REVISION);
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				Logging.Close();
				return 0;

			}
		}

		else if(strcmp(remote_node_hostname.c_str(), "SC-2-2") == 0)
		{
			if (access(prc_status2, F_OK) == 0){
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - Remote PRC  process instance is running Exit from checkAndRemovePeerPartecipant()- Return Code 1", PRCBIN_REVISION);
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				Logging.Close();
				return 0;

			}

		}

		else
		{

			if(strcmp(local_hostname.c_str(), "SC-2-1") == 0)
			{

				if (access(prc_status2, F_OK) == 0){
					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - Remote PRC  process instance is running Exit from checkAndRemovePeerPartecipant()- Return Code 1", PRCBIN_REVISION);
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);
					Logging.Close();
					return 0;

				}
			}

			else if(strcmp(local_hostname.c_str(), "SC-2-2") == 0)
			{
				if (access(prc_status1, F_OK) == 0){
					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - Remote PRC  process instance is running Exit from checkAndRemovePeerPartecipant()- Return Code 1", PRCBIN_REVISION);
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);
					Logging.Close();
					return 0;

				}

			}

			else{

				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - Wrong node id - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				Logging.Close();

				return 1;
			}
		}

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - Remote PRC process is not running, so remove the BRF partecipant-", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		//changes HV92897 END

		removePartecipant( remote_node_id, local_hostname );
	}


	return 0;
}

bool sortVectorByRules_2 ( string objA, string objB ) {

	return strcmp ( objA.c_str(), objB.c_str() ) < 0;
}

time_t getLastBackup_Creation_Time_2 (){

	OmHandler immHandle;
	vector<ACS_APGCC_ImmAttribute *> attributes;
	ACS_CC_ReturnType result;
	vector<string> tmp_vector;
	vector<string> Backup_Creation_Time_Vector;

	result = immHandle.Init();

	if ( result != ACS_CC_SUCCESS ){
		return 0;
	}

	result = immHandle.getClassInstances("BrmBackup",tmp_vector);

	if ( result != ACS_CC_SUCCESS ){
		immHandle.Finalize();
		return 0;
	}

	ACS_CC_ImmParameter Backup_Creation_Time;
	Backup_Creation_Time.attrName = const_cast<char*>("creationTime");

	for ( unsigned int i = 0; i < tmp_vector.size(); i++){
		result = immHandle.getAttribute(tmp_vector[i].c_str(), &Backup_Creation_Time);

		if ( result != ACS_CC_SUCCESS ){
			immHandle.Finalize();
			return 0;
		}

		Backup_Creation_Time_Vector.push_back( reinterpret_cast<char*>(Backup_Creation_Time.attrValues[0]));
	}

	immHandle.Finalize();

	sort(Backup_Creation_Time_Vector.begin(), Backup_Creation_Time_Vector.end(), sortVectorByRules_2);

	struct tm tm;

	memset(&tm, 0, sizeof(struct tm));

	strptime(Backup_Creation_Time_Vector[Backup_Creation_Time_Vector.size()-1].c_str(), "%Y-%m-%dT%H:%M:%S", &tm);

	tm.tm_isdst = -1;
	return mktime(&tm);
}


bool isNodeRestored ( void ){

	char hostname[16] = {0};
	gethostname( hostname, sizeof(hostname));

	char prc_restore_path_full[256] = {0};

	FILE* lotc_restore_clear_path_fd = fopen(lotc_restore_path_clear, "r");

	if ( lotc_restore_clear_path_fd != NULL )
		fgets ( prc_restore_path_full, sizeof(prc_restore_path_full), lotc_restore_clear_path_fd );

	fclose(lotc_restore_clear_path_fd);

	prc_restore_path_full[strlen(prc_restore_path_full)-1] = 0;

	strncat( prc_restore_path_full, prc_restore_path, sizeof(prc_restore_path) );

	strncat( prc_restore_path_full, hostname, sizeof(hostname) );

	if ( access ( prc_restore_path_full, F_OK ) == 0  ){ // If the file still exist the reboot is not due to a restore
		return false;
	}
	else { // The file is no more present, this means that the node has been restored
		return true;
	}
}

int create_restore_file ( void ) {

	char hostname[16] = {0};
	gethostname( hostname, sizeof(hostname));

	char prc_restore_path_full[256] = {0};

	FILE* lotc_restore_clear_path_fd = fopen(lotc_restore_path_clear, "r");

	if ( lotc_restore_clear_path_fd != NULL )
		fgets ( prc_restore_path_full, sizeof(prc_restore_path_full), lotc_restore_clear_path_fd );
	else
		return -1;

	fclose(lotc_restore_clear_path_fd);

	prc_restore_path_full[strlen(prc_restore_path_full)-1] = 0;

	strncat( prc_restore_path_full, prc_restore_path, sizeof(prc_restore_path) );

	if ( access ( prc_restore_path_full, F_OK ) != 0  ){ // If the PRC path doesn't exist, then create the directory
		if ( mkdir( prc_restore_path_full, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH ) != 0 ){// If mkdir fails then return -2
			return -2;
		}
	}

	strncat( prc_restore_path_full, hostname, sizeof(hostname) );

	FILE* prc_restore_file = fopen ( prc_restore_path_full, "w" );

	if ( prc_restore_file == NULL )
		return -3;

	fclose( prc_restore_file );

	return 0;
}

void create_reboot_file( void ){

	FILE* reboot_file_fd = fopen(reboot_file, "w");

	fclose(reboot_file_fd);
}

int remove_reboot_file( void ){

	return remove ( reboot_file );
}

time_t get_time_from_reboot_file (){

	struct stat buffer;

	stat ( reboot_file, &buffer);

	return buffer.st_mtime;

}

void Send_AP_Reboot (){

	acs_prc_report report;
	ACS_PRC_API prc_api_object;

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	string clusterPath = "/cluster/etc/ap/acs/prc/conf/";
	string p_local_node_hostname;

	ifstream ifs;

	ifs.open("/etc/cluster/nodes/this/hostname");
	if (ifs.good())
		getline(ifs, p_local_node_hostname);
	ifs.close();

	clusterPath.append(p_local_node_hostname);

	sleep (5); //wait 5 sec to wait ALH

	if (  access ( clusterPath.c_str(), F_OK ) == 0 ){ // The node has been rebooted due to PRCBOOT command
		report.alarmAPReboot("",ACS_PRC_ceaseSeverity);
		report.alarmAPReboot(ACS_PRC_user);
		remove(clusterPath.c_str());
		remove_reboot_file();

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted due to PRCBOOT command - Raise O1 AP REBOOT alarms, this one can be filtered by ALH", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);

	}else { // The node has been rebooted due to CMW, to PRCEVA, to Software Update, to restore or acs_prcmand has been restarted.

		bool NodeRestored = isNodeRestored();//HX48354
		ACS_APGCC_CommonLib getHWInfo_obj;
		ACS_APGCC_HWINFO hwInfo;
		ACS_APGCC_HWINFO_RESULT hwInfoResult;
		getHWInfo_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - prc_api_object.lastRebootTime() VALUE = %ld -1", PRCBIN_REVISION,prc_api_object.lastRebootTime());
		Logging.Write(strErr_no, LOG_LEVEL_WARN);

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - get_time_from_reboot_file() VALUE = %ld -1", PRCBIN_REVISION,get_time_from_reboot_file());
		Logging.Write(strErr_no, LOG_LEVEL_WARN);


		if ( prc_api_object.lastRebootTime() < get_time_from_reboot_file() ){
			// If the file has been created AFTER the node startup this means that PRC has been
			// manually stopped without node reboot
			// As PRC restarts during restore checking whether node is restored or not here also
			if(NodeRestored){//HX48354
				if (hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
					if ( hwInfo.hwVersion == ACS_APGCC_HWVER_VM ){
						ACS_PRC_NotificationThread::setRestoreFlagInRunLevelThread(true);
						memset(strErr_no, 0, sizeof(strErr_no));
						snprintf(strErr_no,1024,"acs_prcmand %s - restore flag is set in SEND_AP_REBOOT", PRCBIN_REVISION );
						Logging.Write(strErr_no, LOG_LEVEL_ERROR);
					}
				}
			}

			//start of HY26482
			if ( access ( reboot_file_fc, F_OK ) == 0 ){ // Reboot ordered by Software Update

				report.alarmAPReboot(ACS_PRC_function);
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted due to Software Update - Raise O1 AP REBOOT alarms, this one can be filtered by ALH", PRCBIN_REVISION );
				Logging.Write(strErr_no, LOG_LEVEL_WARN);
			}
			//end of HY26482
			remove_reboot_file();
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - prc_api_object.lastRebootTime() < get_time_from_reboot_file() - server has been manually stopped", PRCBIN_REVISION );
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		}
		else {

			// If the file has been created BEFORE the node startup this means that PRC has been
			// stopped due to a node reboot. Now we have to check if the node has been restarted by PRCEVA
			// or due to Software update or to a AP restore
			bool is_VM = false;
			if(NodeRestored){//HX48354
				if (hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
					if ( hwInfo.hwVersion == ACS_APGCC_HWVER_VM ){
						ACS_PRC_NotificationThread::setRestoreFlagInRunLevelThread(true);
						is_VM=true;
						memset(strErr_no, 0, sizeof(strErr_no));
						snprintf(strErr_no,1024,"acs_prcmand %s - restore flag is set in SEND_AP_REBOOT ", PRCBIN_REVISION );
						Logging.Write(strErr_no, LOG_LEVEL_ERROR);
					}
				}
			}
			report.alarmAPReboot("",ACS_PRC_ceaseSeverity);
			if ( NodeRestored ){ // Reboot ordered by restore, do not raise any alarm, just log
                          	if ( is_VM ) {//start of IA36751
                                  	memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted after a Restore, no AP REBOOT raised", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				}//end of IA36751
				else {
					int result = create_restore_file(); // The node has been restored so PRC have to create a file under restore "clear" path in order to understand if the reboot is due to a restore or not
					memset(strErr_no, 0, sizeof(strErr_no));
					switch ( result ){
                                	case -1 :
                                        	snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted after a Restore, failed to open /usr/share/pso/storage-paths/clear file", PRCBIN_REVISION );
                                        	break;
                                	case -2 :
                                        	snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted after a Restore, failed to create PRC folder", PRCBIN_REVISION );
                                        	break;
                                	case -3 :
                                        	snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted after a Restore, failed to create PRC file", PRCBIN_REVISION );
                                        	break;
                                	default :
                                        	snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted after a Restore, no AP REBOOT raised", PRCBIN_REVISION );
                                	}
                                	Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				}
				remove_reboot_file(); // IA81937 TR fix
			}
			else if ( access( reboot_file_eva, F_OK) == 0 ){ // Reboot ordered by PRCEVA
				remove( reboot_file_eva );
				report.alarmAPReboot(ACS_PRC_evstop,ACS_PRC_a2Severity);
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted due to PRCEVA - Raise A2 AP REBOOT alarms", PRCBIN_REVISION );
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			}
			else if ( access ( reboot_file_fc, F_OK ) == 0 ){ // Reboot ordered by Software Update
				report.alarmAPReboot(ACS_PRC_function);
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted due to Software Update - Raise O1 AP REBOOT alarms, this one can be filtered by ALH", PRCBIN_REVISION );
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			}
                        else if ( access ( reboot_file, F_OK) == -1 ){ // start of IA40986
                                memset(strErr_no, 0, sizeof(strErr_no));
                                snprintf(strErr_no,1024,"acs_prcmand %s - The reboot file is not created as PRC service has been killed abruptly, no AP REBOOT raised", PRCBIN_REVISION );
                                Logging.Write(strErr_no, LOG_LEVEL_ERROR);
                        }// end of IA40986
			else {
				report.alarmAPReboot("Unknown",ACS_PRC_a2Severity);
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - The node has been rebooted due to other reboot commands - Raise A2 AP REBOOT alarms", PRCBIN_REVISION );
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				remove_reboot_file(); // IA81937 TR fix
			}
		}
	}

	Logging.Close();
}

int main(int /*argc*/, char* /*argv*/[]){

	syslog(LOG_INFO, "Starting PRCMAND ");
	acs_prc_report report;
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	is_swm_2_0=isSWM20();

	int fdl, retry = 0, IMM_Try = 1;
	bool result = false;
	//Added retry mechanism to create a lock file
	do {
		if ( (fdl = open(LOCK_FILE, O_RDWR | O_CREAT | O_CLOEXEC,0664)) < 0) {
			snprintf(strErr_no,1024,"acs_prcmand %s - Unable to open lock file - retry - count = %i, errno = %i", PRCBIN_REVISION, retry, ACE_OS::last_error());
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			++retry;
			usleep(600000);
		} else {
			result = true;
		}
	}while((!result) && ( retry < 10 ));
	if(!result)
	{
		snprintf(strErr_no,1024,"acs_prcmand %s - Unable to open lock file - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		create_reboot_file();
		return 1;
	}



	memset(strErr_no, 0, sizeof(strErr_no));

	if (flock(fdl, LOCK_EX | LOCK_NB)) {
		if (errno == EWOULDBLOCK) {
			snprintf(strErr_no,1024,"acs_prcmand %s - Lock active. Another process instance is running - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			Logging.Close();
			create_reboot_file();

			return 1;
		}
		snprintf(strErr_no,1024,"acs_prcmand %s - Lock failed - Return Code 3 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		create_reboot_file();

		return 3;
	}

        //creating /cluster/etc/ap/acs/prc/conf directory
        int status = ACE_OS::mkdir(PRC_CONFIG_DIR_PRC_NAME, S_IRWXU | S_IRWXG );
        if ( status != 0 )
        {
                ACE_UINT32  dwError = ACS_APGCC::getLastError();
                if (EEXIST != dwError)
                {
                        memset(strErr_no, 0, sizeof(strErr_no));
                        snprintf(strErr_no,1024,"acs_prcmand %s - Unable to create the first level directory for storing config parameters - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
                        Logging.Write(strErr_no, LOG_LEVEL_ERROR);
                        Logging.Close();
                        create_reboot_file();
                        return 1;

                }
        }
        status = ACE_OS::mkdir(PRC_CONFIG_DIR_CONF_NAME, S_IRWXU | S_IRWXG );
        if ( status != 0 )
        {
                ACE_UINT32 dwError = ACS_APGCC::getLastError();
                if (EEXIST != dwError)
                {
                        memset(strErr_no, 0, sizeof(strErr_no));
                        snprintf(strErr_no,1024,"acs_prcmand %s - Unable to create the second level directory for storing config parameters - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
                        Logging.Write(strErr_no, LOG_LEVEL_ERROR);
                        Logging.Close();
                        create_reboot_file();
                        return 1;
                }
        }

	char hostname[16] = {0};
	int res = gethostname( hostname, sizeof(hostname));

	if ( res == -1 ){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - gethostname failed - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		create_reboot_file();

		return 1;

	}

	if(strcmp(hostname, "SC-2-1") == 0)
	{

		FILE * file_prc_stat = fopen(prc_status1, "w");
		fclose (file_prc_stat);

	}

	else if(strcmp(hostname, "SC-2-2") == 0)
	{
		FILE * file_prc_stat = fopen(prc_status2, "w");
		fclose (file_prc_stat);

	}

	else
	{
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - Wrong node id - Return Code 1", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		create_reboot_file();

		return 1;
	}


	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - Start", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_WARN);



	ACE_UNIX_Addr addr(pipeName);

	ACE_LSOCK_Acceptor Acceptor;
	ACE_LSOCK_Stream stream;
	bool running = true;
	char buffer[1024] = {0};

	ACS_PRC_prcmanThread PrcmanSocketTread;
	ACS_PRC_NotificationThread PrcmanNotificationThread;
	acs_prc_eventmatrix_pipe_handler eventmatrix_pipe_hadler;

	acs_prc_observerImplementer ManagedElementImplementer;

	acs_apgcc_oihandler_V3 oiHandler;

	string local_hostname_path = "/etc/cluster/nodes/this/hostname";
	string local_id_path = "/etc/cluster/nodes/this/id";
	string remote_hostname_path = "/etc/cluster/nodes/peer/hostname";
	string remote_id_path = "/etc/cluster/nodes/peer/id";

	string p_local_node_id, p_local_node_hostname;
	string p_remote_node_id, p_remote_node_hostname;
	ifstream ifs;

	ifs.open(local_id_path.c_str());
	if (ifs.good())
		getline(ifs, p_local_node_id);
	ifs.close();

	ifs.open(local_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, p_local_node_hostname);
	ifs.close();

	ifs.open(remote_id_path.c_str());
	if (ifs.good())
		getline(ifs, p_remote_node_id);
	ifs.close();

	ifs.open(remote_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, p_remote_node_hostname);
	ifs.close();

	PrcmanSocketTread.start();
	PrcmanSocketTread.activate();

	PrcmanNotificationThread.start();
	PrcmanNotificationThread.activate();

	acs_prc_brfImplementerThread BRFThread ( p_local_node_id, hostname);

	if( checkAndRemovePeerPartecipant( p_remote_node_id, p_remote_node_hostname, hostname ))
	{
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - checkAndRemovePeerPartecipant Failure - Return Code 3", PRCBIN_REVISION);
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		create_reboot_file();
		if(strcmp(hostname, "SC-2-1") == 0)
			remove(prc_status1);

		else if(strcmp(hostname, "SC-2-2") == 0)
			remove(prc_status2);

		return 3;

	}

	BRFThread.start();
	BRFThread.activate();

	eventmatrix_pipe_hadler.start();
	eventmatrix_pipe_hadler.activate();

	ManagedElementImplementer.setScope ( ACS_APGCC_ONE );
	ManagedElementImplementer.setObjName ( dnManagedElement );

	if ( p_local_node_id == "1" )
		ManagedElementImplementer.setImpName ("@PRCMAN_OBSERVER_1");
	else if (p_local_node_id == "2")
		ManagedElementImplementer.setImpName ("@PRCMAN_OBSERVER_2");
	else{

		if(strcmp(hostname, "SC-2-1") == 0)
		{
			ManagedElementImplementer.setImpName ("@PRCMAN_OBSERVER_1");
			snprintf(strErr_no,1024,"acs_prcmand %s - @PRCMAN_OBSERVER_1  - Implementer set", PRCBIN_REVISION );
			Logging.Write(strErr_no, LOG_LEVEL_TRACE);
		}

		else if(strcmp(hostname, "SC-2-2") == 0)
		{
			ManagedElementImplementer.setImpName ("@PRCMAN_OBSERVER_2");
			snprintf(strErr_no,1024,"acs_prcmand %s - @PRCMAN_OBSERVER_2  - Implementer set", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_TRACE);
		}

		else
		{
			snprintf(strErr_no,1024,"acs_prcmand %s -  Wrong node information - Return Code 1", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			Logging.Close();
			create_reboot_file();
			if(strcmp(hostname, "SC-2-1") == 0)
				remove(prc_status1);

			else if(strcmp(hostname, "SC-2-2") == 0)
				remove(prc_status2);

			return 1;
		}

	}

             //start of TR IA18259
               while ( ( oiHandler.addObjectImpl(&ManagedElementImplementer) == ACS_CC_FAILURE ) && ( IMM_Try <= 30 ) ) {

                        snprintf(strErr_no,1024,"acs_prcmand %s - Failed to set ManagedElement implementer, try n. %i", PRCBIN_REVISION, IMM_Try++ );
                        Logging.Write(strErr_no, LOG_LEVEL_ERROR);

                        usleep(2500000); // wait 2.5 sec
                }

                if ( IMM_Try <= 30 ){
                     IMM_Try = 1;
                     ManagedElementImplementer.start();
                     ManagedElementImplementer.activate();
                }
	        else{
                  snprintf(strErr_no,1024,"acs_prcmand %s - Failed to set ManagedElement implementer even after retries..", PRCBIN_REVISION );
                  Logging.Write(strErr_no, LOG_LEVEL_ERROR);
               }  // end of TR IA18259

	ACE_Sig_Handler ace_sigdispatcher;

	ACS_PRC_SignalHandler prcmanSignalHandler ( running );

	int numsigs = sizeof ( siglist ) / sizeof(int);
	for ( int i=0 ; i < numsigs ; ++i )
		ace_sigdispatcher.register_handler(siglist[i], & prcmanSignalHandler);

	::unlink(pipeName);

	if ( Acceptor.open(addr) != 0 ){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - ACE_LSOCK_Acceptor - Acceptor.open fails - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();

		stream.close();
		Acceptor.close();
		Acceptor.remove();
		create_reboot_file();
		if(strcmp(hostname, "SC-2-1") == 0)
			remove(prc_status1);

		else if(strcmp(hostname, "SC-2-2") == 0)
			remove(prc_status2);

		exit(1);
	}

	::chmod(pipeName,0777);

	ACE_Thread_Manager::instance()->spawn( (ACE_THR_FUNC)Send_AP_Reboot, 0, THR_DETACHED|THR_NEW_LWP );

	while( prcmanSignalHandler.getValue() ){

		if ( Acceptor.accept(stream,0,0,0) == -1 ){
			if ( ACE_OS::last_error() != EINTR ){

				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - ACE_LSOCK_Acceptor - Acceptor.accept fails - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				Logging.Close();
				stream.close();
				Acceptor.close();
				Acceptor.remove();
				create_reboot_file();
				if(strcmp(hostname, "SC-2-1") == 0)
					remove(prc_status1);

				else if(strcmp(hostname, "SC-2-2") == 0)
					remove(prc_status2);


				exit(1);
			}
		} else {

			memset ( buffer, 0, sizeof(buffer) );
			stream.recv(buffer,sizeof(buffer));

			if (strcmp(buffer,"SHUTDOWN") == 0) {

				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - Shutdown", PRCBIN_REVISION);
				Logging.Write(strErr_no, LOG_LEVEL_WARN);

				PrcmanSocketTread.stop();
				PrcmanSocketTread.wait();
				PrcmanNotificationThread.stop();
				PrcmanNotificationThread.wait();
				BRFThread.stop();
				BRFThread.wait();
				eventmatrix_pipe_hadler.stop();
				eventmatrix_pipe_hadler.wait();
				ManagedElementImplementer.stop();
				ManagedElementImplementer.wait();

				if ( report.alarmAPNotRedundant ( ACS_PRC_ceaseSeverity, ACS_PRC_up, p_local_node_hostname.c_str() ) != ACS_PRC_Report_OK ){
					memset(strErr_no, 0, sizeof(strErr_no));
					Logging.Write("alarmAPNotRedundant Error - ACS_PRC_ceaseSeverity",LOG_LEVEL_ERROR);
					snprintf(strErr_no,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
					Logging.Write(strErr_no,LOG_LEVEL_ERROR);
				}

				stream.close();
				Acceptor.close();
				Acceptor.remove();
				Logging.Close();
				create_reboot_file();
				if(strcmp(hostname, "SC-2-1") == 0)
					remove(prc_status1);

				else if(strcmp(hostname, "SC-2-2") == 0)
					remove(prc_status2);

				exit(0);
			}
			else if (strcmp(buffer,"GETNODESTATE") == 0 ){
				if ( ACS_PRC_NotificationThread::getNodeState() == 1 )
					stream.send_n ( "ACTIVE", 7 );
				else if ( ACS_PRC_NotificationThread::getNodeState() == 2 )
					stream.send_n ( "STAND-BY", 9 );
			}
			else if (strcmp(buffer,"GET_BACKUP_RESTORE") == 0){
				if ( acs_prc_brfImplementer::getBackupAndRestore() )
					stream.send_n ( "TRUE", 5 );
				else
					stream.send_n ( "FALSE", 6 );
			}
			else if (strcmp(buffer,"GET_SOFTWARE_UPDATE") == 0){
				if ( ACS_PRC_NotificationThread::getSoftwareUpdate() )
					stream.send_n ( "TRUE", 5 );
				else
					stream.send_n ( "FALSE", 6 );
			}
			else if (strcmp(buffer,MODEL_CHANGE_COMMITTED) == 0) {
				FILE * 	file_write = fopen(apg_backup_not_done_file, "w");
				fclose (file_write);

				file_write = fopen(apg_backup_not_done_file_cluster, "w");
				fclose (file_write);

				PrcmanNotificationThread.raise_APG_Backup_Missing();
			}
			else if (strstr(buffer,LOG_FROM_PRC_API_COM_COMPONENT)){
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - %s", PRCBIN_REVISION, buffer+strlen(LOG_FROM_PRC_API_COM_COMPONENT));
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			}
			else{
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - ERROR RECEIVED UNMATCHED OPTION (%s)", PRCBIN_REVISION, buffer);
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			}

			stream.close();
		}
	}

	if ( ACE_OS::last_error() == EINTR ){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - Signal %s received ==> Shutdown", PRCBIN_REVISION, prcmanSignalHandler.getStrSignal().c_str() );
		Logging.Write(strErr_no, LOG_LEVEL_WARN);
	}

	for ( int i=0 ; i < numsigs ; ++i )
		ace_sigdispatcher.remove_handler( siglist[i] );

	PrcmanSocketTread.stop();
	PrcmanNotificationThread.stop();
	eventmatrix_pipe_hadler.stop();
	BRFThread.stop();
	ManagedElementImplementer.stop();

	PrcmanNotificationThread.wait();
	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - PrcmanNotificationThread terminated", PRCBIN_REVISION );
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	eventmatrix_pipe_hadler.wait();
	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - eventmatrix_pipe_hadler terminated", PRCBIN_REVISION );
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	BRFThread.wait();
	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - BRFThread terminated", PRCBIN_REVISION );
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	ManagedElementImplementer.wait();
	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - ManagedElementImplementer terminated", PRCBIN_REVISION );
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	PrcmanSocketTread.wait();
	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - PrcmanSocketTread terminated", PRCBIN_REVISION );
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	if ( report.alarmAPNotRedundant ( ACS_PRC_ceaseSeverity, ACS_PRC_up, p_local_node_hostname.c_str() ) != ACS_PRC_Report_OK ){
		memset(strErr_no, 0, sizeof(strErr_no));
		Logging.Write("alarmAPNotRedundant Error - ACS_PRC_ceaseSeverity",LOG_LEVEL_ERROR);
		snprintf(strErr_no,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
	}

	if ( report.alarmFCHipCause( ACS_PRC_ceaseSeverity, ACS_PRC_function, p_local_node_hostname.c_str() ) != ACS_PRC_Report_OK ){
		memset(strErr_no, 0, sizeof(strErr_no));
		Logging.Write("alarmFCHipCause Error - ACS_PRC_ceaseSeverity",LOG_LEVEL_ERROR);
		snprintf(strErr_no,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
	}

	stream.close();
	Acceptor.close();
	Acceptor.remove();


	if(strcmp(hostname, "SC-2-1") == 0)
		remove(prc_status1);

	else if(strcmp(hostname, "SC-2-2") == 0)
		remove(prc_status2);

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - Stop", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	Logging.Close();

	create_reboot_file();

	return 0;

}
