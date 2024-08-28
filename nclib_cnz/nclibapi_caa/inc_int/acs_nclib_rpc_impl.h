/** @file acs_nclib_rpc_impl.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-09
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-10-09 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_RPC_IMPL_H_
#define ACS_NCLIB_RPC_IMPL_H_
#include "acs_nclib_rpc.h"

class acs_nclib_rpc_impl: public acs_nclib_rpc {
public:

	//get methods for operation and rpc type
	virtual acs_nclib::Operation get_operation() const;
	virtual acs_nclib::RPCType get_rpc_type() const;

	//rpc filter for get and get-config
	virtual void add_filter(acs_nclib_filter*){};
	virtual void clear_filters(){};

	//rpc config for edit-config
	virtual void set_config(const char*){};
    virtual void set_default_operation(acs_nclib::DefaultOperation){};
    virtual void set_test_option(acs_nclib::TestOption){};
    virtual void set_error_option(acs_nclib::ErrorOption){};

	//source and/or target datastore
	virtual void set_source(acs_nclib::Datastore){};
	virtual void set_target(acs_nclib::Datastore){};

	//rpc data for action
	virtual void set_data(const char*){};
	virtual void set_action_namespace(const char*){};

	//dump methods to convert msg in xml format
	virtual const char* dump() const;
	virtual void dump(std::string&) const;

protected:
	acs_nclib_rpc_impl(acs_nclib::Operation = acs_nclib::OP_UNKNOWN, unsigned int = 0);
	virtual ~acs_nclib_rpc_impl();

	virtual char* dump_operation() const = 0;
	virtual void dump_operation(std::string&) const = 0;

private:
    acs_nclib::Operation operation;
    acs_nclib::RPCType rpc_type;

};

#endif /* ACS_NCLIB_RPC_IMPL_H_ */
