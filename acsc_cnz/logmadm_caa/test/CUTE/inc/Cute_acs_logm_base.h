#ifndef CUTE_ACS_LOGM_BASE_H_
#define CUTE_ACS_LOGM_BASE_H_

#include <acs_logm_base.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

class cute_acs_logm_base
{

public:
	cute_acs_logm_base();
	static void isFileObjectDirectory_isDirectoryEmpty_unittest();
	static void deleteEmptydirectory_unittest();
	static void deletedirectory_unittest();
	static void getElapsedTimeSincelastModified_unittest();
	static void deletefile_unittest();
	static bool ExecuteCommand(string cmd);
	static cute::suite make_suite();
	~cute_acs_logm_base();

};



#endif /* CUTE_ACS_LOGM_BASE_H_ */
