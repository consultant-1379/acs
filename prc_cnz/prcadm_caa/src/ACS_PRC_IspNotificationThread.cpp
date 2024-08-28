#include "ACS_PRC_IspNotificationThread.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_PRC_suInfo.h"
#include "acs_prc_api.h"
#include "ACS_TRA_Logging.h"
#include <saNtf.h>
#include <saAmf.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <fstream>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include <acs_prc_runlevel.h>

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif
extern bool is_swm_2_0_isp;
SaAisErrorT subscribeForNotifications(const saNotificationFilterAllocationParamsT *, SaNtfSubscriptionIdT);
SaAisErrorT waitForNotifications_prcispd(SaNtfHandleT myHandle, int selectionObject, int timeout_ms);
void saNtfNotificationCallback(SaNtfSubscriptionIdT subscriptionId, const SaNtfNotificationsT *notification);
void freeNtfFilter(SaNtfNotificationFilterHandleT *fh_ptr);

static Ispapi pcr_isp_Event;
SaNtfHandleT ntfHandle;
SaSelectionObjectT selObj;
char node_id[8];
string node_id_path = "/etc/cluster/nodes/this/id";
const char pipeName[] = "/var/run/ap/AcsIspprintServer";

string p_node_hostname = "";
string p_node_id = "";

bool thread_sleep = false;

SaNtfCallbacksT ACS_PRC_IspNotificationThread::ntfCallbacks = { NotificationCallback, 0 };
ACS_TRA_Logging* ACS_PRC_IspNotificationThread::Logging = new ACS_TRA_Logging();

runLevelType acs_prc_runlevel::currentRunLevel_local = level_0;
runLevelType acs_prc_runlevel::currentRunLevel_remote = level_0;

ACS_PRC_IspAdminOperationImplementer IspAdminOperationImplementer;
ACS_PRC_IspAdminOperationImplementerThread IspAdminOperationImplementerThread;
acs_apgcc_oihandler_V2 oiHandler;

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

acs_apgcc_oihandler_V2* ACS_PRC_IspNotificationThread::getOI_Handler(){
	return &oiHandler;
}

ACS_PRC_IspAdminOperationImplementer* ACS_PRC_IspNotificationThread::getOI(){
	return &IspAdminOperationImplementer;
}

ACS_PRC_IspNotificationThread::ACS_PRC_IspNotificationThread() {
	// TODO Auto-generated constructor stub
	Logging->Open("PRC");
}

ACS_PRC_IspNotificationThread::~ACS_PRC_IspNotificationThread() {
	// TODO Auto-generated destructor stub
	Logging->Close();
	delete Logging;
}

