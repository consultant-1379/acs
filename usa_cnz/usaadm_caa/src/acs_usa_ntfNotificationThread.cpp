#include <poll.h>
#include <saAmf.h>
#include <saNtf.h>
#include <saAis.h>
#include <saClm.h>
#include "acs_usa_logtrace.h"
#include <stdio.h>
#include <acs_usa_ntfNotificationThread.h>
#include <acs_usa_types.h>
#include "acs_usa_ntpSrvM.h"
#include "acs_usa_adhIntUserObserver.h"
#include "acs_usa_sysloganalyser.h"

ACS_USA_Trace traNTF ("ACS_USA_NtfNotificationThread         ");
bool ACS_USA_NtfNotificationThread::thread_sleep = false;
extern bool is_swm_2_0;
/* Release code, major version, minor version */
static SaVersionT version = { 'A', 0x01, 0x01 };
static struct s_filters_T {
	int all;
	int alarm;
	int obj_cr_del;
	int att_ch;
	int st_ch;
	int sec_al;
} used_filters = {1,1,1,1,1,1};

namespace ntfConstants {
	const string ap_node_id = get_ap_node_id();
	const string ap_other_node_id = get_partner_ap_node_id();
	const string ap_node_hostname = get_ap_hostname();
}

static char *error_output(SaAisErrorT result)
{
	static char error_result[256];
	sprintf(error_result, "error: %u", result);
	return (error_result);
}

SaNtfCallbacksT ACS_USA_NtfNotificationThread::ntfCallbacks = {
	saNtfNotificationCallback, 0
};

