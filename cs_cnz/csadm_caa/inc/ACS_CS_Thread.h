//  Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_Thread_h
#define ACS_CS_Thread_h 1

#include "ace/Thread.h"
#include "ACS_CS_Event.h"


class ACS_CS_Thread
{
    public:
        ACS_CS_Thread();

        virtual ~ACS_CS_Thread();

        bool activate();

        void deActivate();

        bool wait(unsigned int timeout);

        bool wait(unsigned int &rv, unsigned int timeout);

        bool isFinished() const;

        ACS_CS_EventHandle getWaithandle() const {return finishedEvent_;}

        void close();

        virtual void init();

        virtual void finish();

        virtual int exec() = 0;

        void terminate();

    protected:
        int unsigned ID_;

        ACE_thread_t thHandle_;

        ACS_CS_EventHandle finishedEvent_;

        int rValue_;

        bool exit_;

    private:
        ACS_CS_Thread(const ACS_CS_Thread &right);

        ACS_CS_Thread & operator=(const ACS_CS_Thread &right);

        static unsigned int entry(void *obj);

        void cleanup();
};

#endif
