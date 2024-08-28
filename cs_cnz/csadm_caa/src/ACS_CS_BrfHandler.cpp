/*
 * ACS_CS_BrfHandler.cpp
 *
 *  Created on: APR 23, 2014
 *      Author: tcspada
 */

#include "ACS_CS_BrfHandler.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_SubscriptionAgent.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_Trace.h"
using namespace std;
using namespace ACS_CS_NS;
ACS_CS_Trace_TDEF(ACS_CS_BrfHandler_TRACE);

ACS_CS_BrfHandler* ACS_CS_BrfHandler::m_instance = 0;
//class ACS_CS_BrfPersistentDataOwner;
ACS_CS_BrfHandler::ACS_CS_BrfHandler()
:m_oi_BrfPersistentDataOwner(0),
m_backup(false),
m_svc_run(false),
isBrfImplementerSet(false),
m_neHandler(0)
{
	m_ShutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);
	m_oi_BrfPersistentDataOwner = new ACS_CS_BrfPersistentDataOwner();
}

ACS_CS_BrfHandler::~ACS_CS_BrfHandler()
{
	if((m_oi_BrfPersistentDataOwner!=0)  && (isBrfImplementerSet==true))
	{
		if(m_oiHandlerBrfPersistentDataOwner.removeObjectImpl(m_oi_BrfPersistentDataOwner)!=ACS_CC_SUCCESS)
		{
			ACS_CS_FTRACE((ACS_CS_BrfHandler_TRACE, LOG_LEVEL_INFO," RemoveObjImpl Fail"));
		}
		else
		{
			ACS_CS_FTRACE((ACS_CS_BrfHandler_TRACE, LOG_LEVEL_INFO," RemoveObjImpl"));
		}
	}
	
	if (m_oi_BrfPersistentDataOwner!=0){
		delete(m_oi_BrfPersistentDataOwner);
		m_oi_BrfPersistentDataOwner = 0;
	}
	ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE, " destructor"));

}

ACS_CS_BrfHandler* ACS_CS_BrfHandler::getInstance ()
{
	//Lock lock;
	if (m_instance == 0)
	{
		m_instance = new ACS_CS_BrfHandler();
	}

	return m_instance;
}
void ACS_CS_BrfHandler::initialize(ACS_CS_NEHandler* neHandler)
{
	m_neHandler = neHandler;
}
void ACS_CS_BrfHandler::finalize ()
{
	delete(m_instance);
	m_instance = 0;
}

int ACS_CS_BrfHandler::open(void *args)
{
	UNUSED(args);
	//int retries = 0;

	activate(THR_DETACHED);
	cout<<"\n............Wait for events.........\n"<<endl;

	return 0;

}


/*============================================================================
	ROUTINE: svc
 ============================================================================ */
int ACS_CS_BrfHandler::svc()
{

	ACS_CC_ReturnType result;

	// Set the svc thread state to on
	m_svc_run = true;

	bool brf_participant = false;
	
	ACS_CS_EventHandle subscriptionEvent = ACS_CS_Event::CreateEvent(false,false,0);
	
	const int shutdownEvent_Index = 0;
	const int subscrEvent_Index = 1;
	const int handles =2;	
	int events[handles] = {0};
	events[0]=m_ShutdownEvent;

	// Brf Subscription
	events[1]=subscriptionEvent;

	ACS_CS_BrfSubscriber subscriber(&m_oiHandlerBrfPersistentDataOwner, m_oi_BrfPersistentDataOwner, subscriptionEvent);
	subscriber.open();

	
	while(m_svc_run)
	{
		int eventIndex=ACS_CS_Event::WaitForEvents(handles,events,5000);

		if (eventIndex == ACS_CS_EVENT_RC_TIMEOUT )
		{
			continue;
		}

		else if (eventIndex == ACS_CS_EVENT_RC_ERROR)
		{
				ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE, "received error"));break;
		}

		else if (eventIndex == shutdownEvent_Index)   // SHUTDOWN  event
		{
			{
				ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE, "ACS_CS_BrfHandler::svc, recieved a SHUTDOWN event"));
			}
		//	ACS_CS_Event::ResetEvent(m_StopEvent);
			m_svc_run=false;
			break;
		}
		else if (eventIndex == subscrEvent_Index )   // IMM callback or subscription event
		{
			if (brf_participant)
			{//Brf callback
				// Received a IMM request
				result = m_oi_BrfPersistentDataOwner->dispatch(ACS_APGCC_DISPATCH_ONE);

					ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE, "ACS_CS_BrfPersistentDataOwner::svc, dispatch event"));
				if(ACS_CC_SUCCESS != result)
				{
					ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE, "ACS_CS_BrfPersistentDataOwner::svc, error on BrmPersistentDataOwner dispatch event"));
					ACS_CS_Event::ResetEvent(events[1]);
				}
			}
			else
			{//Subscribed to Brf
				ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE,"ACS_CS_BrfPersistentDataOwner::svc, Received Brf Subscription event, waiting for callbacks."));
				events[1] = m_oi_BrfPersistentDataOwner->getSelObj();
				brf_participant = true;
			}
			continue;
		}

	}

	subscriber.stop();
	ACS_CS_Event::CloseEvent(subscriptionEvent);

	
	// Set the svc thread state to off
	m_svc_run = false;

	
	ACS_CS_TRACE((ACS_CS_BrfHandler_TRACE, " BrfHandler Thread closed"));
	return 0;

}
void ACS_CS_BrfHandler::stop ()
{
/*        std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << " Inside STOP.... "<< std::endl;
        if (m_StopEvent)
        {
                int retries=0;
                while (ACS_CS_Event::SetEvent(m_StopEvent) == 0)
                {
                        if( ++retries > 10) break;
                        sleep(1);
                }

                if (retries>10)
                {
			ACS_CS_FTRACE((ACS_CS_BrfHandler_TRACE, LOG_LEVEL_ERROR,
                                        " Error:Cannot signal Stop event "));
                }
        }*/
}


