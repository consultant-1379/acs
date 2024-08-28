/*
 * ACS_CS_SNMPMasterHandler.cpp
 *
 *  Created on: May 29, 2012
 *      Author: estevol
 */


//	Copyright Ericsson AB 2007, 2010. All rights reserved.


#include <sstream>
#include <string>
#include <time.h>

#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_time.h>



#include <ace/RW_Mutex.h>
#include <ace/Semaphore.h>

//#include "ACS_CS_Util.h"
//#include "ACS_CS_ImUtils.h"

#include "ACS_CS_SNMPMasterHandler.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_HostFile.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_SNMPMasterHandler_TRACE);
#include <iostream>


using std::string;
using std::ostringstream;
using namespace ACS_CS_NS;

ACS_CS_SNMPMasterHandler::ACS_CS_SNMPMasterHandler (ACS_CS_ServiceHandler* ParentService)
      : ACS_CS_ShelfManager(ParentService),
        missingMaster(false)
{
	isCBA = ACS_CS_Registry::IsCba();
}


ACS_CS_SNMPMasterHandler::~ACS_CS_SNMPMasterHandler()
{

}

 void ACS_CS_SNMPMasterHandler::checkOutQueue ()
{
	 ACS_CS_FTRACE((ACS_CS_SNMPMasterHandler_TRACE, LOG_LEVEL_INFO,
			 "ACS_CS_SNMPMasterHandler::checkOutQueue()\n"
			 "Entering method"));

	 //removeCanceledClients();

	 //ACS_CS_DHCP_Info * info = 0;

	 size_t size = normalDhcpVector.size();

	 queueUpdated = false;
	 missingMaster = false;

	 //Copy the DHCP queue and the SNMP map so we don't lock the CLI while trying SNMP communication
	 dhcpQueueLock.start_reading();
	 std::vector<ACS_CS_DHCP_Info> copyQueue = normalDhcpVector;
	 dhcpQueueLock.stop_reading();

	 snmpMapLock.start_reading();
	 snmpMapType copyMap = snmpMap;
	 snmpMapLock.stop_reading();

	 for (size_t i = 0; i < copyQueue.size() && !queueUpdated && !exit_; i++)
	 {

		 ACS_CS_DHCP_Info info = copyQueue[i];

		 ACS_CS_FTRACE((ACS_CS_SNMPMasterHandler_TRACE, LOG_LEVEL_INFO,
				 "(%t) ACS_CS_SNMPMasterHandler::checkOutQueue()\n"
				 "Queue size %d\n", size));


		 // Search for SNMPmap entry
		 snmpMapType::iterator it = copyMap.find(info.getMagAddress()); // Search for old entry

		 if (it != copyMap.end()) // If found
		 {
			 bool requestForwarded = false;

			 // Get the map entry
			 SnmPHandlerMapEntry * mapEntry = it->second;

			 char shelfAddr[INET_ADDRSTRLEN];
			 uint32_t naddr = info.getMagAddress();
			 inet_ntop(AF_INET, &naddr, shelfAddr, sizeof(shelfAddr));

			 ACS_CS_SNMPHandler* handler_0 = mapEntry->handlerSlot0;
			 ACS_CS_SNMPHandler* handler_25 = mapEntry->handlerSlot25;

			 // Check if there are SNMP handlers available and call them
			 if (handler_0 && handler_0->isMaster(0))
			 {
				 ACS_CS_FTRACE((ACS_CS_SNMPMasterHandler_TRACE, LOG_LEVEL_INFO,
						 "(%t) ACS_CS_SNMPMasterHandler::checkOutQueue()\n"
						 "SNMP request forwarded to handler 0 of magazine %d\n", info.getMagAddress() ));
				 requestForwarded = true;

				 handler_0->insertIntoClientOutQueue(info);
				 handler_0->wakeUpGetMac();

				 cout << "======================== Host " << info.getHostName() << " added to queue for handler 0 of magazine " << shelfAddr <<" ======================" << endl;

				 canceledClients.push(info);

			 }
			 else if (!queueUpdated && handler_25 && handler_25->isMaster(25))
			 {
				 ACS_CS_FTRACE((ACS_CS_SNMPMasterHandler_TRACE, LOG_LEVEL_INFO,
						 "(%t) ACS_CS_SNMPMasterHandler::checkOutQueue()\n"
						 "SNMP request forwarded to handler 25 of magazine %d\n", info.getMagAddress() ));
				 requestForwarded = true;

				 handler_25->insertIntoClientOutQueue(info);
				 handler_25->wakeUpGetMac();

				 cout << "========================  Host " << info.getHostName() << " added to queue for handler 25 of magazine " << shelfAddr <<" ======================" << endl;


				 canceledClients.push(info);
			 }

			 if ((handler_0 || handler_25) && !requestForwarded)
				 missingMaster = true;
		 }
	 }

	 //Check if anything need to be deleted from SNMP map or SNMP handlers
	snmpHandlerCleanup();

	ACS_CS_FTRACE((ACS_CS_SNMPMasterHandler_TRACE, LOG_LEVEL_INFO,
		"ACS_CS_SNMPMasterHandler::checkOutQueue()\n"
		"Leaving method"));
}



