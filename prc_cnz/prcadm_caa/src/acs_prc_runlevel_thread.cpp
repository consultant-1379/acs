/*
 * acs_prc_runlevel_thread.cpp
 *
 *  Created on: Nov 20, 2011
 *      Author: xlucpet
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_omhandler.h"
#include "acs_prc_runlevel.h"
#include "ACS_PRC_NotificationThread.h"
#include "acs_prc_reporting.h"
#include "acs_prc_runlevel_thread.h"

#define MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP2 15 // refresh every 30 seconds
#define MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP1 45 // refresh every 90 seconds

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif
bool acs_prc_runlevel_thread::restore_flag = false;
namespace {
	const char * const HTTP_SERVER_STATUS_CHECK_COMMAND_STRING = "/opt/ap/apos/bin/httpmgr -q daemon";
	const char * const HTTP_SERVER_RESTART_COMMAND_STRING      = "/opt/ap/apos/bin/httpmgr -d restart &> /dev/null";
	const char * const HTTP_SERVER_NOT_RUNNING_STATE_STRING    = "Web server is NOT RUNNING";
	const char * const HTTP_SERVER_STOPPED_STATE_STRING        = "Web server is STOPPED";
	const char * const HTTP_SERVER_ALARM_FILE_PATH             = "/var/log/prc_http_server_alarm";
	const int HTTP_SERVER_MAX_NUMBER_OF_RESTARTS               = 14;
	const int HTTP_SERVER_RESTART_WAIT_MICROSECONDS            = 1000000;		// 1 second
	const char reboot_file_fc[]                                = "/var/log/prc_reboot_fc";
	const char lotc_restore_path_clear[] 					   = "/usr/share/pso/storage-paths/clear";
	const char prc_restore_path[]  							   = "/acs-prc/";
}

acs_prc_runlevel_thread::acs_prc_runlevel_thread(): thread_sleep ( false ),_stop_working_condition(_thread_mutex, _condition_attributes){
	RunLevel_thread = new acs_prc_runlevel();
}

acs_prc_runlevel_thread::~acs_prc_runlevel_thread() {
	delete RunLevel_thread;
}

int acs_prc_runlevel_thread::svc (){

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	Logging.Write("acs_prc_runlevel_thread::svc - Start",LOG_LEVEL_WARN);
	int retry_before_refreshSuList = MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP2;
	int retry_GEP_Type = MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP2;

	RunLevel_thread->setlocalNode( local_Node.c_str() );
	RunLevel_thread->setremoteNode( remote_Node.c_str() );

	ACS_APGCC_CommonLib getHWInfo_obj;

	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;

	getHWInfo_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );

	if (hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		if ( hwInfo.hwVersion == ACS_APGCC_HWVER_GEP1 ){ //GEP1 software
			retry_before_refreshSuList = MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP1;
			retry_GEP_Type = MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP1;
		}
		else { // GEP2 - GEP5
			retry_before_refreshSuList = MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP2;
			retry_GEP_Type = MAX_NUMBER_OF_RETRY_BEFORE_RELOAD_SU_LIST_GEP2;
		}
	}
	// start of TR HX58252
	if (hwInfo.hwVersion == ACS_APGCC_HWVER_GEP1 || hwInfo.hwVersion == ACS_APGCC_HWVER_GEP2){
		
		decreaseTipcTolerance();//CC-21559
		if ( access ( reboot_file_fc, F_OK ) != 0 ){
			Logging.Write("acs_prc_runlevel_thread:: reboot_file_fc file is not present",LOG_LEVEL_WARN);
			if(!isPBEEnabled())
			{
				if(!enablePBE(true))
				{
					Logging.Write("acs_prc_runlevel_thread:: enablePBE is success",LOG_LEVEL_WARN);
				}
				else
					Logging.Write("acs_prc_runlevel_thread:: enablePBE is failed",LOG_LEVEL_WARN);
			}
		}
	}  // End of TR HX58252

	while( !thread_sleep ) {

		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
		timeout = timeout.now() + ACE_Time_Value ( 2 );

		if ( retry_before_refreshSuList == 0 ){
			RunLevel_thread->refreshSuList();
			retry_before_refreshSuList = retry_GEP_Type;

			// Only on the active node, execute the monitoring on the HTTP server
			if (ACS_PRC_NotificationThread::getNodeState() == ACS_PRC_NODE_ACTIVE)
				execute_service_monitoring();
		}

		retry_before_refreshSuList--;

		RunLevel_thread->calculateRunLevel();

		if(restore_flag)//start of HX48354
		{
			char strErr_no[1024] = {0};
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - In runlevel thread the restore flag is set", PRCBIN_REVISION );
			Logging.Write(strErr_no, LOG_LEVEL_WARN);
			if(hwInfo.hwVersion == ACS_APGCC_HWVER_VM){
				if(ACS_PRC_NotificationThread::getNodeState() == ACS_PRC_NODE_ACTIVE){
					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - In runlevel thread the current node is Active", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_WARN);
					string tmp = RunLevel_thread->getlocalNode();
					if(RunLevel_thread->getRunLevel(tmp) == 5){
						memset(strErr_no, 0, sizeof(strErr_no));
						snprintf(strErr_no,1024,"acs_prcmand %s - In runlevel thread The Active node is up and runlevel is 5", PRCBIN_REVISION );
						Logging.Write(strErr_no, LOG_LEVEL_WARN);
						memset(strErr_no, 0, sizeof(strErr_no));
						snprintf(strErr_no,1024,"acs_prcmand %s - In runlevel thread calling run_ntp_config_scripts()", PRCBIN_REVISION );
						Logging.Write(strErr_no, LOG_LEVEL_WARN);
						run_ntp_config_scripts();
						if(ACS_PRC_NotificationThread::getSoftwareUpdate() == false)//HY26482
						{
						int result = create_restore_flag(); // The node has been restored so PRC have to create a file under restore "clear" path in order to understand if the reboot is due to a restore or not
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
						restore_flag = false;
					}
				}
				else if(ACS_PRC_NotificationThread::getNodeState() == ACS_PRC_NODE_PASSIVE)
				{
					if(ACS_PRC_NotificationThread::getSoftwareUpdate() == false)//HY26482
					{
					int result = create_restore_flag(); // The node has been restored so PRC have to create a file under restore "clear" path in order to understand if the reboot is due to a restore or not
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
					}
					restore_flag = false;
				}
			}
			else
				restore_flag = false;
		}//end of HX48354
		if ( _stop_working_condition.wait(&timeout) == -1 )
		{
			if (errno != ETIME)  usleep (500000); //TR HU38623
			if (errno != ETIME)  usleep (500000); //TR HU38623
			continue;
		}
		else{
			Logging.Write("acs_prc_runlevel_thread::svc - Stop signaled",LOG_LEVEL_WARN);
			thread_sleep = true;
		}

	}

	Logging.Write("acs_prc_runlevel_thread::svc - Stop",LOG_LEVEL_WARN);

	Logging.Close();
	return 0;
}

void acs_prc_runlevel_thread::updateSUState ( std::string SuName, int SuState ) {
	RunLevel_thread->updateSUState ( SuName, SuState );
}

void acs_prc_runlevel_thread::setNodes ( std::string localNode, std::string remoteNode ){
	local_Node = localNode;
	remote_Node = remoteNode;
}

void acs_prc_runlevel_thread::execute_service_monitoring () {
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	int number_of_restarts = 0, ret_code = 0;
	size_t log_msg_size = 1024;
	char log_msg[log_msg_size];

	::memset(log_msg, 0, log_msg_size);
	::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Checking the HTTP Server state!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
	Logging.Write(log_msg, LOG_LEVEL_TRACE);

	do {
		// Execute the command to check the HTTP server state
		FILE * service_status_fd = 0;
		if (!(service_status_fd = ::popen(HTTP_SERVER_STATUS_CHECK_COMMAND_STRING, "r"))) { // ERROR: Call 'popen' failed
			int errno_save = errno;
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Call 'popen' returned a NULL pointer, errno = %d", PRCBIN_REVISION, __PRETTY_FUNCTION__, errno_save);
			Logging.Write(log_msg, LOG_LEVEL_ERROR);
			return;
		}

		size_t buffer_size = 1024;
		char buffer[buffer_size];

		// Read the command output in order to understand the HTTP server state
		if (!(::fgets(buffer, buffer_size, service_status_fd))) {	// ERROR: Call 'fgets' failed
			int errno_save = errno;
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Call 'fgets' failed, errno == %d", PRCBIN_REVISION, __PRETTY_FUNCTION__, errno_save);
			Logging.Write(log_msg, LOG_LEVEL_ERROR);
			::pclose(service_status_fd);
			return;
		}
		::pclose(service_status_fd);

		// Remove the new line character if present
		size_t str_len = ::strlen(buffer);
		(buffer[str_len - 1] != '\n' ?: buffer[str_len - 1] = 0);

		// If the HTTP server is in stopped state, an O1 alarm must be raised
		if (!(::strncmp(buffer, HTTP_SERVER_STOPPED_STATE_STRING, str_len))) {
			raise_service_stopped_alarm();
			return;
		}

		// In all the other HTTP server states, the alarm must be ceased
		cease_service_stopped_alarm();

		// Check if the HTTP server is running or not, if different from not running, no more operation is needed
		if (::strncmp(buffer, HTTP_SERVER_NOT_RUNNING_STATE_STRING, str_len)) {
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP Server state is '%s', nothing to do!", PRCBIN_REVISION, __PRETTY_FUNCTION__, buffer);
			Logging.Write(log_msg, LOG_LEVEL_TRACE);
			return;
		}

		//changes for the TR HV85090 Start

		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP Server state is '%s'", PRCBIN_REVISION, __PRETTY_FUNCTION__, buffer);
		Logging.Write(log_msg, LOG_LEVEL_TRACE);

		ret_code = ::system("pidof -x apos_operations > /dev/null");

		if(0 == ret_code){
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP Server state is '%s', apos_operations are ongoing, don't restart HTTP Server!", PRCBIN_REVISION, __PRETTY_FUNCTION__, buffer);
			Logging.Write(log_msg, LOG_LEVEL_WARN);
			return;

		}
		//changes for the TR HV85090 End
		//changes for GSNH feature
		ret_code = ::system("pidof -x wssadm > /dev/null");

                if(0 == ret_code){
                        ::memset(log_msg, 0, log_msg_size);
                        ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP Server state is '%s', wssadm command is  ongoing, don't restart HTTP Server!", PRCBIN_REVISION, __PRETTY_FUNCTION__, buffer);
                        Logging.Write(log_msg, LOG_LEVEL_WARN);
                        return;

                }

		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP Server needs to be restarted! ""(number_of_restarts == %d)", PRCBIN_REVISION, __PRETTY_FUNCTION__, ++number_of_restarts);
		Logging.Write(log_msg, LOG_LEVEL_WARN);

		// Execute the command to restart the k Web Server
		::system(HTTP_SERVER_RESTART_COMMAND_STRING);

		// After restarting the Apache Web Server, wait a while before checking again its state
		::usleep(HTTP_SERVER_RESTART_WAIT_MICROSECONDS);
	}
	while ((number_of_restarts < HTTP_SERVER_MAX_NUMBER_OF_RESTARTS) && (!thread_sleep));

	/*
	 * If this point is reached, the HTTP Server has been restarted for
	 * HTTP_SERVER_MAX_NUMBER_OF_RESTARTS but it's still in a bad state.
	 * For that reason it's needed to reboot the node via PRC API.
	 */
	::memset(log_msg, 0, log_msg_size);
	::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP Server has been restarted %d times, "
			"but it's still not running: REBOOTING NODE!", PRCBIN_REVISION, __PRETTY_FUNCTION__, HTTP_SERVER_MAX_NUMBER_OF_RESTARTS);
	Logging.Write(log_msg, LOG_LEVEL_FATAL);

	ACS_PRC_API prc_api_obj;
	prc_api_obj.rebootNode(causedByError, "failover due to HTTP server", true);
	Logging.Close();
	return;
}

