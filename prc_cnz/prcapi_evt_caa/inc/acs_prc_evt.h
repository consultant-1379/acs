#ifndef PRCAPI_EVT_CAA_SRC_ACS_PRC_EVT_H_
#define PRCAPI_EVT_CAA_SRC_ACS_PRC_EVT_H_

#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_prc_evt

#define stringize_(s) #s
#define stringize(s) stringize_(s)

#include <stdio.h>
#include <MafMwSpiServiceIdentities_1.h>
#include <MafMgmtSpiServiceIdentities_1.h>
#include <MafMgmtSpiInterfacePortal_3.h>
#include <MafMgmtSpiComponent_2.h>
#include <MafMgmtSpiInterfacePortalAccessor.h>
#include <MafMgmtSpiInterfacePortal_1.h>
#include <MafOamSpiEvent_1.h>
#include <MafOamSpiServiceIdentities_1.h>
#include <MafOamSpiCmRouterEvent_1.h>

const char pipeName[] = "/var/run/ap/PrcManServer";
const char MODEL_CHANGE_COMMITTED[] = "MODEL_CHANGE_COMMITTED";
const char LOG_FROM_PRC_API_COM_COMPONENT[] = "#LOG_FROM_PRC_API_COM_COMPONENT#";

class __CLASS_NAME__ {

public:

	__CLASS_NAME__();

	virtual ~__CLASS_NAME__();

	static __CLASS_NAME__& getInstance();

	MafReturnT init(MafMgmtSpiInterfacePortalAccessorT* accessor, const char* config);

	MafReturnT terminate();

	MafReturnT start();

	MafReturnT stop();

	MafReturnT notify(MafOamSpiEventConsumerHandleT handle, const char * eventType, MafNameValuePairT** filter, void * value);

private:

	void sendMsgToServer(const char*, ...);

	MafMgmtSpiInterfacePortal_3T* _portal;
	MafOamSpiEventRouter_1T * _interface;
	MafMgmtSpiComponent_2 _component;
	MafMgmtSpiInterface_1T* _ifArray[1];
	MafMgmtSpiInterface_1T* _depArray[2];

	MafNameValuePairT BrMFilter, AxeFunctionsFilter, FileMFilter, HealthCheckMFilter, SecMFilter, SwInventoryFilter, SysMFilter;
	MafNameValuePairT *myFilter[8];

	MafOamSpiEventConsumer_1T _eventConsumer;
	MafOamSpiEventConsumerHandleT _consumerHandle;

	static __CLASS_NAME__ _instance;

	int _prc_local_socket;

};

#endif
