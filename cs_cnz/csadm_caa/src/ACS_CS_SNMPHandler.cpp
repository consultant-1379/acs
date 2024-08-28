
//	Copyright Ericsson AB 2007, 2010. All rights reserved.


#include <sstream>
#include <string>
#include <time.h>

// ACE SNMP API

//#include <asnmp/oid.h>
//#include <asnmp/enttraps.h>
//#include <asnmp/vb.h>
//#include <asnmp/address.h>
//#include <asnmp/wpdu.h>
//#include <asnmp/transaction.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_time.h>


#include "ACS_CS_Util.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_SNMPHandler.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_HostFile.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_SNMPHandler_TRACE);
#include <iostream>
#include "ACS_CS_SNMPManager.h"

using std::string;
using std::ostringstream;
using namespace ACS_CS_NS;

//class ACS_CS_Snmp_Result: public Snmp_Result
//{
//	public:
//		ACS_CS_Snmp_Result();
//		virtual ~ACS_CS_Snmp_Result();
//		virtual void result(Snmp *snmp, int result);
//		int waitForResult();
//	private:
//		int result_;
//		Snmp* snmp_;
//		ACE_Semaphore * sem;
//};
//
//ACS_CS_Snmp_Result::ACS_CS_Snmp_Result(): result_(0), snmp_(0)
//{
//	sem = new ACE_Semaphore(0);
//}
//
//ACS_CS_Snmp_Result::~ACS_CS_Snmp_Result()
//{
//	if (sem)
//	{
//		sem->remove();
//		delete sem;
//	}
//}

//void ACS_CS_Snmp_Result::result(Snmp *snmp, int result)
//{
//	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//		"(%t) ACS_CS_Snmp_Result::result()\n"
//		"Result = %d", result));
//
//	result_ = result;
//	snmp_ = snmp;
//	sem->release();
//}

//int ACS_CS_Snmp_Result::waitForResult()
//{
//	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//		"(%t) ACS_CS_Snmp_Result::waitForResult()"));
//
//	sem->acquire();
//	return result_;
//}

//--------------------

ACS_CS_SNMPHandler::ACS_CS_SNMPHandler (uint32_t ipAddressEthA, uint32_t ipAddressEthB,
		uint16_t udpPort, std::string roCommunity, std::string rwCommunity, ACS_CS_ServiceHandler* ParentService)
      :
		m_pSem(NULL),
		scbRpEthA(ipAddressEthA),
		scbRpEthB(ipAddressEthB),
		scbRpActiveAddress(ipAddressEthA),
    	agentPort(udpPort),
        mutexHandle(0),
        updateSNMPEvent(0),
        shutdownEvent(0),
        changeScbRpAddress(false),
        outQueueEmpty(true),
        isCBA(false)

{
	readOnlyCommunity = roCommunity;
	readWriteCommunity = rwCommunity;
	// Create mutex to make class thread safe
	mutexHandle = new ACE_RW_Mutex();

	queueMutex = new ACE_RW_Mutex();

	m_pSem = new ACE_Semaphore(0);

	snmpMan = new ACS_CS_SNMPManager("");




	if (ParentService !=0x0)
		serviceCs = ParentService;/*so that it is possible to use it without CS service,
		to facilitate easier testing */

	isCBA = ACS_CS_Registry::IsCba();

	if (mutexHandle == 0)
	{
		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
			"(%t) ACS_CS_SNMPHandler::ACS_CS_SNMPHandler()\n"
			"Error: Cannot create mutex\n"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Create Mutex",
				"Cannot create mutex",
				"");
	}

	// This event is signaled when the service must shutdown
	shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);

	if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
			"(%t) ACS_CS_SNMPHandler::ACS_CS_SNMPHandler()\n"
			"Error: Cannot open shutdown event\n"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Open Event",
				"Cannot open event",
				"");
	}

	// This event is signaled when there is SNMP data to send
	updateSNMPEvent = ACS_CS_Event::CreateEvent(false, false, NULL);

	if (updateSNMPEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
			"(%t) ACS_CS_SNMPHandler::ACS_CS_SNMPHandler()\n"
			"Error: Cannot open shutdown event\n"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
					"Open Event",
					"Cannot open event",
					"");
	}

	//this->clientOutQueue=clientOutQueue;
}


