/*
 * acs_nclib_commit.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: eiolbel
 */

#include "acs_nclib_commit.h"
#include "acs_nclib_utils.h"


acs_nclib_commit::acs_nclib_commit()
	:acs_nclib_rpc_impl(acs_nclib::OP_COMMIT){

}

acs_nclib_commit::~acs_nclib_commit() {
	// TODO Auto-generated destructor stub
}



char* acs_nclib_commit::dump_operation() const
{
	unsigned long dump_size = strlen(ACS_NCLIB_COMMIT_TAG::OPERATION) +1;
	char* dump = new char[dump_size];

	memset(dump,0,dump_size);

	strcat(dump, ACS_NCLIB_COMMIT_TAG::OPERATION);

	return dump;

}

void acs_nclib_commit::dump_operation(std::string& return_value) const
{
	return_value = ACS_NCLIB_COMMIT_TAG::OPERATION;
}


















