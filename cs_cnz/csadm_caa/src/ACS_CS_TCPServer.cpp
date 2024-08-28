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

#include "ACS_CS_TCPServer.h"
#include "ACS_CS_TCPClient.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_TCPServer_TRACE);

using std::vector;


ACS_CS_TCPServer::ACS_CS_TCPServer()
{
}


ACS_CS_TCPServer::~ACS_CS_TCPServer()
{
    if (activeServers_.size() > 0) {
        stopListen();
    }

    if (requestedServers_.size() > 0) {
        ACS_CS_ServerInfoIterator it = requestedServers_.begin();

        for (;it != requestedServers_.end(); it++) {
            delete (*it);
        }


        requestedServers_.clear();
    }
}


void ACS_CS_TCPServer::addTcpServer(uint32_t addr, uint16_t port)
{
    ACS_CS_ServerInfo_t *si = new ACS_CS_ServerInfo_t();

    si->addr = addr;
    si->port = port;

    requestedServers_.push_back(si);
}


bool ACS_CS_TCPServer::getActiveServerHandles(int* handles, size_t &size)
{
    int n = 0;
    ACS_CS_ActiveServersIterator it = activeServers_.begin();

    if (size < getActiveHandles()) {
        // buffer size too small
        return false;
    }

    for (;it != activeServers_.end(); it++) {
        handles[n++] = (*it)->server.get_handle();
    }

    size = n;

    return true;
}


void ACS_CS_TCPServer::addActiveServerHandlesToVector(vector<int> &v)
{
    ACS_CS_ActiveServersIterator it = activeServers_.begin();

    for (;it != activeServers_.end(); it++) {
        v.push_back((*it)->server.get_handle());
    }
}


ACS_CS_TCPClient* ACS_CS_TCPServer::acceptConnectionForHandle(int handle)
{
    ACS_CS_ActiveServer_t *server = findServerByHandle(handle);

    if (server != NULL)
    {
        ACE_INET_Addr peer;
        ACE_SOCK_Stream stream;

        if (server->server.accept(stream, &peer) == -1) {
            return NULL;
        }

        ACS_CS_TCPClient *tcpClient = new ACS_CS_TCPClient(stream, peer);
        return tcpClient;
    }

    return NULL;
}


bool ACS_CS_TCPServer::isServerHandle(int handle)
{
    return findServerByHandle(handle) != NULL;
}


size_t ACS_CS_TCPServer::getActiveHandles()
{
    return activeServers_.size();
}


unsigned int ACS_CS_TCPServer::startListen()
{
    ACS_CS_ServerInfoIterator it = requestedServers_.begin();

    for (;it != requestedServers_.end(); it++)
    {
        char addr[INET_ADDRSTRLEN];

        ACS_CS_ActiveServer_t *ptr  = new ACS_CS_ActiveServer_t();
        ptr->serverInfo = (*it);

        // convert address to human readable format
        uint32_t naddr = htonl(ptr->serverInfo->addr);
        inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

        // setup the connections
        ptr->addr.set(ptr->serverInfo->port, ptr->serverInfo->addr);

        if (ptr->server.open(ptr->addr, 1, AF_INET) < 0) {
            ACS_CS_FTRACE((ACS_CS_TCPServer_TRACE,LOG_LEVEL_ERROR,
                    "(%t) Failed to listen for connections on %s:%d", addr, ptr->serverInfo->port));

            // free memory and continue to next item
            delete ptr;
            continue;
        }

        //HS35978 - begin
        //activate the KEEPALIVE option
        int optval = 1;
        ptr->server.set_option(SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
        //HS35978 - end

        // server is up and running, save this instance
        ACS_CS_FTRACE((ACS_CS_TCPServer_TRACE,LOG_LEVEL_WARN,
                "(%t) Listen for connections on %s:%d", addr, ptr->serverInfo->port));

        activeServers_.push_back(ptr);
    }

    return activeServers_.size();
}


void ACS_CS_TCPServer::stopListen()
{
    ACS_CS_ActiveServersIterator it = activeServers_.begin();

    for (;it != activeServers_.end(); it++)
    {
        ACS_CS_ActiveServer_t *server = (*it);

        // shutdown server
        server->server.close();

        // free memory
        delete server;
    }

    activeServers_.clear();
}


ACS_CS_TCPServer::ACS_CS_ActiveServer_t* ACS_CS_TCPServer::findServerByHandle(int handle)
{
    ACS_CS_ActiveServersIterator it = activeServers_.begin();

    for (;it != activeServers_.end(); it++) {
        if ((*it)->server.get_handle() == handle) {
            return (*it);
        }
    }

    return NULL;
}


uint32_t ACS_CS_TCPServer::getActiveServerAddress(int handle)
{
	ACS_CS_ActiveServer_t * activeServer = findServerByHandle(handle);

	if (activeServer && activeServer->serverInfo)
		return activeServer->serverInfo->addr;
	else
		return 0;
}
