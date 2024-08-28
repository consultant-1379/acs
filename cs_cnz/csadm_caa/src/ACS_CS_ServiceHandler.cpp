//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

// This class
#include "ACS_CS_ServiceHandler.h"

// Platform
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "ACS_APGCC_Util.H"
#include <arpa/inet.h>
#include <sys/inotify.h>
#include <math.h>
#include <fstream>

#include <boost/filesystem.hpp>

// Ace
#include <ace/Reactor.h>

#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_adminoperation.h"
#include "acs_prc_api.h"

// CS
#include "ACS_CS_TCPClient.h"
#include "ACS_CS_TCPServer.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_HWCHandler.h"
#include "ACS_CS_CPIdHandler.h"
#include "ACS_CS_CPGroupHandler.h"
#include "ACS_CS_FuncDistHandler.h"
#include "ACS_CS_NEHandler.h"
#include "ACS_CS_VLANHandler.h"
#include "ACS_CS_GLKHandler.h"
#include "ACS_CS_MODDHandler.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_ServiceConnection.h"
#include "ACS_CS_KeyValueString.h"
#include "ACS_CS_TableRequest.h"
#include "ACS_CS_Master.h"
#include "ACS_CS_Slave.h"
#include "ACS_CS_Common_Util.h"
#include "ACS_CS_TableOperationWorker.h"
#include "ACS_CS_SubscriptionAgent.h"
#include "ACS_CS_HostFile.h"
#include "ACS_CS_TableHandler.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_SNMPMasterHandler.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Timer.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImObjectCreator.h"
#include "ACS_CS_ImModel.h"
#include "ace/INET_Addr.h"
#include "ACS_CS_Util.h"
#include "acs_apgcc_objectimplementerinterface_V2.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImEntryIdHelper.h"
#include "ACS_CS_ImIPAssignmentHelper.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_TrapHandler.h"
#include "ACS_CS_MethodHandler.h"
#include "ACS_CS_IronsideHandler.h"
#include "ACS_CS_CpClusterManager.h"
#include "ACS_CS_SECCallbackThread.h"
#include "ACS_CS_ComputeResourceHandler.h"
#include "ACS_CS_CRRProxyHandler.h"
#include "ACS_CS_VirtualVLANHandler.h"
#include "ACS_CS_NativeVLANHandler.h"
#include "ACS_CS_NetworkHandler.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_API_SetClusterOpModeChanged.h"

#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_ImCpProductInfo.h"
#include "ACS_CS_FTPAccessHandler.h"
ACS_CS_Trace_TDEF(ACS_CS_ServiceHandler_TRACE);

// using directives
using std::vector;
using std::set;
using std::string;
using std::ostringstream;
using std::stringstream;

//TODO : capire dove spostare questi dati in comune con ACS_CS_API_SetOmProfileNotification
namespace
{
	const ACS_APGCC_AdminOperationIdType OM_PROFILE_NOTIFICATION_SUCCESS = 1;
	const ACS_APGCC_AdminOperationIdType OM_PROFILE_NOTIFICATION_FAILURE = 2;

	const int PHASE_IDLE = -1;
	const int PHASE_VALIDATE = 0;
	const int PHASE_AP_NOTIFY = 1;
	const int PHASE_CP_NOTIFY = 2;
	const int PHASE_COMMIT = 3;

	char const * const ACTION_PHASE_PARAMETER = "phase";
	char const * const ACTION_REASON_PARAMETER = "reason";
	char const * const ACTION_APZ_PARAMETER = "apzProfile";
	char const * const ACTION_APT_PARAMETER = "aptProfile";

	const char * const SUBSCRIPTION_TIMER_ID  = "SubscriptionSupervisionTimer";

	const char * const LOGICALMGMT_CLEANUP_TIMER_ID = "LogicalMgmtCleanupTimer";

	const char PSAClearFilePath[] = "/usr/share/pso/storage-paths/clear";
	const char CsBRFolder[] = "acs-csbin";
	const char DirDelim ='/';	
}

namespace Inotify_Size{
	const size_t EVENT_SIZE = sizeof( struct inotify_event );
	const size_t EVENT_BUF_LEN = ( 64U * ( EVENT_SIZE  ) );
}

//----------------------------------------------
// Internal class to handle hwcsync

ACS_CS_Sync_Thread::ACS_CS_Sync_Thread(ACS_CS_SNMPHandler* handler, std::vector<ACS_CS_DHCP_Info*>* vector):
								snmpHandler(handler),
								syncVector(vector),
								syncSuccess(false)
{}

int ACS_CS_Sync_Thread::exec()
{
	if (snmpHandler && syncVector)
	{
		syncSuccess = snmpHandler->syncClients(*syncVector);
	}
	return 0;
}

//----------------------------------------------
// Internal class to run the ACE reactor

int ACS_CS_ReactorRunner::exec()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ReactorRunner::exec()"));

	while ( ! exit_ )
	{
		//int result = ACE_Reactor::instance()->handle_events();

		ACE_Reactor::instance()->owner(ACE_OS::thr_self());
		int result = ACE_Reactor::instance()->run_reactor_event_loop();

		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ReactorRunner::exec(): "
				"handle_events returning with result=%d", result));

		ACE_Reactor::instance()->reset_reactor_event_loop();

		// TODO
		// Sleep here as extra precaution, should probably be removed
		sleep(1);
	}

	return 0;
}

void ACS_CS_ReactorRunner::deActivate()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ReactorRunner::deActivate()"));

	ACS_CS_Thread::deActivate();
	int result = ACE_Reactor::instance()->end_reactor_event_loop();

	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
			LOG_LEVEL_INFO,
			"ACS_CS_ReactorRunner::deActivate(): "
			"end_reactor_event_loop() returning with result=%d", result));
}

//----------------------------------------------

ACS_CS_ServiceHandler::ACS_CS_ServiceHandler()
: handlerMap(0),
  apzType(0),
  isBladeCluster(false),
  csTcpServer(0),
  moddTcpServer(0),
  shutdownAllEvent(0),
  shutdownHandlerEvent(0),
  removeClosedConnectionsEvent(0),
  immDispatchFailedEvent(0),
  immDispatchFailedSecEvent(0),
  neTable(0),
  hwcTable(0),
  subscriptionAgent(0),
  tableOperations(0),
  isAP1(true),
  thread_running(false),
  OngoingAction(false),
  bootIpAddressEthA(ACS_CS_NS::DEFAULT_BOOT_IP_ETHA),
  bootIpAddressEthB(ACS_CS_NS::DEFAULT_BOOT_IP_ETHB),
  shelfArchitecture(NODE_SCB),
  isDHCP_ON(-1),
  ptbAHandler(0),
  ptbBHandler(0),
  dhcp(0),
  trapHandler(0),
  shelfManager(0),
  immWorker(0),
  cpClusterManager(0),
  reactorRunner(0),
  secLdapWorker(0),
  crrWorker(0),
  m_brfHandler(0),
  m_InotifyFD(-1),
  m_watcherID(-1),
  m_isRestartAfterRestore(false),
  m_clearDataPath(),
  subscriptionSupervisionTimerHandle(ACS_CS_EVENT_RC_ERROR),
  logicalMgmtCleanupTimer(ACS_CS_EVENT_RC_ERROR),
  m_isSubscriptionTimerEnded(false),
  m_NohwcChange(false),
  m_computeResourceHandler(0)
{


	isAP1 = ACS_CS_Protocol::CS_ProtocolChecker::checkIfAP1();

	csTcpServer = new ACS_CS_TCPServer();

	moddTcpServer = new ACS_CS_TCPServer();

	// Check the shelf Architecture
	ACS_CS_Registry::getNodeArchitecture(shelfArchitecture);
	isDHCP_ON=ACS_CS_Registry::getDHCPOnOffFlag();

	if (isDHCP_ON){
		dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);
	}

	// Open event used to signal service shutdown
	shutdownHandlerEvent = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);

	if (shutdownHandlerEvent == ACS_CS_EVENT_RC_ERROR) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandler::ACS_CS_ServiceHandler()"
				"Error: Cannot open shutdown event"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event, "Create Event",
				"Cannot open event", "");
	}

	// Open event used to signal service shutdown
	shutdownAllEvent = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	if (shutdownAllEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandler::ACS_CS_ServiceHandler()"
				"Error: Cannot open shutdown event"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Create Event",
				"Cannot open event",
				"");
	}

	removeClosedConnectionsEvent =
			ACS_CS_Event::CreateEvent(false, false, ACS_CS_NS::EVENT_NAME_REMOVE_CLOSED_CONNECTIONS);

	if (removeClosedConnectionsEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
				"Error: Could not create remove closed connections event"));
	}

	immChangedEvent =
			ACS_CS_Event::CreateEvent(false, false, ACS_CS_NS::EVENT_NAME_IMM_UPDATED);

	if (immChangedEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
				"Error: Could not create imm updated event"));
	}

	immDispatchFailedEvent =
			ACS_CS_Event::CreateEvent(false, false, ACS_CS_NS::EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SERVICE);
	if (immDispatchFailedEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
				"Error: Could not create DISPATCH FAILED SERVICE event"));
	}

	immDispatchFailedSecEvent =
			ACS_CS_Event::CreateEvent(false, false, ACS_CS_NS::EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SEC);
	if (immDispatchFailedSecEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
				"Error: Could not create DISPATCH FAILED SEC event"));
	}

	// Check if blade cluster system
	isBladeCluster = ACS_CS_Registry::isMultipleCPSystem();



	// Create map to hold table handlers
	handlerMap = new maptype();

	// Create thread for the ACE reactor
	reactorRunner = new ACS_CS_ReactorRunner();

	// Table operations class
	tableOperations = new ACS_CS_TableOperation();

	trapHandler= new ACS_CS_TrapHandler(this);

	if (shelfArchitecture == NODE_SCX || shelfArchitecture == NODE_SCB || shelfArchitecture == NODE_SMX )
		shelfManager = new ACS_CS_SNMPMasterHandler(this);
	else if (shelfArchitecture == NODE_DMX)
		shelfManager = new ACS_CS_IronsideHandler(this);

	//Memory will be allocated to crrWorker only when node is MCP and env is virtual
	if (shelfArchitecture == NODE_VIRTUALIZED && isBladeCluster){
			bool result;
			crrWorker = new ACS_CS_CRRProxyHandler(ACS_CS_ImmMapper::classCRR,ACS_CS_ImmMapper::IMPLEMENTER_CRR,ACS_APGCC_ONE );
			if(crrWorker)
			{
				crrWorker->activate();
			}	
	}

	secLdapWorker = new ACS_CS_SECCallbackThread();
	if(isBladeCluster)
	{
	m_brfHandler = ACS_CS_BrfHandler::getInstance();
	        ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
                        "ACS_CS_ServiceHandler::brfcreated"));
	}
	// Initialize inotify object
	m_InotifyFD = inotify_init();

	//create cluster Aligned event
	clusterAlignedTimer = ACS_CS_Timer::CreateTimer(false, ACS_CS_NS::EVENT_NAME_CLUSTER_ALIGNED);
	if (clusterAlignedTimer == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
				"Error: Could not create cluster aligned timer"));
	}

	//static Ip aligned event
	staticIpAlignedTimer = ACS_CS_Timer::CreateTimer(false, ACS_CS_NS::EVENT_NAME_STATICIP_ALIGNED);
	if (staticIpAlignedTimer == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
				"Error: Could not create static aligned timer"));
	}

	subscriptionSupervisionTimerHandle = ACS_CS_Timer::CreateTimer(false, SUBSCRIPTION_TIMER_ID);

    if (subscriptionSupervisionTimerHandle == ACS_CS_EVENT_RC_ERROR)
    {
    	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
    			LOG_LEVEL_ERROR,
    			"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
    			"Error: Could not create subscription timer"));
    }

    if(shelfArchitecture == NODE_VIRTUALIZED) {
    	logicalMgmtCleanupTimer = ACS_CS_Timer::CreateTimer(false, LOGICALMGMT_CLEANUP_TIMER_ID);

    	if (logicalMgmtCleanupTimer == ACS_CS_EVENT_RC_ERROR)
    	{
    		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
    				LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandle::ACS_CS_ServiceHandler()"
					"Error: Could not create timer named 'logicalMgmtCleanupTimer'"));
    	}
    }
}


ACS_CS_ServiceHandler::~ACS_CS_ServiceHandler()
{

	if (ACS_CS_Event::SetEvent(shutdownAllEvent) == 0)
	{// in case the ServiceHandler thread still did not get the shutdown event
		char problemText[512];
		snprintf(problemText, sizeof(problemText) - 1, "[%s]Error: Cannot signal event.", __FUNCTION__);
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "(%t) %s", problemText));
		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Create Event",
				problemText,
				"");
	}


	// Remove finished Service Connection threads
	set<ACS_CS_ServiceConnection*>::iterator connectionIt;
	connectionIt = connectionSet.begin();

	while (connectionIt != connectionSet.end())
	{
		ACS_CS_ServiceConnection *client = *connectionIt;

		if (!client->isFinished())
		{
			// thread has still not closed
			client->terminate();
		}

		// Remove finished threads
		delete client;
		connectionIt++;
	}

	tableOperationWorkerCleanup();
//IPTables cleanup
 	if (hwcTable)
        {
  
            (void) hwcTable->loadTable(); // Load HWC table last 
             ACS_CS_FTPAccessHandler::instance()->disableCPandAPBladesForFTP(hwcTable->getTable());
        }

	
	stopHandlers();
	connectionSet.clear();

	if (ptbAHandler)
	{
		if (!ptbAHandler->isFinished())
		{
			ptbAHandler->wait(3000);
			ptbAHandler->close();
		}
		delete ptbAHandler;
		ptbAHandler = 0;
	}
	if (ptbBHandler)
	{
		if (!ptbBHandler->isFinished())
		{
			ptbBHandler->wait(3000);
			ptbBHandler->close();
		}
		delete ptbBHandler;
		ptbBHandler = 0;
	}

	if (csTcpServer)
	{
		delete csTcpServer;
		csTcpServer = 0;
	}

	if (moddTcpServer) {
		delete moddTcpServer;
		moddTcpServer = 0;
	}

	if (removeClosedConnectionsEvent > 0) {
		ACS_CS_Event::CloseEvent(removeClosedConnectionsEvent);
	}

	if(immChangedEvent > 0)
		ACS_CS_Event::CloseEvent(immChangedEvent);

	if (handlerMap)
	{
		delete handlerMap;
		handlerMap = 0;
	}

	if (reactorRunner)
	{
		delete reactorRunner;
		reactorRunner = 0;
	}

	if (tableOperations)
	{
		delete tableOperations;
		tableOperations = 0;
	}

	if (trapHandler)
	{
		delete(trapHandler);
		trapHandler = 0;
	}

	if(shelfManager)
	{
		delete shelfManager;
		shelfManager = 0;
	}

	if(m_brfHandler !=0 )
	{
		ACS_CS_BrfHandler::getInstance()->finalize();
		m_brfHandler = 0;
	}
	if(cpClusterManager)
	{
		delete cpClusterManager;
		cpClusterManager = 0;
	}

	//Delete Network Handler instance
	ACS_CS_NetworkHandler::finalize();

	//Delete ImRepository instance
	ACS_CS_ImRepository::finalize();

	if(isAP1)
	{
		//Delete Master instance
		ACS_CS_Master::finalize();
	}
	else
	{
		//Delete Slave instance
		ACS_CS_Slave::finalize();
	}

	if (immWorker)
	{
		delete immWorker;
		immWorker = 0;
	}

	if (secLdapWorker)
	{
		delete secLdapWorker;
		secLdapWorker = 0;
	}

	if(crrWorker){
    crrWorker->shutdown();
    crrWorker->wait();
		delete crrWorker;
		crrWorker = 0;
	}

	//Clean DHCP configurator map
	ACS_CS_DHCP_Factory::finalize();

	if(clusterAlignedTimer > 0)
	{
			ACS_CS_Timer::CloseTimer(clusterAlignedTimer);
			clusterAlignedTimer = ACS_CS_EVENT_RC_ERROR;
	}

	if(staticIpAlignedTimer > 0)
	{
			ACS_CS_Timer::CloseTimer(staticIpAlignedTimer);
			staticIpAlignedTimer = ACS_CS_EVENT_RC_ERROR;
	}

	if (subscriptionSupervisionTimerHandle > 0)
	{
		ACS_CS_Timer::CloseTimer(subscriptionSupervisionTimerHandle);
		subscriptionSupervisionTimerHandle = ACS_CS_EVENT_RC_ERROR;
	}

	if(logicalMgmtCleanupTimer > 0)
	{
		ACS_CS_Timer::CloseTimer(logicalMgmtCleanupTimer);
		logicalMgmtCleanupTimer = ACS_CS_EVENT_RC_ERROR;
	}

}

void ACS_CS_ServiceHandler::setNodeSpecificOptions(ACS_CS_DHCP_Info &info)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::setNodeSpecificOptions()"));

	/*assuming that if it is single or multi cp system, is systemwide and does not need to
	 * be specified as argument. So is the active/passive side
	 */

	stringstream bootpath;

	if (!(ACS_CS_Registry::isMultipleCPSystem()))
	{
		switch (info.getSide())
		{
			case ACS_CS_Protocol::Side_B:
				bootpath << ACS_CS_NS::DHCP_CPUBB_BOOT_PATH;
				break;
			default:
				//According to DS "A side" value is default one.
				bootpath << ACS_CS_NS::DHCP_CPUBA_BOOT_PATH;
				break;
		}
	}
	else
	{
		string defCpName;
		ACS_CS_API_Util::getDefaultCPName(info.getSysId(),defCpName);
		bootpath << "/" << defCpName;
		switch(info.getSide())
		{
			case ACS_CS_Protocol::Side_B:
				bootpath << ACS_CS_NS::DHCP_BCB_BOOT_PATH;
				break;
			default:
				//According to DS "A side" value is default one.
				bootpath << ACS_CS_NS::DHCP_BCA_BOOT_PATH;
				break;
		}
	}

	// Set the boot path
	info.setBootPath(bootpath.str());

	uint32_t bootIp = 0;

	// Find out which boot ip to use
	switch(info.getNetwork())
	{
		case ACS_CS_Protocol::Eth_A:
			bootIp = this->bootIpAddressEthA; //ACS_CS_NS::DEFAULT_BOOT_IP_ETHA; //
			break;
		case ACS_CS_Protocol::Eth_B:
			bootIp = this->bootIpAddressEthB; //ACS_CS_NS::DEFAULT_BOOT_IP_ETHB;//
			break;
		default:
			break;
	};

	// Convert boot IP to C-string
	char addr[INET_ADDRSTRLEN];
	uint32_t naddr = htonl(bootIp);
	inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

	// Set the boot IP
	info.setBootServer(addr);
}



bool ACS_CS_ServiceHandler::createDHCPInfo(ACS_CS_DHCP_Info &info,
		const ACS_CS_ImBlade * blade,
		ACS_CS_Protocol::CS_Network_Identifier network, ACS_CS_Protocol::SwitchType switchType)
{
	bool result = true;

	// Network
	info.setNetwork(network);

	// Slot
	info.setSlot(blade->slotNumber);

	// DHCP
	DhcpEnum dhcpMethod = blade->dhcpOption;

	if( (blade->functionalBoardName==SCXB) || (blade->functionalBoardName==EPB1) ||
			(blade->functionalBoardName == EVOET) || (blade->functionalBoardName == CMXB) || (blade->functionalBoardName == SMXB))
	{
		const ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(blade);

		if (otherBlade == NULL)
			return false;

		uint32_t ipActive=0;

		// get the ip of active node and set it
		ipActive = getIpForTftpAddress(network); // ip address is to be used by SCXB or SMXB
		info.setAcAddress(ipActive);

		info.setDHCPMethod(ACS_CS_Protocol::DHCP_Client);

		info.setBladeProductNumber(otherBlade->bladeProductNumber.c_str());
		std::string productIdentity;
		std::string loadModuleFileName;

		if (ACS_CS_ImUtils::getImmAttributeString(otherBlade->currentLoadModuleVersion,"productIdentity",productIdentity))
		{
			info.setCurrentLoadModuleVersion(productIdentity);
		}
		else
		{
			result = false;
		}

		if (ACS_CS_ImUtils::getImmAttributeString(otherBlade->currentLoadModuleVersion,"loadModuleFileName",loadModuleFileName))
		{
			info.setLoadModuleFileName(loadModuleFileName);
		}
		else
		{
			result = false;
		}

		//set trap receiver
		bool smxTrapReceiver=false;
		if(blade->functionalBoardName == SMXB)
			smxTrapReceiver=true;
			
		info.setTrapReceiverAddress(getIpForTftpAddress(network,smxTrapReceiver));
	}
	else
	{
		if (dhcpMethod == DHCP_NONE)
		{
			info.setDHCPMethod(ACS_CS_Protocol::DHCP_None);
		}
		else if (dhcpMethod == DHCP_NORMAL)
		{
			if (blade->functionalBoardName== IPTB || blade->functionalBoardName== IPLB)
			{
				const ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(blade);

				if (otherBlade == NULL)
				{
					//Error
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Cannot fetch otherBlade object",
							__FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "%s", trace));
					return false;
				}

				uint32_t ipActive=0;

				// get the ip of active node and set it
				ipActive = getIpForTftpAddress(network);
				info.setAcAddress(ipActive);
				info.setDHCPMethod(ACS_CS_Protocol::DHCP_Normal);

				if (shelfArchitecture != NODE_VIRTUALIZED){
					std::string loadModuleFileName;
					{
						//Debug trace
						char trace[256] = {0};
						snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Reading loadModuleFileName attribute for object==%s",
								__FILE__, __FUNCTION__, __LINE__, otherBlade->currentLoadModuleVersion.c_str());
						std::cout << trace << std::endl;
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "%s", trace));
					}
					if (ACS_CS_ImUtils::getImmAttributeString(otherBlade->currentLoadModuleVersion,"loadModuleFileName",loadModuleFileName))
					{
						char trace[256] = {0};
						snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Read IPT LoadModule==%s", __FILE__, __FUNCTION__, __LINE__, loadModuleFileName.c_str());
						std::cout << trace << std::endl;
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "%s", trace));
						info.setLoadModuleFileName(loadModuleFileName);
					}
					else
					{
						char trace[256] = {0};
						snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] ACS_CS_ImUtils::getImmAttributeString failed. Cannot fetch IPT LoadModule.", __FILE__, __FUNCTION__, __LINE__);
						std::cout << trace << std::endl;
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "%s", trace));
						result = false;
					}
				}
			}
			else
			{
				//Double Sided CP Board
				info.setDHCPMethod(ACS_CS_Protocol::DHCP_Normal);
				uint32_t ipActive=0;
				ipActive = getIpForTftpAddress(network); // ip address is to be used by SCXB
				info.setAcAddress(ipActive);
			}

			char mac[16] = {0};
			int length = 0;

			//Use the MAC addresses on IMM (if provided)
			if (info.getNetwork() == ACS_CS_Protocol::Eth_A)
			{
				length = ACS_CS_Registry::hexMACtoDecMAC(blade->macAddressEthA.c_str(), mac);
			}
			else
			{
				length = ACS_CS_Registry::hexMACtoDecMAC(blade->macAddressEthB.c_str(), mac);
			}

			if (length > 0)
			{
				std::string macString = ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*) mac, 6);

				ACS_CS_DEBUG(("DEBUG: MAC String: %s", macString.c_str()));

				info.setMAC(mac, length);
			}

		}
		else if (dhcpMethod == DHCP_CLIENT)
		{
			info.setDHCPMethod(ACS_CS_Protocol::DHCP_Client);
			uint32_t ipActive=0;
			ipActive = getIpForTftpAddress(network); // ip address is to be used by SCXB OR SMXB
			info.setAcAddress(ipActive);
		}
		else
			info.setDHCPMethod(ACS_CS_Protocol::DHCP_NotSpecified);
	}

	// Side
	SideEnum side = blade->side;

	if (side == A)
	{
		info.setSide(ACS_CS_Protocol::Side_A);
	}
	else if (side == B)
	{
		info.setSide(ACS_CS_Protocol::Side_B);
	}
	else
	{
		info.setSide(ACS_CS_Protocol::Side_NotSpecified);
	}

	// FBN
	FunctionalBoardNameEnum functionalBoardName = blade->functionalBoardName;
	ACS_CS_Protocol::CS_FBN_Identifier fbn = ACS_CS_Protocol::FBN_NotSpecified;

	if (functionalBoardName == SCB_RP)
		fbn = ACS_CS_Protocol::FBN_SCBRP;
	else if (functionalBoardName == RPBI_S)
		fbn = ACS_CS_Protocol::FBN_RPBIS;
	else if (functionalBoardName == GESB)
		fbn = ACS_CS_Protocol::FBN_GESB;
	else if (functionalBoardName == CPUB)
		fbn =ACS_CS_Protocol::FBN_CPUB;
	else if (functionalBoardName == SCXB)
		fbn =ACS_CS_Protocol::FBN_SCXB;
	else if (functionalBoardName == IPTB)
		fbn =ACS_CS_Protocol::FBN_IPTB;
	else if (functionalBoardName == EPB1)
		fbn =ACS_CS_Protocol::FBN_EPB1;
	else if (functionalBoardName == EVOET)
		fbn =ACS_CS_Protocol::FBN_EvoET;
	else if (functionalBoardName == CMXB)
		fbn =ACS_CS_Protocol::FBN_CMXB;
	else if (functionalBoardName ==IPLB)
		fbn =ACS_CS_Protocol::FBN_IPLB;
	else if (functionalBoardName ==SMXB)
		fbn =ACS_CS_Protocol::FBN_SMXB;


	info.setFBN(fbn);

	// IP address
	string ipAddressString;

	uint32_t ip=0;
	if ((ACS_CS_Protocol::Eth_A == network) && (ACS_CS_Protocol::TRANSPORT_SWITCH != switchType))
	{
		inet_pton(AF_INET, blade->ipAddressEthA.c_str(), &ip);
	}
	else if ((ACS_CS_Protocol::Eth_A == network) && (ACS_CS_Protocol::TRANSPORT_SWITCH == switchType))
	{
		inet_pton(AF_INET, blade->ipAliasEthA.c_str(), &ip);
	}
	else if ((ACS_CS_Protocol::Eth_B == network) && (ACS_CS_Protocol::TRANSPORT_SWITCH != switchType))
	{
		inet_pton(AF_INET, blade->ipAddressEthB.c_str(), &ip);
	}
	else if ((ACS_CS_Protocol::Eth_B == network) && (ACS_CS_Protocol::TRANSPORT_SWITCH == switchType))
	{
		inet_pton(AF_INET, blade->ipAliasEthB.c_str(), &ip);
	}
	if(ip)
	{
		ip = ntohl(ip);
		info.setIP(ip);
	}
	else
	{
		info.setIP(0);
	}	

	// Sys type
	SystemTypeEnum systemType = blade->systemType;

	if (systemType == SINGLE_SIDED_CP)
	{
		info.setSystemType(ACS_CS_Protocol::SysType_BC);
	}
	else if (systemType == DOUBLE_SIDED_CP)
	{
		info.setSystemType(ACS_CS_Protocol::SysType_CP);
	}
	else if (systemType == AP)
	{
		info.setSystemType(ACS_CS_Protocol::SysType_AP);
	}
	else
	{
		info.setSystemType(ACS_CS_Protocol::SysType_NotSpecified);
	}

	// Magazine
	const ACS_CS_ImBase * parent = ACS_CS_ImRepository::instance()->getParentOf(blade->rdn);
	if (parent)
	{
		const ACS_CS_ImShelf * shelf = dynamic_cast<const ACS_CS_ImShelf *>(parent);

		if (shelf)
		{
			string magazine = shelf->address;
			struct in_addr convaddr;
			memset(&convaddr, 0, sizeof convaddr);
			inet_pton(AF_INET, magazine.c_str(), &convaddr);
			uint32_t magAddress = convaddr.s_addr;
			info.setMagAddress(magAddress);
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}

	// Sys Id
	systemType = blade->systemType;
	ACE_UINT16 systemNumber = blade->systemNumber;
	int32_t sequenceNumber = blade->sequenceNumber;
	uint16_t sysId = 0;

	if (systemType == SINGLE_SIDED_CP)
	{
		sysId = ACS_CS_Protocol::SysType_BC + sequenceNumber;
	}
	else if (systemType == DOUBLE_SIDED_CP)
	{
		sysId = ACS_CS_Protocol::SysType_CP + systemNumber;
	}
	else if (systemType == AP)
	{
		sysId = ACS_CS_Protocol::SysType_AP + systemNumber;
	}

	info.setSysId(sysId);

	// Update
	setNodeSpecificOptions(info);

	// CLINET ID

	// skip the info, if it is both EPB1 type and the product id is not still fetched

	if (info.getFBN() != ACS_CS_Protocol::FBN_EPB1 &&
			info.getFBN() != ACS_CS_Protocol::FBN_EvoET &&
			info.getFBN() != ACS_CS_Protocol::FBN_CMXB)
	{
		info.calcOption61Id(fbn, switchType);		
	}
	
	info.setHostName(info.generateHostname(switchType));

	return result;
}


