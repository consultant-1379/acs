#include "ACS_PRC_NotificationThread.h"
#include "ACS_PRC_RunTimeOwnerThread.h"
#include "ACS_PRC_IspAdminOperationImplementer.h"
#include "ACS_PRC_IspAdminOperationImplementerThread.h"
#include "acs_prc_eventmatrix_implementer.h"
#include "acs_prc_eventmatrix_implementerThread.h"
#include "ACS_PRC_RunTimeOwner.h"
#include "acs_prc_brfImplementer.h"
#include "ACS_PRC_suInfo.h"
#include "acs_prc_api.h"
#include "acs_prc_runlevel_thread.h"
#include "ACS_TRA_Logging.h"
#include "acs_prc_runlevel.h"
#include "acs_prc_reporting.h"
#include "acs_apgcc_oihandler_V3.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"
#include <saNtf.h>
#include <saAmf.h>
#include <saSmf.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include "ACS_APGCC_CommonLib.h"
#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

SaAisErrorT subscribeForNotifications(const saNotificationFilterAllocationParamsT *, SaNtfSubscriptionIdT);
SaAisErrorT waitForNotifications_prcmand(SaNtfHandleT myHandle, int selectionObject, int timeout_ms);
SaAisErrorT sendNotification(const saNotificationAllocationParamsT *, saNotificationParamsT *, const saNotificationFlagsT * );
void freeNtfFilter(SaNtfNotificationFilterHandleT *fh_ptr);
void fill_header_part(SaNtfNotificationHeaderT *, saNotificationParamsT *, SaUint16T );
void fillInDefaultValues(saNotificationAllocationParamsT *, saNotificationFilterAllocationParamsT *, saNotificationParamsT *);

SaNtfHandleT ntfHandle;
SaSelectionObjectT selObj;
char node_id[8];
string node_id_path = "/etc/cluster/nodes/this/id";
const char pipeName[] = "/var/run/ap/PrcManServer";
const char reboot_file_fc[] = "/var/log/prc_reboot_fc";
const char backup_not_done_file[] = "/var/log/backup_not_done";
const char apg_backup_not_done_file[] = "/var/log/apg_backup_not_done";
const char apg_backup_not_done_file_cluster[] = "/cluster/etc/ap/acs/prc/conf/apg_backup_not_done";
const char apg_burbackup_names[] = "ls -lth  /data/opt/ap/internal_root/backup_restore |grep -v '^total' |awk '{print $9}'";//TR HX58934
ACS_PRC_RunTimeOwner RunTimeOwnerHandler;
ACS_PRC_RunTimeOwnerThread ImmDispatchThread;
acs_prc_runlevel_thread acs_prc_runlevel_thread;

acs_prc_eventmatrix_implementer eventMatrix_Implementer;
acs_prc_eventmatrix_implementerThread eventMatrix_Implementer_Thread;
acs_apgcc_oihandler_V3 oiHandler;

string p_node_hostname_local = "";
string p_node_id_local = "";
string p_node_hostname_remote = "";
string p_node_id_remote ="";
bool thread_sleep = false;
extern bool is_swm_2_0 ;
SaNtfCallbacksT ACS_PRC_NotificationThread::ntfCallbacks = { NotificationCallback, 0 };
ACS_TRA_Logging* ACS_PRC_NotificationThread::Logging = new ACS_TRA_Logging();
acs_prc_runlevel* ACS_PRC_NotificationThread::RunLevel = new acs_prc_runlevel();
Ispapi* ACS_PRC_NotificationThread::isp_Event = new Ispapi();

int ACS_PRC_NotificationThread::NodeState = 0;
bool ACS_PRC_NotificationThread::SoftwareUpdate = false;
static SaVersionT version = { 'A', 0x01, 0x01 };

static struct s_filters_T {
	int all;
	int alarm;
	int obj_cr_del;
	int att_ch;
	int st_ch;
	int sec_al;
} used_filters = {1,1,1,1,1,1};

using namespace std;

int ACS_PRC_NotificationThread::getNodeState(){
	return NodeState;
}

bool sortVectorByRules ( string objA, string objB ) {

	return strcmp ( objA.c_str(), objB.c_str() ) < 0;
}
//Start of TR HX58934
bool get_latest_APG_Backup_data(struct tm* tm){

	FILE *fp1=0,*fp2=0;
	int backup_time_len=19,backup_name_length=200;
	char tar_output[200]={0},temp_backup_name[200]={0},Error_to_Log[1024]={0},backup_creation_time[100]={0};
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	/* Open the command for reading. */
	fp1 = popen(apg_burbackup_names, "r");
	if (fp1 == NULL) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - Failed to run command to get list of backups from data disk ", PRCBIN_REVISION);
		Logging.Write(Error_to_Log, LOG_LEVEL_ERROR);
		return false;
	}
	while (fgets(temp_backup_name,backup_name_length, fp1) != NULL) {
		string tmp = temp_backup_name;
		tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
		bool executeTar = false;
		if(tmp.find("-43L-") != string::npos)
		{
			if(strlen(temp_backup_name) >= 24)
			{
				memcpy(backup_creation_time, &temp_backup_name[strlen(temp_backup_name)-backup_time_len-1], backup_time_len);
				if(strptime(backup_creation_time, "%Y-%m-%dT%H-%M-%S", tm) != NULL)  {
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - -43L- found returning with backup name %s", PRCBIN_REVISION,backup_creation_time);
					Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
					if(fp1 != NULL)
						pclose(fp1);
					return true;
				}
				else {
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - -43L- found but no timestamp is present with it so going to execute tar", PRCBIN_REVISION);
					Logging.Write(Error_to_Log, LOG_LEVEL_ERROR);
					executeTar = true;
				}
			}
			else
			{
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - found -43L- but backup length is not >24 so executing tar", PRCBIN_REVISION);
				Logging.Write(Error_to_Log, LOG_LEVEL_ERROR);
				executeTar = true;
			}
		}
		else{
			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - -43L- not found in backup name so executing tar", PRCBIN_REVISION);
			Logging.Write(Error_to_Log, LOG_LEVEL_ERROR);
			executeTar = true;
		}

		if(executeTar)
		{
			memset(backup_creation_time, 0, sizeof(backup_creation_time));
			string str = temp_backup_name;
			str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
			string creation_time_command = "/bin/tar xfO /data/opt/ap/internal_root/backup_restore/" +str+ " "+ str +"/config.metadata";
			fp2 = popen(creation_time_command.c_str(), "r");
			if (fp2 == NULL){
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - Failed to run command to get list of backups from data disk ",PRCBIN_REVISION);
				Logging.Write(Error_to_Log, LOG_LEVEL_ERROR);
				if(fp1 != NULL)
					pclose(fp1);
				return false;
			}
			while (fgets(tar_output, sizeof(tar_output), fp2) != NULL){
				string s_crtm(tar_output);
				if(s_crtm.find("creationTime:") == string::npos)
				{
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - NOT found creation time ", PRCBIN_REVISION);
					Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
					memset(temp_backup_name, 0, sizeof(temp_backup_name));
					continue;
				}
				else
				{
					memcpy(backup_creation_time, &tar_output[strlen(tar_output)-backup_time_len-1], backup_time_len);
					if(strptime(backup_creation_time, "%Y-%m-%d %H:%M:%S", tm) == NULL)
					{
						memset(Error_to_Log, 0, sizeof(Error_to_Log));
						snprintf(Error_to_Log,1024,"acs_prcmand %s - strptime failed ", PRCBIN_REVISION);
						Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
						if(fp1 != NULL)
							pclose(fp1);
						if(fp2 != NULL)
							pclose(fp2);
						return false;
					}
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - latest backup time from data disk is %s ", PRCBIN_REVISION,tar_output);
					Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
					if(fp1 != NULL)
						pclose(fp1);
					if(fp2 != NULL)
						pclose(fp2);
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - in executeTar returning with backup name %s", PRCBIN_REVISION,backup_creation_time);
					Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
					return true;
				}
			}
			if(fp2 != NULL)
				pclose(fp2);
		}

	}
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - leaving get_latest_APG_Backup_data with false as no backup in data disk is valid", PRCBIN_REVISION);
	Logging.Write(Error_to_Log, LOG_LEVEL_ERROR);
	if(fp1 != NULL)
		pclose(fp1);
	return false;
}//End of TR HX58934

