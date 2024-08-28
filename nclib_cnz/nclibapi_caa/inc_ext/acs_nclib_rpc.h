/** @file acs_nclib_rpc.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-06
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
 *	| R-001 | 2012-10-06 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 *	| R-002 | 2012-10-06 | estevol      | Added test-option: test-only in     |
 *	|       |			 |				| accord to RFC 6241				  |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_RPC_H_
#define ACS_NCLIB_RPC_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_message.h"
#include "acs_nclib_filter.h"

namespace acs_nclib {
	enum Operation {
		OP_UNKNOWN          = -1,
		OP_GETCONFIG        =  0,
		OP_EDITCONFIG       =  1,
		OP_COPYCONFIG       =  2,
		OP_DELETECONFIG     =  3,
		OP_LOCK             =  4,
		OP_UNLOCK           =  5,
		OP_GET              =  6,
		OP_CLOSESESSION     =  7,
		OP_KILLSESSION      =  8,
		OP_COMMIT			=  9,
		OP_ACTION			= 10

	};

	enum RPCType {
		RPC_UNKNOWN          = -1,
		RPC_DATASTORE_READ   =  0,
		RPC_DATASTORE_WRITE  =  1,
		RPC_SESSION          =  2

	};

	//Additional options for edit-config operation
	enum DefaultOperation {
		DEF_OP_UNDEFINED        =-1,
		DEF_OP_MERGE            = 0,
		DEF_OP_REPLACE          = 1,
		DEF_OP_NONE             = 2

	};

	enum TestOption {
		TEST_OP_UNDEFINED       =-1,
		TEST_OP_TEST_THEN_SET   = 0,
		TEST_OP_SET             = 1,
		TEST_OP_TEST_ONLY		= 2
	};

	enum ErrorOption {
		ERR_OP_UNDEFINED        =-1,
		STOP_ON_ERROR           = 0,
		CONTINUE_ON_ERROR       = 1,
		ROLLBACK_ON_ERROR       = 2

	};
}

class acs_nclib_rpc: public acs_nclib_message {
public:

	//destructor
	virtual ~acs_nclib_rpc(){};

	//get methods for operation and rpc type
    virtual acs_nclib::Operation get_operation() const = 0;
    virtual acs_nclib::RPCType get_rpc_type() const = 0;

    //rpc filter for get and get-config
    virtual void add_filter(acs_nclib_filter*) = 0;
    virtual void clear_filters() = 0;
    
    //rpc config for edit-config
    virtual void set_config(const char*) = 0;
    virtual void set_default_operation(acs_nclib::DefaultOperation) = 0;
    virtual void set_test_option(acs_nclib::TestOption) = 0;
    virtual void set_error_option(acs_nclib::ErrorOption) = 0;

    //rpc data for action
    virtual void set_data(const char*) = 0;
    virtual void set_action_namespace(const char*) = 0;

    //source and/or target datastore
    virtual void set_source(acs_nclib::Datastore) = 0;
    virtual void set_target(acs_nclib::Datastore) = 0;

    //dump methods to convert msg in xml format
    virtual const char* dump() const = 0;
    virtual void dump(std::string&) const = 0;

protected:
    acs_nclib_rpc(unsigned int msg_id):acs_nclib_message(acs_nclib::RPC_MSG,msg_id){};
};

#endif /* defined(ACS_NCLIB_RPC_H_) */