void ACS_CS_ServiceHandler::abort(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId ccbId)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::abort(), ccbId=%d", ccbId));

	//cout << "==========================================================" << endl;
	cout << "========== ABORT CALLBACK ================================" << endl;
	//cout << "==========================================================" << endl;

	if(m_computeResourceHandler) {
		m_computeResourceHandler->abortNetworkHandlerDeletionTransaction(ccbId);
	}

	ACS_CS_ImRepository::instance()->disposeTransaction(ccbId);

	//tableOperations->disposeOperation(ccbId);
}

void ACS_CS_ServiceHandler::apply(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId ccbId)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ServiceHandler::apply()"
				"args: ccbId = %d", ccbId));
	//cout << "==========================================================" << endl;
	cout << "========== APPLY CALLBACK ================================" << endl;
	//cout << "==========================================================" << endl;

	// Get the current (and now applied) operation
	const ACS_CS_ImModel * operation = ACS_CS_ImRepository::instance()->getSubset(ccbId);

	////////////////////////////////////////////////

	if (isAP1) // If we are the master CS
	{
		//Look for object that may need deletion/////////////////////////////////
		bool islogicalMgmtCleanupNeeded_native = false;
		bool islogicalMgmtCleanupNeeded_virtual = false;

		if (operation)
		{
			if (shelfArchitecture != NODE_VIRTUALIZED)
			{
				std::set<const ACS_CS_ImBase*> obj;
				operation->getObjects(obj,CPBLADE_T);
				if (obj.size() > 0 && ACS_CS_Registry::getAPZType() == 4){ //Skip Logical object deletion for classic configurations
					std::cout<< "Found CP blade in operation with APZType:"<<ACS_CS_Registry::getAPZType()<<" -> cleanUpNeeded = true."<<std::endl;
					islogicalMgmtCleanupNeeded_native = true;
				}
				else {
					obj.clear();
					operation->getObjects(obj,APBLADE_T);
					if (obj.size() > 0){
						islogicalMgmtCleanupNeeded_native = true;
						std::cout<< "Found AP blade in operation with APZType:"<<ACS_CS_Registry::getAPZType()<<" -> cleanUpNeeded = true."<<std::endl;
					}
				}
			}
			else if(shelfArchitecture == NODE_VIRTUALIZED && ACS_CS_Registry::getAPZType() == 4)
			{
				//If the current CcbId transaction has deletion of any of the CpBlades, check if DualSidedCp=<> MOs & CpCluster=1 MOs
				// are empty w.r.t their references to CpBlade MOs and ClusterCp MOs. If empty, set timer for cleanup of LogicalMgmt MO
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - Checking if LogicalMgmt cleanup is needed", ccbId));
				std::set<const ACS_CS_ImBase*> setOfCpBladeObjsInCCb;
				operation->getObjects(setOfCpBladeObjsInCCb,CPBLADE_T);
				if (setOfCpBladeObjsInCCb.size() > 0)
				{
					std::set<const ACS_CS_ImBase*>::iterator iter = setOfCpBladeObjsInCCb.begin();
					for(; iter != setOfCpBladeObjsInCCb.end(); iter++)
					{
						const ACS_CS_ImBase* cpBladeObj = dynamic_cast<const ACS_CS_ImBase*>(*iter);
						if(cpBladeObj && cpBladeObj->action == ACS_CS_ImBase::DELETE)
						{
							ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();

							std::set<const ACS_CS_ImBase*> setOfDualSidedCpObjs;
							model->getObjects(setOfDualSidedCpObjs, DUALSIDEDCP_T);

							std::set<const ACS_CS_ImBase*>::iterator it;

							for (it = setOfDualSidedCpObjs.begin(); it != setOfDualSidedCpObjs.end(); it++)
							{
								const ACS_CS_ImBase* dualSidedCpObj = *it;
								const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(dualSidedCpObj);

								if (cp && cp->axeCpBladeRefTo.size() == 0)
								{
									ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - DualSidedCp has NO references, set timer for LogicalMgmt cleanup", ccbId));
									islogicalMgmtCleanupNeeded_virtual = true;
									break;
								}
								else
									ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - DualSidedCp has references", ccbId));
							}

							if(!islogicalMgmtCleanupNeeded_virtual)
							{
								const ACS_CS_ImBase * cpClusterBase = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());
								if (cpClusterBase)
								{
									const ACS_CS_ImCpCluster* cpClusterObj = dynamic_cast<const ACS_CS_ImCpCluster*>(cpClusterBase);
									ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - cpClusterBase - allBcSize == %d && opgroupSize == %d", ccbId, cpClusterObj->allBcGroup.size(), cpClusterObj->operativeGroup.size()));
									if (cpClusterObj && cpClusterObj->allBcGroup.size() == 0)
									{
										ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - CpCluster's allBC has no blade references, set timer for LogicalMgmt cleanup", ccbId));
										islogicalMgmtCleanupNeeded_virtual = true;
									}
									else
									{
										ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - internal model's allBcGroup size == %d; no cleanup needed for CpCluster=1", ccbId, cpClusterObj->allBcGroup.size()));
									}
								}
							}
							// Break after checking 1 CpBlade with DELETE action in this operation/CcbId
							// because our check for cleanup necessity is based on overall ACS_CS_ImRepository's model DualSidedCp & CpCluster state
							// which already includes the modification/deletion of other CpBlades in this CcbId
							break;
						}
					}
				}
			}



			if (isDHCP_ON)
			{
				handleDHCPTable(operation);
			}
			if (shelfArchitecture == NODE_VIRTUALIZED){
				handleTFTPArea(operation);
			}

			// Replicate to slaves...
			ACS_CS_Master::sendOperation(operation);

			//...and notify Table Subscribers
			const ACS_CS_ImModelSubset* subset = dynamic_cast<const ACS_CS_ImModelSubset *>(operation);
			if (subset)
			{
				if ((!isSubscriptionTimerEnded())&&(!m_NohwcChange))
				{
					//store the subset and postpone the notification
					const ACS_CS_ImModelSubset* storeSubset =  new ACS_CS_ImModelSubset(*subset);
					if (storeSubset)
					{
						tableSubsetsLock.start_writing();
						tableSubsets.push_back(storeSubset);
						tableSubsetsLock.stop_writing();

						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
								LOG_LEVEL_INFO,
								"ACS_CS_ServiceHandler::apply(), Subscription timer does not expired yet, transaction(ccbId=%d) stored.", ccbId));
					}
				}
				if (isSubscriptionTimerEnded())
				{
					newTableOperationRequest(subset);
				}


				//Notify ComputeResource Handler, regardless the subscription timer status
				if (m_computeResourceHandler)
				{
					std::set<const ACS_CS_ImBase*> setOfComputeResourceObjs;
					operation->getObjects(setOfComputeResourceObjs,COMPUTERESOURCE_T);
					if(setOfComputeResourceObjs.size() > 0)
					{
						bool deletedComputeResourcesFoundInCcb = false;
						std::set<const ACS_CS_ImBase*>::iterator iter = setOfComputeResourceObjs.begin();
						for(; iter != setOfComputeResourceObjs.end(); iter++) //TODO: for loop break condition had wrongly obj.begin() until 20 Jul. Have to see if fixing it causes issues
						{
							const ACS_CS_ImBase* computeResourceInCcb = dynamic_cast<const ACS_CS_ImBase*>(*iter);
							if(computeResourceInCcb && computeResourceInCcb->action == ACS_CS_ImBase::DELETE)
							{
								// DELETED ComputeResources related async cleanup operations are triggered via MethodHandler.remove() and are augmented in current Ccb.
								// But the other type of operations on ComputeResource MOs still go through design base behavior i.e. async operation in new thread spawned from newTableOperationRequest
								deletedComputeResourcesFoundInCcb = true;
								ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - Removed ComputeResource DN == %s - would not be added to async operations after apply()", ccbId,computeResourceInCcb->rdn.c_str()));
							}
						}
						newTableOperationRequest(subset, m_computeResourceHandler);
						if(deletedComputeResourcesFoundInCcb)
						{
							m_computeResourceHandler->storeComputeResourcesOnDisk();
							m_computeResourceHandler->applyNetworkHandlerDeletionTransaction(ccbId);
						}
					}
				}
			}
			
			//store static IP in disk
			ACS_CS_ImHardwareMgmt* hw = dynamic_cast<ACS_CS_ImHardwareMgmt*>(operation->getObject(ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));
			if(hw)
			{
				std::cout<< "Store Static Ip in disk...\n";
				ACS_CS_ImIPAssignmentHelper helper;
				helper.storeStaticIpToDisk(operation);
			}

		}

		if(shelfArchitecture != NODE_VIRTUALIZED)
		{
			if(islogicalMgmtCleanupNeeded_native)
				immUpdated();
		}
		else if(shelfArchitecture == NODE_VIRTUALIZED)
		{
			if(islogicalMgmtCleanupNeeded_virtual)
				startLogicalMgmtCleanupTimer();
		}

	}
	else // If we are a Slave CS
	{
		// Just notify Table Subscribers
		if (operation)
		{
			const ACS_CS_ImModelSubset* subset = dynamic_cast<const ACS_CS_ImModelSubset *>(operation);
			if (subset)
				newTableOperationRequest(subset);

		}
	}

	//Update CP product Infos
	if (operation)
	{
		std::set<const ACS_CS_ImBase*> obj;
		std::set<const ACS_CS_ImBase*>::iterator it;
		operation->getObjects(obj,CPPRODUCTINFO_T);

		for (it = obj.begin(); it != obj.end(); it++)
		{
			const ACS_CS_ImBase *object = dynamic_cast<const ACS_CS_ImBase *> (*it);

			if (object->action == ACS_CS_ImBase::DELETE)
				updateProductInfoAttr((ACS_CS_ImUtils::getParentName(object)).c_str(), true);
			else
				updateProductInfoAttr((ACS_CS_ImUtils::getParentName(object)).c_str());
		}
	}

	if (isAP1 && cpClusterManager)
	{
		if (!cpClusterManager->isClusterAligned())
		{
			if (operation)
			{
				//check for ClusterCp object in the current transaction
				std::set<const ACS_CS_ImBase*> obj;
				operation->getObjects(obj,CLUSTERCP_T);
				if (obj.size() >0)
				{
					//set event to align cluster after restore procedure
					startClusterAlignedTimer();
				}
			}
		}
		else
		{
			//Update QuorumData File
			if (operation)
			{
				cpClusterManager->storeCpClusterOnDisk(operation);
			}
		}
	}

	// Remove this operation since it's applied now
	ACS_CS_ImRepository::instance()->disposeTransaction(ccbId);

	if (neTable)
		neTable->immDataSynchronized = true;
	if (m_NohwcChange)
		m_NohwcChange =false;
}

ACS_CC_ReturnType ACS_CS_ServiceHandler::complete(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId ccbId)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::complete() args: ccbId = %d", ccbId));

	cout << "==========================================================" << endl;
	cout << "========== COMPLETE CALLBACK ============================ " << endl;
	cout << "==========================================================" << endl;

	int errNo = 0;
	bool result = false;

	if (isAP1) // We are the Master (AP1)
	{
		result = ACS_CS_ImRepository::instance()->completeTransaction(ccbId, errNo);
	}

	if ( ! isAP1) // If we are a slave (not running on AP1) we need to send the complete operation
	{			  // to the master before it's applied here

		// Get the complete, but not yet applied, operation
		ACS_CS_ImModel * operation = const_cast<ACS_CS_ImModel *>(ACS_CS_ImRepository::instance()->getSubset(ccbId));

		if ( ! ACS_CS_Slave::isUpdatingTables(operation)	// Don't send to master while updating tables
		&& ! ACS_CS_Slave::isUpdatingEntry(operation)) // also not when updating individual entries
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::complete() calling sendToMaster function, MASTER Validation is REQUIRED !!! args: ccbId = %d", ccbId));

			//check if the slave is ready to send the request to master
			if (ACS_CS_Slave::isReadyForRequest())
			{
				// And send it to the master
				result = ACS_CS_Slave::sendToMaster(operation);

				if(result)
				{
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::complete() ACS_CS_Slave::sendToMaster() returned OK"));

					if(!ACS_CS_ImRepository::instance()->completeTransaction(ccbId, errNo))
					{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,
								"ACS_CS_ServiceHandler::complete() ERROR: ACS_CS_ImRepository::instance()->completeTransaction returned failure"));
					}
				}
				else
				{
					errNo = TC_SENDTOMASTERFAILED;
				}
			}
			else
			{
				errNo = TC_SENDTOMASTERFAILED;
			}
		}
		else
		{
			//result = ACS_CS_ImRepository::instance()->completeTransaction(ccbId, errNo);
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::complete() NO MASTER Validation is REQUIRED !!! args: ccbId = %d", ccbId));
			ACS_CS_ImRepository::instance()->applySubset(operation);
			result = true;
		}
	}

	if(result)
	{
		this->setExitCode(0);
		return ACS_CC_SUCCESS;

	} else
	{
		if(errNo == 0)
			errNo = TC_COMPLETIONFAULT;

		
		this->setExitCode(errNo, ACS_CS_ImUtils::getErrorText(errNo));
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandler::complete() Failure in completing transaction = %d Error %d: %s",
				ccbId, errNo, ACS_CS_ImUtils::getErrorText(errNo).c_str()));
		m_NohwcChange=false;
		return ACS_CC_FAILURE;
	}
}


ACS_CC_ReturnType ACS_CS_ServiceHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		const char *className,
		const char *parentName,
		ACS_APGCC_AttrValues **attr)
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - objName == %s", ccbId, className));
	cout << "==========================================================" << endl;
	cout << "========== CREATE CALLBACK FOR " << className << endl;
	cout << "==========================================================" << endl;

	ACS_CS_MethodHandler methodHandler;

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::create() args: ccbId = %d, parentName = %s",ccbId,parentName != 0 ? parentName : "0"));
	int RC = 17;
	//ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;

	bool internalError = false;
	ACS_CS_ImBase *object;
	std::string strClassName(className);
	if(strClassName.compare(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE) == 0)	// Start of TR HY60399
	{
		ACS_CS_ImBase * baseObject = ACS_CS_ImObjectCreator::createImBaseObject(className, parentName, attr);
		if(baseObject == NULL)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::create()",
					"Failed to create temporary ImBaseObject. Classname = %s Parent = %s", className, parentName));
			internalError = true;
		}
		else
		{
			const ACS_CS_ImComputeResource *computeResource = dynamic_cast<const ACS_CS_ImComputeResource *>(baseObject);
			if(computeResource)
			{
				if(computeResource->crType == IPLB_TYPE)
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_TRACE, "ACS_CS_ServiceHandler::create() "
							"ComputeResource->crType is IPLB_TYPE - Check for Default IPLB SW on ACTIVE node"));
					bool isValid = isIplbCrObjCreationAllowed();
					if(isValid)
					{
						internalError = false;
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_WARN, "ACS_CS_ServiceHandler::create() "
								"isIplbCrObjCreationAllowed() -> returned TRUE! Allow IPLB ComputeResource creation!"));
					}
					else
					{
						internalError = true;	// Default IPLB SW not present or node state is NOT ACTIVE
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_WARN, "ACS_CS_ServiceHandler::create() "
								"isIplbCrObjCreationAllowed() -> returned FALSE! Do not allow IPLB ComputeResource creation!"));
					}
				}
			}
			else	//Undefined behavior - should not occur as class name is already matching ComputeResource class
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::create() "
						"ImBase object could not be cast to ComputeResource type!"));
			}
			if(baseObject)
				delete baseObject;
		}
		if(internalError)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::create() "
					"Error: Failed to create imRepository object!!!"));
			this->setExitCode(RC, "CREATION FAILURE: FAILED TO CREATE REPOSITORY OBJECT (internal error)");
			return ACS_CC_FAILURE;
		}
	}	// end of TR HY60399

	object = ACS_CS_ImRepository::instance()->createObject(ccbId, className, parentName, attr);

	if (object == NULL)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandler::create() "
				"Error: Failed to create imRepository object!"));
		this->setExitCode(RC, "CREATION FAILURE: FAILED TO CREATE REPOSITORY OBJECT (internal error)");
		return ACS_CC_FAILURE;
	}
	else
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::create() Incoming object is of type %s with rdn %s", ACS_CS_ImUtils::getClassName(object->type).c_str(), object->rdn.c_str()));

	//Trigger augmentation only if the transaction has been initiated on this AP
	if ((isAP1 && !ACS_CS_Master::isIncomingEntry(object)) || //We are on AP1 and current entry is not coming from a slave
			(!isAP1 && !(ACS_CS_Slave::isUpdatingTables(object) || ACS_CS_Slave::isUpdatingEntry(object)))) //We are on APn and current entry is not coming from the master
	{
		methodHandler.setObject(oiHandle,ccbId,className,parentName,attr,object);

		int error = methodHandler.create();
		if (error != TC_NOERROR) {

			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::create() "
					"Error in creating object in transaction = %d Error %d: %s!", ccbId, error, ACS_CS_ImUtils::getErrorText(error).c_str()));
			this->setExitCode(error, ACS_CS_ImUtils::getErrorText(error));
			return ACS_CC_FAILURE;
		}
	}

	this->setExitCode(0);
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_CS_ServiceHandler::deleted(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::deleted() args: ccbId = %d, objName = %s",ccbId,objName != 0 ? objName : "0"));
	ACS_CS_MethodHandler methodHandler;
	if(m_computeResourceHandler)
		methodHandler.setComputeResourceHandlerObj(m_computeResourceHandler);

	ACS_CS_ImRepository::instance()->deleteObject(ccbId, objName);

	const ACS_CS_ImModel *subset =  ACS_CS_ImRepository::instance()->getSubset(ccbId);


	if (subset) {
		ACS_CS_ImBase *object = subset->getObject(objName);

		if (object) {

			//Trigger augmentation only if the transaction has been initiated on this AP
			if ((isAP1 && !ACS_CS_Master::isIncomingEntry(object)) || //We are on AP1 and current entry is not coming from a slave
					(!isAP1 && !(ACS_CS_Slave::isUpdatingTables(object) || ACS_CS_Slave::isUpdatingEntry(object)))) //We are on APn and current entry is not coming from the master
			{
				methodHandler.setObject(oiHandle,ccbId,"","",0,object);
				int error = methodHandler.remove();
				if (error != 0)
				{
					this->setExitCode(error,ACS_CS_ImUtils::getErrorText(error));
					return ACS_CC_FAILURE;
				}
			}
		}
	} else
	{
		return ACS_CC_FAILURE;
	}

	this->setExitCode(0);
	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType ACS_CS_ServiceHandler::modify(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName,
		ACS_APGCC_AttrModification **attrMods)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::modify() objName = %s", objName != 0 ? objName : "0"));
	ACS_CS_MethodHandler methodHandler;

	bool result = ACS_CS_ImRepository::instance()->modifyObject(ccbId, objName, attrMods);

	if(!result)
		return ACS_CC_FAILURE;


	//	ACS_APGCC_CcbHandle ccbHandleVal;
	//	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	//	ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);
	//
	//	if (retVal != ACS_CC_SUCCESS)
	//	{
	//		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
	//				"ACS_CS_ServiceHandler::create() "
	//				"Error: getCcbAugmentationInitialize() failed %d - %s ",
	//				getInternalLastError(), getInternalLastErrorText()));
	//		return ACS_CC_FAILURE;
	//	}


	bool sideMod = false;
	bool clmMod = false;
	bool isomProfileSupervisionTimeout = false;
	bool iscancelClusterOpModeSupervisionTimer = false;//HZ17022

	for(int i = 0; attrMods[i] != NULL; i++)
	{
		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if(strcmp(modAttr->attrName, "side") == 0)
		{
			sideMod = true;
		}
		else if (strcmp(modAttr->attrName, "currentLoadModuleVersion") == 0)
		{
			clmMod = true;
		}
		else if (strcmp(modAttr->attrName, "omProfileSupervisionTimeout") == 0)
		{
			isomProfileSupervisionTimeout = true;
		}
		else if (strcmp(modAttr->attrName, "cancelClusterOpModeSupervision") == 0)//HZ17022
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_TRACE,"ACS_CS_ServiceHandler::modify() cancelClusterOpModeSupervision is modified"));
			iscancelClusterOpModeSupervisionTimer = true;
		}
	}

	const ACS_CS_ImModel *subset =  ACS_CS_ImRepository::instance()->getSubset(ccbId);

	ACS_CS_ImBase *object = 0;

	if (subset) {
		object = subset->getObject(objName);
	}

	if(!object)
		return ACS_CC_FAILURE;

	//Trigger augmentation only if the transaction has been initiated on this AP
	if ((isAP1 && !ACS_CS_Master::isIncomingEntry(object)) || //We are on AP1 and current entry is not coming from a slave
			(!isAP1 && !(ACS_CS_Slave::isUpdatingTables(object) || ACS_CS_Slave::isUpdatingEntry(object)))) //We are on APn and current entry is not coming from the master
	{

		if (shelfArchitecture != NODE_VIRTUALIZED)
		{
			if (object && (object->type == CPBLADE_T || object->type == OTHERBLADE_T)) {

				ACS_CS_ImBlade* blade = dynamic_cast <ACS_CS_ImBlade*> (object);

				if (blade) {
					if (blade->functionalBoardName == CPUB || ((blade->functionalBoardName == EPB1 ||
							blade->functionalBoardName == EVOET || blade->functionalBoardName == CMXB ||
							blade->functionalBoardName == IPTB || blade->functionalBoardName == IPLB) && clmMod))
					{
						blade->macAddressEthA = "-";
						blade->macAddressEthB = "-";
					}

					//IPLB side is changed, DHCP reservation need to be updated
					if (blade->functionalBoardName == IPLB && sideMod)
					{
						blade->macAddressEthA = "-";
						blade->macAddressEthB = "-";
					}
				}
			}
		}

		if ((shelfArchitecture == NODE_VIRTUALIZED) && (!isSubscriptionTimerEnded()))
		{
			m_NohwcChange=false;
			if (object && ((object->type == CPBLADE_T)||(object->type == APBLADE_T)||(object->type == OTHERBLADE_T)))
			{
		        std::set<const ACS_CS_ImBase *>::const_iterator it_old;
			    const ACS_CS_ImBlade *currentBlade = 0;
			    const ACS_CS_ImBlade *oldBlade = 0;
				currentBlade = dynamic_cast<const ACS_CS_ImBlade *>(object);
				const ACS_CS_ImModel *oldModel = ACS_CS_ImRepository::instance()->getModelCopy();
				std::set<const ACS_CS_ImBase *> OldBladeobj ;
				if(oldModel && currentBlade)
				  {
					if(object->type == CPBLADE_T)
					oldModel->getObjects(OldBladeobj, CPBLADE_T);
					if (object->type == APBLADE_T)
					oldModel->getObjects(OldBladeobj, APBLADE_T);
					if (object->type == OTHERBLADE_T)
					oldModel->getObjects(OldBladeobj, OTHERBLADE_T);
					    if (OldBladeobj.size() > 0)
					    {
					        for (it_old = OldBladeobj.begin(); it_old != OldBladeobj.end(); it_old++)
					    	{
					        	oldBlade = dynamic_cast<const ACS_CS_ImBlade *>(*it_old);
					    		if(oldBlade && (oldBlade->uuid == currentBlade->uuid))
					    		{
					    			if((oldBlade->systemNumber == currentBlade->systemNumber )&&
					    			   (oldBlade->systemType == currentBlade->systemType )&&
								       (oldBlade->side == currentBlade->side )&&
									   (oldBlade->sequenceNumber == currentBlade->sequenceNumber )&&
									   (oldBlade->functionalBoardName == currentBlade->functionalBoardName )&&
									   (oldBlade->ipAddressEthA == currentBlade->ipAddressEthA )&&
									   (oldBlade->ipAddressEthB == currentBlade->ipAddressEthB )&&
									   (oldBlade->dhcpOption == currentBlade->dhcpOption)&&
									   (oldBlade->slotNumber == currentBlade->slotNumber)&&
									   (oldBlade->ipAliasEthA == currentBlade->ipAliasEthA)&&
									   (oldBlade->ipAliasEthB == currentBlade->ipAliasEthB)&&
									   (oldBlade->aliasNetmaskEthA == currentBlade->aliasNetmaskEthA)&&
									   (oldBlade->aliasNetmaskEthB == currentBlade->aliasNetmaskEthB))
					    			{
					    				m_NohwcChange=true;
					    			}
					    		}
					    	}
					    }
					    delete oldModel;
					}

			}
		}
		/*start of HZ17022*/
		if (object && (object->type == ADVANCEDCONFIGURATION_T) && iscancelClusterOpModeSupervisionTimer){
			iscancelClusterOpModeSupervisionTimer = false;
			ACS_CS_ImAdvancedConfiguration * advancedConfigurationMO = dynamic_cast <ACS_CS_ImAdvancedConfiguration*> (object);
			if(advancedConfigurationMO){
				if(advancedConfigurationMO->cancelClusterOpModeSupervision == true){
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_TRACE,"ACS_CS_ServiceHandler::modify() cancelClusterOpModeSupervision == true"));
					if(neTable){
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_TRACE,"NEHANDLER is created cancelClusterOpModeSupervisionTimer cancelling it as ADH notified that ADH operation is completed "));
					neTable->cancelClusterOpModeSupervisionTimer();
					}
					else{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_TRACE,"NEHANDLER is not yet created so NOT calling cancelClusterOpModeSupervisionTimer"));
					}
				}
			}
		}/*end of HZ17022*/