ACS_CS_SNMPHandler::~ACS_CS_SNMPHandler()
{
	if(m_pSem != NULL){
		delete m_pSem;
		m_pSem = NULL;
	}
	if (mutexHandle)
		delete mutexHandle;

	if (queueMutex)
		delete queueMutex;

	if (snmpMan != NULL) {
		delete(snmpMan);
		snmpMan = NULL;
	}

	std::vector<ACS_CS_DHCP_Info *>::iterator it;
	for(it = clientOutQueue.begin(); it != clientOutQueue.end(); ++it)
	{
		delete *it;
	}

	ACS_CS_Event::CloseEvent(updateSNMPEvent);
	updateSNMPEvent = 0;
}



 int ACS_CS_SNMPHandler::exec ()
{
	 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"(%t) ACS_CS_SNMPHandler::exec()\n"
		"Thread starting\n"));

	// Array to use with WaitForEvents
	ACS_CS_EventHandle handleArr[2] = {shutdownEvent, updateSNMPEvent};

	int aMonth = 108000000;
	int twoSeconds = 5000;
	int timeout = 0;

	while ( ! exit_ )	// Loop as long as exit_ is false (set to false by the deactivate function)
	{
		// If out queue is empty, we wait for something to be added or for service shutdown
		// i.e. long timeout
		if (outQueueEmpty)
		{
			timeout = aMonth;
		}
		// If queue isn't empty, we wait just a short time, basically we are only interesting
		// in if the shutdown event has been signaled, otherwise we will continue checking the out queue
		else
		{
			timeout = twoSeconds;
		}


		// Wait for tasks to handle
		int returnValue = ACS_CS_Event::WaitForEvents( 2, handleArr, timeout);

		if (returnValue == ACS_CS_EVENT_RC_ERROR)		// Event failed
		{
			ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
				"(%t) ACS_CS_SNMPHandler::exec()\n"
				"Error: WaitForEvents\n"));

			ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Waiting for socket",
				"Unable to wait for event.",
				"");
		}
		else if (returnValue == ACS_CS_EVENT_RC_TIMEOUT)	// Wait timeout
		{
			ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
				"(%t) ACS_CS_SNMPHandler::exec()\n"
				"Event timeout\n"));

			checkOutQueue();				// Send first item in queue
		}
		else if (returnValue == 0)			// Shutdown event
		{
			ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
				"(%t) ACS_CS_SNMPHandler::exec()\n"
				"Shutdown event signaled. Closing down SNMP session.\n"));

			exit_ = true;					// Leave this thread function
		}
		else if (returnValue == 1)			// SNMP update event
		{
			ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
				"(%t) ACS_CS_SNMPHandler::exec()\n"
				"SNMP event.\n"));

			checkOutQueue();				// Send first item in queue

			ACS_CS_Event::ResetEvent(handleArr[1]);
		}
	}

	ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"(%t) ACS_CS_SNMPHandler::exec()\n"
		"Thread finishing\n"));

	return 0;
}



 void ACS_CS_SNMPHandler::sendGetMAC (const ACS_CS_DHCP_Info &client)
{
	 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"ACS_CS_SNMPHandler::sendGetMAC()\n"
		"Entering method"));

    (void) mutexHandle->acquire();

    ACS_CS_DHCP_Info * newClient = new ACS_CS_DHCP_Info(client);
    clientOutQueue.push_back(newClient);			// Add client to update queue

	(void) mutexHandle->release();

	if (!ACS_CS_Event::SetEvent(updateSNMPEvent))		// Notify updater
	{
		ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_SNMPHandler::sendGetMAC()\n"
			"Error: Cannot signal event\n"));
	}

	ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"ACS_CS_SNMPHandler::sendGetMAC()\n"
		"exiting from method"));

}

