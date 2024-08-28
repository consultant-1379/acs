/*
 * ACS_TRAPDS_SNMPTrapReceiver.cpp
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */


#include "ACS_TRAPDS_SNMPTrapReceiver.h"


netsnmp_transport* ACS_TRAPDS_SNMPTrapReceiver::transport1=NULL;
netsnmp_transport* ACS_TRAPDS_SNMPTrapReceiver::transport2=NULL;
netsnmp_transport* ACS_TRAPDS_SNMPTrapReceiver::transport3=NULL;
netsnmp_transport* ACS_TRAPDS_SNMPTrapReceiver::transport4=NULL;


ACS_TRAPDS_SNMPTrapReceiver* ACS_TRAPDS_SNMPTrapReceiver::m_instance = 0;

ACS_TRAPDS_SNMPTrapReceiver::ACS_TRAPDS_SNMPTrapReceiver()
{
	bRun = false;
	bExit = true;
}


ACS_TRAPDS_SNMPTrapReceiver::~ACS_TRAPDS_SNMPTrapReceiver()
{

}

ACS_TRAPDS_SNMPTrapReceiver* ACS_TRAPDS_SNMPTrapReceiver::getInstance ()
{
	if (m_instance == 0)
	{
		m_instance = new ACS_TRAPDS_SNMPTrapReceiver();
	}

	return m_instance;
}

int ACS_TRAPDS_SNMPTrapReceiver::initialize()
{
	bExit = false;

	int result = activate();

	if(result == -1)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Activation failed" << std::endl;
		bExit = true;
	}

	return result;
}

void ACS_TRAPDS_SNMPTrapReceiver::finalize ()
{
	delete(m_instance);
	m_instance = NULL;
}

int ACS_TRAPDS_SNMPTrapReceiver::close (){
	//Thread will stop!
	bExit = true;
	return 0;
}


netsnmp_transport* ACS_TRAPDS_SNMPTrapReceiver::getSnmpTransport()
{
	return transport1;

}