//		if(sideMod)
//		{
//			ACS_APGCC_CcbHandle ccbHandleVal;
//			ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
//			ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);
//
//			if (retVal != ACS_CC_SUCCESS)
//			{
//				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//						"ACS_CS_ServiceHandler::create() "
//						"Error: getCcbAugmentationInitialize() failed %d - %s ",
//						getInternalLastError(), getInternalLastErrorText()));
//				return ACS_CC_FAILURE;
//			}
//
//
//			ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
//			ACS_CS_ImBase *base = model->getObject(objName);
//
//			if(base && APBLADE_T == base->type)
//			{
//				ACS_CS_ImApBlade* blade = dynamic_cast<ACS_CS_ImApBlade*>(base);
//
//				if (blade && blade->functionalBoardName == APUB) {
//					ACS_CS_ImIPAssignmentHelper helper;
//
//					if(helper.assignIp(model, blade))
//					{
//						ACS_CS_ImApBlade *newObject = reinterpret_cast<ACS_CS_ImApBlade *>(model->getObject(objName));
//						if(newObject)
//						{
//							ACS_CS_ImRepository::instance()->replaceObject(newObject->clone());
//
//							ACS_CC_ImmParameter ipEthAParam;
//							ACS_CC_ImmParameter ipEthBParam;
//							char *ipA = strdup(newObject->ipAddressEthA.c_str());
//							char *ipB = strdup(newObject->ipAddressEthB.c_str());
//
//							char ipAddressEthAstr[] = "ipAddressEthA";
//							ipEthAParam.attrName = ipAddressEthAstr;
//							ipEthAParam.attrType = ATTR_STRINGT;
//							ipEthAParam.attrValuesNum = 1;
//							ipEthAParam.attrValues = new void*[ipEthAParam.attrValuesNum];
//							ipEthAParam.attrValues[0] = reinterpret_cast<void *>(ipA);
//
//							char ipAddressEthBstr[] = "ipAddressEthB";
//							ipEthBParam.attrName = ipAddressEthBstr;
//							ipEthBParam.attrType = ATTR_STRINGT;
//							ipEthBParam.attrValuesNum = 1;
//							ipEthBParam.attrValues = new void*[ipEthBParam.attrValuesNum];
//							ipEthBParam.attrValues[0] = reinterpret_cast<void*>(ipB);
//
//							retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, objName, &ipEthAParam);
//							if (retVal != ACS_CC_SUCCESS)
//							{
//								ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//										"ACS_CS_ServiceHandler::modify() "
//										"Error: modifyObjectAugmentCcb() failed %d - %s in line:%d",
//										getInternalLastError(), getInternalLastErrorText(), __LINE__));
//								result = false;
//							}
//
//							retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, objName, &ipEthBParam);
//							if (retVal != ACS_CC_SUCCESS)
//							{
//								ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//										"ACS_CS_ServiceHandler::modify() "
//										"Error: modifyObjectAugmentCcb() failed %d - %s in line:%d",
//										getInternalLastError(), getInternalLastErrorText(), __LINE__));
//								result = false;
//							}
//
//							free(ipA);
//							free(ipB);
//							delete[] ipEthAParam.attrValues;
//							delete[] ipEthBParam.attrValues;
//						}
//					}
//				}
//			}
//
//			delete model;
//
//			retVal = applyAugmentCcb(ccbHandleVal);
//			if (retVal != ACS_CC_SUCCESS)
//			{
//				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//						"ACS_CS_ServiceHandler::modify() "
//						"Error: applyAugmentCcb() failed with %s  %d - %s ",
//						objName, getInternalLastError(), getInternalLastErrorText()));
//				return ACS_CC_FAILURE;
//			}
//		}


		methodHandler.setObject(oiHandle,ccbId,"","",0,object);

		int error = methodHandler.modify();
		if (error != TC_NOERROR) {

			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::modify() "
					"Error in modifying object in transaction = %d Error %d: %s!", ccbId, error, ACS_CS_ImUtils::getErrorText(error).c_str()));
			this->setExitCode(error, ACS_CS_ImUtils::getErrorText(error));
			m_NohwcChange=false;
			return ACS_CC_FAILURE;
		}

	}

	if(result)
	{
		this->setExitCode(0);
		return ACS_CC_SUCCESS;
	}
	else
	{   m_NohwcChange=false;
		return ACS_CC_FAILURE;
	}

}


void ACS_CS_ServiceHandler::MacUpdate(ACS_CS_DHCP_Info* infoobj)
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate()"));

	if (!infoobj) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() ACS_CS_DHCP_Info object is NULL"));
		return;
	}

	if (!dhcp) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() DhcpConfigurator object is NULL"));
		return;
	}

	int length = infoobj->getMacLength();

	char* macAddr = new char[length];
	memset(macAddr,0,length);
	infoobj->getMAC(macAddr, length);


	string macString = "";

	if (infoobj->getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
	{
		if (length != 6)
		{
			//error
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() Unexpected length of MAC, %d", length));
			delete []macAddr;
			return;
		}
		else
		{
			macString = ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*) macAddr, length);
		}

	}
	else if (infoobj->getDHCPMethod() == ACS_CS_Protocol::DHCP_Client &&
			(infoobj->getFBN() == ACS_CS_Protocol::FBN_EPB1 || infoobj->getFBN() == ACS_CS_Protocol::FBN_EvoET || infoobj->getFBN() == ACS_CS_Protocol::FBN_CMXB))
	{

		char* tempmac = new char[length+1];
		memset(tempmac,0,length+1);
		memcpy(tempmac,macAddr,length);
		macString = tempmac;
		delete []tempmac;
	}
	delete []macAddr;


	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
			"MAC ADDR RECEIVED %s", macString.c_str()));

	char shelfAddr[INET_ADDRSTRLEN];
	uint32_t naddr = infoobj->getMagAddress();
	inet_ntop(AF_INET, &naddr, shelfAddr, sizeof(shelfAddr));

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();
	ACS_CS_ImBlade* cloned_blade = 0;

	if (model)
	{
		const ACS_CS_ImBlade* blade = model->getBladeObject(shelfAddr, infoobj->getSlot());

		if (blade)
		{
			cloned_blade = dynamic_cast<ACS_CS_ImBlade*> (blade->clone());
		}

		delete model;
	}

	if (!cloned_blade)
	{
		return;
	}


	//Check if MAC address is changed. If so refresh dhcp reservation.
	if (infoobj->getNetwork() == ACS_CS_Protocol::Eth_A)
	{
		if (cloned_blade->macAddressEthA.size() == 0 || macString.compare(cloned_blade->macAddressEthA) != 0)
		{
			if((infoobj->getDHCPMethod() == ACS_CS_Protocol::DHCP_Client) &&
					(infoobj->getFBN()==ACS_CS_Protocol::FBN_EPB1 || infoobj->getFBN()==ACS_CS_Protocol::FBN_EvoET || infoobj->getFBN()==ACS_CS_Protocol::FBN_CMXB))
			{
				dhcp->removeClientDhcpBoard(infoobj);

				if (addEBSBoardToDHCPTable(infoobj))
				{
					cloned_blade->macAddressEthA = macString;
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
							"EBSBOARD (NORMAL) added"));
					ACS_CS_ImRepository::instance()->replaceObject(cloned_blade);
					cloned_blade = 0;
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() "
							"EBSaddNormalDhcpBoard failed"));
				}
			}
			// If DHCP normal boards
			else if (infoobj->getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
			{
				dhcp->removeNormalDhcpBoardByMAC(infoobj);
				dhcp->removeNormalDhcpBoard(infoobj);

				if (dhcp->addNormalDhcpBoard(infoobj)) {
					cloned_blade->macAddressEthA = macString;

					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
							"BOARD(NORMAL) added"));
					ACS_CS_ImRepository::instance()->replaceObject(cloned_blade);
					cloned_blade = 0;
				} else {
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() "
							"addNormalDhcpBoard NORMAL failed"));
				}
			}
		}

	} else if (infoobj->getNetwork() == ACS_CS_Protocol::Eth_B) {
		if (cloned_blade->macAddressEthB.size() == 0 || macString.compare(cloned_blade->macAddressEthB) != 0) {

			if((infoobj->getDHCPMethod() == ACS_CS_Protocol::DHCP_Client) &&
					(infoobj->getFBN()==ACS_CS_Protocol::FBN_EPB1 || infoobj->getFBN()==ACS_CS_Protocol::FBN_EvoET || infoobj->getFBN()==ACS_CS_Protocol::FBN_CMXB))
			{
				dhcp->removeClientDhcpBoard(infoobj);

				if (addEBSBoardToDHCPTable(infoobj)) {
					cloned_blade->macAddressEthB = macString;
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
							"EBS BOARD  (NORMAL) added"));
					ACS_CS_ImRepository::instance()->replaceObject(cloned_blade);
					cloned_blade = 0;
				} else {
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() "
							"EBS addNormalDhcpBoard failed"));
				}
			}
			// If DHCP normal boards
			else if (infoobj->getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
			{
				dhcp->removeNormalDhcpBoardByMAC(infoobj);
				dhcp->removeNormalDhcpBoard(infoobj);

				if (dhcp->addNormalDhcpBoard(infoobj)) {
					cloned_blade->macAddressEthB = macString;

					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
							"BOARD  (NORMAL) added"));
					ACS_CS_ImRepository::instance()->replaceObject(cloned_blade);
					cloned_blade = 0;
				} else {
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::MacUpdate() "
							"addNormalDhcpBoard failed"));
				}
			}
		}
	}

	if (cloned_blade)
		delete cloned_blade;


	// DONT SAVE MAC ADDRESS TO IMM FOR NOW
	/*

	ACS_CS_API_HWC_R1 * hwc;
	ACS_CS_API_BoardSearch_R1 *boardSearch;

	hwc = ACS_CS_API_R1::createHWCInstance();
	boardSearch = hwc->createBoardSearchInstance();
	boardSearch->reset();

	uint32_t magazine;
	uint16_t slot;

	magazine = infoobj->getMagAddress();

	ACS_CS_TRACE(
			(ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::MacUpdate() "
			"magazine = %u", magazine));

	slot = infoobj->getSlot();

	boardSearch->setMagazine(magazine);
	boardSearch->setSlot(slot);

	ACS_CS_API_IdList boardList;

	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;
	cs_call_result = hwc->getBoardIds(boardList, boardSearch);

	hwc->deleteBoardSearchInstance(boardSearch);
	ACS_CS_API_R1::deleteHWCInstance(hwc);

	int size = boardList.size();

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_CS_FTRACE(
				(ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
				"getEntries failed"));

		return;
	}

	if (size != 1) {
		ACS_CS_FTRACE(
				(ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
				"Unexpected number of boards, %d", size));

		return;
	}

	auto_ptr<ACS_CS_ImModel> model(ACS_CS_ImRepository::instance()->getModelCopy());
	const ACS_CS_ImBlade *constBlade = model.get()->getBlade(boardList[0]);

	if (constBlade == NULL)
	{
		ACS_CS_FTRACE(
				(ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
				"getBlade failed"));
		return;
	}

	ACS_CS_ImBlade *blade = const_cast<ACS_CS_ImBlade *>(constBlade);

	if (infoobj->getNetwork() == ACS_CS_Protocol::Eth_A)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::MacUpdate()  constBlade->macAddressEthA = %s  macString = %s", constBlade->macAddressEthA.c_str(), macString.c_str()));
		if(constBlade->macAddressEthA == macString)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::MacUpdate()  RETURN"));
			return;
		}
		blade->macAddressEthA = macString;
		cout << "blade->macAddressEthA = " << blade->macAddressEthA << endl;
	} else if (infoobj->getNetwork() == ACS_CS_Protocol::Eth_B)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::MacUpdate()  constBlade->macAddressEthB = %s  macString = %s", constBlade->macAddressEthB.c_str(), macString.c_str()));
		if(constBlade->macAddressEthB == macString)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::MacUpdate()  RETURN"));
			return;
		}
		blade->macAddressEthB = macString;
		cout << "blade->macAddressEthB = " << blade->macAddressEthB << endl;
	} else
	{
		ACS_CS_FTRACE(
				(ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
				"Unknown network"));
		return;
	}

	blade->action = ACS_CS_ImBase::MODIFY;
	ACS_CS_ImModelSaver saver(model.get());
	ACS_CC_ReturnType result = saver.save("MacUpdate");

	if (result != ACS_CC_SUCCESS) {
		ACS_CS_FTRACE(
				(ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
				"Could not save repository"));
		return;
	}
	 */

	//DHCP queue has been moved to SNMP Master Handler class
	//	ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port,dhcpSem);
	//
	////	 Cancel this request since it now has been successfully completed ----- REQUEST ALREADY REMOVED!!!
	//	snmpMapType::iterator it = snmpMap.find(infoobj->getMagAddress());
	//
	//	if (it != snmpMap.end())
	//	{
	//		SnmPHandlerMapEntry * mapEntry = it->second;
	//
	//		// Check if there are SNMP handlers available and call them
	//		if (mapEntry->handlerSlot0)
	//		{
	//			mapEntry->handlerSlot0->cancelGetMac(*infoobj);
	//		}
	//
	//		if (mapEntry->handlerSlot25)
	//		{
	//			mapEntry->handlerSlot25->cancelGetMac(*infoobj);
	//		}
	//	}

	//	dhcpQueueLock.start_writing();
	//
	//	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
	//		"normalDhcpVector size: %d", normalDhcpVector.size()));
	//
	//	// Also remove this request from the queue
	//	for (size_t i = normalDhcpVector.size(); i > 0; i--) {
	//		ACS_CS_DHCP_Info info = normalDhcpVector[i - 1];
	//
	//		// The request is the same if mag and slot are equal
	//		if (info.getMagAddress() == infoobj->getMagAddress()
	//				&& info.getSlot() == infoobj->getSlot()
	//				&& info.getNetwork() == infoobj->getNetwork()) {
	//			normalDhcpVector.erase(normalDhcpVector.begin() + i - 1);
	//
	//			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::MacUpdate() "
	//				"info found in normalDhcpVector"));
	//
	//			break;
	//		}
	//	}
	//
	//	dhcpQueueLock.stop_writing();
}

void ACS_CS_ServiceHandler::ProdInfoUpdate(ACS_CS_DHCP_Info* infoobj)
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::ProdInfoUpdate()"));

	if (!infoobj) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::ProdInfoUpdate() ACS_CS_DHCP_Info object is NULL"));
		return;
	}

	char shelfAddr[INET_ADDRSTRLEN];
	uint32_t naddr = infoobj->getMagAddress();
	inet_ntop(AF_INET, &naddr, shelfAddr, sizeof(shelfAddr));

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();
	const ACS_CS_ImBlade* blade = 0;
	const ACS_CS_ImCpBlade* cpBlade=0;

	if (model)
	{
		blade = model->getBladeObject(shelfAddr, infoobj->getSlot());

		//In DMX environment create or update CpProductInfo struct (we do this only for eth_A)
		if (shelfArchitecture == NODE_DMX && blade->type == CPBLADE_T)
		{
			cpBlade = dynamic_cast<const ACS_CS_ImCpBlade*>(blade);
			if (cpBlade)
			{
				ACS_CS_ProductInfo prodInfo = infoobj->getProductInfo();
				createCpProductInfo(cpBlade->rdn.c_str(), prodInfo);
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::ProdInfoUpdated "
				"BOARD  (NORMAL) added"));
			}
		}

		
		delete model;
	}

}



bool ACS_CS_ServiceHandler::verifyDhcpConfiguration (ACS_CS_Protocol::CS_DHCP_Method_Identifier method)
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::verifyDhcpConfiguration() "
			"args: method = %d", method));

	bool result = true;

	ACS_CS_API_HWC * hwc;
	ACS_CS_API_BoardSearch *boardSearch;
	ACS_CS_API_IdList boardList;

	hwc =  ACS_CS_API::createHWCInstance();
	boardSearch = hwc->createBoardSearchInstance();
	boardSearch->reset();
	boardSearch->setDhcpMethod(method);
	ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(boardList, boardSearch);

	if (cs_call_result == ACS_CS_API_NS::Result_Failure)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::verifyDhcpConfiguration() boardIds returned %i", cs_call_result));
		return false;
	}
	else
	{
		const ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();

		size_t size = boardList.size();

		// Loop through all returned boards
		for(uint16_t i = 0; i < size; i++)
		{
			const ACS_CS_ImBlade *blade = model->getBlade(boardList[i]);

			if (blade != NULL)
			{
				//Clean up CP product info
				if (blade->functionalBoardName == CPUB && blade->dhcpOption == DHCP_NORMAL)
				{
					updateProductInfoAttr(blade->rdn.c_str(), true);
				}

				ACS_CS_DHCP_Info infoA,infoB,infoAliasA,infoAliasB;
				ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE;				
				if(blade->functionalBoardName == SMXB)
					switchType = ACS_CS_Protocol::CONTROL_SWITCH;

				// Create DHCP info objects from this board (one for each backplane)
				if (createDHCPInfo(infoA, blade, ACS_CS_Protocol::Eth_A,switchType)	&& createDHCPInfo(infoB, blade, ACS_CS_Protocol::Eth_B,switchType))
				{
					if (infoA.getFBN()==ACS_CS_Protocol::FBN_SMXB)
					{
						if(createDHCPInfo(infoAliasA, blade, ACS_CS_Protocol::Eth_A, ACS_CS_Protocol::TRANSPORT_SWITCH)	&& createDHCPInfo(infoAliasB, blade, ACS_CS_Protocol::Eth_B, ACS_CS_Protocol::TRANSPORT_SWITCH))
						{
							ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::verifyDhcpConfiguration() Created DHCP infoAliasA and infoAliasB"));
						}
						else
						{
							ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "[%s:%d] Failed to create DHCP Info Alias for <%s>",
														__FUNCTION__, __LINE__, blade->rdn.c_str()));
							continue;
						}
					}

					if ((infoA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Client))
					{

						if (infoA.getFBN()==ACS_CS_Protocol::FBN_EPB1 || infoA.getFBN()==ACS_CS_Protocol::FBN_EvoET ||
								infoA.getFBN()==ACS_CS_Protocol::FBN_CMXB)
						{
							result = dhcp->removeClientDhcpBoard(&infoA);
							result = dhcp->removeClientDhcpBoard(&infoB);
							addToDHCPQueue(infoA,infoB);

						}
						else if((infoA.getFBN()==ACS_CS_Protocol::FBN_SCXB) || (infoB.getFBN()==ACS_CS_Protocol::FBN_SCXB))
						{
							// SCX boards
							result = dhcp->removeClientDhcpBoard(&infoA);
							result = dhcp->removeClientDhcpBoard(&infoB);

							result = dhcp->addScxbDhcpBoard(&infoA);
							result = dhcp->addScxbDhcpBoard(&infoB);
						}
						else if ((infoA.getFBN()==ACS_CS_Protocol::FBN_SMXB) || (infoB.getFBN()==ACS_CS_Protocol::FBN_SMXB))
						{													
							result = dhcp->removeClientDhcpBoard(&infoA);
							result = dhcp->removeClientDhcpBoard(&infoB);	
							result = dhcp->removeClientDhcpBoard(&infoAliasA);
							result = dhcp->removeClientDhcpBoard(&infoAliasB);							
							result = dhcp->addSmxbDhcpBoard(&infoA);
							result = dhcp->addSmxbDhcpBoard(&infoB);
							result = dhcp->addSmxbDhcpBoard(&infoAliasA);
							result = dhcp->addSmxbDhcpBoard(&infoAliasB);							
						}
						else
						{
							// Other client reservation boards, such as SCBRP and BCs
							result=dhcp->removeClientDhcpBoard(&infoA);
							result=dhcp->removeClientDhcpBoard(&infoB);
							result=dhcp->addClientDhcpBoard(&infoA);
							result=dhcp->addClientDhcpBoard(&infoB);
						}
					}
					else if ((infoA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal))
					{
						// DHCP normal boards
						result = dhcp->removeNormalDhcpBoard(&infoA);
						result = dhcp->removeNormalDhcpBoard(&infoB);

						if (shelfManager)
						{
							shelfManager->insertIntoDhcpQueue(infoB);
							shelfManager->insertIntoDhcpQueue(infoA);
							shelfManager->wakeUpQueue();
						}
						else
						{
							//No Shelf Manager available: use MAC addresses stored in IMM
							int macA_length = infoA.getMacLength();
							int macB_length = infoB.getMacLength();

							if ( macA_length > 0 )
							{
								dhcp->removeNormalDhcpBoardByMAC(&infoA);
								dhcp->removeNormalDhcpBoard(&infoA);
								dhcp->addNormalDhcpBoard(&infoA);
							}

							if (macB_length > 0)
							{
								dhcp->removeNormalDhcpBoardByMAC(&infoB);
								dhcp->removeNormalDhcpBoard(&infoB);
								dhcp->addNormalDhcpBoard(&infoB);
							}
						}

					}

					// Set shelf Manager in case of SCB-RP or SCX boards at startup
					if ((infoA.getFBN() == ACS_CS_Protocol::FBN_SCBRP) || (infoA.getFBN() == ACS_CS_Protocol::FBN_SCXB) || 
						(infoA.getFBN() == ACS_CS_Protocol::FBN_SMXB))
					{
						ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::verifyDhcpConfiguration() Adding snmp handler"));
						if (shelfManager) shelfManager->addShelfHandler(infoA, infoB);
					}

				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "[%s:%d] Failed to create DHCP Info for <%s>",
							__FUNCTION__, __LINE__, blade->rdn.c_str()));
				}
			}
		}

		delete model;
	}

	hwc->deleteBoardSearchInstance(boardSearch);
	ACS_CS_API::deleteHWCInstance(hwc);
	return result;
}


