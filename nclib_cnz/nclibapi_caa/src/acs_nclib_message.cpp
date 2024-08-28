//
//  acs_nclib_message.cpp
//  NetConf
//
//  Created by Stefano Volpe on 06/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include "acs_nclib_message.h"
#include "acs_nclib_utils.h"

acs_nclib_message::acs_nclib_message(acs_nclib::MsgType msgType, unsigned int msgId):
type(msgType),
messageId(msgId)
{
    
}

acs_nclib_message::~acs_nclib_message()
{
    
}

acs_nclib::MsgType acs_nclib_message::get_msg_type() const
{
    return type;
}

unsigned int acs_nclib_message::get_message_id() const
{
    return messageId;
}

void acs_nclib_message::set_message_id(unsigned int  msgId)
{
    messageId = msgId;
}

std::ostream& operator<<(std::ostream& output, const acs_nclib_message* msg)
{
	if (msg)
	{
		const char* temp = msg->dump();
		if (temp)
		{
			output << temp;
			delete []temp;
		}
	}

	return output;
}
