/*=================================================================== */
/**
  @file acs_nsf_server.cpp

  Class method implementation for ACS_NSF_Server class.

  This module contains the implementation of class declared in
  the acs_nsf_server.h module.

  @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011   XCHEMAD     APG43 on Linux.
   N/A       25/02/2014   XFABPAG     TR HS31786 Fix
   N/A       25/04/2014   XQUYDAO     TR HS48771 Fix
   N/A       05/05/2014   XQUYDAO     Update for cable-less environment
   N/A       05/06/2014   XQUYDAO     Update for to fix faults related to system time change, 
                                      file descriptor leak and wrong gateway IP when 2 gateway are defined
 **/
/*=================================================================== */

/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Task.h>
#include <ace/Ping_Socket.h>
#include <ace/INET_Addr.h>
#include <acs_nsf_server.h>
#include <ACS_APGCC_Util.H>
#include <acs_prc_api.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <acs_nsf_service.h>

using namespace std;

/**
 * @brief	Clientportno
 * 			Serverportno
 */
int Clientportno = 0, Serverportno = 0;

/**
 * @brief	currentNode
 */
NODE_NAME currentNode = UNDEFINED;

/**
 * @brief	m_srcMutex
 */
ACE_Recursive_Thread_Mutex m_srctMutex;

/**
 * @brief	NSFEventList
 */
eventStruct* NSFEventList = 0;

/**
 * @brief       isStopPingThread
 */
bool isStopPingThread = false;


ACS_NSF_Common ACS_NSF_Server::objCommon;

std::string timeStampFileName(ACS_NSF_TIMESTAMPFILE_PATH);

/**
 * @brief	in_cksum
 * @param 	addr
 * @param 	len
 * @return	uint16_t
 *
 */
uint16_t in_cksum(uint16_t *addr, unsigned len)
{
	uint16_t answer = 0;
	/*
	 * This algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	uint32_t sum = 0;
	while (len > 1)
	{
		sum += *addr++;
		len -= 2;
	}

	// mop up an odd byte, if necessary
	if (len == 1)
	{
		*(unsigned char *)&answer = *(unsigned char *)addr ;
		sum += answer;
	}

	// add back carry outs from top 16 bits to low 16 bits
	sum = (sum >> 16) + (sum & 0xffff); // add high 16 to low 16
	sum += (sum >> 16); // add carry
	answer = ~sum; // truncate to 16 bits

	return answer;
}

/**
 * @brief	stopPingThreadFds
 *		This handle is used to send a stop signal to the 
 *		to the heartbeat thread.
 *
 */
ACE_HANDLE stopPingThreadFds[2];

/**
 * @brief	stopGatewayThreadFds
 *		This handle is used to send a stop signal to the gateway 
 *		threads.
 */
ACE_HANDLE stopGatewayThreadFds[2] ;

#ifdef _NSF_PING_SIMULATION
int ping(int network)
{
	stringstream ss;
	ss << "/tmp/nsf_network_" << network;	
	ifstream ifs(ss.str().c_str(), std::ifstream::in);
	int pingValue = 1;
	if (ifs)
	{
	   ifs >> pingValue;
		ifs.close();
	}
	else
	{
		DEBUG("Ping simulation file is not opened: %s", ss.str().c_str());
	}
	
	return pingValue;
}
#else
/**
 * @brief	ping
 * @param 	target
 * @param 	s
 * @return	int
 * 			-1: Error
 * 			 0: Success
 */
int ping(string target, int s)
{
	int i, cc, packlen, datalen = DEFDATALEN;
	struct hostent *hp= 0;
	struct sockaddr_in to, from;
	//struct ip *ip_addr = 0;
	u_char *packet = 0;
	//Fix for TR HU83374 start
	//u_char outpack[MAXPACKET] = { 0 };
	struct icmp outpack_mem ;
	struct icmp *outpack=(&outpack_mem) ;
	//Fix for TR HU83374 end
	char hnamebuf[MAXHOSTNAMELEN];
	string hostname;
	struct icmp *icp = 0;
	int ret, fromlen, hlen;
	fd_set rfds;
	struct timeval tv;
	int retval;
	struct timeval start, end;
	int end_t;
	bool cont = true;

	ACE_OS::memset(&to,0,sizeof( struct sockaddr_in));
	ACE_OS::memset(&from,0, sizeof( struct sockaddr_in));

	to.sin_family = AF_INET;

	// try to convert as dotted decimal address, else if that fails assume it's a hostname

	to.sin_addr.s_addr = inet_addr(target.c_str());
	if (to.sin_addr.s_addr != (u_int)-1)
	{
		hostname = target;
	}
	else
	{
		hp = gethostbyname(target.c_str());
		if (!hp)
		{
			ERROR("unknown host %s", target.c_str());
			ERROR("%s", "Exiting ping");
			return -1;
		}
		to.sin_family = hp->h_addrtype;
		ACE_OS::memcpy((caddr_t)&to.sin_addr, hp->h_addr, hp->h_length);
		strncpy(hnamebuf, hp->h_name, sizeof(hnamebuf) - 1);
		hostname = hnamebuf;
	}
	packlen = datalen + MAXIPLEN + MAXICMPLEN;

	if ( (packet = (u_char *)malloc((u_int)packlen)) == NULL)
	{
		ERROR("%s", "Error occured while allocating memory for packet.");
		ERROR("%s", "Exiting ping");
		return -1;
	}

	string seq = "123";
	//Fix for TR HU83374 start
	int random_seq = rand() % 100;         // v1 in the range 0 to 99
	//Fix for TR HU83374 end
	string temp = (ACS_APGCC::itoa(random_seq));
	seq.append(temp);
	int seq_no = (ACS_APGCC::atoi(seq)); 

	DEBUG("Set seq_no= %d, seq= %s, temp= %s", seq_no, seq.c_str(), temp.c_str());


	icp = outpack; 		//Fix for TR HU83374
	icp->icmp_type = ICMP_ECHO;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = seq_no;  /* seq and id must be reflected */
	icp->icmp_id = getpid();

	DEBUG("icmp_id= %d, icmp_seq= %d", icp->icmp_id, icp->icmp_seq);

	cc = datalen + ICMP_MINLEN;
	icp->icmp_cksum = in_cksum((unsigned short *)icp,cc);

	gettimeofday(&start, NULL);

	i = sendto(s, (char *)outpack, cc, 0, (struct sockaddr*)&to, (socklen_t)sizeof(struct sockaddr_in));
	if (i < 0 || i != cc)
	{
		if (i < 0)
		{
			ERROR("%s", "Error occured while writing data into socket");
			ERROR("%s", "Exiting ping");
			free(packet);
			return -1;
		}
		DEBUG("wrote %s  %d chars, ret= %d\n", hostname.c_str(),cc, i);
	}

	// Watch stdin (fd 0) to see when it has input.
	FD_ZERO(&rfds);
	FD_SET(s, &rfds);
	FD_SET(stopPingThreadFds[0], &rfds);

	int maxfd = s;

	if( stopPingThreadFds[0] > s )
	{
		maxfd = stopPingThreadFds[0] ;
	} 	
	// Wait up to 3 seconds.
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	while(cont)
	{
		retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
		//retval = select(s+1, &rfds, NULL, NULL, &tv);
		if (retval == -1)
		{
			ERROR("%s", "Error occured in select system call");
			ERROR("%s", "Exiting ping");
			free(packet);
			return -1;
		}
		else if (retval)
		{
			fromlen = sizeof(sockaddr_in);
			if( FD_ISSET(stopPingThreadFds[0], &rfds))
			{
				ERROR("%s", "Stop event signalled while waiting for client connection");
				free(packet);
				isStopPingThread = true;
				return 0;
			}
			if ( (ret = recvfrom(s, (char *)packet, packlen, 0,(struct sockaddr *)&from, (socklen_t*)&fromlen)) < 0)
			{
				ERROR("%s", "Error occured in recvfrom");
				ERROR("%s", "Exiting ping");
				free(packet);

				return -1;
			}

			// Check the IP header
			//ip_addr = (struct ip *)((char*)packet);

			hlen = sizeof( struct ip );
			if (ret < (hlen + ICMP_MINLEN))
			{
				ERROR("%s", "Error occured in recvfrom");
				ERROR("%s", "Exiting ping");
				free(packet);
				//ip_addr = 0;
				return -1;

			}

			// Now the ICMP part
			icp = (struct icmp *)(packet + hlen);
			if (icp->icmp_type == ICMP_ECHOREPLY)
			{
				if (icp->icmp_seq != seq_no)
				{
					ERROR("received sequence # %d", icp->icmp_seq);
					ERROR("expected sequence_no # %d", seq_no);
					continue;
				}
				if (icp->icmp_id != getpid())
				{
					ERROR("received id %d", icp->icmp_id);
					continue;
				}
				DEBUG("%s", "Setting cont=false ");
				cont = false;
			}
			else
			{
				continue;
			}

			DEBUG("%s", "ICMP check part ended");
			gettimeofday(&end, NULL);
			end_t = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

			if(end_t < 1)
				end_t = 1;

			free(packet);
			//ip_addr = 0;
			return end_t;
		}
		else
		{
			free(packet);
			return 0;
		}
	}
	free(packet);
	return 0;
}
#endif

/**
 * @brief	checkBondingStatus 
 * @return	int
 * 			-1: Error
 * 			 0: UP
 *				 1: DOWN
 */
int checkBondingStatus()
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd <= 0) 
	{
		return -1;
	}
	
	int rc = 0;
	struct ifreq ifr;  
	strncpy(ifr.ifr_name, NSF_CBL_BONDING_INTERFACE, sizeof(ifr.ifr_name));
	if (!ioctl (fd, SIOCGIFFLAGS, &ifr)) 
	{		
		if (ifr.ifr_flags & IFF_RUNNING)
		{
			rc = 0; // UP
		}
		else
		{
			rc = 1; // DOWN
		}
	}
	else
	{		
		rc = -1;
	}
	
	::close(fd);
	return rc;	
}

/**
 * @brief	createTimeStampFolder
 * @param 	lpszDirname : const ACE_TCHAR*
 * @return	none
 */
void createTimeStampFolder(const ACE_TCHAR* lpszDirname)
{
	ACE_stat fileStat;

	if (ACE_OS::stat(lpszDirname,&fileStat) != 0)
	{
		FILE *fp = NULL;
		std::string cmd("mkdir -p ");
		cmd.append(lpszDirname);
		fp = popen(cmd.c_str(),"r");
		if(fp == NULL)
		{
			ERROR("ERROR occurred while executing %s",cmd.c_str());
		}
		else
		{
			pclose(fp);
		}
	}
	else
	{
		INFO("%s is Already Exists",lpszDirname);
	}
}

/**
 * @brief	createRestoreFlag
 * @param	pso_clear_path: The path to the PSO clear area
 * @param	flag_name: The name of the flag to be created
 * @return	0 on success, a non-zero value otherwise
 */
int createRestoreFlag (const char * pso_clear_path, const char * flag_name) {
	ACE_stat stat_info;

	// First, check if the directory under PSO area exists
	if (ACE_OS::stat(pso_clear_path, &stat_info)) {
		INFO("The PSO Clear folder (%s) doesn't exist: creating it!", pso_clear_path);

		// The folder doesn't exists, create it
		if (ACE_OS::mkdir(pso_clear_path)) {
			const int errno_save = ACE_OS::last_error();
			ERROR("Call 'mkdir' failed for folder '%s', error == %d", pso_clear_path, errno_save);
			return -1;
		}
	}

	// Build the path to the restore flag file
	const size_t flag_path_len = 2048;
	char flag_path[flag_path_len] = {0};
	ACE_OS::snprintf(flag_path, flag_path_len, "%s%s", pso_clear_path, flag_name);

	// Second, check if the restore flag has been already created
	if (ACE_OS::stat(flag_path, &stat_info)) {
		INFO("The restore flag (%s) doesn't exist: creating it!", flag_path);

		// The restore flag doesn't exists, create it
		ACE_HANDLE file_fd = ACE_INVALID_HANDLE;
		if ((file_fd = ACE_OS::creat(flag_path, ACE_DEFAULT_FILE_PERMS)) < 0) {
			const int errno_save = ACE_OS::last_error();
			ERROR("Call 'creat' failed for file '%s', error == %d", flag_path, errno_save);
			return -2;
		}
		ACE_OS::close(file_fd);
	}
	else {
		INFO("The restore flag (%s) already exists, nothing to do!", flag_path);
	}

	return 0;
}

/**
 * @brief	ReportEvent
 * @param 	specificProblem
 * @param 	severity
 * @param 	probableCause
 * @param 	objectOfReference
 * @param 	problemData
 * @param 	problemText
 * @return	ACE_INT32
 */
/*=====================================================================*/
ACE_INT32 ReportEvent(ACE_UINT32 specificProblem,
		const char* severity,
		const char* probableCause,
		const char* objectOfReference,
		const char* problemData,
		const char* problemText) ;
/*=====================================================================*/
/**
 * @brief	SuperviseGateway
 * @return	ACE_THR_FUNC_RETURN
 */
/*=====================================================================*/
ACE_THR_FUNC_RETURN SuperviseGateway(void* );
/*=====================================================================*/

/**
 * @brief	pingThreadCallbackFunc
 * @return	ACE_THR_FUNC_RETURN
 */
/*=====================================================================*/
ACE_THR_FUNC_RETURN pingThreadCallbackFunc(void *);
/*=====================================================================*/

/**
 * @brief	theThrStatusMap
 */
/*=====================================================================*/
//map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;
/*=====================================================================*/

/**
 * @brief	theThrMutex
 */
/*=====================================================================*/
//ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
/*=====================================================================*/

/*=====================================================================
  ROUTINE DECLARATION SECTION
  =====================================================================*/

/*=====================================================================
ROUTINE: ACS_NSF_Server
=====================================================================*/

ACS_NSF_Server::ACS_NSF_Server():
	m_poCommandHandler(0)
{

	/*=====================================================================
	  Default Initialization
	  =====================================================================*/
	DEBUG("%s","Entering server ACS_NSF_Server()");
	NSFEventList = 0;
	noOfGW = 0;
	noOfGW2 = 0;
	
	//ACE_OS::pipe(stopPingThreadFds);
	//ACE_OS::pipe(stopGatewayThreadFds);

	NSFFunctionalThreadId = 0;
	isNSFServerStopped = false;
	m_threadGroupId = -1;

	if (ACS_NSF_Server::objCommon.getNode() == NODE_A)
	{
		Clientportno = NODE_B_PORT;
		Serverportno =  NODE_A_PORT;
	}
	else if(ACS_NSF_Server::objCommon.getNode() == NODE_B)
	{
		Clientportno = NODE_A_PORT;
		Serverportno =  NODE_B_PORT;
	}
	threadManager_ =  new ACE_Thread_Manager();
	if( theNSFServerOmHandler.Init() != ACS_CC_SUCCESS )
	{
		ERROR("%s", "Initialization of OmHandler failed");

	}
	DEBUG("%s","Exiting server ACS_NSF_Server()");
}

/*=====================================================================
ROUTINE: ~ACS_NSF_Server
=====================================================================*/
ACS_NSF_Server::~ACS_NSF_Server()
{
	DEBUG("%s", "Entering ~ACS_NSF_Server()");
	// Destroy the mutex.
	m_srctMutex.remove();

	if (ACS_NSF_Server::objCommon.GetApgOamAccess() == 1)
	{
		// Close the stop event for cable less gateway threads.
		ACE_OS::close(gwInfo[0].stopEvent[0]);
		ACE_OS::close(gwInfo[0].stopEvent[1]);
		gwInfo[0].stopEvent[0] = -1;
		gwInfo[0].stopEvent[1] = -1;
	}
	else
	{
		// Close the stop event for gateway threads.
		for (ACE_UINT32 gw1 = 0; gw1 < noOfGW; gw1++)
		{
			DEBUG("Close the stop event for gateway: %d", gw1);
			ACE_OS::close(gwInfo[gw1].stopEvent[0]);
			ACE_OS::close(gwInfo[gw1].stopEvent[1]);
			gwInfo[gw1].stopEvent[0] = -1;
			gwInfo[gw1].stopEvent[1] = -1;
		}

		// Close the stop event for gateway threads for PS.
		for (ACE_UINT32 gw2 = 0; gw2 < noOfGW2; gw2++)
		{
			DEBUG("Close the stop event for gateway: %d", gw2);
			ACE_OS::close(gwInfo2[gw2].stopEvent[0]);
			ACE_OS::close(gwInfo2[gw2].stopEvent[1]);
			gwInfo2[gw2].stopEvent[0] = -1;
			gwInfo2[gw2].stopEvent[1] = -1;
		}
	}

	// Deallocate memory for Commandhandler.
	if (m_poCommandHandler != 0)
	{
		delete m_poCommandHandler;
		m_poCommandHandler = 0;
	}

	if (threadManager_ != 0)
	{
		delete threadManager_;
	}

	theNSFServerOmHandler.Finalize();

	DEBUG("%s", "Exiting ~ACS_NSF_Server()");
}

/*=====================================================================
ROUTINE: active 
=====================================================================*/
int ACS_NSF_Server::active(ACS_NSF_Service* haObj)
{
	ACS_CC_ReturnType myReturnErrorCode;
	m_haObj=haObj;
	isNSFServerStopped = false;
	ACE_OS::pipe(stopPingThreadFds);
	ACE_OS::pipe(stopGatewayThreadFds);
	myReturnErrorCode = ACS_NSF_Server::setupNSFThread(this);
	if(myReturnErrorCode == ACS_CC_FAILURE)
	{
		ERROR("%s", "NSF Application Thread was not started properly");
		return  -1;
	}
	sleep(2);
	return 0;
}
/*=====================================================================
ROUTINE: passive
=====================================================================*/
int ACS_NSF_Server::passive(ACS_NSF_Service* haObj)
{
	ACS_CC_ReturnType myReturnErrorCode;
	m_haObj=haObj;
	isNSFServerStopped = false;
	ACE_OS::pipe(stopPingThreadFds);
	ACE_OS::pipe(stopGatewayThreadFds);
	myReturnErrorCode = ACS_NSF_Server::setupNSFThread(this);
	if(myReturnErrorCode == ACS_CC_FAILURE)
	{
		ERROR("%s", "NSF Application Thread was not started properly");
		return  -1;
	}
	sleep(2);
	return 0;
}

/*=====================================================================
ROUTINE: stop 
=====================================================================*/
void ACS_NSF_Server::stop()
{
	DEBUG("%s","Entering ACS_NSF_Server::stop()");
	StopServer();
	sleep(2);	
	ACE_OS::close(stopPingThreadFds[0]);
	ACE_OS::close(stopPingThreadFds[1]);

	ACE_OS::close(stopGatewayThreadFds[0]);
	ACE_OS::close(stopGatewayThreadFds[1]);
	DEBUG("%s","Exiting ACS_NSF_Server::stop()");
}



