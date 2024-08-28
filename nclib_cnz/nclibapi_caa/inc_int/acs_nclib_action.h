/** @file acs_nclib_action.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-12-05
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
 *	| R-001 | 2012-12-05 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_ACTION_H_
#define ACS_NCLIB_ACTION_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_rpc_impl.h"

class acs_nclib_action: public acs_nclib_rpc_impl {
public:
	acs_nclib_action();
	virtual ~acs_nclib_action();

protected:
    virtual char* dump_operation() const;
    virtual void dump_operation(std::string&) const;

    virtual void set_data(const char*);
    virtual void set_action_namespace(const char*);

private:
    char * _namespace;
    char * _data;

};

#endif /* ACS_NCLIB_ACTION_H_ */
