/** @file acs_nclib_udp_reply.h
 *	@brief
 *	@author stefano Volpe)
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

#ifndef ACS_NCLIB_UDP_REPLY_H_
#define ACS_NCLIB_UDP_REPLY_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_message.h"
#include "acs_nclib_filter.h"

namespace acs_nclib {
        enum OperationUDP_REPLY {
                OP_UDP_REPLY_UNKNOWN          = -1,
                OP_UDP_REPLY_GET              = 0,
                OP_UDP_REPLY_REPLY            = 1
	};
}

class acs_nclib_udp_reply: public acs_nclib_message {
public:

	//destructor
	virtual ~acs_nclib_udp_reply(){};


    //dump methods to convert msg in xml format
    virtual const char* dump() const = 0;
    virtual void dump(std::string&) const = 0;
    virtual void set_data(std::string&) = 0;
    virtual void get_data(std::string&) = 0;

protected:
    acs_nclib_udp_reply():acs_nclib_message(acs_nclib::UDP_REPLY_MSG){};
};

#endif /* defined(ACS_NCLIB_UDP_REPLY_H_) */
