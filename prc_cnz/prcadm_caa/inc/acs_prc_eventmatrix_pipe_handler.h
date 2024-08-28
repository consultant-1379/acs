#ifndef ACS_PRC_EVENTMATRIX_PIPE_HANDLER_H_
#define ACS_PRC_EVENTMATRIX_PIPE_HANDLER_H_

#include "ace/Task.h"
#include <sys/poll.h>
#include "ACS_TRA_Logging.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_prc_eventmatrix_pipe_handler

class __CLASS_NAME__ : public ACE_Task_Base{
public:
	__CLASS_NAME__();
	virtual ~__CLASS_NAME__();

	virtual int svc ( void );

	void stop(){ sleep = true; };

	void start(){ sleep = false; };

private:

	void parseAPEvent ( char* );
	void performAction ( std::string );
	int checkEventMatrix ( std::string, std::string, std::string& );
	int getSwUpgradeOnGoing();
	int getRestoreOnGoing();

	std::string process_name;
	std::string specific_problem;
	bool sleep;
	int counter;
	time_t event_time, time1, time2, time3;
	int isSwUpgrade_flag, isRestore_flag;
	ACS_TRA_Logging* Logging;
};

#endif /* ACS_PRC_EVENTMATRIX_PIPE_HANDLER_H_ */