void acs_prc_runlevel_thread::raise_service_stopped_alarm () {
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	size_t log_msg_size = 1024;
	char log_msg[log_msg_size];

	// Raise the AP FUNCTION NOT AVAILABLE 01 alarm only if not yet raised
	if (!(::access(HTTP_SERVER_ALARM_FILE_PATH, F_OK))) {
		// If the file is already present, no more operation is needed
		return;
	}

	::memset(log_msg, 0, log_msg_size);
	::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: The HTTP server has been stopped, raising alarm!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
	Logging.Write(log_msg, LOG_LEVEL_WARN);

	// Before raising the alarm, create the alarm file
	FILE * alarm_file = 0;
	if (!(alarm_file = ::fopen(HTTP_SERVER_ALARM_FILE_PATH, "w"))) {
		int errno_save = errno;
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Call 'fopen' for file '%s' returned a NULL pointer, errno = %d",
				PRCBIN_REVISION, __PRETTY_FUNCTION__, HTTP_SERVER_ALARM_FILE_PATH, errno_save);
		Logging.Write(log_msg, LOG_LEVEL_ERROR);
		return;
	}
	::fclose(alarm_file);

	// File created, create the problem text string and raise the alarm!
	char problem_text [1024] = {0};
	char current_date_str [32] = {0};
	char current_time_str [32] = {0};
	time_t current_time = 0;
	struct tm stTm;
	struct tm * ptm = 0;

	::time(&current_time);
	if ((ptm = ::localtime_r(&current_time, &stTm))) {
		::snprintf(current_date_str, sizeof(current_date_str), "%04d-%02d-%02d", stTm.tm_year + 1900, stTm.tm_mon + 1, stTm.tm_mday);
		::snprintf(current_time_str, sizeof(current_time_str), "%02d:%02d:%02d", stTm.tm_hour, stTm.tm_min, stTm.tm_sec);
	}

	::snprintf(problem_text, sizeof(problem_text), "%-16s %s\r\n%-16s %s\r\n\r\n%-16s %-14s %s\r\n%-16s %-14s %s\n",
			"RESOURCE GROUP", "PROCESS", "", "Web Server", "CAUSE", "DATE", "TIME", "Manual stop", current_date_str, current_time_str);

	acs_prc_report report;
	if (report.alarmAPFuncNotAvailable(problem_text) != ACS_PRC_Report_OK) {
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Call 'alarmAPFuncNotAvailable' failed, error = %d, error message: '%s'",
				PRCBIN_REVISION, __PRETTY_FUNCTION__, report.getError(), report.getErrorText().c_str());
		Logging.Write(log_msg, LOG_LEVEL_ERROR);

	}
	Logging.Close();
}