bool ACS_CS_ServiceHandler::syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::syncClients()"));
	if (shelfManager)
		return shelfManager->syncClients(syncVector);
	else
		return true;

	//	if (syncVector.size() == 0)	// Simple case, nothing to sync
	//		return true;
	//	else if (snmpMap.size() == 0) // Simple case, no SCB-RP boards
	//		return false;
	//
	//	bool syncSuccess = true;
	//	int threadNo = 0;
	//
	//	magazineMapType magazineMap;
	//
	//	// Loop through all SCB-RP boards and create a DHCP info vector for each magazine
	//	for (snmpMapType::iterator snmpIt = snmpMap.begin(); snmpIt != snmpMap.end(); ++snmpIt)
	//	{
	//		uint32_t magazine = snmpIt->first;
	//		if (magazineMap.find(magazine) == magazineMap.end())	// Check that we haven't already created magazine
	//		{
	//			std::vector<ACS_CS_DHCP_Info*> * tmpVector = new std::vector<ACS_CS_DHCP_Info*>();
	//			magazineMap.insert( magazineMapType::value_type(magazine, tmpVector));
	//		}
	//
	//		SnmPHandlerMapEntry * mapEntry = snmpIt->second;
	//
	//		if (mapEntry)
	//		{
	//			if (mapEntry->handlerSlot0)
	//				threadNo++;
	//
	//			if (mapEntry->handlerSlot25)
	//				threadNo++;
	//		}
	//	}
	//
	//	std::vector<ACS_CS_DHCP_Info*>::iterator dhcpIt;
	//
	//	// Loop through all DHCP objects that should be synced
	//	for (dhcpIt = syncVector.begin(); dhcpIt != syncVector.end(); ++dhcpIt)
	//	{
	//		uint32_t magazine = (*dhcpIt)->getMagAddress();
	//		magazineMapType::iterator it = magazineMap.find(magazine);
	//
	//		// Find the correct vector to add it to
	//		if (it != magazineMap.end())
	//		{
	//			it->second->push_back(*dhcpIt);
	//		}
	//		else
	//		{
	//			// No SCB-RP board for this magazine
	//			syncSuccess = false;
	//			break;
	//		}
	//	}
	//
	//	if (syncSuccess)
	//	{
	//		// If we get here we have SCB-RP boards for each magazine in which there are CPU boards
	//		//int threadNo = snmpMap.size() * 2;
	//		// Create a thread for each SNMP handler
	//		ACS_CS_Sync_Thread** threads = new ACS_CS_Sync_Thread*[threadNo];
	//		ACS_CS_EventHandle* threadHandles = new ACS_CS_EventHandle[threadNo];

	//		int threadCounter = 0;
	//
	//		// Loop through SNMP handlers
	//		for (snmpMapType::iterator snmpIt = snmpMap.begin(); snmpIt != snmpMap.end(); ++snmpIt)
	//		{
	//			uint32_t magazine = snmpIt->first;
	//			SnmPHandlerMapEntry * mapEntry = snmpIt->second;
	//
	//			//ACS_CS_SNMPHandler * handler =  snmpIt->handler;
	//
	//			magazineMapType::iterator magIt = magazineMap.find(magazine);
	//
	//			// retrieve vector with DHCP info objects for this magazine
	//			std::vector<ACS_CS_DHCP_Info*> * tempvector = magIt->second;
	//
	//			if (mapEntry->handlerSlot0)
	//			{
	//				// Create thread and start it
	//				threads[threadCounter] = new ACS_CS_Sync_Thread(mapEntry->handlerSlot0, tempvector);
	//				threadHandles[threadCounter] = threads[threadCounter]->getWaithandle();
	//				threads[threadCounter]->activate();
	//				threadCounter++;
	//			}
	//			if (mapEntry->handlerSlot25)
	//			{
	//				// Create thread and start it
	//				threads[threadCounter] = new ACS_CS_Sync_Thread(mapEntry->handlerSlot25, tempvector);
	//				threadHandles[threadCounter] = threads[threadCounter]->getWaithandle();
	//				threads[threadCounter]->activate();
	//				threadCounter++;
	//			}
	//		}
	//
	//		// Reuse threadCounter
	//		threadCounter = 0;
	//
	//		// Wait for all threads to finish
	//		while ( (threadCounter != threadNo)
	//				&& ACS_CS_Event::WaitForEvents(threadNo, threadHandles, 10000) >= 0)
	//		{
	//			threadCounter++;
	//		}
	//
	//		// If all threads finished successfully
	//		if (threadCounter == threadNo)
	//		{
	//			// All threads finished successfully
	//			for (int i = 0; i < threadNo; i++)
	//			{
	//				// Check result for each thread - at least one should be a success
	//				if (threads[i]->success())
	//				{
	//					syncSuccess = true;
	//				}
	//
	//				// remove thread
	//				delete threads[i];
	//				threads[i] = 0;
	//			}
	//		}
	//
	//		delete [] threads;
	//		delete [] threadHandles;
	//	}
	//
	//	// Remove all temporary vectors
	//	for (magazineMapType::iterator it = magazineMap.begin(); it != magazineMap.end(); ++it)
	//	{
	//		std::vector<ACS_CS_DHCP_Info*> * tmpVector = it->second;
	//		delete tmpVector;
	//	}
	//
	//	return syncSuccess;
}

bool ACS_CS_ServiceHandler::commitSync ()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::commitSync()"));

	return verifyDhcpConfiguration(ACS_CS_Protocol::DHCP_Normal);
}

int ACS_CS_ServiceHandler::exec ()
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_ServiceHandler::exec() "
			"Entering function "));

	//check if start after restore
	m_isRestartAfterRestore = isRestartAfterRestore();

	// Start the reactor runner
	reactorRunner->activate();

	//Start SNMP Master Handler Thread
	if (shelfManager)
	{
		shelfManager->activate();
	}

	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,  "ACS_CS_ServiceHandler::exec () isAP1 == %b", isAP1));

	if (isAP1)
	{
		//Remove invalid entries from HWC table. This MUST be done before setting IMM implementers!!!
		removeInvalidEntries();
	}

	//-----------------------------

	size_t handleArrSize = 0;               // Size of array
	ACS_CS_EventHandle * handleArr = 0;     // Array used in WaitForMultipleObjects

	vector<ACS_CS_EventHandle> eventVector;        // Vector to hold events for different listening sockets
	//MIHA    vector<HANDLE> timerVector;        // Vector to hold handles for timers determining different phases.

	//bool isTestEnvironment = ACS_CS_Registry::isTestEnvironment();
	//CS_Service_Type currentState = ServiceType_NotSpecified;
	//CS_Service_Type newState = ServiceType_NotSpecified;
	//CS_Service_Type oldState = ServiceType_NotSpecified;

	// Initialize Function Distribution structure
	// Has to be called before to set the implementer
	FuncDistHandler::instance()->initFunctionDistribution();

	if (shelfArchitecture == NODE_VIRTUALIZED)
		removeInvalidComputeResourceObjects();

	//////////////////////////// SETTING IMM IMPLEMENTERS!!!! //////////////////////////////////
	// Initialize IMM
	initImm();

	if (exit_)	//shutdown is ordered
	{
		thread_running = true;			// HX59136
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::exec() - exit_ is TRUE immediately after initImm()!"));
		reactorRunner->deActivate();

		// remove folder watcher if defined
		if(-1 != m_watcherID)
			inotify_rm_watch( m_InotifyFD, m_watcherID );

		// Close the INOTIFY instance
		ACE_OS::close( m_InotifyFD );

		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	if (isAP1)
	{
		if (isDHCP_ON)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,
						"[%s:%d] Starting DHCP configuration...", __FUNCTION__, __LINE__));

			//Reload DHCP configuration
			dhcp->cleanUpDhcpLeases();
			verifyDhcpConfiguration(ACS_CS_Protocol::DHCP_Client);
			verifyDhcpConfiguration(ACS_CS_Protocol::DHCP_Normal);

			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,
									"[%s:%d] DHCP configuration done", __FUNCTION__, __LINE__));

		}

		if (shelfArchitecture == NODE_VIRTUALIZED)
		{
			verifyTftpConfiguration();
		}
	}

	if (isAP1)
		ACS_CS_Master::start();
	else
		ACS_CS_Slave::start();


	//read static IPs from disk after restore
	if (isAP1 && isDHCP_ON)
	{
		if (isAfterRestore())
		{
			bool result = loadStaticIpFromDisk();

			if(result == false)
			{
				//retry for imm setting
				startStaticIpAlignedTimer();
			}
		}
		else
		{
			//compare static IP stored in disk and imm to check for misalignment 
			checkStaticIpDiskWithIMM();
			// initialize static map
			initStaticIpMap();
		}
	}

	// Start the subscription agent
	subscriptionAgent = ACS_CS_SubscriptionAgent::getInstance();
	ACS_CS_SubscriptionAgent::start();

	// start tcp server for listening on incoming connection for the CS protocol
	if (isAP1)
		startListener();

	// start table handlers
	startHandlers();

	//Start Trap Handler
	if(shelfArchitecture != NODE_VIRTUALIZED)
	{
		startTrapManager();
	}

	if (isAP1)
	{
		//This operation shall be performed after implementers are set and subscription agent is started.
		//In this way we are sure that CP table observer are properly notified.
		initializeMauType();
	}

	
	if (isAP1 && cpClusterManager)
	{
		//check if start after restore
		//m_isRestartAfterRestore = isRestartAfterRestore();
		if (isAfterRestore())
		{
			if(!cpClusterManager->restoreCpClusterFromDisk()){
				//reschedule event to align cluster after restore procedure
				startClusterAlignedTimer();
			}
		}
	}
//Removed due to Double Node Disaster Recovery functionality and possible misalignment
//	between CrM and CrMgmt models
//	if (isAP1 && NODE_VIRTUALIZED == shelfArchitecture)
//	{
//		//check if start after restore
//		if (isAfterRestore() && m_computeResourceHandler)
//		{
//			//restore CR from disk
//			m_computeResourceHandler->restoreComputeResourcesFromDisk();
//		}
//	}

	// Activate all threads
	maptype::iterator mapIt;

	// setup all events
	eventVector.push_back(shutdownHandlerEvent);
	eventVector.push_back(removeClosedConnectionsEvent);
	eventVector.push_back(immChangedEvent);

	ACS_CS_EventHandle neClusterOpModeTimer = 0;
	ACS_CS_EventHandle neValidateTimer = 0;
	ACS_CS_EventHandle neApNotifyTimer = 0;
	ACS_CS_EventHandle neCpNotifyTimer = 0;
	ACS_CS_EventHandle neCommitTimer = 0;

	if (isAP1)
	{
		// On AP1 we should also wait for a set of timers used to change cluster OP mode and profile phases
		neClusterOpModeTimer = ACS_CS_Timer::OpenNamedTimer(NE_CLUSTEROPMODE_TIMER_ID);

		if (neClusterOpModeTimer == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::exec() "
					"Failed to open timer %s", NE_CLUSTEROPMODE_TIMER_ID));
		}

		neValidateTimer      = ACS_CS_Timer::OpenNamedTimer(NE_PHASE_VALIDATE_TIMER_ID);

		if (neValidateTimer == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::exec() "
					"Failed to open timer %s", NE_PHASE_VALIDATE_TIMER_ID));
		}

		neApNotifyTimer      = ACS_CS_Timer::OpenNamedTimer(NE_PHASE_APNOTIFY_TIMER_ID);

		if (neApNotifyTimer == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::exec() "
					"Failed to open timer %s", NE_PHASE_APNOTIFY_TIMER_ID));
		}

		neCpNotifyTimer      = ACS_CS_Timer::OpenNamedTimer(NE_PHASE_CPNOTIFY_TIMER_ID);

		if (neCpNotifyTimer == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::exec() "
					"Failed to open timer %s", NE_PHASE_CPNOTIFY_TIMER_ID));
		}

		neCommitTimer        = ACS_CS_Timer::OpenNamedTimer(NE_PHASE_COMMIT_TIMER_ID);

		if (neCommitTimer == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::exec() "
					"Failed to open timer %s", NE_PHASE_COMMIT_TIMER_ID));
		}

		eventVector.push_back(neClusterOpModeTimer);
		eventVector.push_back(neValidateTimer);
		eventVector.push_back(neApNotifyTimer);
		eventVector.push_back(neCpNotifyTimer);
		eventVector.push_back(neCommitTimer);
		eventVector.push_back(staticIpAlignedTimer);
		eventVector.push_back(subscriptionSupervisionTimerHandle);

		// We should only listen to the protocol port on AP1
		csTcpServer->addActiveServerHandlesToVector(eventVector);

		// Will not be any if not in APZ 212 3X
		moddTcpServer->addActiveServerHandlesToVector(eventVector);
	}
	else
	{
		// On AP2
		// Create AP2 node if not already created
		FuncDistHandler::instance()->createAPobject();
	}
	

	if(ptbAHandler)
	{
		eventVector.push_back(*ptbAHandler->getEvent());
	}
	if(ptbBHandler)
	{
		eventVector.push_back(*ptbBHandler->getEvent());
	}

	//active a watcher
	if (addWatcherOnFolder())
	{
		eventVector.push_back(m_InotifyFD);
	}

	if (isAP1 && cpClusterManager){
		eventVector.push_back(clusterAlignedTimer);
	}

	eventVector.push_back(immDispatchFailedEvent);


	if(shelfArchitecture == NODE_VIRTUALIZED) //Timer created only in VIRTUALIZED ENV
		eventVector.push_back(logicalMgmtCleanupTimer);

	// convert handles to array
	handleArrSize = eventVector.size();
	handleArr = new ACS_CS_EventHandle[handleArrSize];

	for (size_t i = 0;i < eventVector.size(); i++) {
		handleArr[i] = eventVector[i];
	}

	if(exit_)					// start of HX59136
	{
		thread_running = true;
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::exec() - thread exit_ already TRUE before start of while() - so set thread_running = true to skip loop in HAClass"));
	}							// end of HX59136

	//stop other activities
	while (!exit_)  // exit_ is an inherited variable. It is set to false by the inherited
	{               // function activate() and set to true by the function deActivate().

		// Wait for shutdown and accept events
		thread_running = true;
		int eventIndex = ACS_CS_Event::WaitForEvents(handleArrSize, handleArr, 0);

	//	thread_running = true;


		if (eventIndex == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_ERROR,
					"ACS_CS_ServiceHandler::exec() "
					"Error: WaitForEvents(), Unable to wait for clients. Disconnecting all clients."));

			break;

		}
		else if (eventIndex == 0) // Shutdown event
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_INFO,
					"ACS_CS_ServiceHandler::exec() "
					"Shutdown event received"));

			exit_ = true;
		}
		else if (eventIndex == 1)   // Remove connections that has been closed from ACS_CS_ServiceConnection
		{
//			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
//					LOG_LEVEL_INFO,
//					"ACS_CS_ServiceHandle::exec() "
//					"ACS_CS_ServiceConnection thread closed event received"));


			removeClosedConnections(connectionSet);
		}
		else if (eventIndex == 2)	// IMM updated
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::exec() "
					"IMM Updated event"));

			// Free unused workers
			//tableOperationWorkerCleanup();

			// Free old SNMP handlers
			//snmpHandlerCleanup();

			//Remove empty logical objects
			LogicalObjCleanUp();

		}
		//TODO: network element functionalities are not available on AP2
		else if (isAP1 && eventIndex > 2 && eventIndex < 8)	// Timer timeout
		{
			// A timer has expired, call the appropriate function in NEHandler
			if (eventIndex == 3)
				neTable->handleClusterOpModeSupervisionTimerExpired();
			else if (eventIndex == 4)
				neTable->handleOmProfileSupervisionTimerExpired(ACS_CS_API_OmProfilePhase::Validate);
			else if (eventIndex == 5)
				neTable->handleOmProfileSupervisionTimerExpired(ACS_CS_API_OmProfilePhase::ApNotify);
			else if (eventIndex == 6)
				neTable->handleOmProfileSupervisionTimerExpired(ACS_CS_API_OmProfilePhase::CpNotify);
			else if (eventIndex == 7)
				neTable->handleOmProfileSupervisionTimerExpired(ACS_CS_API_OmProfilePhase::Commit);

		}
		else if (csTcpServer->isServerHandle(handleArr[eventIndex]))  // Incoming TCP connection
		{
			openNewServiceConnection(handleArr[eventIndex], connectionSet,
					csTcpServer, ACS_CS_Protocol::Protocol_CS);
		}
		else if (moddTcpServer->isServerHandle(handleArr[eventIndex]))  // MODD protocol (only APZ 212 3X
		{
//			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//					"ACS_CS_ServiceHandler::exec() "
//					"Accept event received on MODD protocol"));

			openNewServiceConnection(handleArr[eventIndex], connectionSet,
					moddTcpServer, ACS_CS_Protocol::Protocol_MODD);
		}
		else if(ptbAHandler && *ptbAHandler->getEvent() == handleArr[eventIndex])  // APZ212 50
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::exec() "
					"new MAC event received; side A"));

			maptype::iterator it = handlerMap->find(ACS_CS_Protocol::Scope_MODD);

			if(handlerMap->end() != it)
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
						"ACS_CS_ServiceHandler::exec() "
						"handler iterator found"));

				ACS_CS_MODDHandler *moddHandler = dynamic_cast<ACS_CS_MODDHandler *>(it->second);
				if(moddHandler)
				{
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
							"ACS_CS_ServiceHandler::exec() "
							"moddHandler obtained"));

					moddHandler->updateDhcpAPZ21250(ptbAHandler->getMacMap(), ACS_CS_PTBPeerHandler::PTBA);
				}
			}
		}
		else if(ptbBHandler && *ptbBHandler->getEvent() == handleArr[eventIndex])  // APZ212 50
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::exec() "
					"new MAC event received; side B"));

			maptype::iterator it = handlerMap->find(ACS_CS_Protocol::Scope_MODD);

			if(handlerMap->end() != it)
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
						"ACS_CS_ServiceHandler::exec() "
						"handler iterator found"));

				ACS_CS_MODDHandler *moddHandler = dynamic_cast<ACS_CS_MODDHandler *>(it->second);
				if(moddHandler)
				{
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
							"ACS_CS_ServiceHandler::exec() "
							"moddHandler obtained"));

					moddHandler->updateDhcpAPZ21250(ptbBHandler->getMacMap(), ACS_CS_PTBPeerHandler::PTBB);
				}
			}
		}
		else if (isAP1 && m_InotifyFD == handleArr[eventIndex])
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::exec() "
					"received folder change notification"));

			handlingFolderChange();
		}
		else if (isAP1 && cpClusterManager && clusterAlignedTimer == handleArr[eventIndex])	// Cluster Aligned
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::exec() "
					"Cluster Aligned event"));

			if (cpClusterManager)
			{
				usleep(500000);
				if (!cpClusterManager->restoreCpClusterFromDisk())
				{
					//reschedule event to align cluster after restore procedure
					startClusterAlignedTimer();
				}
			}
		}
		else if (isAP1 && staticIpAlignedTimer == handleArr[eventIndex])	// static Ip Aligned
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::exec() "
					"Static Ip Aligned event"));

			usleep(500000);
			if (!loadStaticIpFromDisk())
			{
				//reschedule event to align static ip after restore procedure
				startStaticIpAlignedTimer();
			}
		}
		else if (isAP1 && subscriptionSupervisionTimerHandle == handleArr[eventIndex])
		{
			usleep(500000);
			m_isSubscriptionTimerEnded = true;
			//send all notifications stored during the supervision period
			storedTableOperationWorker();
		}
		else if (immDispatchFailedEvent == handleArr[eventIndex])	// IMM DISPATCHED
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_WARN,
					"ACS_CS_ServiceHandler::exec() "
					"IMM Dispatch Failed"));
			handleDispatchFailed();
		}
		else if((shelfArchitecture == NODE_VIRTUALIZED) && (logicalMgmtCleanupTimer == handleArr[eventIndex]))
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_WARN,
					"ACS_CS_ServiceHandler::exec() "
					"logicalMgmtCleanupTimer - Timer elapsed - Cleaning up LogicalMgmt MO subtree"));
			LogicalObjCleanUp_vAPZ();
			ACS_CS_DEBUG(("CcbId[] - DONE Cleaning up LogicalMgmt MO subtree"));
		}

		//thread_running = false;
	} // End of while( !exit_ )
	if (handleArr)
	{
		delete [] handleArr;
		handleArr=0;
	}

	//Close PTB Handler Threads if in APZ21250 configuration
	if(ptbAHandler)
	{
		ptbAHandler->deActivate();
//		ptbAHandler->wait(1000);
//		ptbAHandler->close();
	}
	if(ptbBHandler)
	{
		ptbBHandler->deActivate();
//		ptbBHandler->wait(1000);
//		ptbBHandler->close();
	}

	//Stop Function Distribution Thread
	FuncDistHandler::instance()->stopThread();

	reactorRunner->deActivate();

	csTcpServer->stopListen();

	moddTcpServer->stopListen();

	ACS_CS_SubscriptionAgent::stop();

	//Remove IMM Class Implementers and stop callback thread
	oiHandler.removeMultipleClassImpl(this,classNames);

	if (immWorker)
		immWorker->deActivate();

	// Stop all service connection threads
	set<ACS_CS_ServiceConnection*>::iterator connectionIt;
	connectionIt = connectionSet.begin();

	while (connectionIt != connectionSet.end())
	{
		ACS_CS_ServiceConnection *client = *connectionIt;

		if (!client->isFinished())
		{
			// Thread is not finished
			client->disconnect();
		}
		connectionIt++;
	}

	// remove folder watcher if defined
	if(-1 != m_watcherID)
		inotify_rm_watch( m_InotifyFD, m_watcherID );

	// Close the INOTIFY instance
        
        
	ACE_OS::close( m_InotifyFD );
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
                                LOG_LEVEL_INFO,
                                "ACS_CS_ServiceHandler::exec() "
                                "Exiting"));


	return 0;
}

int ACS_CS_ServiceHandler::newRequest (ACS_CS_PDU *pdu)
{

	if (pdu == 0)  // Check for valid PDU
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandler::newRequest() "
				"Error: Invalid PDU"));

		return -1;
	}

	int error = -1;
	const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU

	if (header)
	{
		ACS_CS_Protocol::CS_Scope_Identifier scope = header->getScope();

		if (handlerMap)
		{
			ACS_CS_TableHandler * handler = 0;
			maptype::iterator it = handlerMap->find(scope); // Search for entry in table

			if ( it != handlerMap->end() )  // If handler exists
			{
//				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//						"ACS_CS_ServiceHandler::newRequest() "
//						"Handling request for scope %d", scope));

				handler = it->second;        // Get pointer to handler

				if (handler)
				{
					error = handler->handleRequest(pdu);     // Send request to handler
				}

			}
			else  // Unknown scope
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
						LOG_LEVEL_ERROR,
						"ACS_CS_ServiceHandler::newRequest() "
						"Error: Unknown scope %d",
						scope));

				error = -1;
			}
		}
	}

	return error;
}

bool ACS_CS_ServiceHandler::newTableOperationRequest(const ACS_CS_ImModelSubset *subset, ACS_CS_TableHandler *selectedHandler)
{
	bool result = true;

	//No specific handler requested. Notify all handlers in the map
	if (!selectedHandler)
	{
		ACS_CS_TableHandler *handler = NULL;
		maptype::iterator it = handlerMap->begin();

		tableOperationWorkerCleanup();

		for (;it != handlerMap->end(); ++it)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::newTableOperationRequest() "
					"Handling internal table operation request for scope %d", it->first));

			handler = it->second;

			if (handler)
			{
				// Let the table handler do some work
				ACS_CS_TableOperationWorker *worker = new ACS_CS_TableOperationWorker(handler, subset);

				tableWorkersLock.start_writing();

				// Save the worker so we can free it at a later time
				tableWorkers.push_back(worker);

				tableWorkersLock.stop_writing();

				if (!worker->activate())
				{
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
							"ACS_CS_ServiceHandler::newTableOperationRequest() "
							"Handler for table %d failed processing the request", it->first));

					result = false; // A table handler has failed processing the data
				}
			}
		}
	}
	else
	{
		//Notify only the handler provided as input
		// Let the table handler do some work
		ACS_CS_TableOperationWorker *worker = new ACS_CS_TableOperationWorker(selectedHandler, subset);

		tableWorkersLock.start_writing();

		// Save the worker so we can free it at a later time
		tableWorkers.push_back(worker);

		tableWorkersLock.stop_writing();

		if (!worker->activate())
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::newTableOperationRequest() "
					"Requested Handler failed processing the request"));

			result = false; // A table handler has failed processing the data
		}
	}

	return result;
}

