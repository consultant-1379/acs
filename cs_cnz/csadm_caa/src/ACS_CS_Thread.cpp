//  Copyright Ericsson AB 2007. All rights reserved.


// ACS_CS_Thread
#include "ACS_CS_Thread.h"


ACS_CS_Thread::ACS_CS_Thread()
      : ID_(0),
        thHandle_(0),
        finishedEvent_(0),
        rValue_(0),
        exit_(false)
{
    finishedEvent_ = ACS_CS_Event::CreateEvent(false, false, (const char*) NULL);
}


ACS_CS_Thread::~ACS_CS_Thread()
{
	//std::cout << "Destroying thread " << this << std::endl;

    if (thHandle_) {
        // Make sure that this thread is stopped first
        this->terminate();
    }

    if (finishedEvent_) {
        ACS_CS_Event::CloseEvent(finishedEvent_);
    }

    //std::cout << "Destroyed thread " << this << std::endl;
}


bool ACS_CS_Thread::activate ()
{
    // Check if the thread is already activated
    if (thHandle_)
        return false;

    exit_ = false;

    // Create the thread
   // std::cout << "Creating new thread " << this << std::endl;

    int retval = ACE_OS::thr_create((ACE_THR_FUNC)&ACS_CS_Thread::entry, (void*)this, THR_DETACHED, &thHandle_);

    // Check if all is OK
    if (retval < 0) {
        return false;
    }

    return true;                // Everything is OK
}


void ACS_CS_Thread::deActivate ()
{
    // Set the exit_ boolean to true
    exit_ = true;
}


bool ACS_CS_Thread::wait (unsigned int timeout)
{
    ACS_CS_EventHandle handles[] = { finishedEvent_ };

    int r = ACS_CS_Event::WaitForEvents(1, handles, timeout);

    // Check if we got a signal
    return r == 0;
}


bool ACS_CS_Thread::wait (unsigned int &rv, unsigned int timeout)
{
    bool success = this->wait(timeout);

    if (success) {
        rv = rValue_;
    }

    return success;
}


bool ACS_CS_Thread::isFinished() const
{
    return thHandle_ == 0;
}


void ACS_CS_Thread::close ()
{
    if (!exit_)
    {
        deActivate();
    }

    (void) wait (100);

    if (thHandle_)
    {
        thHandle_ = 0;
    }
}


void ACS_CS_Thread::init ()
{
}


void ACS_CS_Thread::finish ()
{
}


unsigned int ACS_CS_Thread::entry (void *obj)
{
    // Cast the object pointer
    ACS_CS_Thread *thread = (ACS_CS_Thread *)obj;

    int old_type = 0;

    ACE_OS::thr_setcanceltype(THR_CANCEL_ASYNCHRONOUS, &old_type);

    // Run the methods and save the return value from exec()
    thread->init();
    thread->rValue_ = thread->exec();
    thread->finish();


    thread->cleanup();

    //std::cout << "Exiting from thread " << thread << std::endl;
    return 0;
}


void ACS_CS_Thread::cleanup()
{
	thHandle_ = 0;
	ACS_CS_Event::SetEvent(finishedEvent_);
}


void ACS_CS_Thread::terminate ()
{
	//std::cout << "Terminating thread " << this << std::endl;

    if (thHandle_)
    {
        ACE_OS::thr_cancel(thHandle_);
        thHandle_ = 0;
    }
}