time_t getLastBackup_Creation_Time (){

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char Error_to_Log[1024] = {0};
	struct tm tm_sys,tm_data;
	bool no_sys_backup= false;
	OmHandler immHandle;
	vector<ACS_APGCC_ImmAttribute *> attributes;
	ACS_CC_ReturnType result;
	vector<string> tmp_vector;
	vector<string> Backup_Creation_Time_Vector;
	memset(&tm_sys, 0, sizeof(struct tm));
	memset(&tm_data, 0, sizeof(struct tm));
	result = immHandle.Init();
	if(result == ACS_CC_SUCCESS)
	{
		result = immHandle.getClassInstances("BrMBrmBackup",tmp_vector);
		if ( result == ACS_CC_SUCCESS ){
			ACS_CC_ImmParameter Backup_Creation_Time;
			Backup_Creation_Time.attrName = const_cast<char*>("creationTime");

			for ( unsigned int i = 0; i < tmp_vector.size(); i++){
				result = immHandle.getAttribute(tmp_vector[i].c_str(), &Backup_Creation_Time);
				if ( result != ACS_CC_SUCCESS ) {
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - OmHandler.getAttribute() failed! Unable to fetch backups from BrmBackupManager getattribute failed ", PRCBIN_REVISION);
					Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
					break;
				}
				Backup_Creation_Time_Vector.push_back( reinterpret_cast<char*>(Backup_Creation_Time.attrValues[0]));
			}

			immHandle.Finalize();
			sort(Backup_Creation_Time_Vector.begin(), Backup_Creation_Time_Vector.end(), sortVectorByRules);
			if(Backup_Creation_Time_Vector.size() > 0)
				strptime(Backup_Creation_Time_Vector[Backup_Creation_Time_Vector.size()-1].c_str(), "%Y-%m-%dT%H:%M:%S", &tm_sys);
			else
			{
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - No backups found in BrmBackupManager ", PRCBIN_REVISION);
				Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
				memset(&tm_sys, 0, sizeof(struct tm));
				no_sys_backup = true;
			}
		}
		else
		{
			immHandle.Finalize();
			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - OmHandler.getClassInstances() failed! Unable to fetch backups from BrmBackupManager ", PRCBIN_REVISION);
			Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
			memset(&tm_sys, 0, sizeof(struct tm));
			no_sys_backup = true;
		}
	}
	else
	{
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - OmHandler.init() failed! Unable to fetch backups from BrmBackupManager init failed", PRCBIN_REVISION);
		Logging.Write(Error_to_Log, LOG_LEVEL_TRACE);
		memset(&tm_sys, 0, sizeof(struct tm));
		no_sys_backup = true;
	}


	if(get_latest_APG_Backup_data(&tm_data) == false)
	{
		if(no_sys_backup)
			return 0;
		else
		{
			tm_sys.tm_isdst = -1;
			return mktime(&tm_sys);
		}
	}
	tm_sys.tm_isdst = -1;
	tm_data.tm_isdst = -1;
	if(difftime(mktime(&tm_data),mktime(&tm_sys))< 0)
		return mktime(&tm_sys);
	else
		return mktime(&tm_data);

}

string getBlockName ( string& additional_info ){

	size_t start = additional_info.find("safSi=") + 6;
	size_t end;
	if ( is_swm_2_0 == true )
		end = additional_info.find(",safApp=ERIC-apg.nbi.aggregation.service");
	else
		end = additional_info.find(",safApp=ERIC-APG");
	
     return additional_info.substr ( start, end - start);
}

string getBlockName_NoRed ( string& additional_info ){
	size_t start;
	size_t end;
	string tmp;	
        if ( is_swm_2_0 == true){
 		start = additional_info.find("safApp=ERIC-") + 12;
	        tmp = additional_info.substr (start);
        	return tmp + "-" + "NWA-1";
	}	
       else {
		start = additional_info.find("safSg=NoRed_") + 12;
		end = additional_info.find(",safApp=ERIC-APG");
	
		tmp = additional_info.substr ( start, end - start);

		start = additional_info.find("safSu=") + 6;
		end = additional_info.find(",safSg=NoRed_");

		return tmp + "-" + additional_info.substr ( start, end - start);
	}	
}

ACS_PRC_NotificationThread::ACS_PRC_NotificationThread(): implementer(0){
	Logging->Open("PRC");
}

ACS_PRC_NotificationThread::~ACS_PRC_NotificationThread() {
	Logging->Close();
	isp_Event->close();
	delete Logging;
	delete RunLevel;
	delete isp_Event;
}

