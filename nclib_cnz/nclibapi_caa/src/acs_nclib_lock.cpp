/*
 * acs_nclib_lock.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: eiolbel
 */

#include "acs_nclib_lock.h"
#include "acs_nclib_utils.h"

acs_nclib_lock::acs_nclib_lock(acs_nclib::Datastore ds):
acs_nclib_rpc_impl(acs_nclib::OP_LOCK),
datastore(ds)
{


}

acs_nclib_lock::~acs_nclib_lock() {

}

void acs_nclib_lock::set_target(acs_nclib::Datastore dat)
{
	datastore = dat;
}

char* acs_nclib_lock::dump_operation() const
{

	unsigned long dump_size = 512;
	char* return_value = new char[dump_size];
	memset(return_value, 0, dump_size);

	strcpy(return_value, ACS_NCLIB_LOCK_TAG::HEADER);

	switch (datastore) {
	case acs_nclib::DATASTORE_RUNNING:
		strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER);
		strcat(return_value, ACS_NCLIB_DATASTORE_TAG::RUNNING);
		strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER);
		break;

	case acs_nclib::DATASTORE_CANDIDATE:
		strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER);
		strcat(return_value, ACS_NCLIB_DATASTORE_TAG::CANDIDATE);
		strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER);
		break;

	case acs_nclib::DATASTORE_UNKNOWN:
	default:
		break;
	}

	strcat(return_value, ACS_NCLIB_LOCK_TAG::TRAILER);

	return return_value;

}

void acs_nclib_lock::dump_operation(std::string& return_value) const {

	return_value = ACS_NCLIB_LOCK_TAG::HEADER;


	switch (datastore) {
	case acs_nclib::DATASTORE_RUNNING:
		return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER;
		return_value += ACS_NCLIB_DATASTORE_TAG::RUNNING;
		return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER;
		break;
	case acs_nclib::DATASTORE_CANDIDATE:
		return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER;
		return_value += ACS_NCLIB_DATASTORE_TAG::CANDIDATE;
		return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER;
		break;

	case acs_nclib::DATASTORE_UNKNOWN:
	default:
		break;
	}

	return_value += ACS_NCLIB_LOCK_TAG::TRAILER;

}
