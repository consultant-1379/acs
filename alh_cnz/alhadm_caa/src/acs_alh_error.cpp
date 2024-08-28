/*
 * acs_alh_error.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: efabron
 */

#include "acs_alh_error.h"

using namespace std;


//========================================================================================
//	Constructor
//========================================================================================

acs_alh_error::acs_alh_error() : errorId_(0), errorText_(ACS_ALH_noError)
{
}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_error::~acs_alh_error()
{
}


//========================================================================================
//	Set error
//========================================================================================

void acs_alh_error::setError(const int id)
{
	errorId_ = id;				// Set the errorId
}



//========================================================================================
//	Set error text
//========================================================================================

void acs_alh_error::setErrorText(const char message[])
{
	errorText_ = message;
}




//========================================================================================
//	Get error
//========================================================================================

int acs_alh_error::getError() const
{
	return errorId_;				// Get the error type
}


//========================================================================================
//	Get error text
//========================================================================================

const char* acs_alh_error::getErrorText() const
{
	return errorText_.data();	 	// Get the error text string
}


