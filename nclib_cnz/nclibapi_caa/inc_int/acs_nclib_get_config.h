/** @file acs_nclib_get_config.h
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
 */

#ifndef ACS_NCLIB_GET_CONFIG_H_
#define ACS_NCLIB_GET_CONFIG_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_rpc_impl.h"
#include "acs_nclib_filter.h"


class acs_nclib_get_config: public acs_nclib_rpc_impl {
public:
    acs_nclib_get_config(acs_nclib::Datastore = acs_nclib::DATASTORE_UNKNOWN);
    virtual ~acs_nclib_get_config();
    
    virtual void add_filter(acs_nclib_filter*);
    virtual void clear_filters();

    virtual void set_source(acs_nclib::Datastore);

protected:
    virtual char* dump_operation() const;
    virtual void dump_operation(std::string&) const;

private:
    acs_nclib::Datastore datastore;
    std::vector<acs_nclib_filter*> filters;
};

#endif /* defined(ACS_NCLIB_GET_CONFIG_H_) */