int ACS_PRC_IspNotificationThread:: svc (){

	ACS_PRC_API internal;

	char Error_to_Log[1024] = {0};

	ifstream ifs;

	ifs.open("/etc/cluster/nodes/this/hostname");
	if ( ifs.good())
		getline(ifs,p_node_hostname);
	ifs.close();

	ifs.open("/etc/cluster/nodes/this/id");
	if ( ifs.good())
		getline(ifs,p_node_id);
	ifs.close();
	int tmp_Node_State = internal.askForNodeState();
	int IMM_Try = 1;

	while ( ( ( tmp_Node_State <= 0 ) || ( tmp_Node_State >= 3 ) ) && ( IMM_Try <= 30 ) ) {	// It's not possible to get the Node HA state, try until the state is ACTIVE or PASSIVE

		snprintf(Error_to_Log,1024,"acs_prcispd %s - IMM not ready askForNodeState fails, try n. %i", PRCBIN_REVISION, IMM_Try++ );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

		sleep(2); // wait 2 second
		tmp_Node_State = internal.askForNodeState();
	}

	if ( IMM_Try > 30 ){

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - Too many failed connection attempt to IMM - Server is going to restart...", PRCBIN_REVISION );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

		ACE_LSOCK_Stream Stream;
		ACE_LSOCK_Connector Connector;
		ACE_UNIX_Addr addr (pipeName);

		Connector.connect ( Stream, addr );

		Stream.send_n ( "SHUTDOWN", 9 );

		Stream.close();

		return(EXIT_FAILURE);
	}

	pcr_isp_Event.open();

    //OmHandler prc_OmHandler;
    //vector<string> tmp_vector;

    //prc_OmHandler.Init();

    //prc_OmHandler.getClassInstances("ProcessControl",tmp_vector);

    //prc_OmHandler.Finalize();

	IspAdminOperationImplementer.setImpName("PRCISP");
	//IspAdminOperationImplementer.setObjName ("ispConfigId=1,"+tmp_vector[0]);
	IspAdminOperationImplementer.setObjName ("ispConfigId=1,processControlId=1");
	IspAdminOperationImplementer.setScope ( ACS_APGCC_ONE );

	if ( tmp_Node_State == ACS_PRC_NODE_ACTIVE ){ // Local Node is in ACTIVE state

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - Try to set PRCISP as object implementer for ispConfigId objects", PRCBIN_REVISION );
		Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

		IMM_Try = 1;

		while ( ( oiHandler.addObjectImpl(&IspAdminOperationImplementer) == ACS_CC_FAILURE ) && ( IMM_Try <= 30 ) ) {

			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcispd %s - Failed to set PRCISP as class implementer for ispConfigId objects, try n. %i", PRCBIN_REVISION, IMM_Try++ );
			Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

			usleep(250000); // wait 2.5 sec
		}

		if ( IMM_Try <= 30 ){

			IspAdminOperationImplementerThread.set(&IspAdminOperationImplementer);
			IspAdminOperationImplementerThread.start();
			IspAdminOperationImplementerThread.activate();

			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcispd %s - PRCISP successfully registered has implementer of ispConfigId objects", PRCBIN_REVISION );
			Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		}
		else{
			memset(Error_to_Log, 0, sizeof(Error_to_Log));
			snprintf(Error_to_Log,1024,"acs_prcispd %s - PRCISP failed registering has implementer of ispConfigId objects", PRCBIN_REVISION );
			Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		}
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
		snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfInitialize failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		pcr_isp_Event.close();
		return(EXIT_FAILURE);
	}

	error = saNtfSelectionObjectGet(ntfHandle, &selObj);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfSelectionObjectGet failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		pcr_isp_Event.close();
		return(EXIT_FAILURE);
	}

	error = subscribeForNotifications(&notificationFilterAllocationParams, subscriptionId);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - subscribeForNotifications failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		pcr_isp_Event.close();
		return(EXIT_FAILURE);
	}

	error = waitForNotifications_prcispd(ntfHandle, selObj, timeout);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - waitForNotifications_prcispd failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		pcr_isp_Event.close();
		return(EXIT_FAILURE);
	}

	error = saNtfNotificationUnsubscribe(subscriptionId);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfNotificationUnsubscribe failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		pcr_isp_Event.close();
		return(EXIT_FAILURE);
	}

	error = saNtfFinalize(ntfHandle);
	if (SA_AIS_OK != error) {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfFinalize failed - SAF Return Code = %i", PRCBIN_REVISION, error );
		Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
		pcr_isp_Event.close();
		return(EXIT_FAILURE);
	}

	IspAdminOperationImplementerThread.stop();
	IspAdminOperationImplementerThread.wait();

	pcr_isp_Event.close();
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

