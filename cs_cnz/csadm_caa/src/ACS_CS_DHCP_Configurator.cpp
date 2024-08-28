 /* @file ACS_CS_DHCP_Configurator.cpp
 * @author xminaon
 * @date 2010-09-07
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

// Include C-code for use in the C-based DHCPCTL API
extern "C" {
#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <isc/types.h>
//#include <isc-dhcp/result.h>
#include <dhcpctl/dhcpctl.h>
#include <omapip/omapip.h>
#include <string.h>
}
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "acs_apgcc_omhandler.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_DHCP_Configurator.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Protocol.h"
#include "ACS_APGCC_Util.H"
#include <set>
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_Common_Util.h"
static const std::string NFS_SERVER_ADDRESS = "169.254.208.100";

static const uint32_t INTERNAL_SUBNET	= 0xA9FED000;	// 169.254.208.0
static const std::string CLUSTERCONF_CMD = "/opt/ap/apos/bin/clusterconf/clusterconf";

ACS_CS_Trace_TDEF(ACS_CS_DHCP_Configurator_TRACE);

using std::string;
using std::cout;
using std::endl;

// Debug functions
void printStatusEx(dhcpctl_status, const char*, int);


// Debug function
#define printStatus(X) printStatusEx(X, __FILE__, __LINE__)

// Debug function
void printStatusEx(dhcpctl_status status, const char* file, int line) {

	printf("ACS_CS_DHCP_Configurator::printStatus() %s %d %s %d\n",isc_result_totext(status),status,file,line);

	ACS_CS_FTRACE((ACS_CS_DHCP_Configurator_TRACE,
				LOG_LEVEL_INFO,
				"%s:%d\n"
				"Status: <%s> <error code:%d>",
				file, line, isc_result_totext(status),
				status));
}

/** @class ACS_CS_DHCP_ConfiguratorImpl
 *	@brief Implementation class according to the "pimpl idom"
 *	@author xminaon
 *
 *	This class is used to hide implementation details and internal requirements (C-code)
 *	from clients of the ACS_CS_DHCP_Configurator class.
 */


class ACS_CS_DHCP_ConfiguratorImpl {

public:
	ACS_CS_DHCP_ConfiguratorImpl(string ip, int port);
	virtual ~ACS_CS_DHCP_ConfiguratorImpl();
	bool connect() ;
	bool disconnect() ;
	bool isConnected() const {return connected;}
	bool addClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool updateClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool updateNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool removeClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool removeNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool fetchIP(const std::string &name, std::string &ip_addr) ;
	bool fetchMAC(const std::string &name, char* mac) ;
	bool existsNormalHost(const char * mac, int size) ;
	bool existsClientHost(const char * uid, int size) ;
	bool addClientAPZ2123XDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addNormalAPZ21250DhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addScxbClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addSmxbClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addCmxbClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addEBSClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addIptNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool addIplbNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	int fetchShelfInfo(int shelf, std::string &outShelfId) const;
	bool removeDhcpReservationByHost(std::string host);
	bool removeNormalDhcpBoardByMAC(const ACS_CS_DHCP_Info * info);

	int loadDHCPFile(std::string& s);
	int getLeasesToBeDeleted(vector<string> &add);
	int cleanUpDhcpLeases();



protected:


private:
	ACS_CS_DHCP_ConfiguratorImpl(const ACS_CS_DHCP_ConfiguratorImpl &right);
	ACS_CS_DHCP_ConfiguratorImpl & operator=(const ACS_CS_DHCP_ConfiguratorImpl &right);

	bool getImmAttributeString (std::string object, std::string attribute, std::string &value) const;
	bool getImmAttributeInt(std::string object, std::string attribute, int &value) const;

	bool getNTPservers(ACS_CS_Protocol::CS_Network_Identifier eth, std::string& scx0, std::string& scx25);

//	bool getFileName(const path & directory, std::string &fileName ) const;

	bool getTimeZone(std::string&);

	int getTurboBoostSetting();
	string server_ip;			// DHCP server IP address
	int server_port;			// API port for accessing server
	bool connected;				// Connection state
	dhcpctl_handle connection;	// Connection handle
	ACE_Semaphore *m_pSem;

	boost::regex HostDeleted;
	boost::regex Host;

	int shelf_arch;
};



ACS_CS_DHCP_Configurator::ACS_CS_DHCP_Configurator(string ip, int port) {
	impl = new ACS_CS_DHCP_ConfiguratorImpl(ip, port);
}

ACS_CS_DHCP_Configurator::~ACS_CS_DHCP_Configurator() {

	try {

		if (impl)
			delete impl;
	}
	catch (...) {}
}

bool ACS_CS_DHCP_Configurator::connect()  {
	return impl->connect();
}

bool ACS_CS_DHCP_Configurator::disconnect()  {
	return impl->disconnect();
}

bool ACS_CS_DHCP_Configurator::isConnected() const {
	return impl->isConnected();
}

bool ACS_CS_DHCP_Configurator::addClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addClientDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::addNormalDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addNormalDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::updateClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->updateClientDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::updateNormalDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->updateNormalDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::removeClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->removeClientDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::removeNormalDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->removeNormalDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::removeNormalDhcpBoardByMAC(const ACS_CS_DHCP_Info * info)  {
	return impl->removeNormalDhcpBoardByMAC(info);
}

bool ACS_CS_DHCP_Configurator::fetchIp(const std::string &name, std::string &ip_addr)  {
	return impl->fetchIP(name, ip_addr);
}

bool ACS_CS_DHCP_Configurator::fetchMAC(const std::string &name, char* mac)  {
	return impl->fetchMAC(name, mac);
}

bool ACS_CS_DHCP_Configurator::existsNormalHost(const char * mac, int size)
{
	return impl->existsNormalHost(mac, size);
}

bool ACS_CS_DHCP_Configurator::existsClientHost(const char * uid, int size)
{
	return impl->existsClientHost(uid, size);
}

bool ACS_CS_DHCP_Configurator::addClientAPZ2123XDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addClientAPZ2123XDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::addNormalAPZ21250DhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addNormalAPZ21250DhcpBoard(info);
}


// Function implementations of private class

ACS_CS_DHCP_ConfiguratorImpl::ACS_CS_DHCP_ConfiguratorImpl(string ip, int port):
	server_ip(ip), server_port(port), connected(false), connection(NULL) {

	m_pSem=new ACE_Semaphore();

	ACS_CS_Registry::getNodeArchitecture(shelf_arch);

	boost::regex HostDeleted(
			"^( *host *)([0-9._AB]+)( *\\{\n *dynamic;\n *deleted;\n *\\})"
	);

	this->HostDeleted=HostDeleted;

	boost::regex Host(
			"^( *host *)([0-9._AB]+) *\\{\n *dynamic;\n *(uid|hardware)[^\\}]"
	);

	this->Host=Host;

}

ACS_CS_DHCP_ConfiguratorImpl::~ACS_CS_DHCP_ConfiguratorImpl() {

	if (connected)
		disconnect();

	if(m_pSem)
		delete m_pSem;

}

bool ACS_CS_DHCP_ConfiguratorImpl::connect() {

	dhcpctl_status status = ISC_R_SUCCESS;

	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_connect(&connection, server_ip.c_str(), server_port, 0);

		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS){
		connected = true;
	}
	else{
		ACS_CS_FTRACE((ACS_CS_DHCP_Configurator_TRACE,
				LOG_LEVEL_ERROR,
				"ACS_CS_DHCP_ConfiguratorImpl::connect()\n"
				"Error: dhcpctl_connect() returned Failure: %d", status)
		);
	}

	// return connection status
	return connected;
}



bool ACS_CS_DHCP_ConfiguratorImpl::disconnect()  {

	// Disconnect the server
	if (connection && connection->outer && connection->outer->outer) {
		dhcpctl_status status = ISC_R_SUCCESS;
		status = omapi_disconnect(connection->outer->outer, 1);
		////printStatus(status);

		if (connection) {
			(void) omapi_object_dereference(&connection, MDL);
			connection = NULL;
		}

		connected = false;
		return (status == ISC_R_SUCCESS);
	}

	connected = false;
	return false;
}


bool ACS_CS_DHCP_ConfiguratorImpl::addClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;



	int clientIdLength = info->getClientIdLength();

	if (clientIdLength == 0)
		return false;


	m_pSem->acquire();

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
		m_pSem->release();

	    return false;
	}

	char * clientId = new char[clientIdLength];
	info->getClientId(clientId, clientIdLength);

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object(&hostHandle, connection, "host");
	////printStatus(status);

	// Set hostname on object
	dhcpctl_data_string hostString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostString, 0, sizeof hostString);
		status = omapi_data_string_new(&hostString, info->getHostName().size(), MDL);
		////printStatus(status);
	}

	// Copy hostname to dhcpctl string
	if (status == ISC_R_SUCCESS) {
		memcpy(hostString->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostString, "name");
		//printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		//printStatus(status);
	}


	//////////////////////////////

	// Set MAC address
