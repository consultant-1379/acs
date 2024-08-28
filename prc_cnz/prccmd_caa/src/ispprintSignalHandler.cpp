/*
 * ispprintSignalHandler.cpp
 *
 *  Created on: Sep 29, 2010
 *      Author: xlucpet
 */

#include "ispprintSignalHandler.h"
#include "ace/Log_Msg.h"
#include <iostream>
using namespace std;

ispprintSignalHandler::ispprintSignalHandler() {
	// TODO Auto-generated constructor stub

}

ispprintSignalHandler::~ispprintSignalHandler() {
	// TODO Auto-generated destructor stub
}

int ispprintSignalHandler::handle_signal (int /*signum*/, siginfo_t *,ucontext_t *){
	//signum = 0;
	exit(1);
}
