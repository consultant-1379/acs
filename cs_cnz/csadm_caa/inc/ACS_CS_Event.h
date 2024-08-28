//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#ifndef ACS_CS_Event_h
#define ACS_CS_Event_h 1

#include <map>
#include <vector>
#include <iostream>

#include <stdlib.h>
#include "ace/Mutex.h"
#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "ace/Recursive_Thread_Mutex.h"

using std::string;
typedef int ACS_CS_EventHandle;
const int ACS_CS_EVENT_RC_ERROR   = -1;
const int ACS_CS_EVENT_RC_TIMEOUT = -2;

class ACS_CS_Event
{
    public:
        //
        // Create a new event. If the parameter name is set to NULL then a
        // unnamed event will be created
        //
        // Returns:
        //   Handle to the created event or ACS_CS_EVENT_RC_ERROR if the
        //   event could not be created for some reason
        //
        static ACS_CS_EventHandle CreateEvent(bool manualReset, bool initialState, const char* name);

        //
        // Opens a named event
        //
        // Returns:
        //   Handle to the named event or ACS_CS_EVENT_RC_ERROR if the
        //   event could not be found
        //
        static ACS_CS_EventHandle OpenNamedEvent(string name);

        //
        // Closes an event with handle as parameter, used for both named and unnamed events
        //
        static void CloseEvent(ACS_CS_EventHandle eventHandle);

        //
        // Wait for multiple events
        //
        // Parameters:
        //   count         - Specifies how many events the eventHandles array contains
        //   *eventHandles - A array of handles to events (or file descriptors) that we're going to wait for.
        //                   So all file handles, socket descriptors can be sent in also.
        //   timeoutInMilliseconds - Timeout in ms (0 = INFINITE timeout)
        //
        // Returns:
        //   If successful then the return value will return the index to the handle that has changed state.
        //   ACS_CS_EVENT_RC_ERROR   - Returned if some fault has occurred
        //   ACS_CS_EVENT_RC_TIMEOUT - Returned if nothing has changed state in the given time slot
        //
        static int WaitForEvents(int count, ACS_CS_EventHandle *eventHandles, int timeoutInMilliseconds);

        //
        // Flag event as set
        //
        static bool SetEvent(ACS_CS_EventHandle eventHandle);

        //
        // Resets the event
        //
        static bool ResetEvent(ACS_CS_EventHandle eventHandle);

        friend class ACE_Singleton<ACS_CS_Event, ACE_Recursive_Thread_Mutex>;

    protected:
        ACS_CS_Event();
        static ACS_CS_Event* instance();

    private:
        typedef struct {
            ACS_CS_EventHandle handle;
            bool manualReset;
        } EventProperties_t;

        std::vector<EventProperties_t*> events_;
        std::map<string, EventProperties_t*> namedEvents_;

        typedef std::pair<string, ACS_CS_Event::EventProperties_t*> NamedEventKeyTypePair;
        typedef std::vector<ACS_CS_Event::EventProperties_t*>::iterator EventIteratorType;
        typedef std::map<string, ACS_CS_Event::EventProperties_t*>::iterator NamedEventIteratorType;

        typedef ACE_Singleton<ACS_CS_Event, ACE_Recursive_Thread_Mutex> instance_;


        // Private methods
        NamedEventIteratorType findNamedEventByHandle(ACS_CS_EventHandle handle);
        EventIteratorType findEventByHandle(ACS_CS_EventHandle handle);


        // Private variables
        ACE_RW_Mutex lock_;
};

#endif // ACS_CS_Event_h