//	dhcpctl_data_string uidString = NULL;
//	if (status == ISC_R_SUCCESS) {
//		memset (&uidString, 0, sizeof uidString);
//		status = omapi_data_string_new(&uidString, clientIdLength+1, MDL);
//		printStatus(status);
//	}
//
//	if (status == ISC_R_SUCCESS)
//	{
//
//		char valore[strlen(clientId)+2];
//
//		memset(valore,0,strlen(clientId)+2);
//
//		valore[0]='\0';
//
//		int ind=0;
//
//		for(ind=0;ind<(int)strlen(clientId);ind++)
//		{
//			valore[ind+1]=(unsigned char)clientId[ind];
//		}
//
//		memcpy(uidString->value, valore, clientIdLength+1);
//
//		status = dhcpctl_set_value(hostHandle, uidString, "dhcp-client-identifier");
//		printStatus(status);
//	}
	/////////////////////////////////////


//	// Set client identifier
	dhcpctl_data_string clientIdString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&clientIdString, 0, sizeof clientIdString);
		status = omapi_data_string_new(&clientIdString, clientIdLength, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(clientIdString->value, clientId, clientIdLength);
		status = dhcpctl_set_value(hostHandle, clientIdString, "dhcp-client-identifier");
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {


		if(info->getFBN()==ACS_CS_Protocol::FBN_CPUB && info->getSystemType()==ACS_CS_Protocol::SysType_BC)
		{
			dhcpctl_data_string statementString = NULL;

			std::string ipaddress;

			char addr[INET_ADDRSTRLEN];
			uint32_t naddr = htonl(info->getAcAddress());
			inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

			ipaddress=addr;

			string statement("option server.next-server ");
			statement.append(ipaddress);
			statement.append("; ");
			statement.append("option bootfile-name ");
			statement.append("\"");
			statement.append(info->getBootPath());
			statement.append("\";");

			memset (&statementString, 0, sizeof statementString);
			status = omapi_data_string_new(&statementString, statement.size(), MDL);

			if (status == ISC_R_SUCCESS) {
				memcpy(statementString->value, statement.c_str(), statement.size());
				status = dhcpctl_set_value(hostHandle, statementString, "statements");
				//printStatus(status);
			}
		}

	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::addClientDhcpBoard()\n"
				"dhcpctl_open_object for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		//printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::addClientDhcpBoard()\n"
				"Waiting for open object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		//printStatus(waitstatus);

		ACS_CS_FTRACE((ACS_CS_DHCP_Configurator_TRACE, LOG_LEVEL_INFO,
				"[%s:%d] dhcpctl_wait_for_completion returned %d for hostName %s",
				__FUNCTION__,__LINE__, status, info->getHostName().c_str() )
		);
	}



	// Free dhcpctl string objects
	if (hostString)
		(void) dhcpctl_data_string_dereference(&hostString, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (clientIdString)
		(void) dhcpctl_data_string_dereference(&clientIdString, MDL);

	if (hostHandle)
		(void) omapi_object_dereference(&hostHandle, MDL);

	if (clientId)
		delete [] clientId;

	//Terminate connection
	disconnect();

	m_pSem->release();


	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::addNormalDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
	{
		return false;
	}

	if (info->getFBN() == ACS_CS_Protocol::FBN_IPTB)
	{
		return addIptNormalDhcpBoard(info);
	}
	else if (info->getFBN() == ACS_CS_Protocol::FBN_IPLB)
	{
		return addIplbNormalDhcpBoard(info);
	}

	m_pSem->acquire();

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	// Get mac
	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	// Create host object

	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;

	// create a local handle for "host" object on the the server and set up local data structures
	status = dhcpctl_new_object (&hostHandle, connection, "host");
	//printStatus(status);

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		//printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		//printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string macString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&macString, 0, sizeof macString);
		status = omapi_data_string_new(&macString, length, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(macString->value, mac, length);
		status = dhcpctl_set_value(hostHandle, macString, "hardware-address");
		//printStatus(status);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
					"ACS_CS_DHCP_ConfiguratorImpl::addNormalDhcpBoard()\n"
					"Set MAC address %s for %s %d hostName %s\n",mac,__FUNCTION__,__LINE__,info->getHostName().c_str())
			);
	}

	// Set hardware type
	dhcpctl_data_string typeString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&typeString, 0, sizeof typeString);
		status = omapi_data_string_new(&typeString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		unsigned char typeArr[4] = {0x0, 0x0, 0x0, 0x1};
		memcpy(typeString->value, typeArr, 4);
		status = dhcpctl_set_value(hostHandle, typeString, "hardware-type");
		//printStatus(status);
	}

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;

	std::string ipaddress;

	char addr[INET_ADDRSTRLEN];
	uint32_t naddr = htonl(info->getAcAddress());
	inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

	ipaddress=addr;


	if (status == ISC_R_SUCCESS) {
		//string statement("option tftp-server-name ");
		//statement.append("\"");
		string statement("option server.next-server ");
		//statement.append(info->getBootServer());
		statement.append(ipaddress);
		statement.append("; ");
		//statement.append("\"; ");
		    if (shelf_arch != NODE_VIRTUALIZED)
    {
      statement.append("option bootfile-name ");
      statement.append("\"");
      statement.append(info->getBootPath());
      statement.append("\";");
    }
		int turbo = getTurboBoostSetting();
		if(shelf_arch == NODE_SMX)
		{
			if(turbo == 1)
				statement.append("option environment=\"TURBO_BOOST=TRUE\";");
                        else 
			{
				if(turbo == 0)
               		                 statement.append("option environment=\"TURBO_BOOST=FALSE\";");
			}
		}

		if (shelf_arch == NODE_DMX)
		{
			// for configuring turbo boost DHCP option
              		if(turbo == 1)
				statement.append("option environment=\"env=bsp,TURBO_BOOST=TRUE\";");
			else if(turbo == 0)
				statement.append("option environment=\"env=bsp,TURBO_BOOST=FALSE\";");
			else
				statement.append("option environment=\"env=bsp\";");
		}
		memset (&statementString, 0, sizeof statementString);
		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		//printStatus(status);

		if (status == ISC_R_SUCCESS) {
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			//printStatus(status);
		}
	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		//printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		//printStatus(waitstatus);

		ACS_CS_FTRACE((ACS_CS_DHCP_Configurator_TRACE, LOG_LEVEL_INFO,
				"[%s:%d] dhcpctl_wait_for_completion returned %d for hostName %s",
				__FUNCTION__,__LINE__, status, info->getHostName().c_str() )
		);

	}

	// Free dhcpctl string objects
	if (hostName)
		(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (macString)
		(void) dhcpctl_data_string_dereference(&macString, MDL);

	if (typeString)
		(void) dhcpctl_data_string_dereference(&typeString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	if (mac)
		delete [] mac;
	//Terminate connection
	disconnect();

	m_pSem->release();

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::updateClientDhcpBoard(const ACS_CS_DHCP_Info * info)
{
	if (!info)
		return false;

	bool result = true;

	string ipAddress;

	// Lookup board based on hostname to see if it exists
	if ( ! fetchIP(info->getHostName(), ipAddress))
	{
		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
			"ACS_CS_DHCP_Configurator::updateClientDhcpBoard()\n"
			"Adding board hostname %s, address %s",
			info->getHostName().c_str(), info->getIPString().c_str()));

		// Hostname doesn't exist, add dhcp reservation board


			result = addClientDhcpBoard(info);
	}
	else
	{
		printf("ACS_CS_DHCP_Configurator::updateClientDhcpBoard() Board with hostname already exists\n");

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
			"ACS_CS_DHCP_Configurator::updateClientDhcpBoard()\n"
			"Board with hostname %s exists",
			info->getHostName().c_str()));


		if (ipAddress.compare(info->getIPString()) != 0)
		{
			ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_Configurator::updateClientDhcpBoard()\n"
				"IP addresses doesn't match, old = %s, new = %s",
				ipAddress.c_str(), info->getIPString().c_str()));

			result = false;
		}
	}

	return result;
}

