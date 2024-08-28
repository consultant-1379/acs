/*===================================================================
 *
 *  @file   "acs_usa_evtConsumer.cpp"
 *
 *  @brief
 *
 *
 *  @version 1.0.0
 *
 *
 *  HISTORY
 *
 *
 *
 *
 *       PR           DATE      INITIALS    DESCRIPTION
 *--------------------------------------------------------------------
 *       N/A       DD/MM/YYYY     NS       Initial Release
 *       XFABPAG   05/05/2014     FP       Interface name(s) for USAFM_ETHERNET_BONDING_FAILED generalized
 *       XGIACRI   22/05/2014     FP       USAFM_ETHERNET_BONDING_FAILED disabled for APG CABLE LESS
 *       XQUYDAO   06/06/2014     QD       Fixed alarm raising & ceasing handling for USAFM_ETHERNET_BONDING_FAILED
 *       XQUYDAO   12/06/2014     QD       Updated to let USAFM re-sends alarm after reboot
 *       XQUYDAO   23/06/2014     QD       Updated to save alarmList to file in every change
 *       XQUYDAO   19/08/2014     QD       Updated to subscribe event type _2 due to the change of COM 4.0
 *       XQUYDAO   19/09/2014     QD       Clear the internal alarm list at the startup due to cluster reboot
 *       XNAZBEG   11/11/2016     NB	   Fixed the TRs HU24295 and HU21450 	
 *       ZBHEGNA   15/11/2019     GB       Handled multiple CERT alarms and CERT alarm during upgrade
 *       TEISMA, TEIMON 29/01/2020         Trusted Certificate Expiry alarm handling APG 3.9
 *       XDAKANJ   13/11/2020              Handled Disk Replication,NTP alarms and isClusterRebootInProgress()check in APG 4.1
 *					   for HY37852,HX91388 and HY69443
 *       XCSRPAD   18/06/2021              Alarm for Log Streaming 
 *==================================================================== */

/*====================================================================
 *                           DIRECTIVE DECLARATION SECTION
 * =================================================================== */
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <syslog.h>
#include <cstdio>
#include <cstdlib>
#include <ComMgmtSpiServiceIdentities_1.h>
#include <ComMwSpiServiceIdentities_1.h>
#include <ComOamSpiServiceIdentities_1.h>
#include <boost/filesystem.hpp>

#include "acs_prc_api.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

#include "acs_usa_evtConsumer.h"
#include "acs_apgcc_omhandler.h"

#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/foreach.hpp> 
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <saAis.h>
#include <saClm.h>

#include <saNtf.h>
#include <saAmf.h>
#include <saSmf.h>

#include <boost/lexical_cast.hpp>


bool is_swm_2_0_fm = true;
bool isSWM20();
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";

static SaVersionT immVersion = { 'A', 2, 1 };

acs_usa_evtConsumer acs_usa_evtConsumer::_instance;

extern "C" ComReturnT usaFMStart(ComStateChangeReasonT reason)
{
	(void)reason;
	syslog(LOG_INFO, "ACS_USAFM: usaFMStart");
	return acs_usa_evtConsumer::getInstance().start();
}

extern "C" ComReturnT usaFMStop(ComStateChangeReasonT reason)
{
	(void)reason;
	syslog(LOG_INFO, "ACS_USAFM: usaFMStop");
	return acs_usa_evtConsumer::getInstance().stop();
}

extern "C" ComReturnT comLCMinit( struct ComMgmtSpiInterfacePortalAccessor* accessor,
		const char* config)
{
	syslog(LOG_INFO, "ACS_USAFM: comLCMinit");	
	return acs_usa_evtConsumer::getInstance().init(accessor, config);
}

extern "C" ComReturnT comLCMterminate()
{
	syslog(LOG_INFO, "ACS_USAFM: comLCMterminate");	
	return acs_usa_evtConsumer::getInstance().terminate();
}

extern "C" ComReturnT  usaFMNotify(ComOamSpiEventConsumerHandleT handle, const char* eventType, ComNameValuePairT** filter, void* value)
{
	return acs_usa_evtConsumer::getInstance().notify(handle, eventType, filter, value);
}

acs_usa_evtConsumer::acs_usa_evtConsumer() : _eventRouter(0), _portal(0)
{
	log.Open("USA");
	_consumerHandle=0;

	_ifArray[0]=0;

	_depArray[0] = (ComMgmtSpiInterface_1T*)&ComMwSpiLog_1Id;
	_depArray[1] = (ComMgmtSpiInterface_1T*)&ComMwSpiTrace_1Id;
	_depArray[2] = (ComMgmtSpiInterface_1T*)&ComOamSpiEventService_1Id;
	_depArray[3] = 0;

	_component.base.componentName = "ACS_USAFM";
	_component.base.interfaceName=  ComMgmtSpiComponentInterfaceName_1;
	_component.base.interfaceVersion= ComMgmtSpiComponentInterfaceVersion_1;

	_component.interfaceArray = _ifArray;
	_component.dependencyArray = _depArray;
	_component.start = &usaFMStart;
	_component.stop = &usaFMStop;

	_fmNotificationFilter.name = ComOamApiFmNotificationEventType_2;
	_fmNotificationFilter.value = 0;

	_fmNotificationFilters[0] = &_fmNotificationFilter;
	_fmNotificationFilters[1] = 0;
	_eventConsumer.notify=&usaFMNotify;

	mThisNodeId = 0;
	mPeerNodeId = 0;
	severity = MafOamSpiNotificationFmSeverityCleared;


}

acs_usa_evtConsumer::~acs_usa_evtConsumer()
{
	// empty
}

ComReturnT acs_usa_evtConsumer::init(ComMgmtSpiInterfacePortalAccessorT* accessor, const char* config)
{
	(void)config;

	mThisNodeId = getNodeId("/etc/cluster/nodes/this/id");
	mPeerNodeId = getNodeId("/etc/cluster/nodes/peer/id");

	fetchClsAttributes("SaAmfNode");
	fetchClsAttributes("SaAmfSIAssignment");

	_portal = (ComMgmtSpiInterfacePortal_1T*)accessor->getPortal("1");
	is_swm_2_0_fm = isSWM20();

	return _portal->registerComponent(&_component);
}

ComReturnT acs_usa_evtConsumer::terminate()
{
	return _portal->unregisterComponent(&_component);
}

acs_usa_evtConsumer& acs_usa_evtConsumer::getInstance()
{
	return _instance;
}

ComReturnT acs_usa_evtConsumer::start()
{
	syslog(LOG_INFO, "ACS_USAFM: Starting USAFM component");
	log.Write("ACS_USAFM: Starting USAFM component", LOG_LEVEL_INFO);
	ComMgmtSpiInterface_1T* interface = 0;
	ComReturnT retVal = _portal->getInterface((ComMgmtSpiInterface_1T)ComOamSpiEventService_1Id, &interface);
	if (retVal != ComOk) {
		syslog(LOG_INFO, "ACS_USAFM:  eventRouter 'getInterface' Failure");
		log.Write("ACS_USAFM:  eventRouter 'getInterface' Failure", LOG_LEVEL_ERROR);
	}
	_eventRouter = (ComOamSpiEventRouter_1T*)interface;
	retVal =_eventRouter->registerConsumer(&_eventConsumer, &_consumerHandle);

	if (retVal != ComOk)
	{   
		syslog(LOG_INFO, "ACS_USAFM:  registerConsumer Failure");
		log.Write("ACS_USAFM:  registerConsumer Failure", LOG_LEVEL_ERROR);
	}

	syslog(LOG_INFO, "ACS_USAFM: subscribe for ComOamApiFmNotificationEventType_2");
	log.Write("ACS_USAFM: subscribe for ComOamApiFmNotificationEventType_2", LOG_LEVEL_INFO);
	retVal=_eventRouter->addSubscription(_consumerHandle, ComOamApiFmNotificationEventType_2, _fmNotificationFilters);
	if (retVal != ComOk)
	{   
		syslog(LOG_INFO, "ACS_USAFM: addSubscrtption Failure");
		log.Write("ACS_USAFM: addSubscrtption Failure", LOG_LEVEL_ERROR);
	}

	// Check if USAFM starts due to cluster reboot
	// If yes, clear the alarm list
	if (isClusterRebootInProgress())
	{
		syslog(LOG_INFO, "ACS_USAFM: Clearing alarm list due to cluster reboot just happened");
		log.Write("ACS_USAFM: Clearing alarm list due to cluster reboot just happened", LOG_LEVEL_ERROR);
		alarmList.clear();
		saveAlarmList();
	}
	else
	{
		loadAlarmList();
	}

	return ComOk;
}

ComReturnT acs_usa_evtConsumer::stop()
{
	syslog(LOG_INFO, "ACS_USAFM: Stopping USAFM Component");
	log.Write("ACS_USAFM: Stopping USAFM Component", LOG_LEVEL_INFO);
	bool usafmStopping = true;
	saveAlarmList(usafmStopping);

	return ComOk;
}

