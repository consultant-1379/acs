#include<acs_logm_cleanupAction.h>


acs_logm_cleanupAction::acs_logm_cleanupAction()
:theEndEvent(0)
{
	DEBUG("%s","Entering acs_logm_cleanupAction::acs_logm_cleanupAction");
	theEndEvent = new ACE_Event(true,false,USYNC_THREAD,"EV_ACS_LOGM_TERMINATE_EVENT");
	DEBUG("%s","Leaving acs_logm_cleanupAction::acs_logm_cleanupAction");
}
acs_logm_cleanupAction::~acs_logm_cleanupAction()
{
	DEBUG("%s","Entering acs_logm_cleanupAction::~acs_logm_cleanupAction");
	if( theEndEvent != 0)
	{
		delete theEndEvent;
	}
	DEBUG("%s","Leaving acs_logm_cleanupAction::~acs_logm_cleanupAction");

}
void acs_logm_cleanupAction::getHouseKeepingComponents()
{
	DEBUG("Entering %s","acs_logm_cleanupAction::getHouseKeepingComponents()");
	acs_logm_abstractFactory * myBaseFactoryPtr = new acs_logm_apglogmFactory();
	myBaseFactoryPtr->createComponents(theLogmHousekeepingPointerList);
	delete myBaseFactoryPtr;
	DEBUG("Leaving %s","acs_logm_cleanupAction::getHouseKeepingComponents()");

}
void acs_logm_cleanupAction::clearHouseKeepingComponents()
{
	DEBUG("Entering %s","acs_logm_cleanupAction::clearHouseKeepingComponents()");
	std::list<acs_logm_housekeeping*>::iterator myLogIter = theLogmHousekeepingPointerList.begin();
	std::list<acs_logm_housekeeping*>::iterator myLogIterEnd = theLogmHousekeepingPointerList.end();
	for(;myLogIter != myLogIterEnd;)
	{
		delete (*myLogIter);
		theLogmHousekeepingPointerList.erase(myLogIter++);

	}
	DEBUG("Leaving %s","acs_logm_cleanupAction::clearHouseKeepingComponents()");
}

void acs_logm_cleanupAction::cleanUpAction(bool aStartUpFlag)
{
	DEBUG("%s","Entering cleanUpAction");
	std::list<acs_logm_housekeeping*>::iterator myLogIter = theLogmHousekeepingPointerList.begin();
	std::list<acs_logm_housekeeping*>::iterator myLogIterEnd = theLogmHousekeepingPointerList.end();
	for(;myLogIter != myLogIterEnd;++myLogIter)
	{
		DEBUG("Starting Cleanup Action analysis on %s",(*myLogIter)->getLogName().c_str());
		if((*myLogIter)->isTimeForCleanUpElapsed() || aStartUpFlag)
		{
			(*myLogIter)->houseKeepingAction();
		}
		DEBUG("Ending Cleanup Action analysis on %s",(*myLogIter)->getLogName().c_str());
	}
	DEBUG("%s","Leaving cleanUpAction");
}

void acs_logm_cleanupAction::startCleanUpAction()
{
	DEBUG("%s","Entering acs_logm_cleanupAction::startCleanUpAction");
	cleanUpAction(true);
	acs_logm_time myNextCleanupTime;
	myNextCleanupTime.addMinutes(LOGM_CLEANUP_ACTION);
	acs_logm_time myCurrentTime;
	double myTimeToWait = myNextCleanupTime.diffTime(myCurrentTime);
	ACE_Time_Value oTimeValue(myTimeToWait);
	while( theEndEvent->wait(&oTimeValue,0))
	{
		DEBUG("%s","---Cleanup activity started-----");
		cleanUpAction(false);
		DEBUG("%s","---Cleanup activity stopped-----");

	}
	DEBUG("%s","Leaving acs_logm_cleanupAction::startCleanUpAction");
}
void acs_logm_cleanupAction::start()
{
	DEBUG("acs_logm_cleanupAction::start() %s","Entering");
	activate();
	DEBUG("acs_logm_cleanupAction::start() %s","Leaving");
}



int acs_logm_cleanupAction::svc()
{
	DEBUG("Entering %s","acs_logm_cleanupAction::svc()");
	getHouseKeepingComponents();
	startCleanUpAction();
	clearHouseKeepingComponents();
	DEBUG("Leaving %s","acs_logm_cleanupAction::svc()");
	return 0;

}
void acs_logm_cleanupAction::stop()
{
	DEBUG("Entering %s","acs_logm_cleanupAction::stop()");
	theEndEvent->pulse();
	DEBUG("Leaving %s","acs_logm_cleanupAction::stop()");
}