void ACS_CS_SNMPMasterHandler::addShelfHandler(ACS_CS_DHCP_Info &infoA, ACS_CS_DHCP_Info &infoB)
{
	ACS_CS_TRACE((ACS_CS_SNMPMasterHandler_TRACE,
		"ACS_CS_ServiceHandler::addSnmpHandler()"));

	// Find map entry for this magazine
	snmpMapLock.start_writing();


	snmpMapType::iterator it = snmpMap.find(infoA.getMagAddress()); // Search for old entry

	SnmPHandlerMapEntry * mapEntry = 0;

	if (it != snmpMap.end())
	{
		// If there already is a map entry
		mapEntry = it->second;
	}
	else
	{
		// Otherwise create it
		mapEntry = new SnmPHandlerMapEntry();
		mapEntry->handlerSlot0 = 0;
		mapEntry->handlerSlot25 = 0;

		snmpMap.insert(snmpMapType::value_type(infoA.getMagAddress(), mapEntry));
	}

	// Create handler
	ACS_CS_SNMPHandler * handler =  new ACS_CS_SNMPHandler(infoA.getIP(), infoB.getIP(),
			161, "public", "private", serviceCs);


	if (infoA.getSlot() == 0)
	{
		if (mapEntry->handlerSlot0)
		{
			// Deactivate old handler and save pointer for later removal
			mapEntry->handlerSlot0->deActivate();
			mapEntry->handlerSlot0->wakeUpGetMac(); //to return from wait method

			snmpHandlerLock.start_writing();
			removedSnmpHandlers.push_back(mapEntry->handlerSlot0);
			snmpHandlerLock.stop_writing();

			mapEntry->handlerSlot0 = 0;
		}

		mapEntry->handlerSlot0 = handler;
		mapEntry->handlerSlot0->activate();
		handler = 0;

	}
	else if (infoA.getSlot() == 25)
	{
		if (mapEntry->handlerSlot25)
		{
			// Deactivate old handler and save pointer for later removal
			mapEntry->handlerSlot25->deActivate();
			mapEntry->handlerSlot25->wakeUpGetMac(); //to return from wait method

			snmpHandlerLock.start_writing();
			removedSnmpHandlers.push_back(mapEntry->handlerSlot25);
			snmpHandlerLock.stop_writing();

			mapEntry->handlerSlot25 = 0;
		}

		mapEntry->handlerSlot25 = handler;
		mapEntry->handlerSlot25->activate();
		handler = 0;

	}
	else
	{
		// Some error
	}

	if (handler)
		delete handler;

	snmpMapLock.stop_writing();

	wakeUpQueue();
}


void ACS_CS_SNMPMasterHandler::removeShelfHandler(uint32_t magazine, uint16_t slot)
{
	ACS_CS_TRACE((ACS_CS_SNMPMasterHandler_TRACE,
		"ACS_CS_SNMPMasterHandler::removeSnmpHandler()"));

	// Find map entry for this magazine
	snmpMapLock.start_writing();

	snmpMapType::iterator it = snmpMap.find(magazine); // Search for old entry

	if (it != snmpMap.end())
	{
		SnmPHandlerMapEntry * mapEntry = it->second;

		if (slot == 0)
		{
			if (mapEntry->handlerSlot0)
			{
				// Deactivate handler and save pointer for later removal
				mapEntry->handlerSlot0->deActivate();
				mapEntry->handlerSlot0->wakeUpGetMac(); //to return from wait method

				snmpHandlerLock.start_writing();
				removedSnmpHandlers.push_back(mapEntry->handlerSlot0);
				snmpHandlerLock.stop_writing();

				mapEntry->handlerSlot0 = 0;
			}
		}
		else if (slot == 25)
		{
			if (mapEntry->handlerSlot25)
			{
				// Deactivate handler and save pointer for later removal
				mapEntry->handlerSlot25->deActivate();
				mapEntry->handlerSlot25->wakeUpGetMac(); //to return from wait method

				snmpHandlerLock.start_writing();
				removedSnmpHandlers.push_back(mapEntry->handlerSlot25);
				snmpHandlerLock.stop_writing();

				mapEntry->handlerSlot25 = 0;
			}
		}
		else
		{
			// Some error
		}


	}

	snmpMapLock.stop_writing();

}