void ACS_USA_NtfNotificationThread::saNtfNotificationCallback(SaNtfSubscriptionIdT /*subscriptionId*/, const SaNtfNotificationsT *notification)
{
	SaNtfNotificationHandleT notificationHandle;
	const SaNtfNotificationHeaderT *notificationHeader;
	char notificationObject[SA_MAX_NAME_LENGTH + 1];
	char additional_info[SA_MAX_NAME_LENGTH + 1]= {0};
	char tmpObject_1[SA_MAX_NAME_LENGTH + 1] = {0};
	char tmpObject_2[SA_MAX_NAME_LENGTH + 1] = {0};
	SaNtfStateChangeT *saNtfStateChange = 0;

	if(!notification)
	{	 // should not occur, but protect us against this anomaly
		traNTF.traceAndLogFmt(ERROR, "%s() - saNtfNotificationCallback()' invoked with the parameter 'notification' equal to 0 !", __func__);
		return;
	}
	if(notification->notification.stateChangeNotification.numStateChanges == 0)
	{
		traNTF.traceAndLogFmt(INFO, "ACS_USA_NtfNotificationThread:%s() - received state change notification with zero state changes !", __func__);
		return;
	}

	if(!thread_sleep) {		
		if(notification->notificationType == SA_NTF_TYPE_STATE_CHANGE) {
			notificationHandle = notification->notification.stateChangeNotification.notificationHandle;
			notificationHeader = &notification->notification.stateChangeNotification.notificationHeader;
			strncpy(notificationObject, (char *)notificationHeader->notificationObject->value, notificationHeader->notificationObject->length);
			notificationObject[notificationHeader->notificationObject->length] = '\0';
			// Get Additional Info associated to the notification
			if ( notificationHeader->additionalInfo != NULL ) {
				if ( notificationHeader->additionalInfo[0].infoType == SA_NTF_VALUE_LDAP_NAME) {
					SaNameT *dataPtr;
					SaUint16T dataSize;
					SaAisErrorT rc;
					rc = saNtfPtrValGet( notificationHandle, &notificationHeader->additionalInfo[0].infoValue, (void **)&dataPtr, &dataSize );
					if (rc == SA_AIS_OK){
						strncpy( additional_info, (char*)dataPtr->value, dataPtr->length);
					}
				}
			}
			  string NodeStateSu;
			  string NodeStateSi;
			  if ( is_swm_2_0 == true )
        	          {
                               NodeStateSu="safSu=SC-%s,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service";
                               NodeStateSi="safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service";
                          }
                          else{
                               NodeStateSu= "safSu=%s,safSg=2N,safApp=ERIC-APG";
                               NodeStateSi="safSi=AGENT,safApp=ERIC-APG";
                          }

			// check if the STATE_CHANGE is of type "SA_AMF_HA_STATE" and if the notification is related to AP nodes
			if (!ntfConstants::ap_node_id.empty()) sprintf(tmpObject_1, NodeStateSu.c_str(), ntfConstants::ap_node_id.c_str());
			if (!ntfConstants::ap_other_node_id.empty()) sprintf(tmpObject_2, NodeStateSu.c_str(), ntfConstants::ap_other_node_id.c_str());
			bool cond_1 = !strcmp(tmpObject_1, notificationObject);
			bool cond_2 = !strcmp(tmpObject_2, notificationObject);
			saNtfStateChange = & notification->notification.stateChangeNotification.changedStates[0];
			if(saNtfStateChange && (saNtfStateChange->stateId == SA_AMF_HA_STATE) && (cond_1 || cond_2) && strstr(additional_info,NodeStateSi.c_str() ) != NULL)
			{
				if(cond_1 && (saNtfStateChange->newState == SA_AMF_HA_ACTIVE))
				{
					if (ACS_USA_Global::instance()->isVirtual()) {
						traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() ----this AP node is active", __func__);
						ACS_USA_Global::instance()->NtpSrvM()->monitorNtpServer();
						traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() ----check if implementer set", __func__);

						if(!ACS_USA_Global::instance()->implementer_set())
						{
							traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() ----let set the implementer ", __func__);
							ACS_USA_Global::instance()->NtpSrvM()->registerNtpObserver();
						}
					}
					if(!ACS_USA_Global::instance()->adh_implementer_set())
					{
						traNTF.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s() ----let set the implementer ", __func__);
						ACS_USA_Global::instance()->AdhIntThread()->activate();
						ACS_USA_Global::instance()->Analyser()->loadConfig();
					}

				}
				else if( (cond_2 && (saNtfStateChange->newState == SA_AMF_HA_ACTIVE)) ||  /* the other AP Node has become ACTIVE */
					(cond_1 &&
					(
				        (saNtfStateChange->newState == SA_AMF_HA_STANDBY)   || /* this AP Node has become PASSIVE */
				        (saNtfStateChange->newState == SA_AMF_HA_QUIESCED)  ||   /* this AP Node entered QUIESCED state */
					(saNtfStateChange->newState == SA_AMF_HA_QUIESCING) /* this AP Node entered QUIESCING state */
					)
			        	)
			   		)
				{
					if (ACS_USA_Global::instance()->isVirtual()) {
						traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() ----this AP node is not active", __func__);
						if(ACS_USA_Global::instance()->implementer_set())
						{
							traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() ----let remove the implementer ", __func__);
							ACS_USA_Global::instance()->NtpSrvM()->removeNtpObserver();
						}
					}
					traNTF.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver():%s ----this AP node is not active", __func__);
					if(ACS_USA_Global::instance()->adh_implementer_set())
					{
						traNTF.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s() ----let remove the implementer ", __func__);
						ACS_USA_Global::instance()->AdhIntThread()->shutdown();
						traNTF.traceAndLogFmt(ERROR,"%s() waiting for the ADHObserver to close", __func__);
					}
				}
			}
		}
	}
	if(notification->notificationType == SA_NTF_TYPE_STATE_CHANGE) {
		saNtfNotificationFree(notification->notification.stateChangeNotification.notificationHandle);
	}
}
SaAisErrorT ACS_USA_NtfNotificationThread::waitForNotifications(SaNtfHandleT myHandle, int selectionObject, int timeout_ms)
{
	SaAisErrorT error = SA_AIS_OK;
	int rv;
	struct pollfd fds[1];

	fds[0].fd = (int)selectionObject;
	fds[0].events = POLLIN;
	while( !thread_sleep ) {
		rv = poll(fds, 1, timeout_ms);

		if ( rv == 0 ){
			continue;
		}	

		if (rv == -1) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "poll FAILED: %s\n", strerror(errno));
			return SA_AIS_ERR_BAD_OPERATION;
		}
		else {
			do {
				error = saNtfDispatch(myHandle, SA_DISPATCH_ALL);
				if (SA_AIS_ERR_TRY_AGAIN == error)
					sleep(1);
			} while (SA_AIS_ERR_TRY_AGAIN == error);
				
			if (error != SA_AIS_OK)
				fprintf(stderr, "saNtfDispatch Error %d\n", error);
		}
	}
	return error;
}


