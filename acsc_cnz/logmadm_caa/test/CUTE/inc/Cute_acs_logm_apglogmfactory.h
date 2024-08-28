/*
 * Cute_acs_logm_apglogmFactory.h
 *
 *  Created on: Aug 17, 2012
 *      Author: tcskrpr1
 */

#ifndef CUTE_ACS_LOGM_APGLOGMFACTORY_H_
#define CUTE_ACS_LOGM_APGLOGMFACTORY_H_


#include <acs_logm_apglogmFactory.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

class cute_acs_logm_apglogmfactory
{

public:
	cute_acs_logm_apglogmfactory();
	static void createComponents_unittest();
	static bool ExecuteCommand(string cmd);
	static cute::suite make_suite();
	~cute_acs_logm_apglogmfactory();

};



#endif /* CUTE_ACS_LOGM_APGLOGMFACTORY_H_ */
