/*ACS_CS_BrfSubscriber.cpp
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
ACS_CS_Trace_TDEF(ACS_CS_BrfSubscriber_TRACE);


/*============================================================================
        ROUTINE: ACS_CS_BrfSubscriber
 ============================================================================ */

int ACS_CS_BrfSubscriber::svc()
{
        ACS_CS_TRACE((ACS_CS_BrfSubscriber_TRACE,  "ACS_CS_BrfSubscriber::svc, start!"));

        m_svc_run = true;

        ACS_CS_EventHandle shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);

        const int stopEvent_Index  = 0;
        const int shutdownEvent_Index  = 1;

        const int num_handle = 2;
        int events[num_handle] = {0};
        events[0] = m_stopEvent;
        events[1]= shutdownEvent;
        bool svc_exit = false;
do
        {
                if (ACS_CS_BrfHandler::getInstance()->checkBrmPersistentDataOwnerObject() == TC_NOERROR)
                {
                        if ((setBrfImplementer()))
                        {
                                ACS_CS_FTRACE((ACS_CS_BrfSubscriber_TRACE,LOG_LEVEL_INFO,  "ACS_CS_BrfSubscriber::svc, Successfully registered as Brf Participant!"));
                                ACS_CS_BrfHandler::getInstance()->setBrfImplementerState(true);
                                ACS_CS_Event::SetEvent(m_subscriptionEvent);
                                svc_exit = true;
                        }
                        else
                        {
                                ACS_CS_FTRACE((ACS_CS_BrfSubscriber_TRACE, LOG_LEVEL_INFO, "ACS_CS_BrfSubscriber::svc, registration as BrfParticipant failed. Trying again..."));
                        }
                }
                else
                {
                                ACS_CS_FTRACE((ACS_CS_BrfSubscriber_TRACE, LOG_LEVEL_INFO, "ACS_CS_BrfSubscriber::svc, creation of BrfPersistentDataOwner object failed. Trying again..."));
                }


                if (!svc_exit)
                {//Try again
                        int eventIndex = ACS_CS_Event::WaitForEvents(2,events,5000);

                        if (eventIndex == stopEvent_Index || eventIndex == shutdownEvent_Index)
                        {
                                if(eventIndex == stopEvent_Index)
                                        ACS_CS_Event::ResetEvent(m_stopEvent);;
                                svc_exit = true;
                        }
                }

        } while (!svc_exit);

        m_svc_run = false;

        return 0;
}
bool ACS_CS_BrfSubscriber::setBrfImplementer()
{
        bool result = false;

        ACS_CC_ReturnType returnCode;

        if (m_oi_BrfPersistentDataOwner != 0 && m_oiHandlerBrfPersistentDataOwner != 0)
        {

                m_oi_BrfPersistentDataOwner->setObjName(ACS_CS_ImmMapper::DN_BRFPERSISTENTDATAOWNER);
                m_oi_BrfPersistentDataOwner->setImpName(ACS_CS_ImmMapper::IMPLEMENTER_BRF);
                m_oi_BrfPersistentDataOwner->setScope(ACS_APGCC_ONE);

                returnCode = m_oiHandlerBrfPersistentDataOwner->addObjectImpl(m_oi_BrfPersistentDataOwner);

                if(ACS_CC_SUCCESS == returnCode )
                {
                        result = true;;
                }
        }
        return result;
}