int ACS_CS_BrfHandler::checkBrmPersistentDataOwnerObject()
{
        int res = TC_NOERROR;

        std::string dn_brm = ACS_CS_ImmMapper::DN_BRFPERSISTENTDATAOWNER;

        ACS_APGCC_ImmObject object;

        if (ACS_CS_ImUtils::getObject(dn_brm, &object))
        {
		//isBrfImplementerSet = true;	
                cout <<"FOUND"<<endl;
        }
        else
        {
                cout << "No BrmPersistentDataOwnerObject object exists, create!" << endl;
                if(createBrmPersistentDataOwnerObject() != 0)
                {
                        cout << "ERROR: Could not create BrmPersistentDataOwnerObject object!" <<endl;
                        res = TC_INTERNALERROR;
                }
        }

        return res;
}

int ACS_CS_BrfHandler::createBrmPersistentDataOwnerObject()
{

        vector<ACS_CC_ValuesDefinitionType> AttrList;

        int backupType = 1;
//      int rebootAfterRestore = 0;
        int res = TC_NOERROR;

        ACS_CC_ValuesDefinitionType RDN = ACS_CS_ImUtils::defineAttributeString(ACS_CS_ImmMapper::ATT_BRFPERSISTENTDATAOWNER_RDN,ATTR_STRINGT,ACS_CS_ImmMapper::RDN_BRFPERSISTENTDATAOWNER,1);
        ACS_CC_ValuesDefinitionType VERSION = ACS_CS_ImUtils::defineAttributeString(ACS_CS_ImmMapper::ATT_BRFPERSISTENTDATAOWNER_VERSION,ATTR_STRINGT,const_cast<char*>("1.0"),1);
        ACS_CC_ValuesDefinitionType BACKUPTYPE = ACS_CS_ImUtils::defineAttributeInt(ACS_CS_ImmMapper::ATT_BRFPERSISTENTDATAOWNER_BACKUPTYPE,ATTR_INT32T,&backupType,1);
//      ACS_CC_ValuesDefinitionType REBOOTAFTERRESTORE = ACS_CS_ImUtils::defineAttributeInt(ACS_CS_ImmMapper::ATT_BRFPERSISTENTDATAOWNER_REBOOTAFTERRESTORE,ATTR_INT32T,&rebootAfterRestore,1);

        AttrList.push_back(RDN);
        AttrList.push_back(VERSION);
        AttrList.push_back(BACKUPTYPE);
//      AttrList.push_back(REBOOTAFTERRESTORE);


        if (!ACS_CS_ImUtils::createImmObject(ACS_CS_ImmMapper::classBrfPersistentDataOwner,ACS_CS_ImmMapper::RDN_BRFPARTICIPANTCONTAINER,AttrList))
        {
                cout << "BrfPersistentDataOwner Object Creation FAILURE for " << ACS_CS_ImmMapper::RDN_BRFPERSISTENTDATAOWNER << endl;
                res = TC_INTERNALERROR;
        }
        else
        {
                cout << "BrfPersistentDataOwner Object Creation Successful for " << ACS_CS_ImmMapper::RDN_BRFPERSISTENTDATAOWNER << endl;
		//isBrfImplementerSet = true;
                res = TC_NOERROR;
        }

        //free memory
        delete[] RDN.attrValues;
        RDN.attrValues = NULL;
        delete[] VERSION.attrValues;
        VERSION.attrValues = NULL;
        delete[] BACKUPTYPE.attrValues;
        BACKUPTYPE.attrValues = NULL;
//      delete[] REBOOTAFTERRESTORE.attrValues;
//      REBOOTAFTERRESTORE.attrValues = NULL;

        return res;
}

void ACS_CS_BrfHandler::setBackupOngoing(bool backup)
{
                ACS_CS_FTRACE((ACS_CS_BrfHandler_TRACE, LOG_LEVEL_ERROR,
                                        " setBackupOngoingto %d",backup));

        m_backup = backup;
}

bool ACS_CS_BrfHandler::isBackupOngoing()
{
        ACS_CS_FTRACE((ACS_CS_BrfHandler_TRACE, LOG_LEVEL_ERROR,
                                        " isBackupOngoing %d",m_backup));

        return m_backup;
}
bool ACS_CS_BrfHandler::isSwitchOngoing()
{
	bool switchOngoing = false;
	if(m_neHandler)
	{
		if((m_neHandler->isOmPrfSwitching()) || (m_neHandler->isOpModeSwitching()))
		switchOngoing = true;
	}
	return switchOngoing;
}
