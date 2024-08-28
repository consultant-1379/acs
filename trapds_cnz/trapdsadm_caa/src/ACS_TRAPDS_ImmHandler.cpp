/*
 * ACS_TRAPDS_ImmHandler.cpp
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#include "ACS_TRAPDS_ImmHandler.h"

int ACS_TRAPDS_Implementer::svc()
{
	char msg_buff[256]={'\0'};
	ACS_TRA_trace* _trace = new ACS_TRA_trace("ACS_TRAPDS_Implementer");

	svc_run = true;

	ACS_TRAPDS_EventHandle m_shutdownEvent = ACS_TRAPDS_Event::OpenNamedEvent(TRAPDS_Util::EVENT_NAME_SHUTDOWN);

	int events[2] = {0};
	events[0] = m_stopEvent;
	events[1]= m_shutdownEvent;
	bool svc_exit = false;
	uint32_t implementerCount = 0;

	bool *implementerSet = new bool[m_size];
	for (uint32_t i = 0; i < m_size; i++)
	{
		implementerSet[i] = false;
	}

	do
	{
		for (uint32_t i = 0; i < m_size; i++)
		{
			if (!(implementerSet[i]))
			{

				if ((implementerSet[i] = setImplementer(i)))
				{
					snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_Implementer::svc, Successfully set implementer on class %s!", m_names[i].c_str());
					if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);
					cout << msg_buff << std::endl;
					ACS_TRAPDS_Event::SetEvent(m_subscriptionEvent[i]);
					implementerCount++;
				}
				else
				{
					cout<<"FAILURE:: " << m_names[i] << ".setClasstImplementer"<<endl;
					snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_Implementer::svc, setting implementer on class %s failed. Trying again...", m_names[i].c_str());
					cout << msg_buff << std::endl;
					if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);
				}
			}
		}

		if (implementerCount < m_size)
		{//Not all implementer are set. Try again

			snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_Implementer::svc, Not all class implementers are set. Trying again in 2 seconds...");
			cout << msg_buff << std::endl;
			if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);

			int eventIndex = ACS_TRAPDS_Event::WaitForEvents(2,events,2000);

			if (eventIndex == 0 || eventIndex == 1)
			{
				svc_exit = true;
			}
		}
		else
		{
			snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_Implementer::svc, All class implementer set.");
			if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);
		}
	} while (implementerCount < m_size && !svc_exit);

	delete[] implementerSet;
	delete(_trace);
	svc_run = false;

	return 0;
}

bool ACS_TRAPDS_Implementer::setImplementer(uint32_t index)
{

	bool result = false;

	ACS_CC_ReturnType returnCode;

	if (m_oImplementer[index] != NULL)
	{
		returnCode = m_oiHandler[index]->addClassImpl(m_oImplementer[index], m_names[index].c_str());
		if(ACS_CC_SUCCESS == returnCode)
		{
			result = true;
		}

	}
	return result;

}

/*============================================================================
	ROUTINE: ACS_TRAPDS_ImmHandler
 ============================================================================ */
ACS_TRAPDS_ImmHandler::ACS_TRAPDS_ImmHandler()
{
	// create the file descriptor to signal stop
	m_StopEvent = ACS_TRAPDS_Event::OpenNamedEvent(TRAPDS_Util::EVENT_NAME_SHUTDOWN);

	// create the file descriptor to signal svc terminated
	m_svcTerminated = eventfd(0,0);

	// Initialize the svc state flag
	svc_run = false;

	m_oi_TrapService = new ACS_TRAPDS_OI_TrapService();

	m_oi_TrapSubscriber = new ACS_TRAPDS_OI_TrapSubscriber();


	_trace = new ACS_TRA_trace("TRAPDS IMMHandler");

}

/*============================================================================
	ROUTINE: ~ACS_TRAPDS_ImmHandler
 ============================================================================ */
ACS_TRAPDS_ImmHandler::~ACS_TRAPDS_ImmHandler()
{


	ACS_TRAPDS_Event::CloseEvent(m_svcTerminated);

	m_oiHandlerService.removeClassImpl(m_oi_TrapService,m_oi_TrapService->getIMMClassName());

	m_oiHandlerSubscriber.removeClassImpl(m_oi_TrapSubscriber,m_oi_TrapSubscriber->getIMMClassName());

	if (m_oi_TrapSubscriber != NULL)
		delete m_oi_TrapSubscriber;

	if (m_oi_TrapService != NULL)
		delete m_oi_TrapService;

	delete(_trace);
}

/*============================================================================
	ROUTINE: open
 ============================================================================ */