bool ACS_CS_SNMPHandler::syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector)
{
	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
		"ACS_CS_SNMPHandler::syncClients()\n"
		"Entering method"));

	bool syncResult = true;

	time_t startTime = time(NULL);

	std::vector<ACS_CS_DHCP_Info*>::iterator it;

	for (it = syncVector.begin(); it != syncVector.end(); ++it)
	{
		ACS_CS_DHCP_Info* info = *it;

		// Try once to retrieve MAC
		bool result = sendGetRequest(info);

		time_t currentTime = time(NULL);
		// Check if we exceed timeout
		if ( (currentTime - startTime) > 10 )
		{
			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
				"(%t) ACS_CS_SNMPHandler::syncClients()\n"
				"Time exceeding 10 s after first call, exiting\n"));

			syncResult = false;
			break;
		}

		// If necessary, try again on the other address
		if (!result)
		{
			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
				"(%t) ACS_CS_SNMPHandler::syncClients()\n"
				"SNMP GET failed, changing SCB-RP address \n"));

			changeScbRpAddress = true;
			result = sendGetRequest(info);
		}

		// Check if we exceed HWC Sync timeout
		currentTime = time(NULL);

		if ( (currentTime - startTime) > 10 )
		{
			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
				"(%t) ACS_CS_SNMPHandler::syncClients()\n"
				"Time exceeding 10 s after second call, exiting\n"));

			syncResult = false;
			break;
		}

		if (!result)
		{
			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
				"(%t) ACS_CS_SNMPHandler::syncClients()\n"
				"SNMP GET failed on both addresses, exiting.\n"));

			syncResult = false;
			break;
		}
	}

	return syncResult;

}

 bool ACS_CS_SNMPHandler::sendGetRequest (ACS_CS_DHCP_Info * info)
{
	 bool result = true;
	 int length;
	 int iStatus;
	 unsigned char out[SNMP_MAXBUF];
	 char *pBuffer;
	 //ACS_CS_SNMPManager* snmpMan = NULL;

	 std::string strIP("");
	 ostringstream oidStr("");
	 string MAC_OID("");
	 // Init
	 iStatus = ACS_CS_SNMPManager::OPEN_SESS_ERROR;
	 length = -1;
	 pBuffer = NULL;

	 if(isCBA)
	 {
		MAC_OID = ACS_CS_SNMP::OID_SCX_MAC_AGG;

		if((info->getDHCPMethod() == ACS_CS_Protocol::DHCP_Client) &&
				((info->getFBN() == ACS_CS_Protocol::FBN_EPB1) || (info->getFBN() == ACS_CS_Protocol::FBN_EvoET) ||
						(info->getFBN() == ACS_CS_Protocol::FBN_CMXB)))
		{
			MAC_OID = ACS_CS_SNMP::OID_BLADE_PRODUCT_NUMBER;
		}

		result = getScbRpAddress(&strIP);
	 }
	 else
	 {
		 MAC_OID = ACS_CS_SNMP::OID_MAC_AGG;
		 result = getScbRpAddress(&strIP);  // Get IP from scb
	 }

	 if (result == true)
	 {
		 // Get OID and slot (slot used as index in board table)
		 oidStr << MAC_OID << "." << info->getSlot();

		 // Set IP address
		 snmpMan->setIpAddress(strIP);

		 int typeOut;

		 typeOut=ACS_CS_SNMPManager::SNMPGET_HEX_STRING;

		 if (isCBA &&
				 (info->getFBN()==ACS_CS_Protocol::FBN_EPB1 || info->getFBN()==ACS_CS_Protocol::FBN_EvoET || info->getFBN()==ACS_CS_Protocol::FBN_CMXB))
		 {
			 typeOut = ACS_CS_SNMPManager::SNMPGET_STD_STRING;
		 }

		 iStatus = snmpMan->snmpGet("PUBLIC", snmpMan->getIpAddress().c_str(), oidStr.str().c_str(), ASN_OCTET_STR, out, SNMP_MAXBUF, typeOut);

		 if(iStatus != ACS_CS_SNMPManager::GETSET_SNMP_OK)
		 {
			 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
						"(%t) ACS_CS_SNMPHandler::sendGetRequest()\n"
						"Unable to execute snmpGet!\n"
					 	 "Board's IP: %s\n"
					 	 "Board's slot: %d\n"
					 	 "SCBRP's IP: %s", info->getIPString().c_str(), info->getSlot(), snmpMan->getIpAddress().c_str()));

			 return false;
		 }
	 }else
	 {
		 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
			"ACS_CS_SNMPHandler::sendGetRequest()\n"
			"##### Unable to find SCB IP!"));
	 }

	 if(iStatus == ACS_CS_SNMPManager::GETSET_SNMP_OK)
	 {
		 pBuffer = reinterpret_cast<char*>(out);
		 if (strlen(pBuffer) >= 6)
		 {
			 length = strlen(pBuffer);

			 std::cout << __FUNCTION__ << "@" << __LINE__ << "@@@@@@ buffer: " << pBuffer << std::endl;
			 std::cout << __FUNCTION__ << "@" << __LINE__ << "@@@@@@@@@ len: " << length << std::endl;

			 char * mac = new char[length+1];
			 memset(mac,0,length+1);
			 memcpy(mac, pBuffer, length);

			 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
					 "ACS_CS_SNMPHandler::sendGetRequest()\n"
					 "MAC retrieved %s\n", ACS_CS_Protocol::CS_ProtocolChecker::binToString(mac, length).c_str()));


			 if(info->getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
			 {
				 int macAddition = 0;

				 // MAC for EthA == Aggregated MAC + 1
				 if (info->getNetwork() == ACS_CS_Protocol::Eth_A)
					 macAddition = 1;
				 // MAC for EthB == Aggregated MAC + 2
				 else if (info->getNetwork() == ACS_CS_Protocol::Eth_B)
					 macAddition = 2;

				 // Add 1 or 2 to the MAC. Since a MAC is 6 byte and doesn't conform
				 // to a standard data type, the addition is carried out byte by byte

				 length = hexCharToBinChar(out,mac,macAddition);
				 //addToMac(mac, length, macAddition); //to be verified
			 }

			 info->setMAC(mac, length);

			 delete[] mac;
			 result = true;


		 } else {
			 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
					 "(%t) ACS_CS_SNMPHandler::sendGetRequest()\n"
					 "Not correct data length, desc_.length()=%d\n", length));

			 result = false;
		 }
	 }



	 // Exit from method
	 return(result);
}



 void ACS_CS_SNMPHandler::checkOutQueue ()
{
	 ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
			 "ACS_CS_SNMPHandler::checkOutQueue()\n"
			 "Entering method"));

	 //removeCanceledClients();

	 ACS_CS_DHCP_Info * info = 0;

	 size_t size = clientOutQueue.size();

	 outQueueEmpty = false;

	 uint position = 0;

	while(position < size && !exit_)
	{
		ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
			"Queue size %d\n", size));


		(void) mutexHandle->acquire();
		info = clientOutQueue.at(position);
		(void) mutexHandle->release();

		if (info)
		{
			bool result = false;


			result = sendGetRequest(info);

			if (!result)
			{
//				ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//					"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
//					"Failed to get MAC, changing address\n"));

				// Try another address to the SCB-RP next time
				changeScbRpAddress = true;

				result = sendGetRequest(info);

				if (!result) {

					info->nextTries();

					cout << "Tries = " << info->getTries() << " for " << info->getHostName() << std::endl;

					ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
							"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
							"Failed to get MAC for %s, try = %d\n", info->getHostName().c_str(), info->getTries()));

					if(info->getTries() >= 3)
					{
						ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
								"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
								"Failed to get MAC %d times, deleting the entry\n", info->getTries()));

						(void) mutexHandle->acquire();
						clientOutQueue.erase(clientOutQueue.begin() + position);
						(void) mutexHandle->release();

						delete info;
						info = 0;
					} else {
						//Skip this request for now and try again in a few seconds
						position++;
					}
				}
			}

			if (result)	// MAC successfully retrieved, call back to daemon
			{
				info->resetTries();

				(void) mutexHandle->acquire();
				clientOutQueue.erase(clientOutQueue.begin() + position);
				(void) mutexHandle->release();

				if((info->getDHCPMethod() == ACS_CS_Protocol::DHCP_Client) &&
						(info->getFBN()==ACS_CS_Protocol::FBN_EPB1 || info->getFBN()==ACS_CS_Protocol::FBN_EvoET || info->getFBN()==ACS_CS_Protocol::FBN_CMXB))
				{
					if (serviceCs)
					{
						//serviceCs->addEPB1ToDHCPTable(info);
						serviceCs->MacUpdate(info);
					}

					ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
						"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
						"Successfully retrieved MAC for EPB\n"));
				}
				else
				{

					if (serviceCs) serviceCs->MacUpdate(info);

					ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
						"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
						"Successfully retrieved MAC\n"));
				}

				delete info;
				info = 0;
			}
		}
		else
		{
			ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
				"(%t) ACS_CS_SNMPHandler::checkOutQueue()\n"
				"info is NULL\n"));
		}

		size = clientOutQueue.size();

	}

	if(size == 0)
	{
		outQueueEmpty = true;
	}


	ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"ACS_CS_SNMPHandler::checkOutQueue()\n"
		"Leaving method"));
}

 bool ACS_CS_SNMPHandler::getScbRpAddress(std::string *strIp)
 {
 	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 		"ACS_CS_SNMPHandler::getScbRpAddress()\n"
 		"Entering method"));

 	// If test environment
 	if (ACS_CS_Registry::isTestEnvironment())
 	{
 		// Get IP address for simulated SCB-RP
 		uint32_t testIp = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::SCBRP_FOR_TEST);

 		if (testIp)
 		{
 			// If simulated SCB-RP available
 			char tempValueStr[INET_ADDRSTRLEN + 1];
 			in_addr_t tempValue = htonl(testIp);
 			tempValueStr[INET_ADDRSTRLEN] = 0;

 			if ( inet_ntop(AF_INET, &tempValue, tempValueStr, INET_ADDRSTRLEN))
 			{
 				ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 					"ACS_CS_SNMPHandler::getScbRpAddress()\n"
 					"Simulated SCB-RP board at %s", tempValueStr));
 				// set output string
 				*strIp = tempValueStr;
 				return (true);
 			}
 			else
 			{
 				ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 					"ACS_CS_SNMPHandler::getScbRpAddress()\n"
 					"Failed to convert address for simulated SCB-RP board"));
 			}
 		}
 		else
 		{
 			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 				"ACS_CS_SNMPHandler::getScbRpAddress()\n"
 				"Failed to find address for simulated SCB-RP board"));
 		}
 	}

 	// Change the active IP address
 	if (changeScbRpAddress)
 	{
 		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 			"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
 			"Changing SCB-RP address to use\n"));

 		if (scbRpActiveAddress == scbRpEthA)
 		{
 			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 				"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
 				"Using EthA address\n"));

 			scbRpActiveAddress = scbRpEthB;
 		}
 		else
 		{
 			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 				"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
 				"Using EthB address\n"));

 			scbRpActiveAddress = scbRpEthA;
 		}

 		changeScbRpAddress = false;
 	}

 	char tempValueStr[INET_ADDRSTRLEN + 1];
 	in_addr_t tempValue = htonl(scbRpActiveAddress);
 	tempValueStr[INET_ADDRSTRLEN] = 0;

 	if ( ! inet_ntop(AF_INET, &tempValue, tempValueStr, INET_ADDRSTRLEN))
 	{
 		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
 			"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
 			"inet_ntop failed\n"));
 	}else{
		// set output string
		*strIp = tempValueStr;
		return(true);
 	}

 		return (false);
 }



