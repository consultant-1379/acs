/*
 * * @file acs_rtr_immhandler.cpp
 *	@brief
 *	Class method implementation for ACS_RTR_ImmHandler.
 *
 *  This module contains the implementation of class declared in
 *  the acs_rtr_service.h module
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2011-06-14
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-12-03 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_rtr_immhandler.h"
#include "acs_rtr_filebased_objectImpl.h"
#include "acs_rtr_hashkey_objectImpl.h"
#include "acs_rtr_blockbased_objectImpl.h"
#include "acs_rtr_statistics_impl.h"
#include "acs_rtr_cpstatistics_impl.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"

#include "acs_apgcc_oihandler_V3.h"

#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"

ACS_RTR_TRACE_DEFINE(ACS_RTR_ImmHandler);

namespace {
	const unsigned int IMM_FAIL_RETRY_TIME = 1U; // 1 second
}

/*============================================================================
	ROUTINE: ACS_RTR_ImmHandler
 ============================================================================ */
ACS_RTR_ImmHandler::ACS_RTR_ImmHandler(ACS_RTR_Server* rtrServer)
: m_RTRServerObj(rtrServer),
  m_oi_FileBasedJob(NULL),
  m_oi_BlockBasedJob(NULL),
  m_oi_StatisticInfo(NULL),
  m_oi_CpStatisticInfo(NULL),
  m_oi_Hashkey(NULL),
  m_ImmReactor(NULL),
  m_StopRequest(false)
{
	// Instance a Reactor to handle IMM events
	ACE_TP_Reactor* tp_reactor_impl = new ACE_TP_Reactor();

	// the reactor will delete the implementation (tp_reactor_impl object) on destruction
	m_ImmReactor = new (std::nothrow) ACE_Reactor(tp_reactor_impl, true);

	if(NULL != m_ImmReactor)
	{
		// Initialize the ACE_Reactor
		m_ImmReactor->open(1);
	}
}

/*============================================================================
	ROUTINE: ~ACS_RTR_ImmHandler
 ============================================================================ */
ACS_RTR_ImmHandler::~ACS_RTR_ImmHandler()
{
	//cleanup already created OI
	removeFileBaseOI();
	removeHashKeyOI();
	removeBlockBaseOI();
	removeStatisticsInfoOI();
	removeCpStatisticsInfoOI();
	delete m_ImmReactor;
}

/*============================================================================
	ROUTINE: open
 ============================================================================ */
