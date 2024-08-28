/*
 * acs_nclib_close_session.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: estevol
 */

#include "acs_nclib_close_session.h"
#include "acs_nclib_utils.h"

acs_nclib_close_session::acs_nclib_close_session()
:acs_nclib_rpc_impl(acs_nclib::OP_CLOSESESSION){

}

acs_nclib_close_session::~acs_nclib_close_session() {
	// TODO Auto-generated destructor stub
}


char* acs_nclib_close_session::dump_operation() const
{
	unsigned long dump_size = strlen(ACS_NCLIB_CLOSE_SESSION_TAG::OPERATION) +1;
	char* dump = new char[dump_size];

	memset(dump,0,dump_size);

	strcat(dump, ACS_NCLIB_CLOSE_SESSION_TAG::OPERATION);

	return dump;

}

void acs_nclib_close_session::dump_operation(std::string& return_value) const
{
	return_value = ACS_NCLIB_CLOSE_SESSION_TAG::OPERATION;
}
