/*
 * ACS_TRAPDS_ImmHandler.h
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_IMMHANDLER_H_
#define ACS_TRAPDS_IMMHANDLER_H_


/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/Task.h>
#include "ace/OS_NS_poll.h"
#include <sys/eventfd.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <list>
#include <map>

#include <acs_apgcc_oihandler_V3.h>
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"

#include "ACS_TRA_trace.h"

#include "ACS_TRAPDS_OI_TrapSubscriber.h"
#include "ACS_TRAPDS_OI_TrapService.h"
#include "ACS_TRAPDS_Event.h"
#include "ACS_TRAPDS_Util.h"
#include "ACS_TRAPDS_Imm_Util.h"


class ACS_TRAPDS_OI_TrapSubscriber;

class ACS_TRAPDS_Implementer: public ACE_Task_Base {
public:
	ACS_TRAPDS_Implementer(acs_apgcc_oihandler_V3 **handler, acs_apgcc_objectimplementerinterface_V3** implementer, ACS_TRAPDS_EventHandle *subscriptionEvent, std::string *names, uint32_t size):
		m_oiHandler(handler),
		m_oImplementer(implementer),
		m_subscriptionEvent(subscriptionEvent),
		m_names(names),
		m_stopEvent(ACS_TRAPDS_Event::CreateEvent(false,false,0)),
		m_size(size),
		svc_run(false)
	{};
	virtual ~ACS_TRAPDS_Implementer(){ACS_TRAPDS_Event::CloseEvent(m_stopEvent);};

	virtual int svc(void);

	virtual int open (void ) {activate(THR_DETACHED);return 0;};

	virtual void stop (){ACS_TRAPDS_Event::SetEvent(m_stopEvent);};

private:
	bool setImplementer(uint32_t);

	acs_apgcc_oihandler_V3 **m_oiHandler;
	acs_apgcc_objectimplementerinterface_V3 **m_oImplementer;
	ACS_TRAPDS_EventHandle *m_subscriptionEvent;
	std::string *m_names;
	ACS_TRAPDS_EventHandle m_stopEvent;
	uint32_t m_size;
	//bool *implementerSet;

	bool svc_run;

};

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class ACS_TRAPDS_ImmHandler: public ACE_Task_Base {
 public:

	/**
		@brief		Constructor
	*/
	ACS_TRAPDS_ImmHandler();

	/**
		@brief		Destructor
	*/
	virtual ~ACS_TRAPDS_ImmHandler();

	/**
	   @brief  		Run by a daemon thread
	*/
	virtual int svc(void);

	/**
	   @brief  		This method initializes a task and prepare it for execution
	*/
	virtual int open (void *args = 0);

	/**
	   @brief  		This method get the stop handle to terminate the svc thread
	*/
	void getStopHandle(int& stopFD){stopFD = m_StopEvent;};

	/**
	   @brief  		This method get the svc thread termination handle
	*/
	int& getSvcEndHandle() {return m_svcTerminated;};

	/**
	   @brief  		This method get the svc thread state
	*/
	bool getSvcState() const {return svc_run;};


 private:

	/**
	   @brief  		This method signal the svc thread termination
	*/
	void signalSvcTermination();

	/**
	   @brief  	m_oiHandler
	*/

	ACS_TRA_trace* _trace;

	acs_apgcc_oihandler_V3 m_oiHandlerSubscriber;

	ACS_TRAPDS_OI_TrapSubscriber* m_oi_TrapSubscriber;

	acs_apgcc_oihandler_V3 m_oiHandlerService;

	ACS_TRAPDS_OI_TrapService* m_oi_TrapService;

	int m_StopEvent;

	/**
	   @brief  	m_svcTerminated: to signal out-side the svc termination
	*/
	int m_svcTerminated;

	/**
	   @brief  	svc_run: svc state flag
	*/
	bool svc_run;

};


#endif /* ACS_TRAPDS_IMMHANDLER_H_ */
