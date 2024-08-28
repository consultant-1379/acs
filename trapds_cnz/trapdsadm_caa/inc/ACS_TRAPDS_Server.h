/*
 * ACS_TRAPDS_Server.h
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_SERVER_H_
#define ACS_TRAPDS_SERVER_H_

#include <iostream>
#include <sys/eventfd.h>
#include <istream>
#include <ostream>
#include <sstream>
#include <list>
#include <string>

#include "ace/OS.h"
#include "ace/SOCK_Dgram.h"
#include "ace/INET_Addr.h"

#include "ACS_TRAPDS_ImmHandler.h"
#include "ACS_TRAPDS_SNMPTrapReceiver.h"
#include "ACS_TRAPDS_ServerConnectionImpl.h"
#include "ACS_TRAPDS_ServerConnection.h"
#include "ACS_TRAPDS_Util.h"
#include "ACS_TRA_trace.h"


//class FIXS_CCH_ImmHandler;
/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

class ACS_TRAPDS_Server {

 public:


	ACS_TRAPDS_Server();

	virtual ~ACS_TRAPDS_Server();

	bool run();

	bool startWorkerThreads();

	bool startImmThread();

	bool stopWorkerThreads();

	bool waitOnShotdown();

	int& getStopHandle() { return m_StopEvent; };

	void printError(std::string mess,const char *func,int line);

 private:

	bool fxInitialize();

	int startTrapManager();

	int fetchSubscribers();

	int assignTrapPort();

	int checkTrapPort(int l);

	/**
		@brief		Handler to IMM objects
	 */
	ACS_TRAPDS_ImmHandler* IMM_Handler;

	/**
		@brief	fms_cpf_serverTrace
	 */
	ACS_TRA_trace* _trace;

	/**
			@brief	m_StopEvent : signal to internal thread to exit
	 */
	int m_StopEvent;

	ACE_INET_Addr *trap_port_;
	/**
		@brief	stopImmHandler
		stop the IMM handler thread
	*/
	bool stopImmHandler();


};


#endif /* ACS_TRAPDS_SERVER_H_ */
