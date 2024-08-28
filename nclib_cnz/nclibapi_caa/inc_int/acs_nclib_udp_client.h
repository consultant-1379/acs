//  *********************************************************
//   COPYRIGHT Ericsson 2010.
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

#ifndef acs_nclib_udp_client_h
#define acs_nclib_udp_client_h 1

#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"
#include "ace/SOCK_Dgram.h"


class acs_nclib_udp_client
{
    public:
        typedef enum {
            OK,
            NotConnected,
            Failed
        } LinkStatus_t;


    public:
        acs_nclib_udp_client(ACE_SOCK_Dgram &stream, ACE_INET_Addr &peer);
        ~acs_nclib_udp_client();
        std::string getFullAddressAsString();
        int getHandle();
        LinkStatus_t read(char *buf, size_t bufsize, int &bytesRead);
        LinkStatus_t send(unsigned char *buf, size_t bufsize);
        LinkStatus_t send(unsigned char *buf, size_t bufsize, size_t & bytesSent);
        void close();


    private:
        ACE_INET_Addr addr_;

        ACE_SOCK_Dgram *stream_;
};

#endif // acs_nclib_udp_client_h