int ACS_RTR_ImmHandler::open(void *args)
{
	ACS_RTR_TRACE_MESSAGE("In");
	int result = FAILURE;
	UNUSED(args);

	if( (NULL != m_ImmReactor) )
	{
		// start the reactor scheduling thread
		result = activate();
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%d>", result);
	return result;
}

/*============================================================================
	ROUTINE: svc
 ============================================================================ */
int ACS_RTR_ImmHandler::svc()
{
	ACS_RTR_TRACE_MESSAGE("In");

	// register OI to IMM
	do
	{
		if( registerImmOI() )
		{
			ACS_RTR_TRACE_MESSAGE("OI registered to IMM");
			// OI registered
			break;
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("failed to register OI to IMM, try again after <%d> sec", IMM_FAIL_RETRY_TIME);
			// failed to register OI to IMM
			// wait IMM_FAIL_RETRY_TIME sec before retry
			sleep(IMM_FAIL_RETRY_TIME);
		}

	}while(!m_StopRequest);

	if(!m_StopRequest)
	{
		// Start of TR IA58647
		try
		{
			// start reactor loop
			m_ImmReactor->run_reactor_event_loop();
		}
		catch(const std::exception &e)
                {
			ACS_RTR_TRACE_MESSAGE("Exception: <%s>", e.what());
			ACS_RTR_TRACE_MESSAGE("Exception caught when calling run_reactor_event_loop()");
                }
		catch(...)
		{
			ACS_RTR_TRACE_MESSAGE("Unknown exception caught when calling run_reactor_event_loop()");
		}
		// End of TR IA58647
	}

	// remove all registered handles
	m_ImmReactor->purge_pending_notifications(0);

	m_ImmReactor->close();

	ACS_RTR_TRACE_MESSAGE("IMM reactor event handler terminate");

	//cleanup already created OI
	removeFileBaseOI();
	removeHashKeyOI();
	removeBlockBaseOI();
	removeStatisticsInfoOI();
	removeCpStatisticsInfoOI();

	ACS_RTR_TRACE_MESSAGE("Out");
	return SUCCESS;
}


/*============================================================================
	ROUTINE: stopImmHandler
 ============================================================================ */
void ACS_RTR_ImmHandler::stopImmHandler()
{
	ACS_RTR_TRACE_MESSAGE("In");

	m_StopRequest = true;
	// stop reactor
	m_ImmReactor->end_reactor_event_loop();

	// Wait for svc thread termination
	wait();

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*============================================================================
	ROUTINE: registerImmOI
 ============================================================================ */
bool ACS_RTR_ImmHandler::registerImmOI()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	// Create RTR OIs
	if( setFileBaseOI() &&  setBlockBaseOI() && setHashKeyOI()
			&& setStatisticsInfoOI() && setCpStatisticsInfoOI())
	{
		result = true;
	}
	else
	{
		//cleanup already created OI
		removeFileBaseOI();
		removeHashKeyOI();
		removeBlockBaseOI();
		removeStatisticsInfoOI();
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: setHashKeyOI 
 ============================================================================ */
bool ACS_RTR_ImmHandler::setHashKeyOI()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	// create the OI of FileBaseJob class
	m_oi_Hashkey = new (std::nothrow) ACS_RTR_HashKey_ObjectImpl();

	// check for error
	if(NULL == m_oi_Hashkey)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to allocate the OI of  class");
	}
	else
	{
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.addClassImpl(m_oi_Hashkey, m_oi_Hashkey->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to add Hash Key  OI");
			delete m_oi_Hashkey;
			m_oi_Hashkey= NULL;
		}
		else
		{
			// set the handler of events
			m_oi_Hashkey->dispatch(m_ImmReactor, ACS_APGCC_DISPATCH_ALL);
			result = true;
			ACS_RTR_TRACE_MESSAGE("HashKey OI added");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: removeHashKeyOI 
 ============================================================================ */
void ACS_RTR_ImmHandler::removeHashKeyOI()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(NULL != m_oi_Hashkey)
	{
		ACS_RTR_TRACE_MESSAGE("remove Hash Key OI");
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.removeClassImpl(m_oi_Hashkey , m_oi_Hashkey->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to remove Hash Key OI");
		}

		// remove handler from the reactor
		m_ImmReactor->remove_handler(m_oi_Hashkey, ACE_Event_Handler::DONT_CALL);

		delete m_oi_Hashkey;
		m_oi_Hashkey = NULL;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*============================================================================
	ROUTINE: setFileBaseOI
 ============================================================================ */
bool ACS_RTR_ImmHandler::setFileBaseOI()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	// create the OI of FileBaseJob class
	m_oi_FileBasedJob = new (std::nothrow) ACS_RTR_FileBased_ObjectImpl(m_RTRServerObj);

	// check for error
	if(NULL == m_oi_FileBasedJob)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to allocate the OI of FileBaseJob class");
	}
	else
	{
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.addClassImpl(m_oi_FileBasedJob, m_oi_FileBasedJob->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to add File Base OI");
			delete m_oi_FileBasedJob;
			m_oi_FileBasedJob = NULL;
		}
		else
		{
			// set the handler of events
			m_oi_FileBasedJob->dispatch(m_ImmReactor, ACS_APGCC_DISPATCH_ALL);
			result = true;
			ACS_RTR_TRACE_MESSAGE("File Job OI added");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: removeFileBaseOI
 ============================================================================ */
void ACS_RTR_ImmHandler::removeFileBaseOI()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(NULL != m_oi_FileBasedJob)
	{
		ACS_RTR_TRACE_MESSAGE("remove File Base OI");
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.removeClassImpl(m_oi_FileBasedJob, m_oi_FileBasedJob->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to remove File Base OI");
		}

		// remove handler from the reactor
		m_ImmReactor->remove_handler(m_oi_FileBasedJob, ACE_Event_Handler::DONT_CALL);

		delete m_oi_FileBasedJob;
		m_oi_FileBasedJob = NULL;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}


/*============================================================================
	ROUTINE: setBlockBaseOI
 ============================================================================ */
bool ACS_RTR_ImmHandler::setBlockBaseOI()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	// create the OI of BlockBaseJob class
	m_oi_BlockBasedJob = new (std::nothrow) ACS_RTR_BlockBased_ObjectImpl(m_RTRServerObj);

	// check for error
	if(NULL == m_oi_BlockBasedJob)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to allocate the OI of BlockBaseJob class");
	}
	else
	{
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.addClassImpl(m_oi_BlockBasedJob, m_oi_BlockBasedJob->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to add Block Base OI");
			delete m_oi_BlockBasedJob;
			m_oi_BlockBasedJob = NULL;
		}
		else
		{
			// set the handler of events
			m_oi_BlockBasedJob->dispatch(m_ImmReactor, ACS_APGCC_DISPATCH_ALL);
			result = true;
			ACS_RTR_TRACE_MESSAGE("Block Job OI added");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: removeFileBaseOI
 ============================================================================ */
void ACS_RTR_ImmHandler::removeBlockBaseOI()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(NULL != m_oi_BlockBasedJob)
	{
		ACS_RTR_TRACE_MESSAGE("remove Block Base OI");
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.removeClassImpl(m_oi_BlockBasedJob, m_oi_BlockBasedJob->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to remove Block Base OI");
		}

		// remove handler from the reactor
		m_ImmReactor->remove_handler(m_oi_BlockBasedJob, ACE_Event_Handler::DONT_CALL);

		delete m_oi_BlockBasedJob;
		m_oi_BlockBasedJob = NULL;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*============================================================================
	ROUTINE: setStatisticsInfoOI
 ============================================================================ */
bool ACS_RTR_ImmHandler::setStatisticsInfoOI()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	// create the OI of StatisticsInfo class
	m_oi_StatisticInfo = new (std::nothrow) ACS_RTR_Statistics_ObjectImpl(m_RTRServerObj);

	// check for error
	if(NULL == m_oi_StatisticInfo)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to allocate the OI of StatisticsInfo class");
	}
	else
	{
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.addClassImpl(m_oi_StatisticInfo, m_oi_StatisticInfo->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to add StatisticsInfo OI");
			delete m_oi_StatisticInfo;
			m_oi_StatisticInfo = NULL;
		}
		else
		{
			// set the handler of events
			m_oi_StatisticInfo->dispatch(m_ImmReactor, ACS_APGCC_DISPATCH_ALL);
			result = true;
			ACS_RTR_TRACE_MESSAGE("StatisticsInfo OI added");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: removeStatisticsInfoOI
 ============================================================================ */
void ACS_RTR_ImmHandler::removeStatisticsInfoOI()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(NULL != m_oi_StatisticInfo)
	{
		ACS_RTR_TRACE_MESSAGE("remove StatisticsInfo OI");
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.removeClassImpl(m_oi_StatisticInfo, m_oi_StatisticInfo->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to remove StatisticsInfo OI");
		}

		// remove handler from the reactor
		m_ImmReactor->remove_handler(m_oi_StatisticInfo, ACE_Event_Handler::DONT_CALL);

		delete m_oi_StatisticInfo;
		m_oi_StatisticInfo = NULL;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}


/*============================================================================
	ROUTINE: setCpStatisticsInfoOI
 ============================================================================ */
bool ACS_RTR_ImmHandler::setCpStatisticsInfoOI()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	// create the OI of CpStatisticsInfo class
	m_oi_CpStatisticInfo = new (std::nothrow) ACS_RTR_CpStatistics_ObjectImpl(m_RTRServerObj);

	// check for error
	if(NULL == m_oi_CpStatisticInfo)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to allocate the OI of CpStatisticsInfo class");
	}
	else
	{
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.addClassImpl(m_oi_CpStatisticInfo, m_oi_CpStatisticInfo->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to add CpStatisticsInfo OI");
			delete m_oi_CpStatisticInfo;
			m_oi_CpStatisticInfo = NULL;
		}
		else
		{
			// set the handler of events
			m_oi_CpStatisticInfo->dispatch(m_ImmReactor, ACS_APGCC_DISPATCH_ALL);
			result = true;
			ACS_RTR_TRACE_MESSAGE("CpStatisticsInfo OI added");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: removeStatisticsInfoOI
 ============================================================================ */
void ACS_RTR_ImmHandler::removeCpStatisticsInfoOI()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(NULL != m_oi_CpStatisticInfo)
	{
		ACS_RTR_TRACE_MESSAGE("remove CpStatisticsInfo OI");
		// add OI to IMM
		acs_apgcc_oihandler_V3 oiHandler;

		if(ACS_CC_FAILURE == oiHandler.removeClassImpl(m_oi_CpStatisticInfo, m_oi_CpStatisticInfo->getIMMClassName()))
		{
			ACS_RTR_TRACE_MESSAGE("Failed to remove CpStatisticsInfo OI");
		}

		// remove handler from the reactor
		m_ImmReactor->remove_handler(m_oi_CpStatisticInfo, ACE_Event_Handler::DONT_CALL);

		delete m_oi_CpStatisticInfo;
		m_oi_CpStatisticInfo = NULL;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}