void ACS_CS_ServiceHandler::startListener ()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::startListener() "
			"Entering method "));


	// CS Protocol

	unsigned short port = ACS_CS_NS::CS_Server_Port;

	uint32_t ipA1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_A_HOSTNAME);
	uint32_t ipB1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_A_HOSTNAME);
	uint32_t ipA2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_B_HOSTNAME);
	uint32_t ipB2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_B_HOSTNAME);

	// listen to these addresses...
	csTcpServer->addTcpServer(ipA1, port);
	csTcpServer->addTcpServer(ipB1, port);
	csTcpServer->addTcpServer(ipA2, port);
	csTcpServer->addTcpServer(ipB2, port);

	// ...and also localhost (127.0.0.1)
	csTcpServer->addTcpServer(INADDR_LOOPBACK, port);

	// start the servers
	unsigned int active = csTcpServer->startListen();

	if (active == 0)
	{
		char message[100] = {0};
		sprintf(message, "Server cannot open port %d for listening on any network", port);

		ACS_CS_EVENT(Event_SocketAPIFailure, ACS_CS_EventReporter::Severity_Event, "Open TCP port", message, "");
	}
	else {

		// Find out which addresses on which we could successfully listen
		// These addresses will be used as boot server IP since they are known to work
		size_t activeHandles = csTcpServer->getActiveHandles();
		int * handles = new int[activeHandles];
		csTcpServer->getActiveServerHandles(handles, activeHandles);

		if (activeHandles)
		{
			// Loop through all active servers
			for (size_t i = activeHandles; i > 0; i--)
			{
				uint32_t address = csTcpServer->getActiveServerAddress(handles[i - 1]);

				// If address is on EthA, update boot ip address for EthA
				if ( address == ipA1 || address == ipB1 )
				{
					bootIpAddressEthA = address;
				}
				// If address is on EthB, update boot ip address for EthB
				else if ( address == ipA2 || address == ipB2 )
				{
					bootIpAddressEthB = address;
				}
			}
		}

		char addr[INET_ADDRSTRLEN];
		uint32_t naddr = htonl(bootIpAddressEthA);
		inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ServiceHandler::startListener() "
				"Using address %s as Boot IP on EthA", addr));

		naddr = htonl(bootIpAddressEthB);
		inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ServiceHandler::startListener() "
				"Using address %s as Boot IP on EthB", addr));

		delete [] handles;
	}


	// MODD protocol
	// Check if we are running in APZ 212 3X system
	int apzType = ACS_CS_Registry::getAPZType();
	if (1 == apzType )
	{
		port = ACS_CS_NS::MODD_Protocol_Port;

		// listen to these addresses...
		moddTcpServer->addTcpServer(ipA1, port);
		moddTcpServer->addTcpServer(ipB1, port);
		moddTcpServer->addTcpServer(ipA2, port);
		moddTcpServer->addTcpServer(ipB2, port);

		active = moddTcpServer->startListen();

		if (active == 0)
		{
			char message[100] = {0};
			sprintf(message, "Server cannot open port %d for listening on any network", port);

			ACS_CS_EVENT(Event_SocketAPIFailure, ACS_CS_EventReporter::Severity_Event, "Open TCP port", message, "");
		}
	}
	else if (2 == apzType) //APZ 21240
	{
		port = ACS_CS_NS::MODD_Protocol_Port;

	}
	else if (3 == apzType) //APZ 21250
	{
		port = ACS_CS_NS::MODD_Protocol_Port;

		// listen to these addresses...
		moddTcpServer->addTcpServer(ipA1, port);
		moddTcpServer->addTcpServer(ipB1, port);
		moddTcpServer->addTcpServer(ipA2, port);
		moddTcpServer->addTcpServer(ipB2, port);

		active = moddTcpServer->startListen();

		if (active == 0)
		{
			char message[100] = {0};
			sprintf(message, "Server cannot open port %d for listening on any network", port);

			ACS_CS_EVENT(Event_SocketAPIFailure, ACS_CS_EventReporter::Severity_Event, "Open TCP port", message, "");
		}
	}
}

void ACS_CS_ServiceHandler::startHandlers ()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::startHandlers()() "
			"Entering function"));

	if (handlerMap)
	{
		handlerMap->clear();

		ACS_CS_HWCHandler * hwcHandler = new ACS_CS_HWCHandler();
		if (hwcHandler)
		{
			hwcHandler->setServiceHandler(this);
			handlerMap->insert( maptype::value_type(ACS_CS_Protocol::Scope_HWCTable, hwcHandler) ); 
			hwcTable = hwcHandler;
		}

		ACS_CS_CPIdHandler * cpIdHandler = new ACS_CS_CPIdHandler();
		handlerMap->insert( maptype::value_type(ACS_CS_Protocol::Scope_CPIdTable, cpIdHandler) );

		ACS_CS_CPGroupHandler * cpGroupHandler = new ACS_CS_CPGroupHandler();
		handlerMap->insert( maptype::value_type(ACS_CS_Protocol::Scope_CPGroupTable, cpGroupHandler) );

		if (NODE_VIRTUALIZED == shelfArchitecture)
			m_computeResourceHandler = new ACS_CS_ComputeResourceHandler();

		ACS_CS_NEHandler * neHandler = 0;

		if(isAP1)
		{
			neHandler = new ACS_CS_NEHandler();
			handlerMap->insert( maptype::value_type(ACS_CS_Protocol::Scope_NE, neHandler) );
			neTable = neHandler;
			neHandler->m_isRestartAfterRestore = m_isRestartAfterRestore; /*HW99445*/
		}

		ACS_CS_TableHandler * vlanHandler = 0;
		if (NODE_VIRTUALIZED != shelfArchitecture)
		{
			vlanHandler = new ACS_CS_NativeVLANHandler();
		}
		else
		{
			vlanHandler = new ACS_CS_VirtualVLANHandler();
		}
		
		handlerMap->insert( maptype::value_type(ACS_CS_Protocol::Scope_VLAN, vlanHandler) );

		ACS_CS_GLKHandler * glkHandler = new ACS_CS_GLKHandler();
		handlerMap->insert( maptype::value_type(ACS_CS_Protocol::Scope_GlobalLogicalClock,glkHandler) );
		// Add CP id, CP group and Functional distribution as table observers to the HWC table

		int apzType = ACS_CS_Registry::getAPZType();

		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ServiceHandler::readAPZType() "
				"Current used apzProtocolType: %d ", apzType));

		//Clean all potential HWC and DHCP entries statically set for Old APZ systems
		ACS_CS_MODDHandler moddHandler(bootIpAddressEthA, bootIpAddressEthB);

		if(isAP1 && apzType == 4) //APZ 212 55/60
			moddHandler.clearImmEntries();

		moddHandler.clearAPZ2123xEntry();
		moddHandler.clearAPZ21240Entry();
		moddHandler.clearAPZ21250Entry();
		moddHandler.clearGESBEntries();

		if (1 == apzType) // Only on APZ 212 3X
		{
			ACS_CS_MODDHandler * moddHandler = new ACS_CS_MODDHandler(
                    bootIpAddressEthA, bootIpAddressEthB, ACS_CS_MODDHandler::APZ2123X);

			if (isAP1)
			{
				// Add default entries (CPUB and MAUB) for this single CP system
				moddHandler->addAPZ2123xEntry();

				// Update DHCP table with these entries
				moddHandler->updateDhcpAPZ2123X();
			}

			handlerMap->insert(maptype::value_type(ACS_CS_Protocol::Scope_MODD,
					moddHandler));
		}
		else if (2 == apzType) // Only on APZ 212 40
		{
			ACS_CS_MODDHandler * moddHandler = new ACS_CS_MODDHandler(
					bootIpAddressEthA, bootIpAddressEthB, ACS_CS_MODDHandler::APZ21240);

			if (isAP1)
			{
				// Add default entries (CPUB and MAUB) for this single CP system
				moddHandler->addHwcClassicEntries();

				// Update DHCP table
				moddHandler->updateDhcpAPZ21240();
				moddHandler->addStaticGESBEntries();
			}

			handlerMap->insert(maptype::value_type(ACS_CS_Protocol::Scope_MODD,
					moddHandler));
		}
		else if (3 == apzType) // Only on APZ 212 50
		{
			ACS_CS_MODDHandler * moddHandler = new ACS_CS_MODDHandler(
					bootIpAddressEthA, bootIpAddressEthB, ACS_CS_MODDHandler::APZ21250);

			if (isAP1)
			{
				// Add default entries (CPUB and MAUB)
				moddHandler->addHwcClassicEntries();

				// Update DHCP table
				//moddHandler->updateDhcpAPZ21250();
				moddHandler->addStaticGESBEntries();
			}

			std::cout << "Inserting handler in map" << std::endl;
			handlerMap->insert(maptype::value_type(ACS_CS_Protocol::Scope_MODD,
					moddHandler));

			if (isAP1)
			{
				std::cout << "Starting PTB Handler A" << std::endl;
				ptbAHandler = new ACS_CS_PTBPeerHandler(
						ACS_CS_PTBPeerHandler::PTBA, PTBAIPAddressStr, PTBAPort);
				ptbAHandler->activate();

				std::cout << "Starting PTB Handler B" << std::endl;
				ptbBHandler = new ACS_CS_PTBPeerHandler(
						ACS_CS_PTBPeerHandler::PTBB, PTBBIPAddressStr, PTBBPort);
				ptbBHandler->activate();
			}
		}


		// Load all tables (add observers before loading)
		if (cpGroupHandler)
			(void) cpGroupHandler->loadTable();

		if (cpIdHandler)
			(void) cpIdHandler->loadTable();

		//if (funcDistHandler)
		//		(void) funcDistHandler->loadTable();

		if (isAP1 && neHandler)
			(void) neHandler->loadTable();

		if (vlanHandler)
			(void) vlanHandler->loadTable();

		if (hwcTable)
		{
			(void) hwcTable->loadTable(); // Load HWC table last
			ACS_CS_FTPAccessHandler::instance()->enableCPandAPBladesForFTP(hwcTable->getTable());
		}

		if (isAP1)
		{
			if (!isBladeCluster)
			{
				LogicalObjCleanUp();
			}
			else
			{
				cpClusterManager = new ACS_CS_CpClusterManager(neHandler);
				cpClusterManager->open();

				m_brfHandler->initialize(neHandler);
				m_brfHandler->open();
			}
		}

		// Initiate startup supervision for NEHandler in MultipleCPSystem
		if (isAP1 && isBladeCluster && neHandler)
		{
			if (neHandler->createCommandClassificationFolder()){
				(void) neHandler->handleStartupSupervision();
			}
		}

		//start subscription supervision timer
		startSubscriptionTimer();
	}
}

void ACS_CS_ServiceHandler::stopHandlers ()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::stopHandlers()"));

	if (cpClusterManager) cpClusterManager->shutDown(false);

	// Loop through handler map and delete table handlers
	// This should be done after all table observers have beed removed
	if (handlerMap)
	{
		maptype::iterator it = handlerMap->begin();

		it = handlerMap->begin();
		for (it = handlerMap->begin(); it != handlerMap->end(); ++it)
			//while (it != handlerMap->end())
		{
			ACS_CS_TableHandler * table = it->second;
			//it = handlerMap->erase(it);   // Remove from map
			if (table)
			{
				delete table;	// Delete table
			}
		}

		handlerMap->clear();
		neTable = 0;        // Set this pointer to NULL since the object is deleted
		hwcTable = 0;
	}

	if (m_computeResourceHandler)
	{
		delete m_computeResourceHandler;
		m_computeResourceHandler = 0;
	}

}


void ACS_CS_ServiceHandler::removeClosedConnections(set<ACS_CS_ServiceConnection*> &connections)
{

	set<ACS_CS_ServiceConnection*>::iterator it;

	it = connections.begin();

	while (it != connections.end())
	{
		ACS_CS_ServiceConnection *client = (*it);

		if (client->isFinished()) {
			//std::cout << "Thread Finished " << client << std::endl;
			client->close();
			connections.erase(it++);
			delete client;
		}
		else
		{
			++it;
		}
	}
}


void ACS_CS_ServiceHandler::openNewServiceConnection(int tcpServerHandle,
		set<ACS_CS_ServiceConnection*> &connectionSet,
		ACS_CS_TCPServer * tcpServer,
		ACS_CS_Protocol::CS_Protocol_Type protocolType)
{

	ACS_CS_TCPClient *tcpClient = tcpServer->acceptConnectionForHandle(tcpServerHandle);

	if (tcpClient == NULL)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_ServiceHandler::openNewServiceConnection() "
				"Error: Invalid socket"));
		return;
	}

	// Already too many threads, disconnect
	if (connectionSet.size() >= ACS_CS_NS::CS_Max_Threads)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_INFO,
				"ACS_CS_ServiceHandler::openNewServiceConnection "
				"Too many threads running, disconnecting client"));

		tcpClient->close();
		delete tcpClient;
		return;
	}

	// -------------------------------------------------
	// Log entry
//	string message = tcpClient->getFullAddressAsString().c_str();
//
//	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
//			"ACS_CS_ServiceHandler::openNewServiceConnection() "
//			"Connection from %s ", message.c_str()));

	// End of log entry
	// -------------------------------------------------

	// Create connection event and pass connected socket
	ACS_CS_ServiceConnection *connection = new ACS_CS_ServiceConnection(tcpClient,
			this, protocolType);

	if (connection)
	{
		(void) connection->activate();  // Start thread
		connectionSet.insert(connection);
	}
}


int ACS_CS_ServiceHandler::APZ2123xHandler ()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::APZ2123xHandler()"));

	int res=0;
	/*RoGa	if (!handlerMap)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() Exit - Can't find the handler Map "));
		return -1;
	}
RoGa */
	/*RoGa	maptype::iterator it = handlerMap->find(ACS_CS_Protocol::Scope_HWCTable); // Looking for the HWC table handler
	if (it == handlerMap->end())
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() Exit - Can't find the HWC table handler "));
		return -1;
	}
RoGa */
	/*RoGa	ACS_CS_HWCHandler *tmp = dynamic_cast<ACS_CS_HWCHandler*>(it->second);
	if (!tmp)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() Exit - Can't dynamic cast the HWC table handler "));
		return -1;
	}

	if (readAPZType() > 1)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"(%t) ACS_CS_ServiceHandler::APZ3xHandler()() "
			"Exit from ACS_CS_ServiceHandler::APZ2123xHandler method. CPAndProtocolType > 1 "));
		int tmpRes = 0;
		tmpRes = tmp->handleClearTable();//clear hwc table if cb boards entry for old ss are present for CPandProtocolType > 1
		if (tmpRes == 0)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() "
				"HWC table has been cleared as old ss entries has been found "));
		}
		else
		{
			if (tmpRes == -1)
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() "
					"Exit - failure during hwc table clearing "));
			}
		}
		return tmpRes;
	}

	res = tmp->handleAddAPZ2123xEntry();
	if (res == -1)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() "
			"ERROR - Not possible to configure the system with the old source system "));
	}
	else
	{
		tmp->saveTable();
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"(%t) ACS_CS_ServiceHandler::APZ2123xHandler()() "
			"The system with the old source system is configured "));
	}
RoGa */
	return res;
}

void ACS_CS_ServiceHandler::immUpdated()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::immUpdated()"));

	if (!ACS_CS_Event::SetEvent(immChangedEvent))
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ServiceHandler::immUpdated() "
				"Failed to set event"));
	}
}

void ACS_CS_ServiceHandler::reloadTables()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::reloadTables()"));

	maptype::iterator it;// = handlerMap->begin();

	for (it = handlerMap->begin(); it != handlerMap->end(); ++it)
	{
		bool result = it->second->loadTable();

		if (!result)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
					"ACS_CS_ServiceHandler::reloadTables() "
					"Failed to load table with scope %d", it->first));
		}
	}
}


uint16_t ACS_CS_ServiceHandler::parseString(const std::string &intString)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::openNewServiceConnection() "
			"args: intString = %s", intString.c_str()));

	std::istringstream parser(intString);
	uint16_t value = 0;
	if ( !( parser >> value) )
	{
		value = 0;
	}
	return value;
}


ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType ACS_CS_ServiceHandler::parseTableType(const char *objName)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::parseTableType() "
			"objName = %s", objName != 0 ? objName : "0"));

	ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType tableType = ACS_CS_INTERNAL_API::Table_NotSpecified;

	if (!objName)
		return tableType;

	std::vector<KeyValuePair> values = ACS_CS_KeyValueString::splitKeyValueStrByDelim(objName, ',');

	for (size_t i = 0;i < values.size(); i++)
	{
		string key = values[i].first;
		string value = values[i].second;

		if (key == "tableId")
		{
			if (value == "HWCTable")
			{
				tableType = ACS_CS_INTERNAL_API::Table_HWC;
			}
			else if (value == "NETable")
			{
				tableType = ACS_CS_INTERNAL_API::Table_NE;
			}
			else if (value == "CPIdTable")
			{
				tableType = ACS_CS_INTERNAL_API::Table_CP;
			}
			else if (value == "CPGNamesTable")
			{
				tableType = ACS_CS_INTERNAL_API::Table_CPGroupNames;
			}
			else if (value == "CPGMembersTable")
			{
				tableType = ACS_CS_INTERNAL_API::Table_CPGroupMembers;
			}
			else if (value == "FDTable")
			{
				tableType = ACS_CS_INTERNAL_API::Table_FD;
			}
		}
	}

	return tableType;
}


void ACS_CS_ServiceHandler::handleDHCPTable(const ACS_CS_ImBase *object)
{
	const ACS_CS_ImBlade * blade = dynamic_cast<const ACS_CS_ImBlade*>(object);

	if (blade)
	{
		if (ACS_CS_ImBase::DELETE == blade->action)
		{
			if (blade->dhcpOption == DHCP_NORMAL || blade->functionalBoardName == EPB1) {
				removeFromDHCPQueue(blade);
			}

			removeFromDHCPTable(blade);

		} else
		{
			const ACS_CS_ImBlade *oldBlade = dynamic_cast<const ACS_CS_ImBlade *>(ACS_CS_ImRepository::instance()->getModel()->getObject(blade->rdn.c_str()));
			if(oldBlade)
			{
				removeFromDHCPTable(oldBlade); //a new definition of modifying: remove old one and insert changed one
			}
			addToDHCPTable(blade);
		}
	}


}


void ACS_CS_ServiceHandler::handleDHCPTable(const ACS_CS_ImModel *model)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::handleDHCPTable()"));
	if (!model)
		return;

	std::set<const ACS_CS_ImBase *> objects;
	model->getObjects(objects);

	set<const ACS_CS_ImBase *>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBase * base = *it;

		const ACS_CS_ImBlade * blade = dynamic_cast<const ACS_CS_ImBlade*>(base);

		if (blade)
		{
			if (ACS_CS_ImBase::DELETE == blade->action)
			{
				if (blade->dhcpOption == DHCP_NORMAL || blade->functionalBoardName == EPB1
						|| blade->functionalBoardName == EVOET || blade->functionalBoardName == CMXB)
				{
					removeFromDHCPQueue(blade);
				}

				removeFromDHCPTable(blade);

			} else
			{
				const ACS_CS_ImBlade *oldBlade = dynamic_cast<const ACS_CS_ImBlade *>(ACS_CS_ImRepository::instance()->getModel()->getObject(blade->rdn.c_str()));

				if(oldBlade) //Blade already exists. Remove old reservation and make a new one
				{
					//For the Normal (and EVO) boards, do not remove old reservation.
					//It will be refreshed only if MAC address is changed
					if ((oldBlade->dhcpOption == DHCP_CLIENT) &&
							(oldBlade->functionalBoardName != EPB1) && (oldBlade->functionalBoardName != EVOET)
							&& (oldBlade->functionalBoardName != CMXB))
					{
						removeFromDHCPTable(oldBlade); //a new definition of modifying: remove old one and insert changed one
					}
				}
				std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
				addToDHCPTable(blade);
			}
		}
	}
}



void ACS_CS_ServiceHandler::addToDHCPTable(const ACS_CS_ImBlade *blade)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::addToDHCPTable()"));
	if (!blade)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
		return;
	}

	if(blade->functionalBoardName == DVD || blade->functionalBoardName == DISK || blade->functionalBoardName == GEA)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
		return;
	}

	if (!dhcp) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::addToDHCPTable() DhcpConfigurator object is NULL"));
		return;
	}

	ACS_CS_DHCP_Info infoA,infoB,infoAliasA,infoAliasB;	
	ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE;

	//
	//   ACS_CS_ImBlade *addressedBlade = NULL;

	// to be fixed: ipAddressEthA unset variable is not empty

	if(strlen(blade->ipAddressEthA.c_str()) < 7 || "0.0.0.0" == blade->ipAddressEthA || "0.0.0.0" == blade->ipAddressEthB)
	{
		ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
		if(model)
		{
			ACS_CS_ImIPAssignmentHelper helper;
			if (!helper.assignIp(model, const_cast<ACS_CS_ImBlade *>(blade)))
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::addToDHCPTable() assignIp failed!"));
		}
		delete model;
	}     
	 char trace[256] = {0};
	 snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d]", __FILE__, __FUNCTION__, __LINE__);
 	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "%s ", trace));
	
	if(blade->functionalBoardName == SMXB)
		switchType = ACS_CS_Protocol::CONTROL_SWITCH;

	// Create DHCP info objects from taddToDHCPTablehis entry (one for each
	if (createDHCPInfo(infoA, blade, ACS_CS_Protocol::Eth_A,switchType)
			&& createDHCPInfo(infoB, blade, ACS_CS_Protocol::Eth_B,switchType))
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"Create DHCP info objects from taddToDHCPTablehis entry"));
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;

		if(infoA.getFBN()==ACS_CS_Protocol::FBN_SMXB)
		{
			if (createDHCPInfo(infoAliasA, blade, ACS_CS_Protocol::Eth_A, ACS_CS_Protocol::TRANSPORT_SWITCH)
			&& createDHCPInfo(infoAliasB, blade, ACS_CS_Protocol::Eth_B, ACS_CS_Protocol::TRANSPORT_SWITCH))
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"Created DHCP info objects from taddToDHCPTablehis entry for infoAliasA, infoAliasB"));
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"Create DHCP info objects from taddToDHCPTablehis entry for infoAliasA, infoAliasB failed"));
				return;
			}
			
		}
		// If DHCP client boards
		if ((infoA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Client) &&
				(infoA.getFBN()!=ACS_CS_Protocol::FBN_EPB1 && infoA.getFBN()!=ACS_CS_Protocol::FBN_EvoET && infoA.getFBN()!=ACS_CS_Protocol::FBN_CMXB))
		{
			// Add reservations, one per backplane
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;

			
			if(infoA.getFBN()==ACS_CS_Protocol::FBN_SCXB)
			{
				std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
				dhcp->addScxbDhcpBoard(&infoA);
				dhcp->addScxbDhcpBoard(&infoB);

			}
			else if (infoA.getFBN()==ACS_CS_Protocol::FBN_SMXB) // Add 2 reservations, 2 per backplane
			{
				std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
				dhcp->addSmxbDhcpBoard(&infoA);
				dhcp->addSmxbDhcpBoard(&infoB);		
				dhcp->addSmxbDhcpBoard(&infoAliasA);
				dhcp->addSmxbDhcpBoard(&infoAliasB);
			}
			else
			{
				std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
				dhcp->addClientDhcpBoard(&infoA);
				dhcp->addClientDhcpBoard(&infoB);
			}
		}
		// If DHCP normal boards
		else if ((infoA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)|| (infoA.getFBN()==ACS_CS_Protocol::FBN_EPB1)
				|| (infoA.getFBN()==ACS_CS_Protocol::FBN_EvoET) || (infoA.getFBN()==ACS_CS_Protocol::FBN_CMXB))
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
			if (shelfManager) {
				shelfManager->insertIntoDhcpQueue(infoB);
				shelfManager->insertIntoDhcpQueue(infoA);

				shelfManager->wakeUpQueue();
			}
			else //No ShelfManager available, use MAC addresses stored in IMM
			{
				int macA_length = infoA.getMacLength();
				int macB_length = infoA.getMacLength();

				if ( macA_length > 0 )
				{

					dhcp->removeNormalDhcpBoardByMAC(&infoA);
					dhcp->removeNormalDhcpBoard(&infoA);
					dhcp->addNormalDhcpBoard(&infoA);
				}

				if (macB_length > 0)
				{
					dhcp->removeNormalDhcpBoardByMAC(&infoB);
					dhcp->removeNormalDhcpBoard(&infoB);
					dhcp->addNormalDhcpBoard(&infoB);
				}
			}

		}

		// If this board is an SCB-RP board itself we need to create a corresponding SNMP handler
		if((infoA.getFBN() == ACS_CS_Protocol::FBN_SCBRP) ||(infoA.getFBN() == ACS_CS_Protocol::FBN_SCXB) || (infoA.getFBN() == ACS_CS_Protocol::FBN_SMXB))
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::addToDHCPTable() Adding snmp handler"));
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << std::endl;
			if (shelfManager) shelfManager->addShelfHandler(infoA, infoB);
		}

	}

}

void ACS_CS_ServiceHandler::removeFromDHCPQueue(const ACS_CS_ImBlade * blade)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::removeFromDHCPQueue()"));

	if (!blade)
		return;

	ACS_CS_DHCP_Info infoA,infoAliasA;
	ACS_CS_DHCP_Info infoB,infoAliasB;
	ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE;

	infoA.setNetwork(ACS_CS_Protocol::Eth_A);	
	infoB.setNetwork(ACS_CS_Protocol::Eth_B);
	infoAliasA.setNetwork(ACS_CS_Protocol::Eth_A);
	infoAliasB.setNetwork(ACS_CS_Protocol::Eth_B);
       
	 char trace[256] = {0};
	 snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d]", __FILE__, __FUNCTION__, __LINE__);
	 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "%s ", trace));
	
	if(blade->functionalBoardName == SMXB)
		switchType = ACS_CS_Protocol::CONTROL_SWITCH;
		
	// Create DHCP info objects from this entry (one for each
	if (createDHCPInfo(infoA, blade, ACS_CS_Protocol::Eth_A,switchType)
			&& createDHCPInfo(infoB, blade, ACS_CS_Protocol::Eth_B,switchType))
	{
		//Remove possible pending request from DHCP queue
		if (shelfManager) shelfManager->cancelClient(infoA,infoB);
		if (infoA.getFBN() == ACS_CS_Protocol::FBN_SMXB)
		{
			if (createDHCPInfo(infoAliasA, blade, ACS_CS_Protocol::Eth_A, ACS_CS_Protocol::TRANSPORT_SWITCH)
			&& createDHCPInfo(infoAliasB, blade, ACS_CS_Protocol::Eth_B, ACS_CS_Protocol::TRANSPORT_SWITCH))
			{
				//Remove possible pending request from DHCP queue
				if (shelfManager) shelfManager->cancelClient(infoAliasA,infoAliasB);
			}			
		}
		
	}

}

