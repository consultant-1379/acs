/*
 * acs_apgcc_paramhandling.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: xlucpet
 */

#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_error.h"


//static SaVersionT immVersion = { 'A', 2, 1 };

acs_apgcc_paramhandling::acs_apgcc_paramhandling() {
	// TODO Auto-generated constructor stub
	//paramhandling_Impl = new acs_apgcc_paramhandling_impl();
	errorCode = 0;
}

acs_apgcc_paramhandling::~acs_apgcc_paramhandling() {
	// TODO Auto-generated destructor stub
	//delete paramhandling_Impl;
}

int acs_apgcc_paramhandling::getInternalLastError(){

	return ((-1)*errorCode);
}

char* acs_apgcc_paramhandling::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}

void acs_apgcc_paramhandling::setInternalError( int p_errorCode ) {

	errorCode = p_errorCode;
}

