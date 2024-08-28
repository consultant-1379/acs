/** @file acs_nclib_rpc_error.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-07
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
 *	| R-001 | 2012-10-07 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_RPC_ERROR_H_
#define ACS_NCLIB_RPC_ERROR_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_errorinfoprovider.h"

namespace acs_nclib {
	enum ErrorType {
		NC_ERR_UNKNOWN      =-1,
		NC_ERR_TRANSPORT    = 0,
		NC_ERR_RPC          = 1,
		NC_ERR_PROTOCOL     = 2,
		NC_ERR_APPLICATION  = 3
	};

	enum ErrorSeverity {
		SEVERITY_ERROR    = 0,
		SEVERITY_WARNING  = 1
	};
}

class acs_nclib_rpc_error: public acs_nclib_errorinfoprovider {
public:
    virtual ~acs_nclib_rpc_error(){};
    
    virtual void set_error_type(acs_nclib::ErrorType) = 0;
    virtual void set_error_severity(acs_nclib::ErrorSeverity) = 0;
    virtual void set_error_tag(const char*) = 0;
    virtual void set_error_app_tag(const char*) = 0;
    virtual void set_error_path(const char*) = 0;
    virtual void set_error_message(const char*) = 0;
    virtual void set_error_info(const char*) = 0;

    virtual acs_nclib::ErrorType get_error_type() const = 0;
    virtual acs_nclib::ErrorSeverity get_error_severity() const = 0;
    virtual const char* get_error_tag() const = 0;
    virtual const char* get_error_app_tag() const = 0;
    virtual const char* get_error_path() const = 0;
    virtual const char* get_error_message() const = 0;
    virtual const char* get_error_info() const = 0;

    virtual char* dump() const = 0;
    virtual void dump(std::string&) const = 0;
};
#endif /* defined(ACS_NCLIB_RPC_ERROR_H_) */
