#include <unistd.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <ComMgmtSpiServiceIdentities_1.h>
#include <ComMwSpiServiceIdentities_1.h>
#include <ComOamSpiServiceIdentities_1.h>
#include <ComOamFmEvent_1.h> // N.B. Not in the official SPI, yet

#include "FmApg.h"

FmApg FmApg::_instance;

extern "C" ComReturnT fm_apg_start(ComStateChangeReasonT reason)
{
	(void)reason;
    	return FmApg::getInstance().start();
}

extern "C" ComReturnT fm_apg_stop(ComStateChangeReasonT reason)
{
	(void)reason;
	return FmApg::getInstance().stop();
}

extern "C" ComReturnT comLCMinit( struct ComMgmtSpiInterfacePortalAccessor* accessor,
                                  const char* config)
{

	std::cout <<"FmApg::extern comLCMInit"<<std::endl;
	return FmApg::getInstance().init(accessor, config);
}

extern "C" ComReturnT comLCMterminate()
{
	std::cout << "I made it!" << std::endl;
	return FmApg::getInstance().terminate();
}

extern "C" ComReturnT  fm_apg_notify(ComOamSpiEventConsumerHandleT handle, const char* eventType, ComNameValuePairT** filter, void* value)
{
	return FmApg::getInstance().notify(handle, eventType, filter, value);
}

FmApg::FmApg() : _eventRouter(0)
{
	log.Open("APG_COM_FM");
	_consumerHandle=0;

   	_ifArray[0]=0;

   	_depArray[0] = (ComMgmtSpiInterface_1T*)&ComMwSpiLog_1Id;
   	_depArray[1] = (ComMgmtSpiInterface_1T*)&ComMwSpiTrace_1Id;
    	_depArray[2] = (ComMgmtSpiInterface_1T*)&ComOamSpiEventService_1Id;
   	_depArray[3] = 0;

    	_component.base.componentName = "fm_apg:kaspars";
    	_component.base.interfaceName=  ComMgmtSpiComponentInterfaceName_1;
    	_component.base.interfaceVersion= ComMgmtSpiComponentInterfaceVersion_1;

    	_component.interfaceArray = _ifArray;
    	_component.dependencyArray = _depArray;
    	_component.start = &fm_apg_start;
   	_component.stop = &fm_apg_stop;
    
    	_fmNotificationFilter.name = ComOamFmNotificationEventFilterType_1;
    	_fmNotificationFilter.value = 0;
    
    	_fmNotificationFilters[0] = &_fmNotificationFilter;
    	_fmNotificationFilters[1] = 0;
    	_eventConsumer.notify=&fm_apg_notify;

    	std::cout << "sample apg fm module" << std::endl;
}

FmApg::~FmApg()
{
    	std::cout << "sample apg fm module destructor" << std::endl;
    	log.Write("sample apg fm module destructor", LOG_LEVEL_INFO);
	
}

ComReturnT FmApg::init(ComMgmtSpiInterfacePortalAccessorT* accessor, const char* config)
{
	(void)config;
    	_portal = (ComMgmtSpiInterfacePortal_1T*)accessor->getPortal("1");
    	return _portal->registerComponent(&_component);
}

ComReturnT FmApg::terminate()
{
    	return _portal->unregisterComponent(&_component);
}

FmApg& FmApg::getInstance()
{
    	return _instance;
}

ComReturnT FmApg::start()
{
    	std::cout<<"starting fm_apg component"<<std::endl;
    	log.Write("starting fm_apg component",LOG_LEVEL_INFO); 
    	ComMgmtSpiInterface_1T* interface = 0;
    	ComReturnT retVal = _portal->getInterface((ComMgmtSpiInterface_1T)ComOamSpiEventService_1Id, &interface);
    	if (retVal != ComOk)
  	{
        	std::cout<<"eventRouter getInterface --- very bad"<<std::endl;
        	log.Write("eventRouter getInterface --- very bad", LOG_LEVEL_INFO);
    	}
    	_eventRouter = (ComOamSpiEventRouter_1T*)interface;
    	retVal =_eventRouter->registerConsumer(&_eventConsumer, &_consumerHandle); 

    	if (retVal != ComOk)
    	{
       		std::cout<<"registerConsumer Failure"<<std::endl;
       		log.Write("registerConsumer Failure", LOG_LEVEL_ERROR);
    	}

    	retVal=_eventRouter->addSubscription(_consumerHandle, ComOamFmNotificationEventType_1, _fmNotificationFilters);
    	if (retVal != ComOk)
   	{
        	std::cout<<"addSubscrtption Failure"<<std::endl;
        	log.Write("addSubscrtption Failure", LOG_LEVEL_INFO);
    	}

    	return ComOk;
}

ComReturnT FmApg::stop()
{
    	std::cout<<"stopping fm_apg component"<<std::endl;
    	log.Write("stopping fm_apg component", LOG_LEVEL_INFO);
    	return ComOk;
}