int ACS_TRAPDS_SNMPTrapReceiver::svc(void)
{
	int local_port = SNMP_TRAP_PORT;


	init_snmp(TRAP_RECEIVER_SERVICE_NAME);
	// Open snmp server

	bool isAP1 = TRAPDS_Util::checkIfAP1();

	TRAPDS_Util::EnvironmentType env = TRAPDS_Util::getEnvironment();

	if (env == TRAPDS_Util::SINGLECP_DMX || env == TRAPDS_Util::MULTIPLECP_DMX)
	{
		std::string trap_receiver_port1("");
		std::string trap_receiver_port2("");
		std::string trap_receiver_port3("");
		std::string trap_receiver_port4("");

		if(isAP1){
			std::cout << "\n----------------------------------------------------------------------"<< std::endl;
			std::cout << "            TRAPDS knows that is running on AP1 in BSP !!!             " << std::endl;
			std::cout << "----------------------------------------------------------------------\n"<< std::endl;

			char _bgcia_str [2][16] = {{0}};
			char _bgcib_str [2][16] = {{0}};

			if(TRAPDS_Util::getBgciIpAddress(_bgcia_str,_bgcib_str,1))
			{
				std::cout << "\n Bgci Address fetching from CS Successful\n";	
				trap_receiver_port1= "udp:" + std::string(_bgcia_str[1]) + ":162";
				trap_receiver_port2= "udp:" + std::string(_bgcib_str[1]) + ":162";
				trap_receiver_port3= "udp:" + std::string(_bgcia_str[0]) + ":162";
				trap_receiver_port4= "udp:" + std::string(_bgcib_str[0]) + ":162";

				transport1 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port1.c_str());
				transport2 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port2.c_str());
				transport3 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port3.c_str());
				transport4 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port4.c_str());
			}

		}
		else{
			std::cout << "\n----------------------------------------------------------------------"<< std::endl;
			std::cout << "            TRAPDS knows that is running on AP2 in BSP !!!             " << std::endl;
			std::cout << "----------------------------------------------------------------------\n"<< std::endl;

			char _bgcia_str [2][16] = {{0}};
			char _bgcib_str [2][16] = {{0}};

			if(TRAPDS_Util::getBgciIpAddress(_bgcia_str,_bgcib_str,2))
			{
				std::cout << "\n Bgci Address fetching from CS Successful\n";
				trap_receiver_port1= "udp:" + std::string(_bgcia_str[1]) + ":162";
				trap_receiver_port2= "udp:" + std::string(_bgcib_str[1]) + ":162";
				trap_receiver_port3= "udp:" + std::string(_bgcia_str[0]) + ":162";
				trap_receiver_port4= "udp:" + std::string(_bgcib_str[0]) + ":162";

				transport1 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port1.c_str());
				transport2 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port2.c_str());
				transport3 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port3.c_str());
				transport4 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, trap_receiver_port4.c_str());
			}

		}
	}
	else
	{
		if(isAP1){
			std::cout << "\n---------------------------------------------------------------"<< std::endl;
			std::cout << "            TRAPDS knows that is running on AP1 !!!             " << std::endl;
			std::cout << "---------------------------------------------------------------\n"<< std::endl;

			if (env == TRAPDS_Util::SINGLECP_SMX || env == TRAPDS_Util::MULTIPLECP_SMX)
			{
				transport1 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_CLUSTER1_AP1);
				transport2 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_CLUSTER2_AP1);
			}
			else
			{
				transport1 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT1_AP1);
				transport2 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT2_AP1);
				transport3 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT3_AP1);
				transport4 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT4_AP1);
			}
		}
		else{
			std::cout << "\n---------------------------------------------------------------"<< std::endl;
			std::cout << "            TRAPDS knows that is running on AP2 !!!             " << std::endl;
			std::cout << "---------------------------------------------------------------\n"<< std::endl;

			if (env == TRAPDS_Util::SINGLECP_SMX || env == TRAPDS_Util::MULTIPLECP_SMX)
			{
				// As in SMX environment there is no AP2, this part of the code will not be used
				transport1 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_CLUSTER1_AP2);
				transport2 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_CLUSTER2_AP2);
			}
			else
			{

				transport1 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT1_AP2);
				transport2 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT2_AP2);
				transport3 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT3_AP2);
				transport4 = netsnmp_transport_open_server(TRAP_RECEIVER_SERVICE_NAME, TRAP_RECEIVER_PROT_AND_PORT4_AP2);
			}
		}
	}

	// Check errors

    if((transport1==NULL)&& (transport2==NULL)&& (transport3==NULL)&& (transport4==NULL))
    {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " error: session setting is impossible... "  << std::endl;
		return TRAP_RECEIVER_SVC_ERR;
		return -1;
    }

	bRun = true;

	netsnmp_session sess;
	int count, numfds, block;
	fd_set fdset;
	numfds = 0;
	block = 0;

	struct timeval timeout, *tvp;
	tvp = &timeout;
	timerclear(tvp);
	tvp->tv_usec = 0;
	FD_ZERO(&fdset);

	snmp_sess_init(&sess);
	sess.peername = SNMP_DEFAULT_PEERNAME; /* Original code had NULL here */
	sess.version = SNMP_DEFAULT_VERSION;
	sess.community = (u_char *)(TRAP_RECEIVER_COMMUNITY_NAME);
	sess.community_len = strlen(TRAP_RECEIVER_COMMUNITY_NAME);
	sess.retries = SNMP_DEFAULT_RETRIES;
	sess.timeout = SNMP_DEFAULT_TIMEOUT;
	sess.local_port = local_port;
	sess.callback = (ACS_TRAPDS_SNMPTrapReceiver::trapDispatch);
	//session->callback_magic = (void *) transport;
	sess.callback_magic = (void *)this;
	sess.authenticator = NULL;
	sess.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;

//	snmp_add(&sess, transport, NULL, NULL);

	if(transport1!=0)
		snmp_add(&sess, transport1, NULL, NULL);

	if(transport2!=0)
		snmp_add(&sess, transport2, NULL, NULL);

	if(transport3!=0)
		snmp_add(&sess, transport3, NULL, NULL);

	if(transport4!=0)
		snmp_add(&sess, transport4, NULL, NULL);

	printf("sessions added!\n");

	shutdownEvent = ACS_TRAPDS_Event::OpenNamedEvent(TRAPDS_Util::EVENT_NAME_SHUTDOWN);
	// If "bExit" is false, exit from while (end trap receive thread).

	bExit=false;
	while(! bExit) {

		// Initialize file descriptors set
		snmp_select_info(&numfds, &fdset, tvp, &block);
		// Add a shutdown event to file descriptor set
		FD_SET(shutdownEvent, &fdset);
		// Increment events counter
		++numfds;
		// select loop...
		count = select(numfds, &fdset, 0, 0, NULL);
		// check return value
		if (count > 0){
			// We test if event is a shutdown
			if (FD_ISSET(shutdownEvent, &fdset) != 0){
				// shutdown... Set exit flag
				close();
			}else{
				// read
				snmp_read(&fdset);
			}
		}else if(count == 0){
			// Timeout event. Impossible: the select is used without timeout.
			snmp_timeout();
		}else{
			// Error...
			fprintf(stderr, "select returned %d\n", count);
		}
	} // end while

	// Release snmp resource
	::close(numfds);
	snmp_close_sessions();
	// Thread close
	// Exit from thread

	return 0;
}