//IpAddress ACS_CS_SNMPHandler::getScbRpAddress()
//{
//	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//		"ACS_CS_SNMPHandler::getScbRpAddress()\n"
//		"Entering method"));
//
//	// If test environment
//	if (ACS_CS_Registry::isTestEnvironment())
//	{
//		// Get IP address for simulated SCB-RP
//		uint32_t testIp = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::SCBRP_FOR_TEST);
//
//		if (testIp)
//		{
//			// If simulated SCB-RP available
//			char tempValueStr[INET_ADDRSTRLEN + 1];
//			in_addr_t tempValue = htonl(testIp);
//			tempValueStr[INET_ADDRSTRLEN] = 0;
//
//			if ( inet_ntop(AF_INET, &tempValue, tempValueStr, INET_ADDRSTRLEN))
//			{
//				ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//					"ACS_CS_SNMPHandler::getScbRpAddress()\n"
//					"Simulated SCB-RP board at %s", tempValueStr));
//
//				return IpAddress(tempValueStr);
//			}
//			else
//			{
//				ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//					"ACS_CS_SNMPHandler::getScbRpAddress()\n"
//					"Failed to convert address for simulated SCB-RP board"));
//			}
//		}
//		else
//		{
//			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//				"ACS_CS_SNMPHandler::getScbRpAddress()\n"
//				"Failed to find address for simulated SCB-RP board"));
//		}
//	}
//
//	// Change the active IP address
//	if (changeScbRpAddress)
//	{
//		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//			"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
//			"Changing SCB-RP address to use\n"));
//
//		if (scbRpActiveAddress == scbRpEthA)
//		{
//			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//				"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
//				"Using EthA address\n"));
//
//			scbRpActiveAddress = scbRpEthA;
//		}
//		else
//		{
//			ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//				"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
//				"Using EthB address\n"));
//
//			scbRpActiveAddress = scbRpEthA;
//		}
//
//		changeScbRpAddress = false;
//	}
//
//	char tempValueStr[INET_ADDRSTRLEN + 1];
//	in_addr_t tempValue = htonl(scbRpActiveAddress);
//	tempValueStr[INET_ADDRSTRLEN] = 0;
//
//	if ( ! inet_ntop(AF_INET, &tempValue, tempValueStr, INET_ADDRSTRLEN))
//	{
//		ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
//			"(%t) ACS_CS_SNMPHandler::getScbRpAddress()\n"
//			"inet_ntop failed\n"));
//	}
//		return IpAddress(tempValueStr);
//}

