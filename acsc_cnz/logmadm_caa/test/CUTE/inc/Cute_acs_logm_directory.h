#ifndef CUTE_ACS_LOGM_DIRECTORY_H_
#define CUTE_ACS_LOGM_DIRECTORY_H_

#include <acs_logm_directory.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

class cute_acs_logm_directory
{

public:
	cute_acs_logm_directory();
	static void cleanup_unittest();
	static bool ExecuteCommand(string cmd);
	static cute::suite make_suite();
	~cute_acs_logm_directory();
	static void addDaysToCurrentDate(int aNumberDays);

};


#endif /* CUTE_ACS_LOGM_LOGFILE_H_ */