bool ACS_CS_DHCP_ConfiguratorImpl::updateNormalDhcpBoard(const ACS_CS_DHCP_Info * info)
{
	if (!info)
		return false;

	bool result = true;

	string ipAddress;

	// Lookup board based on hostname to see if it exists
	if ( ! fetchIP(info->getHostName(), ipAddress))
	{
		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
			"ACS_CS_DHCP_Configurator::updateNormalDhcpBoard()\n"
			"Adding board hostname %s, address %s",
			info->getHostName().c_str(), info->getIPString().c_str()));

		// Hostname doesn't exist, add board
		result = addNormalDhcpBoard(info);
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
			"ACS_CS_DHCP_Configurator::updateNormalDhcpBoard()\n"
			"Board with hostname %s exists",
			info->getHostName().c_str()));

		if (ipAddress.compare(info->getIPString()) != 0)
		{
			ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_Configurator::updateNormalDhcpBoard()\n"
				"IP addresses doesn't match, old = %s, new = %s",
				ipAddress.c_str(), info->getIPString().c_str()));
		}

		char storedMac[6] = {0};

		// Fetch stored mac
		if ( fetchMAC(info->getHostName(), storedMac))
		{
			// Fetch new mac
			char newMac[6] = {0};
			info->getMAC(newMac, 6);

			// Compare macs
			if (memcmp(storedMac, newMac, 6) != 0)
			{
				ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
					"ACS_CS_DHCP_Configurator::updateNormalDhcpBoard()\n"
					"MAC differs, updating mac"));

				// Macs differs, remove board with stored mac
				ACS_CS_DHCP_Info temp(*info);
				temp.setMAC(storedMac, 6);
				result = removeNormalDhcpBoard(&temp);
				// and add board with the new mac

				if (result)
					result = addNormalDhcpBoard(info);
			}
		}
	}

	return result;
}