void ACS_CS_SNMPHandler::addToMac(char * mac, int size, int add)
{
	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
		"ACS_CS_SNMPHandler::addToMac()\n"
		"Entering: mac = %s size = %d, add = %d", ACS_CS_Protocol::CS_ProtocolChecker::binToString(mac, size).c_str(),
		size,
		add));

	if (size == 6 && add > 0)
	{
	 char b0 = mac[0];
	 char b1 = mac[1];
	 char b2 = mac[2];
	 char b3 = mac[3];
	 char b4 = mac[4];
	 char b5 = mac[5];


	 char before = b5;
	 b5 += add;

	 if (before > b5)
	 {
		before = b4;
		b4++;

		if (before > b4)
		{
		   before = b3;
		   b3++;

		   if (before > b3)
		   {
			  before = b2;
			  b2++;

			  if (before > b2)
			  {
				 before = b1;
				 b1++;

				 if (before > b1)
					b0++;
			  }
		   }
		}
	 }

	 memcpy(mac, &b0, 1);
	 memcpy(mac + 1, &b1, 1);
	 memcpy(mac + 2, &b2, 1);
	 memcpy(mac + 3, &b3, 1);
	 memcpy(mac + 4, &b4, 1);
	 memcpy(mac + 5, &b5, 1);
	}

	ACS_CS_TRACE((ACS_CS_SNMPHandler_TRACE,
		"ACS_CS_SNMPHandler::addToMac()\n"
		"Leaving: mac = %s size = %d, add = %d", ACS_CS_Protocol::CS_ProtocolChecker::binToString(mac, size).c_str(),
		size,
		add));
}


