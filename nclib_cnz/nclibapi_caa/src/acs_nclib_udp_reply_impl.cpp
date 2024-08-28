/*
 * acs_nclib_udp_reply_impl.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: estevol
 */

#include "acs_nclib_udp_reply_impl.h"
#include "acs_nclib_utils.h"
#include <sstream>
#define UNUSED(expr) do { (void)(expr); } while (0)

acs_nclib_udp_reply_impl::acs_nclib_udp_reply_impl():
acs_nclib_udp_reply()
{
    
}

acs_nclib_udp_reply_impl::~acs_nclib_udp_reply_impl()
{
    
}



const char* acs_nclib_udp_reply_impl::dump() const
{
    char* ret=0;
    int len = cmd.length();
    if(len > 0)
    {
	ret = new char[len + 1];
	memset(ret,0,len+1);
	memcpy(ret, cmd.c_str(), len);
    }
    return ret;
}



void acs_nclib_udp_reply_impl::dump(std::string& ret_val) const
{
	acs_nclib_udp_reply_impl *temp = const_cast<acs_nclib_udp_reply_impl*>(this);
	temp->cmd = ret_val;
}


void acs_nclib_udp_reply_impl::set_data(std::string& ret_val)
{
	cmd =ret_val;
}

void acs_nclib_udp_reply_impl::get_data(std::string& ret_val)
{
	ret_val= cmd;
}