/*=====================================================================
ROUTINE: SaveSystemTime
=====================================================================*/
ACE_INT32 ACS_NSF_Server::SaveSystemTime()
{
	// Open the NSF Timestamp file to save the current system time.
	// BEGIN TR:H053106
	ACE_HANDLE fd = ACE_OS::open(timeStampFileName.c_str(),O_CREAT | O_TRUNC | O_WRONLY );
	// END  TR:H053106

	// Fetching the current system time in Unix Epoch format
	time_t sysTime;
	sysTime = time(NULL);
	int systemTime = (uintmax_t)sysTime;
	std::string message (ACS_APGCC::itoa(systemTime));

	// Write the system time to the NSF timestamp file
	if (fd != ACE_INVALID_HANDLE)
	{
		ACE_OS::write(fd, message.c_str(), strlen(message.c_str()));
		INFO("Current system time saved in timestamp file (Unix Epocs secs) is  -  %s", message.c_str());
	}
	else
	{
		ERROR("%s", "Unable to save the system time");
		ACE_OS::close(fd);
		return -1;
	}

	// Close the handle of the timestamp file
	ACE_OS::close(fd);
	return 0;
}
/*=====================================================================
ROUTINE: GetDefaultGateways
=====================================================================*/
ACE_INT32 ACS_NSF_Server::GetDefaultGateways() 
{
	FILE *fp = 0;
	char myPubNetEthName[16], myGatewayBuff[512];

	std::string myPubNetEthNameCmd(INTERACE_NAME_PUBLIC_NETWORK_CMD);

	if (!(fp = popen(myPubNetEthNameCmd.c_str(), "r")))
	{
		ERROR("%s", "Error occurred while getting the interface name");
		return -1;
	}
	if (fscanf(fp, "%16s", myPubNetEthName )  <=  0)
	{
		ERROR("%s", "Error occurred while reading interface name");
		pclose(fp);
		return -1;
	}

	pclose(fp);

	DEBUG("Name of interface for public network is %s", myPubNetEthName);

	std::string routeCommand("");
	routeCommand.append("route -n | grep UG | grep ");
	routeCommand.append(myPubNetEthName);
	routeCommand.append(" | awk '{print $2}'");

	/* popen creates a pipe so we can read the output
	   of the program we are invoking */
	if (!(fp = popen(routeCommand.c_str(), "r")))
	{
		ERROR("%s", "Error while reading from popen");
		return -1;
	}

	/* clear the gateway vector before adding element */
	sGatewayVector.clear();
	/* read the output of route, one line at a time */
	while (fgets(myGatewayBuff, sizeof(myGatewayBuff), fp) != NULL)
	{
		string gatewayIP(myGatewayBuff);
		// Right trim
		gatewayIP.erase(gatewayIP.find_last_not_of(" \n\r\t") + 1);
		sGatewayVector.push_back(gatewayIP);
		DEBUG("Default Gateways - %s", gatewayIP.c_str());
	}

	/* close the pipe */
	pclose(fp);
	return 0;
} 
/*=====================================================================
ROUTINE: GetDefaultGateways2
=====================================================================*/
ACE_INT32 ACS_NSF_Server::GetDefaultGateways2() 
{
	FILE *fp = 0;

	char myPubNet2EthName[16], myGatewayBuff[512];

	std::string myPubNet2EthNameCmd(INTERACE_NAME_PUBLIC_NETWORK2_CMD);

	if (!(fp = popen(myPubNet2EthNameCmd.c_str(), "r")))
	{
		ERROR("%s", "Error occurred while getting the interface name");
		return -1;
	}

	if (fscanf(fp, "%16s", myPubNet2EthName)  <=  0)
	{
		ERROR("%s", "Error occurred while reading interface name");
		pclose(fp);
		return -1;
	}

	pclose(fp);

	DEBUG("Name of interface for public network 2 is %s", myPubNet2EthName);


	std::string routeCommand("");
	routeCommand.append("route -n | grep UG | grep ");
	routeCommand.append(myPubNet2EthName);
	routeCommand.append(" | awk '{print $2}'");

	/* popen creates a pipe so we can read the output
	   of the program we are invoking */
	if (!(fp = popen(routeCommand.c_str(), "r")))
	{
		ERROR("%s", "Error while reading from popen");
		return -1;
	}

	/* clear the gateway2 vector before adding element */
	sGatewayVector2.clear();
	/* read the output of route, one line at a time */
	while (fgets(myGatewayBuff, sizeof(myGatewayBuff), fp) != NULL)
	{
		string gatewayIP2(myGatewayBuff);
		// Right trim
		gatewayIP2.erase(gatewayIP2.find_last_not_of(" \n\r\t") + 1);
		sGatewayVector2.push_back(gatewayIP2);
		DEBUG("Default Gateways 2 - %s", gatewayIP2.c_str());
	}

	/* close the pipe */
	pclose(fp);
	return 0;
} 

/*=====================================================================
ROUTINE: GetPhaParameters
=====================================================================*/
ACE_INT32 ACS_NSF_Server::GetPhaParameters()
{
	// Fetch initial values from IMM
	ACS_APGCC_ImmAttribute attrPingPeriod;
	ACS_APGCC_ImmAttribute attrRouterResponse;
	ACS_APGCC_ImmAttribute attrRestTime;
	ACS_APGCC_ImmAttribute attrFailOvers;
	ACS_APGCC_ImmAttribute attrActiveFlag;
	ACS_APGCC_ImmAttribute attrFailoverPriority;

	attrPingPeriod.attrName = NSF_PING_PERIOD;
	attrRouterResponse.attrName = NSF_ROUTER_RESPONSE;
	attrRestTime.attrName = NSF_RESET_TIME;
	attrFailOvers.attrName = NSF_FAIL_OVERS;
	attrActiveFlag.attrName = NSF_ACTIVE_FLAG;
	attrFailoverPriority.attrName = NSF_FAILOVER_PRIORITY;

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
	std::vector<ACS_APGCC_ImmAttribute *> attributes1;
	attributes1.push_back(&attrFailoverPriority);
	attributes.push_back(&attrPingPeriod);
	attributes.push_back(&attrRouterResponse);
	attributes.push_back(&attrRestTime);
	attributes.push_back(&attrFailOvers);
	attributes.push_back(&attrActiveFlag);


	std::string dnName = ACS_NSF_Common::parentObjDNofNSF;
	ACS_CC_ReturnType enResult = ACS_CC_SUCCESS;
	if((enResult = (ACS_NSF_Server::objCommon.theOmHandler).getAttribute(dnName.c_str(),attributes1)) == ACS_CC_SUCCESS)	
	{
		if(attributes1[0]->attrName == NSF_FAILOVER_PRIORITY)
		{	
			ACS_NSF_Server::objCommon.failoverPriority = *(reinterpret_cast<ACE_INT32*>(attributes1[0]->attrValues[0]));
		}
	}
	else
	{
		ERROR("Unable to fetch attribute values from %s", dnName.c_str());
		return -1;
	}

	std::string nsfParamDN = ACS_NSF_PARAM_OBJ_DN; 
	nsfParamDN.append(",");
	nsfParamDN.append(ACS_NSF_Common::parentObjDNofNSF);

	if((enResult = (ACS_NSF_Server::objCommon.theOmHandler).getAttribute(nsfParamDN.c_str(),attributes)) == ACS_CC_SUCCESS)
	{
		for(int i=0; i<(int)attributes.size(); i++)
		{
			if (attributes[i]->attrName == NSF_PING_PERIOD )
			{
				ACS_NSF_Server::objCommon.pingPeriod = *(reinterpret_cast<ACE_UINT32*>(attributes[i]->attrValues[0]));
			}
			else if ( attributes[i]->attrName == NSF_ROUTER_RESPONSE )
			{
				ACS_NSF_Server::objCommon.routerResponse = *(reinterpret_cast<ACE_UINT32*>(attributes[i]->attrValues[0]));
			}
			else if ( attributes[i]->attrName == NSF_FAIL_OVERS )
			{
				ACS_NSF_Server::objCommon.maxFailoverAttempts = *(reinterpret_cast<ACE_UINT32*>(attributes[i]->attrValues[0]));
			}
			else if ( attributes[i]->attrName == NSF_RESET_TIME )
			{ 
				ACS_NSF_Server::objCommon.resetTime = *(reinterpret_cast<ACE_UINT32*>(attributes[i]->attrValues[0]));
			}
			else if ( attributes[i]->attrName == NSF_ACTIVE_FLAG )
			{
				ACS_NSF_Server::objCommon.isSurveillanceEnabled = *(reinterpret_cast<ACE_INT32*>(attributes[i]->attrValues[0]));
			}
		}
	}
	else
	{
		ERROR("Unable to fetch attribute values from %s", dnName.c_str());
		return -1;
	}

	return 0;
}
/*=====================================================================
ROUTINE: GetConfigParameters
=====================================================================*/
ACE_INT32 ACS_NSF_Server::GetConfigParameters()
{
	// Fetch initial values from IMM
	ACS_APGCC_ImmAttribute attrNetworkSurveillanceFlag;
	ACS_APGCC_ImmAttribute attrNumberFailover;

	attrNetworkSurveillanceFlag.attrName = NSF_SURVELLIENCEACTIVEFLAG;
	attrNumberFailover.attrName = NSF_NOOFFAILOVER;

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
	attributes.push_back(&attrNetworkSurveillanceFlag);
	std::vector<ACS_APGCC_ImmAttribute *> attributes1;
	attributes1.push_back(&attrNumberFailover);

	std::string dnName = ACS_NSF_Common::parentObjDNofNSF;

	ACS_CC_ReturnType enResult = ACS_CC_SUCCESS;

	if((enResult = ((ACS_NSF_Server::objCommon).theOmHandler).getAttribute(dnName.c_str(),attributes)) == ACS_CC_SUCCESS)
	{
		if (attributes[0]->attrName == NSF_SURVELLIENCEACTIVEFLAG )
		{
			ACS_NSF_Server::objCommon.isSurveillanceActive = *(reinterpret_cast<ACE_INT32 *>(attributes[0]->attrValues[0]));
		}
	}
	else
	{
		ERROR("Unable to fetch attribute values from %s", dnName.c_str());
		return -1;
	}

	std::string paramDnName = ACS_NSF_PARAM_OBJ_DN;
	paramDnName.append(",");
	paramDnName.append(ACS_NSF_Common::parentObjDNofNSF);
	if((enResult = ((ACS_NSF_Server::objCommon).theOmHandler).getAttribute(paramDnName.c_str(),attributes1)) == ACS_CC_SUCCESS)
	{
		if ( attributes1[0]->attrName == NSF_NOOFFAILOVER )
		{
			ACS_NSF_Server::objCommon.failoverCount = *(reinterpret_cast<ACE_UINT32 *>(attributes1[0]->attrValues[0]));
		}
	}
	else
	{
		ERROR("Unable to fetch attribute values from %s", dnName.c_str());
		return -1;
	}

	//DEBUG("%s", "Parameters are:");
	INFO("ACS_NSF_Server - isSurveillanceActive  -  %d ", ACS_NSF_Server::objCommon.isSurveillanceActive);
	INFO("ACS_NSF_Server - failoverCount -  %u ", ACS_NSF_Server::objCommon.failoverCount);

	return 0;
}

/*=====================================================================
ROUTINE: SetDefaultNetworkSurveillanceValue
=====================================================================*/
ACE_UINT32 ACS_NSF_Server::SetDefaultNetworkSurveillanceValue(ACE_UINT32 aSurveillanceActiveFlag)
{
	DEBUG("%s", "Entering SetDefaultNetworkSurveillanceValue");

	int retCode = 0;
	ACE_UINT32 dummy = 0;
	int result = 0;

	//Acquire mutex.

	m_srctMutex.acquire();

	unsigned int myStartStopValue = 0;

	//Get NSF_SURVELLIENCEACTIVEFLAG.

	result = ACS_NSF_Server::objCommon.getRegWord(NSF_SURVELLIENCEACTIVEFLAG,dummy);
	//DEBUG( "SESHU : NSF_SURVELLIENCEACTIVEFLAG : %d", dummy);

	if (result == 1) // Value does not exist
	{
		myStartStopValue = 0; // stopped, passive
		if (aSurveillanceActiveFlag == 1)
		{
			myStartStopValue = 3; // started, active
		}
		ACS_NSF_Server::objCommon.setInitialSetFlag(true);

		result = ACS_NSF_Server::objCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG, 1);

		ACS_NSF_Server::objCommon.setInitialSetFlag(false);
		ACS_NSF_Server::objCommon.setStartStopValue(myStartStopValue);
		DEBUG( "NSF_SURVELLIENCEACTIVEFLAG Value does not exist, Hence the default value 1 is set. Here  myStartStopValue: %d", myStartStopValue);
	}
	else
	{
		if (result == 0)
		{
			/*if (aSurveillanceActiveFlag == 1)
			  {
			  myStartStopValue = 3; // started, active
			  ACS_NSF_Server::objCommon.setInitialSetFlag(true);
			  result = ACS_NSF_Server::objCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG,1);
			  ACS_NSF_Server::objCommon.setInitialSetFlag(false);
			  }*/
			//else
			//{
			//Maintain the previous state
			if( dummy == 1)
			{
				ACS_NSF_Server::objCommon.setInitialSetFlag(true);
				result = ACS_NSF_Server::objCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG,1);
				ACS_NSF_Server::objCommon.setInitialSetFlag(false);

				if (aSurveillanceActiveFlag == 1)
					myStartStopValue = 3;
				else
					myStartStopValue = 2;
				DEBUG( "NSF_SURVELLIENCEACTIVEFLAG is set to 1 and myStartStopvalue is: %d", myStartStopValue);
			}
			else
			{
				ACS_NSF_Server::objCommon.setInitialSetFlag(true);
				result = ACS_NSF_Server::objCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG,0);
				ACS_NSF_Server::objCommon.setInitialSetFlag(false);

				if (aSurveillanceActiveFlag == 1)
					myStartStopValue = 1;
				else
					myStartStopValue = 0;
				DEBUG( "NSF_SURVELLIENCEACTIVEFLAG is set to 0 and myStartStopvalue is: %d", myStartStopValue);

			}
			//}
			ACS_NSF_Server::objCommon.setStartStopValue(myStartStopValue);
		}
	}
	if (result != 0)
	{
		retCode = -1;
	}
	m_srctMutex.release();
	return retCode;
}

/*=====================================================================
ROUTINE: SetDefaultFailoverCount
=====================================================================*/
ACE_UINT32 ACS_NSF_Server::SetDefaultFailoverCount()
{
	DEBUG("%s", "Entering SetDefaultFailoverCount");

	int retCode = 0;
	ACE_UINT32 dummy = 0;
	int result;

	//Acquire mutex
	m_srctMutex.acquire();

	result = ACS_NSF_Server::objCommon.getRegWord(NSF_NOOFFAILOVER,dummy);
	if ( result == 1) // Value does not exist
	{
		result = ACS_NSF_Server::objCommon.setRegWord(NSF_NOOFFAILOVER,0);
		if ( result != 0 )
		{
			ERROR("%s", "Value is not set to the IMM.");
			retCode = -1;
		}
	}
	else
	{
		DEBUG("setInitialRegistryValues (to IMM): NSF_NOOFFAILOVER = %d\n",dummy);
	}
	if (result != 0)
	{
		retCode = -1;
	}

	//Release the mutex.
	m_srctMutex.release();

	if( retCode == -1)
	{
		ERROR("%s", "Error occured while setting failover count");
		ERROR("%s", "Exiting SetDefaultFailoverCount");
	}
	else
	{
		DEBUG("%s", "Exiting SetDefaultFailoverCount");
	}

	return retCode;
}

/*=====================================================================
ROUTINE: SetDefaultLastFailoverTime
=====================================================================*/
ACE_UINT32 ACS_NSF_Server::SetDefaultLastFailoverTime()
{
	DEBUG("%s", "Entering SetDefaultLastFailoverTime");
	int result;
	int retCode = 0;

	//Acquire mutex.
	m_srctMutex.acquire();

	ACE_UINT32 nsfTime = 0;

	result = ACS_NSF_Server::objCommon.getRegTime(NSF_LASTFAILOVERTIME,nsfTime);
	if (result == 1) // Value does not exist
	{
		result = ACS_NSF_Server::objCommon.setRegTime(NSF_LASTFAILOVERTIME);
	}
	if (result != 0)
	{
		retCode = -1;
	}

	//Release mutex.
	m_srctMutex.release();
	if( retCode < 0)
	{
		ERROR("%s", "Exiting SetDefaultFailoverTime");
	}
	else
	{
		DEBUG("%s", "Exiting SetDefaultFailoverTime");
	}
	return retCode;
}

/*=====================================================================
ROUTINE: SetDefaultAlarmState
=====================================================================*/
ACE_UINT32 ACS_NSF_Server::SetDefaultAlarmState(NODE_NAME currentNode)
{
	ACE_UINT32 dummy = 0;
	int result;
	int retCode = 0;
	//Acquire the mutex.

	m_srctMutex.acquire();

	if (currentNode == NODE_A) // A-node
	{
		result = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,dummy);
		std::string strAlarmStateA = (ACS_APGCC::itoa(dummy));
		if (result == 1)
		{
			result = ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_A,0);
		}
		else
		{
			DEBUG("setInitialRegistryValues (to IMM): NSF_ALARMSTATE_A = %s",strAlarmStateA.c_str());
		}
		if (result != 0)
		{
			retCode = -1;
		}
	}
	else if(currentNode == NODE_B) //B-Node
	{
		result = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,dummy);
		std::string strAlarmStateB = (ACS_APGCC::itoa(dummy));
		if (result == 1)
		{
			result = ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_B,0);
		}
		else
		{
			DEBUG("setInitialRegistryValues (to IMM): NSF_ALARMSTATE_B = %s",strAlarmStateB.c_str());
		}
		if (result != 0)
		{
			retCode = -1;
		}
	}

	if (  currentNode == NODE_A )	//A-Node
	{
		result = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A, dummy );
		std::string strAlarmStateNet2A = (ACS_APGCC::itoa(dummy));
		if( result == 1)
		{
			result = ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_NET2_A, 0);
		}
		else
		{
			DEBUG("Set Default Alarm State : NSF_ALARMSTATE_NET2_A = %s", strAlarmStateNet2A.c_str() );
		}
		if( result != 0 )
		{
			retCode = -1;
		}
	}
	else if (  currentNode == NODE_B )	//A-Node
	{
		result = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B, dummy );
		std::string strAlarmStateNet2B = (ACS_APGCC::itoa(dummy));
		if( result == 1)
		{
			result = ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_NET2_B, 0);
		}
		else
		{
			DEBUG("Set Default Alarm State : NSF_ALARMSTATE_NET2_B = %s", strAlarmStateNet2B.c_str() );
		}
		if( result != 0 )
		{
			retCode = -1;
		}
	}
	//Release the mutex.

	m_srctMutex.release();

	return retCode;
}
/*=====================================================================
ROUTINE: SetDefaultIMMParams
=====================================================================*/
ACE_INT32 ACS_NSF_Server::SetDefaultIMMParams(ACE_UINT32 aSurveillanceActiveFlag)
{
	DEBUG("%s", "Entered the SetDefaultIMMParams function.");

	m_srctMutex.acquire();
	int retCode = 0;

	//Set DefaultFailoverCount
	retCode =  SetDefaultFailoverCount();
	if( retCode == -1)
	{
		ERROR("%s", "Error occurred while setting DefaultFailoverCount");
		m_srctMutex.release();
		ERROR("%s", "Exited the SetDefaultIMMParams function..");
		return retCode;
	}

	//Set NetworkSurveillanceValue
	retCode =  SetDefaultNetworkSurveillanceValue(aSurveillanceActiveFlag);
	if( retCode == -1)
	{
		ERROR("%s", "Error occurred while setting NetworkSurveillanceValue");
		m_srctMutex.release();
		ERROR("%s", "Exited the SetDefaultIMMParams function..");
		return retCode;
	}

	//Set DefaultLastFailoverTime
	retCode =  SetDefaultLastFailoverTime();
	if( retCode == -1)
	{
		ERROR("%s", "Error occurred while setting LastFailoverTime");
		m_srctMutex.release();
		ERROR("%s", "Exited the SetDefaultIMMParams function..");
		return retCode;
	}

	m_srctMutex.release();
	DEBUG("%s", "Exited the SetDefaultIMMParams function..");

	return retCode;
} 
/*=====================================================================
ROUTINE: CeaseAllAlarms
=====================================================================*/
int ACS_NSF_Server::CeaseAllAlarms(ACE_UINT32 state)
{
	DEBUG("%s", "Entered CeaseAllAlarms");
	ACE_UINT32 alarmState = 0;
	// B0 = O2 NSF is deactivated
	// B1 = A1 AP external network fault. All GW both nodes
	// B2 = A2 AP external network fault. All GW active node
	// B3  = A2 AP external network fault. GW 0
	// B4  = A2 AP external network fault. GW 1
	// ...
	// B12  = A2 AP external network fault. GW 9
	int r;
	if (currentNode == NODE_A) // A-node
	{
		r = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,alarmState);
	}
	else // B-node
	{
		r = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,alarmState);
	}
	if (r != 0) // Cannot retrieve alarmState.
	{
		ERROR("%s", "Exiting CeaseAllAlarms!! Unable to retrieve alarm state.");
		return -1;
	}
	ACE_UINT32 mask = 8;
	for (unsigned int i = 0; i < noOfGW; i++)
	{
		if ((alarmState & mask) != 0)
		{
			CeaseAlarm(gwInfo[i].IPAddrStr,i+3,currentNode);
			DEBUG("Ceasing alarm on position %d", (i+3));
		}
		mask = mask << 1;
	} // End of for-loop

	if ((alarmState & 0X04) != 0) // A2 AP external network fault. All GW active node
	{
		CeaseAlarm("",2,currentNode); // Ignore error
		DEBUG("%s", "Ceasing A2 alarm");
	}
	if ((alarmState & 0X02) != 0) // A1 AP external network fault. All GW both nodes
	{
		CeaseAlarm("",1,currentNode); // Ignore error
		DEBUG("%s", "Ceasing A1 alarm");
	}
	if (((alarmState & 0X01) != 0) && (state != 1)) // O2 NSF is deactivated
	{
		CeaseAlarm("",0,currentNode); // Ignore error
		DEBUG("%s", "Ceasing O2 alarm");
	}
	DEBUG("%s", "Exiting CeaseAllAlarms");
	return 0;

} 

