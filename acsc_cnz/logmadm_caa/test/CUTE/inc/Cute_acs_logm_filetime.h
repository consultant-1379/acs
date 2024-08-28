#ifndef CUTE_LOGM_FILETIME_H_
#define CUTE_LOGM_FILETIME_H_

#include<acs_logm_filetime.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

class cute_acs_logm_filetime
{

public:
	cute_acs_logm_filetime();
	static void isFileObjectExists_unittest();
	static void isDirectory_unittest();
	static void isOlder_unittest();
	static bool ExecuteCommand(string cmd);
	static cute::suite make_suite();
	~cute_acs_logm_filetime();

};


#endif /* CUTE_LOGM_FILETIME_H_ */
