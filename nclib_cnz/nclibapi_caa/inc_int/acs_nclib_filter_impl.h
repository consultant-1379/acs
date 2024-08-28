/** @file acs_nclib_filter_impl.h
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

#ifndef ACS_NCLIB_FILTER_IMPL_H_
#define ACS_NCLIB_FILTER_IMPL_H_
#include "acs_nclib_filter.h"

class acs_nclib_filter_impl: public acs_nclib_filter {
public:
	acs_nclib_filter_impl(acs_nclib::FilterType = acs_nclib::FILTER_UNKNOWN);
	acs_nclib_filter_impl(const acs_nclib_filter& new_filter);

    virtual ~acs_nclib_filter_impl();

    virtual acs_nclib::FilterType get_filter_type() const;
    virtual void set_filter_type(acs_nclib::FilterType);

    virtual void set_filter_content(const char*);
    virtual char* get_filter_content() const;

    virtual char* dump() const;
    virtual void dump(std::string&) const;

    acs_nclib_filter_impl& operator=(const acs_nclib_filter&);

protected:

private:
    acs_nclib::FilterType filter_type;
    char* filter_content;
    unsigned long filter_size;
};

#endif /* ACS_NCLIB_FILTER_IMPL_H_ */