/*=====================================================================
ROUTINE: CeaseAllAlarms2
=====================================================================*/
int ACS_NSF_Server::CeaseAllAlarms2(ACE_UINT32 state)
{
	DEBUG("%s", "Entered CeaseAllAlarms2");
	ACE_UINT32 alarmState2 = 0;
	// B0 = O2 NSF is deactivated
	// B1 = A1 AP external network fault. All GW both nodes
	// B2 = A2 AP external network fault. All GW active node
	// B3  = A2 AP external network fault. GW 0
	// B4  = A2 AP external network fault. GW 1
	// ...
	// B12  = A2 AP external network fault. GW 9
	int r;
	if (currentNode == NODE_A) // A-node
	{
		r = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A,alarmState2);
	}
	else // B-node
	{
		r = ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B,alarmState2);
	}
	if (r != 0) // Cannot retrieve alarmState.
	{
		ERROR("%s", "Exiting CeaseAllAlarms!! Unable to retrieve alarm state.");
		return -1;
	}
	ACE_UINT32 mask = 8;
	for (unsigned int i = 0; i < noOfGW2; i++)
	{
		if ((alarmState2 & mask) != 0)
		{
			CeaseAlarm2(gwInfo2[i].IPAddrStr,i+3,currentNode);
			DEBUG("Ceasing alarm on position %d", (i+3));
		}
		mask = mask << 1;
	} // End of for-loop

	if ((alarmState2 & 0X04) != 0) // A2 AP external network fault. All GW active node
	{
		CeaseAlarm2("",2,currentNode); // Ignore error
		DEBUG("%s", "Ceasing A2 alarm");
	}
	if ((alarmState2 & 0X02) != 0) // A1 AP external network fault. All GW both nodes
	{
		CeaseAlarm2("",1,currentNode); // Ignore error
		DEBUG("%s", "Ceasing A1 alarm");
	}
	if (((alarmState2 & 0X01) != 0) && (state != 1)) // O2 NSF is deactivated
	{
		CeaseAlarm2("",1,currentNode); // Ignore error
		DEBUG("%s", "Ceasing O2 alarm");
	}
	DEBUG("%s", "Exiting CeaseAllAlarms2");
	return 0;

} 

/*=====================================================================
ROUTINE: TerminateGatewaySup
=====================================================================*/
void ACS_NSF_Server::TerminateGatewaySup(int forceTermination)
{
	DEBUG("%s", "Entered NetworkSurveillance: TerminateGatewaySup");
	char buf[] = {1 , 1};
	int bytes = 0;
	
	if (ACS_NSF_Server::objCommon.GetApgOamAccess() == 1)
	{
		bytes = ACE_OS::write(gwInfo[0].stopEvent[1], buf, sizeof(buf));
		if (bytes <= 0)
		{
			ERROR("Error occurred while signalling stop event for bond%d", 1);
			ERROR("%s", "Leaving StopServer");
			return;
		}
	}
	else
	{
		for (ACE_UINT32 gw1 = 0; gw1 < noOfGW; gw1++)
		{
			DEBUG("Signalling stop event for gateway: %d", gw1);

			bytes = ACE_OS::write(gwInfo[gw1].stopEvent[1], buf, sizeof(buf));
			if (bytes <= 0)
			{
				ERROR("Error occurred while signalling stop event for network 1 with index %lu", gw1);
				ERROR("%s", "Leaving StopServer");
				return;
			}
		}

		for (ACE_UINT32 gw2 = 0; gw2 < noOfGW2; gw2++)
		{
			DEBUG("Signalling stop event for gateway: %d", gw2);
		
			bytes = ACE_OS::write(gwInfo2[gw2].stopEvent[1], buf, sizeof(buf));
			if (bytes <= 0)
			{
				ERROR("Error occurred while signalling stop event for network 2 with index %lu", gw2);
				ERROR("%s", "Leaving StopServer");
				return;
			}
		}

		if (forceTermination != 0) // We dont have time to wait
		{
			DEBUG("%s", "NetworkSurveillance: TerminateGatewaySup forceTermination");
			CeaseAllAlarms2(0);
			CeaseAllAlarms(0);		

			DEBUG("%s", "Exiting NetworkSurveillance: TerminateGatewaySup");
			return;
		}
	}
	
	DEBUG("%s", "Exiting NetworkSurveillance: TerminateGatewaySup");
	return;
}

/*=====================================================================
ROUTINE: TryToFailoverForPS
=====================================================================*/
void ACS_NSF_Server::TryToFailoverForPS( ACE_UINT32 activeAlarm, 
		ACE_UINT32 activeAlarm2)
{
	// Description:
	//	This routine trigger the Failover if possible
	//  when Physical Separation is configured.

	ACE_UINT32 failoverCount = 65535;
	std::string errorMsg = "";
	failoverCount = ACS_NSF_Server::objCommon.failoverCount;

	if( failoverCount < ACS_NSF_Server::objCommon.maxFailoverAttempts )
	{
		DEBUG("NetworkSurveillance: failoverCount = %d", failoverCount);
		int heartBeat = 0;
		int attempts = 0;
		// Try 4 attempts to verify NSF is running on the other node or not
		do
		{
			heartBeat = VerifyHeartBeat();
			DEBUG("NetworkSurveillance: VerifyHeartBeat = %d", heartBeat);

		}
		while((heartBeat == 0) && (attempts++ < 4));

		if(heartBeat == 1)
		{
			if((activeAlarm & 0X02) != 0)
			{
				CeaseAlarm("",1,currentNode); // 	cease A1 for Public network
			}
			if((activeAlarm2 & 0X02) != 0)
			{
				CeaseAlarm2("",1,currentNode); // 	cease A1 for Public_2 network
			}
			DoFailover(); // Will never return
		}
		else
		{
			errorMsg = "NSF Server unavailable on other node";
		}
	}
	else
	{   
		DEBUG("NetworkSurveillance: failoverCount = %d", failoverCount);
		errorMsg = "too many failovers";
	}

	if(errorMsg.size() > 0)
	{
		errorMsg = "Not allowed to failover due to "+ errorMsg; 
		ACS_NSF_Server::objCommon.reportError(3, errorMsg.c_str());
		if ((activeAlarm & 0X02) != 0)
		{
			CeaseAlarm("", 1, currentNode); // 	cease A1 for Public network
		}
		if ((activeAlarm2 & 0X02) != 0)
		{
			CeaseAlarm2("", 1, currentNode); // 	cease A1 for Public_2 network
		}
	}

}//End TryToFailoverForPS

/*=====================================================================
ROUTINE: CheckIfFailoverForPS
=====================================================================*/
bool ACS_NSF_Server::CheckIfFailoverForPS()
{
	DEBUG("%s", "Entering CheckforfailoversforPS");

	// Description:
	//	This routine start/cease A1 and A2 alarms on 
	//	both networks, and handle the failover algorithm
	//  when Physical Separation is configured.

	ACE_UINT32 A_alarm;
	ACE_UINT32 B_alarm;
	ACE_UINT32 A_alarm_2;
	ACE_UINT32 B_alarm_2;

	bool AllGatewayBlockedActive = false;
	bool AllGatewayBlockedPassive = false;
	bool AllGateway2BlockedActive = false;
	bool AllGateway2BlockedPassive = false;
	bool AllPrimaryGWBlockedActive = false;
	bool AllPrimaryGWBlockedPassive = false;
	bool AllSecondaryGWBlockedActive = false;
	bool AllSecondaryGWBlockedPassive = false;

	// Handle Alarms for Public and Public_2 Network
	if((ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A, A_alarm) == 0) &&
			(ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B, B_alarm) == 0) &&
			(ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A, A_alarm_2) == 0) &&
			(ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B, B_alarm_2) == 0)) 
	{
		DEBUG("%s", "CheckforfailoverforPS: Parameters successfully fetched");

		ACE_UINT32 activeAlarm = A_alarm;
		ACE_UINT32 passiveAlarm = B_alarm;
		ACE_UINT32 activeAlarm2 = A_alarm_2;
		ACE_UINT32 passiveAlarm2 = B_alarm_2;

		if(ACS_NSF_Server::objCommon.getActiveNodeFlag())
		{
			if ( currentNode == NODE_A  )
			{
				activeAlarm = A_alarm;
				passiveAlarm = B_alarm;
				activeAlarm2 = A_alarm_2;
				passiveAlarm2 = B_alarm_2;
			}
			else if ( currentNode == NODE_B )
			{
				activeAlarm = B_alarm;
				passiveAlarm = A_alarm;
				activeAlarm2 = B_alarm_2;
				passiveAlarm2 = A_alarm_2;
			}
		}
		else
		{
			if ( currentNode == NODE_A  )
			{
				activeAlarm = B_alarm;
				passiveAlarm = A_alarm;
				activeAlarm2 = B_alarm_2;
				passiveAlarm2 = A_alarm_2;
			}
			else if ( currentNode == NODE_B )
			{
				activeAlarm = A_alarm;
				passiveAlarm = B_alarm;
				activeAlarm2 = A_alarm_2;
				passiveAlarm2 = B_alarm_2;
			}
		}

		int active = 0; // 0=passive,1=active
		if (ACS_NSF_Server::objCommon.getActiveNodeFlag() == true )
		{
			active = 1; // 0=passive,1=active
		}

		if(!ACS_NSF_Server::objCommon.calculateStartStopValue(ACS_NSF_Server::objCommon.isSurveillanceEnabled))
		{
			ERROR("%s", "Error occured while calculating with start/stop value.");
			ERROR("%s", "Leaving CheckIfFailoverForPS");
			return false;
		}
		ACE_UINT32 startStopFlag = ACS_NSF_Server::objCommon.getStartStopValue();


		//Raise or cease A1 alarm For Public Network.
		if(	(CountBlockedGw(activeAlarm, noOfGW) == noOfGW) &&
				(CountBlockedGw(passiveAlarm, noOfGW) == noOfGW))
		{
			//All gateways blocked.
			//If NSF active and started, active node, and A1 alarm not raised,
			//raise A1 alarm.

			if((startStopFlag == 3) && (active == 1) && ((activeAlarm & 0X02) == 0))
			{
				SendAlarm("", 1, currentNode, 0);
			}

			DEBUG("%s","CheckforfailoverforPS: All gateways are blocked on both nodes for public1");

		}
		else
		{
			//Not all gateways blocked.
			//If A1 alarm raised in node, cease A1 alarm.
			if((currentNode == NODE_A) && ((A_alarm & 0X02) != 0))
			{
				CeaseAlarm("",1,currentNode);
			}
			if((currentNode == NODE_B) && ((B_alarm & 0X02) != 0))
			{
				CeaseAlarm("",1,currentNode);
			}
			DEBUG("%s", "CheckforfailoverforPS: NOT All gateways are blocked on both nodes for public1 ");

		}

		//Raise or cease A1 alarm For Public_2 Network.
		if(	(CountBlockedGw(activeAlarm2, noOfGW2) == noOfGW2) &&
				(CountBlockedGw(passiveAlarm2, noOfGW2) == noOfGW2))
		{
			//All gateways blocked for Network 2.
			//If NSF active and started, active node, and A1 alarm not raised,
			//raise A1 alarm.

			if((startStopFlag == 3) && (active == 1) && ((activeAlarm2 & 0X02) == 0))
			{
				SendAlarm2("", 1, currentNode, 0);
			}
			DEBUG("%s", "CheckforfailoverforPS: All gateways are blocked on both nodes for public2 ");

		}
		else
		{
			//Not all gateways blocked on Network 2.
			//If A1 alarm raised in node, cease A1 alarm.
			if((currentNode == NODE_A) && ((A_alarm_2 & 0X02) != 0))
			{
				CeaseAlarm2("",1,currentNode);
			}
			if((currentNode == NODE_B) && ((B_alarm_2 & 0X02) != 0))
			{
				CeaseAlarm2("",1,currentNode);
			}
			DEBUG("%s", "CheckforfailoverforPS: NOT All gateways are blocked on both nodes for public2 ");

		}

		if((active == 1) && ((startStopFlag & 1) == 1))
		{

			if ((CountBlockedGw(activeAlarm, noOfGW) == noOfGW) &&
					(CountBlockedGw(passiveAlarm, noOfGW) < noOfGW))
			{
				ACE_OS::sleep(ACS_NSF_Server::objCommon.pingPeriod);
				ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,A_alarm);
				ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,B_alarm);

				// Added for copying with TR HL37087 also for Physical Network impacts
				ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A, A_alarm_2); 
				// Added for copying with TR HL37087 also for Physical Network impacts
				ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B, B_alarm_2); 

				//Added for copying with TR HL37087 also for Physical Network impacts
				activeAlarm = A_alarm;
				passiveAlarm = B_alarm;
				activeAlarm2 = A_alarm_2;
				// Added for copying with TR HL37087 also for Physical Network impacts
				passiveAlarm2 = B_alarm_2;

				if (currentNode == NODE_B)
				{
					activeAlarm = B_alarm;
					passiveAlarm = A_alarm;
					activeAlarm2 = B_alarm_2;// Added for copying with TR HL37087 also for Physical Network impacts
					passiveAlarm2 = A_alarm_2;// Added for copying with TR HL37087 also for Physical Network impacts
				}
			}

			DEBUG("%s", "CheckforfailoverforPS: Check if a failover is needed for Public Network ");


			// Check if a failover is needed for Public Network
			if( CountBlockedGw(activeAlarm, noOfGW) == noOfGW)
			{
				// All gateways on current active for Public Network node are blocked
				if((activeAlarm & 0X04) == 0)
				{
					SendAlarm("",2,currentNode, 0); // send A2 all GW on active blocked
					DEBUG("%s", "CheckforfailoverforPS: A2 alarm sent after sleep for network 1");

				}

				AllGatewayBlockedActive = true;

				DEBUG("%s", "CheckforfailoverforPS: Public Network has all gateways blocked on Active Node");


			}
			else
			{
				if((activeAlarm & 0X04) != 0) // A2 all gateways on active node Public Network
				{
					CeaseAlarm("", 2, currentNode);  // cease A2
					DEBUG("%s", "CheckforfailoverforPS: A2 alarm ceased after sleep for network 1");

				}
				DEBUG("%s", "CheckforfailoverforPS: Public Network has NOT all gateways blocked on Active Node");

			}


			//  TR HM54770: moved externally to the IF statement BEGIN

			if( CountBlockedGw(passiveAlarm, noOfGW) == noOfGW)
			{
				if(CountBlockedGw(activeAlarm, noOfGW) == noOfGW)
				{	
					if((activeAlarm & 0X02) == 0)
					{
						SendAlarm("", 1, currentNode,0); // send A1 both nodes blocked
						DEBUG("%s", "CheckforfailoverforPS: A1 alarm sent after sleep for network 1");

					}

				}
				AllGatewayBlockedPassive = true;

				DEBUG("%s", "CheckforfailoverforPS: Public Network has all gateways blocked on Passive Node");

			}
			else
			{
				DEBUG("%s", "CheckforfailoverforPS: Public Network has NOT all gateways blocked on Passive Node");

			}

			// Check if a failover is needed for Public_2 Network
			//DEBUG("%s", "CheckforfailoverforPS: Check if a failover is needed for Public_2 Network ");

			if( CountBlockedGw(activeAlarm2, noOfGW2) == noOfGW2)
			{
				// All gateways on current active for Public_2 Network node are blocked
				if((activeAlarm2 & 0X04) == 0)
				{
					SendAlarm2("",2,currentNode,0); // send A2 all GW on active blocked
					DEBUG("%s", "CheckforfailoverforPS: A2 alarm sent after sleep for network 2");
				}

				AllGateway2BlockedActive = true;

				DEBUG("%s", "CheckforfailoverforPS: Public_2 Network has all gateways blocked on Active Node");
			}
			else
			{
				if((activeAlarm2 & 0X04) != 0) // A2 all gateways on active node Public_2 Network
				{
					CeaseAlarm2("", 2, currentNode);  // cease A2
					DEBUG("%s", "CheckforfailoverforPS: A2 alarm ceased after sleep for network 2");

				}
				DEBUG("%s", "CheckforfailoverforPS: Public_2 Network has NOT all gateways blocked on Active Node");

			}

			if( CountBlockedGw(passiveAlarm2, noOfGW2) == noOfGW2)
			{
				if( CountBlockedGw(activeAlarm2, noOfGW2) == noOfGW2)
				{
					if((activeAlarm2 & 0X02) == 0)
					{
						SendAlarm2("",1,currentNode,0); // send A1 both nodes blocked
						DEBUG("%s", "CheckforfailoverforPS: A1 alarm sent after sleep for network 2");
					}
				}

				AllGateway2BlockedPassive = true;

				DEBUG("%s", "CheckforfailoverforPS: Public_2 Network has all gateways blocked on Passive Node");
			}
			else
			{

				DEBUG("%s", "CheckforfailoverforPS: Public_2 Network has NOT all gateways blocked on Passive Node");

			}	

			//  TR HM54770: moved externally to the IF statement END


			AllPrimaryGWBlockedActive  = AllGatewayBlockedActive;
			AllPrimaryGWBlockedPassive = AllGatewayBlockedPassive;

			AllSecondaryGWBlockedActive  = AllGateway2BlockedActive;
			AllSecondaryGWBlockedPassive = AllGateway2BlockedPassive;

			if (ACS_NSF_Server::objCommon.failoverPriority == 1) 
			{
				AllPrimaryGWBlockedActive  = AllGateway2BlockedActive;
				AllPrimaryGWBlockedPassive = AllGateway2BlockedPassive;

				AllSecondaryGWBlockedActive  = AllGatewayBlockedActive;
				AllSecondaryGWBlockedPassive = AllGatewayBlockedPassive;

				DEBUG("%s", "CheckforfailoverforPS: failoverPriority == 1 public_2 has priority");
			}
			else
			{

				DEBUG("CheckforfailoverforPS: failoverPriority has value: %d", ACS_NSF_Server::objCommon.failoverPriority);
			}

			DEBUG("%s", "CheckforfailoverforPS: Comparing public1 and public 2 with values:");

			DEBUG("CheckforfailoverforPS: AllPrimaryGWBlockedActive = %d", AllPrimaryGWBlockedActive);
			DEBUG("CheckforfailoverforPS: AllPrimaryGWBlockedPassive = %d", AllPrimaryGWBlockedPassive);
			DEBUG("CheckforfailoverforPS: AllSecondaryGWBlockedActive = %d", AllSecondaryGWBlockedActive);
			DEBUG("CheckforfailoverforPS: AllSecondaryGWBlockedPassive = %d", AllSecondaryGWBlockedPassive);

			// Check if a failover is needed

			if ((AllPrimaryGWBlockedActive == true) && (AllPrimaryGWBlockedPassive == false))
			{
				TryToFailoverForPS(activeAlarm,activeAlarm2);
				DEBUG("%s", "NetworkSurveillance: Try to failover Primary GW blocked on Active Node");
			}

			if ((AllPrimaryGWBlockedActive == false) && 
					(AllSecondaryGWBlockedActive == true) &&
					(AllPrimaryGWBlockedPassive == false) && 
					(AllSecondaryGWBlockedPassive == false))
			{
				TryToFailoverForPS(activeAlarm,activeAlarm2);		// Try to Failover
				DEBUG("%s", "NetworkSurveillance: Try to failover Primary GW OK but Secondary GW blocked on Active Node");
			}

			if ((AllPrimaryGWBlockedActive == true) && 
					(AllPrimaryGWBlockedPassive == true) &&
					(AllSecondaryGWBlockedActive == true) &&
					(AllSecondaryGWBlockedPassive == false))
			{
				TryToFailoverForPS(activeAlarm,activeAlarm2);		//Try to Failover
				DEBUG("%s", "NetworkSurveillance: Try to failover Primary GW blocked on both nodes, Secondary GW blocked on Active Node but working on Passive");
			}

		}
		else // Passive node or passive supervision
		{
			if((active == 1) && (activeAlarm & 0X04) != 0) // A2 all gateways on active node Public Network
			{
				CeaseAlarm("", 2, currentNode);  // cease A2
			}
			if((active == 1) && (activeAlarm2 & 0X04) != 0) // A2 all gateways on active node Public_2 Network
			{
				CeaseAlarm2("", 2, currentNode);  // cease A2
			}
			DEBUG("%s", "CheckforfailoverforPS: Passive node or passive supervision no actions but ceasing alarms if needed");

		}
	}
	DEBUG("%s", "Leaving CheckIfFailoverForPS");
	return true;
}  //End CheckIfFailoverForPS




