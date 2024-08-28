/** @file acs_nclib_builder.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-11
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
 *	| R-001 | 2012-10-11 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_BUILDER_H_
#define ACS_NCLIB_BUILDER_H_

#include "acs_nclib_factory.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


typedef boost::property_tree::ptree acs_nclib_tree;

class acs_nclib_builder{
public:
	acs_nclib_builder();
	virtual ~acs_nclib_builder();

	static acs_nclib_rpc_reply* build_rpc_reply(const char*);
	static acs_nclib_rpc* build_rpc(const char*);
    static acs_nclib_hello* build_hello(const char*);
    
    static acs_nclib_message* build_message(const char*);
    static acs_nclib_udp* build_udp_message(const char*);
    static acs_nclib_udp_reply* build_udp_reply_message(const char*);

private:
    static acs_nclib_rpc_reply* build_rpc_reply(acs_nclib_tree&);
	static acs_nclib_rpc* build_rpc(acs_nclib_tree&);
    static acs_nclib_hello* build_hello(acs_nclib_tree&);

};

#endif /* ACS_NCLIB_BUILDER_H_ */
