///	*********************************************************
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

#ifndef ACS_CS_ServiceHandler_h
#define ACS_CS_ServiceHandler_h 1

// Platform
#include <map>
#include <set>
#include <string>
#include <stdio.h>
#include <vector>
#include <boost/regex.hpp>
//ACE
#include "ace/Event.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_time.h"
#include "ace/Process.h"
#include "ace/Signal.h"
#include "ace/OS_NS_fcntl.h"
#include <ace/Semaphore.h>

// AP-SDK
//CS

#include "ACS_CS_Thread.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_TableOperation.h"
#include "ACS_CS_Internal_Table.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_TableOperationWorker.h"
#include "ACS_CS_PTBPeerHandler.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_BrfHandler.h"
#include "ACS_CS_TFTP_Configurator.h"
#include "ACS_CS_LdapProxyHandler.h"
#include "ACS_CS_CRRProxyHandler.h"

#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "ACS_CS_DHCP_Configurator.h"
#include "ACS_CS_IMMCallbackThread.h"
#include <acs_apgcc_oihandler_V3.h>

struct IPAssignment
{
	uint32_t iPEthA;
	uint32_t iPEthB;
};


// Forward declarations
//class ACS_CS_TableHandler;
class ACS_CS_ServiceConnection;
class ACS_CS_TrapHandler;
class ACS_CS_ShelfManager;
class ACS_CS_SNMPHandler;
class ACS_CS_Attribute;
class ACS_CS_PDU;
class ACS_CS_TCPServer;
class ACS_CS_TableRequest;
class ACS_CS_TableHandler;
class ACS_CS_SubscriptionAgent;
class ACS_CS_DHCP_Info;
class ACS_CS_NEHandler;
class ACS_CS_HWCHandler;
class ACS_CS_ImRepository;
class ACS_CS_ImBlade;
class ACS_CS_CpClusterManager;
class ACS_CS_ProductInfo;
class ACS_CS_SECCallbackThread;
class ACS_CS_TFTP_Configurator;
class ACS_CS_ComputeResourceHandler;
class ACS_CS_CRRProxyHandler;
//------------------------------------------------------

/** @class ACS_CS_ReactorRunner
 *	@brief Internal class to run the ACE Reactor event loop
 *	@author xminaon
 *
 *	This class is not a Singleton but only one class should be created. It must be started
 *	prior to using any reactor driven class.
 */

class ACS_CS_ReactorRunner: public ACS_CS_Thread
{
public:
	virtual int exec();
	void deActivate();
};

//------------------------------------------------------

/** @class ACS_CS_Sync_Thread
 *	@brief Internal thread class to handle hwcsync
 *	@author xminaon
 *
 */
class ACS_CS_Sync_Thread: public ACS_CS_Thread
{
public:
	ACS_CS_Sync_Thread(ACS_CS_SNMPHandler* handler, std::vector<ACS_CS_DHCP_Info*>* vector);
	bool success() const {return syncSuccess;}
	int exec();
private:
	ACS_CS_SNMPHandler* snmpHandler;
	std::vector<ACS_CS_DHCP_Info*>* syncVector;
	bool syncSuccess;
};


//------------------------------------------------------

//	This class implements the CS service. It creates the
//	objects that make up the service and receives and
//	distributes requests for the tables. The class polls to
//	check if it should execute as master or slave and
//	changes between the two roles accordingly.

class ACS_CS_ServiceHandler : public ACS_CS_Thread, acs_apgcc_objectimplementerinterface_V3
{

	typedef std::map<unsigned short, ACS_CS_TableHandler *> maptype;
public:
	ACS_CS_ServiceHandler();
	virtual ~ACS_CS_ServiceHandler();
	virtual int exec();
	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentname, ACS_APGCC_AttrValues **attr);
	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual ACS_CC_ReturnType updateRuntime(const char*, const char**)
	{
		return ACS_CC_SUCCESS;
	}

	int newRequest(ACS_CS_PDU *pdu);
	bool syncClients(std::vector<ACS_CS_DHCP_Info*> & syncVector);
	bool commitSync();
	void MacUpdate(ACS_CS_DHCP_Info *infoobj);
	void ProdInfoUpdate(ACS_CS_DHCP_Info *infoobj);
	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char *p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType **paramList);
	inline bool isRunningThread()
	{
		return thread_running;
	}

	void handleDHCPTable(const ACS_CS_ImModel *model);
	void handleDHCPTable(const ACS_CS_ImBase *object);
	void addToDHCPTable(const ACS_CS_ImBlade *blade);
	void removeFromDHCPTable(const ACS_CS_ImBlade *blade);
	uint32_t getIpForTftpAddress(ACS_CS_Protocol::CS_Network_Identifier network, bool smxTrapReceiver=false);
	bool addEBSBoardToDHCPTable(ACS_CS_DHCP_Info *infoA);
	void handleBoardPresenceTrap(int lShelfNum, int lSlotPos);
	void handleBoardPresenceTrap(string shelfAddr, int lSlotPos);
	void handleBSPBoardPresenceTrap(int lShelfNum, int lSlotPos);
	bool clearDhcpLeases();
	bool createDHCPInfo(ACS_CS_DHCP_Info & info, const ACS_CS_ImBlade *blade, ACS_CS_Protocol::CS_Network_Identifier network, ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE);
	void handleTFTPArea(const ACS_CS_ImModel *model);
	inline bool isOperationOngoing()
	{
		return OngoingAction;
	}

	inline void setOperationOngoing(bool value)
	{
		OngoingAction = value;
	}

	inline bool isAfterRestore() const
	{
		return m_isRestartAfterRestore;
	}

	inline bool isSubscriptionTimerEnded() const
	{
		return m_isSubscriptionTimerEnded;
	}

	;