ComReturnT acs_usa_evtConsumer::notify(ComOamSpiEventConsumerHandleT handle, const char* eventType, ComNameValuePairT** filter, void* value)
{
	(void)eventType;
	(void)filter;
	(void)handle;
	std::string objOfSrcStr,objOfSrcDn;	
	if (std::strcmp(eventType, ComOamApiFmNotificationEventType_2) == 0)
	{
		ComOamApiFmNotificationEventValue_2T* theValue = static_cast<ComOamApiFmNotificationEventValue_2T*>(value);		
		AlarmInfo alarm;		
		char problemData[1024]={'\0'};
		char problemText[1024]={'\0'};
		char curDate[20]={'\0'};
		char curTimeStr[20]={'\0'};
		char objectOfRef[60]={'\0'};		//Increasing it to hold DN
		bool sendEvent = true;
		bool isUpdInProgress = isSwUpdateInProgress();

		alarm.manualCease = true;
		alarm.processName = EVENT_CONSUMER_PROC_NAME;
		alarm.severity = getSeverityStr(theValue->notification->severity);		
		alarm.objClassOfReference = ACS_USAFM_objOfClassRef;
		alarm.originalEventTime = theValue->originalEventTime;

#if 0
		{ // Debug block, set the "#if" above to 1 in order to see the alarm content
			char buffer[512] = {0}; buffer[sizeof(buffer) - 1] = 0;
			::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Severity \"%s\" (%d), dn \"%s\", additionalText \"%s\", probableCause %d, specificProblem \"%s\", originalAdditionalText \"%s\"",
					alarm.severity.c_str(), theValue->notification->severity, theValue->notification->dn,
					theValue->notification->additionalText, theValue->probableCause, theValue->specificProblem, theValue->originalAdditionalText);
			log.Write(buffer, LOG_LEVEL_INFO);
		}
#endif

		if (theValue->notification->majorType == 193) {
			syslog(LOG_INFO, "ACS_USAFM: 193 %s , sev=%d", theValue->notification->additionalText, theValue->notification->severity );
			switch (theValue->notification->minorType) {
			case USAFM_NODE_UNAVALABLE:  /* COM SA CLM Cluster Node Unavailable */
				alarm.specificProblem = ACS_USAFM_IntimatePRCHANodeUnavailable;
				if ((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor)) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Cluster Node Unavailable, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				if (theValue->notification->severity == MafOamSpiNotificationFmSeverityCleared) {
					sendEvent = false;
				}
				else {
					alarm.severity = ACS_USAFM_Severity_EVENT;
					alarm.probableCause = ACS_USAFM_APInternalFault;
					alarm.objectOfReference = ACS_USAFM_objOfRefMiddleware;
					sprintf(problemData, "%s,%s", theValue->notification->dn, theValue->notification->additionalText);
					alarm.problemData = problemData;
					alarm.problemText = "PROBLEM\nHA NODE UNAVAILABLE";
				}
				break;
			case USAFM_TIME_SYNC_FAULT: /*    */
				if (isVirtual()){
					alarm.specificProblem = ACS_USAFM_TimeSynchronizationFault;
					//alarm.originalEventTime = theValue->originalEventTime;
					/*					char buffer[512]= {0};
						sprintf(buffer,"ACS_USAFM: Event recieved with severity==%d,additionaltext==%s,originaladditionaltext=%s,seq no==%ld,eventtype==%d,probablecause==%d,specificproblem==%s,originalseqno==%ld,originalseverity==%ld,originaleventtime==%ld",theValue->notification->severity,theValue->notification->additionalText,theValue->originalAdditionalText,theValue->seqNo,theValue->eventType,theValue->probableCause,theValue->specificProblem,theValue->originalSeqNo,theValue->originalSeverity,theValue->originalEventTime);
						log.Write(buffer, LOG_LEVEL_INFO);	 */
					acs_aeh_evreport objEvReport;
					ACS_AEH_ReturnType retValue;

					if ((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
							(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor) &&
							(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor) &&
							(theValue->notification->severity != MafOamSpiNotificationFmSeverityCritical))
					{
						char buffer[256]= {0};
						sprintf(buffer, "ACS_USAFM: Time Synchronization Fault, Severity=%d Not supported", theValue->notification->severity);
						log.Write(buffer, LOG_LEVEL_INFO);
					}
					if (MafOamSpiNotificationFmSeverityCritical == theValue->notification->severity)
					{
						char buffer[256]={0};
						alarm.severity = ACS_USAFM_Severity_A1;
						sprintf(buffer,theValue->notification->additionalText);
						string str(buffer);
						std::size_t index1 = str.find("off by");
						str.erase(0,index1+6);
						std::size_t index2 = str.find("seconds");
						str.erase(index2,str.length()-index2);
						snprintf(problemText,sizeof(problemText),NTP_TIME_NOT_SYNC,str.c_str());
						alarm.problemText = problemText;
					}
					else if (MafOamSpiNotificationFmSeverityMajor == theValue->notification->severity)
					{
						char buffer[256]={0};
						alarm.severity = ACS_USAFM_Severity_A2;
						sprintf(buffer,theValue->notification->additionalText);
						string str(buffer);
						std::size_t index1 = str.find(":");
						str.erase(0,index1+2);
						std::size_t index2 = str.find("(");
						str.erase(index2,str.length()-index2);
						snprintf(problemText,sizeof(problemText),NTP_SERVERS_UNSTABLE,str.c_str());
						alarm.problemText = problemText;
					}
					else if (MafOamSpiNotificationFmSeverityMinor == theValue->notification->severity)
					{
						char buffer[256]={0};
						alarm.severity = ACS_USAFM_Severity_A3;
						sprintf(buffer,theValue->notification->additionalText);
						string str(buffer);
						std::size_t index1 = str.find(":");
						str.erase(0,index1+2);
						std::size_t index2 = str.find("(");
						str.erase(index2,str.length()-index2);
						snprintf(problemText,sizeof(problemText),NTP_SERVERS_UNSTABLE,str.c_str());
						alarm.problemText = problemText;
					}
					else if (MafOamSpiNotificationFmSeverityCleared == theValue->notification->severity) {
						alarm.severity = ACS_USAFM_Severity_CEASING;
					}
					alarm.probableCause = ACS_USAFM_NTPSyncFault;
					alarm.objectOfReference = ACS_USAFM_objOfRefOS;
					sprintf(problemData, "%s", theValue->notification->additionalText);
					alarm.problemData = problemData;
					{
						char hostName[256] = {0};
						if (!getToken(hostName, theValue->notification->dn, "HostName=", ','))
						{
							char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
							std::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM:HostName not found in notification->dn: \"%s\"", theValue->notification->dn);
							syslog(LOG_ERR, buffer);
							log.Write(buffer, LOG_LEVEL_ERROR);
							sendEvent = false;
							break; // Error in Host Name
						}
						alarm.node = (strcmp(hostName, "SC-2-1") == 0) ? 1 : 2;
					}
					// Event for passive node, no need to raise/cease alarm
					if (alarm.node == mPeerNodeId)
					{
						if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared)
						{
							log.Write("ACS_USAFM: Passive raise event", LOG_LEVEL_INFO);
							eraseFromAlarmList(alarm); //Erase the previous alarm present in the xml file
							saveAlarmList();
							addToAlarmList(alarm);
						}
						else
						{
							log.Write("ACS_USAFM: Passive cease event", LOG_LEVEL_INFO);
							eraseFromAlarmList(alarm);
						}
						saveAlarmList();
						sendEvent = false;
					} else {
						if(MafOamSpiNotificationFmSeverityCleared != theValue->notification->severity) {
							retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, getSeverityStr(MafOamSpiNotificationFmSeverityCleared).c_str(),alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(),alarm.problemData.c_str(), alarm.problemText.c_str(), alarm.manualCease);
							if (retValue != ACS_AEH_ok)
							{
								syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
								log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
							}

							eraseFromAlarmList(alarm); //Erase the previous alarm present in the xml file
							saveAlarmList();
						}
					}
				}
				else {
					char buffer[1024]; buffer[sizeof(buffer) - 1] = 0;
					syslog(LOG_INFO, "ACS_USAFM: Unhandled Event '%s'", theValue->specificProblem);
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Unhandled Event '%s'", theValue->specificProblem);
					log.Write(buffer, LOG_LEVEL_INFO);
					sendEvent = false;
				}
				break;
			case USAFM_DISK_REPLICATION_COMM_FAILED: /* LOTC Disk Replication communication */
				alarm.specificProblem = ACS_USAFM_DiskReplication;
				if ((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor))
				{
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Disk Replication Communication, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_APFault;
				alarm.objectOfReference = ACS_USAFM_objOfRefOS;
				sprintf(problemData, "%s,%s", theValue->notification->dn, theValue->notification->additionalText);
				alarm.problemData = problemData;
				alarm.problemText = "PROBLEM\nDISK REPLICATION";
				{
					char hostName[256] = {0};
					if (!getToken(hostName, theValue->notification->dn, "HostName=", ','))
					{
						char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
						::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM:HostName not found in notification->dn: \"%s\"", theValue->notification->dn);
						syslog(LOG_ERR, buffer);
						log.Write(buffer, LOG_LEVEL_ERROR);
						sendEvent = false;
						break; // Error in Host Name
					}
					alarm.node = (strcmp(hostName, "SC-2-1") == 0) ? 1 : 2;
				}

				// Event for passive node, no need to raise/cease alarm
				if (alarm.node == mPeerNodeId)
				{
					if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared)
					{
						addToAlarmList(alarm);
					}
					else
					{
						removeFromAlarmList(alarm);
					}
					saveAlarmList();
					sendEvent = false;
				}
				break;


			case USAFM_DISK_REPLICATION_INCONSISTANT: /* LOTC Disk Replication Consistency */
				alarm.specificProblem = ACS_USAFM_DiskReplication;
				if ((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor))
				{
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Disk Replication inconsistent, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_APFault;
				alarm.objectOfReference = ACS_USAFM_objOfRefOS;
				sprintf(problemData, "%s,%s", theValue->notification->dn, theValue->notification->additionalText);
				alarm.problemData = problemData;
				alarm.problemText = "PROBLEM\nDISK REPLICATION";
				{
					char hostName[256] = {0};
					if (!getToken(hostName, theValue->notification->dn, "HostName=", ','))
					{
						char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
						::snprintf(buffer, sizeof(buffer) - 1, "HostName not found in notification->dn: \"%s\"", theValue->notification->dn);
						syslog(LOG_ERR, buffer);
						log.Write(buffer, LOG_LEVEL_ERROR);
						sendEvent = false;
						break; // Error in Host Name
					}
					alarm.node = (strcmp(hostName, "SC-2-1") == 0) ? 1 : 2;
				}

				// Event for passive node, no need to raise/cease alarm
				if (alarm.node == mPeerNodeId)
				{
					if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared)
					{
						addToAlarmList(alarm);
					}
					else
					{
						removeFromAlarmList(alarm);
					}
					saveAlarmList();
					sendEvent = false;
				}
				break;

				//START CERT MGMT ALARMS only RDN is displayed.

			case USAFM_VALIDCERT_NOT_AVAILABLE: /* VAlid Certificate Not Available */
				alarm.specificProblem = ACS_USAFM_ValidCertUnavailable;
				if((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityCritical)) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Valid Certificate Not Available, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_APCertMgmtFault;
				objOfSrcStr = theValue->notification->dn;
				objOfSrcDn = objOfSrcStr.substr(objOfSrcStr.find_last_of(",")+1);
				strncpy(objectOfRef,objOfSrcDn.c_str(),strlen(objOfSrcDn.c_str())+1);
				alarm.objectOfReference = "certificate Unavailable";
				sprintf(problemData, "%s", theValue->notification->additionalText);
				alarm.problemData = problemData;

				sprintf(problemText, "PROBLEM\nCERTIFICATE EXPIRED, REVOKED OR NOT AVAILABLE\n\nSOURCE\n%s",theValue->notification->dn);

				alarm.problemText = problemText;
				log.Write("CERTM: AP cERT FAULT: valid cert not avaialble",LOG_LEVEL_WARN);
				break;

			case USAFM_CERT_ABOUTTO_EXPIRE: /* Certificate about to expire */
				alarm.specificProblem = ACS_USAFM_ValidCertUnavailable;
				if((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityWarning) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityCritical) ) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Certificate about to expire, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_APCertMgmtFault;
				objOfSrcStr = theValue->notification->dn;
				objOfSrcDn = objOfSrcStr.substr(objOfSrcStr.find_last_of(",")+1);
				strncpy(objectOfRef,objOfSrcDn.c_str(),strlen(objOfSrcDn.c_str())+1);
				alarm.objectOfReference = "Certificate About To Expire";
				sprintf(problemData, "%s", theValue->notification->additionalText);
				alarm.problemData = problemData;
				sprintf(problemText, "PROBLEM\nCERTIFICATE IS ABOUT TO EXPIRE\n\nSOURCE\n%s",theValue->notification->dn);
				alarm.problemText = problemText;
				log.Write("CERTM: AP cERT FAULT: certificate about to expire",LOG_LEVEL_WARN);
				break;

			case USAFM_TRUSTED_CERT_ABOUTTO_EXPIRE: /* Trusted Certificate about to expire */
				alarm.specificProblem = ACS_USAFM_CertAboutToExpire;  /* verify if it is not ACS_USAFM_ValidCertUnavailable the one to be used */
				if((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityWarning) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityCritical) ) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Certificate about to expire, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_APTrustedCertMgmtFault; /* New alarm for Trusted Certificate    */
				objOfSrcStr = theValue->notification->dn;
				objOfSrcDn = objOfSrcStr.substr(objOfSrcStr.find_last_of(",")+1);
				strncpy(objectOfRef,objOfSrcDn.c_str(),strlen(objOfSrcDn.c_str())+1);
				alarm.objectOfReference = "Trusted Certificate About To Expire"; /* Trusted Certificate    */
				sprintf(problemData, "%s", theValue->notification->additionalText);
				alarm.problemData = problemData;
				sprintf(problemText, "PROBLEM\nTRUSTED CERTIFICATE IS ABOUT TO EXPIRE\n\nSOURCE\n%s",theValue->notification->dn); /* Trusted Certificate    */
				alarm.problemText = problemText;
				log.Write("CERTM: AP cERT FAULT: Trusted certificate about to expire",LOG_LEVEL_WARN); /* Trusted Certificate    */
				break;


			case USAFM_AUTO_ENROLL_FAILED: /* Auto Enroll Failed */
				alarm.specificProblem = ACS_USAFM_ValidCertUnavailable;
				if((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityWarning)) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Auto Enrollment of certificate Failed, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_APCertMgmtFault;
				objOfSrcStr = theValue->notification->dn;
				objOfSrcDn = objOfSrcStr.substr(objOfSrcStr.find_last_of(",")+1);
				strncpy(objectOfRef,objOfSrcDn.c_str(),strlen(objOfSrcDn.c_str())+1);

				alarm.objectOfReference = "Auto Enroll Failed";
				sprintf(problemData, "%s", theValue->notification->additionalText);
				alarm.problemData = problemData;
				sprintf(problemText, "PROBLEM\nONLINE CERTIFICATE ENROLLMENT OR RENEWAL FAILED\n\nSOURCE\n%s",theValue->notification->dn);
				alarm.problemText = problemText;
				log.Write("CERTM: AP cERT FAULT: online cert enrollment or renewal failed",LOG_LEVEL_WARN);

				break;

			case USAFM_AUTH_FAILURE_LIMIT_REACHED: /* Authentication Failure Limit Reached */
				alarm.specificProblem = ACS_USAFM_AuthenticationFailureLimitReached;
				if((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityIndeterminate)) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Authentication Failure Limit Reached, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				alarm.probableCause = ACS_USAFM_AuthenticationFailure;
				alarm.manualCease = false;
				if(theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared ) {
					alarm.severity = std::string("O1");
				}
				alarm.objectOfReference = "User Authentication Failure Limit Reached";
				sprintf(problemText, "CAUSE\nMANY FAILED ADMINISTRATOR ACCOUNT LOGON ATTEMPTS");
				alarm.problemText = problemText;
				log.Write("AP AuthFail: Several consecutive failed logon attempts for the Administrator account",LOG_LEVEL_WARN);

				break;

				//END CERT MGMT ALARMS



			case USAFM_ETHERNET_BONDING_FAILED: /* LDE Ethernet Bonding */
			{
				int apgOamAccVal = getApgOamAccess();

				if ((1 != check_teaming_status()) || (1 == apgOamAccVal) || (2 == apgOamAccVal))
				{
					sendEvent = false;
					break; // Error or teaming not active
				}

				char hostName[256] = {0};
				if (!getToken(hostName, theValue->notification->dn, "HostName=", ','))
				{
					char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: HostName not found in notification->dn: \"%s\"", theValue->notification->dn);
					syslog(LOG_ERR, buffer);
					log.Write(buffer, LOG_LEVEL_ERROR);
					sendEvent = false;
					break; // Error in Host Name
				}

				if (strcmp(hostName, "SC-2-1") == 0)
				{
					alarm.specificProblem = ACS_USAFM_EthernetBondingFailed1;
					alarm.node = 1;
				}
				else
				{
					alarm.specificProblem = ACS_USAFM_EthernetBondingFailed2;
					alarm.node = 2;
				}

				alarm.probableCause = ACS_USAFM_APRelPublicInterfaceFault;
				alarm.objectOfReference = ACS_USAFM_objOfRefOS;