/*=====================================================================
ROUTINE: PingThread
=====================================================================*/
ACE_UINT32 ACS_NSF_Server::PingThread() 
{

	DEBUG("%s", "Entering ACS_NSF_Server::PingThread()");

	int sockfd = 0, newsockfd = 0, portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0)
	{
		ERROR("Error occured while opening server socket %s", strerror(errno));
		return 0;
	}
	else
	{
		DEBUG("%s", "Socket successfully opened!!");
	}

	int optval = 1;
	if((setsockopt( sockfd, SOL_SOCKET , SO_REUSEADDR, (char*)&optval, sizeof(optval)))
			== -1)
	{
		ERROR("%s", "Error occured while asking to reuse socket addr");
		::close(sockfd);
		return 0;
	}

	//Initialize the server address with zero.
	ACE_OS::memset((char *) &serv_addr,0,sizeof(serv_addr));

	portno = (int) Serverportno;

	// HQ50818 - BEGIN
	char current_hostname [20];
	ACE_OS::memset( current_hostname, 0, sizeof(current_hostname));
	char data [100] = { 0 };
	FILE *fp = 0;
	string cmd;
	cmd = APOS_CLUSTERCONF_BIN;

	if (ACS_NSF_Server::objCommon.getNode() == NODE_A)
	{
		cmd.append(" ip -D | grep \"ip 1 eth3 \" | awk '{print $6}'");
	}
	else if (ACS_NSF_Server::objCommon.getNode() == NODE_B)
	{
		cmd.append(" ip -D | grep \"ip 2 eth3 \" | awk '{print $6}'");
	}

	if (!(fp = popen(cmd.c_str(), "r")))
	{
		ERROR("%s", "Error while reading from popen in PingThread");
		ERROR("%s", "Leaving PingThread.");
		::close(sockfd);
		return 0;
	}

	if (fscanf(fp, "%100s", data ) != 0)
	{
		ACE_OS::strcpy(current_hostname, data);
	}
	else
	{
		ERROR("%s", "Error occurred while getting current host name");
		ERROR("%s", "Leaving PingThread.");
		pclose(fp);
		return 0;
	}

	pclose(fp);

	DEBUG("Current node HostName:%s", current_hostname);

	unsigned long net_addr;
	net_addr = inet_network(current_hostname);
	serv_addr.sin_addr.s_addr = (unsigned long)htonl(net_addr);
	// HQ50818 - END

	serv_addr.sin_family = AF_INET;
	//	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//Bind the socket to server address.
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		ERROR("Error occurred while binding server socket %s", strerror(errno));
		shutdown(sockfd, SHUT_RDWR);
		::close(sockfd);
		return 0;
	}
	else
	{
		DEBUG("%s", "Address successfully bound with socket!!");
	}

	if (listen(sockfd, 5) == -1)
	{
		ERROR("%s", "Error occured in listen");
		::close(sockfd);
		return 0;
	}

	while (1)
	{
		fd_set readHandleSet;
		FD_ZERO(&readHandleSet);
		FD_SET(sockfd, &readHandleSet);
		FD_SET(stopPingThreadFds[0], &readHandleSet);
		
		int maxfd = sockfd;
		if (stopPingThreadFds[0] > sockfd)
		{
			maxfd = stopPingThreadFds[0] ;
		}
		int ret = select(maxfd + 1, &readHandleSet, NULL, NULL, 0);
		if (ret < 0)
		{
			if (ACE_OS::last_error() == EINTR)
			{
				continue;
			}
			else
			{
				ERROR("%s", "Error occurred in select system call");
				shutdown(sockfd, SHUT_RDWR);
				::close(sockfd);
				return 0;
			}
		}
		else if (ret > 0)
		{
			if (FD_ISSET(stopPingThreadFds[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signalled while waiting for client connection");
				shutdown(sockfd, SHUT_RDWR);				
				::close(sockfd);
				return 0;
			}

			clilen = sizeof(cli_addr);

			//Accept the incoming client connection.
			newsockfd = accept(sockfd,
					(struct sockaddr *) &cli_addr,
					(socklen_t*)&clilen);

			DEBUG("newsockfd = %d ",newsockfd);
			//If data is not valid, return error.
			if (newsockfd < 0)
			{
				ERROR("Error occured while accepting connection from client: %s", strerror(errno));
				//Dont return failure, try to accept other connections.
				continue;
			}
			else
			{
				DEBUG("%s", "Connection successfully accepted!!");
			}

			//Initialize the buffer with 0.
			ACE_OS::memset(buffer,0,256);

			//Read the data sent by client.
			n = recv(newsockfd,buffer,255, 0);

			if (n <= 0)
			{
				ERROR("Error occurred while reading data from client: %s", strerror(errno));
				//Close the socket and continue.
				shutdown(newsockfd, SHUT_RDWR);
				::close(newsockfd);
				continue;
			}
			else
			{
				DEBUG("%s", "Data read from socket successful!!");
				DEBUG("Data received from client is %s", buffer );
			}

			DEBUG("Here is the message: %s",buffer);

			if (ACE_OS::strcmp(buffer, "TIME") == 0)
			{
				char buffer1[100];
				ACE_OS::memset(buffer1, 0, sizeof(buffer1));

				time_t sysTime;

				sysTime = time(NULL);

				int systemTime = (uintmax_t)sysTime;
				ACE_OS::itoa(systemTime, buffer1, 10);

				//Send the system time to client.
				n = ACE_OS::send(newsockfd,buffer1,100, 0);

				if (n <= 0)
				{
					ERROR("%s", "Error occured while writing data into socket");
					shutdown(newsockfd, SHUT_RDWR);
					::close(newsockfd);
					continue;
				}
				else
				{
					DEBUG("PingThread: Response message sent to other node: = %s, bufferRead is %d",buffer1,  n);
				}
			}
			else 	//It will be the data related to IMM parameters.
			{
				ACE_OS::memcpy(	&ACS_NSF_Server::objCommon.pingPeriod,  &buffer[0], sizeof( ACS_NSF_Server::objCommon.pingPeriod ));
				ACE_OS::memcpy(	&ACS_NSF_Server::objCommon.routerResponse,  &buffer[32], sizeof( ACS_NSF_Server::objCommon.routerResponse ));
				ACE_OS::memcpy( &ACS_NSF_Server::objCommon.maxFailoverAttempts,  &buffer[64], sizeof( ACS_NSF_Server::objCommon.maxFailoverAttempts ));
				ACE_OS::memcpy(	&ACS_NSF_Server::objCommon.resetTime, &buffer[96], sizeof( ACS_NSF_Server::objCommon.resetTime ));
				ACE_OS::memcpy( &ACS_NSF_Server::objCommon.isSurveillanceEnabled,  &buffer[128], sizeof( ACS_NSF_Server::objCommon.isSurveillanceEnabled ));
				ACE_OS::memcpy( &ACS_NSF_Server::objCommon.failoverPriority, &buffer[160], sizeof( ACS_NSF_Server::objCommon.failoverPriority ));
				ACE_OS::memcpy( &ACS_NSF_Server::objCommon.failoverCount, &buffer[192], sizeof( ACS_NSF_Server::objCommon.failoverCount ));
				ACE_OS::memcpy( &ACS_NSF_Server::objCommon.isSurveillanceActive, &buffer[224], sizeof( ACS_NSF_Server::objCommon.isSurveillanceActive ));

				DEBUG( "%s", "Passive node received the following values");
				DEBUG( "Ping Period: %u", ACS_NSF_Server::objCommon.pingPeriod );
				DEBUG( "Router Response: %u", ACS_NSF_Server::objCommon.routerResponse );
				DEBUG( "Max Failovers: %u", ACS_NSF_Server::objCommon.maxFailoverAttempts );
				DEBUG( "ResetTime: %u", ACS_NSF_Server::objCommon.resetTime );
				DEBUG( "isSurveillanceEnabled: %d", ACS_NSF_Server::objCommon.isSurveillanceEnabled );
				DEBUG( "Failover Priority: %d", ACS_NSF_Server::objCommon.failoverPriority );
				DEBUG( "Number Failover: %u", ACS_NSF_Server::objCommon.failoverCount );
				DEBUG( "isSurveillanceActive: %d", ACS_NSF_Server::objCommon.isSurveillanceActive);
			}

		}
		shutdown(newsockfd, SHUT_RDWR);
		::close(newsockfd);		
	}
	shutdown(sockfd, SHUT_RDWR);
	::close(sockfd);
	DEBUG("%s", "Exiting ACS_NSF_Server::PingThread()");
	return 0;
}

/*=====================================================================
ROUTINE: StartPingServer
=====================================================================*/

bool ACS_NSF_Server::StartPingServer() 
{
	DEBUG("%s", "Entered ACS_NSF_Server::StartPingServer()");
	DEBUG("%s", "Spawning pingServerThread");
	
	ACE_thread_t pingThreadId = 0;
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&pingThreadCallbackFunc,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&pingThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			m_threadGroupId,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if (threadHandle == -1)	// Thread Creation Failed.
	{
		DEBUG("%s", "Error occurred while creating pingServerThread");
		DEBUG("%s", "Leaving StartPingServer.");
		return false;
	}	

	DEBUG("%s", "Leaving StartPingServer.");
	return true;
} 

/*=====================================================================
ROUTINE: sendUpdatedParamsToPassiveNode
========================================================================*/
ACS_CC_ReturnType ACS_NSF_Server::sendUpdatedParamsToPassiveNode()
{
	DEBUG("%s", "Entering sendUpdatedParamsToPassiveNode");

	if (ACS_NSF_Server::objCommon.getActiveNodeFlag() == false )
	{
		DEBUG("%s", "No Need to send the updated parameters from passive node");
		DEBUG("%s", "Leaving sendUpdatedParamsToPassiveNode");
		return ACS_CC_SUCCESS;
	}

	char partner_hostname [20];
	ACE_OS::memset( partner_hostname, 0, sizeof(partner_hostname));
	char data [100] = { 0 };

	ACE_HANDLE Clientsockfd = ACE_INVALID_HANDLE ;
	int noOfBytes = 0;
	struct sockaddr_in Clientserv_addr;
	struct hostent *Clientserver = 0;

	char buffer[256];
	ACE_OS::memset(buffer, 0, sizeof(buffer));

	Clientsockfd = ACE_OS::socket(AF_INET, SOCK_STREAM, 0);

	if ( Clientsockfd < 0 )
	{
		ERROR("%s", "Error occured while opening client socket in sendUpdatedParamsToPassiveNode"); 
		DEBUG("%s", "Leaving sendUpdatedParamsToPassiveNode");
		return ACS_CC_FAILURE;
	}

	FILE *fp = 0;
	string cmd;
	cmd = APOS_CLUSTERCONF_BIN;

	if (ACS_NSF_Server::objCommon.getNode() == NODE_A)
	{
		cmd.append(" ip -D | grep \"ip 2 eth3 \" | awk '{print $6}'");
	}
	else if (ACS_NSF_Server::objCommon.getNode() == NODE_B)
	{
		cmd.append(" ip -D | grep \"ip 1 eth3 \" | awk '{print $6}'");
	}

	if (!(fp = popen(cmd.c_str(), "r")))
	{
		ERROR("%s", "Error while reading from popen in sendUpdatedParamsToPassiveNode");
		ERROR("%s", "Leaving sendUpdatedParamsToPassiveNode.");
		ACE_OS::close(Clientsockfd);
		return ACS_CC_FAILURE;
	}
	
	if (fscanf(fp, "%100s", data ) != 0)
	{
		ACE_OS::strcpy(partner_hostname, data);
	}
	else
	{
		ERROR("%s", "Error occured while getting partner hostname");
		ERROR("%s", "Leaving sendUpdatedParamsToPassiveNode.");
		pclose(fp);
		ACE_OS::close(Clientsockfd);
		return ACS_CC_FAILURE;
	}
	pclose(fp);

	DEBUG("Partner HostName:%s", partner_hostname);

	Clientserver = ACE_OS::gethostbyname(partner_hostname);

	if (Clientserver == 0 )
	{
		ERROR("%s", "Partner host name does not exist");
		ERROR("%s", "Leaving sendUpdatedParamsToPassiveNode.");
		ACE_OS::close(Clientsockfd);
		return ACS_CC_FAILURE;
	}

	//Initialize the client server address with zero.
	ACE_OS::memset((char *) &Clientserv_addr,0,sizeof(Clientserv_addr));

	Clientserv_addr.sin_family = AF_INET;
	ACE_OS::memcpy((char *)&Clientserv_addr.sin_addr.s_addr,
			(char *)Clientserver->h_addr,
			Clientserver->h_length);

	Clientserv_addr.sin_port = htons(Clientportno);

	if( ACE_OS::connect(Clientsockfd,
				(struct sockaddr *)&Clientserv_addr,
				sizeof(Clientserv_addr)) < 0)
	{
		ERROR("Error occured while connecting to the server : %d", ACE_OS::last_error());
		ERROR("%s", "Leaving sendUpdatedParamsToPassiveNode.");
		ACE_OS::close(Clientsockfd);
		return ACS_CC_FAILURE;
	}

	//DEBUG("%s", "Sending the below values to passive node");

	//DEBUG( "Ping Period: %u", ACS_NSF_Server::objCommon.pingPeriod );
	//DEBUG( "Router Response: %u", ACS_NSF_Server::objCommon.routerResponse );
	//DEBUG( "Failovers: %u", ACS_NSF_Server::objCommon.maxFailoverAttempts );
	//DEBUG( "ResetTime: %u", ACS_NSF_Server::objCommon.resetTime );
	//DEBUG( "isSurveillanceEnabled: %d", ACS_NSF_Server::objCommon.isSurveillanceEnabled );
	//DEBUG( "Failover Priority: %d", ACS_NSF_Server::objCommon.failoverPriority );
	DEBUG( "Number Failover: %u", ACS_NSF_Server::objCommon.failoverCount );
	DEBUG( "Surveillance Active Flag: %d", ACS_NSF_Server::objCommon.isSurveillanceActive);


	ACE_OS::memcpy( &buffer[0], &ACS_NSF_Server::objCommon.pingPeriod, sizeof( ACS_NSF_Server::objCommon.pingPeriod ));
	ACE_OS::memcpy( &buffer[32], &ACS_NSF_Server::objCommon.routerResponse, sizeof( ACS_NSF_Server::objCommon.routerResponse ));
	ACE_OS::memcpy( &buffer[64], &ACS_NSF_Server::objCommon.maxFailoverAttempts, sizeof( ACS_NSF_Server::objCommon.maxFailoverAttempts ));
	ACE_OS::memcpy( &buffer[96], &ACS_NSF_Server::objCommon.resetTime, sizeof( ACS_NSF_Server::objCommon.resetTime ));
	ACE_OS::memcpy( &buffer[128], &ACS_NSF_Server::objCommon.isSurveillanceEnabled, sizeof( ACS_NSF_Server::objCommon.isSurveillanceEnabled ));
	ACE_OS::memcpy( &buffer[160], &ACS_NSF_Server::objCommon.failoverPriority, sizeof( ACS_NSF_Server::objCommon.failoverPriority ));
	ACE_OS::memcpy( &buffer[192], &ACS_NSF_Server::objCommon.failoverCount, sizeof( ACS_NSF_Server::objCommon.failoverCount ));
	ACE_OS::memcpy( &buffer[224], &ACS_NSF_Server::objCommon.isSurveillanceActive, sizeof( ACS_NSF_Server::objCommon.isSurveillanceActive));

	//Now, after constructing the message, write the data into the socket.
	noOfBytes = ACE_OS::send(Clientsockfd, buffer, sizeof(buffer), 0);


	DEBUG("Number of bytes sent to server is %d", noOfBytes );

	//If no of bytes written into the socket are less than or equal to zero, then 
	//return failure.

	if (noOfBytes <= 0 )
	{
		ERROR("%s", "Error occured while writing params in sendUpdatedParamsToPassiveNode");
		ERROR("%s", "Leaving sendUpdatedParamsToPassiveNode.");
		ACE_OS::close(Clientsockfd);
		return ACS_CC_FAILURE;
	}
	ACE_OS::close(Clientsockfd);

	DEBUG("%s", "Leaving sendUpdatedParamsToPassiveNode");
	return ACS_CC_SUCCESS;
}

/*=====================================================================
ROUTINE: VerifyHeartbeat
=====================================================================*/
int ACS_NSF_Server::VerifyHeartBeat()
{
	DEBUG("%s", "Entering VerifyHeartBeat");

	char partner_hostname [20];
	memset( partner_hostname, 0, sizeof(partner_hostname));
	char data [112];

	int Clientsockfd, Clientn;
	struct sockaddr_in Clientserv_addr;
	struct hostent *Clientserver;

	char buffer[256];
	ACE_OS::memset(buffer, 0, sizeof(buffer));

	Clientsockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (Clientsockfd < 0)
	{
		ERROR("%s", "Error occured while opening client socket in VerifyHeartBeat.");
		ERROR("%s", "Exiting VerifyHeartBeat.");
		return -1;
	}
	string cmd;
	cmd = APOS_CLUSTERCONF_BIN;

	if (ACS_NSF_Server::objCommon.getNode() == NODE_A)
	{

		FILE *fp = 0;
		cmd.append(" ip -D | grep \"ip 2 eth3\" | awk '{print $6}'");

		if (!(fp = popen(cmd.c_str(), "r")))
		{

			ERROR("%s", "Error while reading from popen in VerifyHeartBeat. Returning..");
			ERROR("%s", "Exiting VerifyHeartBeat.");
			return -1;
		}

		if(fgets(data, 100, fp) != NULL)
		{
			ACE_INT32 len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';

			strcpy(partner_hostname, data );
		}

		pclose(fp);
	}
	else if (ACS_NSF_Server::objCommon.getNode() == NODE_B)
	{
		FILE *fp = 0;
		cmd.append(" ip -D | grep \"ip 1 eth3\" | awk '{print $6}'");

		if (!(fp = popen(cmd.c_str(), "r")))
		{

			ERROR("%s\n", "Error while reading from popen in VerifyHeartBeat.");
			ERROR("%s", "Exiting VerifyHeartBeat.");
			return -1;
		}

		if(fgets(data, 100, fp) != NULL)
		{
			ACE_INT32 len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';

			strcpy(partner_hostname, data );

		}

		pclose(fp);
	}

	DEBUG("Partner HostName:%s", partner_hostname);

	Clientserver = gethostbyname(partner_hostname);

	if (Clientserver == NULL)
	{
		ERROR("%s", "Partner host name does not exist");
		ERROR("%s", "Exiting VerifyHeartBeat.");
		::close(Clientsockfd);

		return -1;
	}
	//Initialize the client server address with zero.

	ACE_OS::memset((char *) &Clientserv_addr, 0,sizeof(Clientserv_addr));

	Clientserv_addr.sin_family = AF_INET;
	ACE_OS::memcpy((char *)&Clientserv_addr.sin_addr.s_addr,
			(char *)Clientserver->h_addr,
			Clientserver->h_length);

	Clientserv_addr.sin_port = htons(Clientportno);

	if (connect(Clientsockfd,(struct sockaddr *)&Clientserv_addr,sizeof(Clientserv_addr)) < 0)
	{
		ERROR("%s", "Error occured while connecting to the server in VerifyHeartBeat");
		ERROR("%s", "Exiting VerifyHeartBeat.");
		::close(Clientsockfd);

		return -1;
	}
	ACE_OS::memset(buffer,0,256);

	strcpy(buffer,"TIME");

	Clientn = send(Clientsockfd,buffer,strlen(buffer), 0);

	if (Clientn < 0)
	{
		ERROR("%s", "Error occurred while writing to socket in VerifyHeartBeat");
		ERROR("%s", "Exiting VerifyHeartBeat.");
		shutdown(Clientsockfd, SHUT_RDWR);
		::close(Clientsockfd);

		return -1;
	}
	ACE_OS::memset(buffer,0,256);
	Clientn = recv(Clientsockfd,buffer,255, 0);
	if (Clientn <= 0)
	{
		ERROR("%s", "Error occurred while reading from socket in VerifyHeartBeat");
		ERROR("%s", "Exiting VerifyHeartBeat.");
		shutdown(Clientsockfd, SHUT_RDWR);
		::close(Clientsockfd);

		return -1;

	}
	else
	{
		DEBUG("Received message is : %s",buffer);

		time_t sysTime;

		sysTime = time(NULL);

		int thisNodeCurrentTime = (uintmax_t)sysTime;
		//int otherNodeCurrentTime = ACS_APGCC::atoi(buffer);
		int otherNodeCurrentTime = atoi(buffer);

		DEBUG("System time is %d", thisNodeCurrentTime);
		DEBUG("Other node time is %d", otherNodeCurrentTime);

		unsigned int deviationTime = 0;
		if(thisNodeCurrentTime >= otherNodeCurrentTime)
		{
			deviationTime = thisNodeCurrentTime - otherNodeCurrentTime;
		}
		else
		{
			deviationTime = otherNodeCurrentTime - thisNodeCurrentTime;
		}

		DEBUG("VerifyHeartBeat: heartBeatDiff = %d",deviationTime);

		// There is no document explaining this formula. So it is left as it is
		// to avoid introduction of new problems.

		if(deviationTime < (4 * ACS_NSF_Server::objCommon.pingPeriod))
		{
			Clientn = 1;// NSF other node is working
		}
	}

	shutdown(Clientsockfd, SHUT_RDWR);
	::close(Clientsockfd);

	DEBUG("%s", "Exiting VerifyHeartBeat.");
	return Clientn;

}


/*=====================================================================
ROUTINE: CountBlockedGw
=====================================================================*/

ACE_UINT32 ACS_NSF_Server::CountBlockedGw(ACE_UINT32 alarmStatus,
		ACE_UINT32 noGW)
{
	//DEBUG("%s", "Entering CountBlockedGw.");

	ACE_UINT32 blocked = 0;
	ACE_UINT32 mask = 8;

	for (ACE_UINT32 i = 0; i < noGW; i++)
	{
		if ((alarmStatus & mask) != 0)
		{
			blocked++;
		}
		mask = mask << 1;
	} // End of for-loop

	//	DEBUG("%s", "Exiting CountBlockedGw.");
	return blocked;

} 


/*=====================================================================
ROUTINE: error
=====================================================================*/
void ACS_NSF_Server::error(const char *msg)
{
	DEBUG("Error in Socket Code %s\n ",msg);
	exit(1);
}

/*=====================================================================
ROUTINE: DoFailover
=====================================================================*/
void ACS_NSF_Server::DoFailover()
{
	DEBUG("%s", "Entering DoFailover");	
	
	// Non cable-less configuration
	if (ACS_NSF_Server::objCommon.GetApgOamAccess() != 1)
	{
		ACE_UINT32 noFail = 0;

		ACS_NSF_Server::objCommon.reportError(2,"NSF requested failover"); // Event
		ACS_NSF_Server::objCommon.setRegTime(NSF_LASTFAILOVERTIME);

		if (ACS_NSF_Server::objCommon.getRegWord(NSF_NOOFFAILOVER,noFail) == 0)
		{
			noFail++;
			ACS_NSF_Server::objCommon.setFailOverSetFlag(true);
			int retval;
			retval = ACS_NSF_Server::objCommon.setRegWord(NSF_NOOFFAILOVER,noFail);
			if(retval == -1)
			{
				DEBUG("%s", "Failure of setRegWord(NSF_NOOFFAILOVER,noFail)");
			}
			ACS_NSF_Server::objCommon.setFailOverSetFlag(false);
		}
		
		ACE_OS::sleep(2);
	}
	
	//Performing reboot with AMF API
	DEBUG("%s","NSF Calling componentReportError(ACS_APGCC_NODE_FAILOVER) to initiate FAILOVER");
	// TR HQ70118 - ACS_APGCC_COMPONENT_FAILOVER is replaced with ACS_APGCC_NODE_FAILOVER
	if (ACS_APGCC_SUCCESS != m_haObj->componentReportError(ACS_APGCC_NODE_FAILOVER))
	{
		ACS_NSF_Server::objCommon.reportError(3, "componentReportError failed", ACS_APGCC::getLastError());
		exit(EXIT_FAILURE);
	}
	//ACE_OS::sleep(40000);
	//ACS_NSF_Server::objCommon.reportError(3, "Failed to reboot system");
	DEBUG("%s", "Exiting DoFailover");
}

/*=====================================================================
ROUTINE: NetworkSurveillance
=====================================================================*/
bool ACS_NSF_Server::NetworkSurveillance()
{
	DEBUG("%s","Entering NetworkSurveillance()");
	int result = -1;
	fd_set readHandleSet;
	result = ACS_NSF_Server::objCommon.setNodeSpecificParams();
	if( result == -1 )
	{
		ERROR("%s", "Exiting NetworkSurveillance");
		return false;
	}
	// Get parameters from PHA
	result = GetPhaParameters();
	if ( result == -1 )
	{
		ERROR("%s", "Unable to retrieve the IMM Parameters");
		ERROR("%s", "Exiting NetworkSurveillance");
		return false;
	}
	// Decide which node (A or B)
	currentNode  = ACS_NSF_Server::objCommon.getNode();

	DEBUG("ACS_NSF_Server current node  -  %d",currentNode);
	if(currentNode == UNDEFINED)
	{
		ERROR("ACS_NSF_Server current node  -  %d ",currentNode);
		ACS_NSF_Server::objCommon.reportError(3, "Cannot decide which node (A or B). Terminating");
		ERROR("%s", "Exiting NetworkSurveillance");
		return false;
	}
	int status = ACE_OS::mkdir(ACE_TEXT(NSF_CONFIG_DIR_NSF_NAME), S_IRWXU | S_IRWXG );
	if ( status != 0 )
	{
		ACE_UINT32  dwError = ACS_APGCC::getLastError();
		if (EEXIST != dwError)
		{
			ERROR("Unable to create the first level directory for storing config parameters %d", dwError);
			ERROR("%s", "Exiting NetworkSurveillance");
			return false;

		}
	}
	status = ACE_OS::mkdir(ACE_TEXT(NSF_CONFIG_DIR_CONF_NAME), S_IRWXU | S_IRWXG );
	if ( status != 0 )
	{
		ACE_UINT32 dwError = ACS_APGCC::getLastError();
		if (EEXIST != dwError)
		{
			ERROR("Unable to create the second level directory for storing config parameters %d", dwError);
			ERROR("%s", "Exiting NetworkSurveillance");
			return false;
		}
	}
	time_t sysTime;
	sysTime = time( NULL );
	int systemTime = sysTime;

	ofstream outfile;
	if( currentNode == NODE_A )
	{
		ACS_NSF_Server::objCommon.theConfigFileMutex.acquire();

		outfile.open(   NSF_CONFIG_FILE_NAME_A, ios::binary|ios::out );
		if( outfile.is_open())
		{
			char *ptrNodeAData =  new char[1024];
			if( ptrNodeAData != 0 )
			{
				memset(ptrNodeAData, 0, sizeof( char)*1024);
				sprintf(ptrNodeAData, "%x;%x;%x;", 0,0,systemTime);
				ifstream::pos_type size  = 1024;
				outfile.write( ptrNodeAData , size);
				delete[] ptrNodeAData;
				ptrNodeAData = 0;
			}
		}
		outfile.close();
		ACS_NSF_Server::objCommon.theConfigFileMutex.release();
	}
	else if ( currentNode == NODE_B )
	{
		ACS_NSF_Server::objCommon.theConfigFileMutex.acquire();

		outfile.open(   NSF_CONFIG_FILE_NAME_B, ios::binary|ios::out );
		if( outfile.is_open())
		{
			char *ptrNodeAData =  new char[1024];
			if( ptrNodeAData != 0 )
			{
				memset(ptrNodeAData, 0, sizeof( char)*1024);
				sprintf(ptrNodeAData, "%d;%d;%d;", 0,0,systemTime);
				ifstream::pos_type size = 1024;
				outfile.write( ptrNodeAData , size);
				delete[] ptrNodeAData;
				ptrNodeAData = 0;
			}
		}
		outfile.close();
		ACS_NSF_Server::objCommon.theConfigFileMutex.release();
	}

	if (ACS_NSF_Server::objCommon.getActiveNodeFlag() == true )
	{
		if( StartOIThreadsOnActiveNode() == ACS_CC_FAILURE )
		{
			ERROR("%s", "Error occured while creating OI callback threads");
			ERROR("%s", "Exiting NetworkSurveillance");

			StopServer();
			return false;
		}

		ACE_Time_Value waitTime(3, 0);
		INFO("%s", "Started waiting for stop event");

		FD_ZERO(&readHandleSet);
		FD_SET(stopGatewayThreadFds[0], &readHandleSet);
		int ret = ACE_OS::select(stopGatewayThreadFds[0]+1, &readHandleSet, NULL, NULL, &waitTime);

		if( ret < 0 )
		{
			if( ACE_OS::last_error() == EINTR )
			{
				ERROR("%s", "EINTR in termination of gateways.");
				return false;
			}
			else
			{
				ERROR("%s", "Error occured in select system call");
				return false;
			}
		}
		else if ( ret > 0 )
		{
			if( FD_ISSET(stopGatewayThreadFds[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signalled for termination of gateways.");
				return false;
			}
		}
		result = SetDefaultIMMParams(ACS_NSF_Server::objCommon.isSurveillanceEnabled); // Dont care if it fails
	}

	if ( result != 0 )
	{
		ERROR("%s", "NetworkSurveillance SetDefaultIMMParams() error");
	}
	result = GetConfigParameters();
	if ( result == -1 )
	{
		ERROR("%s", "Unable to retrieve the configuration parameters");
		ERROR("%s", "Exiting NetworkSurveillance");
		return false;
	}

	if(!ACS_NSF_Server::objCommon.calculateStartStopValue(ACS_NSF_Server::objCommon.isSurveillanceEnabled))
	{
		ERROR("%s", "Error occured while calculating start and stop value");
		ERROR("%s", "Exiting NetworkSurveillance");
		return false;
	}



	int resGateways = 0;
	int resGateways2 = 0;

	if ( ACS_NSF_Server::objCommon.GetPSState() == 1 )	//Physical Sep is configured
	{
		resGateways = GetDefaultGateways();
		resGateways2 = GetDefaultGateways2();
	}
	else
	{
		resGateways = GetDefaultGateways();
	}

	// Get the IP-addresses of the default gateways.
	if(resGateways == -1 || sGatewayVector.empty())
	{
		ACS_NSF_Server::objCommon.reportError(3, "Cannot find any default gateways for the public network adapter. Terminating");
		ERROR("%s", "Error occured while finding default gateways for public network adapter.");
#if 0
		ERROR("%s", "Exiting NetworkSurveillance");
		StopServer();
		return false;
#endif
	}

	// Re-initialize the variable noOfGW to make sure it has the correct value after side switch
	noOfGW = 0;
	for(vector <string>::iterator dwGV_Iter = sGatewayVector.begin(); dwGV_Iter != sGatewayVector.end(); ++dwGV_Iter)
	{
		gwInfo[noOfGW].gwIndex = noOfGW + 3;
		gwInfo[noOfGW].IPAddrStr.assign(*dwGV_Iter);
		ACE_OS::pipe(gwInfo[noOfGW].stopEvent);
		gwInfo[noOfGW].network = 1;	//1 stands for Public Network.
		noOfGW++;
		if(noOfGW > 9)
		{
			break;
		}
	}

	if ( ACS_NSF_Server::objCommon.GetPSState() == 1 )	//PS is configured
	{
		// Get the IP-addresses of the default gateways 2.
		if(resGateways2 == -1 || sGatewayVector2.empty())
		{
			ACS_NSF_Server::objCommon.reportError(3, "Cannot find any default gateways for the public 2 network adapter. Terminating");

			ERROR("%s", "Error occured while finding default gateways for public 2 network adapter.");
#if 0
			ERROR("%s", "Exiting NetworkSurveillance");
			StopServer();
			return false;
#endif
		}

		// Re-initialize the variable noOfGW2 to make sure it has the correct value after side switch
		noOfGW2 = 0;
		for ( vector <string>::iterator dwGV2_Iter = sGatewayVector2.begin(); 
				dwGV2_Iter != sGatewayVector2.end(); ++dwGV2_Iter )
		{
			gwInfo2[noOfGW2].gwIndex = noOfGW2 + 3;
			gwInfo2[noOfGW2].IPAddrStr.assign(*dwGV2_Iter);
			ACE_OS::pipe(gwInfo2[noOfGW2].stopEvent);
			gwInfo2[noOfGW2].network = 2;	//2 stands for Public Network_2.
			noOfGW2++;
			if(noOfGW2 > 9)
			{
				break;
			}
		}
	}

	// Cease all alarms
	ACE_UINT32 state = 0;
	CeaseAllAlarms(state);

	if( ACS_NSF_Server::objCommon.GetPSState() == 1)	//PS is configured.
	{
		CeaseAllAlarms2(state);
	}

	// Start supervision of each GW
	INFO("Number of Gateways monitored by NSF service are - %d ", noOfGW);
	for(ACE_UINT32 gw = 0; gw < noOfGW; gw++)
	{
		ACE_thread_t gatewayThreadId;

		gwInfo[gw].threadH = ACE_Thread_Manager::instance()->spawn(&SuperviseGateway,
				(void *) &gwInfo[gw],
				THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
				&gatewayThreadId,
				0,
				ACE_DEFAULT_THREAD_PRIORITY,
				m_threadGroupId,
				0,
				ACE_DEFAULT_THREAD_STACKSIZE);

		//DEBUG("(%T) Thread created for the gateway - %s ", (gwInfo[gw].IPAddrStr).c_str());
		if(gwInfo[gw].threadH == -1)
		{
			ACS_NSF_Server::objCommon.reportError(3,"cannot start supervision of defaultGateway ",gwInfo[gw].IPAddrStr,". System Error=",ACS_APGCC::getLastError());
			ERROR("%s", "Error occured while creating thread for supervision of default gateways.");
			ERROR("%s", "Exiting NetworkSurveillance");
			StopServer();
			return false;
		}

		//ACE_OS::sleep(5);
		ACE_Time_Value sleepTime(5, 0);
		FD_ZERO(&readHandleSet);
		FD_SET(stopGatewayThreadFds[0], &readHandleSet);
		int ret = ACE_OS::select(stopGatewayThreadFds[0]+1, &readHandleSet, NULL, NULL, &sleepTime);

		if( ret < 0 )
		{
			if( ACE_OS::last_error() == EINTR )
			{
				ERROR("%s", "EINTR in termination of gateways.");
				return false;
			}
			else
			{
				ERROR("%s", "Error occured in select system call");
				return false;
			}
		}
		else if ( ret > 0 )
		{
			if( FD_ISSET(stopGatewayThreadFds[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signalled for termination of gateways.");
				return false;
			}
		}

	} // End of for-loop

	if( ACS_NSF_Server::objCommon.GetPSState() == 1)	//PS is configured.
	{
		// Start supervision of each GW
		DEBUG("Number of Gateways in public 2 network- %d ", noOfGW);
		for(ACE_UINT32 gw = 0; gw < noOfGW2; gw++)
		{
			ACE_thread_t gatewayThreadId;

			gwInfo2[gw].threadH = ACE_Thread_Manager::instance()->spawn(&SuperviseGateway,
					(void *) &gwInfo2[gw],
					THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
					&gatewayThreadId,
					0,
					ACE_DEFAULT_THREAD_PRIORITY,
					m_threadGroupId,
					0,
					ACE_DEFAULT_THREAD_STACKSIZE);

			//DEBUG("(%T) Thread created for the gateway - %s ", (gwInfo2[gw].IPAddrStr).c_str());
			if(gwInfo2[gw].threadH == -1)
			{
				ACS_NSF_Server::objCommon.reportError(3,"cannot start supervision of defaultGateway ",gwInfo[gw].IPAddrStr,". System Error=",ACS_APGCC::getLastError());
				ERROR("%s", "Error occured while creating thread for supervision of default gateways.");
				ERROR("%s", "Exiting NetworkSurveillance");
				StopServer();
				return false;
			}

			//ACE_OS::sleep(5);

			ACE_Time_Value sleepTimeVal(5, 0);;
			FD_ZERO(&readHandleSet);
			FD_SET(stopGatewayThreadFds[0], &readHandleSet);
			int ret = ACE_OS::select(stopGatewayThreadFds[0]+1, &readHandleSet, NULL, NULL, &sleepTimeVal);

			if( ret < 0 )
			{
				if( ACE_OS::last_error() == EINTR )
				{
					ERROR("%s", "EINTR in termination of gateways.");
					return false;
				}
				else
				{
					ERROR("%s", "Error occured in select system call");
					return false;
				}
			}
			else if ( ret > 0 )
			{
				if( FD_ISSET(stopGatewayThreadFds[0], &readHandleSet))
				{
					ERROR("%s", "Stop event signalled for termination of gateways.");
					return false;
				}
			}
		} // End of for-loop
	}


	// Start ping server
	if (!StartPingServer())
	{
		ACS_NSF_Server::objCommon.reportError(3, "cannot start ping server");
		ERROR("%s", "Error occured while starting pinging.");
		ERROR("%s", "Exiting NetworkSurveillance");
		StopServer();
		return false;
	}
	
	// Wait a little bit for ping server thread to start
	sleep(1);

	// BEGIN TR H053106
	// Create a ACS_NSF_TIMESTAMPFILE_PATH folder for storing Timestamp file
	createTimeStampFolder(ACS_NSF_TIMESTAMPFILE_PATH);

	// Create a flag under clear PSO area, allowing BIOS Time Recovery
	// utility to understand that a restore is ongoing.
	//if (const int call_result = createRestoreFlag(ACS_NSF_RESTORE_FLAG_PATH, ACS_NSF_RESTORE_FLAG_FILENAME)) {
	//	WARNING("Failed to create the restore flag under PSO folder, call_result == %d", call_result);
	//}

	// construct File name
	if (ACS_NSF_Server::objCommon.getNode() == NODE_A)
	{
		timeStampFileName.clear();
		timeStampFileName = ACS_NSF_TIMESTAMPFILE_PATH;
		timeStampFileName.append("NODEA_");
		timeStampFileName.append(ACS_NSF_TIMESTAMPFILE_NAME);
	}
	else if (ACS_NSF_Server::objCommon.getNode() == NODE_B)
	{
		timeStampFileName.clear();
		timeStampFileName = ACS_NSF_TIMESTAMPFILE_PATH;
		timeStampFileName.append("NODEB_");
		timeStampFileName.append(ACS_NSF_TIMESTAMPFILE_NAME);
	}
	else
	{
		ERROR("%s","NODE name received by NSF service is UNDEFINED!!");
		StopServer();
		return false;
	}
	INFO("NSF service stored timestamp in %s",timeStampFileName.c_str());
	// END TR H053106
	//Supervision of external network

	while(1)
	{
		if (ACS_NSF_Server::objCommon.getActiveNodeFlag())
		{
			// in case of active node - chk reloading of pha prams required or not ?
			if(ACS_NSF_Server::objCommon.isLoadingOfParamsRequired() )
			{

				int result = GetPhaParameters();
				if ( result == -1 )
				{
					ERROR("%s", "Unable to retrieve the PHA Parameters");
				}

				result = GetConfigParameters();
				if ( result == -1 )
				{
					ERROR("%s", "Unable to retrieve the configuration parameters");
				}

				if ( sendUpdatedParamsToPassiveNode() == ACS_CC_FAILURE )
				{
					ERROR("%s", "Failure occurred while sending IMM parameters to passive node");

				}
				else
				{
					ACS_NSF_Server::objCommon.setLoadParamsFlag(false);
				}
			}
		}
		ACE_Time_Value pingTime(ACS_NSF_Server::objCommon.pingPeriod, 0);
		DEBUG("%s", "Started waiting for stop event");

		FD_ZERO(&readHandleSet);
		FD_SET(stopGatewayThreadFds[0], &readHandleSet);

		int ret = ACE_OS::select(stopGatewayThreadFds[0] + 1, &readHandleSet, NULL, NULL, &pingTime);

		if( ret < 0 )
		{
			if( ACE_OS::last_error() == EINTR )
			{
				ERROR("%s", "EINTR in termination of gateways.");
				return false;
			}
			else
			{
				ERROR("%s", "Error occurred in select system call");
				return false;
			}
		}
		else if ( ret > 0 )
		{
			if( FD_ISSET(stopGatewayThreadFds[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signaled for termination of gateways.");
				return false;
			}

		}

		DEBUG("%s", "Stopped waiting for stop event due to timeout.");

		//TR HK74557
		ACE_stat exists;
		if (ACE_OS::stat (ACE_TEXT(ACS_NSF_DIRPATH), &exists) != 0)
		{
			int status = ACE_OS::mkdir(ACE_TEXT(ACS_NSF_DIRPATH), S_IRWXU | S_IRWXG );
			if ( status != 0 )
			{
				ACE_UINT32  dwError = ACS_APGCC::getLastError();
				if (EEXIST != dwError)
				{
					ACS_NSF_Server::objCommon.reportError(5,ACS_NSF_DIRPATH);
				}
			}//TR HK74557 end
		}

		SaveSystemTime();

		ACE_UINT32 alarmState = 0;
		if (currentNode == NODE_A) // A
		{
			ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A, alarmState);
		}
		else // B
		{
			ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B, alarmState);
		}

		ACE_UINT32 failoverCount = 0;
		failoverCount = ACS_NSF_Server::objCommon.failoverCount;

		// Reset failoverCount ?
		ACE_UINT32 failoverTime = 0;
		ACS_NSF_Server::objCommon.getRegTime(NSF_LASTFAILOVERTIME, failoverTime);
		failoverTime = failoverTime/60;

		// the reason to include the condition (failoverCount!=0) to avoid, setting the
		// registry key "HKLM\cluster\NSF_NOOFFAILOVER" value with 0(zero) for every minute whenever the last failover
		// time exceeds 60 min
		if((failoverTime > ACS_NSF_Server::objCommon.resetTime) && (failoverCount!=0))  //modified for HG26029
		{
			INFO("Last failover time exceeds resettime: %u min", ACS_NSF_Server::objCommon.resetTime);
			INFO("%s", "NSF is setting failover count to 0");
			ACS_NSF_Server::objCommon.setRegWord(NSF_NOOFFAILOVER,0);
			ACS_NSF_Server::objCommon.setLoadParamsFlag(true);
		}

		// Raise/Cease O2
		ACE_UINT32 startStopFlag = 0;

		if(!ACS_NSF_Server::objCommon.calculateStartStopValue(ACS_NSF_Server::objCommon.isSurveillanceEnabled))
		{
			ERROR("%s", "Error occured while getting start/stop value");
			ERROR("%s","Exiting NetworkSurveillance()");
			StopServer();
			return false;
		}
		startStopFlag = ACS_NSF_Server::objCommon.getStartStopValue();

		DEBUG("Start Stop Value is %d", startStopFlag );


		if(startStopFlag == 1) // Passive supervision
		{
			if((alarmState & 0X0001) == 0)
			{
				SendAlarm("", 0, currentNode, 0); // O2.  Ignore error
			}
		}
		else // Active supervision
		{
			if((alarmState & 0X0001) != 0)
			{
				CeaseAlarm("", 0, currentNode); // Ignore error
			}
		}


		//ACE_UINT32 A_alarm = 0 ;
		//ACE_UINT32 B_alarm = 0 ;
		//int active = 0;

		//Check if Physical Separation is configured or not.

		if ( ACS_NSF_Server::objCommon.GetPSState() == 1 )	//PS is configured.
		{
			DEBUG("%s", "Checking if have to do failover for Physical Separation.");
			bool myCheckFailover=CheckIfFailoverForPS();
			DEBUG("The return Code for CheckIfFailoverForPS Function = %d",myCheckFailover);
		}
		else
		{
			int active = 0;
			if(ACS_NSF_Server::objCommon.getActiveNodeFlag())
			{
				active = 1;
			}

			if(!ACS_NSF_Server::objCommon.calculateStartStopValue(ACS_NSF_Server::objCommon.isSurveillanceEnabled))
			{
				ERROR("%s", "Error occured while calculating with start/stop value.");
				ERROR("%s", "Exiting NetworkSurveillance()");
				StopServer();
				return false;
			}
			startStopFlag = ACS_NSF_Server::objCommon.getStartStopValue();

			ACE_UINT32 A_alarm = 0 ;
			ACE_UINT32 B_alarm = 0 ;
			if((ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A, A_alarm) == 0) &&
					(ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B, B_alarm) == 0))
			{
				ACE_UINT32 activeAlarm = A_alarm;
				ACE_UINT32 passiveAlarm = B_alarm;

				if(ACS_NSF_Server::objCommon.getActiveNodeFlag())
				{
					if ( currentNode == NODE_A  )
					{
						activeAlarm = A_alarm;
						passiveAlarm = B_alarm;
					}
					else if ( currentNode == NODE_B )
					{
						activeAlarm = B_alarm;
						passiveAlarm = A_alarm;
					}
				}
				else
				{
					if ( currentNode == NODE_A  )
					{
						activeAlarm = B_alarm;
						passiveAlarm = A_alarm;
					}
					else if ( currentNode == NODE_B ) 
					{
						activeAlarm = A_alarm;
						passiveAlarm = B_alarm;
					}
				}

				DEBUG("Supervision of the current node  -  %d ", active);
				DEBUG("Active alarm is %d", activeAlarm);
				DEBUG("Passive alarm is %d", passiveAlarm);

				//Raise or cease A1 alarm.
				if((CountBlockedGw(activeAlarm, noOfGW) == noOfGW) &&
						(CountBlockedGw(passiveAlarm, noOfGW) == noOfGW))
				{
					//All gateways blocked.
					//If NSF active and started, active node, and A1 alarm not raised,
					//raise A1 alarm.

					INFO("%s","All gateways Blocked ");
					if((startStopFlag == 3) && (active == 1) && ((activeAlarm & 0X02) == 0))
					{
						SendAlarm("", 1, currentNode, 0);
					}
				}
				else
				{
					//Not all gateways blocked.
					//If A1 alarm raised in node, cease A1 alarm.
					INFO("%s", "Not all gateways Blocked");
					if((currentNode == NODE_A) && ((A_alarm & 0X02) != 0))
					{
						CeaseAlarm("",1,currentNode);
					}
					if((currentNode == NODE_B) && ((B_alarm & 0X02) != 0))
					{
						CeaseAlarm("",1,currentNode);
					}
				}
				if((active == 1) && ((startStopFlag & 1) == 1))
				{
					//TR HL37087 Start
					if ((CountBlockedGw(activeAlarm, noOfGW) == noOfGW) &&
							(CountBlockedGw(passiveAlarm, noOfGW) < noOfGW))
					{
						ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,A_alarm);
						ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,B_alarm);
						activeAlarm = A_alarm;
						passiveAlarm = B_alarm;
						if (currentNode == NODE_B)
						{
							activeAlarm = B_alarm;
							passiveAlarm = A_alarm;
						}
					}
					//TR HL37087 End

					if(CountBlockedGw(activeAlarm, noOfGW) == noOfGW)
					{
						// All gateways on current active node is blocked
						if((activeAlarm & 0X04) == 0)
						{
							SendAlarm("",2,currentNode,0); // send A2 all GW on active blocked
						}
						if(CountBlockedGw(passiveAlarm, noOfGW) == noOfGW)
						{
							if((activeAlarm & 0X02) == 0)
							{
								SendAlarm("",1,currentNode,0); // send A1 both nodes blocked
							}
						}
						else
						{
							// All gateways on active node is blocked but at least one gateway on
							// passive node is working. Try to failover

							//	DEBUG("%s", "NetworkSurveillance: Trying to failover");

							ACE_UINT32 failoverCount = 65535;
							std::string errorMsg = "";
							if((ACS_NSF_Server::objCommon.getRegWord(NSF_NOOFFAILOVER, failoverCount) == 0) && (failoverCount < ACS_NSF_Server::objCommon.maxFailoverAttempts))
							{
								DEBUG("NetworkSurveillance: failoverCount = %d",failoverCount);
								int heartBeat = 0;
								int attempts = 0;
								// Try 4 attempts to verify NSF is running on the other node or not
								do
								{
									heartBeat = VerifyHeartBeat();
									DEBUG("NetworkSurveillance: verifyHeartBeat = %d", heartBeat);
								}while((heartBeat == 0) && (attempts++ < 4));

								if(heartBeat == 1)
								{
									if((activeAlarm & 0X02) != 0)
									{
										CeaseAlarm("",1,currentNode); //    cease A1
									}
									DoFailover(); // Will never return
								}
								else
								{
									errorMsg = "NSF Server unavailable on other node";
								}
							}
							else
							{
								DEBUG("NetworkSurveillance: failoverCount = %d", failoverCount);
								errorMsg = "too many failovers";
							}

							if(errorMsg.size() > 0)
							{
								errorMsg = "Not allowed to failover due to "+errorMsg;
								ACS_NSF_Server::objCommon.reportError(3, errorMsg.c_str());
								if ((activeAlarm & 0X02) != 0)
								{
									CeaseAlarm("", 1, currentNode); //  cease A1
								}
							}
						}
					}
					else
					{
						if((activeAlarm & 0X04) != 0) // A2 all gateways on active node
						{
							CeaseAlarm("", 2, currentNode);  // cease A2
						}
					}
				}
				else // Passive node or passive supervision
				{
					if( noOfGW != 0)
					{
						if((active == 1) && (activeAlarm & 0X04) != 0) // A2 all gateways on active node
						{
							CeaseAlarm("", 2, currentNode);  // cease A2
						}
					}
				}
			}
		}
	} // End of while

	return true;

} 

/*=====================================================================
ROUTINE: NetworkSurveillanceForCableLess
=====================================================================*/
bool ACS_NSF_Server::NetworkSurveillanceForCableLess()
{
	DEBUG("%s","Entering NetworkSurveillanceForCableLess()");
	int result = -1;
	fd_set readHandleSet;
	result = ACS_NSF_Server::objCommon.setNodeSpecificParams();
	if (result == -1)
	{
		ERROR("%s", "Exiting NetworkSurveillanceForCableLess");
		return false;
	}
	
	// Get parameters from PHA
	result = GetPhaParameters();
	if (result == -1)
	{
		ERROR("%s", "Unable to retrieve the IMM Parameters");
		ERROR("%s", "Exiting NetworkSurveillanceForCableLess");
		return false;
	}
	
	// Decide which node (A or B)
	currentNode  = ACS_NSF_Server::objCommon.getNode();
	DEBUG("ACS_NSF_Server current node  -  %d",currentNode);
	if (currentNode == UNDEFINED)
	{
		ERROR("ACS_NSF_Server current node  -  %d ",currentNode);
		ACS_NSF_Server::objCommon.reportError(3, "Cannot decide which node (A or B). Terminating");
		ERROR("%s", "Exiting NetworkSurveillanceForCableLess");
		return false;
	}
	
	// Create the config file folder
	int status = ACE_OS::mkdir(ACE_TEXT(NSF_CONFIG_DIR_NSF_NAME), S_IRWXU | S_IRWXG);
	if (status != 0)
	{
		ACE_UINT32  dwError = ACS_APGCC::getLastError();
		if (EEXIST != dwError)
		{
			ERROR("Unable to create the first level directory for storing config parameters %d", dwError);
			ERROR("%s", "Exiting NetworkSurveillanceForCableLess");
			return false;

		}
	}	
	status = ACE_OS::mkdir(ACE_TEXT(NSF_CONFIG_DIR_CONF_NAME), S_IRWXU | S_IRWXG);
	if (status != 0)
	{
		ACE_UINT32 dwError = ACS_APGCC::getLastError();
		if (EEXIST != dwError)
		{
			ERROR("Unable to create the second level directory for storing config parameters %d", dwError);
			ERROR("%s", "Exiting NetworkSurveillanceForCableLess");
			return false;
		}
	}
	
	// Get the system time
	time_t sysTime;
	sysTime = time(NULL);
	int systemTime = sysTime;

	// Initialize the internal config file
	ofstream outfile;
	const char* configFileName = (currentNode == NODE_A) ? NSF_CBL_CONFIG_FILE_NAME_A : NSF_CBL_CONFIG_FILE_NAME_B;
	ACS_NSF_Server::objCommon.theConfigFileMutex.acquire();
	outfile.open(configFileName, ios::binary|ios::out);
	if (outfile.is_open())
	{
		char *ptrNodeData =  new char[1024];
		if (ptrNodeData != 0)
		{
			memset(ptrNodeData, 0, sizeof( char)*1024);
			sprintf(ptrNodeData, "%x;%x;%x;", 0, 0, systemTime);
			ifstream::pos_type size  = 1024;
			outfile.write(ptrNodeData , size);
			delete[] ptrNodeData;
			ptrNodeData = 0;
		}
	}
	else
	{
		ERROR("Failed to open file: %s", configFileName);
		return false;
	}
	outfile.close();
	ACS_NSF_Server::objCommon.theConfigFileMutex.release();

	// start of TR HU34140 - creating /opt/ap/acs/etc/nsf/conf path is done first
	// then the isSurveillanceAcitve flag is modified

	// Create a ACS_NSF_TIMESTAMPFILE_PATH folder for storing Timestamp file
	createTimeStampFolder(ACS_NSF_TIMESTAMPFILE_PATH);
	// Construct File name
	timeStampFileName.clear();
	timeStampFileName = ACS_NSF_TIMESTAMPFILE_PATH;
	timeStampFileName.append((currentNode == NODE_A) ? "NODEA_" : "NODEB_");
	timeStampFileName.append(ACS_NSF_TIMESTAMPFILE_NAME);
	INFO("NSF service stored timestamps in %s", timeStampFileName.c_str());

	// Save system time to file
	SaveSystemTime();
	// end of TR HU34140

	bool isActiveNode = ACS_NSF_Server::objCommon.getActiveNodeFlag();
	if (isActiveNode == true)
	{
		// Set isSurveillanceActive=NO by default
		result = ACS_NSF_Server::objCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG, 0);		
		if (result != 0)
		{
			ERROR("%s", "NetworkSurveillanceForCableLess set isSurveillanceActive=NO error");
			ERROR("setRegWord() error: return code = %d", result);
			return false;
		}
		
		if (StartOIThreadsOnActiveNode() == ACS_CC_FAILURE)
		{
			ERROR("%s", "Error occurred while creating OI callback threads");					
			ERROR("%s", "Exiting NetworkSurveillanceForCableLess");
			return false;
		}
	}
	
	// Init the stopEvent and Start supervision thread to monitor on bond1 interface
	ACE_OS::pipe(gwInfo[0].stopEvent);	
			
	ACE_thread_t superviseForCableLessThreadId;
	gwInfo[0].threadH = ACE_Thread_Manager::instance()->spawn(&SuperviseGateway,
				(void *) &gwInfo[0],
				THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
				&superviseForCableLessThreadId,
				0,
				ACE_DEFAULT_THREAD_PRIORITY,
				m_threadGroupId,
				0,
				ACE_DEFAULT_THREAD_STACKSIZE);
	
	if (gwInfo[0].threadH == -1)
	{		
		ACS_NSF_Server::objCommon.reportError(4, "cannot start supervise thread for cable-less");
		ERROR("%s", "Error occurred while starting supervise thread for cable-less");
		ERROR("%s", "Exiting NetworkSurveillanceForCableLess");		
		return false;
	}
	
	// Start ping server
	if (!StartPingServer())
	{
		ACS_NSF_Server::objCommon.reportError(3, "cannot start ping server");
		ERROR("%s", "Error occurred while starting pinging.");
		ERROR("%s", "Exiting NetworkSurveillanceForCableLess");		
		return false;
	}
	
	
	// Wait a little while to make sure pingServer & supervise threads started
	ACE_OS::sleep(2);
	
	// Supervision of external network
	while (1)
	{
		ACE_Time_Value pingTime(ACS_NSF_Server::objCommon.pingPeriod, 0);
		DEBUG("%s", "Started waiting for stop event");

		FD_ZERO(&readHandleSet);
		FD_SET(stopGatewayThreadFds[0], &readHandleSet);

		int ret = ACE_OS::select(stopGatewayThreadFds[0] + 1, &readHandleSet, NULL, NULL, &pingTime);

		if (ret < 0)
		{
			if (ACE_OS::last_error() == EINTR)
			{
				ERROR("%s", "EINTR in termination of gateways.");
				return false;
			}
			else
			{
				ERROR("%s", "Error occurred in select system call");
				return false;
			}
		}
		else if (ret > 0)
		{
			if (FD_ISSET(stopGatewayThreadFds[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signalled for termination");				
				return true;
			}
		}

		DEBUG("%s", "Stopped waiting for stop event due to time-out.");
		
		// Save system time to file
		SaveSystemTime();
		
		DEBUG("Supervision of the current node  - isActive: %d", (ACE_UINT32) isActiveNode);
		
		if (isActiveNode == true)
		{
			ACE_UINT32 bondingStateActive = 0;
			ACE_UINT32 bondingStatePassive = 0;
			ACE_UINT32 numOfTimeActive = 0;
			ACE_UINT32 numOfTimePassive = 0;
			bool failOver = false;
						
			if (currentNode == NODE_A)
			{
				ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_BONDINGSTATE_A, bondingStateActive);
				ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_BONDINGSTATE_B, bondingStatePassive);				
			}
			else
			{
				ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_BONDINGSTATE_A, bondingStatePassive);
				ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_BONDINGSTATE_B, bondingStateActive);
			}
			
			INFO("Bonding state: Active = %u - Passive = %u", bondingStateActive, bondingStatePassive);
			// In case bonding state in Active is down and Passive is up then try to failover
			if (bondingStateActive == 1 && bondingStatePassive == 0)
			{
				// Read numOfTime of Active and Passive node
				if (currentNode == NODE_A)
				{
					ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_NUMOFTIMES_A, numOfTimeActive);
					ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_NUMOFTIMES_B, numOfTimePassive);				
				}
				else
				{
					ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_NUMOFTIMES_A, numOfTimePassive);
					ACS_NSF_Server::objCommon.getRegWord(NSF_CBL_NUMOFTIMES_B, numOfTimeActive);
				}
				INFO("number of Times: Active = %u - Passive = %u", numOfTimeActive, numOfTimePassive);
				// Check if the bonding state on Active and Passive node are stable enough to conclude
				if (numOfTimeActive >= ACS_NSF_Server::objCommon.routerResponse && 
					numOfTimePassive >= ACS_NSF_Server::objCommon.routerResponse)
				{
					INFO("%s", "Meet condition to failover");
					failOver = true;
				}
			}

			if (failOver)
			{
				std::string errorMsg = "";
				int heartBeat = 0;
				int attempts = 0;
				// Try 4 attempts to verify NSF is running on the other node or not
				do
				{
					heartBeat = VerifyHeartBeat();
					DEBUG("NetworkSurveillanceForCableLess: verifyHeartBeat = %d", heartBeat);
				} while ((heartBeat == 0) && (attempts++ < 4));

				if (heartBeat == 1)
				{						
					DoFailover(); // Will never return
				}
				else
				{
					errorMsg = "NSF Server unavailable on other node";
				}				

				if (errorMsg.size() > 0)
				{
					errorMsg = "Not allowed to failover due to " + errorMsg;
					ACS_NSF_Server::objCommon.reportError(3, errorMsg.c_str());								
				}
			}			
		}
		else // Passive node or passive supervision
		{
			// Do nothing
		}			
	} // End of while

	return true;
} 