void ACS_CS_ServiceHandler::removeFromDHCPTable(const ACS_CS_ImBlade * blade)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::removeFromDHCPTable()"));

	if (!blade)
		return;

	if (!dhcp) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::removeFromDHCPTable() DhcpConfigurator object is NULL"));
		return;
	}

	//ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port,dhcpSem);
	ACS_CS_DHCP_Info infoA,infoB,infoAliasA,infoAliasB;
	ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE;

	infoA.setNetwork(ACS_CS_Protocol::Eth_A);
	infoB.setNetwork(ACS_CS_Protocol::Eth_B);
	infoAliasA.setNetwork(ACS_CS_Protocol::Eth_A);
	infoAliasB.setNetwork(ACS_CS_Protocol::Eth_B);
 
	 char trace[256] = {0};
 	snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d]", __FILE__, __FUNCTION__, __LINE__);
 	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "%s ", trace));

	if(blade->functionalBoardName == SMXB)
		switchType = ACS_CS_Protocol::CONTROL_SWITCH;
		
	// Create DHCP info objects from this entry (one for each
	if (createDHCPInfo(infoA, blade, ACS_CS_Protocol::Eth_A,switchType)
			&& createDHCPInfo(infoB, blade, ACS_CS_Protocol::Eth_B,switchType))
	{
		// If DHCP client boards
		if (infoA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Client)
		{

			dhcp->removeClientDhcpBoard(&infoA);
			//sleep(1);
			dhcp->removeClientDhcpBoard(&infoB);
			if ((infoA.getFBN() == ACS_CS_Protocol::FBN_SMXB)) 
			{
				if (createDHCPInfo(infoAliasA, blade, ACS_CS_Protocol::Eth_A, ACS_CS_Protocol::TRANSPORT_SWITCH)
				&& createDHCPInfo(infoAliasB, blade, ACS_CS_Protocol::Eth_B, ACS_CS_Protocol::TRANSPORT_SWITCH))
				{
					dhcp->removeClientDhcpBoard(&infoAliasA);
					//sleep(1);
					dhcp->removeClientDhcpBoard(&infoAliasB);
				}
			}

		}
		// If DHCP normal boards
		else if (infoA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
		{
			dhcp->removeNormalDhcpBoard(&infoA);
			dhcp->removeNormalDhcpBoard(&infoB);
		}

		// If this board is an SCB-RP board itself we need to remove the corresponding SNMP handler
		if ((infoA.getFBN() == ACS_CS_Protocol::FBN_SCBRP)||(infoA.getFBN() == ACS_CS_Protocol::FBN_SCXB) || (infoA.getFBN() == ACS_CS_Protocol::FBN_SMXB))
		{		
			if (shelfManager) shelfManager->removeShelfHandler(infoA.getMagAddress(), infoA.getSlot());
		
			
		}
	}

}

void ACS_CS_ServiceHandler::tableOperationWorkerCleanup()
{
	tableWorkersLock.start_reading();

	for (size_t i = tableWorkers.size(); i > 0; i--)
	{
		if (tableWorkers[i - 1]->isFinished())
		{
			tableWorkersLock.stop_reading();
			tableWorkersLock.start_writing();

			delete tableWorkers[i - 1];
			tableWorkers.erase(tableWorkers.begin() + i - 1);

			tableWorkersLock.stop_writing();
			tableWorkersLock.start_reading();
		}
	}

	tableWorkersLock.stop_reading();
}


uint32_t ACS_CS_ServiceHandler::getIpForTftpAddress(ACS_CS_Protocol::CS_Network_Identifier network, bool smxTrapReceiver)
{

	bool addressIsLocal = false;

	uint32_t result;

	uint32_t ipA1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_A_HOSTNAME);
	uint32_t ipB1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_A_HOSTNAME);

	uint32_t ipA2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_B_HOSTNAME);
	uint32_t ipB2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_B_HOSTNAME);

	if(network == ACS_CS_Protocol::Eth_A)
	{
		if(smxTrapReceiver)
		{
			result = ACS_CS_NS::CLUSTER_IP_ETHA;
		}
		else
		{
			result = ipA1;

			addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipA1, 0);

			if (addressIsLocal == false)
			{
				result = ipB1;

				addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipB1, 0);

				if(addressIsLocal == false)
				{
					result=0;
				}
			}
		}
	}
	else
	{

		if(smxTrapReceiver)
		{
			result = ACS_CS_NS::CLUSTER_IP_ETHB;
		}
		else
		{
			result=ipA2;
			addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipA2, 0);

			if (addressIsLocal == false)
			{
				result = ipB2;

				addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipB2, 0);


				if(addressIsLocal == false)
				{
					result=0;
				}
			}
		}

	}

	return result;

	/*
	bool addressIsLocal = false;
	char *dotIpAddress = NULL;
	std::string sIpAddress;
	struct in_addr uIpAddress;

	 uint32_t ipA1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_A_HOSTNAME);
	 uint32_t ipB1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_A_HOSTNAME);

	 uint32_t ipA2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_B_HOSTNAME);
	 uint32_t ipB2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_B_HOSTNAME);

	 addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipA1, 0);
	 uIpAddress.s_addr = ipA1;

	 if (addressIsLocal == false)
	 {
	   addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipB1, 0);
	   uIpAddress.s_addr = ipB1;
	 }

     if (addressIsLocal == false)
     {
	   addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipA2, 0);
	   uIpAddress.s_addr = ipA2;
     }

     if (addressIsLocal == false)
     {
	   addressIsLocal = ACS_CS_Protocol::CS_ProtocolChecker::checkAddress(ipB2, 0);
	   uIpAddress.s_addr = ipB2;
     }

     if(addressIsLocal == true)
     {
    //	 uIpAddress.s_addr=htonl(uIpAddress.s_addr);

    	 dotIpAddress = inet_ntoa(uIpAddress);
    	 std::cout<< __FUNCTION__ << "@" << __LINE__ << " ip address of this node: " << dotIpAddress << std::endl;
    	 sIpAddress = dotIpAddress;
     }
     else
     {
    	 std::cout<< __FUNCTION__ << "@" << __LINE__ << " error: reading ip address is not possible. " << dotIpAddress << std::endl;
     }

     return uIpAddress.s_addr;
	 */
}


void ACS_CS_ServiceHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList)
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Admin Operation callback received. ID: %d on object DN: %s",
			__FUNCTION__, __LINE__, operationId, p_objName))

	SaAisErrorT retVal = SA_AIS_ERR_FAILED_OPERATION;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();

	acs_apgcc_adminoperation admin;
	vector<ACS_APGCC_AdminOperationParamType> vectorOUT;
	vectorOUT.clear();

	if (!model) {
		this->adminOperationResult(oiHandle , invocation, retVal);
		return;
	}

	ACS_CS_ImBase *base = model->getObject(p_objName);

	if (!base)  {
		this->adminOperationResult(oiHandle , invocation, retVal);
		return;
	}

        admin.init();

	if (base->type == OTHERBLADE_T) {

		ACS_CS_ImOtherBlade* blade = dynamic_cast<ACS_CS_ImOtherBlade*>(base);

		if (blade) {

			string shelfAddr = ACS_CS_ImUtils::getIdValueFromRdn(ACS_CS_ImUtils::getParentName(blade->rdn));

			ACS_CC_ReturnType result = ACS_CC_FAILURE;

			ACS_APGCC_ContinuationIdType continuation = 0;
			vector<ACS_APGCC_AdminOperationParamType> paramVector;
			paramVector.clear();
			int ret;

			stringstream slot;
			slot << blade->slotNumber;

			string objName = ACS_CS_ImmMapper::ATTR_BLADE_SW_PERS_ID +"=" + shelfAddr + "_" + slot.str() + "," + ACS_CS_ImmMapper::RDN_BLADE_SW_MGMT;

			cout << "Calling action on " << objName << endl;

			//fill parameters
			for (int i = 0; paramList[i] != 0; i++)
			{
				/*
				 typedef struct {
					char* attrName;
					ACS_CC_AttrValueType attrType;
					void* attrValues;
				}ACS_APGCC_AdminOperationParamType;
				 */
				ACS_APGCC_AdminOperationParamType parameter;

				//set ACS_APGCC_AdminOperationParamType field: char* attrName;
				parameter.attrName = paramList[i]->attrName;

				//set ACS_APGCC_AdminOperationParamType field: ACS_CC_AttrValueType attrType;
				parameter.attrType = paramList[i]->attrType;

				//set ACS_APGCC_AdminOperationParamType field: void* attrValues;
				parameter.attrValues = paramList[i]->attrValues;
				paramVector.push_back(parameter);
			}

			//Block/Deblock/Reset actions not supported for these types of OtherBlade
			if (blade->functionalBoardName != SCB_RP && blade->functionalBoardName != GESB)
			{
				if ((blade->functionalBoardName == IPLB) && (!isAP1))
				{
					ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Operation not allowed on AP2");
					vectorOUT.push_back(errorTextElem);
				}
				else
				{
				result = admin.adminOperationInvoke(objName.c_str(), continuation, operationId, paramVector, &ret, 0,vectorOUT);

				if (result == ACS_CC_SUCCESS)
					retVal = (SaAisErrorT) ret;
				else {
					cout << "Failed to invoke adminOperation. RETURN CODE: " << admin.getInternalLastError() << ", " << admin.getInternalLastErrorText() << endl;
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::adminOperationCallback() "
							"Failed to invoke adminOperation. RETURN CODE: %d.",admin.getInternalLastError()));

					ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Service not responding");
					vectorOUT.push_back(errorTextElem);
				}
			}
}
			else
			{
				cout << "Not allowed for this Functional Board Name."  << endl;
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::adminOperationCallback() "
						"Not allowed for this Functional Board Name: %d.",blade->functionalBoardName));

				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Action not allowed for this Functional Board Name (FBN)");
				vectorOUT.push_back(errorTextElem);
			}

		} else {
			cout << "Failed to fetch blade information" << endl;
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::adminOperationCallback() "
					"Failed to fetch blade information."));

			ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
			vectorOUT.push_back(errorTextElem);
		}


	} else if (base->type == APBLADE_T) {

		ACS_CS_ImApBlade* blade = dynamic_cast<ACS_CS_ImApBlade*>(base);

		if (blade) {

			string shelfRdn = ACS_CS_ImUtils::getParentName(blade->rdn);

			OmHandler omHandler;

			if (omHandler.Init() == ACS_CC_SUCCESS) {

				ACS_CC_ReturnType result = ACS_CC_FAILURE;

				std::vector<std::string> rdnList;

				result = omHandler.getChildren(blade->rdn.c_str(), ACS_APGCC_SUBLEVEL, &rdnList);

				omHandler.Finalize();

				if (result != ACS_CC_SUCCESS) {
					cout << "omHandler.getChildren Failed! RC: " << omHandler.getInternalLastError() << ", " << omHandler.getInternalLastErrorText() << endl;

					ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
					vectorOUT.push_back(errorTextElem);

				} else {

					string apBladeInfoDn = "";

					for (unsigned int i=0; i < rdnList.size(); i++) {
						string className = ACS_APGCC::before(rdnList[i],(string) "=");

						if (className.compare(ACS_CS_ImmMapper::ATTR_AP_BLADE_INFO_ID) == 0) {
							apBladeInfoDn = rdnList[i];
							break;
						}
					}

					if (apBladeInfoDn.size() > 0) {

						ACS_APGCC_ContinuationIdType continuation = 0;
						vector<ACS_APGCC_AdminOperationParamType> paramVector;
						paramVector.clear();
						int ret;

						cout << "Calling action on " << apBladeInfoDn << endl;

						result = admin.adminOperationInvoke(apBladeInfoDn.c_str(), continuation, operationId, paramVector, &ret, 0,vectorOUT);

						if (result == ACS_CC_SUCCESS)
							retVal = (SaAisErrorT) ret;
						else {
							cout << "Failed to invoke adminOperation. RETURN CODE: " << admin.getInternalLastError() << ", " << admin.getInternalLastErrorText() << endl;
							ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::adminOperationCallback() "
									"Failed to invoke adminOperation. RETURN CODE: %d, %s.",admin.getInternalLastError(), admin.getInternalLastErrorText()));

							ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Service not responding");
							vectorOUT.push_back(errorTextElem);
						}


					} else {
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::adminOperationCallback() "
								"ERROR: No AxeApBladeInfo object found."));

						ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Service not responding");
						vectorOUT.push_back(errorTextElem);

					}

				}

			} else
			{
				cout << "omHandler.Init Failed! RC: " << omHandler.getInternalLastError() << ", " << omHandler.getInternalLastErrorText() << endl;

				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
				vectorOUT.push_back(errorTextElem);
             }

		}
		else {
			cout << "Failed to fetch blade information" << endl;
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::adminOperationCallback() "
					"Failed to fetch blade information."));

			ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
			vectorOUT.push_back(errorTextElem);

		}

	}


	/* ************************* */
	/* CP CLUSTER OBJECT ACTIONS */
	/* ************************* */
	else if (isAP1 && (base->type == CPCLUSTER_T)){
		retVal = SA_AIS_OK;

		ACS_CS_OperationInfo *actionInfo = 0;
		int execute_action = 1;

		switch (operationId)
		{

			case ACS_CS_ImmMapper::CP_CLUSTER_OP_CHANGE_CLUSTER_OP_MODE_ID: //1 - changeClusterOpMode(clusterOpModeRequested)
				actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OP_MODE_CHANGE);
				if (!actionInfo)
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
							"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
					execute_action = 0;
					retVal = SA_AIS_ERR_BUSY;
					ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
					vectorOUT.push_back(errorTextElem);
					break;
				}
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_OP_MODE_CHANGE", __FUNCTION__, __LINE__));
				actionInfo->clusterOpModeRequested = *reinterpret_cast<int *>(paramList[0]->attrValues);
				break;

			default:
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "[%s@%d] Failed to invoke adminOperation (operationId == %d).",
						__FUNCTION__, __LINE__, operationId));
				execute_action = 0;
				retVal = SA_AIS_ERR_BAD_OPERATION;
				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
				vectorOUT.push_back(errorTextElem);
				break;
		}

		if (actionInfo && base->type == CPCLUSTER_T) actionInfo->ObjectType = CPCLUSTER_T;
		if (actionInfo) actionInfo->OperationId = operationId;  //save operation ID

		/**				  *
		 * EXECUTE ACTION *
		 * 				  */
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Executing Action.", __FUNCTION__, __LINE__));
		if (execute_action)
		{

			if( (SA_AIS_OK == retVal) && (cpClusterManager->wakeUpEvent(actionInfo) == false) )
			{
				//Internal Error
				retVal = SA_AIS_ERR_TRY_AGAIN;
				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
				vectorOUT.push_back(errorTextElem);
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,
						"[%s@%d] Failed to invoke adminOperation (operationId == %d). Internal Error.",
						__FUNCTION__, __LINE__, operationId));
			}


		}
		else
		{
			//A previous request is ongoing
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_WARN,
					"[%s@%d] Failed to invoke adminOperation (operationId == %d). A previous request is ongoing.",
					__FUNCTION__, __LINE__, operationId));
			if (actionInfo) actionInfo->release();
		}

	}


	/* ************************************* */
	/*  OMPROFILEMANAGER OBJECT ACTIONS      */
	/* ************************************* */
	else if (isAP1 && base->type == OMPROFILEMANAGER_T) {
		retVal = SA_AIS_OK;

		ACS_CS_OperationInfo *actionInfo = 0;
		int execute_action = 1;

		switch (operationId)
		{
			case ACS_CS_ImmMapper::CP_CLUSTER_OP_CHANGE_OM_PROFILE_ID: //boolean changeOmProfile ( int16 omProfileRequested );
					actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OM_PROFILE_CHANGE);
					if (!actionInfo)
					{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
								"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
						execute_action = 0;
						retVal = SA_AIS_ERR_BUSY;
						ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
						vectorOUT.push_back(errorTextElem);
						break;
					}
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_OM_PROFILE_CHANGE", __FUNCTION__, __LINE__));
					actionInfo->omProfileRequested = *reinterpret_cast<int *>(paramList[0]->attrValues);
				break;

			case ACS_CS_ImmMapper::CP_CLUSTER_OP_REMOVE_OM_PROFILE_ID:
				//void	removeOmProfiles ( );
				actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OM_REMOVE);
				if (!actionInfo)
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
							"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
					execute_action = 0;
					retVal = SA_AIS_ERR_BUSY;
					ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
					vectorOUT.push_back(errorTextElem);
					break;
				}
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_OM_REMOVE", __FUNCTION__, __LINE__));
				break;

			case ACS_CS_ImmMapper::CP_CLUSTER_OP_ACTIVE_OM_PROFILE_ID:
					actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OM_ACTIVE);
					if (!actionInfo)
					{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
								"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
						execute_action = 0;
						retVal = SA_AIS_ERR_BUSY;
						ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
						vectorOUT.push_back(errorTextElem);
						break;
					}
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_OM_ACTIVE", __FUNCTION__, __LINE__));
				break;

			case ACS_CS_ImmMapper::CP_CLUSTER_OP_EXPORT_CCF_ID:
				//void	exportCcFile ( );
				actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_CCF_EXPORT);
				if (!actionInfo)
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
							"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
					execute_action = 0;
					retVal = SA_AIS_ERR_BUSY;
					ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
					vectorOUT.push_back(errorTextElem);
					break;
				}
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_CCF_EXPORT", __FUNCTION__, __LINE__));
				break;

			case ACS_CS_ImmMapper::CP_CLUSTER_OP_ROLLBACK_FILE_ID:
					actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_CCF_ROLLBACK);
					if (!actionInfo)
					{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
								"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
						execute_action = 0;
						retVal = SA_AIS_ERR_BUSY;
						ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
						vectorOUT.push_back(errorTextElem);
						break;
					}
					ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_CCF_ROLLBACK", __FUNCTION__, __LINE__));
				break;
			default:
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "[%s@%d] Failed to invoke adminOperation (operationId == %d).",
						__FUNCTION__, __LINE__, operationId));
				execute_action = 0;
				retVal = SA_AIS_ERR_BAD_OPERATION;
				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
				vectorOUT.push_back(errorTextElem);
				break;
		}

		if (actionInfo && base->type == OMPROFILEMANAGER_T) actionInfo->ObjectType = OMPROFILEMANAGER_T;
		if (actionInfo) actionInfo->OperationId = operationId;  //save operation ID

		/**				  *
		 * EXECUTE ACTION *
		 * 				  */
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Executing Action.", __FUNCTION__, __LINE__));
		if (execute_action)
		{

			if( (SA_AIS_OK == retVal) && (cpClusterManager->wakeUpEvent(actionInfo) == false) )
			{
				//Internal Error
				retVal = SA_AIS_ERR_TRY_AGAIN;
				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
				vectorOUT.push_back(errorTextElem);
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,
						"[%s@%d] Failed to invoke adminOperation (operationId == %d). Internal Error.",
						__FUNCTION__, __LINE__, operationId));
			}


		}
		else
		{
			//A previous request is ongoing
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_WARN,
					"[%s@%d] Failed to invoke adminOperation (operationId == %d). A previous request is ongoing.",
					__FUNCTION__, __LINE__, operationId));
			if (actionInfo) actionInfo->release();
		}

	}

	/* ************************************* */
	/*  CANDIDATECCFILE OBJECT ACTIONS      */
	/* ************************************* */
	else if (isAP1 && base->type == CANDIDATECCFILE_T) {
		retVal = SA_AIS_OK;

		ACS_CS_OperationInfo *actionInfo = 0;
		int execute_action = 1;

		switch (operationId)
		{
		case ACS_CS_ImmMapper::CP_CLUSTER_OP_IMPORT_CCF_ID:
			//void importCcFile ( string filename );
			cout << __FUNCTION__ << " MT_CCF_IMPORT "<< endl;
			actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_CCF_IMPORT);
			if (!actionInfo)
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
						"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
				execute_action = 0;
				retVal = SA_AIS_ERR_BUSY;
				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
				vectorOUT.push_back(errorTextElem);
				break;
			}
			{
				//check parameter
				actionInfo->filename = ACS_CS_ImUtils::getIdValueFromRdn(base->rdn);
			}
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "(%t) [%s@%d] Created MT_CCF_IMPORT", __FUNCTION__, __LINE__));
			break;

		default:
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "[%s@%d] Failed to invoke adminOperation (operationId == %d).",
					__FUNCTION__, __LINE__, operationId));
			execute_action = 0;
			retVal = SA_AIS_ERR_BAD_OPERATION;
			ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Error when executing (general fault)");
			vectorOUT.push_back(errorTextElem);
			break;
		}

		if (actionInfo && base->type == CANDIDATECCFILE_T) actionInfo->ObjectType = CANDIDATECCFILE_T;
		if (actionInfo) actionInfo->OperationId = operationId;  //save operation ID

		/**				  *
		 * EXECUTE ACTION *
		 * 				  */
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Executing Action.", __FUNCTION__, __LINE__));
		if (execute_action)
		{

			if( (SA_AIS_OK == retVal) && (cpClusterManager->wakeUpEvent(actionInfo) == false) )
			{
				//Internal Error
				retVal = SA_AIS_ERR_TRY_AGAIN;
				ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Not allowed during Cluster Operation ongoing.");
				vectorOUT.push_back(errorTextElem);
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,
						"[%s@%d] Failed to invoke adminOperation (operationId == %d). Internal Error.",
						__FUNCTION__, __LINE__, operationId));
			}


		}
		else
		{
			//A previous request is ongoing
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_WARN,
					"[%s@%d] Failed to invoke adminOperation (operationId == %d). A previous request is ongoing.",
					__FUNCTION__, __LINE__, operationId));
			if (actionInfo) actionInfo->release();
		}

	}

	/* ************************************* */
	/* ADVANCED CONFIGURATION OBJECT ACTIONS */
	/* ************************************* */
	else if (isAP1 && base->type == ADVANCEDCONFIGURATION_T)
	{
		retVal = SA_AIS_OK;
		ACS_CS_ImAdvancedConfiguration * advancedConfigurationMO = dynamic_cast <ACS_CS_ImAdvancedConfiguration*> (base);
		if (!advancedConfigurationMO)
		{
			//Log error
			char tracep[256] = {0};
			snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot cast object type ADVANCEDCONFIGURATION_T", __FILE__, __FUNCTION__, __LINE__);
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, tracep));
			std::cout << "DBG: " << tracep << std::endl;
		}
		else
		{

			ACS_CS_OperationInfo *actionInfo = 0;
			int execute_action = 1;

			//Create OperationInfo
			switch(operationId)
			{
			case (ACS_CS_ImmMapper::ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_SUCCESS):
					actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OM_PROFILE_NOTIFICATION_SUCCESS);
					if (!actionInfo)
					{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
								"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
						execute_action = 0;
						retVal = SA_AIS_ERR_BUSY;
						break;
					}
					actionInfo->OperationId = operationId;  //save operation ID
			case (ACS_CS_ImmMapper::ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_FAILURE):
					if (!actionInfo)
					{
						actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OM_PROFILE_NOTIFICATION_FAILURE);
						actionInfo->OperationId = operationId;  //save operation ID
					}
					for (int i = 0; paramList[i] != 0; i++)
					{
						if (ACE_OS::strcmp(paramList[i]->attrName, ACTION_PHASE_PARAMETER) == 0)
						{
							actionInfo->profilePhase = *(reinterpret_cast <int *>(paramList[i]->attrValues));
						}
						else if (ACE_OS::strcmp(paramList[i]->attrName, ACTION_REASON_PARAMETER) == 0)
						{
							actionInfo->profileReason = *(reinterpret_cast <int *>(paramList[i]->attrValues));
						}
					}

					break;

			case (ACS_CS_ImmMapper::ADVANCED_CONFIGURATION_OP_OM_PROFILE_APA_NOTIFICATION):
					actionInfo = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_OM_PROFILE_CHANGE);
					if (!actionInfo)
					{
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
								"ERROR: Cannot create ACS_CS_OperationInfo",__FUNCTION__, __LINE__, operationId, p_objName));
						execute_action = 0;
						retVal = SA_AIS_ERR_BUSY;
						break;
					}
					actionInfo->OperationId = ACS_CS_ImmMapper::CP_CLUSTER_OP_CHANGE_OM_PROFILE_ID;  //save operation ID
					//fill parameters
					for (int i = 0; paramList[i] != 0; i++)
					{
						if (ACE_OS::strcmp(paramList[i]->attrName, ACTION_APZ_PARAMETER) == 0)
						{
							actionInfo->apzProfileRequested = *(reinterpret_cast <int *>(paramList[i]->attrValues));
						}
						else if (ACE_OS::strcmp(paramList[i]->attrName, ACTION_APT_PARAMETER) == 0)
						{
							actionInfo->aptProfileRequested = *(reinterpret_cast <int *>(paramList[i]->attrValues));
						}
					}

					break;

			default:
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Operation ID: %d on object DN: %s "
						"NOT RECOGNIZED",__FUNCTION__, __LINE__, operationId, p_objName));
				execute_action = 0;
				retVal = SA_AIS_ERR_BUSY;
				break;
			}

			if (actionInfo) actionInfo->ObjectType = ADVANCEDCONFIGURATION_T;

			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Dispatching ID: %d on object DN: %s",
					__FUNCTION__, __LINE__, operationId, p_objName));

			/**				  *
			 * EXECUTE ACTION *
			 * 				  */
			if (execute_action && 1 /*!cpClusterManager->isRunningThread()*/)
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Waking Up Thread!",
						__FUNCTION__, __LINE__, operationId, p_objName))

				if (cpClusterManager->wakeUpEvent(actionInfo) == false)
				{
					//Internal Error
					retVal = SA_AIS_ERR_TRY_AGAIN;
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,
							"[%s@%d] Failed to invoke adminOperation (operationId == %d). Internal Error.",
							__FUNCTION__, __LINE__, operationId));
				}
			}
			else
			{
				//A previous request is ongoing
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_WARN,
						"[%s@%d] Failed to invoke adminOperation (operationId == %d). A previous request is ongoing.",
						__FUNCTION__, __LINE__, operationId));
				std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " ONGOING" << std::endl;
				if (actionInfo) actionInfo->release();
			}
		}
	}
	else if (base->type == FUNCTIONDISTRIBUTION_T)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] adminOperation moveFunction (operationId == %d)",
												__FUNCTION__, __LINE__, operationId));

		// AP FUNCTION MOVE
		std::string apName;
		int functionIdx = -1;

		//Get action parameters
		for(int i = 0; paramList[i] != 0; ++i)
		{
			if (ACE_OS::strcmp(paramList[i]->attrName, moveFunctionParameter::function) == 0)
			{
				functionIdx = *(reinterpret_cast<unsigned int*>(paramList[i]->attrValues));
				continue;
			}

			if (ACE_OS::strcmp(paramList[i]->attrName, moveFunctionParameter::apgNode) == 0)
			{
				int apId = *(reinterpret_cast<unsigned int*>(paramList[i]->attrValues));

				if(1 == apId)
					apName = moveFunctionParameter::ap1Name;
				else
					apName = moveFunctionParameter::ap2Name;

				continue;
			}
		}
		if( (functionIdx != -1) && !apName.empty() )
			retVal = static_cast<SaAisErrorT>(FuncDistHandler::instance()->executeActionMoveFunction(functionIdx, apName ));
		else
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] error on get moveFunction parameters: funId<%d>, apgname:<%s>)",
															__FUNCTION__, __LINE__, functionIdx, apName.c_str()));
			retVal = SA_AIS_ERR_INVALID_PARAM;
			ACS_APGCC_AdminOperationParamType errorTextElem = ACS_CS_ImUtils::createAdminOperationErrorParam("Invalid Parameters");
			vectorOUT.push_back(errorTextElem);
		}
	}
	else if (base->type == EQUIPMENT_T)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] adminOperation EquipmentM (operationId == %d)",
				__FUNCTION__, __LINE__, operationId));

		if (operationId == ACS_CS_ImmMapper::SERVICE_IPTABLE_RESTART)
		{
			retVal = SA_AIS_OK;

			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] adminOperation EquipmentM -> SERVICE_IPTABLE_RESTART",__FUNCTION__, __LINE__));

			if (hwcTable)
			{
				(void) hwcTable->loadTable(); // Load HWC table last
				ACS_CS_FTPAccessHandler::instance()->enableCPandAPBladesForFTP(hwcTable->getTable());
			}
		}

	}


	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Action return: %d", __FUNCTION__, __LINE__, retVal));

	
	this->adminOperationResult(oiHandle, invocation,retVal,vectorOUT);
	admin.freeadminOperationReturnValue(vectorOUT);
	admin.finalize();

	return;
}