int ACS_PRC_NotificationThread:: svc (){

	ACS_PRC_API internal;
	isp_Event->open();

	char Error_to_Log[1024] = {0};

	ifstream ifs;

	ifs.open("/etc/cluster/nodes/this/hostname");
	if ( ifs.good())
		getline(ifs,p_node_hostname_local);
	ifs.close();

	ifs.open("/etc/cluster/nodes/this/id");
	if ( ifs.good())
		getline(ifs,p_node_id_local);
	ifs.close();

	ifs.open("/etc/cluster/nodes/peer/hostname");
	if ( ifs.good())
		getline(ifs,p_node_hostname_remote);
	ifs.close();

	ifs.open("/etc/cluster/nodes/peer/id");
	if ( ifs.good())
		getline(ifs,p_node_id_remote);
	ifs.close();

	char hostname[16] = {0};
	int res = gethostname( hostname, sizeof(hostname));

	if ( res == -1 ){

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - ACS_PRC_NotificationThread - gethostname failed - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

		return(EXIT_FAILURE);

	}


	RunLevel->setlocalNode( p_node_hostname_local.c_str() );
	RunLevel->setremoteNode( p_node_hostname_remote.c_str() );

	RunLevel->calculateRunLevel();

	NodeState = 0;
	int tmp_Node_State = internal.askForNodeState();

	int IMM_Try = 1;

	while ( ( ( tmp_Node_State <= 0 ) || ( tmp_Node_State >= 3 ) ) && ( IMM_Try <= 20 ) ) {	// It's not possible to get the Node HA state, try until the state is ACTIVE or PASSIVE

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - IMM not ready askForNodeState fails, try n. %i", PRCBIN_REVISION, IMM_Try++ );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

		usleep(500000); // wait 0.5 sec
		tmp_Node_State = internal.askForNodeState();
	}

	if ( IMM_Try > 20 ){

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - Too many failed connection attempt to IMM - Server is going to restart...", PRCBIN_REVISION );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

		isp_Event->close();

		ACE_LSOCK_Stream Stream;
		ACE_LSOCK_Connector Connector;
		ACE_UNIX_Addr addr (pipeName);

		Connector.connect ( Stream, addr );

		Stream.send_n ( "SHUTDOWN", 9 );

		Stream.close();

		return(EXIT_FAILURE);
	}


	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - Connection to IMM OK - Node State is %i", PRCBIN_REVISION, tmp_Node_State );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	acs_prc_runlevel_thread.setNodes ( p_node_hostname_local, p_node_hostname_remote );

	eventMatrix_Implementer.setImpName("PRCEVA");
	eventMatrix_Implementer.setScope ( ACS_APGCC_SUBTREE );

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - Try to set PRCMAN as object implementer for ApNodeInfo objects", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	ACS_CC_ReturnType RT_Error;
	IMM_Try = 1;

	do {
		if ( p_node_id_local == "1" )
			RT_Error = RunTimeOwnerHandler.init("PRCMAN_1");
		else if ( p_node_id_local == "2" ) 
			RT_Error = RunTimeOwnerHandler.init("PRCMAN_2");
		else{
                        //changes for HV92897
			if(strcmp(hostname, "SC-2-1") == 0)
			{
				RT_Error = RunTimeOwnerHandler.init("PRCMAN_1");
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - removePartecipant - RDN ACS-PRC-1 fetched", PRCBIN_REVISION );
				Logging->Write(Error_to_Log, LOG_LEVEL_TRACE);
			}

			else if(strcmp(hostname, "SC-2-2") == 0)
			{
				RT_Error = RunTimeOwnerHandler.init("PRCMAN_2");
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - removePartecipant - RDN ACS-PRC-2 fetched", PRCBIN_REVISION);
				Logging->Write(Error_to_Log, LOG_LEVEL_TRACE);
			}

			else
			{
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - removePartecipant: Wrong node information - Failure in delete BRF object", PRCBIN_REVISION);
				Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

				return(EXIT_FAILURE);
			}

		}

		if ( RT_Error == ACS_CC_FAILURE ){
			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - Failed to set PRCMAN as runtime owner of ApNodeInfo object - IMM Error = %i - try n. %i", PRCBIN_REVISION, RunTimeOwnerHandler.getInternalLastError(), IMM_Try++ );
			Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

			usleep(250000); // wait 2.5 sec
		}

	} while (( RT_Error == ACS_CC_FAILURE ) && ( IMM_Try <= 20 ));

	if ( IMM_Try > 20 ){

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - Too many failed retry to set PRCMAN as runtime owner of ApNodeInfo object - Server is going to restart...", PRCBIN_REVISION );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

		isp_Event->close();

		ACE_LSOCK_Stream Stream;
		ACE_LSOCK_Connector Connector;
		ACE_UNIX_Addr addr (pipeName);

		Connector.connect ( Stream, addr );

		Stream.send_n ( "SHUTDOWN", 9 );

		Stream.close();

		return(EXIT_FAILURE);
	}

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - PRCMAN is now object implementer of ApNodeInfo objects", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	RunTimeOwnerHandler.create_object();

	ImmDispatchThread.set(&RunTimeOwnerHandler);
	ImmDispatchThread.start();
	ImmDispatchThread.activate();

	acs_prc_runlevel_thread.start();
	acs_prc_runlevel_thread.activate();

	if ( tmp_Node_State == ACS_PRC_NODE_ACTIVE ) {

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - Try to set PRCMAN as class implementer for EventMatrix class", PRCBIN_REVISION );
		Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

		IMM_Try = 1;

		while ( (oiHandler.addClassImpl(&eventMatrix_Implementer,"EventMatrix") == ACS_CC_FAILURE ) && ( IMM_Try <= 20 ) ) {

			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - Failed to set PRCMAN as class implementer for EventMatrix class, try n. %i", PRCBIN_REVISION, IMM_Try++ );
			Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

			usleep(250000); // wait 2.5 sec
		}

		if ( IMM_Try <= 20 ){

			eventMatrix_Implementer_Thread.set(&eventMatrix_Implementer);
			eventMatrix_Implementer_Thread.start();
			eventMatrix_Implementer_Thread.activate();

			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - PRCMAN successfully registered has implementer of EventMatrix class", PRCBIN_REVISION );
			Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		}
		else{
			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - PRCMAN failed registering has implementer of EventMatrix class", PRCBIN_REVISION );
			Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		}

		if (  access ( backup_not_done_file, F_OK ) == 0 ){ // AP Backup not yet done, raise again AP BACKUP MISSING

			memset(Error_to_Log,0,sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - Backup File ( %s ) exist", PRCBIN_REVISION, backup_not_done_file );
			Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

			acs_prc_report report;

			struct stat backup_file_info;
			stat ( backup_not_done_file, &backup_file_info );

			time_t LastBackup_Creation_Time = getLastBackup_Creation_Time();

			char date2 [10] = {0}, time2 [8] = {0};
			char date1 [10] = {0}, time1 [8] = {0};

			string problemText;

			if ( LastBackup_Creation_Time == 0) { // Backup missing
				problemText = "BACKUP MISSING";
			}
			else {
				struct tm* backupTime = localtime ( &LastBackup_Creation_Time );

				sprintf (date1, "%04d%02d%02d", backupTime->tm_year + 1900, backupTime->tm_mon+1, backupTime->tm_mday );
				sprintf (time1, "%02d%02d%02d", backupTime->tm_hour, backupTime->tm_min, backupTime->tm_sec );

				problemText = "LATEST BACKUP\nDATE      TIME\n" + (string)date1 + "  " + (string)time1;
			}

			time_t tmp_time = backup_file_info.st_mtime;
			struct tm* fchtime = localtime ( &tmp_time );

			sprintf (date2, "%04d%02d%02d", fchtime->tm_year + 1900, fchtime->tm_mon+1, fchtime->tm_mday);
			sprintf (time2, "%02d%02d%02d", fchtime->tm_hour, fchtime->tm_min, fchtime->tm_sec);

			problemText += "\n\nLATEST SUCCESSFUL FUNCTION CHANGE\nDATE      TIME\n" + (string)date2 + "  " + (string)time2;

			memset(Error_to_Log,0,sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - Alarm AP BACKUP NOT CREATED raised ( after a reboot )", PRCBIN_REVISION );
			Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

			if ( report.alarmAPBackup(problemText.c_str(), ACS_PRC_o1Severity ) != ACS_PRC_Report_OK ){
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				Logging->Write("alarmAPBackup Error - ACS_PRC_o1Severity",LOG_LEVEL_ERROR);
				snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
				Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
			}
		}
		else {
			memset(Error_to_Log,0,sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcmand %s - Backup File ( %s ) not exist", PRCBIN_REVISION, backup_not_done_file );
			Logging->Write(Error_to_Log,LOG_LEVEL_WARN);
		}

		raise_APG_Backup_Missing();

	}

	if ( tmp_Node_State == ACS_PRC_NODE_ACTIVE ){ // Local Node is in ACTIVE state
		NodeState = 1;
		bool retcode = isp_Event->nodeStateEvent(p_node_hostname_local, active, unknownReason);
		memset(Error_to_Log,0,sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - ISP NodeStateEvent ACTIVE ( %s ) - result ( %s )", PRCBIN_REVISION, p_node_hostname_local.c_str(), retcode?"OK":"NOK");
		Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

	}
	else {
		NodeState = 2;
		bool retcode =  isp_Event->nodeStateEvent(p_node_hostname_local, passive, unknownReason);
		memset(Error_to_Log,0,sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - ISP NodeStateEvent PASSIVE ( %s ) - result ( %s )", PRCBIN_REVISION, p_node_hostname_local.c_str(), retcode?"OK":"NOK");
		Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

	}

	ifs.open(node_id_path.c_str());
	if ( ifs.good())
		ifs.getline(node_id, 8);

	ifs.close();

	int timeout = 300;
	SaAisErrorT error;
	saNotificationFilterAllocationParamsT notificationFilterAllocationParams = {0,0,0,0,0,0,0};
	SaNtfSubscriptionIdT subscriptionId = 1;

	error = saNtfInitialize(&ntfHandle, &ntfCallbacks, &version);

	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfInitialize failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		isp_Event->close();
		return(EXIT_FAILURE);
	}

	error = saNtfSelectionObjectGet(ntfHandle, &selObj);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfSelectionObjectGet failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		isp_Event->close();
		return(EXIT_FAILURE);
	}

	error = subscribeForNotifications(&notificationFilterAllocationParams, subscriptionId);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - subscribeForNotifications failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		isp_Event->close();
		return(EXIT_FAILURE);
	}

	error = waitForNotifications_prcmand(ntfHandle, selObj, timeout);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - waitForNotifications_prcmand failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		isp_Event->close();
		return(EXIT_FAILURE);
	}

	error = saNtfNotificationUnsubscribe(subscriptionId);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationUnsubscribe failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		isp_Event->close();
		return(EXIT_FAILURE);
	}

	error = saNtfFinalize(ntfHandle);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfFinalize failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		isp_Event->close();
		return(EXIT_FAILURE);
	}

	RunTimeOwnerHandler.finalize();

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - PrcmanNotificationThread termination phase start", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	acs_prc_runlevel_thread.stop();
	eventMatrix_Implementer_Thread.stop();
	ImmDispatchThread.stop();

	acs_prc_runlevel_thread.wait();
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_runlevel_thread terminated", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	eventMatrix_Implementer_Thread.wait();
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - eventMatrix_Implementer_Thread terminated", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	ImmDispatchThread.wait();
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - ImmDispatchThread terminated", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - PrcmanNotificationThread termination phase end", PRCBIN_REVISION );
	Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

	//usleep(450000); //wait 0.45 sec

	isp_Event->close();
	return(EXIT_SUCCESS);
}