private:
	void immUpdated();
	void setNodeSpecificOptions(ACS_CS_DHCP_Info & info);
	uint16_t parseString(const std::string & intString);
	ACS_CS_ServiceHandler(const ACS_CS_ServiceHandler & right);
	ACS_CS_ServiceHandler & operator =(const ACS_CS_ServiceHandler & right);
	bool verifyDhcpConfiguration(ACS_CS_Protocol::CS_DHCP_Method_Identifier method);
	void startListener();
	void startHandlers();
	void stopHandlers();
	void reloadTables();
	void removeClosedConnections(std::set<ACS_CS_ServiceConnection*> & connections);
	void openNewServiceConnection(int tcpServerHandle, std::set<ACS_CS_ServiceConnection*> & connectionSet, ACS_CS_TCPServer *tcpServer, ACS_CS_Protocol::CS_Protocol_Type protocolType);
	int APZ2123xHandler();
	ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType parseTableType(const char *objName);
	void tableOperationWorkerCleanup();
	bool newTableOperationRequest(const ACS_CS_ImModelSubset *subset, ACS_CS_TableHandler *selectedHandler = 0);
	bool isIpAddress(const string ipAddress);
	void LogicalObjCleanUp();
	void LogicalObjCleanUp_vAPZ();
	int startTrapManager();
	void addToDHCPQueue(ACS_CS_DHCP_Info & infoA, ACS_CS_DHCP_Info & infoB);
	void removeFromDHCPQueue(const ACS_CS_ImBlade *blade);
	virtual void initImm();
	void initStaticIpMap();
	void createCpProductInfo(const char *p_objName, ACS_CS_ProductInfo & info);
	bool addWatcherOnFolder();
	void handlingFolderChange();
	void handlingCandidateCcFile(std::string fileName, bool add);
	void removeInvalidEntries();
	void removeInvalidComputeResourceObjects();
	void updateProductInfoAttr(const char *p_objName, bool empty = false);
	bool isRestartAfterRestore();
	bool getClearPath();
	void startClusterAlignedTimer();
	void startLogicalMgmtCleanupTimer();
	bool loadStaticIpFromDisk();
	void checkStaticIpDiskWithIMM();
	void startStaticIpAlignedTimer();
	void initializeMauType();
	bool updateIpStaticListFromDisk(ACS_CS_ImHardwareMgmt *hwMgmt, std::set<std::string> table);
	void storedTableOperationWorker();
	void startSubscriptionTimer();
	void verifyTftpConfiguration();
	void handleDispatchFailed();
	bool isIplbCrObjCreationAllowed();

	maptype *handlerMap;
	int apzType;
	bool isBladeCluster;
	ACS_CS_TCPServer *csTcpServer;
	ACS_CS_TCPServer *moddTcpServer;
	ACS_CS_EventHandle shutdownAllEvent;
	ACS_CS_EventHandle shutdownHandlerEvent;
	ACS_CS_EventHandle removeClosedConnectionsEvent;
	ACS_CS_EventHandle immChangedEvent;
	ACS_CS_EventHandle immDispatchFailedEvent;
	ACS_CS_EventHandle immDispatchFailedSecEvent;
	ACS_CS_NEHandler *neTable;
	ACS_CS_HWCHandler *hwcTable;
	ACS_CS_ReaderWriterLock tableWorkersLock;
	std::vector<ACS_CS_TableOperationWorker*> tableWorkers;
	ACS_CS_SubscriptionAgent *subscriptionAgent;
	ACS_CS_TableOperation *tableOperations;
	bool isAP1;
	bool thread_running;
	bool OngoingAction;
	uint32_t bootIpAddressEthA;
	uint32_t bootIpAddressEthB;
	int shelfArchitecture;
	int isDHCP_ON;
	ACS_CS_PTBPeerHandler *ptbAHandler;
	ACS_CS_PTBPeerHandler *ptbBHandler;
	ACS_CS_DHCP_Configurator *dhcp;
	ACS_CS_TrapHandler *trapHandler;
	ACS_CS_ShelfManager *shelfManager;
	acs_apgcc_oihandler_V3 oiHandler;
	std::vector<std::string> classNames;
	ACS_CS_IMMCallbackThread *immWorker;
	ACS_CS_CpClusterManager *cpClusterManager;
	ACS_CS_ReactorRunner *reactorRunner;

	ACS_CS_SECCallbackThread *secLdapWorker;
	ACS_CS_CRRProxyHandler *crrWorker; //Added to implement scale in feature
	ACS_CS_BrfHandler *m_brfHandler;

	int m_InotifyFD;
	int m_watcherID;

	set<ACS_CS_ServiceConnection*> connectionSet;

	bool m_isRestartAfterRestore;

	std::string m_clearDataPath;

	ACS_CS_EventHandle clusterAlignedTimer;

	ACS_CS_EventHandle staticIpAlignedTimer;

	ACS_CS_EventHandle subscriptionSupervisionTimerHandle;

	ACS_CS_EventHandle logicalMgmtCleanupTimer; // only used in VIRTUALIZED environment

	ACS_CS_ReaderWriterLock tableSubsetsLock;
	std::vector<const ACS_CS_ImModelSubset*> tableSubsets;

	bool m_isSubscriptionTimerEnded;
	bool m_NohwcChange;

	ACS_CS_ComputeResourceHandler* m_computeResourceHandler;
};


// Class ACS_CS_ServiceHandler 



#endif