void acs_prc_runlevel_thread::cease_service_stopped_alarm () {
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	size_t log_msg_size = 1024;
	char log_msg[log_msg_size];

	// Cease the AP FUNCTION NOT AVAILABLE 01 alarm only if it has been raised
	if ((::access(HTTP_SERVER_ALARM_FILE_PATH, F_OK))) {
		// If the file is not present, no more operation is needed
		return;
	}

	::memset(log_msg, 0, log_msg_size);
	::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Ceasing AP FUNCTION NOT AVAILABLE alarm!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
	Logging.Write(log_msg, LOG_LEVEL_WARN);

	// Before ceasing the alarm, delete the alarm file
	if (::remove(HTTP_SERVER_ALARM_FILE_PATH)) {
		int errno_save = errno;
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Call 'remove' for file '%s' failed, errno = %d",
				PRCBIN_REVISION, __PRETTY_FUNCTION__, HTTP_SERVER_ALARM_FILE_PATH, errno_save);
		Logging.Write(log_msg, LOG_LEVEL_ERROR);
		return;
	}

	// File deleted, cease the alarm!
	acs_prc_report report;
	if (report.alarmAPFuncNotAvailable("", ACS_PRC_ceaseSeverity) != ACS_PRC_Report_OK) {
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Call 'alarmAPFuncNotAvailable' failed, error = %d, error message: '%s'",
				PRCBIN_REVISION, __PRETTY_FUNCTION__, report.getError(), report.getErrorText().c_str());
		Logging.Write(log_msg, LOG_LEVEL_ERROR);
	}
	Logging.Close();
}

