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

#include "ACS_CS_TCPClient.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_TCPClient_TRACE);

using std::string;


ACS_CS_TCPClient::ACS_CS_TCPClient(ACE_SOCK_Stream &stream, ACE_INET_Addr &peer) :
    addr_(NULL),
    stream_(NULL)
{
    addr_ = new ACE_INET_Addr(peer);
    stream_ = new ACE_SOCK_Stream(stream.get_handle());
}


ACS_CS_TCPClient::~ACS_CS_TCPClient()
{
    delete stream_;
    delete addr_;
}


string ACS_CS_TCPClient::getFullAddressAsString()
{
    char buf[512];
    addr_->addr_to_string(buf, sizeof(buf), 1);

    return string(buf);
}


int ACS_CS_TCPClient::getHandle()
{
    return stream_->get_handle();
}


ACS_CS_TCPClient::LinkStatus_t ACS_CS_TCPClient::read(char *buf, size_t bufsize, int &bytesRead)
{
    ACE_Time_Value ts(0, 0);

    int ret = stream_->recv(buf, bufsize, 0, &ts);

    if (ret == 0) {
        // we're not connected
        bytesRead = 0;
        return ACS_CS_TCPClient::NotConnected;
    }
    else if (ret == -1 && ACE_OS::last_error() == ETIME)
    {
        // no data in given time, timeout
        bytesRead = 0;
        return ACS_CS_TCPClient::OK;
    }

    // set bytes read
    bytesRead = ret;

    return ACS_CS_TCPClient::OK;
}


ACS_CS_TCPClient::LinkStatus_t ACS_CS_TCPClient::send(char *buf, size_t bufsize)
{
    size_t foo;
    return this->send(buf, bufsize, foo);
}


ACS_CS_TCPClient::LinkStatus_t ACS_CS_TCPClient::send(char *buf, size_t bufsize, size_t & bytesSent)
{
    ssize_t len;

    if ((len = stream_->send(buf, bufsize, 0)) != static_cast<ssize_t>(bufsize)) {

        if (len == -1) {
            return ACS_CS_TCPClient::NotConnected;
        }

        bytesSent = len;
        return ACS_CS_TCPClient::Failed;
    }

    // update bytes sent
    bytesSent = len;

    return ACS_CS_TCPClient::OK;
}


void ACS_CS_TCPClient::close()
{
    stream_->close();
}