void ACS_CS_SNMPMasterHandler::snmpHandlerCleanup()
{
	ACS_CS_TRACE((ACS_CS_SNMPMasterHandler_TRACE,
		"ACS_CS_SNMPMasterHandler::snmpHandlerCleanup()"));

	bool cleanUpMap = false;

	snmpHandlerLock.start_writing();

	for (size_t i = removedSnmpHandlers.size(); i > 0; i--)
	{
		if (removedSnmpHandlers[i - 1]->isFinished())
		{
			delete removedSnmpHandlers[i - 1];
			removedSnmpHandlers.erase(removedSnmpHandlers.begin() + i - 1);
			cleanUpMap = true;
		}
	}

	snmpHandlerLock.stop_writing();

	if (cleanUpMap)
	{
		snmpMapLock.start_writing();

		snmpMapType::iterator it;

		for (it = snmpMap.begin(); it != snmpMap.end(); )
		{
			SnmPHandlerMapEntry * mapEntry = it->second;

			if ( ! mapEntry->handlerSlot0 && ! mapEntry->handlerSlot25)
			{
				(void) snmpMap.erase(it++); // remove from map
				delete mapEntry;
				mapEntry = 0;
			}
			else
			{
				++it;
			}
		}

		snmpMapLock.stop_writing();
	}

}