SaAisErrorT subscribeForNotifications(const saNotificationFilterAllocationParamsT
		*notificationFilterAllocationParams, SaNtfSubscriptionIdT subscriptionId){
	SaAisErrorT errorCode = SA_AIS_OK;
	SaNtfStateChangeNotificationFilterT stChFilter;
	SaNtfAlarmNotificationFilterT myAlarmFilter;

	SaNtfNotificationTypeFilterHandlesT notificationFilterHandles;
	memset(&notificationFilterHandles, 0, sizeof notificationFilterHandles);

	if (used_filters.all || used_filters.st_ch) {
		errorCode = saNtfStateChangeNotificationFilterAllocate(ntfHandle,
				&stChFilter,
				notificationFilterAllocationParams->numEventTypes,
				notificationFilterAllocationParams->numNotificationObjects,
				notificationFilterAllocationParams->numNotifyingObjects,
				notificationFilterAllocationParams->numNotificationClassIds,
				0,
				0);
		if (errorCode != SA_AIS_OK) {
			cout << "saNtfStateChangeNotificationFilterAllocate failed\n" << endl;
			return errorCode;
		}
		notificationFilterHandles.stateChangeFilterHandle = stChFilter.notificationFilterHandle;
	}

	if (used_filters.all || used_filters.alarm) {
		errorCode = saNtfAlarmNotificationFilterAllocate(ntfHandle,
				&myAlarmFilter,
				notificationFilterAllocationParams->numEventTypes,
				notificationFilterAllocationParams->numNotificationObjects,
				notificationFilterAllocationParams->numNotifyingObjects,
				notificationFilterAllocationParams->numNotificationClassIds,
				notificationFilterAllocationParams->numProbableCauses,
				notificationFilterAllocationParams->numPerceivedSeverities,
				notificationFilterAllocationParams->numTrends);

		if (errorCode != SA_AIS_OK) {
			cout << "saNtfAlarmNotificationFilterAllocate failed\n" << endl;
			return errorCode;
		}
		notificationFilterHandles.alarmFilterHandle = myAlarmFilter.notificationFilterHandle;
	}

	errorCode = saNtfNotificationSubscribe(&notificationFilterHandles, subscriptionId);
	if (SA_AIS_OK != errorCode) {
		cout << "saNtfNotificationSubscribe failed\n" << endl;
		return errorCode;
	}
	freeNtfFilter (&notificationFilterHandles.alarmFilterHandle);
	freeNtfFilter (&notificationFilterHandles.stateChangeFilterHandle);

	return errorCode;
}

SaAisErrorT waitForNotifications_prcmand(SaNtfHandleT myHandle, int selectionObject, int timeout_ms){
	SaAisErrorT error = SA_AIS_OK;
	int rv;
	struct pollfd fds[1];

	fds[0].fd = (int)selectionObject;
	fds[0].events = POLLIN;

	while( !thread_sleep ) {
		rv = poll(fds, 1, timeout_ms);

		if ( rv == 0 ) continue;

		if (rv == -1) {
			if (errno == EINTR)
				continue;
			fprintf(stdout, "poll FAILED: %s\n", strerror(errno));
			return SA_AIS_ERR_BAD_OPERATION;
		}
		else {
			if ( !thread_sleep ){
				do {
					error = saNtfDispatch(myHandle, SA_DISPATCH_ALL);
					if (SA_AIS_ERR_TRY_AGAIN == error)
						sleep(1);
				} while (SA_AIS_ERR_TRY_AGAIN == error);

				if (error != SA_AIS_OK)
					fprintf(stderr, "saNtfDispatch Error %d\n", error);
			}
		}
	}

	return error;
}