#if 1 // To enable or disable the following check, that is to avoid sending alarm coming from the passive node					
				if (alarm.node == mPeerNodeId)
				{
					char buffer[256] = {0}; buffer[sizeof(buffer) - 1] = 0;
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: This comes from passive node %s (notification->dn: \"%s\")", hostName, theValue->notification->dn);
					syslog(LOG_INFO, buffer);
					log.Write(buffer, LOG_LEVEL_INFO);

					// Add RE alarm on passive node to list
					if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared)
					{
						// RE alarm is always raised as A2
						alarm.severity = ACS_USAFM_Severity_A2;
						// Remove the existing first
						removeFromAlarmList(alarm);

						alarm.problemData = theValue->notification->additionalText;

						unsigned short slot, ap;
						char problem[256] = {0};
						if (!getBondingProblemDescription(problem, theValue->notification->additionalText))
						{
							char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
							::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Interface(s) not found in additionalText: \"%s\"", theValue->notification->additionalText);
							syslog(LOG_ERR, buffer);
							log.Write(buffer, LOG_LEVEL_ERROR);
						}
						if (getSlotAp(hostName, slot, ap))
						{
							::snprintf(problemText, sizeof(problemData) - 1, "PROBLEM               SLOTNUMBER\n%-22s%d", problem, slot);
						}
						else
						{
							::snprintf(problemText, sizeof(problemData) - 1, "PROBLEM               SLOTNUMBER\n%-22s%s", problem, "-");
						}
						alarm.problemText = problemText;
						addToAlarmList(alarm);
					}
					else // Remove RE alarm on passive node from list
					{
						removeFromAlarmList(alarm);
					}

					saveAlarmList();

					sendEvent = false;
					break; // If it is not the active node nothing to do
				}
#endif
				// When Severity is CEASING, additional text is "-", so check the bond1 in originalAdditionalText instead
				if ((NULL == strstr(theValue->notification->additionalText, " bond1 ")) &&
						(NULL == strstr(theValue->originalAdditionalText, " bond1 ")))
				{
					char buffer[256] = {0}; buffer[sizeof(buffer) - 1] = 0;
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: LDE Ethernet Bonding, not certainly related to bond1: dn = \"%s\" additionalText = \"%s\"",
							theValue->notification->dn, theValue->notification->additionalText);
					syslog(LOG_INFO, buffer);
					log.Write(buffer, LOG_LEVEL_INFO);
					sendEvent = false;
					break; // If it is not from bond1 nothing to do
				}

				char problem[256] = {0};
				if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared)
				{
					// This alarm is always raised as A2
					alarm.severity = ACS_USAFM_Severity_A2;

					// Only convert to problem text when severity is not CEASING (because additionalText is "-")
					if (!getBondingProblemDescription(problem, theValue->notification->additionalText))
					{
						char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
						::snprintf(buffer, sizeof(buffer) - 1, "Interface(s) not found in additionalText: \"%s\"", theValue->notification->additionalText);
						syslog(LOG_ERR, buffer);
						log.Write(buffer, LOG_LEVEL_ERROR);
						sendEvent = false;
						break; // Error in Interface Name
					}
				}

				bool severityError = false;
				if((theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityCritical) && 
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared))
				{
					char buffer[128]; buffer[sizeof(buffer) - 1] = 0;
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: LDE Ethernet Bonding, Severity=%d Not supported", theValue->notification->severity);
					syslog(LOG_INFO, buffer);
					log.Write(buffer, LOG_LEVEL_INFO);

					severityError = true;
				}
				unsigned short slot, ap;
				::strncpy(problemData, theValue->notification->additionalText, sizeof(problemData) - 1);
				alarm.problemData = problemData;
				if (getSlotAp(hostName, slot, ap))
				{
					::snprintf(problemText, sizeof(problemData) - 1, "PROBLEM               SLOTNUMBER\n%-22s%d", problem, slot);
				}
				else
				{
					::snprintf(problemText, sizeof(problemData) - 1, "PROBLEM               SLOTNUMBER\n%-22s%s", problem, "-");
				}
				alarm.problemText = problemText;

				if (!severityError &&
						(MafOamSpiNotificationFmSeverityCleared != severity) && // There were a previous alarm
						(MafOamSpiNotificationFmSeverityCleared != theValue->notification->severity))
				{
					acs_aeh_evreport objEvReport;
					ACS_AEH_ReturnType retValue;

					// Cease the previous raised alarm
					retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, getSeverityStr(MafOamSpiNotificationFmSeverityCleared).c_str(),
							alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(), "-", "-", alarm.manualCease);
					// Remove it from list
					removeFromAlarmList(alarm);

					if (retValue != ACS_AEH_ok)
					{
						syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
						log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
					}
				}

				severity = theValue->notification->severity;
				break;
			}
			case USAFM_LOG_STREAMING_FAILED: /* LOG Streaming Failure */
			{
				syslog(LOG_INFO, "Log streaming failed case , sev=%d, %s",  theValue->notification->severity, theValue->notification->additionalText );				
				ACS_AEH_ReturnType retValue;
				acs_aeh_evreport objEvReport;
				char remoteServerId[256] = {0};
				char hostName[256] = {0};
				bool cease = false;
				if(theValue->notification->severity == MafOamSpiNotificationFmSeverityCleared)
					cease = true;
				if (getLogmDescription(hostName, remoteServerId, theValue->notification->additionalText, theValue->notification->dn, cease))
				{// If additional text is  "remote log server is not reachable/is reachable" /* only this condition is considered for this alarm*/
					syslog(LOG_INFO, "ACS_USAFM: 'get logmdesc succ %s",theValue->notification->dn);					
					if((theValue->notification->severity != MafOamSpiNotificationFmSeverityWarning)&& 
							(theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared))
					{
						char buffer[256];
						sprintf(buffer, "ACS_USAFM: LOG Streaming failed, Severity=%d Not supported", theValue->notification->severity);
						log.Write(buffer, LOG_LEVEL_INFO);
						break;
					}					
					alarm.probableCause = ACS_USAFM_APLogMgmtFault;
					objOfSrcStr = theValue->notification->dn;
					objOfSrcDn = objOfSrcStr.substr(objOfSrcStr.find_last_of(",")+1);
					strncpy(objectOfRef,objOfSrcDn.c_str(),strlen(objOfSrcDn.c_str())+1);
					alarm.objectOfReference = "Remote Log Server Is Not Reachable";
					//alarm.objectOfReference = "local log service is not active";
					sprintf(problemData, "%s", "Remote Log Server Is Not Reachable");
					alarm.problemData = problemData;
					sprintf(problemText, "PROBLEM\nREMOTE LOG SERVER IS NOT REACHABLE\n\nSOURCE\n%s",theValue->notification->dn);
					alarm.problemText = problemText;
					if(theValue->notification->severity == MafOamSpiNotificationFmSeverityWarning)
					{
						alarm.severity = ACS_USAFM_Severity_A1;
					}				
					if(strcmp(remoteServerId, "regular") == 0)
					{
						if(strcmp(hostName, "SC-2-1")== 0)
							alarm.specificProblem = ACS_USAFM_LogStream_Reg_1_Failed;
						else
							alarm.specificProblem = ACS_USAFM_LogStream_Reg_2_Failed;
					}
					else if(strcmp(remoteServerId, "security") == 0)
					{
						if(strcmp(hostName, "SC-2-1")== 0)
							alarm.specificProblem = ACS_USAFM_LogStream_Sec_1_Failed;
						else
							alarm.specificProblem = ACS_USAFM_LogStream_Sec_2_Failed;
					}
					if(theValue->notification->severity == MafOamSpiNotificationFmSeverityCleared)
					{
						for (AlarmList::iterator itr = alarmList.begin(); itr != alarmList.end();)
						{
							if ( itr->specificProblem == alarm.specificProblem)
							{
								sendEvent = true;
								syslog(LOG_INFO, "ACS_USAFM: sending ceasing event");
							break;
							}
							else
							{
								sendEvent = false;	
								itr++;							
							}
						}
					}
				}
				else
					sendEvent = false;
				break;
			}
			default:
			{
				char buffer[1024]; buffer[sizeof(buffer) - 1] = 0;
				syslog(LOG_INFO, "ACS_USAFM: Unhandled Event '%s'", theValue->specificProblem);
				::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Unhandled Event '%s'", theValue->specificProblem);
				log.Write(buffer, LOG_LEVEL_INFO);
				sendEvent = false;
				break;
			}
			}
		}
		else if (theValue->notification->majorType == 18568) {

			switch (theValue->notification->minorType) {

			case USAFM_COMP_INTANTIATE_FAILED:    /* Instantiation Of Component Failed */
				alarm.specificProblem = ACS_USAFM_InitiatePRCToAPReboot;
				if ((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor)) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Component Instantiate Failed, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				if (theValue->notification->severity == MafOamSpiNotificationFmSeverityCleared) {
					sendEvent = false;
				}
				else {
					alarm.severity = ACS_USAFM_Severity_EVENT;
					alarm.probableCause = ACS_USAFM_APInternalFault;
					alarm.objectOfReference = ACS_USAFM_objOfRefMiddleware;
					sprintf(problemData, "%s,%s", theValue->notification->dn, theValue->notification->additionalText);
					alarm.problemData = problemData;
					alarm.problemText = "PROBLEM\nAP PROCESS FAILED TO START";
				}
				break;

			case USAFM_COMP_CLEANUP_FAILED:    /* Clean Up Component Failed */

				alarm.specificProblem = ACS_USAFM_InitiatePRCToAPReboot;
				if ((theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMinor) &&
						(theValue->notification->severity != MafOamSpiNotificationFmSeverityMajor)) {
					char buffer[256];
					sprintf(buffer, "ACS_USAFM: Component Cleanup Failed, Severity=%d Not supported", theValue->notification->severity);
					log.Write(buffer, LOG_LEVEL_INFO);
				}
				if (theValue->notification->severity == MafOamSpiNotificationFmSeverityCleared) {
					sendEvent = false;
				}
				else {
					alarm.severity = ACS_USAFM_Severity_EVENT;
					alarm.probableCause = ACS_USAFM_APInternalFault;
					alarm.objectOfReference = ACS_USAFM_objOfRefMiddleware;
					sprintf(problemData, "%s,%s", theValue->notification->dn, theValue->notification->additionalText);
					alarm.problemData = problemData;
					alarm.problemText = "PROBLEM\nAP PROCESS CLEANUP FAILED";
				}
				break;

			case USAFM_AP_PRCESS_STOPPED:    /* COM SA, AMF SI Unassigned */
				if(false == isUpdInProgress) {
					alarm.specificProblem = ACS_USAFM_AmfSIUnassigned;
					if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared)
					{
						alarm.severity = ACS_USAFM_Severity_O1;
					}
					alarm.probableCause = ACS_USAFM_APFunctionNotAvailable;
					alarm.objectOfReference = ACS_USAFM_objOfRefMiddleware;					
					alarm.problemData = "-";
					{
						time_t curTime = 0;
						struct tm stTm;
						struct tm *ptm = NULL;
						char appName[50] = {'\0'};
						char *res = NULL;
						::time(&curTime);

						ptm = localtime_r(&curTime, &stTm);
						if (NULL != ptm) {
							sprintf(curDate, "%04d-%02d-%02d",stTm.tm_year+1900, stTm.tm_mon+1, stTm.tm_mday);
							sprintf(curTimeStr, "%02d:%02d:%02d",stTm.tm_hour, stTm.tm_min, stTm.tm_sec);
						}

						char buffer[256]={'\0'};
						sprintf(buffer, "ACS_USAFM: DN = %s", theValue->notification->dn);
						log.Write(buffer, LOG_LEVEL_INFO);
						char *substr =NULL;
						if ( is_swm_2_0_fm == true )
							substr = strstr(theValue->notification->dn, "ERIC-apg");
						else
							substr = strstr(theValue->notification->dn, "ERIC-APG");
						if (NULL != substr) {
							res = strcasestr(theValue->notification->dn, "SafSi="); //updated to address TR: HQ59821
							if (res){
								res += 6;								
								strcpy(appName, res);
							}

							sprintf(objectOfRef, "%s,%s", ACS_USAFM_objOfRefMiddleware, appName);
							alarm.objectOfReference = objectOfRef;
							sprintf(problemText, "%-16s %s\r\n%-16s %s\r\n\r\n%-16s %-14s %s\r\n%-16s %-14s %s\n", "RESOURCE GROUP", "PROCESS", "", appName, "CAUSE", "DATE", "TIME", "Manual stop", curDate, curTimeStr);
							alarm.problemText = problemText;
							alarm.node = 0; // treat as cluster alarm

							int tmpSuId = 0;
							getSuId(appName, tmpSuId);
							if ((tmpSuId > 0) && (tmpSuId != mThisNodeId) && (0 != strPeerNodeAmfDN.length())) { // alarm for passive node
								// Check if passive node is down/rebooting
								if (isPeerNodeDown(strPeerNodeAmfDN)) {
									// No need to raise alarm when the passive is rebooting
									if (theValue->notification->severity != MafOamSpiNotificationFmSeverityCleared) {										
										addToAlarmList(alarm);
										insertIntoFalseAlarmList(appName);
										sendEvent = false;
									}
								}
								else {
									if (theValue->notification->severity == MafOamSpiNotificationFmSeverityCleared) {
										if (removeFalseAlarm(appName)) {
											removeFromAlarmList(alarm);
											sendEvent = false;
										}
									}
								}

								saveAlarmList();								
							}
						}
						else {
							syslog(LOG_INFO, "ACS_USAFM: DN not belongs to ERIC-APG");
							log.Write("ACS_USAFM: DN not belongs to ERIC-APG", LOG_LEVEL_INFO);

							sendEvent = false;
						}
					}			
				}		
				break;
			default:

				char buffer[1024];
				sprintf(buffer, "ACS_USAFM: Unahadled Event '%s'", theValue->specificProblem);
				log.Write(buffer, LOG_LEVEL_INFO);
				sendEvent = false;
				break;
			}
		}
		else {

			sendEvent = false;
			char buffer[1024];
			sprintf(buffer, "ACS_USAFM: Unhandled Event, VendorID = %d, %s", theValue->notification->majorType, theValue->specificProblem);
			log.Write(buffer, LOG_LEVEL_INFO);
		}

		if (sendEvent == true) {
			if (false == isUpdInProgress || alarm.specificProblem == ACS_USAFM_ValidCertUnavailable || alarm.specificProblem == ACS_USAFM_LogStream_Reg_1_Failed ||
			     alarm.specificProblem == ACS_USAFM_LogStream_Reg_2_Failed || alarm.specificProblem == ACS_USAFM_LogStream_Sec_1_Failed  ||
				 alarm.specificProblem == ACS_USAFM_LogStream_Sec_2_Failed) {
				acs_aeh_evreport objEvReport;
				ACS_AEH_ReturnType retValue;				
				if(alarm.specificProblem == ACS_USAFM_TimeSynchronizationFault) {
					if(alarm.severity.compare(ACS_USAFM_Severity_CEASING) != 0) {
						retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(),
								alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(),
								alarm.problemData.c_str(), alarm.problemText.c_str(), alarm.manualCease);			
						if (retValue != ACS_AEH_ok)
						{
							syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
							log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
						} else {
							addToAlarmList(alarm);
							saveAlarmList();
						}
					}
					else {
						for (AlarmList::iterator it2 = alarmList.begin(); it2 != alarmList.end();)
						{
							if ((it2->originalEventTime == alarm.originalEventTime) && (it2->specificProblem == ACS_USAFM_TimeSynchronizationFault)&& (it2->node == alarm.node))
							{
								retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(),
										alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(),
										alarm.problemData.c_str(), alarm.problemText.c_str(), alarm.manualCease);
								if (retValue != ACS_AEH_ok)
								{
									syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
									log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
								} else {
									eraseFromAlarmList(*it2);
									saveAlarmList();
								}
								break;
							}
							else {
								it2++;
							}
						}	
					}
				} else {
					retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(), 
							alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(),
							alarm.problemData.c_str(), alarm.problemText.c_str(), alarm.manualCease);
					if (retValue != ACS_AEH_ok)
					{
						syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH result=%d, error_text=%s", retValue, objEvReport.getErrorText());
						log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
					}
					else
					{
						if (alarm.severity.compare(ACS_USAFM_Severity_EVENT) != 0)
						{
							// Remove the ceased alarm from list
							if (alarm.severity.compare(ACS_USAFM_Severity_CEASING) == 0)
							{
								removeFromAlarmList(alarm);
							}
							else // Add raised alarm to list
							{
								addToAlarmList(alarm);
							}

							saveAlarmList();
						}
					}
				}
			} else {
				syslog(LOG_ERR, "ACS_USAFM: Software Update/Rollback is In Progress");
				log.Write("ACS_USAFM: Software Update/Rollback is In Progress", LOG_LEVEL_INFO);
			}
		}
	}
	else {
		char buffer[256];
		syslog(LOG_INFO, "ACS_USAFM: Error! Event Not Handled  %s", eventType);
		sprintf(buffer, "ACS_USAFM: Error! Event Not Handled  %s", eventType);
		log.Write(buffer, LOG_LEVEL_INFO);
	}

	return ComOk;
}

