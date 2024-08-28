/*
 * ACS_TRAPDS_API_R1.h
 *
 *  Created on: Jan 25, 2012
 *      Author: eanform
 */

#ifndef RECEIVETRAPTEST_H_
#define RECEIVETRAPTEST_H_

#include <ace/Task.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>

#include "ace/OS.h"
#include "ace/SOCK_Dgram.h"
#include "ace/INET_Addr.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/archive_exception.hpp>

#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>

#include <iosfwd>
#include "ACS_TRAPDS_StructVariable.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_OiHandler.h"
#include "ACS_CC_Types.h"
#include "ACS_APGCC_Util.H"


#define SIZE_DATA 500000
#define MIN_PORT 49154
#define MAX_PORT 49169
#define THR_SPAWNED 0x00000001
#define THR_RUNNING 0x00000002

namespace ACS_TRAPDS
{
	// ACS TRAPDS API Return Codes
	enum ACS_TRAPDS_API_Result
	{
		Result_Success =     0,
		Result_Failure =     -1,
		Result_NoFreePort =	 14
	};
}

class ACS_TRAPDS_API_R1 : public ACE_Task_Base {

public:
	 /** @brief Subscribe the caller application to trap message notification from TRAPDS server
	 *
	 *	The Service that want to subscribe for the trap handling needs
	 *	to inherit from this class providing the name for the registration.
	 *
	 *	@param[in] serviceName Name of the service that subscribes to receive the traps.
	 *	@return Return result of subscription as a ACS TRAPDS API Return Codes
	 */
	ACS_TRAPDS::ACS_TRAPDS_API_Result subscribe(std::string serviceName );

	//## Operation: handleTrap
	//	------
	//	Provides a ACS_TRAPDS_StructVariable object containing all informations of trap received.
	//	------
	//	In/Out - var is a pointer of structure that contains
	//	oids and values of the trap.
	//  This method must be implemented by the subscriber to receive the traps.
	//	------

	virtual void handleTrap(ACS_TRAPDS_StructVariable var)=0;

	ACS_TRAPDS_API_R1(int l);
	ACS_TRAPDS_API_R1();
	virtual ~ACS_TRAPDS_API_R1();
	virtual int svc(void);

	int svc_load();
	void setQueue(std::string strBuff);
	int activateThread();

protected:
	int close ();

private:
	bool bExit;
	int local_port;

	int open ();

	int checkLocalPort(int l);
	int assignLocalPort();
	void setLocalPort(int lp);
	std::string intToString (int ivalue);
	int checkSubscriberObject(std::string serviceName);
	int createSubscriberObject(std::string serviceName );
	int checkSubscriberPort(int lport );

	char *data_buf;
	ACE_INET_Addr *remote_addr_;
	ACE_INET_Addr *local_addr_;
	ACE_SOCK_Dgram *local_;

	ACE_thread_t thread_id;
	ACE_Thread_Mutex theLoadMgrMutex;
	std::list<std::string> messageQueue;
};

#endif /* RECEIVETRAPTEST_H_ */