// start of TR HX58252
int acs_prc_runlevel_thread::enablePBE(bool isEnabled)
{
	OmHandler omHandler;
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	size_t log_msg_size = 1024;
	char log_msg[log_msg_size];
	ACS_CC_ReturnType returnCode;
	std::string pbeDn;
	pbeDn.assign("safRdn=immManagement,safApp=safImmService");
	ACE_UINT32 inputValue = 1;
	
	::memset(log_msg, 0, log_msg_size);
                        ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: enablePBE is called",  PRCBIN_REVISION, __PRETTY_FUNCTION__);
                        Logging.Write(log_msg, LOG_LEVEL_WARN);

	if (isEnabled ==  false )
	{
		inputValue = 2;
	}
	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrName = (char *)("saImmRepositoryInit");
	paramToChange.attrType = ATTR_UINT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	void* immRepositoryVal[1] =  {(void*)&inputValue};
	paramToChange.attrValues[0] = reinterpret_cast<void*>(immRepositoryVal[0]);
	for (int i=0;i< 3;i++)
	{
		returnCode = omHandler.Init();
		if ( returnCode != ACS_CC_SUCCESS)
		{
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: OmHandler.Init() failed for %d time!",	PRCBIN_REVISION, __PRETTY_FUNCTION__,i);
			Logging.Write(log_msg, LOG_LEVEL_WARN);
			if ( i >= 3)
			{
				::memset(log_msg, 0, log_msg_size);
				::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: OmHandler.Init() is failed after 3 retries !!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
				Logging.Write(log_msg, LOG_LEVEL_ERROR);
				Logging.Close();
				return returnCode;
			}
			else
			{
				usleep(500000);
				continue;
			}
		}
		else
			break;
	}
	for (int i=0;i< 3;i++)
	{
		if (ACS_CC_FAILURE == omHandler.modifyAttribute(pbeDn.c_str(), &paramToChange))
		{
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: Modify FAILURE. Reason: %s !!!",	PRCBIN_REVISION, __PRETTY_FUNCTION__,omHandler.getInternalLastErrorText());
			Logging.Write(log_msg, LOG_LEVEL_ERROR);
			if ( i >= 3)
			{
				::memset(log_msg, 0, log_msg_size);
				::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: PBE modification is failed after 3 retries !!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
				Logging.Write(log_msg, LOG_LEVEL_ERROR);
				break;
			}
			else
			{
				usleep(500000);
				continue;
			}
		}
		else
		{
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: SUCCESS: Modification of PBE SUCCESS!!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
			Logging.Write(log_msg, LOG_LEVEL_WARN);
			break;
		}
	}
	returnCode= omHandler.Finalize();
	if ( returnCode != ACS_CC_SUCCESS)
	{
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: ERROR: omHandler.Finalize() failure!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
		Logging.Write(log_msg, LOG_LEVEL_ERROR);
	}
	else
	{
		returnCode = ACS_CC_SUCCESS;
	}
	Logging.Close();
	return returnCode;
}