void ACS_PRC_NotificationThread::NotificationCallback(SaNtfSubscriptionIdT /*subscriptionId*/, const SaNtfNotificationsT *notification){

	char Error_to_Log[1024] = {0};

	char tmpObj[SA_MAX_NAME_LENGTH + 1] = {0};
	char tmpObject_notifying[SA_MAX_NAME_LENGTH + 1] = {0};

	SaNtfNotificationHandleT notificationHandle;
	const SaNtfNotificationHeaderT *notificationHeader;

	acs_prc_report report;

	switch (notification->notificationType) {
		case SA_NTF_TYPE_STATE_CHANGE :
			{
				ACS_PRC_API prc_api;

				notificationHandle = notification->notification.stateChangeNotification.notificationHandle;
				notificationHeader = &notification->notification.stateChangeNotification.notificationHeader;

				strncpy(tmpObject_notifying, (char *)notificationHeader->notifyingObject->value, notificationHeader->notifyingObject->length);
				tmpObject_notifying[notificationHeader->notifyingObject->length] = '\0';

				strncpy(tmpObj,(char *)notificationHeader->notificationObject->value, notificationHeader->notificationObject->length);
				tmpObj[notificationHeader->notificationObject->length] = '\0';
				string tmp = (string)tmpObj;

				memset ( tmpObj, 0, SA_MAX_NAME_LENGTH + 1);

				string additional_info;
				string additional_text = "";

				if ( notificationHeader->additionalInfo != NULL ) {
					if ( notificationHeader->additionalInfo[0].infoType == SA_NTF_VALUE_LDAP_NAME) {
						SaNameT *dataPtr;
						SaUint16T dataSize;
						SaAisErrorT rc;

						rc = saNtfPtrValGet( notificationHandle, &notificationHeader->additionalInfo[0].infoValue, (void **)&dataPtr, &dataSize );

						if (rc == SA_AIS_OK){
							strncpy( tmpObj, (char*)dataPtr->value, dataPtr->length);
							additional_info = (string)tmpObj;
						}
					}
				}

				if ( notificationHeader->lengthAdditionalText > 0)
					additional_text = notificationHeader->additionalText;

				if ( notification->notification.stateChangeNotification.numStateChanges <= 0) {
					memset(Error_to_Log, 0, sizeof(Error_to_Log));
					snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> An empty SA_NTF_TYPE_STATE_CHANGE notification has been received ==> notification is discarded", PRCBIN_REVISION );
					Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
					break;
				}

				int stateChangeNotification_NewState = notification->notification.stateChangeNotification.changedStates[0].newState;
				 string ServiceUniteState;
                                 string ServiceUniteState_noRed;
                                 string NodeStateSi;
                                 string NodeStateSu;
				if ( is_swm_2_0 == true ){
					ServiceUniteState = "safApp=ERIC-apg.nbi.aggregation.service";

					ServiceUniteState_noRed = "safSg=NWA";

					NodeStateSi = "safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service";

					NodeStateSu = "safSu=SC-";
					NodeStateSu.append(node_id);
					NodeStateSu.append(",safSg=2N,safApp=ERIC-apg.nbi.aggregation.service");
				}
				else{
					ServiceUniteState = "safApp=ERIC-APG";

					ServiceUniteState_noRed = "safSg=NoRed_";

					NodeStateSi = "safSi=AGENT,safApp=ERIC-APG";

					NodeStateSu = "safSu=";
					NodeStateSu.append(node_id);
					NodeStateSu.append(",safSg=2N,safApp=ERIC-APG");				
				}

				if ( ( notification->notification.stateChangeNotification.changedStates[0].stateId == SA_AMF_HA_STATE ) && ( tmp.find ( NodeStateSu ) != string::npos )) {
					if ( additional_info.find ( NodeStateSi ) != string::npos ){ // The AGENT SI HA State is changed
						if ( stateChangeNotification_NewState == SA_AMF_HA_STANDBY ||
								stateChangeNotification_NewState == SA_AMF_HA_QUIESCED ||
								stateChangeNotification_NewState == SA_AMF_HA_QUIESCING ){ // STANDBY - QUIESCED - QUESCING

							NodeState = 2;

							memset(Error_to_Log, 0, sizeof(Error_to_Log));
							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> Node State Change ==> Node is PASSIVE", PRCBIN_REVISION );
							Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

							//if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
							isp_Event->nodeStateEvent(p_node_hostname_local, passive, causedByError);

							RunLevel->calculateRunLevel();

							if ( eventMatrix_Implementer_Thread.isRunning() ){

								int IMM_Try = 1;

								while ( (oiHandler.removeClassImpl(&eventMatrix_Implementer,"EventMatrix") == ACS_CC_FAILURE ) && ( IMM_Try <= 20 ) ) {

									memset(Error_to_Log, 0, sizeof(Error_to_Log));
									snprintf(Error_to_Log,1024,"acs_prcmand %s - Failed to remove PRCMAN as class implementer for EventMatrix class, try n. %i", PRCBIN_REVISION, IMM_Try++ );
									Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

									usleep(250000); // wait 2.5 sec
								}

								eventMatrix_Implementer_Thread.stop();
								eventMatrix_Implementer_Thread.wait();
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcmand %s - eventMatrix_Implementer_Thread terminated", PRCBIN_REVISION );
								Logging->Write(Error_to_Log, LOG_LEVEL_WARN);
							}

							memset(Error_to_Log, 0, sizeof(Error_to_Log));
							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback - Cease alarmAPNotRedundant alarm on PASSIVE node", PRCBIN_REVISION);
							Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);

							if ( report.alarmAPNotRedundant ( ACS_PRC_ceaseSeverity, ACS_PRC_up, p_node_hostname_local.c_str() ) != ACS_PRC_Report_OK ){
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
								Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
							}

							memset(Error_to_Log, 0, sizeof(Error_to_Log));
							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback - Cease AP BACKUP NOT CREATED alarm on PASSIVE node", PRCBIN_REVISION);
							Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);

							if ( report.alarmAPBackup("",ACS_PRC_ceaseSeverity) != ACS_PRC_Report_OK ){
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
								Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
							}

							memset(Error_to_Log, 0, sizeof(Error_to_Log));
							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback - Cease APG BACKUP NOT CREATED alarm on PASSIVE node", PRCBIN_REVISION);
							Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);

							if ( report.alarmAPGBackup("",ACS_PRC_ceaseSeverity) != ACS_PRC_Report_OK ){
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
								Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
							}

							acs_prc_runlevel_thread.cease_service_stopped_alarm();

						}else if (stateChangeNotification_NewState == SA_AMF_HA_ACTIVE ){ // ACTIVE

							NodeState = 1;

							memset(Error_to_Log, 0, sizeof(Error_to_Log));
							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> Node State Change ==> Node is ACTIVE", PRCBIN_REVISION );
							Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

							if(acs_prc_brfImplementer::getBackupAndRestore())/*START OF HY59098*/
							{
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcmand %s - Clearing BackupAndRestore flag during transistion from passive to ACTIVE", PRCBIN_REVISION );
								Logging->Write(Error_to_Log,LOG_LEVEL_WARN);
								acs_prc_brfImplementer::setBackupAndRestore(false);
							}/*END OF HY59098*/

							//if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
							isp_Event->nodeStateEvent(p_node_hostname_local, active, unknownReason);

							RunLevel->calculateRunLevel();

							if ( !eventMatrix_Implementer_Thread.isRunning() ){
								int IMM_Try = 1;

								while ( (oiHandler.addClassImpl(&eventMatrix_Implementer,"EventMatrix") == ACS_CC_FAILURE ) && ( IMM_Try <= 20 ) ) {

									memset(Error_to_Log, 0, sizeof(Error_to_Log));
									snprintf(Error_to_Log,1024,"acs_prcmand %s - Failed to set PRCMAN as class implementer for EventMatrix class, try n. %i", PRCBIN_REVISION, IMM_Try++ );
									Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

									usleep(250000); // wait 2.5 sec
								}

								if ( IMM_Try <= 20 ){

									eventMatrix_Implementer_Thread.set ( &eventMatrix_Implementer );
									eventMatrix_Implementer_Thread.start();
									eventMatrix_Implementer_Thread.activate();

									memset(Error_to_Log, 0, sizeof(Error_to_Log));
									snprintf(Error_to_Log,1024,"acs_prcmand %s - PRCMAN successfully registered has implementer of EventMatrix class", PRCBIN_REVISION );
									Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
								}
								else{
									memset(Error_to_Log, 0, sizeof(Error_to_Log));
									snprintf(Error_to_Log,1024,"acs_prcmand %s - PRCMAN failed registering has implementer of EventMatrix class", PRCBIN_REVISION );
									Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
								}
							}

							if (  access ( backup_not_done_file, F_OK ) == 0 ){ // AP Backup not yet done, raise again AP BACKUP MISSING

								memset(Error_to_Log,0,sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcmand %s - Backup File ( %s ) exist", PRCBIN_REVISION, backup_not_done_file );
								Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

								struct stat backup_file_info;
								stat ( backup_not_done_file, &backup_file_info );

								time_t LastBackup_Creation_Time = getLastBackup_Creation_Time();

								char date2 [10] = {0}, time2 [8] = {0};
								char date1 [10] = {0}, time1 [8] = {0};

								string problemText;

								if ( LastBackup_Creation_Time == 0) { // Backup missing
									problemText = "BACKUP MISSING";
								}
								else {
									struct tm* backupTime = localtime ( &LastBackup_Creation_Time );

									sprintf (date1, "%04d%02d%02d", backupTime->tm_year + 1900, backupTime->tm_mon+1, backupTime->tm_mday );
									sprintf (time1, "%02d%02d%02d", backupTime->tm_hour, backupTime->tm_min, backupTime->tm_sec );

									problemText = "LATEST BACKUP\nDATE      TIME\n" + (string)date1 + "  " + (string)time1;
								}

								time_t tmp_time = backup_file_info.st_mtime;
								struct tm* fchtime = localtime ( &tmp_time );

								sprintf (date2, "%04d%02d%02d", fchtime->tm_year + 1900, fchtime->tm_mon+1, fchtime->tm_mday);
								sprintf (time2, "%02d%02d%02d", fchtime->tm_hour, fchtime->tm_min, fchtime->tm_sec);

								problemText += "\n\nLATEST SUCCESSFUL FUNCTION CHANGE\nDATE      TIME\n" + (string)date2 + "  " + (string)time2;

								memset(Error_to_Log,0,sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> Alarm AP BACKUP NOT CREATED raised", PRCBIN_REVISION );
								Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

								if ( report.alarmAPBackup(problemText.c_str(), ACS_PRC_o1Severity ) != ACS_PRC_Report_OK ){
									memset(Error_to_Log, 0, sizeof(Error_to_Log));
									Logging->Write("alarmAPBackup Error - ACS_PRC_o1Severity",LOG_LEVEL_ERROR);
									snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
									Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
								}

							}
							else {
								memset(Error_to_Log,0,sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcmand %s - Backup File ( %s ) not exist", PRCBIN_REVISION, backup_not_done_file );
								Logging->Write(Error_to_Log,LOG_LEVEL_WARN);
							}

							raise_APG_Backup_Missing();
						}
					}else { // An 2N APG application has changed its HA state
						string apg_block_name = getBlockName ( additional_info );

						if ( stateChangeNotification_NewState == SA_AMF_HA_STANDBY ||
								stateChangeNotification_NewState == SA_AMF_HA_QUIESCED ||
								stateChangeNotification_NewState == SA_AMF_HA_QUIESCING ){ // STANDBY - QUIESCED - QUESCING

							//if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
							isp_Event->SuStateEvent(apg_block_name,p_node_hostname_local,stopped,causedByError);

							RunLevel->updateSUState( additional_text, -2 );
							acs_prc_runlevel_thread.updateSUState( additional_text, -2 );

							RunLevel->calculateRunLevel();

						}else if ( stateChangeNotification_NewState == SA_AMF_HA_ACTIVE ){ // ACTIVE

							//if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
							isp_Event->SuStateEvent(apg_block_name,p_node_hostname_local,started,unknownReason);

							RunLevel->updateSUState( additional_text, SA_AMF_HA_ACTIVE );
							acs_prc_runlevel_thread.updateSUState( additional_text, SA_AMF_HA_ACTIVE );

							RunLevel->calculateRunLevel();
						}
					}
				}
				else
					if ( ( notification->notification.stateChangeNotification.changedStates[0].stateId == SA_AMF_ADMIN_STATE ) &&
							( ( tmp.find ( ServiceUniteState ) != string::npos ) &&	( tmp.find ( ServiceUniteState_noRed ) != string::npos ))) {

						string apg_block_name_noRed = getBlockName_NoRed ( tmp );

						if ( stateChangeNotification_NewState == SA_AMF_ADMIN_UNLOCKED ){ //No_Red is active

							if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
								isp_Event->SuStateEvent(apg_block_name_noRed,p_node_hostname_local,started,unknownReason);

							RunLevel->updateSUState( apg_block_name_noRed, SA_AMF_HA_ACTIVE );
							acs_prc_runlevel_thread.updateSUState( apg_block_name_noRed, SA_AMF_HA_ACTIVE );

							RunLevel->calculateRunLevel();

						}
						else if ( stateChangeNotification_NewState == SA_AMF_ADMIN_LOCKED ) { //No_Red is passive

							if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
								isp_Event->SuStateEvent(apg_block_name_noRed,p_node_hostname_local,stopped,causedByError);

							RunLevel->updateSUState( apg_block_name_noRed, -2 );
							acs_prc_runlevel_thread.updateSUState( apg_block_name_noRed, -2 );

							RunLevel->calculateRunLevel();

						}
					}
					else
						if ( ( notification->notification.stateChangeNotification.changedStates[0].stateId == SA_AMF_PRESENCE_STATE ) &&
								( ( tmp.find ( ServiceUniteState ) != string::npos ) &&	( tmp.find ( ServiceUniteState_noRed ) != string::npos ))){

							string apg_block_name_noRed = getBlockName_NoRed ( tmp );

							if ( stateChangeNotification_NewState == SA_AMF_PRESENCE_INSTANTIATED ){ //No_Red is active due no node unlock

								//if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
								isp_Event->SuStateEvent(apg_block_name_noRed,p_node_hostname_local,started,unknownReason);

								RunLevel->updateSUState( apg_block_name_noRed, SA_AMF_HA_ACTIVE );
								acs_prc_runlevel_thread.updateSUState( apg_block_name_noRed, SA_AMF_HA_ACTIVE );

								RunLevel->calculateRunLevel();

							}
							else if ( stateChangeNotification_NewState == SA_AMF_PRESENCE_UNINSTANTIATED ) { //No_Red is passive due to node lock

								//if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )
								isp_Event->SuStateEvent(apg_block_name_noRed,p_node_hostname_local,stopped,causedByError);

								RunLevel->updateSUState( apg_block_name_noRed, -2 );
								acs_prc_runlevel_thread.updateSUState( apg_block_name_noRed, -2 );

								RunLevel->calculateRunLevel();

							}

						}

				if ( ( notificationHeader->notificationClassId->vendorId == SA_NTF_VENDOR_ID_SAF ) &&
						( notificationHeader->notificationClassId->majorId == SA_SVC_SMF ) &&
						( notificationHeader->notificationClassId->minorId == 101 ) &&
						( tmp.find ( "safSmfCampaign" ) != string::npos )) { // Software Update Notification

					if ( stateChangeNotification_NewState == SA_SMF_CMPG_CAMPAIGN_COMMITTED ||
							stateChangeNotification_NewState == SA_SMF_CMPG_ROLLBACK_COMMITTED ||
							stateChangeNotification_NewState == SA_SMF_CMPG_ROLLBACK_FAILED ||
							stateChangeNotification_NewState == SA_SMF_CMPG_EXECUTION_FAILED ){

						bool is_VM = false;//start of HY26482
						ACS_APGCC_CommonLib getHWInfo_obj;
						ACS_APGCC_HWINFO hwInfo;
						ACS_APGCC_HWINFO_RESULT hwInfoResult;
						getHWInfo_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );

				   if ( access ( reboot_file_fc, F_OK ) == 0 || (ACS_PRC_NotificationThread::getSoftwareUpdate() == true) ){ // Reboot ordered by Software Update
					 if (hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
						if ( hwInfo.hwVersion == ACS_APGCC_HWVER_VM ){
								//memset(strErr_no, 0, sizeof(strErr_no));
								snprintf(Error_to_Log,1024,"acs_prcmand %s - Setting the NTP connection in software update case,to make connection between CP and NTP server proper", PRCBIN_REVISION );
								Logging->Write(Error_to_Log, LOG_LEVEL_WARN);
								ACS_PRC_NotificationThread::setRestoreFlagInRunLevelThread(true);
								is_VM=true;
									}
							     }
						   }//end of HY26482

						SoftwareUpdate = false;

						remove( reboot_file_fc );

						FILE * 	file_write = fopen(backup_not_done_file, "w");
						fclose (file_write);

						report.alarmFCHipCause( ACS_PRC_ceaseSeverity, ACS_PRC_function, p_node_hostname_local.c_str() );

						snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> Alarm AP FUNCTION CHANGE IN PROGRESS ceased", PRCBIN_REVISION );
						Logging->Write(Error_to_Log,LOG_LEVEL_WARN);
						if (( stateChangeNotification_NewState == SA_SMF_CMPG_CAMPAIGN_COMMITTED ) && ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE )){

							time_t LastBackup_Creation_Time = getLastBackup_Creation_Time();

							char date2 [10] = {0}, time2 [8] = {0};
							char date1 [10] = {0}, time1 [8] = {0};

							string problemText;

							if ( LastBackup_Creation_Time == 0) { // Backup missing
								problemText = "BACKUP MISSING";
							}
							else {
								struct tm* backupTime = localtime ( &LastBackup_Creation_Time );

								sprintf (date1, "%04d%02d%02d", backupTime->tm_year + 1900, backupTime->tm_mon+1, backupTime->tm_mday );
								sprintf (time1, "%02d%02d%02d", backupTime->tm_hour, backupTime->tm_min, backupTime->tm_sec );

								problemText = "LATEST BACKUP\nDATE      TIME\n" + (string)date1 + "  " + (string)time1;
							}

							time_t tmp_time = time(NULL);
							struct tm* fchtime = localtime ( &tmp_time );

							sprintf (date2, "%04d%02d%02d", fchtime->tm_year + 1900, fchtime->tm_mon+1, fchtime->tm_mday);
							sprintf (time2, "%02d%02d%02d", fchtime->tm_hour, fchtime->tm_min, fchtime->tm_sec);

							problemText += "\n\nLATEST SUCCESSFUL FUNCTION CHANGE\nDATE      TIME\n" + (string)date2 + "  " + (string)time2;

							memset(Error_to_Log,0,sizeof(Error_to_Log));
							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> Alarm AP BACKUP NOT CREATED raised", PRCBIN_REVISION );
							Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

							if ( report.alarmAPBackup(problemText.c_str(), ACS_PRC_o1Severity ) != ACS_PRC_Report_OK ){
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								Logging->Write("alarmAPBackup Error - ACS_PRC_o1Severity",LOG_LEVEL_ERROR);
								snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
								Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
							}
						}
					}
					else {
						SoftwareUpdate = true;

						// If the reboot has been initiated by Software Update
						FILE * 	file_write = fopen(reboot_file_fc, "w");
						fclose (file_write);
						if ( prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE ){

							snprintf(Error_to_Log,1024,"acs_prcmand %s - saNtfNotificationCallback ==> Alarm AP FUNCTION CHANGE IN PROGRESS raised", PRCBIN_REVISION );
							Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

							report.alarmFCHipCause( ACS_PRC_o1Severity, ACS_PRC_function, p_node_hostname_local.c_str() );
						}
					}
				}

				saNtfNotificationFree(notification->notification.stateChangeNotification.notificationHandle);

				break;
			}

		default :
			{
				break;
			}
	}
}

