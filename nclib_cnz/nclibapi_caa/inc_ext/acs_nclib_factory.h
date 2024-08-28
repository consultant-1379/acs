/** @file acs_nclib_factory.h
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

#ifndef ACS_NCLIB_API_H_
#define ACS_NCLIB_API_H_

#include "acs_nclib_rpc.h"
#include "acs_nclib_rpc_reply.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_udp.h"
#include "acs_nclib_udp_reply.h"

class acs_nclib_factory {
public:
	acs_nclib_factory();
	virtual ~acs_nclib_factory();

	static acs_nclib_rpc* create_rpc(acs_nclib::Operation);
	static acs_nclib_rpc* create_rpc(acs_nclib::Operation, acs_nclib::Datastore);

	static acs_nclib_filter* create_filter(acs_nclib::FilterType = acs_nclib::FILTER_UNKNOWN);
	static void dereference(acs_nclib_filter*);

	static acs_nclib_rpc_reply* create_rpc_reply(acs_nclib::ReplyType = acs_nclib::REPLY_UNKNOWN);

	static acs_nclib_rpc_error* create_rpc_error(acs_nclib::ErrorType = acs_nclib::NC_ERR_UNKNOWN, acs_nclib::ErrorSeverity = acs_nclib::SEVERITY_ERROR);
	static void dereference(acs_nclib_rpc_error*);
	static void dereference(std::vector<acs_nclib_rpc_error*>);

	static acs_nclib_udp* create_udp(acs_nclib::OperationUDP);
	static acs_nclib_udp_reply* create_udp_reply(acs_nclib::OperationUDP);
    
    static acs_nclib_hello* create_hello();
    
    static void dereference(acs_nclib_message*);
};

#endif /* ACS_NCLIB_API_H_ */
