#include "acs_prc_evt.h"
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdarg.h>

__CLASS_NAME__ __CLASS_NAME__::_instance;

extern "C" MafReturnT prcEvtStart(MafStateChangeReasonT /*reason*/){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	return __CLASS_NAME__::getInstance().start();
}

extern "C" MafReturnT prcEvtStop(MafStateChangeReasonT /*reason*/){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	return __CLASS_NAME__::getInstance().stop();
}

extern "C" MafReturnT prcEvtNotify(MafOamSpiEventConsumerHandleT handle, const char * eventType, MafNameValuePairT **filter, void * value){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	return __CLASS_NAME__::getInstance().notify(handle, eventType, filter, value);
}

extern "C" MafReturnT mafLCMinit(struct MafMgmtSpiInterfacePortalAccessor* accessor, const char* config){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	return __CLASS_NAME__::getInstance().init(accessor, config);
}

extern "C" MafReturnT mafLCMterminate(){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	return __CLASS_NAME__::getInstance().terminate();
}

__CLASS_NAME__::__CLASS_NAME__() : _portal(0), _interface(0), _prc_local_socket(0) {

	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);

	_consumerHandle = 0;
	_ifArray[0]=0;

	_depArray[0] = (MafMgmtSpiInterface_1T*)&MafOamSpiEventService_1Id;
	_depArray[1] = 0;

	_component.base.componentName = "ACS_PRC_EVT";
	_component.base.interfaceName=  MafMgmtSpiComponentInterfaceName_1;
	_component.base.interfaceVersion= MafMgmtSpiComponentInterfaceVersion_1;

	_component.interfaceArray = _ifArray;
	_component.dependencyArray = _depArray;
	_component.start = &prcEvtStart;
	_component.stop = &prcEvtStop;

	BrMFilter.name = MafCmRouterFilterTypeRegExp;
	BrMFilter.value = ".*BrM=1.*";

	AxeFunctionsFilter.name = MafCmRouterFilterTypeRegExp;
	AxeFunctionsFilter.value = ".*AxeFunctions=1.*";

	FileMFilter.name = MafCmRouterFilterTypeRegExp;
	FileMFilter.value = ".*FileM=1.*";

	HealthCheckMFilter.name = MafCmRouterFilterTypeRegExp;
	HealthCheckMFilter.value = ".*HealthCheckM=1.*";

	SecMFilter.name = MafCmRouterFilterTypeRegExp;
	SecMFilter.value = ".*SecM=1.*";

	SwInventoryFilter.name = MafCmRouterFilterTypeRegExp;
	SwInventoryFilter.value = ".*SwInventory=1.*";

	SysMFilter.name = MafCmRouterFilterTypeRegExp;
	SysMFilter.value = ".*SysM=1.*";

	myFilter[0] = &BrMFilter;
	myFilter[1] = &AxeFunctionsFilter;
	myFilter[2] = &FileMFilter;
	myFilter[3] = &HealthCheckMFilter;
	myFilter[4] = &SecMFilter;
	myFilter[5] = &SwInventoryFilter;
	myFilter[6] = &SysMFilter;
	myFilter[7] = 0;

	_eventConsumer.notify=&prcEvtNotify;
}

__CLASS_NAME__::~__CLASS_NAME__() {
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);
}