bool ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;


	m_pSem->acquire();


	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object (&hostHandle, connection, "host");
	printStatus(status);
	// Set client identifier

	dhcpctl_data_string hostName = NULL;

	if (status == ISC_R_SUCCESS) {
			memset (&hostName, 0, sizeof hostName);
			status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
			printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
			memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
			status = dhcpctl_set_value(hostHandle, hostName, "name");
			printStatus(status);
	}

	// Retrieve host object
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard()\n"
				"dhcpctl_open_object for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_open_object(hostHandle, connection, 0);
		printStatus(status);
	}

	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard()\n"
				"Waiting for open object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard()\n"
				"dhcpctl_wait_for_completion returned %d for %s %d hostName %s\n",status,__FUNCTION__,__LINE__,info->getHostName().c_str())
		);
	}

	// Remove host
	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard()\n"
				"dhcpctl_object_remove() for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_object_remove(connection, hostHandle);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard()\n"
				"Waiting for remove object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeClientDhcpBoard()\n"
				"dhcpctl_wait_for_completion returned %d for %s %d hostName %s\n",status,__FUNCTION__,__LINE__,info->getHostName().c_str())
		);
	}

	// Free dhcpctl string objects
	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (hostHandle)
			(void) omapi_object_dereference(&hostHandle, MDL);

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->acquire();

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	// Create host object

	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object(&hostHandle, connection, "host");
	//printStatus(status);

	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
			memset (&hostName, 0, sizeof hostName);
			status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
			printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
			memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
			status = dhcpctl_set_value(hostHandle, hostName, "name");
			printStatus(status);
	}

	// Retrieve host object
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_open_object for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_open_object(hostHandle, connection, 0);
		//printStatus(status);
	}

	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"Waiting for open object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		printStatus(waitstatus);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_wait_for_completion returned %d for %s %d hostName %s\n",status,__FUNCTION__,__LINE__,info->getHostName().c_str())
		);
	}

	// Remove host
	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_object_remove() for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_object_remove(connection, hostHandle);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"Waiting for remove object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion (hostHandle, &waitstatus);
		//printStatus(status);
		printStatus(waitstatus);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_wait_for_completion returned %d for %s %d hostName %s\n",status,__FUNCTION__,__LINE__,info->getHostName().c_str())
		);
	}

	// Free dhcpctl string objects

	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoardByMAC(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->acquire();

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	// Create host object

	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object(&hostHandle, connection, "host");

	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	// Set MAC address
	dhcpctl_data_string macString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&macString, 0, sizeof macString);
		status = omapi_data_string_new(&macString, length, MDL);
		//printStatus(status);
	}



	if (status == ISC_R_SUCCESS)
	{
		//dhcpctl_data_string ip_val = NULL;
		memcpy(macString->value, mac, length);
		status = dhcpctl_set_value(hostHandle, macString, "hardware-address");
		printStatus(status);
	}


	// Retrieve host object
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_open_object for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_open_object(hostHandle, connection, 0);
		//printStatus(status);
	}

	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"Waiting for open object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		printStatus(waitstatus);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_wait_for_completion returned %d for %s %d hostName %s\n",status,__FUNCTION__,__LINE__,info->getHostName().c_str())
		);
	}

	// Remove host
	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_object_remove() for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_object_remove(connection, hostHandle);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"Waiting for remove object completion for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str())
		);

		status = dhcpctl_wait_for_completion (hostHandle, &waitstatus);
		//printStatus(status);
		printStatus(waitstatus);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"ACS_CS_DHCP_ConfiguratorImpl::removeNormalDhcpBoard()\n"
				"dhcpctl_wait_for_completion returned %d for %s %d hostName %s\n",status,__FUNCTION__,__LINE__,info->getHostName().c_str())
		);
	}

	// Free dhcpctl string objects

	if (macString)
			(void) dhcpctl_data_string_dereference(&macString, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	disconnect();

	m_pSem->release();

	if (mac) delete [] mac;

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool  ACS_CS_DHCP_ConfiguratorImpl::fetchIP(const std::string &name, string &ip_addr)
{


	m_pSem->acquire();

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	bool ret=true;

	char ipval_txt[INET_ADDRSTRLEN];
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	//host object for lookup
	dhcpctl_handle host_t = NULL;
	status = dhcpctl_new_object(&host_t, connection, "host");
	//printStatus(status);
	if (status != ISC_R_SUCCESS)
	{
		ret=false;
	}

	dhcpctl_data_string host_name = NULL;

	if (ret) {

		memset(&host_name,0,sizeof host_name);
		status = omapi_data_string_new(&host_name,name.size(),MDL);
		//printStatus(status);
		if (status != ISC_R_SUCCESS) {
			ret=false;
		}
	}

	dhcpctl_data_string ip_val = NULL;


	if (ret) {

		memcpy(host_name->value,name.c_str(),name.size());
		status = dhcpctl_set_value(host_t,host_name,"name");
		//printStatus(status);
		if (status != ISC_R_SUCCESS){
			ret=false;
		}
	}

	if (ret) {
		status = dhcpctl_open_object(host_t,connection,0);
		//printStatus(status);
		if (status != ISC_R_SUCCESS){
			ret=false;
		}
	}

	if (ret) {
		status = dhcpctl_wait_for_completion(host_t,&waitstatus);
		//printStatus(status);
		printStatus(waitstatus);
		if (!(status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS))
		{
			ret=false;
		}
	}

	if (ret) {
		dhcpctl_get_value (&ip_val,host_t,"ip-address");
		memcpy(ipval_txt,ip_val->value,ip_val->len);

		ipval_txt[ip_val->len] = 0x0;

		ip_addr.assign(ipval_txt);
	}

	if (host_name)
		dhcpctl_data_string_dereference(&host_name,MDL);

	if(ip_val)
		dhcpctl_data_string_dereference(&ip_val,MDL);

	if (host_t)
		omapi_object_dereference(&host_t, MDL);

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	m_pSem->release();

	return ret;
}

bool ACS_CS_DHCP_ConfiguratorImpl::fetchMAC(const std::string &name, char* mac)
{

	if (!mac)
		return false;

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	m_pSem->acquire();

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	bool ret=true;

	//char ipval_txt[16];
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	//host object for lookup
	dhcpctl_handle host_t = NULL;
	status = dhcpctl_new_object(&host_t, connection, "host");
	//printStatus(status);
	if (status != ISC_R_SUCCESS)
	{
		ret=false;
	}

	dhcpctl_data_string host_name = NULL;
	if (ret) {

		memset(&host_name,0,sizeof host_name);
		status = omapi_data_string_new(&host_name,name.size(),MDL);
		//printStatus(status);
		if (status != ISC_R_SUCCESS) {
			ret=false;
		}
	}

	dhcpctl_data_string ip_val = NULL;
	if (ret) {

		memcpy(host_name->value,name.c_str(),name.size());
		status = dhcpctl_set_value(host_t,host_name,"name");
		//printStatus(status);
		if (status != ISC_R_SUCCESS){
			ret=false;
		}
	}

	if (ret) {
		status = dhcpctl_open_object(host_t,connection,0);
		//printStatus(status);
		if (status != ISC_R_SUCCESS){
			ret=false;
		}
	}

	if (ret) {
		status = dhcpctl_wait_for_completion(host_t,&waitstatus);
		//printStatus(status);
		printStatus(waitstatus);
		if (!(status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS))
		{
			ret=false;
		}
	}
	if (ret) {
		status = dhcpctl_get_value (&ip_val,host_t,"hardware-address");
		//printStatus(status);
		if (ip_val->len == 6) memcpy(mac, ip_val->value, 6);
		ret=ip_val->len > 0;
	}

	if(host_name)
		dhcpctl_data_string_dereference(&host_name,MDL);

	if(ip_val)
		dhcpctl_data_string_dereference(&ip_val,MDL);

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	return ret;
}

bool ACS_CS_DHCP_ConfiguratorImpl::existsClientHost(const char * uid, int size)
{
	if (!uid || !size)
		return false;



	//printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	m_pSem->acquire();

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	//host object for lookup
	dhcpctl_handle host_t = NULL;
	status = dhcpctl_new_object(&host_t, connection, "host");
	printStatus(status);

	// Set client identifier
	dhcpctl_data_string clientIdString;
	if (status == ISC_R_SUCCESS) {
		memset (&clientIdString, 0, sizeof(clientIdString));
		status = omapi_data_string_new(&clientIdString, size, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(clientIdString->value, uid, size);
		status = dhcpctl_set_value(host_t,clientIdString , "dhcp-client-identifier");
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_open_object(host_t,connection,0);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_wait_for_completion(host_t,&waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	dhcpctl_data_string_dereference(&clientIdString, MDL);

	if (host_t)
		(void) omapi_object_dereference(&host_t, MDL);

	//Terminate connection
	disconnect();

//	printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	m_pSem->release();

//	printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	// waitstatus will indicate not found if host doesn't exist
	return waitstatus == ISC_R_SUCCESS;
}

bool ACS_CS_DHCP_ConfiguratorImpl::existsNormalHost(const char * mac, int size)
{
	if (!mac || !size)
		return false;

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d\n",__FUNCTION__,__LINE__);



	m_pSem->acquire();



	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	//host object for lookup
	dhcpctl_handle host_t = NULL;
	status = dhcpctl_new_object(&host_t, connection, "host");
	printStatus(status);

	dhcpctl_data_string mac_address = NULL;

	if (status == ISC_R_SUCCESS)
	{

		memset(&mac_address, 0, sizeof(mac_address));
		status = omapi_data_string_new(&mac_address, size, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{

		//dhcpctl_data_string ip_val = NULL;
		memcpy(mac_address->value, mac, size);
		status = dhcpctl_set_value(host_t, mac_address, "hardware-address");
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_open_object(host_t,connection,0);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_wait_for_completion(host_t,&waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	dhcpctl_data_string_dereference(&mac_address, MDL);

	if (host_t)
		(void) omapi_object_dereference(&host_t, MDL);

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d\n",__FUNCTION__,__LINE__);

	// waitstatus will indicate not found if host doesn't exist
	return waitstatus == ISC_R_SUCCESS;
}



bool ACS_CS_DHCP_ConfiguratorImpl::addClientAPZ2123XDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	int clientIdLength = info->getClientIdLength();

	if (clientIdLength == 0)
		return false;

	//printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());



	m_pSem->acquire();



	//printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();

	    return false;
	}

	char * clientId = new char[clientIdLength];
	info->getClientId(clientId, clientIdLength);

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object(&hostHandle, connection, "host");
	printStatus(status);

	// Set hostname on object
	dhcpctl_data_string hostString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostString, 0, sizeof hostString);
		status = omapi_data_string_new(&hostString, info->getHostName().size(), MDL);
		printStatus(status);
	}

	// Copy hostname to dhcpctl string
	if (status == ISC_R_SUCCESS) {
		memcpy(hostString->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostString, "name");
		printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		printStatus(status);
	}

	// Set client identifier
	dhcpctl_data_string clientIdString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&clientIdString, 0, sizeof clientIdString);
		status = omapi_data_string_new(&clientIdString, clientIdLength, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(clientIdString->value, clientId, clientIdLength);
		status = dhcpctl_set_value(hostHandle, clientIdString, "dhcp-client-identifier");
		printStatus(status);
	}

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;
	if (status == ISC_R_SUCCESS) {
		string statement("option server.next-server ");
		statement.append(info->getBootServer());
		statement.append("; ");

		statement.append("option server.filename ");
		statement.append("\"");
		statement.append(info->getBootPath());
		statement.append("\";");

		memset (&statementString, 0, sizeof statementString);
		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		printStatus(status);

		if (status == ISC_R_SUCCESS) {
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			printStatus(status);
		}
	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostString)
		(void) dhcpctl_data_string_dereference(&hostString, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (clientIdString)
		(void) dhcpctl_data_string_dereference(&clientIdString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (clientId)
		delete [] clientId;

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::addNormalAPZ21250DhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	// printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());



	m_pSem->acquire();

	// printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
		m_pSem->release();

		return false;
	}


	// Get mac
	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	// Create host object

	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object (&hostHandle, connection, "host");
	printStatus(status);

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string macString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&macString, 0, sizeof macString);
		status = omapi_data_string_new(&macString, length, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(macString->value, mac, length);
		status = dhcpctl_set_value(hostHandle, macString, "hardware-address");
		printStatus(status);
	}

	// Set hardware type
	dhcpctl_data_string typeString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&typeString, 0, sizeof typeString);
		status = omapi_data_string_new(&typeString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		unsigned char typeArr[4] = {0x0, 0x0, 0x0, 0x1};
		memcpy(typeString->value, typeArr, 4);
		status = dhcpctl_set_value(hostHandle, typeString, "hardware-type");
		printStatus(status);
	}

	 std::string tftpAddress;

	 char taddr[INET_ADDRSTRLEN];
	 uint32_t naddr = htonl(info->getAcAddress());
	 inet_ntop(AF_INET, &naddr, taddr, sizeof(taddr));

	 tftpAddress = taddr;

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;
	if (status == ISC_R_SUCCESS) {
		string statement("option tftp-server-name ");
		statement.append("\"");
		statement.append(tftpAddress);
		statement.append("\"; ");
		statement.append("option server.next-server ");
		statement.append(tftpAddress);
		statement.append("; ");
		statement.append("option bootfile-name ");
		statement.append("\"");
		statement.append(info->getBootPath());
		statement.append("\";");
		statement.append("option server.filename ");
		statement.append("\"");
		statement.append(info->getBootPath());
		statement.append("\";");

		memset (&statementString, 0, sizeof statementString);
		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		printStatus(status);

		if (status == ISC_R_SUCCESS) {
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			printStatus(status);
		}
	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (macString)
		(void) dhcpctl_data_string_dereference(&macString, MDL);

	if (typeString)
		(void) dhcpctl_data_string_dereference(&typeString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	// Free allocated memory
	if (mac)
		delete [] mac;

	//Terminate connection
	disconnect();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);

}

// Function implementations of interface class


bool ACS_CS_DHCP_ConfiguratorImpl::addScxbClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->acquire();

	printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}

	dhcpctl_status status = ISC_R_SUCCESS;

	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	int lengthClientId = info->getClientIdLength();
	char * clientId = new char[lengthClientId+1];
	memset(clientId,0,lengthClientId+1);

	info->getClientId(clientId, lengthClientId);



	// Create host object


 	printf("info->getHostName()=%s\n",info->getHostName().c_str());
	printf("info->getIPString()=%s\n",info->getIPString().c_str());
	printf("info->getMagAddress()=%d\n",info->getMagAddress());
	printf("info->getClientId()=%s\n",clientId);
	printf("info->getCurrentLoadModuleVersion()=%s\n",info->getCurrentLoadModuleVersion().c_str());


	dhcpctl_handle hostHandle = NULL;
	dhcpctl_new_object (&hostHandle, connection, "host");
	printStatus(status);

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {

		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string uidString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&uidString, 0, sizeof uidString);
		status = omapi_data_string_new(&uidString, lengthClientId+1, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {

		char valore[strlen(clientId)+2];

		memset(valore,0,strlen(clientId)+2);

		valore[0]='\0';

		int ind=0;

		for(ind=0;ind<(int)strlen(clientId);ind++)
		{
			valore[ind+1]=(unsigned char)clientId[ind];
		}

		memcpy(uidString->value, valore, lengthClientId+1);

		status = dhcpctl_set_value(hostHandle, uidString, "dhcp-client-identifier");
		printStatus(status);
	}
	
	unsigned char operatingMode = 0x80; // autonomous mode
	
	char byte[4]={0};
	
	if (status == ISC_R_SUCCESS) 
	{
		unsigned plug0 = info->getMagAddress() & 0xFF;
		plug0 &= 0x0000001F;//plug0 = plug0 << 27;plug0 |= 0x00000080;
		//plug0 = plug0 >> 27;
		operatingMode |= plug0;//operatingMode = operatingMode | plug0;
		sprintf(byte,"%d",operatingMode);
	}
	
	// Set DHCP options
	dhcpctl_data_string statementString = NULL;
	if (status == ISC_R_SUCCESS) {

	    std::string ipaddress;
	    std::string TrapReceiverAddr;

	    char addr[INET_ADDRSTRLEN];
	    uint32_t naddr = htonl(info->getAcAddress());
		inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

		char TrapAddr[INET_ADDRSTRLEN];
		uint32_t taddr = htonl(info->getTrapReceiverAddress());
		inet_ntop(AF_INET, &taddr, TrapAddr, sizeof(TrapAddr));

		ipaddress=addr;
		TrapReceiverAddr=TrapAddr;

		string statement("option tftp-server-name ");
		statement.append("\"");
		statement.append(ipaddress);
		statement.append("\"; ");

		statement.append("option startup-mode ");
		statement.append("\"");
		statement.append("cc");
		statement.append("\";");

		statement.append("option startup-switch-mode ");
		statement.append("\"");
		statement.append("0000000000808080FFFFFFFF");
		statement.append("\";");

		statement.append("option shelfmgr-operating-mode ");
		statement.append(std::string(byte));
		statement.append(";");

		statement.append("option snmp-trap-receiver ");
		statement.append(TrapReceiverAddr);
		statement.append(";");

		std::string outPath = info->getLoadModuleFileName();

		std::string currtest("");

		currtest= std::string(info->getCurrentLoadModuleVersion().c_str());

		//ret=fetchCurrentLoadModuleInfo(currtest,outPackageId,outPath,"SCXB");

		statement.append("option bootfile-sw-version ");
		statement.append("\"SCX_");
		statement.append(currtest);
		statement.append("\";");

		std::string ouprodWithOutSCX = ACS_APGCC::after(outPath, "SCX");

		statement.append("option bootfile-name ");
		statement.append("\"/boot/SCX");
		statement.append(ouprodWithOutSCX);
		statement.append("\";");

		memset (&statementString, 0, sizeof statementString);

		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		printStatus(status);

		if (status == ISC_R_SUCCESS) {
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			printStatus(status);
		}

	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		printStatus(status);


	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;

	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (uidString)
		(void) dhcpctl_data_string_dereference(&uidString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		(void) omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	delete [] mac;
	delete [] clientId;

	//Terminate connection
	disconnect();

	printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::addSmxbClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->acquire();

	printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}

	dhcpctl_status status = ISC_R_SUCCESS;

	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	int lengthClientId = info->getClientIdLength();
	char * clientId = new char[lengthClientId+1];
	memset(clientId,0,lengthClientId+1);

	info->getClientId(clientId, lengthClientId);



	// Create host object


 	printf("info->getHostName()=%s\n",info->getHostName().c_str());
	printf("info->getIPString()=%s\n",info->getIPString().c_str());
	printf("info->getMagAddress()=%d\n",info->getMagAddress());
	printf("info->getClientId()=%s\n",clientId);
	printf("info->getCurrentLoadModuleVersion()=%s\n",info->getCurrentLoadModuleVersion().c_str());


	dhcpctl_handle hostHandle = NULL;
	dhcpctl_new_object (&hostHandle, connection, "host");
	printStatus(status);

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {

		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string uidString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&uidString, 0, sizeof uidString);
		status = omapi_data_string_new(&uidString, lengthClientId+1, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {

		char valore[strlen(clientId)+2];

		memset(valore,0,strlen(clientId)+2);

		valore[0]='\0';

		int ind=0;

		for(ind=0;ind<(int)strlen(clientId);ind++)
		{
			valore[ind+1]=(unsigned char)clientId[ind];
		}

		memcpy(uidString->value, valore, lengthClientId+1);

		status = dhcpctl_set_value(hostHandle, uidString, "dhcp-client-identifier");
		printStatus(status);
	}
	
	unsigned char operatingMode = 0x80; // autonomous mode
	
	char byte[4]={0};
	
	if (status == ISC_R_SUCCESS) 
	{
		unsigned plug0 = info->getMagAddress() & 0xFF;
		plug0 &= 0x0000001F;//plug0 = plug0 << 27;plug0 |= 0x00000080;
		//plug0 = plug0 >> 27;
		operatingMode |= plug0;//operatingMode = operatingMode | plug0;
		sprintf(byte,"%d",operatingMode);
	}
	
	// Set DHCP options
	dhcpctl_data_string statementString = NULL;
	if (status == ISC_R_SUCCESS) {

	    std::string ipaddress;
	    std::string TrapReceiverAddr;

	    char addr[INET_ADDRSTRLEN];
	    uint32_t naddr = htonl(info->getAcAddress());
		inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

		char TrapAddr[INET_ADDRSTRLEN];
		uint32_t taddr = htonl(info->getTrapReceiverAddress());
		inet_ntop(AF_INET, &taddr, TrapAddr, sizeof(TrapAddr));

		ipaddress=addr;
		TrapReceiverAddr=TrapAddr;

		string statement("option server.next-server ");
		statement.append(ipaddress);
		statement.append("; ");

		statement.append("option tftp-server-name ");
		statement.append("\"");
		statement.append(ipaddress);
		statement.append("\"; ");

		statement.append("option startup-mode ");
		statement.append("\"");
		statement.append("cc");
		statement.append("\";");

		statement.append("option startup-switch-mode ");
		statement.append("\"");
		statement.append("0000000000800080FFFFFFFF");
		statement.append("\";");

		statement.append("option shelfmgr-operating-mode ");
		statement.append(std::string(byte));
		statement.append(";");

		statement.append("option snmp-trap-receiver ");
		statement.append(TrapReceiverAddr);
		statement.append(";");

		std::string outPath = info->getLoadModuleFileName();

		std::string currtest("");

		currtest= std::string(info->getCurrentLoadModuleVersion().c_str());

		//ret=fetchCurrentLoadModuleInfo(currtest,outPackageId,outPath,"SMXB");

		statement.append("option bootfile-sw-version ");
		statement.append("\"SMX_");
		statement.append(currtest);
		statement.append("\";");

		std::string ouprodWithOutSMX = ACS_APGCC::after(outPath, "SMX");

		statement.append("option bootfile-name ");
		statement.append("\"/boot/SMX");
		statement.append(ouprodWithOutSMX);
		statement.append("\";");

		memset (&statementString, 0, sizeof statementString);

		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		printStatus(status);

		if (status == ISC_R_SUCCESS) {
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			printStatus(status);
		}

	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;

	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);

		ACS_CS_FTRACE((ACS_CS_DHCP_Configurator_TRACE, LOG_LEVEL_INFO,
						"[%s:%d] dhcpctl_wait_for_completion returned %d for hostName %s",
						__FUNCTION__,__LINE__, status, info->getHostName().c_str() )
				);
	}

	// Free dhcpctl string objects
	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (uidString)
		(void) dhcpctl_data_string_dereference(&uidString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		(void) omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	delete [] mac;
	delete [] clientId;

	//Terminate connection
	disconnect();

	printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::addCmxbClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {

	if (!info)
		return false;

	//Get NTP Servers
	std::string scx0, scx25;
	if (!getNTPservers(info->getNetwork(), scx0, scx25))
	{
		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE, "Error: Cannot get NTP Server addresses"));
		return false;
	}

	printf("acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->acquire();

	printf("acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}

	dhcpctl_status status = ISC_R_SUCCESS;

	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	int lengthClientId = info->getClientIdLength();
	char * clientId = new char[lengthClientId+1];
	memset(clientId,0,lengthClientId+1);

	info->getClientId(clientId, lengthClientId);

	// Create host object
 	printf("info->getHostName()=%s\n",info->getHostName().c_str());
	printf("info->getIPString()=%s\n",info->getIPString().c_str());
	printf("info->getMagAddress()=%d\n",info->getMagAddress());
	printf("info->getClientId()=%s\n",clientId);
	printf("info->getCurrentLoadModuleVersion()=%s\n",info->getCurrentLoadModuleVersion().c_str());


	dhcpctl_handle hostHandle = NULL;
	dhcpctl_new_object (&hostHandle, connection, "host");
	printStatus(status);

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string uidString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&uidString, 0, sizeof uidString);
		status = omapi_data_string_new(&uidString, lengthClientId+1, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {

		char valore[strlen(clientId)+2];

		memset(valore,0,strlen(clientId)+2);

		valore[0]='\0';

		int ind=0;

		for(ind=0;ind<(int)strlen(clientId);ind++)
		{
			valore[ind+1]=(unsigned char)clientId[ind];
		}

		memcpy(uidString->value, valore, lengthClientId+1);

		status = dhcpctl_set_value(hostHandle, uidString, "dhcp-client-identifier");
		printStatus(status);
	}

	//Option 254
	unsigned char wrapCoreConfig = 0x7; // RSTP enabled

	char byte[4]={0};
	sprintf(byte,"%d",wrapCoreConfig);

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;
	if (status == ISC_R_SUCCESS) {

	    std::string ipaddress;
	    std::string TrapReceiverAddr;

	    char addr[INET_ADDRSTRLEN];
	    uint32_t naddr = htonl(info->getAcAddress());
		inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

		char TrapAddr[INET_ADDRSTRLEN];
		uint32_t taddr = htonl(info->getTrapReceiverAddress());
		inet_ntop(AF_INET, &taddr, TrapAddr, sizeof(TrapAddr));

		ipaddress=addr;
		TrapReceiverAddr=TrapAddr;

		//Option 66
		string statement("option tftp-server-name ");
		statement.append("\"");
		statement.append(ipaddress);
		statement.append("\"; ");

		//Option 233
		statement.append("option startup-switch-mode ");
		statement.append("\"");
		statement.append("0000000000808080FFFFFFFF");
		statement.append("\";");

		//Option 225
		statement.append("option snmp-trap-receiver ");
		statement.append(TrapReceiverAddr);
		statement.append(";");

		std::string outPath = info->getLoadModuleFileName();
		
		std::string curswver = std::string(info->getCurrentLoadModuleVersion().c_str());

		//Option 232		
		statement.append("option bootfile-sw-version ");
		statement.append("\"CMX_");
		statement.append(curswver);
		statement.append("\";");
		
		
		//Option 67
		std::string ouprodWithOutCMX = ACS_APGCC::after(outPath, "CMX");
		statement.append("option bootfile-name ");
		statement.append("\"/boot/CMX");
		statement.append(ouprodWithOutCMX);
		statement.append("\";");

		//Option 254
		statement.append("option wrap-core-config ");
		statement.append(std::string(byte));
		statement.append(";");

		//Option 42
		//statement.append("option ntp-servers 192.168.169.56, 192.168.169.57;");
		statement.append("option ntp-servers ");
		if (!scx0.empty())
		{
			statement.append(scx0);
		}
		if (!scx25.empty())
		{
			if (!scx0.empty()) statement.append(", ");
			statement.append(scx25);
		}
		statement.append(";");

		memset (&statementString, 0, sizeof statementString);
		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		printStatus(status);

		if (status == ISC_R_SUCCESS) {
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			printStatus(status);
		}

	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		printStatus(status);


	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;

	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (uidString)
		(void) dhcpctl_data_string_dereference(&uidString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		(void) omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	delete [] mac;
	delete [] clientId;

	//Terminate connection
	disconnect();

	printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}


bool ACS_CS_DHCP_Configurator::addScxbDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addScxbClientDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::addSmxbDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addSmxbClientDhcpBoard(info);
}

bool ACS_CS_DHCP_Configurator::addCmxbDhcpBoard(const ACS_CS_DHCP_Info * info) {
	return impl->addCmxbClientDhcpBoard(info);
}


bool ACS_CS_DHCP_Configurator::removeDhcpReservationByHost(std::string host)
{
	return impl->removeDhcpReservationByHost(host);
}

bool ACS_CS_DHCP_ConfiguratorImpl::removeDhcpReservationByHost(std::string host)
{
 	m_pSem->acquire();

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}


	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;
	status = dhcpctl_new_object(&hostHandle, connection, "host");
	//printStatus(status);

	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
			memset (&hostName, 0, sizeof hostName);
			status = omapi_data_string_new(&hostName, host.size(), MDL);
			printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
			memcpy(hostName->value, host.c_str(), host.size());
			status = dhcpctl_set_value(hostHandle, hostName, "name");
			printStatus(status);
	}

	// Retrieve host object
	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_open_object(hostHandle, connection, 0);
		//printStatus(status);
	}

	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		printStatus(waitstatus);

	}

	// Remove host
	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {


		status = dhcpctl_object_remove(connection, hostHandle);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS) {


		status = dhcpctl_wait_for_completion (hostHandle, &waitstatus);
		//printStatus(status);
		printStatus(waitstatus);

	}

	// Free dhcpctl string objects

	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	//Terminate connection
	disconnect();


	m_pSem->release();

	//printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}



bool ACS_CS_DHCP_ConfiguratorImpl::addEBSClientDhcpBoard(const ACS_CS_DHCP_Info * info)
{
	if (!info)
		return false;

	printf("ACS_CS_DHCP_ConfiguratorImpl acquiring() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->acquire();

	printf("ACS_CS_DHCP_ConfiguratorImpl acquired() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	//Start a new connection
	if (isConnected())
		disconnect();

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}

	dhcpctl_status status = ISC_R_SUCCESS;

	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	int lengthClientId = info->getClientIdLength();
	char * clientId = new char[lengthClientId+1];
	memset(clientId,0,lengthClientId+1);

	info->getClientId(clientId, lengthClientId);

	// Create host object
 	printf("info->getHostName()=%s\n",info->getHostName().c_str());
	printf("info->getIPString()=%s\n",info->getIPString().c_str());
	printf("info->getMagAddress()=%d\n",info->getMagAddress());
	printf("info->getClientId()=%s\n",clientId);
	printf("info->getCurrentLoadModuleVersion()=%s\n",info->getCurrentLoadModuleVersion().c_str());

	dhcpctl_handle hostHandle = NULL;
	dhcpctl_new_object (&hostHandle, connection, "host");
	printStatus(status);

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string uidString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&uidString, 0, sizeof uidString);
		status = omapi_data_string_new(&uidString, lengthClientId+1, MDL);
		printStatus(status);
	}

	if (status == ISC_R_SUCCESS)
	{

		char value[strlen(clientId)+2];
		memset(value,0,strlen(clientId)+2);

		value[0]='\0';

		int ind=0;

		for(ind=0;ind<(int)strlen(clientId);ind++)
		{
			value[ind+1]=(unsigned char)clientId[ind];
		}

		memcpy(uidString->value, value, lengthClientId+1);

		status = dhcpctl_set_value(hostHandle, uidString, "dhcp-client-identifier");
		printStatus(status);
	}

	// Set DHCP options
	std::string outPath = info->getLoadModuleFileName();

	std::string currtest("");

	currtest= std::string(info->getCurrentLoadModuleVersion().c_str());

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;

	std::string ipaddress;

	char addr[INET_ADDRSTRLEN];
	uint32_t naddr = htonl(info->getAcAddress());
	inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

	ipaddress=addr;

	string statement("option server.next-server ");
	statement.append(ipaddress);
	statement.append("; ");

	std::string outprod;
	if (info->getFBN() == ACS_CS_Protocol::FBN_EPB1)
	{
		outprod = ACS_APGCC::after(outPath, "EPB1");
		statement.append("option server.filename ");
		statement.append("\"/boot/EPB1");
		statement.append(outprod);
		statement.append("\";");
	}
	else if (info->getFBN() == ACS_CS_Protocol::FBN_EvoET)
	{
		outprod = ACS_APGCC::after(outPath, "EVOET");
		statement.append("option server.filename ");
		statement.append("\"/boot/EVOET");
		statement.append(outprod);
		statement.append("\";");
	}

	printf("\nSTATEMENT\n%s\n",statement.c_str());

	memset (&statementString, 0, sizeof statementString);

	status = omapi_data_string_new(&statementString, statement.size(), MDL);
	printStatus(status);

	if (status == ISC_R_SUCCESS) {
		memcpy(statementString->value, statement.c_str(), statement.size());
		status = dhcpctl_set_value(hostHandle, statementString, "statements");
		printStatus(status);
	}

	// Create host on server
	if (status == ISC_R_SUCCESS) {
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		printStatus(status);

	}
	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;

	if (status == ISC_R_SUCCESS) {

		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		printStatus(status);
		printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostName)
			(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (uidString)
		(void) dhcpctl_data_string_dereference(&uidString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	delete [] mac;
	delete [] clientId;

	//Terminate connection
	disconnect();

	printf("ACS_CS_DHCP_ConfiguratorImpl releasing() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());

	m_pSem->release();

	printf("ACS_CS_DHCP_ConfiguratorImpl released() semaphore for %s %d hostName %s\n",__FUNCTION__,__LINE__,info->getHostName().c_str());


	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}



bool ACS_CS_DHCP_Configurator::addEBSClientDhcpBoard(const ACS_CS_DHCP_Info * info)  {
	return impl->addEBSClientDhcpBoard(info);
}


ACS_CS_DHCP_Configurator * ACS_CS_DHCP_Factory::createDhcpConfigurator(std::string ip, int port)
{
	map<DhcpHost, ACS_CS_DHCP_Configurator *>::iterator it = dhcpConfMap.find(DhcpHost(ip, port));
	ACS_CS_DHCP_Configurator *dhcpConf = 0;

	if(dhcpConfMap.end() == it)
	{
		dhcpConf = new ACS_CS_DHCP_Configurator(ip, port);
		dhcpConfMap.insert(pair<DhcpHost, ACS_CS_DHCP_Configurator *>(DhcpHost(ip, port), dhcpConf));
	} else
	{
		dhcpConf = it->second;
	}

	return dhcpConf;
}

ACS_CS_DHCP_Factory::ACS_CS_DHCP_Factory()
{
	dhcpctl_initialize();
}

ACS_CS_DHCP_Factory::~ACS_CS_DHCP_Factory()
{
	map<DhcpHost, ACS_CS_DHCP_Configurator *>::iterator it;

	for(it = dhcpConfMap.begin(); it != dhcpConfMap.end(); ++it)
	{
		delete it->second;
	}
}

ACS_CS_DHCP_Factory * ACS_CS_DHCP_Factory::instance()
{
	return instance_::instance();
}

void ACS_CS_DHCP_Factory::finalize()
{
	instance_::close();
}

int ACS_CS_DHCP_ConfiguratorImpl::loadDHCPFile(std::string& s)
{
	string line;

	s.erase();

	ifstream myfile (ACS_CS_NS::PATH_DHCP_FILE);

	if (myfile.is_open())
	{
		s.reserve(myfile.rdbuf()->in_avail());

		char c;

		while(myfile.get(c))
		{
			if(s.capacity() == s.size())
				s.reserve(s.capacity() * 3);
			s.append(1, c);
		}
	}
	else
	{

		return 1;
	}

	return 0;
}

int ACS_CS_DHCP_ConfiguratorImpl::getLeasesToBeDeleted(vector<string> &add)
{
	vector<string> del;

	std::string file_in_memory;

	if(loadDHCPFile(file_in_memory)!=0)
	{
		return 1;
	}

	std::string::const_iterator start, end;
	start = file_in_memory.begin();
	end = file_in_memory.end();

	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;

	while(regex_search(start, end, what, Host, flags))
	{
		add.push_back(what[2]);

		start = what[0].second;

		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;

	}

	flags = boost::match_default;
	start = file_in_memory.begin();
	end = file_in_memory.end();

	while(regex_search(start, end, what, HostDeleted, flags))
	{

		del.push_back(what[2]);

		start = what[0].second;

		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;

	}

	for(unsigned int i=0;i<del.size();i++)
	{
		for(unsigned int j=0;j<add.size();j++)
		{
			if(add[j].compare(del[i])==0)
			{
				add.erase(add.begin()+j);
				break;
			}
		}
	}

	return 0;
}

int ACS_CS_DHCP_Configurator::cleanUpDhcpLeases()
{
	return impl->cleanUpDhcpLeases();
}

int ACS_CS_DHCP_ConfiguratorImpl::cleanUpDhcpLeases()
{

	vector<string> add;

	getLeasesToBeDeleted(add);

	for(unsigned int j=0;j<add.size();j++)
	{
		std::cout << "remove dhcp host " << add[j] << std::endl;

		removeDhcpReservationByHost(add[j]);
	}

	return 0;

}

bool ACS_CS_DHCP_ConfiguratorImpl::getNTPservers(ACS_CS_Protocol::CS_Network_Identifier eth, std::string& scx0, std::string& scx25)
{
	bool result = false;
	//Thread safe copy of model
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
	if (!model)
	{
		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE, "[%s] Error: Cannot fetch model", __FUNCTION__));
		return false;
	}

	//Get SCX blades
	const ACS_CS_ImBlade* scx0Blade = 0;
	const ACS_CS_ImBlade* scx25Blade = 0;
	model->getApzMagazineScx(scx0Blade, scx25Blade);

	if (!scx0Blade && !scx25Blade)
	{
		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
				"[%s] Error: Cannot fetch SCX in APZ magazine. Maybe SCXs have been deleted", __FUNCTION__));

		delete model;
		return false;
	}


	//Fetch information from SCX blades
	if (scx0Blade)
	{
		if (eth == ACS_CS_Protocol::Eth_A)
		{
			scx0 = scx0Blade->ipAddressEthA;
		}
		else
		{
			scx0 = scx0Blade->ipAddressEthB;
		}
		result = true;
	}
	if (scx25Blade)
	{
		if (eth == ACS_CS_Protocol::Eth_A)
		{
			scx25 = scx25Blade->ipAddressEthA;
		}
		else
		{
			scx25 = scx25Blade->ipAddressEthB;
		}
		result = true;
	}

	delete model;
	return result;
}

bool ACS_CS_DHCP_ConfiguratorImpl::addIptNormalDhcpBoard(const ACS_CS_DHCP_Info * info)
{
	if (!info)
	{
		return false;
	}


	m_pSem->acquire();

	//Start a new connection
	if (isConnected())
	{
		disconnect();
	}

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}

	// Get mac
	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;

	// create a local handle for "host" object on the the server and set up local data structures
	status = dhcpctl_new_object (&hostHandle, connection, "host");

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		//printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		//printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string macString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&macString, 0, sizeof macString);
		status = omapi_data_string_new(&macString, length, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(macString->value, mac, length);
		status = dhcpctl_set_value(hostHandle, macString, "hardware-address");
		//printStatus(status);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
					"ACS_CS_DHCP_ConfiguratorImpl::addNormalDhcpBoard()\n"
					"Set MAC address %s for %s %d hostName %s\n",mac,__FUNCTION__,__LINE__,info->getHostName().c_str())
			);
	}

	// Set hardware type
	dhcpctl_data_string typeString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&typeString, 0, sizeof typeString);
		status = omapi_data_string_new(&typeString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		unsigned char typeArr[4] = {0x0, 0x0, 0x0, 0x1};
		memcpy(typeString->value, typeArr, 4);
		status = dhcpctl_set_value(hostHandle, typeString, "hardware-type");
		//printStatus(status);
	}

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;

	std::string ipaddress;
	char addr[INET_ADDRSTRLEN];
	uint32_t naddr = htonl(info->getAcAddress());
	inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

	ipaddress = addr;

	if (status == ISC_R_SUCCESS) {
		string statement("option server.next-server ");
		statement.append(ipaddress);
		statement.append("; ");

		//Option 66: Boot server host name
		statement.append("option tftp-server-name ");
		statement.append("\"");
		statement.append(ipaddress);
		statement.append("\"; ");

		//Option 67: Boot file name
		std::string outPath = info->getLoadModuleFileName();
		std::string ouprodWithOutIPT = ACS_APGCC::after(outPath, "DATA");

		statement.append("option bootfile-name ");
		statement.append("\"/IPT/DATA");
		statement.append(ouprodWithOutIPT);
		statement.append("\";");

		//Option 015: Domain Name
		statement.append("option domain-name ");
		statement.append("\"");
		statement.append("aplocaldomain.com");
		statement.append("\";");


		//Option 209: Start-up mode
		/*if (ACS_CS_Registry::IsCba())
		{
			statement.append("option startup-mode ");
			statement.append("\"");
			statement.append("cc");
			statement.append("\";");
		}*/
		//Option 229: AP OS Platform and the EGEM2 Env
		if (shelf_arch == NODE_SCB)
		{
			statement.append("option environment=\"env=APG43L\";");
		}
		if ((shelf_arch == NODE_SCX)||(shelf_arch == NODE_SMX))
		{
			statement.append("option environment=\"env=APG43L,shelf_type=CC\";");
		}
	
		//Override global always-broadcast option
		statement.append("option server.always-broadcast off; ");

		//Write statements into DHCP
		memset (&statementString, 0, sizeof statementString);
		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		//printStatus(status);

		if (status == ISC_R_SUCCESS)
		{
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			//printStatus(status);
		}
	}

	// Create host on server
	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		//printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		//printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostName)
		(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (macString)
		(void) dhcpctl_data_string_dereference(&macString, MDL);

	if (typeString)
		(void) dhcpctl_data_string_dereference(&typeString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	if (mac)
		delete [] mac;
	//Terminate connection
	disconnect();

	m_pSem->release();

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::addIplbNormalDhcpBoard(const ACS_CS_DHCP_Info * info)
{
	if (!info)
	{
		return false;
	}

	m_pSem->acquire();

	//Start a new connection
	if (isConnected())
	{
		disconnect();
	}

	if (!connect())
	{
	    m_pSem->release();
	    return false;
	}

	// Get mac
	int length = info->getMacLength();
	char * mac = new char[length];
	info->getMAC(mac, length);

	// Create host object
	dhcpctl_status status = ISC_R_SUCCESS;
	dhcpctl_handle hostHandle = NULL;

	// create a local handle for "host" object on the the server and set up local data structures
	status = dhcpctl_new_object (&hostHandle, connection, "host");

	// Set hostname on objectprintStatus(waitstatus);
	dhcpctl_data_string hostName = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&hostName, 0, sizeof hostName);
		status = omapi_data_string_new(&hostName, info->getHostName().size(), MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(hostName->value, info->getHostName().c_str(), info->getHostName().size());
		status = dhcpctl_set_value(hostHandle, hostName, "name");
		//printStatus(status);
	}

	// Set IP address
	dhcpctl_data_string ipString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&ipString, 0, sizeof ipString);
		status = omapi_data_string_new(&ipString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		struct in_addr convaddr;
		inet_pton(AF_INET, info->getIPString().c_str(), &convaddr);
		memcpy(ipString->value, &convaddr, 4);
		status = dhcpctl_set_value(hostHandle, ipString, "ip-address");
		//printStatus(status);
	}

	// Set MAC address
	dhcpctl_data_string macString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&macString, 0, sizeof macString);
		status = omapi_data_string_new(&macString, length, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		memcpy(macString->value, mac, length);
		status = dhcpctl_set_value(hostHandle, macString, "hardware-address");
		//printStatus(status);

		ACS_CS_TRACE((ACS_CS_DHCP_Configurator_TRACE,
					"ACS_CS_DHCP_ConfiguratorImpl::addNormalDhcpBoard()\n"
					"Set MAC address %s for %s %d hostName %s\n",mac,__FUNCTION__,__LINE__,info->getHostName().c_str())
			);
	}

	// Set hardware type
	dhcpctl_data_string typeString = NULL;
	if (status == ISC_R_SUCCESS) {
		memset (&typeString, 0, sizeof typeString);
		status = omapi_data_string_new(&typeString, 4, MDL);
		//printStatus(status);
	}

	if (status == ISC_R_SUCCESS) {
		unsigned char typeArr[4] = {0x0, 0x0, 0x0, 0x1};
		memcpy(typeString->value, typeArr, 4);
		status = dhcpctl_set_value(hostHandle, typeString, "hardware-type");
		//printStatus(status);
	}

	// Set DHCP options
	dhcpctl_data_string statementString = NULL;

	std::string ipaddress;
	uint32_t naddr;
	char addr[INET_ADDRSTRLEN];
	if (info->getNetwork() == ACS_CS_Protocol::Eth_A)
	{
	 	naddr = htonl(ACS_CS_NS::CLUSTER_IP_ETHA);
		inet_ntop(AF_INET, &naddr, addr, sizeof(addr));
		ipaddress = addr;
	}
	else if(info->getNetwork() == ACS_CS_Protocol::Eth_B)	
	{
		 naddr = htonl(ACS_CS_NS::CLUSTER_IP_ETHB);
		 inet_ntop(AF_INET, &naddr, addr, sizeof(addr));
                 ipaddress = addr;	
	}

	if (status == ISC_R_SUCCESS) {
		string statement("option server.next-server ");
		statement.append(ipaddress);
		statement.append("; ");

		//Option 66: Boot server host name
		statement.append("option tftp-server-name ");
		statement.append("\"");
		statement.append(ipaddress);
		statement.append("\"; ");

		//Option 67: Boot file name
		std::string outPath = info->getLoadModuleFileName();
		std::string ouprodWithOutIPT = ACS_APGCC::after(outPath, "IPLB");

		statement.append("option bootfile-name ");
		statement.append("\"/IPLB");
		statement.append(ouprodWithOutIPT);
		statement.append("\";");
		if (shelf_arch != NODE_VIRTUALIZED)
		{
			int turbo = getTurboBoostSetting();
			if(turbo == 1)
				statement.append("option environment=\"TURBO_BOOST=TRUE\";");
			else
			{
				if(turbo == 0)
					statement.append("option environment=\"TURBO_BOOST=FALSE\";");
			}
		}
	
		std::string iplb_side("");

		if (info->getSide() == ACS_CS_Protocol::Side_A)
			iplb_side = "A";
		else if (info->getSide() == ACS_CS_Protocol::Side_B)
			iplb_side = "B";


		//IPLB Side Option
		statement.append("option iplb-role ");
		statement.append("\"");
		statement.append(iplb_side);
		statement.append("\"; ");

		uint32_t last_byte = info->getIP() & 0x000000ff;
		uint32_t bgci_ip = 0;
		std::string ntp_server("option ntp-servers ");
		std::string bgci_address("option bgci-address ");

		uint32_t subnet[2];
		uint32_t netmask[2];
		uint32_t dmx_ipA, dmx_ipB = 0;

		ACS_CS_Registry::getBGCIVlan(subnet, netmask);
		ACS_CS_Registry::getDmxcAddress(dmx_ipA, dmx_ipB);

		if (info->getNetwork() == ACS_CS_Protocol::Eth_A)
		{
			char dmx_addr[INET_ADDRSTRLEN];
			dmx_ipA = htonl(dmx_ipA);
			inet_ntop(AF_INET, &dmx_ipA, dmx_addr, sizeof(dmx_addr));
			ntp_server.append(dmx_addr);
			bgci_ip = (subnet[0] & netmask[0]) | last_byte;
		}
		else if (info->getNetwork() == ACS_CS_Protocol::Eth_B)
		{
			char dmx_addr[INET_ADDRSTRLEN];
			dmx_ipB = htonl(dmx_ipB);
			inet_ntop(AF_INET, &dmx_ipB, dmx_addr, sizeof(dmx_addr));
			ntp_server.append(dmx_addr);
			bgci_ip = (subnet[1] & netmask[1]) | last_byte;
		}

		char bgci_addr[INET_ADDRSTRLEN];
		bgci_ip = htonl(bgci_ip);
		inet_ntop(AF_INET, &bgci_ip, bgci_addr, sizeof(bgci_addr));

		uint32_t internal_ip = INTERNAL_SUBNET | last_byte;
		char internal_addr[INET_ADDRSTRLEN];
		internal_ip = htonl(internal_ip);
		inet_ntop(AF_INET, &internal_ip, internal_addr, sizeof(internal_addr));

		ntp_server.append("; ");

		bgci_address.append(bgci_addr);
		bgci_address.append("; ");


		std::cout << ntp_server << std::endl;
		std::cout << bgci_address << std::endl;

		statement.append(ntp_server);
		statement.append(bgci_address);

		//Override global always-broadcast option
		statement.append("option server.always-broadcast off; ");
		
		statement.append("option tz-database-string ");
		std::string time_zone = "";
		getTimeZone(time_zone);
		statement.append("\"");
		statement.append(time_zone);
		statement.append("\"; ");
		
		std::cout << time_zone << std::endl;

		//Write statements into DHCP
		memset (&statementString, 0, sizeof statementString);
		status = omapi_data_string_new(&statementString, statement.size(), MDL);
		//printStatus(status);

		if (status == ISC_R_SUCCESS)
		{
			memcpy(statementString->value, statement.c_str(), statement.size());
			status = dhcpctl_set_value(hostHandle, statementString, "statements");
			//printStatus(status);
		}
	}

	// Create host on server
	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_open_object(hostHandle, connection, DHCPCTL_CREATE);
		//printStatus(status);
	}

	// Wait for call to complete
	dhcpctl_status waitstatus = ISC_R_SUCCESS;
	if (status == ISC_R_SUCCESS)
	{
		status = dhcpctl_wait_for_completion(hostHandle, &waitstatus);
		//printStatus(status);
		//printStatus(waitstatus);
	}

	// Free dhcpctl string objects
	if (hostName)
		(void) dhcpctl_data_string_dereference(&hostName, MDL);

	if (ipString)
		(void) dhcpctl_data_string_dereference(&ipString, MDL);

	if (macString)
		(void) dhcpctl_data_string_dereference(&macString, MDL);

	if (typeString)
		(void) dhcpctl_data_string_dereference(&typeString, MDL);

	if (statementString)
		(void) dhcpctl_data_string_dereference(&statementString, MDL);

	if (hostHandle)
		omapi_object_dereference(&hostHandle, MDL);

	// Free allocated memory
	if (mac)
		delete [] mac;
	//Terminate connection
	disconnect();

	m_pSem->release();

	return (status == ISC_R_SUCCESS && waitstatus == ISC_R_SUCCESS);
}