void ACS_USA_NtfNotificationThread::freeNtfFilter(SaNtfNotificationFilterHandleT *fh_ptr)
{
	SaAisErrorT errorCode = SA_AIS_OK;
	if (*fh_ptr) {
		errorCode = saNtfNotificationFilterFree(*fh_ptr);
		if (SA_AIS_OK != errorCode) {
			traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() - saNtfNotificationFilterFree() failed !", __func__);
		}
	}
}

/* Subscribe */
SaAisErrorT ACS_USA_NtfNotificationThread::subscribeForNotifications(const saNotificationFilterAllocationParamsT * notificationFilterAllocationParams, SaNtfSubscriptionIdT subscriptionId)
{
	SaAisErrorT errorCode = SA_AIS_OK;
	SaNtfStateChangeNotificationFilterT stChFilter;
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
			traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() - saNtfStateChangeNotificationFilterAllocate failed ", __func__);
			return errorCode;
		}
		notificationFilterHandles.stateChangeFilterHandle = stChFilter.notificationFilterHandle;
	}
	errorCode = saNtfNotificationSubscribe(&notificationFilterHandles, subscriptionId);
	if (SA_AIS_OK != errorCode) {
		traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread:%s() - saNtfNotificationSubscribe failed ", __func__);
		return errorCode;
	}
	freeNtfFilter (&notificationFilterHandles.stateChangeFilterHandle);
	return errorCode;
}
ACS_USA_NtfNotificationThread::ACS_USA_NtfNotificationThread(){

	USA_TRACE_ENTER();
	m_global_instance= ACS_USA_Global::instance();
	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_NtfNotificationThread::~ACS_USA_NtfNotificationThread(){

	USA_TRACE_ENTER();

	USA_TRACE_LEAVE();

}

int ACS_USA_NtfNotificationThread::open()
{
	USA_TRACE_ENTER();
	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		traNTF.traceAndLogFmt(ERROR, "%s() - Failed to start ACS_USA_NtfNotificationThread svc thread.", __func__);
		USA_TRACE_LEAVE();
		return -1;
	}

	thread_sleep = false;
	USA_TRACE_LEAVE();
	return 0;
}

