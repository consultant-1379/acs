/*
 * acs_nclib_udp_impl.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: estevol
 */

#include "acs_nclib_udp_impl.h"
#include "acs_nclib_utils.h"
#include <sstream>
#define UNUSED(expr) do { (void)(expr); } while (0)
#define MSG_ID_SIZE 4
#define CMD_SIZE 16

acs_nclib_udp_impl::acs_nclib_udp_impl():
acs_nclib_udp()
{
    
}

acs_nclib_udp_impl::~acs_nclib_udp_impl()
{
    
}



const char* acs_nclib_udp_impl::dump() const
{
    char *ret=NULL;
    char temp[ MSG_ID_SIZE + 1 ]={0};
    ret = new char[CMD_SIZE];
    memset(ret,0,CMD_SIZE);
    int id = get_message_id();
    sprintf(temp, "%04d",id);
    strcat(ret,temp);
    strcat(ret,cmd.c_str());
    return ret;
}



void acs_nclib_udp_impl::dump(std::string& ret_val) const
{
	char temp[ MSG_ID_SIZE + 1 ]={0};
	int id = get_message_id();
	sprintf(temp, "%04d",id);
	unsigned char *buff = new unsigned char[CMD_SIZE];
	memset(buff,0,CMD_SIZE);
	memcpy(buff,temp,MSG_ID_SIZE);
	memcpy((buff+MSG_ID_SIZE),cmd.c_str(),(CMD_SIZE - MSG_ID_SIZE));
	std::string temp1(reinterpret_cast<const char *> (buff),
                     CMD_SIZE);
	if(buff)
		delete [] buff;
	ret_val=temp1;
}


void acs_nclib_udp_impl::set_cmd(std::string& ret_val)
{
	cmd =ret_val;
}