bool ACS_CS_DHCP_ConfiguratorImpl::getTimeZone(std::string& timeZone)
{
	bool result = true;
	timeZone = "";
	std::stringstream ss;

	FILE *in;
	char buff[512] = {0};
	char cmd[512] = {0};

	sprintf(cmd, "%s timezone -D | awk 'NR>1 {print $3}'", CLUSTERCONF_CMD.c_str());

	if(!(in = popen(cmd, "r")))
	{
		result = false;
	}
	else
	{
		while(fgets(buff, sizeof(buff), in) != NULL)
		{
			ss << buff;
			//std::cout << buff;
		}
		pclose(in);

		timeZone = ss.str();

		//Remove NEWLINE at the end of the string
		size_t index = timeZone.find_first_of("\n");
		if (index != string::npos)
			timeZone = timeZone.substr(0,index);
	}

	return result;
}
int ACS_CS_DHCP_ConfiguratorImpl::getTurboBoostSetting()
{
	int result = 0;
	std::string str=" ";
	ifstream myfile (ACS_CS_NS::TURBO_BOOST_FILE);

	if (myfile.is_open()){
		std::getline(myfile,str);
		if(strcmp(str.c_str(),"FALSE") == 0){
			result = 0;
		}
		else{
			result = 1;
		}
	}
	else{
		result = -1;
	}
	myfile.close();
	return result;
}