bool ACS_CS_SNMPMasterHandler::syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector)
{

	if (syncVector.size() == 0)	// Simple case, nothing to sync
		return true;
	else if (snmpMap.size() == 0) // Simple case, no SCB-RP boards
		return false;

	bool syncSuccess = true;
	int threadNo = 0;

	magazineMapType magazineMap;

	// Loop through all SCB-RP boards and create a DHCP info vector for each magazine
	for (snmpMapType::iterator snmpIt = snmpMap.begin(); snmpIt != snmpMap.end(); ++snmpIt)
	{
		uint32_t magazine = snmpIt->first;
		if (magazineMap.find(magazine) == magazineMap.end())	// Check that we haven't already created magazine
		{
			std::vector<ACS_CS_DHCP_Info*> * tmpVector = new std::vector<ACS_CS_DHCP_Info*>();
			magazineMap.insert( magazineMapType::value_type(magazine, tmpVector));
		}

		SnmPHandlerMapEntry * mapEntry = snmpIt->second;

		if (mapEntry)
		{
			if (mapEntry->handlerSlot0)
				threadNo++;

			if (mapEntry->handlerSlot25)
				threadNo++;
		}
	}

	std::vector<ACS_CS_DHCP_Info*>::iterator dhcpIt;

	// Loop through all DHCP objects that should be synced
	for (dhcpIt = syncVector.begin(); dhcpIt != syncVector.end(); ++dhcpIt)
	{
		uint32_t magazine = (*dhcpIt)->getMagAddress();
		magazineMapType::iterator it = magazineMap.find(magazine);

		// Find the correct vector to add it to
		if (it != magazineMap.end())
		{
			it->second->push_back(*dhcpIt);
		}
		else
		{
			// No SCB-RP board for this magazine
			syncSuccess = false;
			break;
		}
	}

	if (syncSuccess)
	{
		// If we get here we have SCB-RP boards for each magazine in which there are CPU boards
		//int threadNo = snmpMap.size() * 2;
		// Create a thread for each SNMP handler
		ACS_CS_Sync_Thread** threads = new ACS_CS_Sync_Thread*[threadNo];
		ACS_CS_EventHandle* threadHandles = new ACS_CS_EventHandle[threadNo];
		int threadCounter = 0;

		// Loop through SNMP handlers
		for (snmpMapType::iterator snmpIt = snmpMap.begin(); snmpIt != snmpMap.end(); ++snmpIt)
		{
			uint32_t magazine = snmpIt->first;
			SnmPHandlerMapEntry * mapEntry = snmpIt->second;

			//ACS_CS_SNMPHandler * handler =  snmpIt->handler;

			magazineMapType::iterator magIt = magazineMap.find(magazine);

			// retrieve vector with DHCP info objects for this magazine
			std::vector<ACS_CS_DHCP_Info*> * tempvector = magIt->second;

			if (mapEntry->handlerSlot0)
			{
				// Create thread and start it
				threads[threadCounter] = new ACS_CS_Sync_Thread(mapEntry->handlerSlot0, tempvector);
				threadHandles[threadCounter] = threads[threadCounter]->getWaithandle();
				threads[threadCounter]->activate();
				threadCounter++;
			}
			if (mapEntry->handlerSlot25)
			{
				// Create thread and start it
				threads[threadCounter] = new ACS_CS_Sync_Thread(mapEntry->handlerSlot25, tempvector);
				threadHandles[threadCounter] = threads[threadCounter]->getWaithandle();
				threads[threadCounter]->activate();
				threadCounter++;
			}
		}

		// Reuse threadCounter
		threadCounter = 0;

		// Wait for all threads to finish
		while ( (threadCounter != threadNo)
				&& ACS_CS_Event::WaitForEvents(threadNo, threadHandles, 10000) >= 0)
		{
			threadCounter++;
		}

		// If all threads finished successfully
		if (threadCounter == threadNo)
		{
			// All threads finished successfully
			for (int i = 0; i < threadNo; i++)
			{
				// Check result for each thread - at least one should be a success
				if (threads[i]->success())
				{
					syncSuccess = true;
				}

				// remove thread
				delete threads[i];
				threads[i] = 0;
			}
		}

		delete [] threads;
		delete [] threadHandles;
	}

	// Remove all temporary vectors
	for (magazineMapType::iterator it = magazineMap.begin(); it != magazineMap.end(); ++it)
	{
		std::vector<ACS_CS_DHCP_Info*> * tmpVector = it->second;
		delete tmpVector;
	}

	return syncSuccess;
}

void ACS_CS_SNMPMasterHandler::cleanUp() {

	//Remove snmp handlers
	snmpMapType::iterator snmpIt;

	snmpMapLock.start_writing();

	for (snmpIt = snmpMap.begin(); snmpIt != snmpMap.end(); ++snmpIt)
	{
		SnmPHandlerMapEntry * snmpEntry = snmpIt->second;

		if (snmpEntry)
		{
			if (snmpEntry->handlerSlot0)
			{
				snmpEntry->handlerSlot0->deActivate();
				snmpEntry->handlerSlot0->wait(ACS_CS_NS::Daemon_Timeout);
				snmpEntry->handlerSlot0->close();
				delete snmpEntry->handlerSlot0;
				snmpEntry->handlerSlot0 = 0;
			}

			if (snmpEntry->handlerSlot25)
			{
				snmpEntry->handlerSlot25->deActivate();
				snmpEntry->handlerSlot25->wait(ACS_CS_NS::Daemon_Timeout);
				snmpEntry->handlerSlot25->close();
				delete snmpEntry->handlerSlot25;
				snmpEntry->handlerSlot25 = 0;
			}

			delete snmpEntry;
		}
	}

	snmpMapLock.stop_writing();

	//CleanUp of dead SNMP Handler threads
	snmpHandlerCleanup();
}

int ACS_CS_SNMPMasterHandler::updateTimeout() {
	//Update Timeout
	int aMonth = 108000000;
	int fiveSeconds = 5000;
	int oneSecond = 1000;
	int timeout = 0;

	// If out queue is empty, we wait for something to be added or for service shutdown
	// i.e. long timeout
	if (!missingMaster && !queueUpdated)
	{
		timeout = aMonth;
	}
	// If queue isn't empty, we wait just a short time, basically we are only interesting
	// in if the shutdown event has been signaled, otherwise we will continue checking the out queue
	else if(!queueUpdated)
	{
		timeout = fiveSeconds;
	} else
		timeout = oneSecond;

	return timeout;
}
