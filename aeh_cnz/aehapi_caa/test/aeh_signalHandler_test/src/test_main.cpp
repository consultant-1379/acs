/*
 * test_main.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: xfabron
 */


#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "acs_aeh_signalhandler.h"


void test(int param){
	std::cout<<"Segnale generato"<<std::endl;
}

int main(){

	acs_aeh_setSignalExceptionHandler("signalHandler_test");

	//int ret = 20/0;

	//std::cout<<"Result: "<<ret<<std::endl;

	raise(SIGABRT);

	return 0;
}