void freeNtfFilter(SaNtfNotificationFilterHandleT *fh_ptr) {

	SaAisErrorT errorCode = SA_AIS_OK;
	if (*fh_ptr) {
		errorCode = saNtfNotificationFilterFree(*fh_ptr);
		if (SA_AIS_OK != errorCode) {
			cout << "saNtfNotificationFilterFree failed\n" << endl;
			exit(EXIT_FAILURE);
		}
	}
}

SaAisErrorT sendNotification(const saNotificationAllocationParamsT *notificationAllocationParams, saNotificationParamsT *notificationParams, const saNotificationFlagsT *notificationFlags){

	SaNtfStateChangeNotificationT myStateChangeNotification;

	if (*notificationFlags == (saNotificationFlagsT)DEFAULT_FLAG)
		notificationParams->eventType = notificationParams->stateChangeEventType;

	SaAisErrorT errorCode = saNtfStateChangeNotificationAllocate ( ntfHandle,
			&myStateChangeNotification,
			notificationAllocationParams->numCorrelatedNotifications,
			notificationAllocationParams->lengthAdditionalText,
			notificationAllocationParams->numAdditionalInfo,
			notificationAllocationParams->numStateChanges,
			notificationAllocationParams->variableDataSize );

	if ( errorCode != SA_AIS_OK)
		return errorCode;

	fill_header_part(&myStateChangeNotification.notificationHeader, ( saNotificationParamsT *)notificationParams, notificationAllocationParams->lengthAdditionalText);

	*(myStateChangeNotification.sourceIndicator) = notificationParams->stateChangeSourceIndicator;

	for ( int i = 0; i < notificationAllocationParams->numStateChanges; i++) {
		myStateChangeNotification.changedStates[i].newState = notificationParams->changedStates[i].newState;
		myStateChangeNotification.changedStates[i].oldState = notificationParams->changedStates[i].oldState;
		myStateChangeNotification.changedStates[i].oldStatePresent = notificationParams->changedStates[i].oldStatePresent;
		myStateChangeNotification.changedStates[i].stateId = notificationParams->changedStates[i].stateId;
	}

	errorCode = saNtfNotificationSend(myStateChangeNotification.notificationHandle);

	if ( errorCode != SA_AIS_OK )
		return errorCode;

	errorCode = saNtfNotificationFree(myStateChangeNotification.notificationHandle);

	if ( errorCode != SA_AIS_OK )
		return errorCode;

	return errorCode;
}

