/*
 * @file ACS_CS_DHCP_Configurator.h
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

#ifndef ACS_CS_DHCP_CONFIGURATOR_H_
#define ACS_CS_DHCP_CONFIGURATOR_H_

#include <string>
#include <map>
#include <ace/Semaphore.h>
#include <ace/Singleton.h>

#include "ACS_CS_Util.h"


class ACS_CS_DHCP_Info;
class ACS_CS_DHCP_ConfiguratorImpl;

/** @class ACS_CS_DHCP_Configurator
 *	@brief DHCP configuration class for CS
 *	@author xminaon
 *
 *	A class used by CS to configure the ISC DHCP server running on APG43 for Linux.
 *	The communication with the DHCP server is done through the omapi API which is wrapped in
 *	an API called DHCPCTL. Both of these API:s are C-based. This class wraps the calls to these
 *	C-based API:s in C++-code adapted to the needs for CS, i.e. more high-level functions like
 *	C-based add/remove board.
 */
class ACS_CS_DHCP_Configurator {

public:
	/** @brief Function to connect to DHCP server
	 * 	Must be called prior to using any of the add/remove functions.
	 * 	@return true if connection succeeded, otherwise false
	 */
	bool connect() ;

	/** @brief Function to disconnect from the DHCP server
	 * 	Must be called when the client has finished using the API.
	 * 	@return true if disconnection succeeded, otherwise false
	 */
	bool disconnect() ;

	/** @brief Function to check if there is a connection to the DHCP server
	 * 	@return true if connected, otherwise false
	 */
	bool isConnected() const;

	/** @brief Function to add client DHCP board (non CPUB boards)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool addClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to add normal DHCP board (CPUB boards)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool addNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to update client DHCP board (non CPUB boards)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool updateClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to update normal DHCP board (CPUB boards)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool updateNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to remove client DHCP board (non CPUB boards)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool removeClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to remove normal DHCP board (CPUB boards)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool removeNormalDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	bool removeNormalDhcpBoardByMAC(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to retrieve IP address from DHCP configuration based on hostname
	 *  @param[in]	name	string containing hostname
	 *  @param[out]	ip_addr	reference to string which will eventually hold ip address
	 * 	return true if success, otherwise false
	*/
	bool fetchIp(const std::string &name, std::string &ip_addr) ;

	/** @brief Function to retrieve MAC address from DHCP configuration based on hostname
	 *  @param[in]	name	string containing hostname
	 *  @param[out]	MAC	pointer to string which will eventually hold mac address
	 * 	@return true if success, otherwise false
	*/
	bool fetchMAC(const std::string &name, char* mac) ;

	/** @brief Returns true if client host exists, otherwise false
	 *  @param[in]	uid	the client Id for the host
	 *  @param[in]	size	size of client Id
	 * 	return true if host exists, otherwise false
	*/
	bool existsClientHost(const char * uid, int size) ;

	/** @brief Returns true if normal host exists, otherwise false
	 *  @param[in]	mac	the hardware address for the host
	 *  @param[in]	size	size of mac address (normally 6)
	 * 	return true if host exists, otherwise false
	*/
	bool existsNormalHost(const char * mac, int size) ;

	/** @brief Function to add DHCP board for APZ2123X system (with uid, tft-server, bootfile in reservation entry)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool addClientAPZ2123XDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to add DHCP board for APZ21250 system (with MAC, next-server, bootfile in reservation entry)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool addNormalAPZ21250DhcpBoard(const ACS_CS_DHCP_Info * info) ;

	/** @brief Function to add DHCP board for APZ21240 system (with MAC, next-server, bootfile in reservation entry)
	 *  @param[in]	info	An ACS_CS_DHCP_Info object describing the DHCP reservation.
	 * 	@return true if success, otherwise false
	 */
	bool addAPZ21240DhcpBoard(const ACS_CS_DHCP_Info * info) ;

	bool addScxbDhcpBoard(const ACS_CS_DHCP_Info * info) ;
	
	bool addSmxbDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	bool addCmxbDhcpBoard(const ACS_CS_DHCP_Info * info);

	bool addEBSClientDhcpBoard(const ACS_CS_DHCP_Info * info) ;

	bool removeDhcpReservationByHost(std::string host);



	int cleanUpDhcpLeases();

private:
	ACS_CS_DHCP_Configurator(const ACS_CS_DHCP_Configurator &right);
	ACS_CS_DHCP_Configurator & operator=(const ACS_CS_DHCP_Configurator &right);
	ACS_CS_DHCP_ConfiguratorImpl * impl;

	/** @brief ACS_CS_DHCP_Configurator Constructor
	 *
	 *	param[in]	ip	The IP-address to use to access the DHCP server
	 *	param[in]	port	The port on which the DHCP server listens for API requests
	 */
	ACS_CS_DHCP_Configurator(std::string ip, int port);

	virtual ~ACS_CS_DHCP_Configurator();

	friend class ACS_CS_DHCP_Factory;

};



class ACS_CS_DHCP_Factory {

	typedef std::pair<std::string, int> DhcpHost;

public:
	static ACS_CS_DHCP_Factory * instance();
	ACS_CS_DHCP_Configurator * createDhcpConfigurator(std::string ip = ACS_CS_NS::DHCP_Server_IP, int port = ACS_CS_NS::DHCP_Server_Port);
	static void finalize();

private:
	std::map<DhcpHost, ACS_CS_DHCP_Configurator *> dhcpConfMap;

	ACS_CS_DHCP_Factory();
	ACS_CS_DHCP_Factory(const ACS_CS_DHCP_Factory &other);
	ACS_CS_DHCP_Factory & operator=(const ACS_CS_DHCP_Factory &right);
	virtual ~ACS_CS_DHCP_Factory();

	typedef ACE_Singleton<ACS_CS_DHCP_Factory, ACE_Recursive_Thread_Mutex> instance_;
	friend class ACE_Singleton<ACS_CS_DHCP_Factory, ACE_Recursive_Thread_Mutex>;
};

#endif /* ACS_CS_DHCP_CONFIGURATOR_H_ */