void ACS_CS_SNMPHandler::cancelGetMac(const ACS_CS_DHCP_Info &client)
{
	ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO, "ACS_CS_SNMPHandler::cancelGetMac()\nEntering"));

	(void) mutexHandle->acquire();
	canceledClients.push(new ACS_CS_DHCP_Info(client));
	(void) mutexHandle->release();
}

void ACS_CS_SNMPHandler::removeCanceledClients()
{
	(void) mutexHandle->acquire();

	while (!canceledClients.empty())
	{
		ACS_CS_DHCP_Info * info = canceledClients.front();
		canceledClients.pop();

		for (size_t i = clientOutQueue.size(); i > 0; i--)
		{
			if (clientOutQueue[i - 1]->getMagAddress() == info->getMagAddress()
				&& clientOutQueue[i - 1]->getSlot() == info->getSlot()
				&& clientOutQueue[i - 1]->getNetwork() == info->getNetwork() )
			{

				delete clientOutQueue[i - 1];
				clientOutQueue.erase(clientOutQueue.begin() + i - 1);

				break;
			}

		}

		delete info;
	}

	(void) mutexHandle->release();


}


//uint32_t ACS_CS_SNMPHandler::getIsCbaAddress(ACS_CS_DHCP_Info * info)
//{
//    ACS_CS_DHCP_Info infoA=*info;
//
//    uint32_t mag=info->getMagAddress();
//
//    std::string shelfId("");
//
//    ulongToStringMagazine(mag,shelfId);
//
//    ACS_CS_ImModel *model =const_cast<ACS_CS_ImModel *> (ACS_CS_ImRepository::instance()->getModel());
//
//    uint32_t ipSCX0EthA=0,	ipSCX0EthB=0;
//    uint32_t ipSCX25EthA=0,ipSCX25EthB=0;
//
//    //ACS_CS_SNMPManager *snmpMan = NULL;
//
//    if(model)
//    {
//       std::string dn;
//
//       dn=std::string("axeOtherBladeId=0,axeShelfId=") + shelfId.c_str() + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;
//	   std::cout<<dn.c_str()<<std::endl;
//
//	   const ACS_CS_ImBlade *blade;
//
//	   // fetch the blade with assigned IP addresses
//	   blade = dynamic_cast<const ACS_CS_ImBlade *>(model->getObject(dn.c_str()));
//
//	   if(blade!=NULL)
//	   {
//			   inet_pton(AF_INET, blade->ipAddressEthA.c_str(), &ipSCX0EthA);
//
//			   if(ipSCX0EthA)
//			   {
//				   ipSCX0EthA = ntohl(ipSCX0EthA);
//			   }
//
//			   inet_pton(AF_INET, blade->ipAddressEthB.c_str(), &ipSCX0EthB);
//
//			   if(ipSCX0EthB)
//			   {
//				   ipSCX0EthB = ntohl(ipSCX0EthB);
//			   }
//	   }
//
//
//	   dn=std::string("axeOtherBladeId=25,axeShelfId=") + shelfId.c_str() + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;
//	   std::cout<<dn.c_str()<<std::endl;
//
//	   blade = dynamic_cast<const ACS_CS_ImBlade *>(model->getObject(dn.c_str()));
//
//	   if(blade!=NULL)
//	   {
//		   inet_pton(AF_INET, blade->ipAddressEthA.c_str(), &ipSCX25EthA);
//
//		   if(ipSCX25EthA)
//		   {
//			   ipSCX25EthA = ntohl(ipSCX25EthA);
//		   }
//
//		   inet_pton(AF_INET, blade->ipAddressEthB.c_str(), &ipSCX25EthB);
//
//		   if(ipSCX25EthB)
//		   {
//			   ipSCX25EthB = ntohl(ipSCX25EthB);
//		   }
//	   }
//
////	   uint32_t ips[4];
//	   uint32_t ret=0;
////	   memset(ips,0,sizeof(uint32_t)*4);
////
////	   ips[0]=ipSCX0EthA; ips[1]=ipSCX0EthB;
////	   ips[2]=ipSCX25EthA; ips[3]=ipSCX25EthB;
//
////	   std::string st_ip("");
////	   snmpMan= new ACS_CS_SNMPManager(st_ip);
//
////	   for(int t=0;t<4;t++)
////	   {
////		   if(ips[t]==0)
////			   continue;
////
////		   st_ip=getIPString(ips[t]);
////		   snmpMan->setIpAddress(st_ip);
////		   //sleep(1);
////		   if(snmpMan->isMaster())
////		   {
////			   ret=ips[t];
////			   break;
////		   }
////
////	   }
//
//	   int res = 0;
//
//	   res=snmpMan->isMasterSCX(getIPString(ipSCX0EthA));
//	   if (res == 1) //SCX0 is Master
//		   ret=ipSCX0EthA;
//	   else if (res==2) {//SCX0 is passive
//		   ret=ipSCX25EthA;
//	   }
//	   else if (res==14) {//SCX0 not reachable
//		   res=snmpMan->isMasterSCX(getIPString(ipSCX25EthB));
//		   if (res == 1) //SCX25 is master
//			   ret=ipSCX25EthB;
//		   else if (res == 2) { //SCX25 is passive
//			   ret=ipSCX0EthB;
//		   }
//
//	   }
//
//	   return ret;
//    }
//    else
//    	return 0;
//}


