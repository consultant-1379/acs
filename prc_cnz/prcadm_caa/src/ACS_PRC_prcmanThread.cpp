/*
 * ACS_PRC_prcmanThread.cpp
 *
 *  Created on: Nov 22, 2010
 *      Author: xpaomaz
 */

#include "ACS_PRC_prcmanThread.h"
#include "ACS_PRC_ispapi.h"
#include "ACS_PRC_RunTimeOwnerThread.h"
#include "ACS_TRA_Logging.h"
#include "ACS_PRC_NotificationThread.h"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

ACS_PRC_prcmanThread::ACS_PRC_prcmanThread() {
	// TODO Auto-generated constructor stub

}

ACS_PRC_prcmanThread::~ACS_PRC_prcmanThread() {
	// TODO Auto-generated destructor stub
}

int ACS_PRC_prcmanThread:: svc ( void ){

	string this_hostname;
	string peer_hostname;

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	char strErr_no[1024] = {0};

	ifstream ifs;

	ifs.open("/etc/cluster/nodes/this/hostname");
	if ( ifs.good())
		getline(ifs,this_hostname);

	ifs.close();

	ifs.open("/etc/cluster/nodes/peer/hostname");

	if ( ifs.good())
		getline(ifs,peer_hostname);

	ifs.close();

	ACE_INET_Addr sockAddress(8881,this_hostname.c_str());
	ACE_SOCK_Acceptor sockAcceptor;
	ACE_SOCK_Stream sockStream;
	char buffer[128] = {0};
	Ispapi Prc_ISP_Log;

	Prc_ISP_Log.open();

	while (sockAcceptor.open (sockAddress) == -1){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - ACE_SOCK_Acceptor - sockAcceptor.open fails - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);

		sockAcceptor.close();

		sleep ( 2 );
	}

	while (! stopPrcmanThread){

		/*if ( sockAcceptor.accept(sockStream,0,0,0) == -1 ){
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - ACE_SOCK_Acceptor - sockAcceptor.accept fails - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
			Logging.Write(strErr_no, LOG_LEVEL_ERROR); */
                //changes HV92897 start

		ACE_Time_Value val(5);

		if (sockAcceptor.accept(sockStream,0,&val,0) != 0 ){

		    int error = ACE_OS::last_error();
		    sockStream.close();

            if (( error == EINTR ) || ( error == ETIMEDOUT ) || ( error == ETIME ))
		          continue;

            memset(strErr_no, 0, sizeof(strErr_no));
            snprintf(strErr_no,1024,"acs_prcmand %s - ACE_SOCK_Acceptor - sockAcceptor.accept fails - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
            Logging.Write(strErr_no, LOG_LEVEL_ERROR); 

            sockAcceptor.close();

            usleep(200000); // wait 0.2 second
            sockAcceptor.open(sockAddress);
            //Changes HV92897 End

            continue;
		}
		else {

			ACE_Time_Value val(1);

			if ( sockStream.recv(buffer,128,&val) > 0 ){

				if ( strcmp(buffer,"SHUTDOWN") == 0){ // Other node has been rebooted

					Prc_ISP_Log.otherNodeShutDown(peer_hostname.c_str(), manualRebootInitiated);

					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_prcmanThread - PRCMAN has received a communication that the other node is rebooted", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				}
				else if ( strcmp(buffer,"GETNODESTATE") == 0){
					if ( ACS_PRC_NotificationThread::getNodeState() == 1 ){
						sockStream.send_n ( "ACTIVE", 7 );
					}
					else if ( ACS_PRC_NotificationThread::getNodeState() == 2 ){
						sockStream.send_n ( "STAND-BY", 9 );
					}

					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_prcmanThread - PRCMAN has received a request for the node state", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);

				}
				else if ( strcmp(buffer,"REBOOT_PRCEVA") == 0){
					ACS_PRC_API prcapi;

					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_prcmanThread - PRCMAN has received a request to order a failover due to PRCEVA", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);

					prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);
				}
				else if ( strcmp(buffer,"PING") == 0){
					sockStream.send_n ( "PRCMAN_UP", 10 );
					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_prcmanThread - PRCMAN has received a PING to calculate the runlevel", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_INFO);
				}
				else {
					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_prcmanThread - PRCMAN has received an unexpected message", PRCBIN_REVISION );
					Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				}

			}
			else {
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_prcmanThread - PRCMAN has received a CONNECT to calculate the runlevel", PRCBIN_REVISION );
				Logging.Write(strErr_no, LOG_LEVEL_TRACE);
			}

			sockStream.close();

		}

	}/*end while 1*/

	Logging.Close();
	Prc_ISP_Log.close();
	sockStream.close();
	sockAcceptor.close();
	return 0;

}/*end svc*/
