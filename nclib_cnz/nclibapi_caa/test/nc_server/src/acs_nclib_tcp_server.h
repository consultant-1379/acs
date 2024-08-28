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

#ifndef acs_nclib_tcp_server_h
#define acs_nclib_tcp_server_h 1

#include <string>
#include <vector>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"

using std::string;

class acs_nclib_tcp_client;

class acs_nclib_tcp_server
{
    public:
        acs_nclib_tcp_server();
        ~acs_nclib_tcp_server();

        void addTcpServer(uint32_t addr, uint16_t port);
        bool getActiveServerHandles(int* handles, size_t &size);
        void addActiveServerHandlesToVector(std::vector<int> &v);
        acs_nclib_tcp_client* acceptConnectionForHandle(int handle);
        bool isServerHandle(int handle);
        size_t getActiveHandles();
        unsigned int startListen();
        void stopListen();
        uint32_t getActiveServerAddress(int handle);

    private:
        typedef struct {
            uint32_t addr;
            uint16_t port;
        } acs_nclib_server_info_t;

        typedef struct {
            ACE_INET_Addr addr;
            ACE_SOCK_Acceptor server;
            acs_nclib_server_info_t *serverInfo;
        } acs_nclib_active_server_t;

        std::vector<acs_nclib_server_info_t*> requestedServers_; // listen to these host and port information
        std::vector<acs_nclib_active_server_t*> activeServers_; // these are the actual running servers

        typedef std::vector<acs_nclib_server_info_t*>::iterator acs_nclib_server_info_iterator;
        typedef std::vector<acs_nclib_active_server_t*>::iterator acs_nclib_active_servers_iterator;

        acs_nclib_active_server_t* findServerByHandle(int handle);
};

#endif // acs_nclib_tcp_server_h