int ACS_TRAPDS_ImmHandler::open(void *args)
{
//	ACS_CC_ReturnType returnCode;

	UNUSED(args);

//	returnCode = m_oiHandlerService.addClassImpl(m_oi_TrapService, m_oi_TrapService->getIMMClassName());
//	if(ACS_CC_FAILURE == returnCode)
//	{
//		cout<<" FAILURE :: setClasstImplemented on TrapSubscriber class !!! "<<endl;
//	}
//
//	returnCode = m_oiHandlerSubscriber.addClassImpl(m_oi_TrapSubscriber, m_oi_TrapSubscriber->getIMMClassName());
//	if(ACS_CC_FAILURE == returnCode)
//	{
//		cout<<" FAILURE :: setClasstImplemented on TrapSubscriber class !!! "<<endl;
//	}
//
//	if (returnCode != ACS_CC_SUCCESS)
//	{
//		m_oiHandlerSubscriber.removeClassImpl(m_oi_TrapSubscriber,m_oi_TrapSubscriber->getIMMClassName());
//		m_oiHandlerService.removeClassImpl(m_oi_TrapService,m_oi_TrapService->getIMMClassName());
//		return 1;
//	}

	activate();
	//sleep(1);

	cout<<"\n............Wait for events.........\n"<<endl;

	return 0;
}


/*============================================================================
	ROUTINE: svc
 ============================================================================ */
int ACS_TRAPDS_ImmHandler::svc()
{

	if (_trace->ACS_TRA_ON())
	{ //trace
		char tmpStr[512] = {0};
		snprintf(tmpStr, sizeof(tmpStr) - 1, "Thread Activated !!!");
		_trace->ACS_TRA_event(1, tmpStr);
	}

	char msg_buff[256]={'\0'};
	ACS_CC_ReturnType result;

	// Set the svc thread state to on
	svc_run = true;

	ACS_TRAPDS_EventHandle implementerEvent[4] = {0};

	int events[3] = {0};
	events[0]=m_StopEvent;

	// Events from 1 to 2 used to handle IMM implementers
	for (uint32_t i = 0; i < 2; i++)
	{
		implementerEvent[i] = ACS_TRAPDS_Event::CreateEvent(false,false,0);
		events[i+1]=implementerEvent[i];
	}

	//Prepare OI handler arrays
	acs_apgcc_oihandler_V3* handlers[2] = {0};
	acs_apgcc_objectimplementerinterface_V3* implementers[2] = {0};
	std::string names[2];

	handlers[0] = &m_oiHandlerService;
	implementers[0] = m_oi_TrapService;
	names[0] = m_oi_TrapService->getIMMClassName();

	handlers[1] = &m_oiHandlerSubscriber;
	implementers[1] = m_oi_TrapSubscriber;
	names[1] = m_oi_TrapSubscriber->getIMMClassName();

	//Start thread to set implementers
	ACS_TRAPDS_Implementer implementerThread(handlers, implementers, implementerEvent, names, 2);
	implementerThread.open();

	//Flags to recognize implementer set
	bool implementerSet[2] = {false};

	// waiting for IMM requests or stop

	while(svc_run)
	{
		int eventIndex=ACS_TRAPDS_Event::WaitForEvents(3,events,0);

        if (eventIndex == -1)
        {
			snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_ImmHandler::svc, exit after error=%s", strerror(errno) );
			if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);
			break;
		}

        else if (eventIndex == 0)   // Shutdown event
        {
        		svc_run=false;
                break;
        }
        else if (1 <= eventIndex && eventIndex <= 2)
        {// Implementer events
        	int j = eventIndex - 1;

        	if (implementerSet[j])
        	{
        		// Received a IMM request
        		result = implementers[j]->dispatch(ACS_APGCC_DISPATCH_ONE);

        		if(ACS_CC_SUCCESS != result)
        		{
        			snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_ImmHandler::svc, error on %s dispatch event", implementers[j]->getImpName().c_str() );
        			if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);
        			ACS_TRAPDS_Event::ResetEvent(events[eventIndex]);
        		}
        		continue;
        	}
        	else
        	{
        		//Received implementer set event
        		snprintf(msg_buff,sizeof(msg_buff)-1,"ACS_TRAPDS_ImmHandler::svc, %s implementer set, waiting for callbacks.",  implementers[j]->getImpName().c_str() );
        		if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, msg_buff);

        		events[eventIndex] = implementers[j]->getSelObj();
        		implementerSet[j] = true;
        	}
        }
	}

	// Signal svc termination
	signalSvcTermination();

	// Set the svc thread state to off
	svc_run = false;

	return 0;

}

/*============================================================================
	ROUTINE: signalSvcTermination
 ============================================================================ */
void ACS_TRAPDS_ImmHandler::signalSvcTermination()
{
	ACE_UINT64 stopEvent=1;
	ssize_t numByte;

	// Signal to IMM thread to stop
	numByte = write(m_svcTerminated, &stopEvent, sizeof(ACE_UINT64));

	if(sizeof(ACE_UINT64) != numByte)
	{
	//	cout << "ACS_TRAPDS_ImmHandler " <<__LINE__<< " call dispach " << __FUNCTION__<< endl;
	}

}
