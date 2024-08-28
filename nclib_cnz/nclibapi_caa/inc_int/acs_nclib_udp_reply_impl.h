/** @file acs_nclib_udp_reply_impl.h
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

#ifndef ACS_NCLIB_UDP_REPLY_IMPL_H_
#define ACS_NCLIB_UDP_REPLY_IMPL_H_

#include <iostream>
#include "acs_nclib_udp_reply.h"

class acs_nclib_udp_reply_impl: public acs_nclib_udp_reply
{
public:
	acs_nclib_udp_reply_impl();
	virtual ~acs_nclib_udp_reply_impl();
    
	virtual const char* dump() const;
	virtual void dump(std::string&) const ;
	virtual void set_data(std::string&);
	virtual void get_data(std::string&);
private:
	std::string cmd;
};

#endif /* defined(ACS_NCLIB_UDP_REPLY_IMPL_H_) */