ComReturnT FmApg::notify(ComOamSpiEventConsumerHandleT handle, const char* eventType, ComNameValuePairT** filter, void* value)
{
	std::cout<<"fm_apg: notified"<<std::endl;
	log.Write("fm_apg: notified", LOG_LEVEL_INFO);
	(void)eventType;
	(void)filter;
	(void)handle;
	char buffer[200];

	if ( std::strcmp(eventType, ComOamFmNotificationEventType_1) == 0)
	{
 	   	ComOamFmNotificationEventValue_1T* theValue = static_cast<ComOamFmNotificationEventValue_1T*>(value);

		/* print sequence number */
		std::cout << "SeqNo: "<<theValue->seqNo<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "SeqNo: [%llu]", (unsigned long long)theValue->seqNo);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print Original SeqNo number */
		std::cout << "Index: "<<theValue->originalSeqNo<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "Index: [%llu]", (unsigned long long)theValue->originalSeqNo);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print specificProblem number */
		std::cout << "Spec: "<<theValue->specificProblem<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "Spec: [%s]", theValue->specificProblem);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print Major number */
		std::cout << "Major: "<<theValue->notification->majorType<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "Major: [%d]", theValue->notification->majorType);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print Minor number */
		std::cout << "Minor: "<<theValue->notification->minorType<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "Minor: [%d]", theValue->notification->minorType);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print MO(dn) */
    		std::cout << "MO: "<< theValue->notification->dn << std::endl; 

		buffer[0]='\0';
		sprintf(buffer, "MO: [%s]", theValue->notification->dn);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print Time */
		std::cout << "Time: "<<getTimeStr(theValue->eventTime)<<std::endl;	

		buffer[0]='\0';
		std::string temp = getTimeStr(theValue->eventTime);
		sprintf(buffer, "Time: [%s]", temp.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print Severity */
		std::cout << "Sev: "<<getSeverityStr(theValue->notification->severity)<<std::endl;	

		buffer[0]='\0';
		temp=getSeverityStr(theValue->notification->severity);
		sprintf(buffer, "Sev: [%s]", temp.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print Severity */
		std::cout << "Stateful: "<<theValue->stateful<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "Stateful: [%d]", theValue->stateful);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print EvType */
		std::cout << "EvType: "<<theValue->eventType<<std::endl;	

		buffer[0]='\0';
		sprintf(buffer, "EvType: [%d]", theValue->eventType);
		log.Write(buffer, LOG_LEVEL_INFO);
		
		/* print ProbCause */
    		std::cout << "ProbCause: "<< theValue->probableCause<< std::endl; 

		buffer[0]='\0';
		sprintf(buffer, "ProbCause: [%d]", theValue->probableCause);
		log.Write(buffer, LOG_LEVEL_INFO);

		/* print OrigTime */
		std::cout << "OrigTime: "<<getTimeStr(theValue->originalEventTime)<<std::endl;	

		buffer[0]='\0';
		temp=getTimeStr(theValue->originalEventTime);
		sprintf(buffer, "OrigTime: [%s]", temp.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);
		
		/* print OrigSev */
		std::cout << "OrigSev: "<<getSeverityStr(theValue->originalSeverity)<<std::endl;	

		buffer[0]='\0';
		temp=getSeverityStr(theValue->originalSeverity);
		sprintf(buffer, "OrigSev: [%s]", temp.c_str());
		log.Write(buffer, LOG_LEVEL_INFO);
		
		/* print OrigText */
    		std::cout << "OrigText: "<<theValue->originalAdditionalText<< std::endl; 

		buffer[0]='\0';
		sprintf(buffer, "OrigText: [%s]", theValue->originalAdditionalText);
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	else {
		std::cout<<"fm_apg:: Error! Event not handled"<<std::endl;
		log.Write("fm_apg:: Error! Event not handled", LOG_LEVEL_ERROR);
	}
    	return ComOk;
}

std::string FmApg::getTimeStr(uint64_t datetime)
{

    time_t date_time =  (time_t) ( datetime / FmApg::NANOS_PER_SEC );
    tm  tm_buf;
    tm* tm = localtime_r( &date_time, &tm_buf );
    std::ostringstream oss;

    if( ! tm ) {
        // localtime_r returned error
        // ERROR("Failed to convert time value (using time '1970 Jan 1 00:00:00.000')");
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

std::string FmApg::getSeverityStr(ComOamSpiNotificationFmSeverityT severity)
{
    std::string severityStr;

    switch ( severity ) {
    case ComOamSpiNotificationFmSeverityCleared:
        severityStr = std::string("CLEARED");
        break;
    case ComOamSpiNotificationFmSeverityIndeterminate:
        severityStr = std::string("INDETERMINATE");
        break;
    case ComOamSpiNotificationFmSeverityWarning:
        severityStr = std::string("WARNING");
        break;
    case ComOamSpiNotificationFmSeverityMinor:
        severityStr = std::string("MINOR");
        break;
    case ComOamSpiNotificationFmSeverityMajor:
        severityStr = std::string("MAJOR");
        break;
    case ComOamSpiNotificationFmSeverityCritical:
        severityStr = std::string("CRITICAL");
        break;
    default:
        severityStr = std::string("UNKNOWN");
    }
    return severityStr;
}
