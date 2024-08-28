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

#include "acs_nclib_tcp_server.h"
#include "acs_nclib_tcp_client.h"

#include <vector>


acs_nclib_tcp_server::acs_nclib_tcp_server()
{
}


acs_nclib_tcp_server::~acs_nclib_tcp_server()
{
    if (activeServers_.size() > 0) {
        stopListen();
    }

    if (requestedServers_.size() > 0) {
        acs_nclib_server_info_iterator it = requestedServers_.begin();

        for (;it != requestedServers_.end(); it++) {
            delete (*it);
        }


        requestedServers_.clear();
    }
}


void acs_nclib_tcp_server::addTcpServer(uint32_t addr, uint16_t port)
{
    acs_nclib_server_info_t *si = new acs_nclib_server_info_t();

    si->addr = addr;
    si->port = port;

    requestedServers_.push_back(si);
}


bool acs_nclib_tcp_server::getActiveServerHandles(int* handles, size_t &size)
{
    int n = 0;
    acs_nclib_active_servers_iterator it = activeServers_.begin();

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


void acs_nclib_tcp_server::addActiveServerHandlesToVector(std::vector<int> &v)
{
    acs_nclib_active_servers_iterator it = activeServers_.begin();

    for (;it != activeServers_.end(); it++) {
        v.push_back((*it)->server.get_handle());
    }
}


acs_nclib_tcp_client* acs_nclib_tcp_server::acceptConnectionForHandle(int handle)
{
    acs_nclib_active_server_t *server = findServerByHandle(handle);

    if (server != NULL)
    {
        ACE_INET_Addr peer;
        ACE_SOCK_Stream stream;

        if (server->server.accept(stream, &peer) == -1) {
            return NULL;
        }

        acs_nclib_tcp_client *tcpClient = new acs_nclib_tcp_client(stream, peer);
        return tcpClient;
    }

    return NULL;
}


bool acs_nclib_tcp_server::isServerHandle(int handle)
{
    return findServerByHandle(handle) != NULL;
}


size_t acs_nclib_tcp_server::getActiveHandles()
{
    return activeServers_.size();
}


unsigned int acs_nclib_tcp_server::startListen()
{
    acs_nclib_server_info_iterator it = requestedServers_.begin();

    for (;it != requestedServers_.end(); it++)
    {
        char addr[INET_ADDRSTRLEN];

        acs_nclib_active_server_t *ptr  = new acs_nclib_active_server_t();
        ptr->serverInfo = (*it);

        // convert address to human readable format
        uint32_t naddr = htonl(ptr->serverInfo->addr);
        inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

        // setup the connections
        ptr->addr.set(ptr->serverInfo->port, ptr->serverInfo->addr);

        if (ptr->server.open(ptr->addr, 1, AF_INET) < 0) {


            // free memory and continue to next item
            delete ptr;
            continue;
        }

        // server is up and running, save this instance

        activeServers_.push_back(ptr);
    }

    return activeServers_.size();
}


void acs_nclib_tcp_server::stopListen()
{
    acs_nclib_active_servers_iterator it = activeServers_.begin();

    for (;it != activeServers_.end(); it++)
    {
        acs_nclib_active_server_t *server = (*it);

        // shutdown server
        server->server.close();

        // free memory
        delete server;
    }

    activeServers_.clear();
}


acs_nclib_tcp_server::acs_nclib_active_server_t* acs_nclib_tcp_server::findServerByHandle(int handle)
{
    acs_nclib_active_servers_iterator it = activeServers_.begin();

    for (;it != activeServers_.end(); it++) {
        if ((*it)->server.get_handle() == handle) {
            return (*it);
        }
    }

    return NULL;
}


uint32_t acs_nclib_tcp_server::getActiveServerAddress(int handle)
{
	acs_nclib_active_server_t * activeServer = findServerByHandle(handle);

	if (activeServer && activeServer->serverInfo)
		return activeServer->serverInfo->addr;
	else
		return 0;
}