/*=====================================================================
ROUTINE: setupIMMCallBacksThreadFunc
=====================================================================*/
ACE_THR_FUNC_RETURN ACS_NSF_Server::setupIMMCallBacksThreadFunc(void *aPtr)
{
	DEBUG("%s", "Entering setupIMMCallBacksThreadFunc.");

	ACS_NSF_Server *myNsfserverptr = (ACS_NSF_Server*) aPtr;

	//Register the exit handler with the IMM callback thread.

	/*if( ThrExitHandler::init() == false )
	  {
	  DEBUG("%s", "Error occured while registering for exit handler");
	  DEBUG("%s", "Exiting setupIMMCallBacksThreadFunc.");
	  return 0;
	  }*/

	if( myNsfserverptr == 0)
	{
		ERROR("%s", "NSFServer pointer is NULL");
		ERROR("%s", "Exiting setupIMMCallBacksThreadFunc");

		return 0;
	}

	//Invoke setupIMMCallBacks.
	myNsfserverptr->setupIMMCallBacks();

	DEBUG("%s", "Exiting setupIMMCallBacksThreadFunc.");
	return 0;
}

/*=====================================================================
ROUTINE: pingThreadCallbackFunc
=====================================================================*/
ACE_THR_FUNC_RETURN pingThreadCallbackFunc(void * ptr)
{
	DEBUG("%s", "Entering pingThreadCallbackFunc.");
	ACS_NSF_Server *nsfServerPtr = (ACS_NSF_Server*) ptr;

	if (nsfServerPtr == 0)
	{
		ERROR("%s", "NSFServer pointer is NULL");
		ERROR("%s", "Exiting pingThreadCallbackFunc");
		return 0;
	}

	nsfServerPtr->PingThread();
	DEBUG("%s", "Exiting pingThreadCallbackFunc.");
	return 0;
}

