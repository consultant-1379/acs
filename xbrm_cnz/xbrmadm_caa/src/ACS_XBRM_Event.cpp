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
 * @file ACS_XBRM_Event.cpp
 *
 * @brief
 * ACS_XBRM_EventHandle Class for Event Handling
 *
 * @details
 * Implementation ofACS_XBRM_EventHandle Class for Event Handling of XBRM 
 * service
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

#include "ACS_XBRM_Event.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include <syslog.h>

using std::map;

// macros
#define FD_MAX(max_now,x)     ((x) > (max_now) ? (x) : (max_now))
using namespace std;

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_Event);

ACS_XBRM_Event::ACS_XBRM_Event(){
    ACS_XBRM_TRACE_FUNCTION;
}

ACS_XBRM_Event::~ACS_XBRM_Event(){
    ACS_XBRM_TRACE_FUNCTION;
}

ACS_XBRM_Event* ACS_XBRM_Event::getInstance(){
    ACS_XBRM_TRACE_FUNCTION;
    return instance_::instance();
}

ACS_XBRM_EventHandle ACS_XBRM_Event::CreateEvent(bool manualReset, bool initialState, const char* name){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_EventHandle eventHandle = -1;

    if(name != NULL){
        if((eventHandle = getInstance()->OpenNamedEvent(name)) != -1){
            return eventHandle;
        }
    }
    if((eventHandle = eventfd(0, 0)) == -1){
        return ACS_XBRM_Event_RC_ERROR;
    }

    EventProperties_t* eventProperties = new EventProperties_t();
    eventProperties->handle = eventHandle;
    eventProperties->manualReset = manualReset;

    if(name != NULL){
        NamedEventKeyTypePair event(name, eventProperties);
        getInstance()->lock_.acquire_write();
        getInstance()->namedEvents_.insert(event);
        getInstance()->lock_.release();
    }else{
        getInstance()->lock_.acquire_write();
        getInstance()->events_.push_back(eventProperties);
        getInstance()->lock_.release();
    }

    if(initialState)
        getInstance()->SetEvent(eventHandle);
    
    return eventHandle;
}

ACS_XBRM_EventHandle ACS_XBRM_Event::OpenNamedEvent(string name){
    ACS_XBRM_TRACE_FUNCTION;
    getInstance()->lock_.acquire_read();
    
    NamedEventIteratorType it = getInstance()->namedEvents_.find(name);

    if(it == getInstance()->namedEvents_.end()){
        getInstance()->lock_.release();
        return ACS_XBRM_Event_RC_ERROR;
    }

    ACS_XBRM_EventHandle eventHandle = it->second->handle;
    getInstance()->lock_.release();

    return eventHandle;
}

void ACS_XBRM_Event::CloseEvent(ACS_XBRM_EventHandle eventHandle)
{
    ACS_XBRM_TRACE_FUNCTION;
    getInstance()->lock_.acquire_write();

    NamedEventIteratorType it = getInstance()->findNamedEventByHandle(eventHandle);

    if (it != getInstance()->namedEvents_.end()) {
        close(eventHandle);     // close event handle
        delete it->second;      // free EventProperties_t
        getInstance()->namedEvents_.erase(it);
        getInstance()->lock_.release();
        return;
    }

    // check the list with unnamed events
    EventIteratorType unnamed_it = getInstance()->findEventByHandle(eventHandle);

    if (unnamed_it != getInstance()->events_.end()) {
        close(eventHandle);     // close event handle
        
        EventProperties_t *ptr = *unnamed_it;
        getInstance()->events_.erase(unnamed_it);
        delete ptr;

        getInstance()->lock_.release();
        return;
    }
    getInstance()->lock_.release();
}

