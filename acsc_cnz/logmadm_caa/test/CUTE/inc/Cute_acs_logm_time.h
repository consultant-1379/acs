
#ifndef CUTE_ACS_LOGM_TIME_H_
#define CUTE_ACS_LOGM_TIME_H_

#include<acs_logm_time.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

class cute_acs_logm_time
{

public:
	cute_acs_logm_time();
	static void isTimeElapsed_unittest();
	static void isTimeInFuture_unittest();
	static void elapsedTimeWithCurrentTime_unittest();
	static void elapsedTime_unittest();
	static void operator_less_than_unit_test();
	static void operator_greater_than_unit_test();
	static void difftime_unit_test();
	static cute::suite make_suite();
	~cute_acs_logm_time();

};


#endif /* CUTE_ACS_LOGM_TIME_H_ */
