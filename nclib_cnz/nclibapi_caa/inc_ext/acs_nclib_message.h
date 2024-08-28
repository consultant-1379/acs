/** @file acs_nclib_message.h
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
 *	| R-002 | 2012-11-13 | estevol      | Added Candidate Datastore.	      |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_MESSAGE_H_
#define ACS_NCLIB_MESSAGE_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "acs_nclib_errorinfoprovider.h"

namespace acs_nclib {
	enum MsgType {
		UNKNOWN_MSG     = -1,
		RPC_MSG         =  0,
		RPC_REPLY_MSG   =  1,
		HELLO_MSG       =  2,
		UDP_MSG         =  3,
		UDP_REPLY_MSG   =  4
	};

	enum Datastore {
		DATASTORE_UNKNOWN 	=-1,
		DATASTORE_RUNNING 	= 0,
		DATASTORE_CANDIDATE = 2
	};
}

class acs_nclib_message: public acs_nclib_errorinfoprovider {

friend std::ostream& operator<<(std::ostream& output, const acs_nclib_message* msg);

public:
    virtual ~acs_nclib_message();
    
    //Return the type of message.
    acs_nclib::MsgType get_msg_type() const;
    
    //Return message ID of rpc or rpc-reply message
    unsigned int get_message_id() const;

    //Set the message ID
    void set_message_id(unsigned int);
    
    //Dump the message to an xml formatted const char*.
    //Return value must be freed by the caller using delete[]
    virtual const char* dump() const = 0;

    //Dump the message to an xml formatted std::string
    virtual void dump(std::string&) const = 0;


protected:
    acs_nclib_message(acs_nclib::MsgType = acs_nclib::UNKNOWN_MSG, unsigned int = 0);
    
private:
    //Private class members
    acs_nclib::MsgType type;
    unsigned int messageId;
};

#endif /* defined(ACS_NCLIB_MESSAGE_H_) */