int ACS_USA_NtfNotificationThread::svc()
{
	int timeout = 300;
	ACE_Message_Block* mb = NULL;
	bool running = true;
	SaAisErrorT error;
	saNotificationFilterAllocationParamsT notificationFilterAllocationParams = {0,0,0,0,0,0,0};
	SaNtfSubscriptionIdT subscriptionId = 1;

	traNTF.traceAndLogFmt(DEBUG, "ACS_USA_NtfNotificationThread : HOSTNAME = <%s>, HOSTID = <%s> !",(!ntfConstants::ap_node_hostname.empty() ? ntfConstants::ap_node_hostname.c_str() : "[UNKNOWN]"), (!ntfConstants::ap_node_id.empty() ? ntfConstants::ap_node_id.c_str() : "[UNKNOWN]"));

	// connect to CoreMW Ntf service and register callbacks
	while ((error = saNtfInitialize(&ntfHandle, &ntfCallbacks, &version)) != SA_AIS_OK) {
		traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread-%s() : saNtfInitialize failed - %s : retrying", __func__,error_output(error));
		::sleep(1);
	}

	while ((error = saNtfSelectionObjectGet(ntfHandle, &selObj)) != SA_AIS_OK) {
		traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread-%s() : saNtfSelectionObjectGet failed - %s : retrying", __func__,error_output(error));
		::sleep(1);
	}

	while ((error = subscribeForNotifications(&notificationFilterAllocationParams, subscriptionId)) != SA_AIS_OK) {
		traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread-%s() : subscribeForNotifications failed - %s : retrying", __func__,error_output(error));
		::sleep(1);
	}

	while (running){
		error = waitForNotifications(ntfHandle, selObj, timeout);
      		if (SA_AIS_OK != error) {
			traNTF.traceAndLogFmt(ERROR, "ACS_USA_NtfNotificationThread-%s() : waitForNotifications failed - %s",__func__,error_output(error));
       		}	

		try {
			if (this->getq(mb) < 0){
				traNTF.traceAndLogFmt(ERROR, "%s() -getq() failed", __func__);
				break;
			}

			// Check msg type

			switch(mb->msg_type()){

				case NTF_TIMEOUT:
					traNTF.traceAndLogFmt(INFO, "INFO - NTF_TIMEOUT received");
					mb->release();
					break;

				case NTF_CLOSE:
					traNTF.traceAndLogFmt(INFO, "INFO - NTF_CLOSE received");
					mb->release();
					running=false;
					break;

				default:
					traNTF.traceAndLogFmt(INFO, "WARNING - NTF not handled message received: %i", mb->msg_type());
					mb->release();
					running=false;
					break;
			} // end of switch
		}// end of try

		catch(...) {
			traNTF.traceAndLogFmt(ERROR, "ERROR: EXCEPTION!");
		}
	}// end of while
	
	return error;
}

int ACS_USA_NtfNotificationThread::close(u_long) {

	USA_TRACE_ENTER();
	SaAisErrorT error;
	SaNtfSubscriptionIdT subscriptionId = 1;

	error = saNtfNotificationUnsubscribe(subscriptionId);
	if (SA_AIS_OK != error) {
		traNTF.traceAndLogFmt(ERROR, "saNtfNotificationUnsubscribe failed - %s", error_output(error));
	}

	error = saNtfFinalize(ntfHandle);
	if (SA_AIS_OK != error) {
		traNTF.traceAndLogFmt(ERROR, "saNtfFinalize failed - %s", error_output(error));
	}

	// check that we're really shutting down.
	// ACS_USA_Adm::close' sets it to 'true
	if (!m_global_instance->shutdown_ordered()) {
		traNTF.traceAndLogFmt(ERROR, "%s(u_long) Abnormal shutdown of NTF subscription thread", __func__);
		USA_TRACE_1("(u_long) Abnormal shutdown of NTF subscription thread");
		exit(EXIT_FAILURE);
	}

	USA_TRACE_LEAVE();
	return 0;
}
int ACS_USA_NtfNotificationThread::close(){

	USA_TRACE_ENTER();
	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN( mb, ACE_Message_Block());
	thread_sleep = true;
	int rCode=0;
	syslog(LOG_INFO, "ACS_USA_NtfNotificationThread::%s() invoked", __func__);
	if (0 == mb) {
		traNTF.traceAndLogFmt(INFO, "%s() Failed to create mb object", __func__);
		USA_TRACE_1("Failed to create mb object");
		rCode= -1;
	}
	if (rCode != -1){
		mb->msg_type(NTF_CLOSE);
		if (this->putq(mb) < 0){
			traNTF.traceAndLogFmt(ERROR, "%s() Fail to post NTF_CLOSE to ourself", __func__);
			USA_TRACE_1("Fail to post NTF_CLOSE to ourself");
			mb->release();
			rCode=-1;
		}
	}
	USA_TRACE_LEAVE();
	return rCode;
}