std::string acs_usa_evtConsumer::getTimeStr(uint64_t datetime)
{

	time_t date_time =  (time_t) ( datetime / acs_usa_evtConsumer::NANOS_PER_SEC );
	tm  tm_buf;
	tm* tm = localtime_r( &date_time, &tm_buf );
	std::ostringstream oss;

	if( ! tm ) {
		memset( &tm_buf, 0, sizeof( tm_buf) );
		tm = & tm_buf;
	}

	oss <<     (uint16_t)( tm->tm_year + 1900 ) <<        // year is defined as years since 1900
			"-" << std::setfill('0') << std::setw(2) << (uint16_t)( tm->tm_mon + 1 ) << // Month is 0-11
			"-" << std::setfill('0') << std::setw(2) << (uint16_t)( tm->tm_mday ) <<
			"_" << std::setfill('0') << std::setw(2) << (uint16_t)( tm->tm_hour ) <<
			":" << std::setfill('0') << std::setw(2) << (uint16_t)( tm->tm_min ) <<
			":" << std::setfill('0') << std::setw(2) << (uint16_t)( tm->tm_sec );

	return oss.str();
}

std::string acs_usa_evtConsumer::getSeverityStr(MafOamSpiNotificationFmSeverityT severity)
{
	std::string severityStr;

	switch ( severity ) {
	case MafOamSpiNotificationFmSeverityCleared:
		severityStr = std::string("CEASING");
		break;

	case MafOamSpiNotificationFmSeverityIndeterminate:
		severityStr = std::string("O1");
		break;

	case MafOamSpiNotificationFmSeverityWarning:
		severityStr = std::string("O2");
		break;

	case MafOamSpiNotificationFmSeverityMinor:
		severityStr = std::string("A3");
		break;

	case MafOamSpiNotificationFmSeverityMajor:
		severityStr = std::string("A2");
		break;

	case MafOamSpiNotificationFmSeverityCritical:
		severityStr = std::string("A1");
		break;

	default:
		severityStr = std::string("O3");
	}
	return severityStr;
}

void acs_usa_evtConsumer::getProcessName(const char *dnPart, char* prosName)
{
	// TR_HQ61511_BEGIN
#if 0 //commented for TR_HQ61511
	const char *tmp = dnPart;
	int len = 0;
	while(*tmp != '\0') {
		if((*tmp >= 'A' && *tmp <= 'Z') || (*tmp >= 'a' && *tmp <= 'z')){
			tmp++;
			len++;
		}
		else{
			break;
		}
	}
	if( (len > 0) && (NULL != prosName) ){
		strncpy(prosName, dnPart, len);
		prosName[len]='\0';
	}
#endif
	if( (NULL != dnPart) && (NULL != prosName) ){
		strcpy(prosName, dnPart);
	}
	// TR_HQ61511_END
}

bool acs_usa_evtConsumer::isPeerNodeDown(std::string amfNode)
{
	OmHandler immHandle;
	ACS_CC_ReturnType result;
	int nodeOperState = 0;
	bool res = false;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		syslog(LOG_INFO, "ACS_USAFM:  OmHandler Init Failed");
		log.Write("ACS_USAFM:  OmHandler Init Failed", LOG_LEVEL_ERROR);
		return res;
	}

	ACS_CC_ImmParameter immNodeOperState;
	immNodeOperState.attrName = const_cast<char*>("saAmfNodeOperState");

	result = immHandle.getAttribute(amfNode.c_str(), &immNodeOperState);
	if ( result == ACS_CC_SUCCESS ){
		nodeOperState =  *(reinterpret_cast<int*>(immNodeOperState.attrValues[0]));

		char buffer[512]={'\0'};
		sprintf(buffer, "ACS_USAFM: Node Operation state: %d", nodeOperState);
		log.Write(buffer, LOG_LEVEL_INFO);

		if(NODE_DISABLED == nodeOperState) {
			res = true;
		}
	}
	else{
		log.Write("ACS_USAFM: Node Operation state NOT FOUND", LOG_LEVEL_ERROR);
	}

	immHandle.Finalize();
	return res;
}

bool acs_usa_evtConsumer::isSwUpdateInProgress(void)
{
	OmHandler immHandle;
	vector<string> tmp_vector;
	ACS_CC_ReturnType result;
	bool swUpdateFlag = false;

	ACS_APGCC_ImmObject object;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		syslog(LOG_INFO, "ACS_USAFM:  OmHandler Init Failed");
		log.Write("ACS_USAFM:  OmHandler Init Failed", LOG_LEVEL_ERROR);
		return swUpdateFlag;
	}

	result = immHandle.getClassInstances("UpgradePackage",tmp_vector);
	if ( result != ACS_CC_SUCCESS ){
		//syslog(LOG_INFO,"ACS_USAFM: UpgradePackage Instance NOT FOUND");
		log.Write("ACS_USAFM: UpgradePackage Instance NOT FOUND", LOG_LEVEL_DEBUG);
		result = immHandle.getClassInstances("CmwSwMUpgradePackage",tmp_vector);
		if ( result != ACS_CC_SUCCESS ){
			immHandle.Finalize();
			log.Write("ACS_USAFM: CmwSwUpgradePackage Instance NOT FOUND", LOG_LEVEL_DEBUG);
			//syslog(LOG_INFO,"ACS_USAFM: CmwSwUpgradePackage Instance NOT FOUND");
			return swUpdateFlag;
		}
		else {
			log.Write("ACS_USAFM: CmwSwUpgradePackage Instance FOUND", LOG_LEVEL_DEBUG);
			//syslog(LOG_INFO,"ACS_USAFM: CmwSwUpgradePackage Instance FOUND");
		}
	}
	else {
		log.Write("ACS_USAFM: UpgradePackage Instance FOUND", LOG_LEVEL_DEBUG);
		syslog(LOG_INFO,"ACS_USAFM: UpgradePackage Instance FOUND");
	}

	char buffer[512]={'\0'};
	sprintf(buffer, "ACS_USAFM:  checkSwUpdateState UpgradePackage Instances vect size : %ld", tmp_vector.size());
	log.Write(buffer, LOG_LEVEL_INFO);

	if(tmp_vector.size() != 0)
	{
		int swUpdateState = SW_UP_UNKNOWN;
		int swUpdateProgressState = UNKNOWN;
		std::string swUpdateDN;
		std::string swUpReportPrgsDN;

		ACS_CC_ImmParameter immSwUpdateState;
		immSwUpdateState.attrName = const_cast<char*>("state");

		ACS_CC_ImmParameter immSwUpdateReportProgress;
		immSwUpdateReportProgress.attrName = const_cast<char*>("reportProgress");


		swUpdateDN = tmp_vector[0].c_str();
		char buffer[512]={'\0'};
		sprintf(buffer, "ACS_USAFM:  checkSwUpdateState UpgradePackage DN: %s", swUpdateDN.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);


		result = immHandle.getAttribute(swUpdateDN.c_str(), &immSwUpdateState);
		if ( result == ACS_CC_SUCCESS ){
			swUpdateState =  *(reinterpret_cast<int*>(immSwUpdateState.attrValues[0]));
			syslog(LOG_INFO, "ACS_USAFM:  checkSwUpdateState UpgradePackage state: %d", swUpdateState);

			char buffer[512]={'\0'};
			sprintf(buffer, "ACS_USAFM:  checkSwUpdateState UpgradePackage state: %d", swUpdateState);
			log.Write(buffer, LOG_LEVEL_INFO);

		}
		else{
			syslog(LOG_ERR, "ACS_USAFM:  checkSwUpdateState UpgradePackage state NOT FOUND");
			log.Write("ACS_USAFM:  checkSwUpdateState UpgradePackage state NOT FOUND", LOG_LEVEL_ERROR);
		}

		result = immHandle.getAttribute(swUpdateDN.c_str(), &immSwUpdateReportProgress);
		//Fix for TR HY44723 Added check for immSwUpdateReportProgress value
		if ( (result == ACS_CC_SUCCESS) && (immSwUpdateReportProgress.attrValuesNum != 0) && (immSwUpdateReportProgress.attrValues[0] != NULL)){
			swUpReportPrgsDN  =  (char*)immSwUpdateReportProgress.attrValues[0];
			char buffer[512]={'\0'};
			sprintf(buffer, "ACS_USAFM:  checkSwUpdateState UpgradePackage reportProgress DN: %s", swUpReportPrgsDN.c_str());
			log.Write(buffer, LOG_LEVEL_INFO);


			result = immHandle.getAttribute( swUpReportPrgsDN.c_str(), &immSwUpdateState);

			if ( result == ACS_CC_SUCCESS ){
				swUpdateProgressState =  *(reinterpret_cast<int*>(immSwUpdateState.attrValues[0]));
				syslog(LOG_INFO, "ACS_USAFM:  checkSwUpdateState UpgradePackage swUpdateProgressState : %d", swUpdateProgressState);
				char buffer[512]={'\0'};
				sprintf(buffer, "ACS_USAFM:  checkSwUpdateState UpgradePackage swUpdateProgressState : %d", swUpdateProgressState);
				log.Write(buffer, LOG_LEVEL_INFO);
			}
			else {
				syslog(LOG_ERR, "ACS_USAFM:  checkSwUpdateState UpgradePackage swUpdateProgressState NOT FOUND");
				log.Write("ACS_USAFM:  checkSwUpdateState UpgradePackage swUpdateProgressState NOT FOUND", LOG_LEVEL_ERROR);
			}
		}
		else{
			syslog(LOG_ERR, "ACS_USAFM:  checkSwUpdateState UpgradePackage reportProgress NOT FOUND");
			log.Write("ACS_USAFM:  checkSwUpdateState UpgradePackage reportProgress NOT FOUND", LOG_LEVEL_ERROR);
		}

		switch(swUpdateState)
		{
		case SW_UP_UNKNOWN:
		case SW_UP_INITIALIZED:
		case SW_UP_PREPARE_IN_PROGRESS:
		case SW_UP_PREPARE_COMPLETED:
		case SW_UP_WAITING_FOR_COMMIT:
		case SW_UP_COMMIT_COMPLETED:
			swUpdateFlag = false;
		default:
			swUpdateFlag = true;
		}

		if(swUpdateFlag) {
			if(CANCELLING == swUpdateProgressState){
				log.Write("ACS_USAFM: checkSwUpdateState SW UPDATE: ACTIVATION/DEACTIVATION - CANCELLING", LOG_LEVEL_INFO);
			}
			else if(RUNNING == swUpdateProgressState){
				log.Write("ACS_USAFM: checkSwUpdateState SW UPDATE: ACTIVATION/DEACTIVATION - RUNNING", LOG_LEVEL_INFO);
			}
			else {
				swUpdateFlag=false;
			}
		}
	}
	immHandle.Finalize();

	return swUpdateFlag;
}