std::string ACS_CS_SNMPHandler::getIPString (uint32_t ipAddress)
 {
    string ip = "";

 	if (ipAddress)
 	{
 		char buffer[16];
 		sprintf(buffer, "%u.%u.%u.%u",	( ( ipAddress  >> 24 ) & 0xFF ),
 										( ( ipAddress  >> 16 ) & 0xFF ),
 										( ( ipAddress  >> 8 ) & 0xFF ),
 										( ipAddress & 0xFF ) );
 		ip = buffer;
 	}

 	return ip;
 }


bool ACS_CS_SNMPHandler::ulongToStringMagazine (uint32_t uvalue, std::string &value)
{

        char tempValueStr[16] = {0}; //size 16, "255.255.255.255" + null character
        uint32_t tempValue;
        tempValue =  ntohl(uvalue);  //changes the representation to little-endian
        snprintf(tempValueStr, sizeof(tempValueStr) - 1, "%u.%u.%u.%u", (tempValue >> 24) & 0xFF, (tempValue >> 16) & 0xFF, (tempValue >> 8) & 0xFF, tempValue & 0xFF);
        value = tempValueStr;
        return true;

}



void ACS_CS_SNMPHandler::addToMac(unsigned char * mac, int size, int add)
{
	if (size == 6 && add > 0)
	{
	 unsigned char b0 = mac[0];
	 unsigned char b1 = mac[1];
	 unsigned char b2 = mac[2];
	 unsigned char b3 = mac[3];
	 unsigned char b4 = mac[4];
	 unsigned char b5 = mac[5];


	 unsigned char before = b5;
	 b5 += add;

	 if (before > b5)
	 {
		before = b4;
		b4++;

		if (before > b4)
		{
		   before = b3;
		   b3++;

		   if (before > b3)
		   {
			  before = b2;
			  b2++;

			  if (before > b2)
			  {
				 before = b1;
				 b1++;

				 if (before > b1)
					b0++;
			  }
		   }
		}
	 }

	 memcpy(mac, &b0, 1);
	 memcpy(mac + 1, &b1, 1);
	 memcpy(mac + 2, &b2, 1);
	 memcpy(mac + 3, &b3, 1);
	 memcpy(mac + 4, &b4, 1);
	 memcpy(mac + 5, &b5, 1);
	}
}



