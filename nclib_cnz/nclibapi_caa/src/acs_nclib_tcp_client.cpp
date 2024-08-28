//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#include "acs_nclib_tcp_client.h"
#include <string>

//using std::string;


acs_nclib_tcp_client::acs_nclib_tcp_client(ACE_SOCK_Stream &stream, ACE_INET_Addr &peer) :
    addr_(NULL),
    stream_(NULL)
{
    addr_ = new ACE_INET_Addr(peer);
    stream_ = new ACE_SOCK_Stream(stream.get_handle());
}


acs_nclib_tcp_client::~acs_nclib_tcp_client()
{
    delete stream_;
    delete addr_;
}


std::string acs_nclib_tcp_client::getFullAddressAsString()
{
    char buf[512];
    addr_->addr_to_string(buf, sizeof(buf), 1);

    return std::string(buf);
}


int acs_nclib_tcp_client::getHandle()
{
    return stream_->get_handle();
}


acs_nclib_tcp_client::LinkStatus_t acs_nclib_tcp_client::read(char *buf, size_t bufsize, int &bytesRead)
{
    ACE_Time_Value ts(0, 0);

    int ret = stream_->recv(buf, bufsize, 0, &ts);

    if (ret == 0) {
        // we're not connected
        bytesRead = 0;
        return acs_nclib_tcp_client::NotConnected;
    }
    else if (ret == -1 && ACE_OS::last_error() == ETIME)
    {
        // no data in given time, timeout
        bytesRead = 0;
        return acs_nclib_tcp_client::OK;
    }

    // set bytes read
    bytesRead = ret;

    return acs_nclib_tcp_client::OK;
}


acs_nclib_tcp_client::LinkStatus_t acs_nclib_tcp_client::send(char *buf, size_t bufsize)
{
    size_t foo;
    return this->send(buf, bufsize, foo);
}


acs_nclib_tcp_client::LinkStatus_t acs_nclib_tcp_client::send(char *buf, size_t bufsize, size_t & bytesSent)
{
    ssize_t len;

    if ((len = stream_->send(buf, bufsize, 0)) != static_cast<ssize_t>(bufsize)) {

        if (len == -1) {
            return acs_nclib_tcp_client::NotConnected;
        }

        bytesSent = len;
        return acs_nclib_tcp_client::Failed;
    }

    // update bytes sent
    bytesSent = len;

    return acs_nclib_tcp_client::OK;
}


void acs_nclib_tcp_client::close()
{
    stream_->close();
}