bool ACS_CS_ServiceHandler::isIpAddress(const string ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}


bool ACS_CS_ServiceHandler::addEBSBoardToDHCPTable(ACS_CS_DHCP_Info *info)
{
	//ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port,dhcpSem);

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "ACS_CS_ServiceHandler::addEBSBoardToDHCPTable()"));
	bool result = false;

	if (!dhcp) {
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_ServiceHandler::addEBSBoardToDHCPTable() DhcpConfigurator object is NULL"));
		return result;
	}

	info->calcOption61Id();

	if (info->getFBN() == ACS_CS_Protocol::FBN_EPB1 || info->getFBN() == ACS_CS_Protocol::FBN_EvoET)
	{
		result = dhcp->addEBSClientDhcpBoard(info);
	}
	else if (info->getFBN() == ACS_CS_Protocol::FBN_CMXB)
	{
		result = dhcp->addCmxbDhcpBoard(info);
	}

	return result;

}



void ACS_CS_ServiceHandler::addToDHCPQueue(ACS_CS_DHCP_Info &infoA,ACS_CS_DHCP_Info &infoB)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::addToDHCPQueue"));
	if (shelfManager) {
		shelfManager->insertIntoDhcpQueue(infoB);
		shelfManager->insertIntoDhcpQueue(infoA);
		shelfManager->wakeUpQueue();
	}
}


int ACS_CS_ServiceHandler::startTrapManager ()
{
	int result = 0;

	//just for TEST !!!

	//usleep(300000);

	(void)trapHandler->activate();

	return result;

}

void ACS_CS_ServiceHandler::handleBoardPresenceTrap(int lShelfNum, int lSlotPos)
{
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
					"Received Board presence Trap. Shelf: %d, Slot: %d.", lShelfNum, lSlotPos));

	if (model)
	{
		std::string shelfId = "";
		std::string shelfAddr = "";
		bool shelfFound = false;

		std::ostringstream shelfNum("");
		shelfNum << lShelfNum;

		set<const ACS_CS_ImBase*> shelfs;
		set<const ACS_CS_ImBase*>::iterator it;
		model->getObjects(shelfs, SHELF_T);

		for (it = shelfs.begin(); it != shelfs.end(); it++) {
			const ACS_CS_ImShelf* shelf = dynamic_cast<const ACS_CS_ImShelf*>(*it);
			if (shelf) {
				std::string plg3 = ACS_APGCC::before(shelf->address, (string) ".");
				if(plg3.compare(shelfNum.str()) == 0) {
					shelfId = shelf->axeShelfId;
					shelfAddr = shelf->address;
					shelfFound = true;
				}
			}

		}

		if (!shelfFound)
		{
			std::cout << "Unable to fetch ShelfId" << endl;
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
					"Failed to fetch Shelf address for Shelf: %d.", lShelfNum));
		}
		else
		{
			char slot[2]={0};
			std::string otherBlade_dn, cpBlade_dn;
			std::string shelf_dn = shelfId + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

			sprintf(slot,"%d",lSlotPos);

			//Check if the board is defined
			const ACS_CS_ImBase *object = NULL;

			object = model->getBladeObject(shelfAddr, lSlotPos);

			if (object == NULL) {
				std::cout << "Board not defined" << endl;
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
						"Unhandled Board presence Trap for an undefined board. Shelf: %d, Slot: %d.", lShelfNum, lSlotPos));
			} else {

				if (object->type == CPBLADE_T ) {
					const ACS_CS_ImCpBlade *blade;
					blade = dynamic_cast<const ACS_CS_ImCpBlade *>(object);

					if (blade && blade->functionalBoardName == CPUB && blade->dhcpOption == DHCP_NORMAL) {

						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
								"Handling Board presence Trap for a CP board. Shelf: %d, Slot: %d.", lShelfNum, lSlotPos));
						if(isAP1)
							addToDHCPTable(blade);
						else
						{
							ACS_CS_ImModel tmpModel;
							ACS_CS_ImBase *tmpBlade = blade->clone();
							tmpBlade->action = ACS_CS_ImBase::MODIFY;
							tmpModel.addObject(tmpBlade);

							ACS_CS_ImModelSaver saver(&tmpModel);
							saver.save("DHCP-Reservation");
						}
						//handleDHCPTable(object);
					}

				} else if (object->type == OTHERBLADE_T ) {
					const ACS_CS_ImOtherBlade *blade;
					blade = dynamic_cast<const ACS_CS_ImOtherBlade *>(object);
					
					if (blade && (blade->dhcpOption == DHCP_NORMAL /*HU64120*/ || blade->functionalBoardName == EPB1 || blade->functionalBoardName == EVOET || blade->functionalBoardName == CMXB)) {
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
								"Handling Board presence Trap for an EPB1 board. Shelf: %d, Slot: %d.", lShelfNum, lSlotPos));

						if(isAP1)
							addToDHCPTable(blade);
						else
						{
							ACS_CS_ImModel tmpModel;
							ACS_CS_ImBase *tmpBlade = blade->clone();
							tmpBlade->action = ACS_CS_ImBase::MODIFY;
							tmpModel.addObject(tmpBlade);

							ACS_CS_ImModelSaver saver(&tmpModel);
							saver.save("DHCP-Reservation");
						}
					}
				}

			}
		}

		delete model;
	}
}

void ACS_CS_ServiceHandler::LogicalObjCleanUp() {
	//Look for object that may need deletion/////////////////////////////////

	bool saveModel = false;

	ACS_CS_ImModel* tmpModel = new ACS_CS_ImModel();
	if (!tmpModel)
		return;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();

	if (!model)
		return;

	std::set<const ACS_CS_ImBase*> baseCps;
	model->getObjects(baseCps, DUALSIDEDCP_T);

	std::set<const ACS_CS_ImBase*> baseAps;
	model->getObjects(baseAps, AP_T);

	const ACS_CS_ImBase * cpClusterBase = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

	std::set<const ACS_CS_ImBase*>::iterator it;

	for (it = baseCps.begin(); it != baseCps.end(); it++) {
		const ACS_CS_ImBase* baseCp = *it;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(baseCp);

		if (cp && cp->axeCpBladeRefTo.size() == 0) {
			cout << "Found CP to delete " << baseCp->rdn << endl;
			ACS_CS_ImDualSidedCp* remCp = new ACS_CS_ImDualSidedCp(cp);
			remCp->action = ACS_CS_ImBase::DELETE;
			tmpModel->addObject(remCp);
			saveModel = true;
		}
	}

	for (it = baseAps.begin(); it != baseAps.end(); it++) {
		const ACS_CS_ImBase* baseAp = *it;
		const ACS_CS_ImAp* ap = dynamic_cast<const ACS_CS_ImAp*>(baseAp);

		if (ap && ap->axeApBladeRefTo.size() == 0 && ap->axeApgId.compare(ACS_CS_ImmMapper::ATTR_APG_ID + "=AP1") != 0) {
			cout << "Found AP to delete " << baseAp->rdn << endl;
			ACS_CS_ImAp* remAp = new ACS_CS_ImAp(*ap);
			remAp->action = ACS_CS_ImBase::DELETE;
			tmpModel->addObject(remAp);
			saveModel = true;
		}
	}

	if (cpClusterBase && (!isBladeCluster)) {
		const ACS_CS_ImCpCluster* cp = dynamic_cast<const ACS_CS_ImCpCluster*>(cpClusterBase);

		if (cp && cp->allBcGroup.size() == 0) {
			cout << "Found CpCluster to delete " << cpClusterBase->rdn << endl;
			ACS_CS_ImCpCluster* remCp = new ACS_CS_ImCpCluster(*cp);
			remCp->action = ACS_CS_ImBase::DELETE;
			tmpModel->addObject(remCp);
			saveModel = true;
		}
	}


	if (saveModel) {
		ACS_CS_ImModelSaver* saver = new ACS_CS_ImModelSaver(tmpModel);
		if (saver) {
			ACS_CC_ReturnType result = saver->save("LogicalCleanUp");
			if (result != ACS_CC_SUCCESS)
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::LogicalObjCleanUp() Failed to remove logical objects."));

			delete saver;
		}
	}

	//////////////////////////////////////////////

	delete tmpModel;
	delete model;
}

void ACS_CS_ServiceHandler::LogicalObjCleanUp_vAPZ() {
	//Look for object that may need deletion/////////////////////////////////

	bool saveModel = false;

	ACS_CS_ImModel* tmpModel = new ACS_CS_ImModel();
	if (!tmpModel)
		return;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();

	if (!model)
		return;

	std::set<const ACS_CS_ImBase*> baseCps;
	model->getObjects(baseCps, DUALSIDEDCP_T);

	std::set<const ACS_CS_ImBase*>::iterator it;

	for (it = baseCps.begin(); it != baseCps.end(); it++) {
		const ACS_CS_ImBase* baseCp = *it;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(baseCp);

		if (cp && cp->axeCpBladeRefTo.size() == 0) {
			cout << "Found CP to delete " << baseCp->rdn << endl;
			ACS_CS_ImDualSidedCp* remCp = new ACS_CS_ImDualSidedCp(cp);
			remCp->action = ACS_CS_ImBase::DELETE;
			tmpModel->addObject(remCp);
			saveModel = true;
		}
	}

	if (saveModel) {
		ACS_CS_ImModelSaver* saver = new ACS_CS_ImModelSaver(tmpModel);
		if (saver) {
			ACS_CC_ReturnType result = saver->save("LogicalCleanUp");
			if (result != ACS_CC_SUCCESS) {
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::LogicalObjCleanUp() Failed to remove logical objects."));
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[] - Failed to save the LogicalMgmt MO subtree cleanup operations in IMM"));
			}
			else
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[] - SUCCESS LogicalMgmt MO subtree cleanup operations in IMM"));
			delete saver;
		}
	}

	delete tmpModel;
	delete model;
}

void ACS_CS_ServiceHandler::initImm()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::initImm()"));

	std::string implementerName = "CSSRV";

	setImpName(implementerName);

	classNames.push_back(ACS_CS_ImmMapper::CLASS_EQUIPMENT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_HARDWARE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_ADVANCED_CONF);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_LOGICAL);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_SHELF);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_APG);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CLUSTER_CP);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_CLUSTER);



	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_AP_SERVICE_CATEGORY);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_VLAN_CATEGORY);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_BLADE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OTHER_BLADE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_AP_BLADE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_AP_SERVICE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_VLAN);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OM_PROFILE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CCFILE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OM_PROFILE_STRUCT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CCFILE_MANAGER);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CANDIDATE_CCFILE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CANDIDATE_STRUCT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_PRODUCT_INFO);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CRMGMT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE_NETWORK);
//	classNames.push_back(ACS_CS_ImmMapper::CLASS_IPLB_CLUSTER);


	classNames.push_back(ACS_CS_ImmMapper::CLASS_FUNCTION_DIST);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_APGNODE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_FUNCTION);


	classNames.push_back(ACS_CS_ImmMapper::CLASS_CRM_EQUIPMENT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CRM_COMPUTE_RESOURCE);

	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_TRANSPORT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_HOST);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_INTERFACE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_EXTERNALNETWORK);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_INTERNALNETWORK);

	int timeout = 1;
	int retry = 0;
	ACS_CS_EventHandle handleArr[1] = {shutdownHandlerEvent};

	while((retry <= 10) && (!exit_))			// HX59136
	{
		int returnValue = ACS_CS_Event::WaitForEvents( 1, handleArr, timeout);

		if (returnValue == ACS_CS_EVENT_RC_ERROR)		// Event failed
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR, "(%t) ACS_CS_ServiceHandler::initImm() Error: WaitForEvents"));
		}
		else if (returnValue == ACS_CS_EVENT_RC_TIMEOUT)	// Wait timeout
		{
			if (ACS_CC_SUCCESS != oiHandler.addMultipleClassImpl(this, classNames))
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::initImm() addMultipleClassImpl failed!!"));
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] ACS_CS_ServiceHandler::initImm() addMultipleClassImpl failed!!", __FUNCTION__, __LINE__));

				retry++;
				timeout = 100 * exp2(retry);
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"ACS_CS_ServiceHandler::initImm() addMultipleClassImpl successful !!!"));

				immWorker = new ACS_CS_IMMCallbackThread(getSelObj(), this);
				immWorker->activate();

				break;
			}
		}
		else if (returnValue == 0)			// Shutdown event
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,"(%t) ACS_CS_ServiceHandler::initImm() Shutdown event signaled. Closing down ServiceHandler session."));
			exit_ = true;					// Leave exec() method
			break;
		}
	}

	// start of HX59136
	if(exit_)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,"initImm() - ServiceHandler thread exit_ set (deactivate() called from HA thread) - stop initializing IMM handles/implementers"));
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO,"initImm() - Set 'isThreadRunning' to true for HA thread exit condition"));
		thread_running = true;
	}
	// end of HX59136
}


void ACS_CS_ServiceHandler::createCpProductInfo(const char* p_objName, ACS_CS_ProductInfo &info)
{

	std::string struct_dn = ACS_CS_ImmMapper::RDN_CP_PRODUCT_INFO + "," + std::string(p_objName) ;

	std::cout << "Entering method createCpProductInfo" << std::endl;

	ACS_CS_ImModel tmpModel;

	ACS_CS_ImCpProductInfo* prod = new ACS_CS_ImCpProductInfo();

	prod->rdn = struct_dn;

	cout << "Prod Info DN " << struct_dn << std::endl;

	prod->cpProductInfoId =	ACS_CS_ImmMapper::RDN_CP_PRODUCT_INFO;
	prod->productRevision = info.productRevision;
	prod->productName = info.productName;
	cout<<"prodName"<<prod->productName<<endl;
	prod->manufacturingDate = info.manufacturingDate;
	prod->productVendor = info.productVendor;
	prod->productNumber = info.productNumber;
	prod->serialNumber = info.serialNumber;

	prod->action = ACS_CS_ImBase::CREATE;

	tmpModel.addObject(prod);

	ACS_CS_ImModelSaver saver(&tmpModel);

	ACS_CC_ReturnType ret;

	if ((ret = saver.save("Create_CpProductInfo")) != ACS_CC_SUCCESS)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] IMM Error: Failed to create CpProductInfo struct %s",
						__FUNCTION__, __LINE__, struct_dn.c_str()));

		std::cout << "Failed to create CpProductInfo struct " << struct_dn << std::endl;
	}


}

void ACS_CS_ServiceHandler::updateProductInfoAttr(const char* p_objName, bool empty)
{
	std::string struct_dn = ACS_CS_ImmMapper::RDN_CP_PRODUCT_INFO + "," + std::string(p_objName);

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Updating productInfo attribute on %s",
					__FUNCTION__, __LINE__, p_objName));

	std::cout << "Updating productInfo attribute on  " << p_objName << std::endl;

	ACS_CC_ImmParameter hwVersionParam;
	hwVersionParam.attrName = const_cast<char*>("productInfo");
	hwVersionParam.attrType = ATTR_NAMET;

	if (empty)
	{
		hwVersionParam.attrValuesNum = 0;
		void *attrVal[0] = {};
		hwVersionParam.attrValues = attrVal;
	}
	else
	{
		hwVersionParam.attrValuesNum = 1;
		void *attrVal[1] = {reinterpret_cast<void*>(const_cast<char*>(struct_dn.c_str()))};
		hwVersionParam.attrValues = attrVal;
	}

	if (modifyRuntimeObj(p_objName, &hwVersionParam) != ACS_CC_SUCCESS)
	{
		//Don't trace the error if the parent doesn't exist
		if (getInternalLastError() != (-1)*SA_AIS_ERR_NOT_EXIST)
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] IMM Error: Failed to update productInfo attribute on %s",
					__FUNCTION__, __LINE__, p_objName));

			std::cout << "Failed to update productInfo attribute on " << p_objName << std::endl;
		}
	}
}

void ACS_CS_ServiceHandler::initStaticIpMap()
{

	//fetch model
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	//fetch SCX addresses from staticIpMap
	ACS_CS_ImHardwareMgmt* hwMgmt = dynamic_cast<ACS_CS_ImHardwareMgmt*> (model->getObject(ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));
	if (!hwMgmt)
	{
		//Error
		if(model)
			delete model;
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] IMM Error: HardwareMgmt does not exist in AxeEquipment", __FUNCTION__, __LINE__));
		return;
	}

	std::set<string> tmpMap = hwMgmt->staticIpMap;

	//fetch SCX addresses from OtherBlade
	std::set<const ACS_CS_ImBase *> objects;
	model->getObjects(objects, CPBLADE_T);
	model->getObjects(objects, OTHERBLADE_T);
	for (std::set<const ACS_CS_ImBase *>::iterator it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);
		if (blade)
		{
			std::string ipA = blade->ipAddressEthA;
			std::string ipB = blade->ipAddressEthB;
			uint16_t slot = blade->slotNumber;
			std::string dnParentShelf = ACS_CS_ImUtils::getParentName(blade->rdn);
			ACS_CS_ImShelf * parentShelf = dynamic_cast<ACS_CS_ImShelf *> (model->getObject(dnParentShelf.c_str()));
			if (parentShelf)
			{
				std::string magazine = parentShelf->address;
				std::string scxAddressMapentry;
				//build new entry for staticIpMap
				ACS_CS_ImUtils::createStaticIpEntry(magazine, slot, ipA, ipB, scxAddressMapentry );
				bool check = (tmpMap.size()>0)? 1:0;
				if(!check || (tmpMap.find(scxAddressMapentry) == tmpMap.end()))
					tmpMap.insert(scxAddressMapentry);
				if(blade->functionalBoardName==SMXB)
				{
					scxAddressMapentry="";					
					std::string ipAliasA = blade->ipAliasEthA;
					std::string ipAliasB = blade->ipAliasEthB;
					uint16_t slotOffset = blade->slotNumber + ACS_CS_NS::SLOT_OFFSET;
					ACS_CS_ImUtils::createStaticIpEntry(magazine, slotOffset, ipAliasA, ipAliasB, scxAddressMapentry );					
					if(tmpMap.find(scxAddressMapentry) == tmpMap.end())
						tmpMap.insert(scxAddressMapentry);				
				}
			}
			else
			{
				//Error
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] IMM Error: blade with address %s doesn't have a parent shelf", __FUNCTION__, __LINE__, ipA.c_str()));
				continue;
			}

		}
	}

	//Is there something new in the MAP?
	if (tmpMap.size() > hwMgmt->staticIpMap.size())
	{
		//Update new value into IMM
		ACS_CS_ImModel hwMgmtSubModel;
		ACS_CS_ImHardwareMgmt *newHwMgmt = new ACS_CS_ImHardwareMgmt(*hwMgmt);
		newHwMgmt->staticIpMap = tmpMap;
		newHwMgmt->action = ACS_CS_ImBase::MODIFY; //specify the action for Model Saver

		if (hwMgmtSubModel.addObject(newHwMgmt))
		{
			ACS_CS_ImModelSaver saver(&hwMgmtSubModel);
			if (saver.save("STATIC_IP_MAP_UPDATE") == ACS_CC_SUCCESS)
			{
				//Trace
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Static IP address Map updated in HardwareMgmt", __FUNCTION__, __LINE__));
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] Static IP address Map updated in HardwareMgmt", __FUNCTION__, __LINE__));

			}
			else
			{
				//Error
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] IMM Error: Cannot update Static IP address Map in HardwareMgmt", __FUNCTION__, __LINE__));
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] IMM Error: Cannot update Static IP address Map in HardwareMgmt", __FUNCTION__, __LINE__));
			}
		}

	}

	if(model)
		delete model;
}

void ACS_CS_ServiceHandler::handleBoardPresenceTrap(string shelfAddr, int lSlotPos)
{

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
			"Received Board presence Trap. Shelf: %s, Slot: %d.", shelfAddr.c_str(), lSlotPos));

	if (model)
	{

		if (strlen(shelfAddr.c_str()) == 0)
		{
			std::cout << "Unable to fetch ShelfId" << endl;
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
					"Failed to fetch Shelf address"));
		}
		else
		{
			char slot[2]={0};
			std::string otherBlade_dn, cpBlade_dn;
			//std::string shelf_dn = shelfId + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

			sprintf(slot,"%d",lSlotPos);

			//Check if the board is defined
			const ACS_CS_ImBase *object = NULL;

			object = model->getBladeObject(shelfAddr, lSlotPos);

			if (object == NULL) {
				std::cout << "Board not defined" << endl;
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
						"Unhandled Board presence Trap for an undefined board. Shelf: %s, Slot: %d.", shelfAddr.c_str(), lSlotPos));
			} else {

				if (object->type == CPBLADE_T ) {
					const ACS_CS_ImCpBlade *blade;
					blade = dynamic_cast<const ACS_CS_ImCpBlade *>(object);

					if (blade && blade->functionalBoardName == CPUB && blade->dhcpOption == DHCP_NORMAL) {

						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
								"Handling Board presence Trap for a CP board. Shelf: %s, Slot: %d.", shelfAddr.c_str(), lSlotPos));
						if(isAP1)
							addToDHCPTable(blade);
						else
						{
							ACS_CS_ImModel tmpModel;
							ACS_CS_ImBase *tmpBlade = blade->clone();
							tmpBlade->action = ACS_CS_ImBase::MODIFY;
							tmpModel.addObject(tmpBlade);

							ACS_CS_ImModelSaver saver(&tmpModel);
							saver.save("DHCP-Reservation");
						}
						//handleDHCPTable(object);
					}

				} else if (object->type == OTHERBLADE_T ) {
					const ACS_CS_ImOtherBlade *blade;
					blade = dynamic_cast<const ACS_CS_ImOtherBlade *>(object);

					if (blade && (blade->dhcpOption == DHCP_NORMAL || blade->functionalBoardName == EPB1 || blade->functionalBoardName == EVOET || blade->functionalBoardName == CMXB)) {
						ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBoardPresenceTrap() "
								"Handling Board presence Trap for an OTHER board. Shelf: %s, Slot: %d.", shelfAddr.c_str(), lSlotPos));

						if(isAP1)
							addToDHCPTable(blade);
						else
						{
							ACS_CS_ImModel tmpModel;
							ACS_CS_ImBase *tmpBlade = blade->clone();
							tmpBlade->action = ACS_CS_ImBase::MODIFY;
							tmpModel.addObject(tmpBlade);

							ACS_CS_ImModelSaver saver(&tmpModel);
							saver.save("DHCP-Reservation");
						}
						//handleDHCPTable(object);
					}
				}

			}
		}

		delete model;
	}
}


void ACS_CS_ServiceHandler::handleBSPBoardPresenceTrap(int lShelfNum, int lSlotPos)
{
	ACS_CS_IronsideHandler *netconfHandler = dynamic_cast<ACS_CS_IronsideHandler*> (shelfManager);

	if (!netconfHandler)
		return;

	std::string shelfAddr=netconfHandler->getMagAddressFromShelfId(lShelfNum);

	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::handleBSPBoardPresenceTrap() "
			"Handling Board presence Trap for BSP board. Shelf: %d,ShelfAddr: %s, Slot: %d.", lShelfNum,shelfAddr.c_str(), lSlotPos));


	handleBoardPresenceTrap(shelfAddr,lSlotPos);
}

