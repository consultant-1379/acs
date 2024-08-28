/** @file acs_nclib_filter.h
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
 */

#ifndef ACS_NCLIB_FILTER_H_
#define ACS_NCLIB_FILTER_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_errorinfoprovider.h"

namespace acs_nclib{
enum FilterType {
    FILTER_UNKNOWN   =-1,
    FILTER_SUBTREE   = 0
};
}

class acs_nclib_filter: public acs_nclib_errorinfoprovider {
public:
    virtual ~acs_nclib_filter(){};
    
    virtual acs_nclib::FilterType get_filter_type() const = 0;
    virtual void set_filter_type(acs_nclib::FilterType) = 0;
    
    virtual void set_filter_content(const char*) = 0;
    virtual char* get_filter_content() const = 0;
    
    virtual char* dump() const = 0;
    virtual void dump(std::string&) const = 0;

};

#endif /* defined(ACS_NCLIB_FILTER_H_) */
