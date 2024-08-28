/** @file acs_nclib_rpc_reply.h
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
 *	| R-002 | 2012-10-22 | estevol      | Added new ReplyType value:		  |
 *	| 		|			 |				| REPLY_DATA_WITH_ERRORS.        				  |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_RPC_REPLY_H_
#define ACS_NCLIB_RPC_REPLY_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_message.h"
#include "acs_nclib_rpc_error.h"

namespace acs_nclib {
	enum ReplyType {
		REPLY_UNKNOWN        	=-1,
		REPLY_OK             	= 0,
		REPLY_ERROR          	= 1,
		REPLY_DATA           	= 2,
		REPLY_DATA_WITH_ERRORS	= 3
	};
}

class acs_nclib_rpc_reply: public acs_nclib_message {
public:
    virtual ~acs_nclib_rpc_reply(){};
    
    virtual acs_nclib::ReplyType get_reply_type() const = 0;
    virtual void set_reply_type(acs_nclib::ReplyType) = 0;
    
    virtual void add_rpc_error(acs_nclib_rpc_error*) = 0;
    virtual void clear_rpc_errors() = 0;
    
    virtual void set_data(const char*) = 0;
    virtual void get_data(std::string&) const = 0;
    virtual const char* get_data() const = 0;

    virtual void get_error_list(std::vector<acs_nclib_rpc_error*>&) = 0;

    virtual const char* dump() const = 0;
    virtual void dump(std::string&) const = 0;

protected:
    acs_nclib_rpc_reply(unsigned int msg_id):acs_nclib_message(acs_nclib::RPC_REPLY_MSG,msg_id){};

};

#endif /* defined(ACS_NCLIB_RPC_REPLY_H_) */
