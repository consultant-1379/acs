/*
 * acs_nclib_unlock.h
 *
 *  Created on: Nov 15, 2012
 *      Author: eiolbel
 */

#ifndef ACS_NCLIB_UNLOCK_H_
#define ACS_NCLIB_UNLOCK_H_

#include "acs_nclib_rpc_impl.h"

class acs_nclib_unlock: public acs_nclib_rpc_impl {
public:
	acs_nclib_unlock(acs_nclib::Datastore = acs_nclib::DATASTORE_UNKNOWN);
	virtual ~acs_nclib_unlock();

	virtual void set_target(acs_nclib::Datastore);

protected:
	virtual char* dump_operation() const;
	virtual void dump_operation(std::string&) const;

private:
	acs_nclib::Datastore datastore;
};

#endif /* ACS_NCLIB_UNLOCK_H_ */