void fill_header_part(SaNtfNotificationHeaderT *notificationHeader, saNotificationParamsT *notificationParams, SaUint16T lengthAdditionalText){

	*notificationHeader->eventType = notificationParams->eventType;
	*notificationHeader->eventTime = (SaTimeT)notificationParams->eventTime;

	*notificationHeader->notificationObject = notificationParams->notificationObject;

	*notificationHeader->notifyingObject = notificationParams->notifyingObject;

	notificationHeader->notificationClassId->vendorId = notificationParams->notificationClassId.vendorId;

	notificationHeader->notificationClassId->majorId = notificationParams->notificationClassId.majorId;
	notificationHeader->notificationClassId->minorId = notificationParams->notificationClassId.minorId;

	strncpy(notificationHeader->additionalText, notificationParams->additionalText, lengthAdditionalText);
}

void fillInDefaultValues(saNotificationAllocationParamsT *notificationAllocationParams,
		saNotificationFilterAllocationParamsT *notificationFilterAllocationParams,
		saNotificationParamsT *notificationParams){

	/* Default notification allocation parameters */
	/* Common notification header */
	notificationAllocationParams->numCorrelatedNotifications = 0;
	notificationAllocationParams->lengthAdditionalText = (SaUint16T)(strlen(DEFAULT_ADDITIONAL_TEXT) + 1);
	notificationAllocationParams->numAdditionalInfo = 0;

	/* Alarm specific */
	notificationAllocationParams->numSpecificProblems = 0;
	notificationAllocationParams->numMonitoredAttributes = 0;
	notificationAllocationParams->numProposedRepairActions = 0;

	/* State change specific */
	notificationAllocationParams->numStateChanges = DEFAULT_NUMBER_OF_CHANGED_STATES;

	/* Object Create/Delete specific */
	notificationAllocationParams->numObjectAttributes = DEFAULT_NUMBER_OF_OBJECT_ATTRIBUTES;

	/* Attribute Change specific */
	notificationAllocationParams->numAttributes = DEFAULT_NUMBER_OF_CHANGED_ATTRIBUTES;

	notificationParams->changedStates[0].stateId = MY_APP_OPER_STATE;
	notificationParams->changedStates[0].oldStatePresent = SA_FALSE;
	notificationParams->changedStates[0].newState = SA_NTF_DISABLED;

	notificationParams->changedStates[1].stateId = MY_APP_USAGE_STATE;
	notificationParams->changedStates[1].oldStatePresent = SA_FALSE;
	notificationParams->changedStates[1].newState = SA_NTF_IDLE;

	notificationParams->changedStates[2].stateId = MY_APP_OPER_STATE;
	notificationParams->changedStates[2].oldStatePresent = SA_TRUE;
	notificationParams->changedStates[2].oldState = SA_NTF_DISABLED;
	notificationParams->changedStates[2].newState = SA_NTF_ENABLED;

	notificationParams->changedStates[3].stateId = MY_APP_USAGE_STATE;
	notificationParams->changedStates[3].oldStatePresent = SA_TRUE;
	notificationParams->changedStates[3].oldState = SA_NTF_IDLE;
	notificationParams->changedStates[3].newState = SA_NTF_ACTIVE;

	notificationParams->objectAttributes[0].attributeId = 58;
	notificationParams->objectAttributes[0].attributeType = SA_NTF_VALUE_INT32;
	notificationParams->objectAttributes[0].attributeValue.int32Val = 1;

	notificationParams->objectAttributes[1].attributeId = 4;
	notificationParams->objectAttributes[1].attributeType = SA_NTF_VALUE_UINT32;
	notificationParams->objectAttributes[1].attributeValue.int32Val = 16;

	notificationParams->changedAttributes[0].attributeId = 0;
	notificationParams->changedAttributes[0].attributeType = SA_NTF_VALUE_INT32;
	notificationParams->changedAttributes[0].oldAttributePresent = SA_FALSE;
	notificationParams->changedAttributes[0].newAttributeValue.int32Val = 1;

	notificationParams->changedAttributes[1].attributeId = 1;
	notificationParams->changedAttributes[1].attributeType = SA_NTF_VALUE_INT32;
	notificationParams->changedAttributes[1].oldAttributePresent = SA_TRUE;
	notificationParams->changedAttributes[1].oldAttributeValue.int32Val = 8;
	notificationParams->changedAttributes[1].newAttributeValue.int32Val = -4;

	/* Misc */
	notificationAllocationParams->variableDataSize = SA_NTF_ALLOC_SYSTEM_LIMIT;

	/* Default notification filter allocation parameters */
	notificationFilterAllocationParams->numEventTypes = 0;
	notificationFilterAllocationParams->numNotificationObjects = 0;
	notificationFilterAllocationParams->numNotifyingObjects = 0;
	notificationFilterAllocationParams->numNotificationClassIds = 0;

	/* Alarm specific */
	notificationFilterAllocationParams->numProbableCauses = 0;
	notificationFilterAllocationParams->numPerceivedSeverities = 2;
	notificationFilterAllocationParams->numTrends = 0;

	/* Default notification parameters */
	notificationParams->additionalText = (SaStringT)malloc(notificationAllocationParams->lengthAdditionalText);

	notificationParams->notificationType = SA_NTF_TYPE_ALARM;

	(void)strncpy(notificationParams->additionalText,
			DEFAULT_ADDITIONAL_TEXT, notificationAllocationParams->lengthAdditionalText);
	notificationParams->notificationObject.length = strlen(DEFAULT_NOTIFICATION_OBJECT);
	(void)memcpy(notificationParams->notificationObject.value,
			DEFAULT_NOTIFICATION_OBJECT, notificationParams->notificationObject.length);
	notificationParams->notifyingObject.length = strlen(DEFAULT_NOTIFYING_OBJECT);
	(void)memcpy(notificationParams->notifyingObject.value,
			DEFAULT_NOTIFYING_OBJECT, notificationParams->notifyingObject.length);
	notificationParams->notificationClassId.vendorId = ERICSSON_VENDOR_ID;
	notificationParams->notificationClassId.majorId = 0;
	notificationParams->notificationClassId.minorId = 0;
	notificationParams->eventTime = (SaTimeT)SA_TIME_UNKNOWN;

	/* Alarm specific */
	notificationParams->probableCause = SA_NTF_BANDWIDTH_REDUCED;
	notificationParams->perceivedSeverity = SA_NTF_SEVERITY_WARNING;
	notificationParams->alarmEventType = SA_NTF_ALARM_COMMUNICATION;

	/* State change specific */
	notificationParams->stateChangeSourceIndicator = SA_NTF_OBJECT_OPERATION;
	notificationParams->stateChangeEventType = SA_NTF_OBJECT_STATE_CHANGE;

	/* Object Create Delete specific */
	notificationParams->objectCreateDeleteSourceIndicator = SA_NTF_UNKNOWN_OPERATION;
	notificationParams->objectCreateDeleteEventType = SA_NTF_OBJECT_CREATION;

	/* Attribute Change params */
	notificationParams->attributeChangeSourceIndicator = SA_NTF_UNKNOWN_OPERATION;
	notificationParams->attributeChangeEventType = SA_NTF_ATTRIBUTE_ADDED;

	/* Security Alarm params */
	notificationParams->securityAlarmEventType = SA_NTF_INTEGRITY_VIOLATION;
	notificationParams->severity = SA_NTF_SEVERITY_CRITICAL;
	notificationParams->securityAlarmProbableCause = SA_NTF_UNAUTHORIZED_ACCESS_ATTEMPT;
	notificationParams->securityAlarmDetector.valueType = SA_NTF_VALUE_INT32;
	notificationParams->securityAlarmDetector.value.int32Val = 1;
	notificationParams->serviceProvider.valueType = SA_NTF_VALUE_INT32;
	notificationParams->serviceProvider.value.int32Val = 2;
	notificationParams->serviceUser.valueType = SA_NTF_VALUE_INT32;
	notificationParams->serviceUser.value.int32Val = 3;

	/* One day */
	notificationParams->timeout = (24 * 3600);
	notificationParams->burstTimeout = 0;

	/* send the same message repeatSends times */
	notificationParams->repeateSends = 1;
	notificationParams->subscriptionId = 1;
}