/*=====================================================================
ROUTINE: setupIMMCallBacks
=====================================================================*/
void ACS_NSF_Server::setupIMMCallBacks( )
{
	DEBUG("%s", "Entering setupIMMCallBacks.");

	DEBUG("%s", "Registering OI Callbacks.");

	//	string dnName("");
	//	dnName = ACS_NSF_Server::objCommon.getPublicdnName();

	m_poCommandHandler = new ACS_NSF_CommandHandler(ACS_CONFIG_IMM_NSF_CLASS_NAME, &ACS_NSF_Server::objCommon,"acs_nsf_handler",ACS_APGCC_ONE);

	if( m_poCommandHandler == 0)
	{
		ERROR("%s", "Error occured while allocating memory for ACS_NSF_CommandHandler");
		ERROR("%s", "Exiting setupIMMCallBacks ");
		return ;
	}


	//Start the reactor.
	int ret = m_poCommandHandler->svc();
	if( ret == -1 )
	{
		ERROR("%s", "Error occured invoking svc of ACS_NSF_CommandHandler");
		ERROR("%s", "Exiting setupIMMCallBacks ");
		return ;
	}

	DEBUG("%s", "Exiting setupIMMCallBacks.");
}

/*=====================================================================
ROUTINE: StartServer
=====================================================================*/
void ACS_NSF_Server::StartServer()
{
	DEBUG("%s", "Entering StartServer");
	//To be sure 
	isNSFServerStopped = false;
	/*
	// Create a flag under clear PSO area, allowing BIOS Time Recovery
	// utility to understand that a restore is ongoing.
	if (const int call_result = createRestoreFlag(ACS_NSF_RESTORE_FLAG_PATH, ACS_NSF_RESTORE_FLAG_FILENAME)) {
		WARNING("Failed to create the restore flag under PSO folder, call_result == %d", call_result);
	}*/
	if (ACS_NSF_Server::objCommon.getNode() == NODE_A)
	{
		if (const int call_result = createRestoreFlag(ACS_NSF_RESTORE_FLAG_PATH, ACS_NSF_RESTORE_FLAG_FILENAME_NODEA)) {
			WARNING("Failed to create the NODE_A restore flag under PSO folder, call_result == %d", call_result);
		}
	}
	else if (ACS_NSF_Server::objCommon.getNode() == NODE_B)
	{
		if (const int call_result = createRestoreFlag(ACS_NSF_RESTORE_FLAG_PATH, ACS_NSF_RESTORE_FLAG_FILENAME_NODEB)) {
			WARNING("Failed to create the NODE_B restore flag under PSO folder, call_result == %d", call_result);
		}
	}
	else
	{
		ERROR("%s","NODE name received by NSF service is UNDEFINED!!");
	}


	// Determine if it's running on cable-less environment
	if (ACS_NSF_Server::objCommon.GetApgOamAccess() == 1)
	{
		NetworkSurveillanceForCableLess();
	}
	else // running in normal environment (front port)
	{
		NetworkSurveillance();
	}

	DEBUG("%s", "Exiting StartServer");
}

