//  *********************************************************
//   COPYRIGHT Ericsson 2014.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2014.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2014 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#include "acs_nclib_udp_client.h"
#include <string>
#include <iostream>

//using std::string;


acs_nclib_udp_client::acs_nclib_udp_client(ACE_SOCK_Dgram &stream, ACE_INET_Addr &peer) :
    addr_(peer),
    stream_(NULL)
{
    stream_ = new ACE_SOCK_Dgram(stream.get_handle());
}


acs_nclib_udp_client::~acs_nclib_udp_client()
{
    delete stream_;
}


std::string acs_nclib_udp_client::getFullAddressAsString()
{
    char buf[512];
    addr_.addr_to_string(buf, sizeof(buf), 1);

    return std::string(buf);
}


int acs_nclib_udp_client::getHandle()
{
    return stream_->get_handle();
}


acs_nclib_udp_client::LinkStatus_t acs_nclib_udp_client::read(char *buf, size_t bufsize, int &bytesRead)
{
    ACE_INET_Addr remote_add;

    int ret = stream_->recv(buf, bufsize, remote_add);

    if (ret == -1 )
    {
        // no data in given time, timeout
        bytesRead = 0;
        return acs_nclib_udp_client::OK;
    }

    // set bytes read
    bytesRead = ret;

    return acs_nclib_udp_client::OK;
}


acs_nclib_udp_client::LinkStatus_t acs_nclib_udp_client::send(unsigned char *buf, size_t bufsize)
{
    size_t foo;
    return this->send(buf, bufsize, foo);
}


acs_nclib_udp_client::LinkStatus_t acs_nclib_udp_client::send(unsigned char *buf, size_t bufsize, size_t & bytesSent)
{
    ssize_t len;
    if ((len = stream_->send(buf, bufsize, addr_)) ) {

        if (len == -1) {
            return acs_nclib_udp_client::NotConnected;
        }
        bytesSent = len;
        return acs_nclib_udp_client::OK;
    }

    // update bytes sent
    bytesSent = len;

    return acs_nclib_udp_client::OK;
}


void acs_nclib_udp_client::close()
{
    stream_->close();
}