bool ACS_CS_ServiceHandler::addWatcherOnFolder()
{
	bool result = false;
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::addWatcherOnLogFile() "));

	// Only in MCP and on AP1 is required a watcher
	if(isAP1 && isBladeCluster && (-1 == m_watcherID))
	{
		std::string pathCcF("");
		if (ACS_CS_ImUtils::getNbiCcFile(pathCcF))
		{
			result = true;
			// add a watcher on folder
			m_watcherID = inotify_add_watch( m_InotifyFD, pathCcF.c_str(), IN_DELETE | IN_CREATE | IN_DELETE_SELF);
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::addWatcherOnLogFile() , add watcher:<%d> on folder:<%s>", m_watcherID, pathCcF.c_str()));
		}
	}
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "out ACS_CS_ServiceHandler::addWatcherOnLogFile() "));

	return result;
}


void ACS_CS_ServiceHandler::handlingFolderChange()
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange() "));

	char buffer[Inotify_Size::EVENT_BUF_LEN] = {0};

	// read the list of change events happens
	ssize_t length = read( m_InotifyFD, buffer, Inotify_Size::EVENT_BUF_LEN );

	if( length > 0)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange() , received <%d> bytes", length));

		ssize_t eventIdx = 0;

		while( eventIdx < length )
		{
			// read the change event one by one and process it accordingly
			struct inotify_event* event = (struct inotify_event*)( &buffer[eventIdx]);

			if( event->len && (event->mask & IN_DELETE))
			{
				// File deleted from folder
				std::string fileName(event->name);
				//ignore partial file
				std::size_t found = fileName.find(".filepart");
				if (found == std::string::npos)
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange()  - delete file: %s",fileName.c_str()));
					handlingCandidateCcFile(fileName,false);
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange()  - file to ignore: %s",fileName.c_str()));
				}
			}
			else if( event->len && (event->mask & IN_CREATE))
			{
				// File added on folder
				std::string fileName(event->name);

				//ignore partial file
				std::size_t found = fileName.find(".filepart");
				if (found == std::string::npos)
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange()  - create file: %s",fileName.c_str()));
					handlingCandidateCcFile(fileName,true);
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange()  - file to ignore: %s",fileName.c_str()));
				}
			}
			else if( event->mask & IN_DELETE_SELF )
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange()  - watched folder removed"));
				m_watcherID = -1;
			}

			eventIdx += Inotify_Size::EVENT_SIZE + event->len;
		}
	}
	else
	{
		// handling read error
		char errorText[256] = {0};
		std::string errorDetail(strerror_r(errno, errorText, 255));
		char errorMsg[512] = {0};
		snprintf(errorMsg, 511, "FMS_CPF_JTP_ConnectionsMgr::handlingFolderChange()(), read events failed, error=<%s>", errorDetail.c_str() );
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "in ACS_CS_ServiceHandler::handlingFolderChange()  - %s", errorMsg));
	}

	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "out ACS_CS_ServiceHandler::handlingFolderChange() "));
}


void ACS_CS_ServiceHandler::handlingCandidateCcFile(std::string fileName, bool add)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"(%t) in ACS_CS_ServiceHandler::handlingCandidateCcFile()"));


	ACS_CS_ImModel *readModel = ACS_CS_ImRepository::instance()->getModelCopy();

	if (readModel)
	{
		ACS_CS_ImModel tempModel;
		string ccFileManagerRdn = ACS_CS_ImmMapper::RDN_AXE_CCFILE_MANAGER;
		string candidateCcFileRdn = ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID + "=" + fileName + "," + ACS_CS_ImmMapper::RDN_AXE_CCFILE_MANAGER;
		string candidateCcFileStructRdn = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=" + fileName + "," + candidateCcFileRdn;


		if (add)
		{
			//create object
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"(%t) ACS_CS_ServiceHandler::handlingCandidateCcFile() - add candidate file "));

			ACS_CS_ImCandidateCcFile *candidateCcFile = new ACS_CS_ImCandidateCcFile();

			candidateCcFile->rdn = candidateCcFileRdn;
			candidateCcFile->type = CANDIDATECCFILE_T;
			candidateCcFile->candidateCcFileId = ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID + "=" + fileName;
			candidateCcFile->reportProgress = candidateCcFileStructRdn;
			candidateCcFile->action = ACS_CS_ImBase::CREATE;

			ACS_CS_ImBase *baseCcFileManager = NULL;
			baseCcFileManager = readModel->getObject(ccFileManagerRdn.c_str());

			if (baseCcFileManager)
			{
				//the CcFileManager already exists
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"(%t) ACS_CS_ServiceHandler::handlingCandidateCcFile() - add candidate - the CcFileManager class already exists!"));

				tempModel.addObject(candidateCcFile);
			}
			else
			{
				ACS_CS_ImCcFileManager *ccFileManager = new ACS_CS_ImCcFileManager();

				ccFileManager->rdn = ccFileManagerRdn;
				ccFileManager->type = CCFILEMANAGER_T;
				ccFileManager->ccFileManagerId = ACS_CS_ImmMapper::ATTR_CCFILE_MANAGER_ID + "=1";
				ccFileManager->action = ACS_CS_ImBase::CREATE;

				tempModel.addObject(ccFileManager);
				tempModel.addObject(candidateCcFile);
			}

		}
		else
		{
			//remove object
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"(%t) ACS_CS_ServiceHandler::handlingCandidateCcFile() - remove candidate file "));

			ACS_CS_ImBase *baseCandidatecFile = NULL;
			baseCandidatecFile = readModel->getObject(candidateCcFileRdn.c_str());
			if (baseCandidatecFile)
			{

				ACS_CS_ImCandidateCcFile *candidateCcFile = new ACS_CS_ImCandidateCcFile();
				candidateCcFile->rdn = candidateCcFileRdn;
				candidateCcFile->action = ACS_CS_ImBase::DELETE;

				tempModel.addObject(candidateCcFile);
			}
		}

		ACS_CS_ImModelSaver saver(&tempModel);
		saver.save("createRemoveCandidateCcFile");

		delete readModel;
	}
}



void ACS_CS_ServiceHandler::removeInvalidEntries()
{

	ACS_CS_ImIMMReader reader;
	ACS_CS_ImModel model;

	//Get all blades currently defined on IMM
	reader.loadBladeObjects(&model);

	std::set<const ACS_CS_ImBase *> objects;
	std::set<const ACS_CS_ImBase *>::iterator it;

	model.getObjects(objects);

	ACS_CS_ImModel invalidBlades;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		ACS_CS_ImBlade* blade = const_cast<ACS_CS_ImBlade*>(dynamic_cast<const ACS_CS_ImBlade*>(*it));

		int errNo;

		//Check if it is a valid entry
		if (blade && !(blade->isValid(errNo, model)))
		{
			//Remove the Blade if the entry is not valid
			ACS_CS_ImBase* invalidObj = blade->clone();

			if (invalidObj)
			{
				invalidObj->action = ACS_CS_ImBase::DELETE;
				invalidBlades.addObject(invalidObj);
			}
		}
	}

	if (invalidBlades.size() > 0)
	{
		ACS_CS_ImModelSaver saver(&invalidBlades);
		saver.save("RemoveInvalidBlades");
	}
}


void ACS_CS_ServiceHandler::removeInvalidComputeResourceObjects()
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_DEBUG," Entering ACS_CS_ServiceHandler::removeInvalidComputeResourceObjects()")); 

	ACS_CS_ImIMMReader reader;

         std::vector<std::string> classNames;
         classNames.push_back(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE);

         std::vector<std::string> computeResourcesRdn = reader.getClassInstancesRdns(classNames);

	 int size=computeResourcesRdn.size();

         for (int i=0; i< size; ++i)
         {
		 int crType = 0;
                 ACS_CS_ImUtils::getImmAttributeInt(computeResourcesRdn[i], ACS_CS_ImmMapper::ATTR_CR_TYPE, crType);

		 if(crType == IPLB_TYPE)
		 {
			 if (! ACS_CS_TFTP_Configurator::instance()->checkDefaultSwPackage(IPLB_TYPE))
			 {
				 int networkNum = 0;
				 if(crrWorker->getNetworkNum(computeResourcesRdn[i],networkNum) ==false)
				 {
					 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_DEBUG," Unable to fetch number of networks"));
				 }

				 //Deleting all the networks associated to IPLB compute resource object
				 for(int j=0;j<networkNum;j++)
				 {
					 stringstream temp_str;
					 temp_str<<(j);
					 std::string str = temp_str.str();
					 std::string network_dn ("id=network_");
					 network_dn.append(str);
					 network_dn.append(",");
					 network_dn.append(computeResourcesRdn[i]);

					 if(ACS_CS_ImUtils::deleteImmObject(network_dn.c_str()) == true)
					 {
						 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_ERROR, " %s deleted successfully",network_dn.c_str()));
					 }
					 else
					 {
						 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_ERROR, "Fail to delete %s with error code %d - %s ",network_dn.c_str(),getInternalLastError(), getInternalLastErrorText()));
					 }
				 }

				 if(ACS_CS_ImUtils::deleteImmObject(computeResourcesRdn[i].c_str()) == true)
				 {
					 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_DEBUG," %s deleted successfully",computeResourcesRdn[i].c_str()));
				 }
				 else
				 {
					 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,LOG_LEVEL_DEBUG,"Fail to delete %s with error code %d - %s ",computeResourcesRdn[i].c_str(),getInternalLastError(), getInternalLastErrorText()));
				 }

			 }
		 }
	 }
}

bool ACS_CS_ServiceHandler::isRestartAfterRestore()
{
	bool result = false;

	if( m_clearDataPath.empty() && !getClearPath())
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "isRestartAfterRestore(), error on get clear path "));
	}
	else
	{
		// check restart type
		std::string csClearPath = m_clearDataPath + CsBRFolder;
		boost::filesystem::path csClearFolder(csClearPath);

		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"(%t) ACS_CS_ServiceHandler::isRestartAfterRestore() - isRestartAfterRestore(), check clear folder:<%s>", csClearPath.c_str()));
		try
		{
			if( boost::filesystem::exists(csClearFolder) )
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "isRestartAfterRestore(), normal restart "));
			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "isRestartAfterRestore(), restart after restore"));
				// create the folder
				boost::filesystem::create_directory(csClearFolder);
				result = true;
			}
		}
		catch(const boost::filesystem::filesystem_error& ex)
		{
			 ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "isRestartAfterRestore() failed to create PSO folder:<%s>, error:<%s>", csClearPath.c_str(), ex.what() ));
		}
	}

	return result;
}

bool ACS_CS_ServiceHandler::getClearPath()
{
	bool result = false;

	int bufferLength;
	ifstream clearFileStream;
	// open the clear file
	clearFileStream.open(PSAClearFilePath, ios::binary );

	// check for open error
	if(clearFileStream.good())
	{
		// get length of stored path:
		clearFileStream.seekg(0, ios::end);
		bufferLength = clearFileStream.tellg();
		clearFileStream.seekg(0, ios::beg);

		// allocate the buffer
		char buffer[bufferLength+1];
		ACE_OS::memset(buffer, 0, bufferLength+1);

		// read data
		clearFileStream.read(buffer, bufferLength);

		if(buffer[bufferLength-1] == '\n') buffer[bufferLength-1] = 0;

		m_clearDataPath = buffer;
		m_clearDataPath += DirDelim;
		result = true;
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,"(%t) ACS_CS_ServiceHandler::handlingCandidateCcFile() - setClearPath(), clear path:<%s>", m_clearDataPath.c_str()));
	}

	clearFileStream.close();

	return result;
}

void ACS_CS_ServiceHandler::startClusterAlignedTimer()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function", __FUNCTION__, __LINE__));

	if (!ACS_CS_Timer::SetTimer(clusterAlignedTimer, 5))		{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"[%s@%d] Failed to set clusterAlignedTimer", __FUNCTION__, __LINE__));
	}

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function", __FUNCTION__, __LINE__));
}

void ACS_CS_ServiceHandler::startLogicalMgmtCleanupTimer()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function", __FUNCTION__, __LINE__));

	if (!ACS_CS_Timer::SetTimer(logicalMgmtCleanupTimer, 10))		{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"[%s@%d] Failed to set clusterAlignedTimer", __FUNCTION__, __LINE__));
	}

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function", __FUNCTION__, __LINE__));
}

bool ACS_CS_ServiceHandler::updateIpStaticListFromDisk(ACS_CS_ImHardwareMgmt *hwMgmt, std::set<std::string> table)
{
	bool retVal = false;

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function", __FUNCTION__, __LINE__));

	ACS_CS_ImModel hwMgmtSubModel;
	ACS_CS_ImHardwareMgmt *newHwMgmt = new ACS_CS_ImHardwareMgmt(*hwMgmt);
	if(newHwMgmt)
	{
		newHwMgmt->staticIpMap = table;
		newHwMgmt->action = ACS_CS_ImBase::MODIFY; //specify the action for Model Saver

		if (hwMgmtSubModel.addObject(newHwMgmt))
		{
			ACS_CS_ImModelSaver saver(&hwMgmtSubModel);
			if (saver.save("STATIC_IP_MAP_UPDATE") == ACS_CC_SUCCESS)
			{
				retVal = true;
				//Trace
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Static IP address Map updated in HardwareMgmt", __FUNCTION__, __LINE__));
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] Static IP address Map updated in HardwareMgmt", __FUNCTION__, __LINE__));
			}
			else
			{
				//Error
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] IMM Error: Cannot update Static IP address Map in HardwareMgmt", __FUNCTION__, __LINE__));
				ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] IMM Error: Cannot update Static IP address Map in HardwareMgmt", __FUNCTION__, __LINE__));
			}
		}
	}

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function with retValue = ", __FUNCTION__, __LINE__, retVal));

	return retVal;
}

bool ACS_CS_ServiceHandler::loadStaticIpFromDisk()
{

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function", __FUNCTION__, __LINE__));

	std::set<std::string> tableFromDisk;
	ACS_CS_ImIPAssignmentHelper helper;

	bool retVal = false;

	if(!helper.readStaticIpFromDisk(tableFromDisk))
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"ACS_CS_ServiceHandler::loadStaticIpFromDisk() failed. Cleaning IMM content."));
	}

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
	if(!model)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Failed to get model", __FUNCTION__, __LINE__));
		return retVal;
	}

	ACS_CS_ImHardwareMgmt* hwMgmt = dynamic_cast<ACS_CS_ImHardwareMgmt*> (model->getObject(ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));

	if(hwMgmt)
	{
		retVal = updateIpStaticListFromDisk(hwMgmt, tableFromDisk);
		if (retVal)
		{
			initStaticIpMap();
		}

	}

	if(model)
		delete model;

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function with retValue = ", __FUNCTION__, __LINE__, retVal));

	return retVal;
}

void ACS_CS_ServiceHandler::checkStaticIpDiskWithIMM()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function", __FUNCTION__, __LINE__));

	bool isIpListFromDiskValid = false;
	bool isUpdateNeeded = true;

	std::set<std::string> ipTableOnDisk;
	std::set<string> ipTableOnImm;

	//read static Ip from disk
	ACS_CS_ImIPAssignmentHelper helper;

	if(!helper.readStaticIpFromDisk(ipTableOnDisk))
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Read Static IP List from disk failed", __FUNCTION__, __LINE__));
	}

	//read static Ip from Imm
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	if(!model)
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Failed to get model", __FUNCTION__, __LINE__));
		return;
	}

	ACS_CS_ImHardwareMgmt* hwMgmt = dynamic_cast<ACS_CS_ImHardwareMgmt*> (model->getObject(ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));

	if(hwMgmt)
	{
		ipTableOnImm = hwMgmt->staticIpMap;

		if(ipTableOnDisk.size() == ipTableOnImm.size())
		{
			//need to compare individual elements
			if(ipTableOnDisk == ipTableOnImm)
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Static IP list on Disk == Imm", __FUNCTION__, __LINE__));
				isUpdateNeeded = false;
			} else
			{
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Static IP list on Disk != Imm", __FUNCTION__, __LINE__));
				isIpListFromDiskValid=false;
			}
		}
		else if(ipTableOnDisk.size() > ipTableOnImm.size())
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Static IP list size on Disk > Imm", __FUNCTION__, __LINE__));
			isIpListFromDiskValid = true;
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Static IP list size on Disk < Imm", __FUNCTION__, __LINE__));
			isIpListFromDiskValid = false;
		}

		if(isUpdateNeeded)
		{
			if(isIpListFromDiskValid)
			{
				//Store DISK IP list in IMM
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Load Static IP List from DISK...", __FUNCTION__, __LINE__));
				updateIpStaticListFromDisk(hwMgmt, ipTableOnDisk);
			}
			else
			{
				//Store IMM IP list on DISK
				ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Store Static IP List to DISK...", __FUNCTION__, __LINE__));

				ACS_CS_ImIPAssignmentHelper helper;
				helper.storeStaticIpToDisk(model);
			}
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Unable to fetch IMM Static IP list...", __FUNCTION__, __LINE__));
	}

	if(model) delete model;

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function", __FUNCTION__, __LINE__));
}

void ACS_CS_ServiceHandler::startStaticIpAlignedTimer()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function", __FUNCTION__, __LINE__));

	if (!ACS_CS_Timer::SetTimer(staticIpAlignedTimer, 5))
	{
		ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
				"[%s@%d] Failed to set staticIpAlignedTimer", __FUNCTION__, __LINE__));
	}

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function", __FUNCTION__, __LINE__));
}

void ACS_CS_ServiceHandler::storedTableOperationWorker()
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Enter function size of tableSubsets %d", __FUNCTION__, __LINE__, tableSubsets.size()));

	//handle queue notifications
	tableSubsetsLock.start_reading();
	std::vector<const ACS_CS_ImModelSubset*>::iterator it = tableSubsets.begin();
	while (it != tableSubsets.end())
	{
		//handle table operation
		const ACS_CS_ImModelSubset * subset = *it;
		newTableOperationRequest(subset);

		tableSubsetsLock.stop_reading();
		//remove from vector
		tableSubsetsLock.start_writing();
		delete subset;
		tableSubsets.erase(it);
		it = tableSubsets.begin();
		tableSubsetsLock.stop_writing();

		tableSubsetsLock.start_reading();
	}

	tableSubsetsLock.stop_reading();

	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE, "[%s@%d] Exit function", __FUNCTION__, __LINE__));

}

void ACS_CS_ServiceHandler::startSubscriptionTimer()
{
	if (!ACS_CS_Timer::SetTimer(subscriptionSupervisionTimerHandle, 30))
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_ERROR,
				"[%s@%d] Failed to set Subscription Timer", __FUNCTION__, __LINE__));
	}
}



void ACS_CS_ServiceHandler::initializeMauType()
{
	ACS_CS_ImIMMReader reader;
	ACS_CS_ImModel model;

	ACS_CS_Registry::loadDefaultMauType();
	//Get all cp objects currently defined on IMM
	if(!reader.loadCpObjects(&model) || !reader.loadBladeObjects(&model))
	{
		std::cout<<"..load CP Object failed..........\n";		
		return;
	}
	std::set<const ACS_CS_ImBase *> objects;
	std::set<const ACS_CS_ImBase *>::iterator it;

	model.getObjects(objects,DUALSIDEDCP_T);

	ACS_CS_ImModel tmpModel;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBase *base = *it;
		ACS_CS_ImDualSidedCp* cp = const_cast<ACS_CS_ImDualSidedCp*>(dynamic_cast<const ACS_CS_ImDualSidedCp*>(base));
		if(cp)
		{
			int errNo = 0;
			//Check if MAU type is valid. Set it to default value if it's not.
			if (!cp->isValid(errNo,model) && (errNo == TC_INVALIDMAUTYPE || errNo == TC_MAUSFAULTY))
			{
				ACS_CS_ImBase *tmpBlade = cp->clone();
				if(tmpBlade)
				{
					ACS_CS_ImDualSidedCp* dualcp = dynamic_cast< ACS_CS_ImDualSidedCp*>(tmpBlade);
					dualcp->mauType = ACS_CS_Registry::getDefaultMauType();
					dualcp->action = ACS_CS_ImBase::MODIFY;
					tmpModel.addObject(dualcp);
				}						
			}			
		}		
	}		
	if (tmpModel.size() > 0)
	{
		ACS_CS_ImModelSaver saver(&tmpModel);
		saver.save("MauType");
	}

}

void ACS_CS_ServiceHandler::handleTFTPArea(const ACS_CS_ImModel *model)
{
	ACS_CS_TRACE((ACS_CS_ServiceHandler_TRACE,
			"ACS_CS_ServiceHandler::handleTFTPArea()"));

	if (!model)
		return;

	std::set<const ACS_CS_ImBase *> objects;

	model->getObjects(objects, CPBLADE_T);
	model->getObjects(objects, OTHERBLADE_T);

	set<const ACS_CS_ImBase *>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBase * base = *it;

		const ACS_CS_ImBlade * blade = dynamic_cast<const ACS_CS_ImBlade*>(base);

		if (blade)
		{
			ACS_CS_TFTP_Info bladeTftpInfo;

			ACS_CS_TFTP_Configurator::instance()->createTftpInfo(bladeTftpInfo, blade);

			if (ACS_CS_ImBase::DELETE == blade->action)
			{
				ACS_CS_TFTP_Configurator::instance()->removeTftpEntry(bladeTftpInfo);
			}
			else
			{
				ACS_CS_TFTP_Configurator::instance()->addTftpEntry(bladeTftpInfo, blade);
			}
		}
	}
}

void ACS_CS_ServiceHandler::verifyTftpConfiguration()
{
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_ServiceHandler::verifyTftpConfiguration()"));

	ACS_CS_TFTP_Configurator::instance()->checkSmartImageStructure();

	std::map<std::string, ACS_CS_TFTP_Info> tftpAreaContent;

	ACS_CS_TFTP_Configurator::instance()->getTftpContent(tftpAreaContent);

	ACS_CS_API_HWC * hwc;
	ACS_CS_API_BoardSearch *boardSearch;
	ACS_CS_API_IdList boardList;

	hwc =  ACS_CS_API::createHWCInstance();
	boardSearch = hwc->createBoardSearchInstance();


	for (std::map<std::string, ACS_CS_TFTP_Info>::iterator it = tftpAreaContent.begin(); it != tftpAreaContent.end(); ++it)
	{

		boardSearch->reset();
		boardSearch->setUuid(it->second.uuid);

		ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(boardList, boardSearch);

		if (cs_call_result == ACS_CS_API_NS::Result_Failure)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::verifyTftpConfiguration() boardIds returned %i", cs_call_result));
			break;
		}
		else
		{
			size_t size = boardList.size();

			if (size == 0)
			{
				ACS_CS_TFTP_Configurator::instance()->removeTftpEntry(it->second);
			}
		}
	}

	hwc->deleteBoardSearchInstance(boardSearch);
	ACS_CS_API::deleteHWCInstance(hwc);

	std::set<const ACS_CS_ImBase *> objects;

	ACS_CS_ImRepository::instance()->getModel()->getObjects(objects, CPBLADE_T);
	ACS_CS_ImRepository::instance()->getModel()->getObjects(objects, OTHERBLADE_T);

	std::set<const ACS_CS_ImBase *>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBase * base = *it;
		//Start of TR HZ73185
		if(!base)
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_ERROR,  "ACS_CS_ServiceHandler::verifyTftpConfiguration() ImBase object is NULL"));
			continue;
		} // End of TR HZ73185

		const ACS_CS_ImBlade * blade = dynamic_cast<const ACS_CS_ImBlade*>(base);

		if (blade)
		{
			ACS_CS_TFTP_Info bladeTftpInfo;
			ACS_CS_TFTP_Configurator::instance()->createTftpInfo(bladeTftpInfo, blade);

			ACS_CS_TFTP_Configurator::instance()->addTftpEntry(bladeTftpInfo, blade);
		}
	}

}

void ACS_CS_ServiceHandler::handleDispatchFailed()
{
	//Remove IMM Class Implementers and stop callback thread
	if (ACS_CC_SUCCESS != oiHandler.removeMultipleClassImpl(this,classNames))
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_INFO,
				"ACS_CS_ServiceHandler::immObjCleanUp() "
				"Remove Implementer Failed"));
	}
	if (immWorker)
	{
		delete immWorker;
		immWorker = 0;
	}
	initImm();
}

bool ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed()
{
	bool createAllowed = false;
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE, LOG_LEVEL_DEBUG, "ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed() "));
	ACS_PRC_API prcapi;
	int node_state = -1;
	node_state = prcapi.askForNodeState();						// TR HX53913
	ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
			LOG_LEVEL_DEBUG,"ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed() "
			"Node state is %d",node_state));

	if(node_state == ACS_PRC_NODE_ACTIVE)
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_DEBUG,"ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed() "
				"Node is ACTIVE. Check for default IPLB SW in TFTP area"));
		if(!ACS_CS_TFTP_Configurator::instance()->checkDefaultSwPackage(IPLB_TYPE))
		{
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_WARN,
					"ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed() "
					"Default IPLB software is NOT PRESENT in APG TFTP Area for FBN IPLB"));
		}
		else
		{
			createAllowed = true;
			ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
					LOG_LEVEL_WARN,
					"ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed() "
					"Default IPLB software is PRESENT in APG TFTP Area for FBN IPLB"));
		}
	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_ServiceHandler_TRACE,
				LOG_LEVEL_WARN,
				"ACS_CS_ServiceHandler::isIplbCrObjCreationAllowed() "
				"Node state is NOT ACTIVE. Cannot check default IPLB SW in TFTP area"));
	}
	// If node state is passive or undefined, reject IMM object creation as we do not know if data disk can be accessed from current node

	return createAllowed;
}
