/*
 * ispprintBackupThread.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: xlucpet
 */

#include "ACS_PRC_ispprintBackupThread.h"
#include <acs_apgcc_paramhandling.h>
#include "acs_apgcc_omhandler.h"
#include <acs_prc_api.h>
#include <ACS_PRC_ispapi.h>
#include <acs_prc_runlevel.h>
#include <unistd.h>
#include <fstream>

const char PRC_ISP_CONF_FILE[] = "/etc/logrot.d/acs_prc_isp_conf";

void createLog(char* logFileName) {

	int status;
	string cmd = "ispprint -a -l -n -r -d > ";
	cmd.append(logFileName);

	pid_t pid = fork();
	if (pid == 0) {
		if(execlp("sh","sh", "-c", cmd.c_str(), (char *) NULL) == -1){
			printf("Error: execlp: %d \n",errno);
		}
	}
	else if (pid < 0)
		printf("Fatal error fork() failed. %d \n",errno);

	waitpid(pid, &status, 0);

}

void send_node_state_sync_event(string hostname, int local_node_state, runLevelType local_Runlevel) {

	Ispapi prc_isp;
	ACS_PRC_API prc_api;

	prc_isp.open();

	reasonType reason;

	if ( local_Runlevel <= level_4 )
		reason = resourceFailed;
	else
		reason = unknownReason;

	if ( local_node_state == ACS_PRC_NODE_ACTIVE )
		prc_isp.node_State_Runlevel_Sync( hostname, active, unknownReason, local_Runlevel, reason);
	else
		prc_isp.node_State_Runlevel_Sync( hostname, passive, causedByError, local_Runlevel, reason);

	prc_isp.close();

}

ACS_PRC_ispprintBackupThread::ACS_PRC_ispprintBackupThread():maxlogSize(10240), sleep_thread(false) {}

ACS_PRC_ispprintBackupThread::~ACS_PRC_ispprintBackupThread() {
	// TODO Auto-generated destructor stub
}

int ACS_PRC_ispprintBackupThread::svc ( void ){

	sleep(10 * 60); // Wait 10 minutes after startup

	ACS_PRC_API internal;

	string p_node_id_local;
	int sync_event = 0;

	char buff_host[32] = {0};
	gethostname(buff_host,sizeof(buff_host));

	string backupLog = "/var/log/";

	backupLog.append(buff_host);
	backupLog += "/ISP_%Y-%m.log";

	ifstream ifs;

	ifs.open("/etc/cluster/nodes/this/id");
	if ( ifs.good())
		getline(ifs,p_node_id_local);
	ifs.close();

	acs_apgcc_paramhandling ispLogSize;

    OmHandler prc_OmHandler;
    vector<string> tmp_vector;

	while( !sleep_thread ){

		int local_node_state = internal.askForNodeState();

		if ( local_node_state == ACS_PRC_NODE_ACTIVE ){
			struct tm  *ts;
			time_t     now;

			time(&now);
			ts = localtime(&now);

			char fileName[128] = {0};

			strftime(fileName, sizeof(fileName), backupLog.c_str(), ts);

			createLog(fileName);
		}

		sleep( 60 * 60 ); // 1 hour
		sync_event++;

		if ( sync_event == 12 ){
			int local_runlevel = 0;

			acs_apgcc_paramhandling par_hdl;

			if ( par_hdl.getParameter("apNodeInfoId=1,processControlId=1", (!p_node_id_local.compare("1") ? "nodeRunLevelA" : "nodeRunLevelB") , &local_runlevel) ){
				if ( par_hdl.getParameter("apNodeInfoId=2,processControlId=1", (!p_node_id_local.compare("1") ? "nodeRunLevelA" : "nodeRunLevelB"), &local_runlevel)){
					local_runlevel = level_5;
				}
			}

			send_node_state_sync_event(string(buff_host), local_node_state,(runLevelType)local_runlevel);
			sync_event = 0;
		}

	    if ( prc_OmHandler.Init() == ACS_CC_SUCCESS ){
	    	if ( prc_OmHandler.getClassInstances("ProcessControl",tmp_vector) == ACS_CC_SUCCESS ){

	    		if ( tmp_vector.size() > 0 ){
						if ( ispLogSize.getParameter("ispConfigId=1,"+tmp_vector[0], "maxLogSize", &maxlogSize) == ACS_CC_SUCCESS )
							init_logrotd();
	    		}
	    	}

	    	prc_OmHandler.Finalize();
	    }
	}

	return 0;
}

void ACS_PRC_ispprintBackupThread::init_logrotd( bool remove_file ) {

	if ( remove_file )
		::remove(PRC_ISP_CONF_FILE);

	if ( ::access( PRC_ISP_CONF_FILE, F_OK) ){ //FIle doesn't exist
	    char isp_conf_file_row[128] = {0};
	    snprintf(isp_conf_file_row,sizeof(isp_conf_file_row),"/cluster/etc/ap/acs/prc/prcisp_syslog.log %lld 3",maxlogSize/2);

		int isp_conf_file = -1;

		if ( (isp_conf_file = ::open(PRC_ISP_CONF_FILE, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR )) != -1 ){

			::write(isp_conf_file, isp_conf_file_row, strlen(isp_conf_file_row));

			::close(isp_conf_file);

			int status;

			string cmd = "/usr/bin/systemctl restart lde-logrot.service &>/dev/null";

			pid_t pid = fork();
			if (pid == 0) {
				if(execlp("sh","sh", "-c", cmd.c_str(), (char *) NULL) == -1){
					printf("Error: execlp: %d \n",errno);
				}
			}
			else if (pid < 0)
				printf("Fatal error fork() failed. %d \n",errno);

			waitpid(pid, &status, 0);
		}
	}
}