void ACS_PRC_NotificationThread::start() {
	thread_sleep = false;
}

void ACS_PRC_NotificationThread::stop() {
	thread_sleep = true;
}

void ACS_PRC_NotificationThread::raise_APG_Backup_Missing(){

	char Error_to_Log[1024] = {0};
	char date2 [10] = {0}, time2 [8] = {0};
	struct tm* fchtime;
	time_t tmp_time;
	bool raiseAlarm = true;

	acs_prc_report report;

	if (access ( apg_backup_not_done_file, F_OK ) == 0){ // APG Backup not yet done, raise again APG BACKUP MISSING
		struct stat backup_file_info;
		stat ( apg_backup_not_done_file, &backup_file_info );

		tmp_time = backup_file_info.st_mtime;
	}
	else if (access(apg_backup_not_done_file_cluster, F_OK) == 0){
		struct stat backup_file_info;
		stat ( apg_backup_not_done_file_cluster, &backup_file_info );

		tmp_time = backup_file_info.st_mtime;
	}
	else {
		raiseAlarm = false;
		memset(Error_to_Log,0,sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - Backup Files ( %s / %s ) not exist", PRCBIN_REVISION, apg_backup_not_done_file, apg_backup_not_done_file_cluster );
		Logging->Write(Error_to_Log,LOG_LEVEL_WARN);
	}

	if (raiseAlarm){
		fchtime = localtime (&tmp_time);

		sprintf (date2, "%04d%02d%02d", fchtime->tm_year + 1900, fchtime->tm_mon+1, fchtime->tm_mday);
		sprintf (time2, "%02d%02d%02d", fchtime->tm_hour, fchtime->tm_min, fchtime->tm_sec);

		std::string problemText = "\n\nLATEST SUCCESSFUL MODEL CHANGE\nDATE      TIME\n" + (string)date2 + "  " + (string)time2;

		memset(Error_to_Log,0,sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - Alarm APG BACKUP NOT CREATED raised", PRCBIN_REVISION );
		Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

		if ( report.alarmAPGBackup(problemText.c_str(), ACS_PRC_o1Severity ) != ACS_PRC_Report_OK ){
			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			Logging->Write("alarmAPGBackup Error - ACS_PRC_o1Severity",LOG_LEVEL_ERROR);
			snprintf(Error_to_Log,1024,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
			Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
		}
	}
}


void ACS_PRC_NotificationThread::setRestoreFlagInRunLevelThread(bool isRestored)
{
	acs_prc_runlevel_thread::setRestoreFlag(isRestored);
}

bool ACS_PRC_NotificationThread::getRestoreFlag()
{
	return acs_prc_runlevel_thread::getRestoreFlag();
}