int acs_prc_runlevel_thread::isPBEEnabled() {

	OmHandler omHandler;
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	size_t log_msg_size = 1024;
	char log_msg[log_msg_size];
	ACS_CC_ImmParameter paramToFind;

	int result = 1;
	std::string pbeDn;
	pbeDn.assign("safRdn=immManagement,safApp=safImmService");

	 if ( ACS_CC_SUCCESS != omHandler.Init())
		{
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: OmHandler.Init() failed!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
			Logging.Write(log_msg, LOG_LEVEL_WARN);
			Logging.Close();
			return 0;
		}
	paramToFind.attrName = const_cast<char*>("saImmRepositoryInit");
	if( omHandler.getAttribute(pbeDn.c_str(),&paramToFind ) != ACS_CC_SUCCESS )
	{
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: ERROR: omHandler.getAttribute  Failed!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
		Logging.Write(log_msg, LOG_LEVEL_ERROR);

	}
	if ( paramToFind.attrValuesNum != 0) {
		ACE_UINT32 pbeValue = *reinterpret_cast<ACE_UINT32 *>(*(paramToFind.attrValues));

		if (pbeValue == 1)
		{
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: PBE value is already 1. No need to enable PBE!!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
			Logging.Write(log_msg, LOG_LEVEL_WARN);
			result = 1;
		}
		else if (pbeValue == 2)
		{
			::memset(log_msg, 0, log_msg_size);
			::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: PBE value is 2. Need to enable PBE!!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
			Logging.Write(log_msg, LOG_LEVEL_WARN);
			result = 0;
		}
	}
	else
	{
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: ERROR: saImmRepositoryInit Attribute has not been set the value!",	PRCBIN_REVISION, __PRETTY_FUNCTION__);
		Logging.Write(log_msg, LOG_LEVEL_ERROR);

	}
	if (omHandler.Finalize() == ACS_CC_FAILURE) {
		::memset(log_msg, 0, log_msg_size);
		::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: ERROR: omHandler.Finalize() failure!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
		Logging.Write(log_msg, LOG_LEVEL_ERROR);
	}
	Logging.Close();
	return result;
} // End of TR HX58252