in_addr ACS_TRAPDS_SNMPTrapReceiver::getRemoteIP(struct snmp_session *session, struct snmp_pdu *pdu)
{
    struct sockaddr_in *to;
    in_addr addrRet;
    netsnmp_transport *transport;
        //Initialization
    memset(&addrRet,0,sizeof(in_addr));
    to = NULL;
    transport = NULL;

    transport = snmp_sess_transport(snmp_sess_pointer(session));
    if ((pdu != NULL) && (pdu->transport_data != NULL)){
        to = (struct sockaddr_in *) pdu->transport_data;
    } else if (transport != NULL && transport->data != NULL) {
        to = (struct sockaddr_in *) transport->data;
    }
    if(to != NULL){
    	addrRet = to->sin_addr;
    }

    return(addrRet);
}


/*static*/
int ACS_TRAPDS_SNMPTrapReceiver::trapDispatch(int op,
													struct snmp_session *session,
													int reqid,
													struct snmp_pdu *pdu,
													void *magic)
{
    int iRet;

    iRet = TRAP_HANDLER_ERROR;

    iRet = ((ACS_TRAPDS_SNMPTrapReceiver*)magic)->trapHandler(op,session,reqid,pdu,NULL);

    return(iRet);
}

int ACS_TRAPDS_SNMPTrapReceiver::trapHandler(int /*op*/,
												   struct snmp_session *session,
												   int /*reqid*/,
												   struct snmp_pdu *pdu,
												   void */*magic*/)
{

	int iRet = 0;
	char *ip_remote_agent;
	in_addr addr;
	addr = getRemoteIP(session,pdu);
	ip_remote_agent = inet_ntoa(addr);

	cout << " ---------------------- ACS_TRAPDS_SNMPTrapReceiver IP TRAP:  " << ip_remote_agent << " -----------------------------" << endl;

//	for(int t=0;t<(int)observers.size();t++)
//	{
//		index=(ACS_TRAPDS_ServerConnection*)observers[t];
//		index->setPdu(pdu);
//	}
//
//	for(int t=0;t<(int)observers.size();t++)
//	{
//		observers[t]->sendPackage();
//	}

	ACS_TRAPDS_ServerConnection *index=0;

	std::map<std::string,ACS_TRAPDS_SnmpObserver*>::iterator it;
	for (it = observers.begin() ; it != observers.end() ; it++)
	{
		index=(ACS_TRAPDS_ServerConnection*)it->second;
		index->setPdu(pdu);
		index->setIpTrap(std::string(ip_remote_agent));

		it->second->sendPackage();
	}

//	std::map<std::string,ACS_TRAPDS_SnmpObserver*>::iterator t;
//	for (t = observers.begin() ; t != observers.end() ; t++)
//	{
//		t->second->sendPackage();
//	}

	return(iRet);

}



void ACS_TRAPDS_SNMPTrapReceiver::OidToString(oid *iodIn, unsigned int len, std::string *strOut)
{
	std::stringstream sStr;
	unsigned int x;
	// Init
	x = 0;
	sStr.str("");
	(*strOut) = "";
	// for all element in oid,
	for (x = 0; x < len ; ++x){
	   sStr << '.';
	   sStr << (int)iodIn[x];
	}
	// Set output string
	(*strOut) = sStr.str();
}

void ACS_TRAPDS_SNMPTrapReceiver::addSubscriber(ACS_TRAPDS_ServerConnection *subsciber, std::string dn)
{
	std::cout << " ...AddSubscriber dn: " << dn.c_str() << "\n map size:" << observers.size()<< std::endl;
	observers.insert ( pair<std::string, ACS_TRAPDS_SnmpObserver*>(dn, subsciber) );
	printMap();
}

void ACS_TRAPDS_SNMPTrapReceiver::removeSubscriber(std::string dn)
{

	std::cout << " ...Try to RemoveSubscriber dn: " << dn.c_str()<< std::endl;

	mapObservers::iterator it = observers.find(dn);
	if (it != observers.end())
	{
		std::cout << " ...RemoveSubscriber dn: " << dn.c_str()<< std::endl;
		ACS_TRAPDS_ServerConnection *obs = (ACS_TRAPDS_ServerConnection*)it->second;

		observers.erase(it);

		if (obs)
		{
			delete (obs);
			obs = NULL;
		}

	}

	printMap();
}

void ACS_TRAPDS_SNMPTrapReceiver::printMap()
{
	cout << "----------- observers map ---------- " << endl;

	for (mapObservers::iterator itr = observers.begin(); itr != observers.end(); itr++)
	{
		cout << "\n Dn: "<< itr->first << endl;
	}

}
