/*
 * ACS_TRAPDS_Server.cpp
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */


#include "ACS_TRAPDS_Server.h"

/*============================================================================
	ROUTINE: ACS_TRAPDS_Server
 ============================================================================ */
ACS_TRAPDS_Server::ACS_TRAPDS_Server()
{
	m_StopEvent = eventfd(0,0);


	// Create the IMM handler object
	IMM_Handler = new (std::nothrow) ACS_TRAPDS_ImmHandler();

	_trace = new ACS_TRA_trace("TRAPDS Server");

}

/*============================================================================
	ROUTINE: ~ACS_TRAPDS_Server
 ============================================================================ */
ACS_TRAPDS_Server::~ACS_TRAPDS_Server()
{

	if (IMM_Handler->getSvcState())
	{
		stopImmHandler();
	}

	if(NULL != IMM_Handler)
		delete IMM_Handler;

	ACS_TRAPDS_SNMPTrapReceiver::getInstance()->finalize();

	close(m_StopEvent);

	delete(_trace);
}

bool ACS_TRAPDS_Server::run()
{
	if (fxInitialize())
	{
		sleep(1);
		startWorkerThreads();
	}

	return true;
}

/*============================================================================
	ROUTINE: startImmThread
 ============================================================================ */
bool ACS_TRAPDS_Server::startImmThread()
{
	bool result = true;

		// Start the IMM handler thread
		if( IMM_Handler->open() != 0 )
		{
			if (_trace->ACS_TRA_ON())
			{ //trace
				char tmpStr[512] = {0};
				snprintf(tmpStr, sizeof(tmpStr) - 1, "startWorkerThreads, error on start DSDHandle");
				_trace->ACS_TRA_event(1, tmpStr);
			}
			result = false;
		}

		if (_trace->ACS_TRA_ON())
		{ //trace
			char tmpStr[512] = {0};
			snprintf(tmpStr, sizeof(tmpStr) - 1, "Leaving startWorkerThreads()");
			_trace->ACS_TRA_event(1, tmpStr);

		}

		return result;
}

/*============================================================================
	ROUTINE: startWorkerThreads
 ============================================================================ */
bool ACS_TRAPDS_Server::startWorkerThreads()
{
	bool result = false;

	if (fetchSubscribers() == 0)
	{
		if (startTrapManager() == 0)
		{
			result = true;
		}
	}

	return result;
}

/*============================================================================
	ROUTINE: stopWorkerThreads
 ============================================================================ */
bool ACS_TRAPDS_Server::stopWorkerThreads()
{

	bool result = true;
	/*
	if (IMM_Handler->getSvcState())
	{
		result = stopImmHandler();
	}

	 */
	return result;
}

/*============================================================================
	ROUTINE: stopImmHandler
 ============================================================================ */
bool  ACS_TRAPDS_Server::stopImmHandler()
{
	bool result = true;

	ACE_UINT64 stopEvent=1;
	ssize_t numByte;
	ACE_INT32 eventfd;


	IMM_Handler->getStopHandle(eventfd);

	// Signal to IMM thread to stop
	numByte = write(eventfd, &stopEvent, sizeof(ACE_UINT64));

	if(sizeof(ACE_UINT64) != numByte)
	{
		result = false;
	}

	return result;
}


bool ACS_TRAPDS_Server::fxInitialize()
{
	bool success = false;

	//create class TrapSubscriber if is not created yet
	//	success = IMM_TRAPDS_Util::createClassTrapSubscriber();

	if (startImmThread())
	{
		if (assignTrapPort() == 0) success = true;
	}

	return success;
}

int ACS_TRAPDS_Server::startTrapManager ()
 {
	int result = 0;

	ACS_TRAPDS_SNMPTrapReceiver::getInstance()->initialize();

	usleep(300000);
	if (ACS_TRAPDS_SNMPTrapReceiver::getInstance()->isRunning())
	{
		std::cout << "\n------------------------------------------------------------"<< std::endl;
		std::cout << "                 TRAP Receiver is running !!!               " << std::endl;
		std::cout << "------------------------------------------------------------\n"<< std::endl;

	}
	else
	{
		std::cout << "\n------------------------------------------------------------"<< std::endl;
		std::cout << "          ERROR: TRAP Receiver is NOT running !!!             " << std::endl;
		std::cout << "------------------------------------------------------------\n"<< std::endl;
		result = -1;
	}

	return result;

 }

int ACS_TRAPDS_Server::fetchSubscribers ()
 {
	int result = 0;

	std::vector<std::string> subscribList;
	if (!IMM_TRAPDS_Util::getClassObjectsList(IMM_TRAPDS_Util::classTrapSubscriber,subscribList))
	{
		std::cout << "\n------------------------------------------------------------"<< std::endl;
		std::cout << "				  - NO SUBSCRIBERS INSTANCES - 					"<< std::endl;
		std::cout << "------------------------------------------------------------\n"<< std::endl;
	}
	else
	{
		for (unsigned i=0; i<subscribList.size();i++)
		{
			std::string dn_subcribe("");
			dn_subcribe = subscribList[i].c_str();

			//## -- PRINT FOR DEBUG -- ##
			IMM_TRAPDS_Util::printDebugObject(dn_subcribe);
			//## -- PRINT FOR DEBUG -- ##

			std::string rdn("");
			IMM_TRAPDS_Util::getRdnObject(dn_subcribe,rdn);

			std::string name("");
			IMM_TRAPDS_Util::getImmAttributeString(dn_subcribe,IMM_TRAPDS_Util::ATT_TRAPDS_NAME,name);

			int port = 0;
			IMM_TRAPDS_Util::getImmAttributeInt(dn_subcribe,IMM_TRAPDS_Util::ATT_TRAPDS_PORT,port);

			ACS_TRAPDS_ServerConnection *obs = new ACS_TRAPDS_ServerConnectionImpl(name,port);
			ACS_TRAPDS_SNMPTrapReceiver::getInstance()->addSubscriber(obs, rdn);
		}
	}

	return result;
 }

void ACS_TRAPDS_Server::printError(std::string mess,const char *func,int line)
{
	char tmpStr[512] = {0};
	snprintf(tmpStr, sizeof(tmpStr) - 1,"[%s@%d] %s",func,line,mess.c_str());
	printf("%s",tmpStr);
	if (_trace->ACS_TRA_ON()) _trace->ACS_TRA_event(1, tmpStr);
}

int ACS_TRAPDS_Server::checkTrapPort(int l)
{
	int res = 0;

	ACE_SOCK_Dgram *local_;

	res = trap_port_->set(l,"127.0.0.1");
	if (res == 0)
	{
		local_=new ACE_SOCK_Dgram();

		res = local_->open(*trap_port_);

		if (res != 0 )
		{
			delete(local_);
			local_=NULL;
			res = 1;
		}
		else
		{
			local_->close();
			delete (local_);
		}
	}

	return res;
}

int ACS_TRAPDS_Server::assignTrapPort()
{
	int res = 0;
	int trap_port = 162;
	trap_port_= new ACE_INET_Addr();

	while (true)
	{
		if (checkTrapPort(trap_port) == 0)	break;
		else sleep(1);
	}

	delete (trap_port_);

	return res;
}
