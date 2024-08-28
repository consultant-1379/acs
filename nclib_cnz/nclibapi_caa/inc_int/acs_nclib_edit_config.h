/** @file acs_nclib_edit_config.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-12
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
 *	| R-001 | 2012-10-12 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#ifndef ACS_NCLIB_EDIT_CONFIG_H_
#define ACS_NCLIB_EDIT_CONFIG_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_rpc_impl.h"
#include "acs_nclib_filter.h"


class acs_nclib_edit_config: public acs_nclib_rpc_impl {
public:
    acs_nclib_edit_config(acs_nclib::Datastore = acs_nclib::DATASTORE_UNKNOWN, acs_nclib::DefaultOperation = acs_nclib::DEF_OP_UNDEFINED,
                       acs_nclib::TestOption = acs_nclib::TEST_OP_UNDEFINED, acs_nclib::ErrorOption = acs_nclib::ERR_OP_UNDEFINED);
    acs_nclib_edit_config(const acs_nclib_edit_config&);
    virtual ~acs_nclib_edit_config();
    
    virtual void set_config(const char*);
    
    virtual void set_target(acs_nclib::Datastore);
    
    virtual void set_default_operation(acs_nclib::DefaultOperation);
    virtual void set_test_option(acs_nclib::TestOption);
    virtual void set_error_option(acs_nclib::ErrorOption);

    acs_nclib_edit_config& operator=(const acs_nclib_edit_config&);
    
protected:
    virtual char* dump_operation() const;
    virtual void dump_operation(std::string&) const;
    
private:
    acs_nclib::Datastore datastore;
    acs_nclib::DefaultOperation default_operation;
    acs_nclib::TestOption test_option;
    acs_nclib::ErrorOption error_option;
    char* configuration;
    unsigned long config_size;
};

#endif /* defined(ACS_NCLIB_EDIT_CONFIG_H_) */
