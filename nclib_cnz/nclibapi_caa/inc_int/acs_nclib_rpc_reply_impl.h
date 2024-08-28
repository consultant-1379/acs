/** @file acs_nclib_rpc_reply_impl.h
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

#ifndef ACS_NCLIB_RPC_REPLY_IMPL_H_
#define ACS_NCLIB_RPC_REPLY_IMPL_H_
#include "acs_nclib_rpc_reply.h"

class acs_nclib_rpc_reply_impl: public acs_nclib_rpc_reply{
public:
	acs_nclib_rpc_reply_impl(acs_nclib::ReplyType = acs_nclib::REPLY_UNKNOWN, unsigned int = 0);
	virtual ~acs_nclib_rpc_reply_impl();

	virtual acs_nclib::ReplyType get_reply_type() const;

	virtual void add_rpc_error(acs_nclib_rpc_error*);
	virtual void clear_rpc_errors();

	virtual void set_data(const char*);
	virtual void get_data(std::string&) const;
    virtual const char* get_data() const;

	virtual void get_error_list(std::vector<acs_nclib_rpc_error*>&);
	virtual void set_reply_type(acs_nclib::ReplyType);

	virtual const char* dump() const;
	virtual void dump(std::string&) const;

private:
	acs_nclib::ReplyType reply_type;
	std::vector<acs_nclib_rpc_error*> errors;
    char* data;
    unsigned long data_size;
};

#endif /* ACS_NCLIB_RPC_REPLY_IMPL_H_ */