bool acs_usa_evtConsumer::isClusterRebootInProgress(void)
{
	char fileName[4][100] = {{0}, {0}};
	sprintf(fileName[0], "/var/log/SC-2-%d/messages", mThisNodeId);
	sprintf(fileName[1], "/var/log/SC-2-%d/messages.1", mThisNodeId);
	sprintf(fileName[2], "/var/log/SC-2-%d/messages", mPeerNodeId);	
	sprintf(fileName[3], "/var/log/SC-2-%d/messages.1", mPeerNodeId);

	for (int i = 0; i < 4; ++i)
	{		
		if (isClusterRebootInProgress(fileName[i]))
		{
			return true;			
		}	
	}

	return false;
}

bool acs_usa_evtConsumer::isClusterRebootInProgress(const char* fileName)
{
	bool isClusterReboot = false;
	char buffer[512]={'\0'};	

	// Get the current time
	time_t currentTimeStamp = time(NULL);
	struct tm* currentTime = localtime(&currentTimeStamp);

	::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Opening file %s for detecting cluster reboot", fileName);
	log.Write(buffer, LOG_LEVEL_INFO);

	// Open the file to parse the cluster info keyword	
	int fd = open(fileName, O_RDONLY);	
	if (fd == -1)
	{		
		::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: failed to open %s for detecting cluster reboot - errno: %d", fileName, errno);
		log.Write(buffer, LOG_LEVEL_ERROR);
		syslog(LOG_INFO, "Failed to open file , errno : %d", errno);
		return isClusterReboot;
	}


	syslog(LOG_INFO, "Able to open file , errno : %d", errno);
	// Start to read and parse the messages file
	const int MAX_BUFFER_SIZE = 65535;
	char* buf = new char[MAX_BUFFER_SIZE + 1];
	const char* CLUSTER_REBOOT_KEY_WORD = "cluster-reboot";
	const int TIME_DIFF = 300; // 5 mins	

	do
	{
		ssize_t count = read(fd, buf, MAX_BUFFER_SIZE);
		if (count < 0)
		{			
			::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: failed to read %s - errno: %d", fileName, errno);
			log.Write(buffer, LOG_LEVEL_ERROR);
			break;
		}

		if (count == 0)
		{
			// end of file so exit the reading loop
			break;
		}

		buf[count] = 0;
		char* content = buf;

		// Find the keyword "cmwea: cluster-reboot" in the reading content
		// and retrieve the date time of the found line.
		// Example line:
		//	2014-09-22 08:25:02 SystemEvent cmwea SC-2-1 info cmwea: cluster-reboot
		//	01234567890123456789012345678901234567890123456789
		//      1         2         3         4         5		
		do
		{
			char* result = strstr(content, CLUSTER_REBOOT_KEY_WORD);
			if (result == NULL)
			{
				break;
			}

			if (result - buf >= 50)
			{
				char time[20] ={0};
				// Get the date/time of the found line
				memcpy(time, (void*) (result - 50), 19);

				// convert date/time string to timestamp
				struct tm foundTime;
				strptime(time, "%Y-%m-%d %H:%M:%S", &foundTime);

				// Convert tm to time_t
				foundTime.tm_isdst = -1;
				time_t foundTimeStamp = mktime(&foundTime);                                
				int diff = (int) currentTimeStamp - (int) foundTimeStamp;

				// Compare the timestamp of the keyword and the current time
				// If it is less than 5 minutes that means it's the cluster reboot indication.
				if (diff >= 0 && diff <= TIME_DIFF)
				{	
					char buffer[512]={'\0'};
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Current time: %s", asctime(currentTime));
					log.Write(buffer, LOG_LEVEL_INFO);
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Found cluster reboot time: %s", asctime(&foundTime));
					log.Write(buffer, LOG_LEVEL_INFO);
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Time diff in seconds - current: %d - found: %d = %d", 
							(int) currentTimeStamp, (int) foundTimeStamp, diff);
					log.Write(buffer, LOG_LEVEL_INFO);
					// Exit the loop if cluster reboot found
					log.Write("ACS_USAFM: cluster reboot in progress found", LOG_LEVEL_INFO);
					isClusterReboot = true;
					break;
				}
			}
			content = result + sizeof(CLUSTER_REBOOT_KEY_WORD);
		} while (true);

		// Cluster reboot in progress is found
		// No need to check again, break the loop and exit
		if (isClusterReboot)
		{
			break;
		}
	} while (true);

	// Deallocate the allocated buf and close the messages file
	delete []buf;
	close(fd);
	return isClusterReboot;
}

int acs_usa_evtConsumer::getNodeId (const char* nodeIdfilePath)
{
	int nodeId=0;

	ifstream ifs;
	char node_Id[8]={'\0'};

	ifs.open(nodeIdfilePath);
	if ( ifs.good())
		ifs.getline(node_Id, 8);

	ifs.close();

	sscanf(node_Id,"%d",&nodeId);

	syslog(LOG_INFO, "ACS_USAFM:  getNodeId() nodeId == [%d]",nodeId);
	return nodeId;

}

bool acs_usa_evtConsumer::insertIntoFalseAlarmList(std::string appName)
{
	bool res = false;
	bool dupEntry = false;
	std::list<std::string>::iterator iter;
	for (iter = mFalseAlarmList.begin(); iter!= mFalseAlarmList.end(); ++iter)
	{
		if (*iter == appName) {
			dupEntry = true;
			char buffer[256]={'\0'};
			sprintf(buffer, "ACS_USAFM: insertIntoFalseAlarmList already existed siPros:%s ",appName.c_str());
			log.Write(buffer, LOG_LEVEL_INFO);
			break;
		}
	}
	if (!dupEntry) {
		mFalseAlarmList.push_back(appName);
		res = true;

		char buffer[256]={'\0'};
		sprintf(buffer, "ACS_USAFM: Inserted appName =%s listSize:%ld",appName.c_str(), mFalseAlarmList.size());
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	return res;
}

bool acs_usa_evtConsumer::removeFalseAlarm(std::string siPros)
{
	bool res = false;
	std::list<std::string>::iterator iter;
	for (iter = mFalseAlarmList.begin( ); iter != mFalseAlarmList.end(); iter++)
	{
		if (siPros == *iter) {
			iter = mFalseAlarmList.erase(iter);
			res = true;
			break;
		}
	}
	if (res == true) {
		char buffer[256]={'\0'};
		sprintf(buffer, "ACS_USAFM: Removed siPros=%s listSize:%ld",siPros.c_str(), mFalseAlarmList.size() );
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	return res;
}
int acs_usa_evtConsumer::fetchClsAttributes(const char *clsName) {

	int state = -2;

	SaImmHandleT immHandle;
	string strObjectName;
	SaAisErrorT error;
	SaImmSearchHandleT searchHandle;
	SaImmSearchParametersT_2 searchParam;
	SaNameT objectName;
	SaImmAttrValuesT_2 **attributes;
	SaNameT rootName = { 0, "" };
	SaImmScopeT scope = SA_IMM_SUBTREE;     /* default search scope */
	char classNameBuf[SA_MAX_NAME_LENGTH] = {0};
	const char *className = classNameBuf;

	strncpy(classNameBuf, clsName, SA_MAX_NAME_LENGTH);


	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK){
		syslog(LOG_ERR, "ACS_USAFM:  getSaAmfNode() saImmOmInitialize error");
		return -1;
	}

	searchParam.searchOneAttr.attrName = const_cast<char *>("SaImmAttrClassName");
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
	searchParam.searchOneAttr.attrValue = &className;

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
			SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
			&searchHandle);

	if (error != SA_AIS_OK) {
		syslog(LOG_ERR, "ACS_USAFM:  getSaAmfNode() saImmOmSearchInitialize_2 error");
		error = saImmOmFinalize(immHandle);
		return -1;
	}

	do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if ( error == SA_AIS_ERR_NOT_EXIST) {
			//syslog(LOG_INFO, "ACS_USAFM:  getSaAmfNode() saImmOmSearchNext_2 error");
			break;
		}

		strObjectName = string((char*)(objectName.value));

		if(0==strncmp("SaAmfNode", clsName, 9))
		{
			std::string strNodeId = getValue(strObjectName, "SC-");
			if(0 != strNodeId.length()){
				int iNodeId = 0;
				sscanf(strNodeId.c_str(),"%d",&iNodeId);

				if(iNodeId == mPeerNodeId) {
					strPeerNodeAmfDN = strObjectName;
					char buffer[256]={'\0'};
					sprintf(buffer, "ACS_USAFM:  PeerNodeDN:%s[%ld]",strPeerNodeAmfDN.c_str(),strPeerNodeAmfDN.length());
					log.Write(buffer, LOG_LEVEL_INFO);
				}
			}
		}
		else if(0==strncmp("SaAmfSIAssignment", clsName, 17))
		{
			std::string buffer1;
			std::string buffer2;

			if ( is_swm_2_0_fm == true ){
				buffer1 = "NWA";
				buffer2 = "ERIC-apg";
			}
			else {
				buffer1 = "NoRed";
				buffer2 = "ERIC-APG";
			}


			if ( ( strObjectName.find (buffer1.c_str()) != std::string::npos ) && ( strObjectName.find ( buffer2.c_str() ) != std::string::npos ) ){

				char buffer[256]={'\0'};
				sprintf(buffer, "ACS_USAFM:  SaAmfSIAssignment ObjectName:%s",strObjectName.c_str());
				log.Write(buffer, LOG_LEVEL_INFO);

				std::string strSuId = getValue(strObjectName, "safSu=");
				std::string strSiPros = getValue(strObjectName, "safSi=");
				if((0 != strSuId.length())&& (0 != strSiPros.length())){
					int iSuId = 0;
					sscanf(strSuId.c_str(),"%d",&iSuId);
					insertIntoSiSuMap(strSiPros, iSuId );
				}
			}
		}
	} while (error != SA_AIS_ERR_NOT_EXIST);

	error = saImmOmSearchFinalize(searchHandle);

	if (error != SA_AIS_OK) {
		syslog(LOG_ERR, "ACS_USAFM:  getSaAmfNode() saImmOmSearchFinalize error");
		return -1;
	}

	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) {
		syslog(LOG_ERR, "ACS_USAFM:  getSaAmfNode() saImmOmFinalize error");
		return -1;
	}

	return state;
}

std::string acs_usa_evtConsumer::getValue(std::string data, const char* compStr)
{
	char * pch;
	char * cstr;
	std::string value;
	std::string token;
	cstr = new char [data.size()+1];
	strcpy (cstr, data.c_str());

	pch = strtok (cstr,",");

	size_t pos;
	while (pch != NULL){
		token.assign ( const_cast <char*> ( pch ) );
		if ( (pos = token.find(compStr)) != std::string::npos ){
			value = token.substr ( pos+strlen(compStr) );
			break;
		}
		pch = strtok (NULL, ",");
	}
	delete[] cstr;
	return value;
}

