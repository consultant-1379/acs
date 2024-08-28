/*
 * ACS_TRAPDS_ServerConnection.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_SERVERCONNECTION_H_
#define ACS_TRAPDS_SERVERCONNECTION_H_


#include <boost/serialization/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ace/OS.h"
#include "ace/SOCK_Dgram.h"
#include "ace/INET_Addr.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_transport.h>
#include <sstream>
#include <iosfwd>
#include <fstream>
#include "ACS_TRAPDS_StructVariable.h"
#include "ACS_TRAPDS_SnmpObserver.h"
#include "ACS_TRAPDS_Sender.h"


//BOOST_CLASS_TRACKING(ACS_TRAPDS_SnmpObserver, track_never);
//BOOST_CLASS_TRACKING(ACS_TRAPDS_StructVariable, track_never);

//#define DATA_BUFFER_SIZE 500000
//#define SIZE_DATA 19

using std::stringstream;

class ACS_TRAPDS_ServerConnection : public ACS_TRAPDS_SnmpObserver
{

public:

	ACS_TRAPDS_ServerConnection();
	ACS_TRAPDS_ServerConnection(int);
	virtual ~ACS_TRAPDS_ServerConnection();
	int send_data();
	int setPort(int port);
	virtual int sendPackage()=0;
	int setPdu(snmp_pdu *pdu);
	int setIpTrap(std::string ipTrap);
	int setFilename(std::string fn);
	void save_trap_date(const ACS_TRAPDS_StructVariable &v);
	void restore_trap_date(ACS_TRAPDS_StructVariable &v);
	void setRemoteAddress(std::string ra);
	void setRemotePort(int rp);
	void setLocalPort(int lp);
	int setConnection();

	ACE_INET_Addr *remote_addr_;

private:

	int remote_port;
	int local_port;
	std::string filename;
	int data_buf_len;
	int port;
	std::string remote_address;
	char *data_buf;
	std::stringstream *dataStringStream;
	struct snmp_pdu *pdu;
	std::string ipTrap;

};


#endif /* ACS_TRAPDS_SERVERCONNECTION_H_ */