void acs_prc_runlevel_thread::increaseTipcTolerance(){//CC-21559
	
	ACS_TRA_Logging Logging;
        Logging.Open("PRC");
        size_t log_msg_size = 1024;
        char log_msg[log_msg_size];
        
	::memset(log_msg, 0, log_msg_size);
                 ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: setting 25000",      PRCBIN_REVISION, __PRETTY_FUNCTION__);
                 Logging.Write(log_msg, LOG_LEVEL_WARN);

        uid_t my_uid = getuid();
        setuid ( 0 );
        int apNumber = getAPNodeNumber();
        if(apNumber == 1){
                system("tipc bear set tolerance 25000 media eth dev eth3.33");
                system("tipc bear set tolerance 25000 media eth dev eth4.33");
        }
        else if(apNumber == 2){
                system("tipc bear set tolerance 25000 media eth dev eth3.34");
                system("tipc bear set tolerance 25000 media eth dev eth4.34");
        }
        setuid( my_uid );
        Logging.Close();
	return;
}

void acs_prc_runlevel_thread::decreaseTipcTolerance(){//CC-21559
	ACS_TRA_Logging Logging;
        Logging.Open("PRC");
        size_t log_msg_size = 1024;
        char log_msg[log_msg_size];

        uid_t my_uid = getuid();
        setuid ( 0 );
        int apNumber = getAPNodeNumber();
	::memset(log_msg, 0, log_msg_size);
                 ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: setting 1500",      PRCBIN_REVISION, __PRETTY_FUNCTION__);
                 Logging.Write(log_msg, LOG_LEVEL_WARN);
        if(apNumber == 1){
                system("tipc bear set tolerance 1500 media eth dev eth3.33");
                system("tipc bear set tolerance 1500 media eth dev eth4.33");
        }
        else if(apNumber == 2){
                system("tipc bear set tolerance 1500 media eth dev eth3.34");
                system("tipc bear set tolerance 1500 media eth dev eth4.34");
	}
        setuid( my_uid );
	Logging.Close();
        return;
}
int acs_prc_runlevel_thread::getAPNodeNumber() {//CC-21559
        OmHandler omHandler;
        ACS_TRA_Logging Logging;
        Logging.Open("PRC");
        size_t log_msg_size = 1024;
        char log_msg[log_msg_size];
        ACS_CC_ImmParameter paramToFind;
        ACE_UINT32 apNodeNumber=0;
        std::string axefunctionDn;
        axefunctionDn.assign("axeFunctionsId=1");
        if ( ACS_CC_SUCCESS != omHandler.Init())
                {
                        ::memset(log_msg, 0, log_msg_size);
                        ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: OmHandler.Init() failed!",      PRCBIN_REVISION, __PRETTY_FUNCTION__);
                        Logging.Write(log_msg, LOG_LEVEL_WARN);
                        Logging.Close();
                        return 0;
                }
        paramToFind.attrName = const_cast<char*>("apNodeNumber");
        if( omHandler.getAttribute(axefunctionDn.c_str(),&paramToFind ) == ACS_CC_SUCCESS )
        {
                apNodeNumber = *reinterpret_cast<ACE_UINT32 *>(*(paramToFind.attrValues));
                 ::memset(log_msg, 0, log_msg_size);
                ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: WARN: omHandler.apNodeNumber is %d",        PRCBIN_REVISION, __PRETTY_FUNCTION__,apNodeNumber);
                Logging.Write(log_msg, LOG_LEVEL_WARN);
        }
        else
        {
                ::memset(log_msg, 0, log_msg_size);
                ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: ERROR: omHandler.getAttribute  Failed!",        PRCBIN_REVISION, __PRETTY_FUNCTION__);
                Logging.Write(log_msg, LOG_LEVEL_ERROR);
	}
        if (omHandler.Finalize() == ACS_CC_FAILURE) {
                ::memset(log_msg, 0, log_msg_size);
                ::snprintf(log_msg, log_msg_size, "acs_prcmand %s - %s: ERROR: omHandler.Finalize() failure!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
                Logging.Write(log_msg, LOG_LEVEL_ERROR);
        }
        Logging.Close();

        return apNodeNumber;
}


void acs_prc_runlevel_thread::run_ntp_config_scripts()//HX48354
{
	system("/usr/lib/lde/config-management/config/C035ntp config reload;");
	system("/usr/lib/lde/config-management/start/S035ntp start reload;");
}
void acs_prc_runlevel_thread::setRestoreFlag(bool isRestored) //HX48354
{
	acs_prc_runlevel_thread::restore_flag = isRestored;
}
bool acs_prc_runlevel_thread::getRestoreFlag() //HX48354
{
	return acs_prc_runlevel_thread::restore_flag;
}
int acs_prc_runlevel_thread::create_restore_flag() //HX48354
{
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