bool acs_usa_evtConsumer::insertIntoSiSuMap(std::string key, int value)
{
	std::pair<std::map<std::string, int>::iterator,bool> ret;

	ret = mSiSuList.insert ( std::pair<std::string, int>(key,value) );
	if (ret.second==false) {
		char buffer[256]={'\0'};
		sprintf(buffer, "ACS_USAFM: insertIntoSiSuMap() %s Already existed with %d ",key.c_str(),ret.first->second );
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	else {
		char buffer[256]={'\0'};
		sprintf(buffer, "ACS_USAFM: insertIntoSiSuMap() successfully INSERTED for si:%s su:%d ",key.c_str(),value );
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	return ret.second;
}

bool acs_usa_evtConsumer::getSuId(std::string siPros, int &suId)
{
	bool res = false;
	std::map<std::string,int>::iterator iter;

	iter = mSiSuList.find(siPros);

	if(iter != mSiSuList.end())
	{
		suId = iter->second;
		res = true;
		char buffer[256]={'\0'};
		sprintf(buffer,"ACS_USAFM: getSuId() suId:%d found for siPros:%s ",suId, siPros.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	else {
		char buffer[256]={'\0'};
		sprintf(buffer,"ACS_USAFM: getSuId() suId NOT found for siPros:%s ",siPros.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	return res;
}

using namespace std;
using boost::property_tree::ptree;

#define FBN_APUB			300

/*! @brief Returns the slot and the ap number of the passed node
 *
 * @param[in] 	hostName	The name of the node
 * @param[out] 	slot			The slot number
 * @param[out] 	ap				The ap number
 */
bool acs_usa_evtConsumer::getSlotAp(string hostName, unsigned short &slot, unsigned short &ap)
{
	slot = -1;
	ap = -1;
	bool returnValue = false;
	int apgNodeNum = getApNodeNumber();
	bool itsMe = hostName == getFileContent("/etc/cluster/nodes/this/hostname");
	string ipString = getFileContent("/etc/cluster/nodes/this/networks/ipna/primary/address");
	stringstream trimmer;
	trimmer << ipString;
	trimmer >> ipString; // In order to remove possible trim spaces
	in_addr addr = {0};
	::inet_pton(AF_INET, ipString.c_str(), &addr);
	unsigned int ip = ntohl(addr.s_addr);

	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

	if (hwc)
	{
		ACS_CS_API_BoardSearch *boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

		if (boardSearch)
		{
			ACS_CS_API_IdList boardList;

			boardSearch->setFBN(FBN_APUB);

			(void) hwc->getBoardIds(boardList, boardSearch);
			int boards = boardList.size();

			char buffer [256];
			sprintf (buffer, "ACS_USAFM: APUB board list size %d ", boards);
			log.Write(buffer, LOG_LEVEL_INFO);
			if (boards > 0)
			{
				for (int i=0; i < boards; i++)
				{
					unsigned short boardId = boardList[i];
					uint32_t IPEthA = 0;
					unsigned short ApSysNo = 0;

					hwc->getIPEthA(IPEthA, boardId);
					hwc->getSysNo(ApSysNo, boardId);
					if ((ApSysNo == (unsigned short) apgNodeNum) &&(apgNodeNum > 0))
					{
						if (itsMe && (IPEthA == ip))
						{
							hwc->getSlot(slot, boardId);
							hwc->getSysNo(ap, boardId);
							returnValue = true;
							break;
						}
						if (!itsMe && (IPEthA != ip))
						{
							hwc->getSlot(slot, boardId);
							hwc->getSysNo(ap, boardId);
							returnValue = true;
							break;
						}
					}
				}
			}
			else
			{
				syslog (LOG_WARNING, "ACS_USAFM: ACS_CS_API::getBoardIds no APUB boars configured");
			}
		}
		else
		{
			syslog (LOG_WARNING, "ACS_USAFM: ACS_CS_API::createHWCInstance failed");
		}
		ACS_CS_API::deleteHWCInstance(hwc);
		char buffer[256]={'\0'};
		sprintf(buffer,"ACS_USAFM: getSlotAp() slot found is:%d ",slot);
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	return returnValue;
}

/*! @brief Returns the first line of the passed file
 *
 * @param[in] 	file			The complete name of the file
 * @return 								The first line (as string)
 */
string acs_usa_evtConsumer::getFileContent(string file)
{
	string content;
	ifstream ifs(file.c_str(), std::ifstream::in);

	if (ifs.good())
		getline(ifs, content);

	ifs.close();

	return content;
}

/*! @brief Extracts the token value following the passed token name
 *
 * @param[out] 	dest			The pointer of the buffer where to write the token value
 * @param[in] 	source		The string where to look for the token name
 * @param[in] 	tok				The token name
 * @param[in] 	delimiter The delimiter terminating the token value
 * @return 								true, if the token has been extracted, false otherwise.
 *												The token value is extraced also if its found an end of
 *												string instead of the delimiter
 */
bool acs_usa_evtConsumer::getToken(char *dest, char *source, const char *tok, char delimiter)
{
	char *p = strstr(source, tok);
	if (!p)
		return false; // Error: Token not found

	char *tmpTok = new char[strlen(tok) + 4];
	sprintf(tmpTok, "%s%%s", tok);

	sscanf(p, tmpTok, dest);
	delete [] tmpTok;

	p = strchr(dest, delimiter);
	if (p)
	{
		*p = 0; // Terminate the Token string

		return true;
	}
	return strlen(dest);
}

//APGCC
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"

/*! @brief Returns the teaming status
 *
 * Function cloned from APBM
 *
 * @return 								-1 if error, the teaming status value otherwise
 */
int acs_usa_evtConsumer::check_teaming_status()
{
	int teaming_status = -1;
	OmHandler omManager;
	acs_apgcc_paramhandling pha;
	ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS)
	{
		char buffer[128];
		strcpy(buffer, "ACS_USAFM: Call 'OmHandler Init' failed: cannot get the class name!");
		syslog(LOG_INFO, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return teaming_status;
	}/*exiting on IMM init failure: generic error*/

	string class_instance_name;
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances( "NorthBound", p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1))
	{
		char buffer[128];
		strcpy(buffer, "ACS_USAFM: Call 'getClassInstances' failed: cannot get the class name!");
		syslog(LOG_INFO, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		/*exiting on error either an erroe occurred or more than one instance found */
		omManager.Finalize();
		return teaming_status;
	}
	class_instance_name = p_dnList[0];
	omManager.Finalize();
	const char * attrib_name = "teamingStatus";

	ACS_CC_ReturnType imm_result = pha.getParameter(class_instance_name.c_str(), attrib_name, &teaming_status);
	if (imm_result != ACS_CC_SUCCESS)
	{ // ERROR: getting parameter value from IMM
		char buffer[256] = {0}; buffer[sizeof(buffer) - 1] = 0;
		::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Call 'getParameter' failed: cannot retrieve the teaming status parameter from IMM: "
				"internal last error == %d: internal laste error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
		syslog(LOG_INFO, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return -1;
	}
	return teaming_status;
}

/*! @brief Compute the problem description for alarm USAFM_ETHERNET_BONDING_FAILED
 *
 * @param[out]	description		The pointer of the buffer where to write the description value
 * @param[in]	originalAdditionalText	Additional text of original notification from COM
 * @return				true in case of success, false otherwise
 */
bool acs_usa_evtConsumer::getBondingProblemDescription(char *description,char *originalAdditionalText)
{
	bool returnValue = false;

	if (strstr(originalAdditionalText, "links down"))
	{
		sprintf(description, "BOTH NIC ARE DOWN");
		returnValue = true;
	} else {
		char interfaceName[256] = {0};
		if (getToken(interfaceName, originalAdditionalText, "link down on ", ')'))
		{
			std::string interfaceNameString (interfaceName);
			std::transform(interfaceNameString.begin(), interfaceNameString.end(), interfaceNameString.begin(), ::toupper);
			sprintf(description, "NIC %s IS DOWN", interfaceNameString.c_str());
			returnValue = true;
		}
	}

	return returnValue;
}

/*! @brief Returns the APG OAM ACCESS
 *
 * Function cloned from NSF
 *This method is used to get the APG OAM access.
 * @return		int
 * 			 0: front cable
 * 			 1: no cable (cable-less)
 * 			 2: no applicable
 *
 * @return 								-1 if error, the teaming status value otherwise
 */
int acs_usa_evtConsumer::getApgOamAccess()
{
	int apgOamAccess = -1;

	OmHandler omHandler;
	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		log.Write("ACS_USAFM: OmHandler initialization failed in method getApgOamAccess", LOG_LEVEL_ERROR);
		return -1;
	}

	ACS_CC_ImmParameter attrApgOamAccess;
	attrApgOamAccess.attrName = (char *)APG_OAM_ACCESS_ATTR_NAME;
	std::string dnName = AXEFUNCTIONS_OBJ_DN;

	if (omHandler.getAttribute(dnName.c_str(), &attrApgOamAccess) == ACS_CC_SUCCESS)
	{
		if (attrApgOamAccess.attrName == (char*)APG_OAM_ACCESS_ATTR_NAME)
		{
			apgOamAccess = *(reinterpret_cast<int*>(*(attrApgOamAccess.attrValues)));
		}
	}
	else
	{
		log.Write("ACS_USAFM: Failed to get value of "APG_OAM_ACCESS_ATTR_NAME" in method getApgOamAccess", LOG_LEVEL_ERROR);
	}

	omHandler.Finalize();

	syslog(LOG_INFO,"APG OAM Access value = %d", apgOamAccess);
	return apgOamAccess;
}

/*! @brief Returns the AP NODE NUMBER
 *
 *This method is used to get the AP NODE NUMBER.
 * @return               int
 *                               1: AP1
 *                               2: AP2
 *
 * @return                                                               -1 if error, the APG NODE NUMBER value otherwise
 */
int acs_usa_evtConsumer::getApNodeNumber()
{
	int apNodeNumber = -1;

	OmHandler omHandler;
	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		log.Write("ACS_USAFM: OmHandler initialization failed in method getApNodeNumber", LOG_LEVEL_ERROR);
		return -1;
	}

	ACS_CC_ImmParameter attrApNodeNumber;
	attrApNodeNumber.attrName = (char *)AP_NODE_NUMBER_ATTR_NAME;
	std::string dnName = AXEFUNCTIONS_OBJ_DN;

	if (omHandler.getAttribute(dnName.c_str(), &attrApNodeNumber) == ACS_CC_SUCCESS)
	{
		if (attrApNodeNumber.attrName == (char*)AP_NODE_NUMBER_ATTR_NAME)
		{
			apNodeNumber = *(reinterpret_cast<int*>(*(attrApNodeNumber.attrValues)));
		}
	}
	else
	{
		log.Write("ACS_USAFM: Failed to get value of "AP_NODE_NUMBER_ATTR_NAME" in method getApNodeNumber", LOG_LEVEL_ERROR);
	}

	omHandler.Finalize();

	syslog(LOG_INFO,"AP Node Number value = %d", apNodeNumber);

	return apNodeNumber;
}

/*! @brief Return the persistent storage path for alarm list xml file
 *
 *	@return 	string	 persistent storage path
 */
string acs_usa_evtConsumer::getPersistentStoragePath()
{
	static string configPath = "";

	// The path is available no need to fetch again
	if (configPath.compare("") != 0)
	{
		return configPath;
	}

	char buffer[256] = {0}; buffer[sizeof(buffer) - 1] = 0;
	// Read the path from PSA
	ifstream ifs(ACS_USAFM_PSO_CONFIG_PATH);
	if (!ifs.is_open())
	{		
		::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Failed to open %s", ACS_USAFM_PSO_CONFIG_PATH);
		syslog(LOG_ERR, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return "";
	}

	getline(ifs, configPath);
	ifs.close();
	if (configPath.compare("") != 0)
	{
		configPath = configPath + "/" + ACS_USAFM_acs_usafm;
	}
	else
	{
		::snprintf(buffer, sizeof(buffer) - 1, "%s", "ACS_USAFM: config path is empty");
		syslog(LOG_ERR, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return "";
	}

	// Check if configPath exist in the system otherwise create it
	struct stat sb;
	if (stat(configPath.c_str(), &sb) == -1) 
	{
		if (mkdir(configPath.c_str(), 0777) == -1)
		{
			::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Failed to create directory %s", configPath.c_str());
			syslog(LOG_ERR, buffer);
			log.Write(buffer, LOG_LEVEL_ERROR);
			return "";
		}
	}

	return configPath;
}

/*! @brief Flush the alarm list to xml file
 * Note: This function must be run on active node
 */
void acs_usa_evtConsumer::saveAlarmList(bool isStopping)
{	
	log.Write("ACS_USAFM: Enter saveAlarmList", LOG_LEVEL_INFO);

	string filePath = getPersistentStoragePath();
	char buffer[256] = {0}; 
	buffer[sizeof(buffer) - 1] = 0;

	// Check the path
	if (filePath.compare("") == 0)
	{
		::snprintf(buffer, sizeof(buffer) - 1, "%s", "ACS_USAFM: Can't save file due to empty config path");
		syslog(LOG_ERR, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return;
	}

	if (isStopping)
	{
		bool isClusterRepl = isClusterReplicated();

		log.Write("ACS_USAFM: saveAlarmList, USAFM is going down", LOG_LEVEL_INFO);
		for (AlarmList::iterator it1 = alarmList.begin(); it1 != alarmList.end(); )
		{
			log.Write("ACS_USAFM: enter alarmList in saveAlarmList", LOG_LEVEL_INFO);
			if (it1->specificProblem == ACS_USAFM_DiskReplication && isClusterRepl)
			{
				log.Write("ACS_USAFM: Cluster is in sync, alarm DiskReplication is removed from internal list", LOG_LEVEL_INFO);
				removeFromAlarmList(*it1);
				it1 = alarmList.begin();
			}
			else
			{
				it1++;
			}
		}
	}
	// Start to save file
	filePath = filePath + "/" + ACS_USAFM_xml_filename;	
	stringstream ss;
	ss << ACS_USAFM_activeFmAlarmList << ACS_USAFM_xml_attr << ACS_USAFM_activeNode;	
	ptree pt;
	try
	{
		pt.add(ss.str(), mThisNodeId);
		for (AlarmList::iterator it = alarmList.begin(); it != alarmList.end(); ++it)
		{
			if(it->specificProblem != ACS_USAFM_AmfSIUnassigned)
			{
				ss.str("");
				ss << ACS_USAFM_activeFmAlarmList << "." << ACS_USAFM_alarm;
				ptree& node = pt.add(ss.str(), "");
				node.put(ACS_USAFM_processName, it->processName);
				node.put(ACS_USAFM_specificProblem, it->specificProblem);
				node.put(ACS_USAFM_severity, it->severity);
				node.put(ACS_USAFM_probableCause, it->probableCause);		
				node.put(ACS_USAFM_objClassOfReference, it->objClassOfReference);
				node.put(ACS_USAFM_objectOfReference, it->objectOfReference);
				node.put(ACS_USAFM_problemData, it->problemData);
				node.put(ACS_USAFM_problemText, it->problemText);
				node.put(ACS_USAFM_manualCease, it->manualCease);
				node.put(ACS_USAFM_eventTime, it->originalEventTime);			
				node.put(ACS_USAFM_node, it->node);			
			}
		}

		boost::property_tree::xml_writer_settings<char> settings('\t', 1);
		write_xml(filePath, pt, std::locale(), settings);
	}
	catch (exception& e)
	{
		::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Failed to save content to file: %s - %s", filePath.c_str(), e.what());
		syslog(LOG_ERR, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
	}

	log.Write("ACS_USAFM: Exit saveAlarmList", LOG_LEVEL_INFO);
}

/*! @brief Load the alarm list from xml file
 * Note: This function must be run on active node
 */
void acs_usa_evtConsumer::loadAlarmList()
{
	log.Write("ACS_USAFM: Enter loadAlarmList", LOG_LEVEL_INFO);
	unsigned int long minorType = 0;
	int vendorId = 0;
	bool removeAlarm = false;
	string filePath = getPersistentStoragePath();
	char buffer[256] = {0}; 
	buffer[sizeof(buffer) - 1] = 0;

	// Check the path
	if (filePath.compare("") == 0)
	{
		::snprintf(buffer, sizeof(buffer) - 1, "%s", "ACS_USAFM: Can't save file due to empty config path");
		syslog(LOG_ERR, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return;
	}

	filePath = filePath + "/" + ACS_USAFM_xml_filename;	

	// Check for zero file size
	struct stat st; 
	if (stat(filePath.c_str(), &st) != 0 ) {
		::snprintf(buffer, sizeof(buffer) - 1, "%s [%s]", "ACS_USAFM: file not found:", filePath.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);
		syslog(LOG_INFO, buffer);
		return;
	} else if (st.st_size == 0){
		::snprintf(buffer, sizeof(buffer) - 1, "%s [%s]", "ACS_USAFM: empty file found: ", filePath.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);
		syslog(LOG_INFO, buffer);
		return;
	}

	// Start to load file
	alarmList.clear();
	try
	{
		stringstream ss;
		ss << ACS_USAFM_activeFmAlarmList << ACS_USAFM_xml_attr << ACS_USAFM_activeNode;	
		ptree pt;
		read_xml(filePath, pt);
		int activeNode = pt.get<int>(ss.str());	
		long l_originalEventTime=0;
		BOOST_FOREACH(ptree::value_type const&v, pt.get_child("activeFmAlarmList")) 
		{ 
			if (v.first == "alarm") 
			{
				l_originalEventTime = v.second.get<long>("originalEventTime", 0);
				AlarmInfo alarm(v.second.get<string>("processName"), v.second.get<long>("specificProblem"),
						v.second.get<string>("severity"), v.second.get<string>("probableCause"),
						v.second.get<string>("objClassOfReference"),
						v.second.get<string>("objectOfReference"), v.second.get<string>("problemData"),
						v.second.get<string>("problemText"),
						v.second.get<bool>("manualCease"),
						l_originalEventTime,
						v.second.get<int>("node"));

				int flag=0;
				log.Write("ACS_USAFM: enter alarmList in saveAlarmList", LOG_LEVEL_INFO);
				if (alarm.specificProblem == ACS_USAFM_EthernetBondingFailed1 || alarm.specificProblem == ACS_USAFM_EthernetBondingFailed2)
				{
					int apgOamAccVal = getApgOamAccess();

					//teaming not active;remove the alarm info from the list
					if ((1 != check_teaming_status()) || (1 == apgOamAccVal) || (2 == apgOamAccVal)){
						removeFromAlarmList(alarm);
					}
					else{

						string  cmdString = "/usr/bin/cat /sys/class/net/eth0/operstate";
						FILE   *fp = popen(cmdString.c_str(), "r");
						if (fp != NULL){
							char *line = NULL;
							size_t len = 0;
							ssize_t read;
							string strLine("");
							while ((read = getline(&line, &len, fp)) != -1){
								strLine.assign(line);
								if (strLine.length() == 0)  continue;
								string link("up");
								if ( string::npos != strLine.find(link) ) {
									acs_aeh_evreport objEvReport;
									ACS_AEH_ReturnType retValue;
									alarm.severity=ACS_USAFM_Severity_CEASING;
									log.Write("ACS_USAFM: Reliable Ethernet False Alarm Present, Ceasing it...",LOG_LEVEL_INFO);
									retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(),
											alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(), "-", "-", alarm.manualCease);
									if (retValue != ACS_AEH_ok)
									{
										syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
										log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
									}
									else
									{
										if (alarm.severity.compare(ACS_USAFM_Severity_EVENT) != 0)
										{
											// Remove the ceased alarm from list
											if (alarm.severity.compare(ACS_USAFM_Severity_CEASING) == 0)
											{
												removeFromAlarmList(alarm);
											}
											else{
												flag=1;
											}
										}
									}
								}
								else{
									flag=1;
								}
							}
						}
						pclose(fp);
					}
				}

				else
				{
					string objRef = alarm.objectOfReference;
					string problemText = alarm.problemText;
					switch (alarm.specificProblem) {
					case ACS_USAFM_DiskReplication:
						vendorId = 193;
						minorType = USAFM_DISK_REPLICATION_COMM_FAILED;
						break;
					case ACS_USAFM_TimeSynchronizationFault:
						vendorId = 193;
						minorType = USAFM_TIME_SYNC_FAULT;
						break;
					case ACS_USAFM_IntimatePRCHANodeUnavailable:
						vendorId = 193;
						minorType = USAFM_NODE_UNAVALABLE;
						break;
					case ACS_USAFM_CertAboutToExpire:
						vendorId = 193;
						minorType = USAFM_TRUSTED_CERT_ABOUTTO_EXPIRE;
						break;
					case ACS_USAFM_AuthenticationFailureLimitReached:
						vendorId = 193;
						minorType = USAFM_AUTH_FAILURE_LIMIT_REACHED;
						break;
					case ACS_USAFM_EthernetBondingFailed1:
					case ACS_USAFM_EthernetBondingFailed2:
						vendorId = 0; // For ethernet bnding, seperate check is already present in loadAlarmList
						break;
					case ACS_USAFM_ValidCertUnavailable:
						vendorId = 193;
						if(objRef.compare("certificate Unavailable")== 0)
							minorType = USAFM_VALIDCERT_NOT_AVAILABLE;
						else if(objRef.compare("Certificate About To Expire")== 0)
							minorType = USAFM_CERT_ABOUTTO_EXPIRE;
						else
							minorType = USAFM_AUTO_ENROLL_FAILED;
						break;
					case ACS_USAFM_AmfSIUnassigned:
						vendorId = 18568;
						minorType = USAFM_AP_PRCESS_STOPPED;
						break;
					case ACS_USAFM_InitiatePRCToAPReboot:
						vendorId = 18568;
						if(problemText.length()== 33)
							minorType = USAFM_COMP_CLEANUP_FAILED;
						else if(problemText.length()== 34)
							minorType = USAFM_COMP_INTANTIATE_FAILED;
						break;
					case ACS_USAFM_LogStream_Reg_1_Failed:				
					case ACS_USAFM_LogStream_Sec_1_Failed:
					case ACS_USAFM_LogStream_Reg_2_Failed:
					case ACS_USAFM_LogStream_Sec_2_Failed:
						vendorId = 193;
						minorType = USAFM_LOG_STREAMING_FAILED;
						break;
					default:
						vendorId = 0;
						break;

					}

					removeAlarm = readMissedAlarmNotifications(alarm, minorType, vendorId, l_originalEventTime);
					if(removeAlarm)
					{

						acs_aeh_evreport objEvReport;
						ACS_AEH_ReturnType retValue;
						alarm.severity=ACS_USAFM_Severity_CEASING;

						log.Write("ACS_USAFM: False Alarm Present, Ceasing it...",LOG_LEVEL_INFO);
						retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(),
								alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(), "-", "-", alarm.manualCease);
						if (retValue != ACS_AEH_ok)
						{
							syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
							log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
						}
						else
						{
							syslog(LOG_INFO, "Deleting the Alarm from alarm list for false alarms");
							removeFromAlarmList(alarm);
						}
					}
					else
					{
						flag=1;
					}
				}


				if(flag == 1){
					addToAlarmList(alarm);


					if (alarm.node == 0)
					{
						if (alarm.specificProblem == ACS_USAFM_AmfSIUnassigned)
						{
							removeFromAlarmList(alarm);
						}
						// Alarm for the cluster different from SI UNASSIGNED
						else
						{
							// Raise alarm
							acs_aeh_evreport objEvReport;
							ACS_AEH_ReturnType retValue;
							retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(),
									alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(),
									alarm.problemData.c_str(), alarm.problemText.c_str(), alarm.manualCease);
							if (retValue != ACS_AEH_ok)
							{
								syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
								log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
							}
						}
					}

					// Switch over happened so raise alarm
					if (activeNode != mThisNodeId && alarm.node != 0)
					{
						if (alarm.node == mThisNodeId )
						{
							// Raise alarm
							acs_aeh_evreport objEvReport;
							ACS_AEH_ReturnType retValue;
							

							retValue = objEvReport.sendEventMessage(alarm.processName.c_str(), alarm.specificProblem, alarm.severity.c_str(),
									alarm.probableCause.c_str(), alarm.objClassOfReference.c_str(), alarm.objectOfReference.c_str(),
									alarm.problemData.c_str(), alarm.problemText.c_str(), alarm.manualCease);

							if (retValue != ACS_AEH_ok)
							{
								syslog(LOG_ERR, "ACS_USAFM: 'sendEventMessage' Failed to send event to AEH");
								log.Write("ACS_USAFM: 'sendEventMessage' Failed to send event to AEH", LOG_LEVEL_ERROR);
							}
						}
					}
				}
				saveAlarmList();
			} 
		}
	}
	catch (exception& e)
	{
		::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Failed to load content from file: %s - %s", filePath.c_str(), e.what());
		syslog(LOG_ERR, buffer);
		log.Write(buffer, LOG_LEVEL_ERROR);
	}

	log.Write("ACS_USAFM: Exit loadAlarmList", LOG_LEVEL_INFO);
}

/*! @brief Add alarm to alarm list
 */
void acs_usa_evtConsumer::addToAlarmList(const AlarmInfo& alarm)
{
	char buffer[256] = {0}; 
	buffer[sizeof(buffer) - 1] = 0;	
	bool alarmExist=false;

	// check if the new alarm is already present in AlarmList
	for (AlarmList::iterator it2 = alarmList.begin(); it2 != alarmList.end();)
	{
		if ((it2->specificProblem == alarm.specificProblem) && (it2->node == alarm.node))
		{
			if (it2->severity.compare(alarm.severity) == 0)
			{
				if (it2->objectOfReference.compare(alarm.objectOfReference) == 0 && it2->problemText.compare(alarm.problemText) == 0)
				{	
					::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Alarm(%ld, %s, %s, %s, %ld ) already exist in the AlarmList, ignoring", alarm.specificProblem, alarm.severity.c_str(), alarm.probableCause.c_str(), alarm.objectOfReference.c_str(), alarm.originalEventTime);
					log.Write(buffer, LOG_LEVEL_INFO);
					syslog(LOG_INFO, buffer);
					alarmExist=true;
					break;
				}
			}		
		}	
		it2++;
	}

	if (!alarmExist){
		::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Adding alarm to list (%ld, %s, %s, %s, %ld)",
				alarm.specificProblem, alarm.severity.c_str(), alarm.probableCause.c_str(), alarm.objectOfReference.c_str(), alarm.originalEventTime);
		log.Write(buffer, LOG_LEVEL_INFO);
		syslog(LOG_INFO, buffer);
		alarmList.push_back(alarm);
	}
	syslog(LOG_INFO, "addToAlarmList ends");

}

/*! @brief Remove alarm from alarm list
 */
void acs_usa_evtConsumer::removeFromAlarmList(const AlarmInfo& alarm)
{
	char buffer[256] = {0}; 
	buffer[sizeof(buffer) - 1] = 0;	
	::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM: Removing alarm from list (%ld, %s, %s, %s)",
			alarm.specificProblem, alarm.severity.c_str(), alarm.probableCause.c_str(), alarm.objectOfReference.c_str());
	log.Write(buffer, LOG_LEVEL_INFO);
	alarmList.remove(alarm);
}
/*! @brief Returns true if the node is virtualized
 */
bool acs_usa_evtConsumer::isVirtual()
{
	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;
	if(ACS_CS_API_NS::Result_Success == ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture))
	{
		if(ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED == nodeArchitecture)
			return true;
	}
	return false;
}
/*! @brief Erase the specific alarm from the alarm list
 */
void acs_usa_evtConsumer::eraseFromAlarmList(const AlarmInfo& alarm)
{
	char buffer[256] = {0};
	sprintf(buffer, "ACS_USAFM: Checking to eras alarm for alarm id ==%ld and node id==%dfrom list",alarm.specificProblem, alarm.node);
	log.Write(buffer, LOG_LEVEL_INFO);

	for (AlarmList::iterator it2 = alarmList.begin(); it2 != alarmList.end();)
	{
		if ((it2->specificProblem == alarm.specificProblem) && (it2->node == alarm.node))
		{
			log.Write("ACS_USAFM: eraseFromAlarmList found alarm to erase", LOG_LEVEL_INFO);
			alarmList.erase(it2);
			break;
		}		
		else
		{
			it2++;
		}
	}
}
/*! @brief Verify the status of /cluster replication (drbd0 status)
 */
bool acs_usa_evtConsumer::isClusterReplicated()
{
	log.Write("ACS_USAFM: Enter isClusterReplicated", LOG_LEVEL_INFO);
	bool rCode = false;

	FILE* fp=0;
	bool found=false;

	fp = fopen(FILE_PROC_DRBD, "r");
	if (fp == 0) {
		log.Write("ACS_USAFM: isClusterReplicated(): fopen failed", LOG_LEVEL_INFO);
		return rCode;
	}

	char *pch=0,buffer[256]={0}, *pch1=0;
	while( fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		pch=buffer;
		while ((*pch == ' ') || (*pch == '\t' )) {
			pch++;
		}
		if( strstr(pch, "0:") != NULL) {
			found=true;
			char role1[20]={0}, role2[20]={0}, disk1[20]={0}, disk2[20]={0};
			int pos1=0;
			int pos2=0;
			pch1 = strstr(buffer, "ro:");
			if (pch1 != NULL) {
				pch1=pch1+3; 
				while( *pch1 != '/' ) {
					role1[pos1++]=*pch1++;
				}
				role1[pos1]='\0';
				if( *pch1 == '/' ) {
					pch1++;
				}
				while( *pch1 != ' ' ) {
					role2[pos2++]=*pch1++;
				}
				role2[pos2]='\0';
			} else {
				found=false;
			}
			pos1=0;
			pos2=0;
			pch1 = strstr(buffer, "ds:");
			if (pch1 != NULL) {
				pch1=pch1+3;
				while (*pch1 != '/') {
					disk1[pos1++]=*pch1++;
				}
				disk1[pos1]='\0';
				if( *pch1 == '/' ) {
					pch1++;
				}
				while (*pch1 != ' ') {
					disk2[pos2++]=*pch1++;
				}
				disk2[pos2]='\0';
			} else {
				found=false;
			}

			if ( (strcmp(role1, "Secondary") == 0  && strcmp(role2, "Primary") == 0 ) || (strcmp(role2, "Secondary") == 0  && strcmp(role1, "Primary") == 0 ) ) {
				log.Write("ACS_USAFM: isClusterReplicated(): DRBD0 roles found are not unknown", LOG_LEVEL_INFO);
				if (strcmp(disk1, "UpToDate") == 0 && strcmp(disk2, "UpToDate") == 0) {
					log.Write("ACS_USAFM: isClusterReplicated(): DRBD0 replication is UpToDate", LOG_LEVEL_INFO);
					rCode=true;
				}
				else{
					log.Write("ACS_USAFM: isClusterReplicated(): DRBD0 replication is not UpToDate", LOG_LEVEL_INFO);
				}
			}
			else{
				log.Write("ACS_USAFM: isClusterReplicated(): DRBD0 role unknown found", LOG_LEVEL_INFO);				
			}			
		}
		if (found == true)
			break;
	}
	if (found == false) {
		log.Write("ACS_USAFM: isClusterReplicated(): Active DRBD resource (drbd0) is not found", LOG_LEVEL_INFO);
	}
	fclose(fp); 

	log.Write("ACS_USAFM: Exit from isClusterReplicated", LOG_LEVEL_INFO);

	return rCode;
}
bool isSWM20()
{
	if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
	{
		return true;
	}
	else {
		return false;
	}
}

static char *error_output(SaAisErrorT result)
{
	static char error_result[256];
	sprintf(error_result, "error: %u", result);
	return (error_result);
}

bool  acs_usa_evtConsumer::readMissedAlarmNotifications(const AlarmInfo& alarm,unsigned int long miType,int vendorId,long originalEventTime)
{
	bool ntf_classId_present = false;
	bool alarm_match = false;
	bool removeAlarm = false;
	bool found_Node = true;
	int nodeValue = 0;
	if(vendorId == 0)
	{
		syslog(LOG_INFO, "Skip checking ntfapi calls");
		return removeAlarm;
	}

	static SaUint16T numEventTypes = 0;
	static SaUint16T numNotificationObjects = 0;
	static SaUint16T numNotifyingObjects = 0;
	static SaUint16T numNotificationClassIds = 1;
	static SaUint16T numProbableCauses = 0;
	static SaUint16T numPerceivedSeverities = 0;
	static SaUint16T numTrends = 0;

	SaNtfHandleT ntfHandle;
	static SaNtfCallbacksT ntfCallbacks = {NULL, NULL};
	/* Release code, major version, minor version */
	static SaVersionT version = {'A', 0x01, 0x01};
	SaAisErrorT error = SA_AIS_OK;
	SaNtfAlarmNotificationFilterT alarmFilter;
	SaNtfNotificationTypeFilterHandlesT notificationFilterHandles;
	SaNtfSearchCriteriaT criteria;
	SaNtfReadHandleT readHandle;
	SaNtfNotificationsT returnedNotification;

	int majorId = 0;
	int minorId = 0;
	bool is_default_notify = false;
	majorId = miType/65536;
	minorId = miType%65536;

	error = saNtfInitialize(&ntfHandle,&ntfCallbacks,&version);
	if(SA_AIS_OK != error)
	{
		syslog(LOG_INFO,"saNtfInitialize failed - %s",error_output(error));
		return removeAlarm;
	}

	error = saNtfAlarmNotificationFilterAllocate(ntfHandle,&alarmFilter,numEventTypes,numNotificationObjects,numNotifyingObjects,numNotificationClassIds,
			numProbableCauses,numPerceivedSeverities,numTrends);

	if(error != SA_AIS_OK) 
	{
		syslog(LOG_INFO,"saNtfAlarmNotificationFilterAllocate failed - %s",error_output(error));
		return removeAlarm;
	}

	alarmFilter.notificationFilterHeader.notificationClassIds[0].vendorId = vendorId;
	alarmFilter.notificationFilterHeader.notificationClassIds[0].majorId = majorId;
	alarmFilter.notificationFilterHeader.notificationClassIds[0].minorId = minorId;

	notificationFilterHandles.alarmFilterHandle = alarmFilter.notificationFilterHandle;
	notificationFilterHandles.objectCreateDeleteFilterHandle = SA_NTF_FILTER_HANDLE_NULL;
	notificationFilterHandles.attributeChangeFilterHandle = SA_NTF_FILTER_HANDLE_NULL;
	notificationFilterHandles.stateChangeFilterHandle = SA_NTF_FILTER_HANDLE_NULL;
	notificationFilterHandles.securityAlarmFilterHandle = SA_NTF_FILTER_HANDLE_NULL;

	criteria.searchMode = SA_NTF_SEARCH_AT_OR_AFTER_TIME;
	criteria.eventTime = originalEventTime;

	error = saNtfNotificationReadInitialize(criteria,&notificationFilterHandles,&readHandle);

	if (error != SA_AIS_OK)
	{
		syslog(LOG_INFO,"saNtfNotificationReadInitialize_3 failed - %s",error_output(error));
		return removeAlarm;
	}

	saNtfNotificationFilterFree(alarmFilter.notificationFilterHandle);

	for (; (error = saNtfNotificationReadNext(readHandle,SA_NTF_SEARCH_YOUNGER,&returnedNotification)) == SA_AIS_OK;)
	{
		ntf_classId_present = true;
		SaNtfAlarmNotificationT *returnedANtf;
		const SaNtfNotificationHeaderT * notification_header;
		char notification_object [SA_MAX_NAME_LENGTH + 1] = {0};
		SaNtfNotificationHandleT notification_handle;
		char notifying_object[SA_MAX_NAME_LENGTH + 1] = {0};
		SaNtfNotificationHandleT notificationHandle;
		const SaNtfNotificationHeaderT *notificationHeader;

		notificationHandle = returnedNotification.notification.alarmNotification.notificationHandle;
		notificationHeader = &returnedNotification.notification.alarmNotification.notificationHeader;

		notification_handle = returnedNotification.notification.alarmNotification.notificationHandle;
		notification_header = &returnedNotification.notification.alarmNotification.notificationHeader;
		::strncpy(notification_object, reinterpret_cast<const char *>(notification_header->notificationObject->value),notification_header->notificationObject->length);
		notification_object[notification_header->notificationObject->length] = '\0';

		strncpy(notifying_object, (char *)notificationHeader->notifyingObject->value, notificationHeader->notifyingObject->length);
		notifying_object[notificationHeader->notifyingObject->length] = '\0';

		if( (std::strcmp(notifying_object, "default notifying object")) == 0)
			is_default_notify = true;

		if((alarm.specificProblem == ACS_USAFM_DiskReplication) || (alarm.specificProblem == ACS_USAFM_TimeSynchronizationFault))
		{
			found_Node = false;
			char hostName[256] = {0};

			if (!getToken(hostName, notification_object, "hostName=", ','))
			{
				char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
				::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM:HostName not found in notification_object: \"%s\"", notification_object);
				syslog(LOG_ERR, buffer);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			nodeValue = (strcmp(hostName, "SC-2-1") == 0) ? 1 : 2;
			if(alarm.node == nodeValue)
				found_Node = true;
		}

		returnedANtf = &returnedNotification.notification.alarmNotification;
		SaNtfSeverityT severity = *(returnedANtf->perceivedSeverity);

		SaNtfSeverityT  check_Severity = SA_NTF_SEVERITY_CLEARED;
		if( (severity == check_Severity) && found_Node && !is_default_notify )
		{
			alarm_match = true;
			break;
		}
	}
	saNtfNotificationReadFinalize(readHandle);
	if(!ntf_classId_present || alarm_match)
	{
		removeAlarm = true;
	}
	return removeAlarm;
}

/*! @brief Read the Additioanl text for alarm USAFM_LOG_STREAMING_FAILED
 *
 * @param[out]	hostName		The pointer of the buffer where to write the hostname value
 * @param[out]	remoteServerId	The pointer of the buffer where to write the remoteid value 
 * @param[in]	originalAdditionalText	Additional text of original notification from COM
 * @param[in]   cease true in case the received severity is ceasing
 * @return				true in case of success, false otherwise
 */
bool acs_usa_evtConsumer::getLogmDescription(char *hostName, char *remoteServerId, char *originalAdditionalText, char * originalNotificationDn, bool cease)
{
	bool returnValue = false;
	bool verify = false;
//incase of  raise event compare additional text with remote log server is reachable/remote log server is not reachable
//incase of  cease event compare additional text with remote log server is reachable/remote log server is not reachable/
//								 local log service is not active/local log service is active
	if (cease == true)
	{
		if ((strcmp(originalAdditionalText, "local log service is not active") == 0) ||
		(strcmp(originalAdditionalText, "local log service is activated") == 0) ||
		(strcmp(originalAdditionalText, "remote log server is not reachable") == 0) ||
		(strcmp(originalAdditionalText, "remote log server is reachable") == 0) ||
		(strcmp(originalAdditionalText, "streaming queues back to normal") == 0) ||
		(strcmp(originalAdditionalText, "streaming queues are congesting") == 0))
		verify = true;
	}
	else
	{
		if ((strcmp(originalAdditionalText, "remote log server is not reachable") == 0) ||
		(strcmp(originalAdditionalText, "remote log server is reachable") == 0))
		verify = true;		
	}
	if (verify == true)
	{			
		if (!getToken(hostName, originalNotificationDn, "Hostname=", ','))
		{
			char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
			std::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM:Hostname not found in notification->dn: \"%s\"", originalNotificationDn);
			syslog(LOG_ERR, buffer);
			log.Write(buffer, LOG_LEVEL_ERROR);
			return returnValue;
		}
		if (!getToken(remoteServerId, originalNotificationDn, "remoteLogServerId=", ','))
		{
		char buffer[128] = {0}; buffer[sizeof(buffer) - 1] = 0;
			std::snprintf(buffer, sizeof(buffer) - 1, "ACS_USAFM:remoteServerId not found in notification->dn: \"%s\"", originalNotificationDn);
			syslog(LOG_ERR, buffer);
			log.Write(buffer, LOG_LEVEL_ERROR);
			return returnValue;
		}
		returnValue = true;
	}				
	return returnValue;
}

//******************************************************************************
//
//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