SaAisErrorT waitForNotifications_prcispd(SaNtfHandleT myHandle, int selectionObject, int timeout_ms){
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
			fprintf(stderr, "poll FAILED: %s\n", strerror(errno));
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

void ACS_PRC_IspNotificationThread::NotificationCallback(SaNtfSubscriptionIdT subscriptionId, const SaNtfNotificationsT *notification){

	ACE_UNUSED_ARG (subscriptionId);

	char Error_to_Log[1024] = {0};

	char tmpObj[SA_MAX_NAME_LENGTH + 1] = {0};
	char tmpObject_notifying[SA_MAX_NAME_LENGTH + 1] = {0};

	SaNtfNotificationHandleT notificationHandle;
	const SaNtfNotificationHeaderT *notificationHeader;

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
			string tmp = tmpObj;

			memset ( tmpObj, 0, SA_MAX_NAME_LENGTH + 1);

			string additional_info;

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

	        if ( notification->notification.stateChangeNotification.numStateChanges <= 0) {
	        	memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfNotificationCallback ==> An empty SA_NTF_TYPE_STATE_CHANGE notification has been received ==> notification is discarded", PRCBIN_REVISION );
				Logging->Write(Error_to_Log,LOG_LEVEL_ERROR);
				break;
	        }

	        int stateChangeNotification_NewState = notification->notification.stateChangeNotification.changedStates[0].newState;
			string NodeStateSi;
			string NodeStateSu;
                        if (is_swm_2_0_isp == true){
				NodeStateSi = "safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service";
				NodeStateSu = "safSu=SC-";
				NodeStateSu.append(node_id);
				NodeStateSu.append(",safSg=2N,safApp=ERIC-apg.nbi.aggregation.service");
                        }
                       else{
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

						memset(Error_to_Log, 0, sizeof(Error_to_Log));
						snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfNotificationCallback ==> Node State Change ==> Node is PASSIVE", PRCBIN_REVISION );
						Logging->Write(Error_to_Log,LOG_LEVEL_WARN);

						if ( IspAdminOperationImplementerThread.isRunning() ){
							int IMM_Try = 1;
							IspAdminOperationImplementerThread.stop();
							IspAdminOperationImplementerThread.wait();

							while ( (oiHandler.removeObjectImpl(&IspAdminOperationImplementer) == ACS_CC_FAILURE ) && ( IMM_Try <= 30 ) ) {

								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcispd %s - Failed to remove PRCISP as object implementer for ispConfigId objects, try n. %i", PRCBIN_REVISION, IMM_Try++ );
								Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

								usleep(250000); // wait 2.5 sec
							}
						}

					}else if (stateChangeNotification_NewState == SA_AMF_HA_ACTIVE ){ // ACTIVE

						snprintf(Error_to_Log,1024,"acs_prcispd %s - saNtfNotificationCallback ==> Node State Change ==> Node is ACTIVE", PRCBIN_REVISION );
						Logging->Write(Error_to_Log, LOG_LEVEL_WARN);

						if ( !IspAdminOperationImplementerThread.isRunning() ){

							int IMM_Try = 1;

							while ( ( oiHandler.addObjectImpl(&IspAdminOperationImplementer) == ACS_CC_FAILURE ) && ( IMM_Try <= 30) ){

								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcispd %s - Failed to set PRCISP as object implementer for ispConfigId objects, try n. %i", PRCBIN_REVISION, IMM_Try++ );
								Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);

								usleep(250000); // wait 2.5 sec
							}

							if ( IMM_Try <= 30 ){

								IspAdminOperationImplementerThread.set(&IspAdminOperationImplementer);
								IspAdminOperationImplementerThread.start();
								IspAdminOperationImplementerThread.activate();

								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcispd %s - PRCISP successfully registered has implementer of ispConfigId objects", PRCBIN_REVISION );
								Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
							}
							else{
								memset(Error_to_Log, 0, sizeof(Error_to_Log));
								snprintf(Error_to_Log,1024,"acs_prcispd %s - PRCISP failed registering has implementer of ispConfigId objects", PRCBIN_REVISION );
								Logging->Write(Error_to_Log, LOG_LEVEL_ERROR);
							}
						}
					}
				}
			}

			saNtfNotificationFree(notification->notification.stateChangeNotification.notificationHandle);

			break;
		}
		default :
			break;
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

void ACS_PRC_IspNotificationThread::start() {
	thread_sleep = false;
}

void ACS_PRC_IspNotificationThread::stop() {
	thread_sleep = true;
}

