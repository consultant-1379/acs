/** @file acs_nclib_rpc_error_impl.h
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


#ifndef ACS_NCLIB_RPC_ERROR_IMPL_H_
#define ACS_NCLIB_RPC_ERROR_IMPL_H_
#include "acs_nclib_rpc_error.h"

class acs_nclib_rpc_error_impl: public acs_nclib_rpc_error {
public:
    acs_nclib_rpc_error_impl(acs_nclib::ErrorType = acs_nclib::NC_ERR_UNKNOWN, acs_nclib::ErrorSeverity = acs_nclib::SEVERITY_ERROR);
    acs_nclib_rpc_error_impl(const acs_nclib_rpc_error&);
    virtual ~acs_nclib_rpc_error_impl();

    virtual void set_error_type(acs_nclib::ErrorType);
    virtual void set_error_severity(acs_nclib::ErrorSeverity);
    virtual void set_error_tag(const char*);
    virtual void set_error_app_tag(const char*);
    virtual void set_error_path(const char*);
    virtual void set_error_message(const char*);
    virtual void set_error_info(const char*);

    virtual acs_nclib::ErrorType get_error_type() const;
    virtual acs_nclib::ErrorSeverity get_error_severity() const;
    virtual const char* get_error_tag() const;
    virtual const char* get_error_app_tag() const;
    virtual const char* get_error_path() const;
    virtual const char* get_error_message() const;
    virtual const char* get_error_info() const;

    virtual char* dump() const;
    virtual void dump(std::string&) const;

    acs_nclib_rpc_error_impl& operator=(const acs_nclib_rpc_error&);

private:
    acs_nclib::ErrorType error_type;
    acs_nclib::ErrorSeverity error_severity;
    char* error_tag;
    char* error_app_tag;
    char* error_path;
    char* error_message;
    char* error_info;
};

#endif /* ACS_NCLIB_RPC_ERROR_IMPL_H_ */