/*=====================================================================
ROUTINE: StopServer
=====================================================================*/
void ACS_NSF_Server::StopServer()
{
	DEBUG("%s", "Entering StopServer");

	if (isNSFServerStopped == false)
	{
		threadManager_->wait();
		
		DEBUG("%s", "Signal the gateway and ping server threads to stop.");

		char buf[] = { 1 , 1};
		int bytes = 0;

		bytes = ACE_OS::write(stopGatewayThreadFds[1], buf, sizeof(buf));
		if (bytes <= 0)
		{
			ERROR("%s","Error occurred while signalling stop event ");
			ERROR("%s","Leaving StopServer");
			return;
		}
		
		// Terminate gateway supervision thread
		TerminateGatewaySup(1);		

		bytes = ACE_OS::write(stopPingThreadFds[1], buf, sizeof(buf));
		if (bytes <= 0)
		{
			ERROR("%s","Error occurred while signalling stop event ");
			ERROR("%s","Leaving StopServer");
			return;
		}

		// Stop the callback thread on active node.
		if (m_poCommandHandler != 0)
		{
			m_poCommandHandler->shutdown();
		}
		
		isNSFServerStopped  = true;
	}
	else
	{
		DEBUG("%s", "Server already stopped!!");
	}

	DEBUG("%s", "LeavingStopServer.");
}

/*=====================================================================
ROUTINE: ReportEvent
=====================================================================*/
ACE_INT32 ReportEvent(ACE_UINT32 specificProblem,
		const char* severity,
		const char* probableCause,
		const char* objectOfReference,
		const char* problemData,
		const char* problemText)
{
	DEBUG("%s", "Entering ReportEvent");

	ACE_INT32 bResult;
	const ACE_UINT32 NSF_BASE = 10300;
	specificProblem = specificProblem + NSF_BASE;

	// Signal the Critical section.
	m_srctMutex.acquire();

	if (severity != 0 && (strcmp(severity,"EVENT") == 0))
	{
		// Find if event is already reported
		time_t currTime = time(0);

		eventStruct* obj = NSFEventList;
		eventStruct* found = NULL;

		while (obj)
		{
			if (obj->eventId == specificProblem)
			{
				if (strcmp(obj->objRef, objectOfReference) == 0)
				{
					if ((currTime - obj->reportTime) < 600)
					{
						DEBUG("%s", "Event already reported. Less than 10 min ago");

						//Release the mutex.
						m_srctMutex.release();

						DEBUG("%s", "Exiting ReportEvent");
						return 0; // Event already reported. Less than 10 min ago
					}
					else
					{
						found = obj;
					}
				}
			}
			obj = obj->next;
		} // End of while

		if (found == NULL)
		{
			found = new eventStruct;
			if( found == 0)
			{
				ERROR("%s", "Error occured while allocating memory for eventStruct");
				ERROR("%s", "Exiting ReportEvent");
				m_srctMutex.release();
				return -1;
			}
			found->eventId = specificProblem;
			found->next = NSFEventList;
			strcpy(found->objRef,objectOfReference);
			NSFEventList = found;
		}
		found->reportTime = currTime;
	}

	bResult = ACS_NSF_AEH::ReportEvent(specificProblem,
			severity,
			probableCause,
			objectOfReference,
			problemData,
			problemText,
			true
			);
	if( bResult == -1 )
	{
		ERROR("%s", "Error occured while reporting event");
		ERROR("%s", "Exiting ReportEvent");

	}
	m_srctMutex.release();
	return bResult;
}

/*=====================================================================
ROUTINE: SendAlarm
=====================================================================*/
int SendAlarm(string IPAddrStr,int gw,int node,int cease)
{
	DEBUG("Entering Send Alarm function with gw = %d, node = %d, cease = %d", gw, node, cease);

	ACE_UINT32 specificProblem = 4;
	string severity = "A2";
	string probableCause = "AP EXTERNAL NETWORK FAULT";
	string objRef = IPAddrStr;
	string problemData = "Connection to remote gateway is down";
	string problemText = "PROBLEM\nAP EXTERNAL NETWORK";
	string objRef1 = "A/";

	if (node == 2)
	{
		objRef1 = "B/";
	}

	switch (gw)
	{
		case 0: // O2
			specificProblem = 5;
			severity = "O2";
			probableCause = "AP NETWORK SURVEILLANCE STOPPED";  // TR HK86874
			objRef = "Both";
			problemData = "Network surveillance of external network is stopped";
			problemText = "PROBLEM\nNETWORK SURVEILLANCE OF EXTERNAL NETWORK IS STOPPED"; // TR HK86874
			break;

		case 1: // A1
			severity = "A1";
			objRef = "Both";
			problemData = "All external connections are down";
			break;

		case 2: // A2 active node
			objRef = "Active";
			problemData = "All external connections from active node are down";
			break;

		default: // A2 specific GW
			break;
	} // End of switch

	if (cease != 0)
	{
		severity = "CEASING";
	}
	ACE_INT32 bResult = 0;
	objRef = objRef1 + objRef;
	bResult = ReportEvent(	specificProblem,
			severity.c_str(),
			probableCause.c_str(),
			objRef.c_str(),
			problemData.c_str(),
			problemText.c_str());


	if (bResult)
	{
		DEBUG("%s", "Report Event Logged successfully.");
	}
	else
	{
		ERROR("%s", "Report Event Logging Failed");
	}

	if (cease != 0)
	{
		DEBUG("%s", "Exiting Send Alarm function");
		return 0;
	}

	ACE_UINT32 mask = 1 << gw;
	ACE_UINT32 alarmState = 0;

	if (node == 1) // A-node
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,alarmState) == 0)
		{
			alarmState = alarmState | mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_A,alarmState) == 0)
			{
				DEBUG("%s", "Exiting Send Alarm function");
				return 0;
			}
		}

	}
	else // B-node
	{

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,alarmState) == 0)
		{
			alarmState = alarmState | mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_B,alarmState) == 0)
			{
				DEBUG("%s", "Exiting Send Alarm function");
				return 0;
			}
		}
	}

	ERROR("%s","Exiting Send Alarm function");
	return -1; // Error
}

/*=====================================================================
ROUTINE: SendAlarm2
=====================================================================*/
int SendAlarm2(string IPAddrStr,int gw,int node,int cease)
{
	DEBUG("%s","Entering SendAlarm2 function");

	ACE_UINT32 specificProblem = 4;
	string severity = "A2";
	string probableCause = "AP EXTERNAL NETWORK FAULT";
	string objRef = IPAddrStr;
	string problemData = "Connection to remote gateway on network 2 is down";
	string problemText = "PROBLEM\nAP EXTERNAL NETWORK 2";
	string objRef1 = "A/";

	if (node == 2)
	{
		objRef1 = "B/";
	}

	switch (gw)
	{
		case 0: // O2
			specificProblem = 5;
			severity = "O2";
			probableCause = "AP NETWORK SURVEILLANCE STOPPED";  // TR HK86874
			objRef = "Both";
			problemData = "Network surveillance of external network is stopped";
			problemText = "PROBLEM\nNETWORK SURVEILLANCE OF EXTERNAL NETWORK IS STOPPED"; // TR HK86874
			break;

		case 1: // A1
			severity = "A1";
			objRef = "Both on Network 2";
			problemData = "All external connections on network 2 are down";
			break;

		case 2: // A2 active node
			objRef = "Active on Network 2";
			problemData = "All external connections on network 2 from active node are down";
			break;

		default: // A2 specific GW
			break;
	} // End of switch

	if (cease != 0)
	{
		severity = "CEASING";
	}
	ACE_INT32 bResult = 0;
	objRef = objRef1 + objRef;
	bResult = ReportEvent(	specificProblem,
			severity.c_str(),
			probableCause.c_str(),
			objRef.c_str(),
			problemData.c_str(),
			problemText.c_str());


	if (bResult)
	{
		DEBUG("%s", "Report Event Logged successfully.");
	}
	else
	{
		ERROR("%s", "Report Event Logging Failed");
	}

	if (cease != 0)
	{
		DEBUG("%s", "Exiting SendAlarm2 function");
		return 0;
	}

	ACE_UINT32 mask = 1 << gw;
	ACE_UINT32 alarmState2 = 0;

	if (node == 1) // A-node
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A,alarmState2) == 0)
		{
			alarmState2 = alarmState2 | mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_NET2_A,alarmState2) == 0)
			{
				DEBUG("%s", "Exiting SendAlarm2 function");
				return 0;
			}
		}

	}
	else // B-node
	{

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B,alarmState2) == 0)
		{
			alarmState2 = alarmState2 | mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_NET2_B,alarmState2) == 0)
			{
				DEBUG("%s", "Exiting SendAlarm2 function");
				return 0;
			}
		}
	}

	ERROR("%s","Exiting SendAlarm2 function");
	return -1; // Error
}

/*=====================================================================
ROUTINE: SendAlarmCableLess
=====================================================================*/
int SendAlarmCableLess(bool isActiveNode, int node, int cease)
{
	DEBUG("Entering SendAlarmCableLess function with isActiveNode = %d, node = %d, cease = %d", 
			(int) isActiveNode, node, cease);

	ACE_UINT32 specificProblem = 5;
	string severity = (isActiveNode) ? "A1" : "A2";
	string probableCause = "AP FAULT";
	string objRef = string((node == 2) ? "B/" : "A/") + "acs_nsfserverd/Alarm";
	string problemData = "bond1 interface is down";
	string problemText = "PROBLEM\nBACKPLANE MALFUNCTION";

	if (cease != 0)
	{
		severity = "CEASING";
	}
	
	ACE_INT32 bResult = 0;
	bResult = ReportEvent(specificProblem,
			severity.c_str(),
			probableCause.c_str(),
			objRef.c_str(),
			problemData.c_str(),
			problemText.c_str());


	if (bResult)
	{
		DEBUG("%s", "Report alarm successfully.");
		ERROR("%s", "Exiting SendAlarmCableLess function");
		return 0;
	}
	else
	{
		ERROR("%s", "Report alarm Failed");
		ERROR("%s", "Exiting SendAlarmCableLess function");
		return -1;
	}
}

