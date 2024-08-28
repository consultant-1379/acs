

#ifndef ACS_LOGM_CLEANUPACTION_H_
#define ACS_LOGM_CLEANUPACTION_H_



#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>

#include<acs_logm_time.h>
#include<acs_logm_housekeeping.h>
#include<acs_logm_abstractFactory.h>
#include<acs_logm_apglogmFactory.h>
#include<acs_logm_time.h>

#include<list>
#include<string>

#include <iostream>
#include <ace/ACE.h>

#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Task_T.h>
#include <ace/OS.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>

using namespace std;

class acs_logm_cleanupAction : public ACE_Task_Base
{
private:
	ACE_Event* theEndEvent;
	std::list<acs_logm_housekeeping*> theLogmHousekeepingPointerList;
	void cleanUpAction(bool aStartupFlag);
	void clearHouseKeepingComponents();

public:
	acs_logm_cleanupAction();
	~acs_logm_cleanupAction();
	void getHouseKeepingComponents();
	void startCleanUpAction();
	int svc();
	void start();
	void stop();
};


#endif /* ACS_LOGM_CLEANUPACTION_H_ */
