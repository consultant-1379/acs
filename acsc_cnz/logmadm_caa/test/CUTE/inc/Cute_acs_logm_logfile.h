#ifndef CUTE_ACS_LOGM_LOGFILE_H_
#define CUTE_ACS_LOGM_LOGFILE_H_

#include <acs_logm_logfile.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

class cute_acs_logm_logfile
{

public:
	cute_acs_logm_logfile();
	static void cleanup_unittest();
	static bool ExecuteCommand(string cmd);
	static cute::suite make_suite();
	~cute_acs_logm_logfile();
	static void addDaysToCurrentDate(int aNumberDays);

};


#endif /* CUTE_ACS_LOGM_LOGFILE_H_ */
