/** @file acs_nclib_hello_impl.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-13
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
 *	| R-001 | 2012-13-09 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_HELLO_IMPL_H_
#define ACS_NCLIB_HELLO_IMPL_H_

#include <iostream>
#include "acs_nclib_hello.h"

class acs_nclib_hello_impl: public acs_nclib_hello
{
public:
	acs_nclib_hello_impl();
	virtual ~acs_nclib_hello_impl();
    
	virtual const char* dump() const;
	virtual void dump(std::string&) const;
    
    virtual void set_session_id(int);
    virtual int get_session_id() const;
    
    virtual void add_capability(const char*);
    virtual void get_capabilities(std::vector<std::string>&) const;
    virtual void clear_capabilities();
    
private:
	std::vector<std::string> capabilities;
	int sessionId;

};

#endif /* defined(ACS_NCLIB_HELLO_IMPL_H_) */
