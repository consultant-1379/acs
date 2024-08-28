/*
 * ACS_TRAPDS_ServerConnectionImpl.cpp
 *
 *  Created on: 13/gen/2012
 *      Author: renato
 */

#include "ACS_TRAPDS_ServerConnectionImpl.h"


ACS_TRAPDS_ServerConnectionImpl::ACS_TRAPDS_ServerConnectionImpl(std::string name, int port)
{
//	setPort(CSPORT);
//	setRemoteAddress(std::string("192.168.169.2"));
	// TODO Auto-generated constructor stub
	nameSubscriber.clear();
	portSubscriber = 0;

	setRemotePort(port);
	nameSubscriber=name;
	portSubscriber=port;

//	setLocalPort(6010);
	setConnection();


}

ACS_TRAPDS_ServerConnectionImpl::~ACS_TRAPDS_ServerConnectionImpl() {
	// TODO Auto-generated destructor stub
}

int ACS_TRAPDS_ServerConnectionImpl::sendPackage()
{
	printf("sent trap to %s! on port %d\n",nameSubscriber.c_str(),portSubscriber);
	send_data();

	return 0;
}

