/*
 * ACS_TRAPDS_Event.cpp
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */


#include "ACS_TRAPDS_Event.h"

using std::map;

// macros
#define FD_MAX(max_now,x)     ((x) > (max_now) ? (x) : (max_now))


using namespace std;

//
// Protected constructor, we're using the singleton pattern
//
ACS_TRAPDS_Event::ACS_TRAPDS_Event()
{
}
ACS_TRAPDS_Event::~ACS_TRAPDS_Event()
{

}

ACS_TRAPDS_Event* ACS_TRAPDS_Event::instance()
{
    return instance_::instance();
}


ACS_TRAPDS_EventHandle ACS_TRAPDS_Event::CreateEvent(bool manualReset, bool initialState, const char* name)
{
    ACS_TRAPDS_EventHandle eventHandle = -1;

    if (name != NULL) {
        // named event, check if it already exists
        if ((eventHandle = instance()->OpenNamedEvent(name)) != -1) {
            // do not create any new event as this already exists
            return eventHandle;
        }
    }

    // allocate new event
    if ((eventHandle = eventfd(0, 0)) == -1) {
        // failed to create eventlock_
        return ACS_TRAPDS_Event_RC_ERROR;
    }

    // create event object
    EventProperties_t *properties = new EventProperties_t();

    properties->handle = eventHandle;
    properties->manualReset = manualReset;

    if (name != NULL) {
        // named event
        NamedEventKeyTypePair event(name, properties);

        instance()->lock_.acquire_write();
        instance()->namedEvents_.insert(event);
        instance()->lock_.release();


    } else {
        // unnamed event
        instance()->lock_.acquire_write();
        instance()->events_.push_back(properties);
        instance()->lock_.release();

    }

    if (initialState) {
        // event requested to be set from start
        instance()->SetEvent(eventHandle);
    }

    return eventHandle;
}


ACS_TRAPDS_EventHandle ACS_TRAPDS_Event::OpenNamedEvent(string name)
{
    instance()->lock_.acquire_read();

    NamedEventIteratorType it = instance()->namedEvents_.end();

    if ((it = instance()->namedEvents_.find(name)) == instance()->namedEvents_.end()) {
        // named event does not exist in our map
        instance()->lock_.release();
        return ACS_TRAPDS_Event_RC_ERROR;
    }

    ACS_TRAPDS_EventHandle handle = it->second->handle;
    instance()->lock_.release();

    return handle;
}


void ACS_TRAPDS_Event::CloseEvent(ACS_TRAPDS_EventHandle eventHandle)
{
    instance()->lock_.acquire_write();

    // check the list with named events
    NamedEventIteratorType neit = instance()->findNamedEventByHandle(eventHandle);

    if (neit != instance()->namedEvents_.end()) {


        close(eventHandle);     // close event handle

        delete neit->second;    // free EventProperties_t
        instance()->namedEvents_.erase(neit);
        instance()->lock_.release();
        return;
    }

    // check the list with unnamed events
    EventIteratorType ueit = instance()->findEventByHandle(eventHandle);

    if (ueit != instance()->events_.end()) {


        close(eventHandle);     // close event handle

        EventProperties_t *ptr = *ueit;
        instance()->events_.erase(ueit);
        delete ptr;

        instance()->lock_.release();
        return;
    }

    instance()->lock_.release();
}


int ACS_TRAPDS_Event::WaitForEvents(int count, int *eventHandles, int timeoutInMilliseconds)
{
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
    	if (eventHandles[i] < 0)
    		return ACS_TRAPDS_Event_RC_ERROR;
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
        return ACS_TRAPDS_Event_RC_TIMEOUT;

    } else if (ret == -1) {
        // a fault has occurred
        return ACS_TRAPDS_Event_RC_ERROR;
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

        ACS_TRAPDS_Event::EventProperties_t *ep = NULL;

        // locate what type of event this is, start in the named events list
        instance()->lock_.acquire_read();
        ACS_TRAPDS_Event::NamedEventIteratorType neit = instance()->findNamedEventByHandle(handleActivated);
        instance()->lock_.release();

        if (neit != instance()->namedEvents_.end()) {
            ep = neit->second;
        }

        if (ep == NULL) {
            // no property found in the named events list, search unnamed events
            instance()->lock_.acquire_read();
            ACS_TRAPDS_Event::EventIteratorType ueit = instance()->findEventByHandle(handleActivated);
            instance()->lock_.release();

            if (ueit != instance()->events_.end()) {
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
    return ACS_TRAPDS_Event_RC_ERROR;
}


bool ACS_TRAPDS_Event::SetEvent(ACS_TRAPDS_EventHandle eventHandle)
{
    ssize_t sz;
    uint64_t n = 1;

    sz = write(eventHandle, &n, sizeof(uint64_t));
    if (sz != sizeof(uint64_t)) {
        // write fault
        //ACS_CS_TRACE((ACS_TRAPDS_Event_TRACE, "(%t) Failed to set event for handle: %d", eventHandle));
        return false;
    }

    return true;
}


bool ACS_TRAPDS_Event::ResetEvent(ACS_TRAPDS_EventHandle eventHandle)
{
    fd_set fds;
    struct timeval tv;

    // poll and make sure that it is possible to reset the event
    tv.tv_sec  = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(eventHandle, &fds);

    int ret = select(eventHandle + 1, &fds, NULL, NULL, &tv);

    if (ret == -1) {
        // something bad has happened


        return false;

    } else if (ret == 0) {
        // no data available (or rather, event not set)
        return false;
    }

    // reset event
    ssize_t s;
    uint64_t u = 0;

    s = read(eventHandle, &u, sizeof(uint64_t));

    if (s != sizeof(uint64_t)) {
        return false;
    }

    return true;
}


// NOTE! Must be wrapped inside acquire/release statement
ACS_TRAPDS_Event::NamedEventIteratorType ACS_TRAPDS_Event::findNamedEventByHandle(ACS_TRAPDS_EventHandle handle)
{
    // check the list with named events
    NamedEventIteratorType neit = instance()->namedEvents_.begin();

    for (;neit != instance()->namedEvents_.end(); neit++) {
        // check if the handle is in the named events list
        if (neit->second->handle == handle) {
            break;
        }
    }

    return neit;
}


// NOTE! Must be wrapped inside acquire/release statement
ACS_TRAPDS_Event::EventIteratorType ACS_TRAPDS_Event::findEventByHandle(ACS_TRAPDS_EventHandle handle)
{
    // check the list of unnamed events
    EventIteratorType ueit = instance()->events_.begin();

    for (;ueit != instance()->events_.end(); ueit++) {
        if ((*ueit)->handle == handle) {
            break;
        }
    }

    return ueit;
}
