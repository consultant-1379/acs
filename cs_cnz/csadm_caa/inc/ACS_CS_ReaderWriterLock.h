// ACS_CS_ReaderWriterLock.h
// Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_ReaderWriterLock_h
#define ACS_CS_ReaderWriterLock_h 1

#include "ace/RW_Mutex.h"


/**
 * ACS_CS_ReaderWriterLock solves the multiple readers/multiple writers problem splitting
 * this processes in two sets that take turn in usage of the resource.
 * The bonds respected by this class are the following:
 * 1) multiple readers can access at the same time to the data structure
 * 2) multiple writers can access exclusively to the data structure
 * 3) there is mutual exclusion between readers and writers
 */
class ACS_CS_ReaderWriterLock
{
    public:
        ACS_CS_ReaderWriterLock();
        ~ACS_CS_ReaderWriterLock(void);

        void start_reading();
        void stop_reading();
        void start_writing();
        void stop_writing();

    private:
        ACE_RW_Mutex lock_;
};

#endif
