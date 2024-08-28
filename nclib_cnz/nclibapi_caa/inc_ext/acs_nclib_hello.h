/** @file acs_nclib_hello.h
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

#ifndef ACS_NCLIB_HELLO_H_
#define ACS_NCLIB_HELLO_H_
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_message.h"

class acs_nclib_hello: public acs_nclib_message {
public:

	virtual ~acs_nclib_hello(){};

	virtual const char* dump() const = 0;
	virtual void dump(std::string&) const = 0;
    
    virtual void set_session_id(int) = 0;
    virtual int get_session_id() const = 0;
    
    virtual void add_capability(const char*) = 0;
    virtual void get_capabilities(std::vector<std::string>&) const = 0;
    virtual void clear_capabilities() = 0;
    
protected:
    acs_nclib_hello():acs_nclib_message(acs_nclib::HELLO_MSG){};

};

#endif /* ACS_NCLIB_HELLO_H_ */