int ACS_XBRM_Event::WaitForEvents(int count, int *eventHandles, int timeoutInMilliseconds)
{
    ACS_XBRM_TRACE_FUNCTION;
    fd_set fds;
    int nfds = 0;
    struct timeval *tv = NULL;

    // set timeout value (if any)
    if (timeoutInMilliseconds > 0) {
        tv = new struct timeval;

        unsigned long seconds = timeoutInMilliseconds / 1000;
        unsigned long ms = timeoutInMilliseconds % 1000;

        tv->tv_sec = seconds;
        tv->tv_usec = ms * 1000;
    }

    FD_ZERO(&fds);

    // go thru all fd's
    for (int i = 0;i < count; i++) {
    	if (eventHandles[i] < 0) { //Invalid handle
    		if (tv) {
    			delete(tv);
    			tv = NULL;
    		}
    		return ACS_XBRM_Event_RC_ERROR;
    	}
        FD_SET(eventHandles[i], &fds);

        // we want the highest fds
        nfds = FD_MAX(nfds, eventHandles[i]);
    }

    // wait for something to happen
    int ret = select(nfds + 1, &fds, NULL, NULL, tv);

    if (tv != NULL) {
        // free allocated memory
        delete tv;
        tv = NULL;
    }

    if (ret == 0) {
        // timeout has occurred
        return ACS_XBRM_Event_RC_TIMEOUT;

    } else if (ret == -1) {
        // a fault has occurred
        return ACS_XBRM_Event_RC_ERROR;
    }    // check the list of unnamed events

    int n = -1;

    // find out which one of the fds that has been signaled
    for (int i = 0;i < count; i++) {
        if (FD_ISSET(eventHandles[i], &fds)) {
            n = i;
            break;
        }
    }

    if (n != -1)
    {
        int handleActivated = eventHandles[n];

        ACS_XBRM_Event::EventProperties_t *ep = NULL;

        // locate what type of event this is, start in the named events list
        getInstance()->lock_.acquire_read();
        ACS_XBRM_Event::NamedEventIteratorType neit = getInstance()->findNamedEventByHandle(handleActivated);
        getInstance()->lock_.release();

        if (neit != getInstance()->namedEvents_.end()) {
            ep = neit->second;
        }

        if (ep == NULL) {
            // no property found in the named events list, search unnamed events
            getInstance()->lock_.acquire_read();
            ACS_XBRM_Event::EventIteratorType ueit = getInstance()->findEventByHandle(handleActivated);
            getInstance()->lock_.release();

            if (ueit != getInstance()->events_.end()) {
                ep = (*ueit);
            }
        }

        if (ep != NULL && !ep->manualReset) {
            // we should reset the event
            ResetEvent(handleActivated);
        }

        return n;
    }

    // should never come here
    return ACS_XBRM_Event_RC_ERROR;
}

bool ACS_XBRM_Event::SetEvent(ACS_XBRM_EventHandle eventHandle)
{
    ACS_XBRM_TRACE_FUNCTION;
    ssize_t buffer_length_read;
    uint64_t n = 1;

    buffer_length_read = write(eventHandle, &n, sizeof(n));
    if (buffer_length_read != sizeof(uint64_t)) {
        return false;
    }
    return true;
}

bool ACS_XBRM_Event::ResetEvent(ACS_XBRM_EventHandle eventHandle){
    ACS_XBRM_TRACE_FUNCTION;
    fd_set fds;
    struct timeval tv;

    // poll and make sure that it is possible to reset the event
    tv.tv_sec  = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(eventHandle, &fds);

    int ret = select(eventHandle + 1, &fds, NULL, NULL, &tv);

    if(ret == -1){
        // something bad has happened
        return false;

    }else if(ret == 0){
        // no data available (or rather, event not set)
        return false;
    }

    // reset event
    ssize_t buffer_length_read;
    uint64_t u = 0;

    buffer_length_read = read(eventHandle, &u, sizeof(u));

    if(buffer_length_read != sizeof(u))
        return false;
    return true;
}

// NOTE! Must be wrapped inside acquire/release statement
ACS_XBRM_Event::NamedEventIteratorType ACS_XBRM_Event::findNamedEventByHandle(ACS_XBRM_EventHandle handle)
{
    ACS_XBRM_TRACE_FUNCTION;
    // check the list with named events
    NamedEventIteratorType it = getInstance()->namedEvents_.begin();

    for (;it != getInstance()->namedEvents_.end(); it++) {
        // check if the handle is in the named events list
        if (it->second->handle == handle) {
            break;
        }
    }

    return it;
}


// NOTE! Must be wrapped inside acquire/release statement
ACS_XBRM_Event::EventIteratorType ACS_XBRM_Event::findEventByHandle(ACS_XBRM_EventHandle handle)
{
    ACS_XBRM_TRACE_FUNCTION;
    // check the list of unnamed events
    EventIteratorType it = getInstance()->events_.begin();

    for (;it != getInstance()->events_.end(); it++) {
        if ((*it)->handle == handle) {
            break;
        }
    }

    return it;
}