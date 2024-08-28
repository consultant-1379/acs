/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_Event.h
 *
 * @brief
 * ACS_XBRM_EventHandle Class for Event Handling
 *
 * @details
 * ACS_XBRM_EventHandle is an interface for Event Handling of XBRM service
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-01  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/


#ifndef ACS_XBRM_EVENT_H_
#define ACS_XBRM_EVENT_H_


#include <map>
#include <vector>
#include <iostream>

#include <stdlib.h>
#include <sys/eventfd.h>
#include "ace/Mutex.h"
#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "ace/Recursive_Thread_Mutex.h"

using std::string;
typedef int ACS_XBRM_EventHandle;
const int ACS_XBRM_Event_RC_ERROR   = -1;
const int ACS_XBRM_Event_RC_TIMEOUT = -2;

class ACS_XBRM_Event
{
    public:
        static ACS_XBRM_EventHandle CreateEvent(bool manualReset, bool initialState, const char* name);
        static ACS_XBRM_EventHandle OpenNamedEvent(string name);
        static void CloseEvent(ACS_XBRM_EventHandle eventHandle);
        static int WaitForEvents(int count, ACS_XBRM_EventHandle *eventHandles, int timeoutInMilliseconds);

        static bool SetEvent(ACS_XBRM_EventHandle eventHandle);
        static bool ResetEvent(ACS_XBRM_EventHandle eventHandle);

        friend class ACE_Singleton<ACS_XBRM_Event, ACE_Recursive_Thread_Mutex>;

    protected:
        ACS_XBRM_Event();
        ~ACS_XBRM_Event();
        static ACS_XBRM_Event* getInstance();

    private:
        ACE_RW_Mutex lock_;
        typedef struct {
            ACS_XBRM_EventHandle handle;
            bool manualReset;
        } EventProperties_t;

        std::vector<EventProperties_t*> events_;
        std::map<string, EventProperties_t*> namedEvents_;

        typedef std::pair<string, ACS_XBRM_Event::EventProperties_t*> NamedEventKeyTypePair;
        typedef std::vector<ACS_XBRM_Event::EventProperties_t*>::iterator EventIteratorType;
        typedef std::map<string, ACS_XBRM_Event::EventProperties_t*>::iterator NamedEventIteratorType;

        typedef ACE_Singleton<ACS_XBRM_Event, ACE_Recursive_Thread_Mutex> instance_;

        NamedEventIteratorType findNamedEventByHandle(ACS_XBRM_EventHandle handle);
        EventIteratorType findEventByHandle(ACS_XBRM_EventHandle handle);

        
};

#endif // ACS_XBRM_Event_h


