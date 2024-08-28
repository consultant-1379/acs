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

#ifndef ACS_CS_TCPServer_h
#define ACS_CS_TCPServer_h 1

#include <string>
#include <vector>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"

using std::string;

class ACS_CS_TCPClient;

class ACS_CS_TCPServer
{
    public:
        ACS_CS_TCPServer();
        ~ACS_CS_TCPServer();

        void addTcpServer(uint32_t addr, uint16_t port);
        bool getActiveServerHandles(int* handles, size_t &size);
        void addActiveServerHandlesToVector(std::vector<int> &v);
        ACS_CS_TCPClient* acceptConnectionForHandle(int handle);
        bool isServerHandle(int handle);
        size_t getActiveHandles();
        unsigned int startListen();
        void stopListen();
        uint32_t getActiveServerAddress(int handle);

    private:
        typedef struct {
            uint32_t addr;
            uint16_t port;
        } ACS_CS_ServerInfo_t;

        typedef struct {
            ACE_INET_Addr addr;
            ACE_SOCK_Acceptor server;
            ACS_CS_ServerInfo_t *serverInfo;
        } ACS_CS_ActiveServer_t;

        std::vector<ACS_CS_ServerInfo_t*> requestedServers_; // listen to these host and port information
        std::vector<ACS_CS_ActiveServer_t*> activeServers_; // these are the actual running servers

        typedef std::vector<ACS_CS_ServerInfo_t*>::iterator ACS_CS_ServerInfoIterator;
        typedef std::vector<ACS_CS_ActiveServer_t*>::iterator ACS_CS_ActiveServersIterator;

        ACS_CS_ActiveServer_t* findServerByHandle(int handle);
};

#endif // ACS_CS_TCPServer_h
