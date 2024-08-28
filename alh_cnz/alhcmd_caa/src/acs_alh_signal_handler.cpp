/*
 * acs_alh_signal_handler.cpp
 *
 *  Created on: Nov 30, 2011
 *      Author: xgiopap
 */

#include "acs_alh_signal_handler.h"
#include <iostream>

acs_alh_signal_handler::acs_alh_signal_handler()
{
	// TODO Auto-generated constructor stub
}

acs_alh_signal_handler::~acs_alh_signal_handler()
{
	// TODO Auto-generated destructor stub
}


int acs_alh_signal_handler::handle_signal (int signum, siginfo_t *,ucontext_t *)
{
	if(signum == SIGINT)
	{
		std::cout << std::endl;
		std::cout << "Command interrupted" << std::endl;
		exit(4);
	}

	return 0;
}
