/** @file acs_nclib_commit.h
 *	@brief
 *	@author eiolbel (Iole Bellucci)
 *	@date 2012-11-14
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
 *	| R-001 | 2012-11-14 | eiolbel      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#ifndef ACS_NCLIB_COMMIT_H_
#define ACS_NCLIB_COMMIT_H_
#include "acs_nclib_rpc_impl.h"

class acs_nclib_commit: public acs_nclib_rpc_impl {
public:
	acs_nclib_commit();
	virtual ~acs_nclib_commit();

protected:
    virtual char* dump_operation() const;
    virtual void dump_operation(std::string&) const;
};

#endif /* ACS_NCLIB_COMMIT_H_ */