int ACS_CS_SNMPHandler::normalize(char r)
{

    switch(r)
    {
        case 'F':
            return 15;
            break;

        case 'E':
            return 14;
            break;
        case 'D':
                    return 13;
            break;
        case 'C':
                    return 12;
            break;
        case 'B':
                    return 11;
            break;

        case 'A':
                    return 10;
            break;

        default:
            return r-48;
    }


}

int ACS_CS_SNMPHandler::hexCharToBinChar(unsigned char *input,char *out,int add)
{

    unsigned char *output=(unsigned char*)malloc(6);
    memset(output,0,6);

    size_t t=0;
    int count=0;
    unsigned int ris=0;
    int index=0;


    for(t=0;t<strlen((char*)input);t++)
    {

        if(input[t]==' ')
        {
            count=0;
            ris=0;
        }
        else
        {
            if(count==0)
            {
                ris+=16*normalize(input[t]);
                count++;
            }
            else
            {
                ris+=(unsigned int) normalize(input[t]);
                output[index++]=ris;
            }
        }
    }


    addToMac(output,index,add);

    for(int t=0;t<index;t++)
    {
         out[t]=output[t];
    }

    free(output);

    return index;

}



void ACS_CS_SNMPHandler::insertIntoClientOutQueue (const ACS_CS_DHCP_Info &client)
{
	ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"(%t) ACS_CS_SNMPHandler::insertIntoClientOutQueue()\n"
		"client.getHostName %s clientOutQueue=%d\n",client.getHostName().c_str(),clientOutQueue.size()));

	printf("client.getHostName %s clientOutQueue=%d...",client.getHostName().c_str(),(int)clientOutQueue.size());

	(void) mutexHandle->acquire();
	ACS_CS_DHCP_Info * newClient = new ACS_CS_DHCP_Info(client);
	clientOutQueue.push_back(newClient);			// Add client to update queue
    (void) mutexHandle->release();

    cout << "....ADDED" << endl;
}

void ACS_CS_SNMPHandler::wakeUpGetMac ()
{

	cout << "Signaling updateSNMPEvent..." << endl;

	if (!ACS_CS_Event::SetEvent(updateSNMPEvent))		// Notify updater
	{
		ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_ERROR,
			"(%t) ACS_CS_SNMPHandler::sendGetMAC()\n"
			"Error: Cannot signal event\n"));

		cout << "Error: Cannot signal event\n";
	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_SNMPHandler::sendGetMAC()\n"
			"Event Signaled\n"));

		cout << "...Event Signaled" << endl;
	}

	ACS_CS_FTRACE((ACS_CS_SNMPHandler_TRACE, LOG_LEVEL_INFO,
		"ACS_CS_SNMPHandler::sendGetMAC()\n"
		"exiting from method"));

}

bool ACS_CS_SNMPHandler::isMaster(int slot) {

	bool result = false;

	if (isCBA) {
		int res = 0;

		if (slot == 0)
			res=snmpMan->isMasterSCX(getIPString(scbRpEthA));
		else
			res=snmpMan->isMasterSCX(getIPString(scbRpEthB));

		if (res == 1) //SCX is Master
			result = true;
		else if (res==2) {//SCX is passive
			result = false;
		}
		else if (res==14) {//ethA not reachable
			if (slot == 0)
				res=snmpMan->isMasterSCX(getIPString(scbRpEthB));
			else
				res=snmpMan->isMasterSCX(getIPString(scbRpEthA));

			if (res == 1) //SCX is master
				result = true;
			else if (res == 2) { //SCX is passive
				result = false;
			} else //ethB not reachable
				result = false;
		}
	} else {
		int res = 0;

		if (slot == 0)
			res=snmpMan->isMasterSCB(getIPString(scbRpEthA), slot);
		else
			res=snmpMan->isMasterSCB(getIPString(scbRpEthB), slot);

		if (res == 1) //SCB is Master
			result = true;
		else if (res==2) {//SCB is passive
			result = false;
		}
		else if (res==14) {//ethA not reachable
			if (slot == 0)
				res=snmpMan->isMasterSCB(getIPString(scbRpEthB), slot);
			else
				res=snmpMan->isMasterSCB(getIPString(scbRpEthA), slot);

			if (res == 1) //SCB is master
				result = true;
			else if (res == 2) { //SCB is passive
				result = false;
			} else //ethB not reachable
				result = false;
		}
	}

	return result;

}