/*=====================================================================
ROUTINE: CeaseAlarm
=====================================================================*/
int CeaseAlarm(string IPAddrStr,int gw,int node)
{

	DEBUG("%s", "Entering CeaseAlarm");

	SendAlarm(IPAddrStr,gw,node,1); // send CEASING

	ACE_UINT32 mask = 1 << gw;
	ACE_UINT32 alarmState = 0;
	if (node == 1) // A-node
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);

		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,alarmState) == 0)
		{
			alarmState = alarmState ^ mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_A,alarmState) == 0)
			{
				DEBUG("%s", "Exiting CeaseAlarm");
				return 0;
			}
		}
	}
	else // B-node
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,alarmState) == 0)
		{
			alarmState = alarmState ^ mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_B,alarmState) == 0)
			{
				DEBUG("%s", "Exiting CeaseAlarm");
				return 0;
			}
		}
	}
	ERROR("%s", "Error occured while ceasing alarm");
	ERROR("%s", "Exiting CeaseAlarm");
	return -1; // Error

}

/*=====================================================================
ROUTINE: CeaseAlarm2
=====================================================================*/
int CeaseAlarm2(string IPAddrStr,int gw,int node)
{

	DEBUG("%s", "Entering CeaseAlarm2");

	SendAlarm2(IPAddrStr,gw,node,1); // send CEASING

	ACE_UINT32 mask = 1 << gw;
	ACE_UINT32 alarmState2 = 0;
	if (node == 1) // A-node
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);

		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A,alarmState2) == 0)
		{
			alarmState2 = alarmState2 ^ mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_NET2_A,alarmState2) == 0)
			{
				DEBUG("%s", "Exiting CeaseAlarm2");
				return 0;
			}
		}
	}
	else // B-node
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
		if (ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B,alarmState2) == 0)
		{
			alarmState2 = alarmState2 ^ mask;
			if (ACS_NSF_Server::objCommon.setRegWord(NSF_ALARMSTATE_NET2_B,alarmState2) == 0)
			{
				DEBUG("%s", "Exiting CeaseAlarm2");
				return 0;
			}
		}
	}
	ERROR("%s", "Error occured while ceasing alarm");
	ERROR("%s", "Exiting CeaseAlarm2");
	return -1; // Error

}

/*=====================================================================
ROUTINE: controlAlarmBit
=====================================================================*/
int controlAlarmBit(int gw, int network, int node, bool mark)
{
	DEBUG("Entering controlAlarmBit gw = %d network = %d node = %d mark = %d", 
			gw, network, node, mark);
	
	ACE_UINT32 alarmState = 0;
	ACE_UINT32 mask = 1 << gw;					
	string valueName = "";
	string action = (mark) ? "marked" : "unmarked";
	
	if (network == 1)
	{
		valueName = (node == NODE_A) ? NSF_ALARMSTATE_A : NSF_ALARMSTATE_B;
	}
	else
	{
		valueName = (node == NODE_A) ? NSF_ALARMSTATE_NET2_A : NSF_ALARMSTATE_NET2_B;
	}					
	
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_srctMutex);
	if (ACS_NSF_Server::objCommon.getRegWord(valueName, alarmState) == 0)
	{	
		if (mark)
		{
			alarmState = alarmState | mask;
		}
		else
		{
			alarmState = alarmState ^ mask;
		}
		
		if (ACS_NSF_Server::objCommon.setRegWord(valueName, alarmState) == 0)
		{
			DEBUG("%s alarm bit successfully", action.c_str());			
			return 0;
		}
		else
		{
			WARNING("Failed to %s alarm bit", action.c_str());
		}
	}
	else
	{
		WARNING("%s", "Failed to get alarm bit");					
	}	

	DEBUG("%s", "controlAlarmBit");
	return -1; // Error
}

/*=====================================================================
ROUTINE: Supervise
=====================================================================*/
void Supervise(void* value)
{
	DEBUG("%s", "Entering Supervise");

	//Socket desciptor.
	int s;

	gwStruct* gwInfo = (gwStruct*) value;

	INFO("Thread starting the pinging for the gateway - %s", (gwInfo->IPAddrStr).c_str());

	const ACE_TCHAR* target_addr = gwInfo->IPAddrStr.c_str();
	if(!target_addr)
	{
		ERROR("%s","Error in getting target IP address");
		ERROR("%s", "Exiting Supervise");
		return ;
	}
	ACE_INET_Addr ping_addr;
	if( ping_addr.set((u_short)0, target_addr)!= 0)
	{
		ERROR("%s","Error in assigning the address");
		ACS_NSF_Server::objCommon.reportError(1, "Unable to assign the address. Error:", ACS_APGCC::getLastError());
		ERROR("%s", "Exiting Supervise");
		return ;
	}
	if ( (s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		ERROR("%s","Error oocured while creating socket.");
		ERROR("%s", "Exiting Supervise");
		return ;
	}

	ACE_UINT32 respCounter = 0;
	ACE_UINT32 noRespCounter = 0;
	ACE_UINT32 alarmSent = 0;	
	ACE_UINT32 alarmState = 0;
	ACE_UINT32 mask = 1;
	ACE_UINT32 alarmBitMarked = 0;

	if (currentNode == NODE_A) // A
	{
		if( gwInfo->network == 1 )
		{
			ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_A,alarmState);
		}
		else
		{
			ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_A,alarmState);
		}

	}
	else // B
	{
		if( gwInfo->network == 1 )
		{

			ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_B,alarmState);
		}
		else
		{

			ACS_NSF_Server::objCommon.getRegWord(NSF_ALARMSTATE_NET2_B,alarmState);
		}
	}

	mask = mask << gwInfo->gwIndex;
	if ((alarmState & mask) != 0)
	{
		alarmSent = 1;
	}

	int forever = 1;

	// Start supervision and continue until service is terminated
	while (forever)
	{
		// Sleep until timer expires or stop event signalled
		DEBUG("Supervise(network = %u - index = %u) - Started waiting for stop event", gwInfo->network, gwInfo->gwIndex - 3);
		
		ACE_Time_Value waitTime(ACS_NSF_Server::objCommon.pingPeriod, 0);
		fd_set readHandleSet;
		FD_ZERO(&readHandleSet);
		FD_SET(gwInfo->stopEvent[0], &readHandleSet);
		int ret = ACE_OS::select(gwInfo->stopEvent[0]+1, &readHandleSet, NULL, NULL, &waitTime);

		if (ret < 0)
		{
			if (ACE_OS::last_error() == EINTR)
			{
				ERROR("%s", "EINTR in termination of gateway supervision.");
				break;
			}
			else
			{
				ERROR("%s", "Error occurred in select system call");
				break;
			}
		}
		else if (ret > 0)
		{
			if (FD_ISSET(gwInfo->stopEvent[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signalled for termination of gateway supervision.");
				break;
			}
		}
		
		DEBUG("Supervise(network = %u - index = %u) - Stopped waiting for stop event due to time-out", gwInfo->network, gwInfo->gwIndex - 3);
		
		
		ACE_UINT32 startStopFlag = 0;

		if (!ACS_NSF_Server::objCommon.calculateStartStopValue(ACS_NSF_Server::objCommon.isSurveillanceEnabled))
		{
			ERROR("%s", "Error occurred while calculating start/stop value");
			ERROR("%s", "Exiting Supervise");
			::close(s);
			return;
		}
		startStopFlag = ACS_NSF_Server::objCommon.getStartStopValue();


		// While NSF activated, send a ping and wait for the response
		if (startStopFlag == 3) // Active
		{
			DEBUG("%s","Sending an echo");
			int ping_success = 0;
#ifdef _NSF_PING_SIMULATION
			int rval = ping(gwInfo->network);
#else
			int rval = ping(target_addr,s);
#endif
			DEBUG("Return value of ping function %d", rval);
			if (rval > 0)
			{
				ping_success = 1;
				DEBUG("ECHO_REPLY received from %s", target_addr);
			}
			else
			{
				if (isStopPingThread)
				{
					INFO("%s","Termination is requested");
					break;
				}
				ping_success = 0;
				if( target_addr != 0 )
				{
					DEBUG("Unable to receive ECHO reply for %s", target_addr);
				}
			}
			if (ping_success == 0)
			{
				noRespCounter++;
				respCounter = 0;

				if( target_addr != 0 )
				{
					DEBUG("Pinging failed for %s for %u time", target_addr, noRespCounter);
				}

				// TR HS48771
				// In case of passive node, mark the gateway alarm bit set 
				// but not actually send alarm. The alarm will be sent 
				// at nth (n=ACS_NSF_Server::objCommon.routerResponse) attempt
				if (ACS_NSF_Server::objCommon.getActiveNodeFlag() == false && alarmBitMarked == 0)
				{
					if (controlAlarmBit(gwInfo->gwIndex, gwInfo->network, currentNode, true) == 0)
					{
						alarmBitMarked = 1;									
					}
				}
				
				if (noRespCounter >= ACS_NSF_Server::objCommon.routerResponse)
				{
					ACE_INT32 bResult = -1;

					string objRef = "ICMP/" + gwInfo->IPAddrStr;
					string s1 = "echo failed for " + gwInfo->IPAddrStr;

					bResult = ReportEvent(1,"EVENT","AP INTERNAL FAULT",objRef.c_str(),s1.c_str(),"ICMP problem");

					if (bResult)
					{
						INFO("%s", "Report Event is Logged");
					}
					else
					{

						ERROR("%s", "Report Event Logging Failed");
					}

					if (alarmSent == 0)
					{
						if (gwInfo->network == 1)
						{
							SendAlarm(gwInfo->IPAddrStr,gwInfo->gwIndex,currentNode,0);
						}
						else 
						{
							SendAlarm2(gwInfo->IPAddrStr,gwInfo->gwIndex,currentNode,0);
						}
						alarmSent = 1;
					}
					noRespCounter = ACS_NSF_Server::objCommon.routerResponse;
				}
			}
			else
			{
				respCounter++;
				noRespCounter = 0;

				DEBUG("Pinging successfully for %s for %u time", target_addr, respCounter);

				if (respCounter >= ACS_NSF_Server::objCommon.routerResponse)
				{	
					if (alarmSent != 0)
					{
						if( gwInfo->network == 1)
						{
							CeaseAlarm(gwInfo->IPAddrStr,gwInfo->gwIndex,currentNode);
						}
						else
						{
							CeaseAlarm2(gwInfo->IPAddrStr,gwInfo->gwIndex,currentNode);
						}
						alarmSent = 0;
						// TR HS48771
						// Unmark the alarm bit after nth success in a row
						// Note: the alarm bit is unmarked in the CeaseAlarm/2 function.						
						alarmBitMarked = 0;
					}
					else if (ACS_NSF_Server::objCommon.getActiveNodeFlag() == false && alarmBitMarked != 0)
					{
						// TR HS48771
						// Unmark the alarm bit after nth success in a row						
						if (controlAlarmBit(gwInfo->gwIndex, gwInfo->network, currentNode, false) == 0)
						{
							alarmBitMarked = 0;
						}						
					}
				}
			}
		}
		else
		{	
			if (alarmSent != 0)
			{
				if( gwInfo->network == 1 )
				{
					CeaseAlarm(gwInfo->IPAddrStr,gwInfo->gwIndex,currentNode);
				}
				else
				{
					CeaseAlarm2(gwInfo->IPAddrStr,gwInfo->gwIndex,currentNode);
				}
				alarmSent = 0;
				// TR HS48771
				// Unmark the alarm bit after nth success in a row
				// Note: the alarm bit is unmarked in the CeaseAlarm/2 function.						
				alarmBitMarked = 0;
			}
			else if (ACS_NSF_Server::objCommon.getActiveNodeFlag() == false && alarmBitMarked != 0)
			{
				// TR HS48771
				// Unmark the alarm bit after nth success in a row				
				if (controlAlarmBit(gwInfo->gwIndex, gwInfo->network, currentNode, false) == 0)
				{
					alarmBitMarked = 0;
				}						
			}
		}
		DEBUG("%s", "NSF PINGING COMPLETED for the gateway");
	} // End of while forever

	::close(s); // Closing Socket
	DEBUG("%s", "Exiting Supervise");
}

/*=====================================================================
ROUTINE: SuperviseForCableLess
=====================================================================*/
void SuperviseForCableLess(void* value)
{
	DEBUG("%s", "Entering SuperviseForCableLess");
	gwStruct* gwInfo = (gwStruct*) value;
	
	ACE_UINT32 bondingState = 0;
	ACE_UINT32 bondingStatus = 0;
	ACE_UINT32 numOfTime = 0;
	bool forever = true;

	bool isActiveNode = ACS_NSF_Server::objCommon.getActiveNodeFlag();
	bool alarmSent = false;
	// Start supervision and continue until service is terminated
	while (forever)
	{	
		// Sleep until timer expires or stop event signalled
		DEBUG("%s", "SuperviseForCableLess() - Started waiting for stop event");
		
		ACE_Time_Value waitTime(ACS_NSF_Server::objCommon.pingPeriod, 0);
		fd_set readHandleSet;
		FD_ZERO(&readHandleSet);
		FD_SET(gwInfo->stopEvent[0], &readHandleSet);
		int ret = ACE_OS::select(gwInfo->stopEvent[0]+1, &readHandleSet, NULL, NULL, &waitTime);

		if (ret < 0)
		{
			if (ACE_OS::last_error() == EINTR)
			{
				ERROR("%s", "EINTR in termination of gateway supervision.");
				break;
			}
			else
			{
				ERROR("%s", "Error occurred in select system call");
				break;
			}
		}
		else if (ret > 0)
		{
			if (FD_ISSET(gwInfo->stopEvent[0], &readHandleSet))
			{
				ERROR("%s", "Stop event signalled for termination of gateway supervision.");
				break;
			}
		}
		
		DEBUG("%s", "SuperviseForCableLess() - Stopped waiting for stop event due to time-out");

		// Check the bonding interface status
		DEBUG("%s", "Checking the bonding status");
		
		// Check bonding status
		bondingStatus = checkBondingStatus();
		INFO("Bonding interface %s status: %d", NSF_CBL_BONDING_INTERFACE, bondingStatus);
		
		// Get the value from config file	
		ACS_NSF_Server::objCommon.getRegWord((currentNode == NODE_A) ? NSF_CBL_BONDINGSTATE_A : NSF_CBL_BONDINGSTATE_B, bondingState);
		ACS_NSF_Server::objCommon.getRegWord((currentNode == NODE_A) ? NSF_CBL_NUMOFTIMES_A : NSF_CBL_NUMOFTIMES_B, numOfTime);
		if (bondingState == bondingStatus)
		{
			numOfTime++;
		}
		else
		{
			bondingState = bondingStatus;
			numOfTime = 1;
		}
		
		// Raise or cease alarm if needed		
		if (bondingState == 0 && numOfTime >= ACS_NSF_Server::objCommon.routerResponse) // UP
		{
			if (alarmSent == true)
			{
				SendAlarmCableLess(isActiveNode, currentNode, 1);
				alarmSent = false;
			}
		}
		else if (bondingState == 1 && numOfTime >= ACS_NSF_Server::objCommon.routerResponse) // DOWN
		{
			if (alarmSent == false)
			{
				SendAlarmCableLess(isActiveNode, currentNode, 0);
				alarmSent = true;
			}
		}
		
		ACS_NSF_Server::objCommon.setRegWord((currentNode == NODE_A) ? NSF_CBL_BONDINGSTATE_A : NSF_CBL_BONDINGSTATE_B, bondingState);
		ACS_NSF_Server::objCommon.setRegWord((currentNode == NODE_A) ? NSF_CBL_NUMOFTIMES_A : NSF_CBL_NUMOFTIMES_B, numOfTime);
		
		DEBUG("%s", "Checking bonding status COMPLETED");
	} // End of while forever
	
	// Cease alarm if needed
	if (alarmSent == true)
	{
		SendAlarmCableLess(isActiveNode, currentNode, 1);
		alarmSent = false;
	}
	DEBUG("%s", "Exiting SuperviseForCableLess");
	return;
}

/*=====================================================================
ROUTINE: SuperviseGateway
=====================================================================*/
ACE_THR_FUNC_RETURN SuperviseGateway(void* value)
{
	DEBUG("%s", "Entering SuperviseGateway");

	//DEBUG("%s", "Invoking Supervise");

	// Determine if it's running on cable-less environment	
	if (ACS_NSF_Server::objCommon.GetApgOamAccess() == 1)
	{
		SuperviseForCableLess(value);
	}
	else
	{
		Supervise(value);
	}

	DEBUG("%s", "Exiting SuperviseGateway");
	return 0;
}

/*=====================================================================
ROUTINE: NSFFunctionalMethod
=====================================================================*/
ACE_THR_FUNC_RETURN ACS_NSF_Server::NSFFunctionalMethod(void* aNSFPtr)
{
	DEBUG("%s", "Entering NSFFunctionalMethod");
	ACS_NSF_Server *myNSFPtr = NULL;
	myNSFPtr	= reinterpret_cast<ACS_NSF_Server *>(aNSFPtr);
	if(myNSFPtr != NULL)
	{
		myNSFPtr->StartServer();
	}
	else
	{
		ERROR("%s", "ACS_NSF_Server is NULL");
		ERROR("%s", "Exiting NSFFunctionalMethod");
		return 0;
	}

	if( myNSFPtr->m_threadGroupId != -1 )
	{
		DEBUG( "%s", "Waiting for threads to come down" );
		DEBUG( "Thread Group Id:%d", myNSFPtr->m_threadGroupId );
		ACE_Thread_Manager::instance()->wait_grp(myNSFPtr->m_threadGroupId);
		//DEBUG("%s","Canceling threads");
		//ACE_Thread_Manager::instance()->cancel(myNSFPtr->m_threadGroupId);
		//DEBUG("%s","Killing threads");
		//ACE_Thread_Manager::instance()->kill( myNSFPtr->m_threadGroupId,SIGINT); 

	}

	DEBUG("%s", "Exiting NSFFunctionalMethod");
	return 0;
}
/*=====================================================================
ROUTINE: setupNSFThread
=====================================================================*/
ACS_CC_ReturnType ACS_NSF_Server::setupNSFThread( ACS_NSF_Server * aNSFPtr)
{
	DEBUG("%s", "Entering setupNSFThread");

	int  mythread = -1;

	mythread = ACE_Thread_Manager::instance()->spawn(&NSFFunctionalMethod,
			(void*)aNSFPtr ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&aNSFPtr->NSFFunctionalThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if(mythread == -1)
	{
		ERROR("%s", "Error occured while spawning NSFFunctionalThread");
		ERROR("%s", "Exiting setupNSFThread");
		return ACS_CC_FAILURE;
	}
	DEBUG("%s", "Exiting setupNSFThread");
	return ACS_CC_SUCCESS;
}

/*=====================================================================
ROUTINE: StartOIThreadsOnActiveNode
=====================================================================*/
ACS_CC_ReturnType ACS_NSF_Server::StartOIThreadsOnActiveNode() 
{
	DEBUG("%s", "Entering StartOIThreadsOnActiveNode");

	//DEBUG("%s", "Creating IMMcallbackThread thread.");
	ACE_thread_t immCallBackThreadId;
	m_threadGroupId = ACE_Thread_Manager::instance()->spawn(&setupIMMCallBacksThreadFunc,
			/*(void*)parm*/ (void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&immCallBackThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			m_threadGroupId,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if( m_threadGroupId == -1)
	{
		ERROR("%s", "Error occurred while spawning IMMcallbackThread");
		ERROR("%s", "Exiting StartOIThreadsOnActiveNode");
		return ACS_CC_FAILURE;
	}
	DEBUG("%s", "IMMcallbackThread thread created successfully.");

	return ACS_CC_SUCCESS;
}
