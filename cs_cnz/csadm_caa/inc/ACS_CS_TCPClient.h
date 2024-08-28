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

#ifndef ACS_CS_TCPClient_h
#define ACS_CS_TCPClient_h 1

#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"
#include "ace/SOCK_Stream.h"


class ACS_CS_TCPClient
{
    public:
        typedef enum {
            OK,
            NotConnected,
            Failed
        } LinkStatus_t;


    public:
        ACS_CS_TCPClient(ACE_SOCK_Stream &stream, ACE_INET_Addr &peer);
        ~ACS_CS_TCPClient();
        std::string getFullAddressAsString();
        int getHandle();
        LinkStatus_t read(char *buf, size_t bufsize, int &bytesRead);
        LinkStatus_t send(char *buf, size_t bufsize);
        LinkStatus_t send(char *buf, size_t bufsize, size_t & bytesSent);
        void close();


    private:
        ACE_INET_Addr *addr_;

        ACE_SOCK_Stream *stream_;
};

#endif // ACS_CS_TCPClient_h