MafReturnT __CLASS_NAME__::start() {

	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);

  	MafReturnT retVal = _portal->getInterface(MafOamSpiEventService_1Id, (MafMgmtSpiInterface_1**)&_interface);

  	if (retVal != MafOk) {
  		::syslog(LOG_INFO,"%s : '_portal->getInterface' Failure - MAF error %i", __PRETTY_FUNCTION__,retVal);
  		sendMsgToServer("%s%s : '_portal->getInterface' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT,__PRETTY_FUNCTION__,retVal);
  		return retVal;
	}

	retVal = _interface->registerConsumer(&_eventConsumer, &_consumerHandle);
	if (retVal != MafOk){
		::syslog(LOG_INFO,"%s : 'interface->registerConsumer' Failure - MAF error %i", __PRETTY_FUNCTION__,retVal);
		sendMsgToServer("%s%s : 'interface->registerConsumer' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT,__PRETTY_FUNCTION__,retVal);
	}

	retVal = _interface->addSubscription(_consumerHandle, MafCmRouterEventTypeModifyMoAttr, myFilter);
	if (retVal != MafOk){
		::syslog(LOG_INFO,"%s : 'interface->addSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeModifyMoAttr,retVal);
		sendMsgToServer("%s%s : 'interface->addSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT,__PRETTY_FUNCTION__,MafCmRouterEventTypeModifyMoAttr,retVal);
	}
	retVal = _interface->addSubscription(_consumerHandle, MafCmRouterEventTypeCreateMo, myFilter);
	if (retVal != MafOk){
		::syslog(LOG_INFO,"%s : 'interface->addSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeCreateMo,retVal);
		sendMsgToServer("%s%s : 'interface->addSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT,__PRETTY_FUNCTION__,MafCmRouterEventTypeCreateMo,retVal);
	}

	retVal = _interface->addSubscription(_consumerHandle, MafCmRouterEventTypeDeleteMo, myFilter);
	if (retVal != MafOk){
		::syslog(LOG_INFO,"%s : 'interface->addSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeDeleteMo,retVal);
		sendMsgToServer("%s%s : 'interface->addSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT,__PRETTY_FUNCTION__,MafCmRouterEventTypeDeleteMo,retVal);
	}
	retVal = _interface->addSubscription(_consumerHandle, MafCmRouterEventTypeTxCommitted, myFilter);
	if (retVal != MafOk){
		::syslog(LOG_INFO,"%s : 'interface->addSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeTxCommitted,retVal);
		sendMsgToServer("%s%s : 'interface->addSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT,__PRETTY_FUNCTION__,MafCmRouterEventTypeTxCommitted,retVal);
	}

	return retVal;
}

MafReturnT __CLASS_NAME__::stop() {
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);

	MafReturnT retVal = MafOk;

	if ( (retVal = _interface->removeSubscription(_consumerHandle, MafCmRouterEventTypeModifyMoAttr, myFilter))){
		::syslog(LOG_INFO,"%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeModifyMoAttr,retVal);
		sendMsgToServer("%s%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__,MafCmRouterEventTypeModifyMoAttr,retVal);
	}
	if ( (retVal = _interface->removeSubscription(_consumerHandle, MafCmRouterEventTypeCreateMo, myFilter))){
		::syslog(LOG_INFO,"%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeCreateMo,retVal);
		sendMsgToServer("%s%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__,MafCmRouterEventTypeCreateMo,retVal);
	}
	if ( (retVal = _interface->removeSubscription(_consumerHandle, MafCmRouterEventTypeDeleteMo, myFilter))){
		::syslog(LOG_INFO,"%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeDeleteMo,retVal);
		sendMsgToServer("%s%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__,MafCmRouterEventTypeDeleteMo,retVal);
	}
	if ( (retVal = _interface->removeSubscription(_consumerHandle, MafCmRouterEventTypeTxCommitted, myFilter))){
		::syslog(LOG_INFO,"%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", __PRETTY_FUNCTION__,MafCmRouterEventTypeTxCommitted,retVal);
		sendMsgToServer("%s%s : 'interface->removeSubscription (%s)' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__,MafCmRouterEventTypeTxCommitted,retVal);
	}

	if ( (retVal = _interface->unregisterConsumer(_consumerHandle, & _eventConsumer))){
		::syslog(LOG_INFO,"%s : 'interface->unregisterConsumer' Failure - MAF error %i", __PRETTY_FUNCTION__,retVal);
		sendMsgToServer("%s%s : 'interface->unregisterConsumer' Failure - MAF error %i", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__,retVal);
	}

	return MafOk;
}

MafReturnT __CLASS_NAME__::init(MafMgmtSpiInterfacePortalAccessorT* accessor, const char* /*config*/){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);

	if ( !(_portal = (MafMgmtSpiInterfacePortal_3T*)accessor->getPortal("1"))){
		::syslog(LOG_INFO,"%s : 'interface->registerConsumer' Failure ", __PRETTY_FUNCTION__);
		sendMsgToServer("%s%s : 'interface->registerConsumer' Failure ", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);
		return MafFailure;
	}

	return _portal->registerComponent(&_component);
}

MafReturnT __CLASS_NAME__::terminate(){
	::syslog(LOG_INFO,"%s", __PRETTY_FUNCTION__);
	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);

	return _portal->unregisterComponent(&_component);
}

MafReturnT __CLASS_NAME__::notify(MafOamSpiEventConsumerHandleT /*handle*/, const char * eventType, MafNameValuePairT** /*filter*/, void * value){

	sendMsgToServer("%s%s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);

	bool inform_Server = true;

	if (!strcmp(eventType, MafCmRouterEventTypeCreateMo)){
		sendMsgToServer("%s%s : Notification type id %s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__, MafCmRouterEventTypeCreateMo);
	}
	else if (!strcmp(eventType, MafCmRouterEventTypeDeleteMo)){
		sendMsgToServer("%s%s : Notification type id %s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__, MafCmRouterEventTypeDeleteMo);
	}
	else if (!strcmp(eventType, MafCmRouterEventTypeModifyMoAttr)){
		sendMsgToServer("%s%s : Notification type id %s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__, MafCmRouterEventTypeModifyMoAttr);
	}
	else if (!strcmp(eventType, MafCmRouterEventTypeTxCommitted)){
		sendMsgToServer("%s%s : Notification type id %s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__, MafCmRouterEventTypeTxCommitted);
		MafOamSpiCmRouterTxCommittedValue_1T * theValue = static_cast<MafOamSpiCmRouterTxCommittedValue_1T*> (value);
		sendMsgToServer("%s%s : The following events have been committed :", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);
		for ( int i = 0; theValue->txChange[i]; i++ ){
			sendMsgToServer("%s%s : Event Type : %s -> Attribute name : %s -> MO : %s", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__, theValue->txChange[i]->eventType, theValue->txChange[i]->attributeName, theValue->txChange[i]->dn);
		}
		inform_Server = false;
	}
	else {
		sendMsgToServer("%s%s : Unexpected notification type", LOG_FROM_PRC_API_COM_COMPONENT, __PRETTY_FUNCTION__);
		inform_Server = false;
	}

	if ( inform_Server )
		sendMsgToServer("%s",MODEL_CHANGE_COMMITTED);

	return MafOk;
}

__CLASS_NAME__& __CLASS_NAME__::getInstance() {
	return _instance;
}

void __CLASS_NAME__::sendMsgToServer(const char* format, ...) {

	sockaddr_un socketAddress;
	memset(&socketAddress, 0, sizeof(struct sockaddr_un));
	socketAddress.sun_family = AF_UNIX;
	strncpy(socketAddress.sun_path, pipeName, sizeof(socketAddress.sun_path) - 1);

	char buffer[512] = {0};
	va_list args;
	va_start (args, format);
	vsnprintf (buffer,sizeof(buffer),format, args);
	va_end (args);

	if ((_prc_local_socket = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		::syslog(LOG_ERR,"%s : socket(%s) failed with error %i (%s)", __PRETTY_FUNCTION__, pipeName, errno, ::strerror(errno));
		return;
	}

	if (::connect(_prc_local_socket, (const struct sockaddr *) &socketAddress, sizeof(struct sockaddr_un))){
		::syslog(LOG_ERR,"%s : connect(%s) failed with error %i (%s)", __PRETTY_FUNCTION__, pipeName, errno, ::strerror(errno));
		::close(_prc_local_socket);
		return;
	}

	if (::write(_prc_local_socket,buffer,sizeof(buffer)) == -1){
		::syslog(LOG_ERR,"%s : write(%s) failed with error %i (%s)", __PRETTY_FUNCTION__, pipeName, errno, ::strerror(errno));
	}

	::close(_prc_local_socket);

}
