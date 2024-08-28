//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#include "ACS_CS_Master.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <sstream>
#include <fstream>
#include <algorithm>

#include "ACS_CS_Table.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_HostFile.h"
#include "ACS_CS_TCPServer.h"
#include "ACS_CS_TCPClient.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_TableOperation.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Trace.h"

#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImSerialization.h"

ACS_CS_Trace_TDEF(ACS_CS_Master_TRACE);

using std::vector;
using std::set;
using std::stringstream;
using ACS_CS_NS::ACS_CS_ReplicationType;
using ACS_CS_NS::ACS_CS_ReplicationData;

ACS_CS_Master::ACS_CS_Master():
		tcpServer(0),
		shutdownEvent(0)
{
	tcpServer = new ACS_CS_TCPServer();

	tableLoader = new ACS_CS_API_TableLoader();

	// Open shutdown event used to signal that the whole service shall shutdown
	shutdownEvent = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"(%t) ACS_CS_Master::ACS_CS_Master()\n"
				"Error: Cannot open shutdown event\n"));
	}

	sendOperationEvent = ACS_CS_Event::CreateEvent(false, false, 0);

	if (sendOperationEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"(%t) ACS_CS_Master::ACS_CS_Master()\n"
			"Error: Cannot create sendOperation event\n"));
	}

    unsigned short port = ACS_CS_NS::CS_Replication_Port;

    uint32_t ipA1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_A_HOSTNAME);
    uint32_t ipB1 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_A_HOSTNAME);
    uint32_t ipA2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_A_ETH_B_HOSTNAME);
    uint32_t ipB2 = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP_1_NODE_B_ETH_B_HOSTNAME);
    uint32_t ip1test = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP1_FOR_TEST);

    // listen to these addresses...
    tcpServer->addTcpServer(ipA1, port);
    tcpServer->addTcpServer(ipB1, port);
    tcpServer->addTcpServer(ipA2, port);
    tcpServer->addTcpServer(ipB2, port);

    if (ACS_CS_Registry::isTestEnvironment())
    {
    	if (ip1test)
    	{
    		tcpServer->addTcpServer(ip1test, port);
    	}
    }
}


ACS_CS_Master::~ACS_CS_Master()
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"(%t) ACS_CS_Master::~ACS_CS_Master()\n"
					"Deconstructor called\n"));
	if (tcpServer)
		delete tcpServer;

	if (tableLoader)
		delete tableLoader;

	std::list<ACS_CS_TableOperationEntry *>::iterator queueIt;

	for (queueIt = outQueue.begin(); queueIt != outQueue.end(); ++queueIt)
	{
		if (*queueIt)
			delete *queueIt;
	}

	entryMap::iterator mapIt;
	for (mapIt = incomingEntries.begin(); mapIt != incomingEntries.end(); ++mapIt)
	{
		if (mapIt->first)
			delete mapIt->first;
	}

	std::vector<ACS_CS_TCPClient *>::iterator slaveIt;

	for (slaveIt = connectedSlaves.begin(); slaveIt != connectedSlaves.end(); ++slaveIt)
	{
		if (*slaveIt){
			ACS_CS_TCPClient * slave = *slaveIt;
			slave->close();
			delete *slaveIt;
		}
	}

	if (sendOperationEvent)
		ACS_CS_Event::CloseEvent(sendOperationEvent);
}


ACS_CS_Master* ACS_CS_Master::instance()
{
    return instance_::instance();
}

void ACS_CS_Master::start()
{
	bool thread_ok = ACS_CS_Master::instance()->activate();

	if(!thread_ok){
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::start()\n"
			"ERROR: Could not start ACS_CS_Master thread!"));

	}
}

void ACS_CS_Master::stop()
{
	ACS_CS_Master::instance()->deActivate();
}


int ACS_CS_Master::exec()
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		"ACS_CS_Master::exec()"));

	// Vector to hold all handles that we will wait for events from
	vector<ACS_CS_EventHandle> eventVector;

	// start tcp server for listening on incoming connection for the CS protocol
	startListener();

	// Add event
	eventVector.push_back(shutdownEvent);
	eventVector.push_back(sendOperationEvent);

	// Add listening sockets
	tcpServer->addActiveServerHandlesToVector(eventVector);

	ACS_CS_EventHandle *handleArr = 0;


	while (!exit_)  // exit_ is an inherited variable. It is set to false by the inherited
	{               // function activate() and set to true by the function deActivate().

		// Create array with all handles
		size_t slaveOffset = eventVector.size();
		size_t handleArrSize = slaveOffset + connectedSlaves.size();

		delete[] handleArr; //don't worry, deleting NULL does not cause a crash
		handleArr = new ACS_CS_EventHandle[handleArrSize];

		// Loop through vector and add handles to array
		for (size_t i = 0;i < slaveOffset; i++) {
			handleArr[i] = eventVector[i];
		}

		// Loop through all connected slaves and add handles to array
		for (size_t i = 0;i < connectedSlaves.size(); i++) {
			handleArr[i + slaveOffset] = connectedSlaves[i]->getHandle();
		}

		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::exec()\n"
			"Waiting for events, size %d", handleArrSize));

		// Wait for events
		int eventIndex = ACS_CS_Event::WaitForEvents(handleArrSize, handleArr, 0);

		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::exec()\n"
			"Event received %d", eventIndex));

		if (eventIndex == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::exec()\n"
				"Error from WaitForEvents"));
		}
		else if (eventIndex == 0) // Shutdown event
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::exec()\n"
					"Shutdown event received"));

			exit_ = true;
		}
		else if (eventIndex == 1) // Send operation event
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::exec()\n"
				"Send operation event received"));

			checkOutQueue();

		}
		else if (tcpServer->isServerHandle(handleArr[eventIndex]))  // Incoming TCP connection
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::exec()\n"
					"Accept event received"));

			openNewConnection(handleArr[eventIndex]);
		}
		else if (eventIndex >= (int) slaveOffset && eventIndex < (int) handleArrSize )
		{
			ACS_CS_TCPClient * client = connectedSlaves[eventIndex - slaveOffset];
			ACS_CS_TCPClient::LinkStatus_t status = handleClientRequest(client);

			if (status != ACS_CS_TCPClient::OK)
			{
				// If connection failed, we remove this faulty client

				client->close();

				std::vector<ACS_CS_TCPClient *>::iterator it;

				for (it = connectedSlaves.begin(); it != connectedSlaves.end(); ++it)
				{
					if (client == *it)
					{
						connectedSlaves.erase(it);
						delete client;
						break;
					}
				}
			}
		}
		else
		{
			// Some error
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::exec()\n"
				"Error: Unknown event %d", eventIndex));

		}
	}

	tcpServer->stopListen();
	delete[] handleArr;

	return 0;
}

void ACS_CS_Master::startListener()
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::%s()", __FUNCTION__));

    // start the servers
    unsigned int active = tcpServer->startListen();

    if (active == 0)
    {
    	ACS_CS_FTRACE((ACS_CS_Master_TRACE,LOG_LEVEL_ERROR,
			"ACS_CS_Master::%s()\n"
    		"Failed to open port %d for listening on any network", __FUNCTION__, ACS_CS_NS::CS_Replication_Port));

    }
}


void ACS_CS_Master::openNewConnection(int tcpServerHandle)
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		 "ACS_CS_Master::openNewConnection()"));

    ACS_CS_TCPClient * tcpClient = tcpServer->acceptConnectionForHandle(tcpServerHandle);

    if (tcpClient == NULL)
    {
        ACS_CS_TRACE((ACS_CS_Master_TRACE,
                "ACS_CS_Master::openNewConnection()\n"
                "Error: Invalid socket"));
        return;
    }

    ACS_CS_TRACE((ACS_CS_Master_TRACE,
		"ACS_CS_Master::openNewConnection()\n"
		"Connection from %s\n", tcpClient->getFullAddressAsString().c_str()));

    sendTables(tcpClient);

    connectedSlaves.push_back(tcpClient);


}

void ACS_CS_Master::sendTables(ACS_CS_TCPClient * tcpClient)
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		 "ACS_CS_Master::sendTables()"));

	ACS_CS_ImModel * model = ACS_CS_ImRepository::instance()->getModelCopy();

	if(!model){
		ACS_CS_FTRACE((ACS_CS_Master_TRACE, LOG_LEVEL_ERROR,
				"ACS_CS_Master::sendTables()\n"
				"Failed to get model copy"));
		return;
	}

	// Create streams used to serialize data
	std::stringstream tableStream(std::ios::in | std::ios::out | std::ios::binary);
	std::stringstream sizeStream(std::ios::in | std::ios::out | std::ios::binary);

	try {
		boost::archive::binary_oarchive oa(tableStream);
		oa << * model;
	}
	catch (boost::archive::archive_exception& e)
	{
		std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::sendTables()\n"
				"Failed to send tables. Serialization exception occurred: %s.", e.what()));
		delete model;
		return;
	}
	catch (boost::exception& )
	{
		std::cout << "Boost Exception Occurred!" << std::endl;
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::sendTables()\n"
				"Failed to send tables. Serialization exception occurred."));
		delete model;
		return;
	}


	// Get total size of table data
	ACS_CS_ReplicationData replicationData;
	replicationData.size = (int) tableStream.tellp();
	replicationData.type = ACS_CS_NS::NewTables;
	replicationData.version = ACS_CS_ImmMapper::MODEL_VERSION;
	//int tableDataSize = (int) tableStream.tellp();

	// TEST *******************
	std::cout << "ACS_CS_Master::sendTables() --> \nreplicationData.size=" << replicationData.size << " - tableStream.tellp()=" << tableStream.tellp() << " - tableStream.tellg()=" << tableStream.tellg() << std::endl;

	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		"ACS_CS_Master::sendTables()\n"
		"Data to send %d", replicationData.size));

	// Write table data to other stream
	(void) sizeStream.write( reinterpret_cast<const char *> (&replicationData), sizeof(replicationData));

	// Create buffer to hold data to be sent over TCP
	char buffer[BUFSIZ];

	// Copy size of table data to buffer
	(void) sizeStream.read(buffer, sizeof(replicationData));

	// Start to copy tables to buffer, the buffer holds BUFSIZ but we have already copied
	// the value of tableDataSize and we should copy no more than what is actually stored
	// in the stream
	tableStream.read(buffer + sizeof(replicationData), std::min( (int) (BUFSIZ - sizeof(replicationData)), (int)replicationData.size));

	int dataCurrentlyInBuffer = (int) sizeof(replicationData) + std::min( (int) (BUFSIZ - sizeof(replicationData)), (int) replicationData.size);
	int tableDataCopied = (int) tableStream.gcount();
	int dataToSend = (int) sizeof(replicationData) + replicationData.size;

	ACS_CS_TCPClient::LinkStatus_t status = ACS_CS_TCPClient::OK;

	// Loop and send until everything in the table stream is sent
	while ( (dataToSend > 0) && (status == ACS_CS_TCPClient::OK) )
	{
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::sendTables()\n"
			"Sending %d bytes", dataToSend));

		// Send data stored in buffer
		status = tcpClient->send(buffer, dataCurrentlyInBuffer);
		// Update counter of sent data
		dataToSend -= dataCurrentlyInBuffer;

		if (tableDataCopied < replicationData.size)
		{
			dataCurrentlyInBuffer = std::min(BUFSIZ, replicationData.size - tableDataCopied);
			(void) tableStream.read(buffer, dataCurrentlyInBuffer);
			tableDataCopied += tableStream.gcount();
		}
	}

	delete model;

}


ACS_CS_TCPClient::LinkStatus_t ACS_CS_Master::handleClientRequest(ACS_CS_TCPClient * tcpClient)
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::handleClientRequest()"));

	ACS_CS_TCPClient::LinkStatus_t status = ACS_CS_TCPClient::OK;

	if (!tcpClient)
	{
		ACS_CS_FTRACE((ACS_CS_Master_TRACE,LOG_LEVEL_ERROR,
			"ACS_CS_Master::handleClientRequest()\n"
			"Error: No TCP client"));

		return ACS_CS_TCPClient::Failed;
	}

	stringstream in(std::ios::in | std::ios::out | std::ios::binary);

	char buffer[BUFSIZ];
	int lastRead = 0;
	int dataToRead = 0;
	size_t readData = 0;
	int version = 0;

	bool allRead = false;

	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		"ACS_CS_Master::handleClientRequest()\n"
		"trying to read from %s", tcpClient->getFullAddressAsString().c_str()));

	do
	{
		// Read data from TCP
		status = tcpClient->read(buffer, BUFSIZ, lastRead);

		// Write data to stream
		in.write(buffer, lastRead);
		readData = (int) in.tellp();

		ACS_CS_ReplicationData replicationData;

		// If enough data read to parse replicationData
		if ( (dataToRead == 0) && (readData >= sizeof(replicationData)) )
		{
			(void) in.read( reinterpret_cast<char *> (&replicationData), sizeof(replicationData) );

			dataToRead = replicationData.size;

			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::handleClientRequest()\n"
				"dataToRead = %d", dataToRead));
		}

		if (readData == (dataToRead + sizeof(replicationData)))
		{
			version = replicationData.version;
			allRead = true;
		}

		// while there is data left to read and connection is ok
	} while (!allRead && status == ACS_CS_TCPClient::OK);


	// If all table data read
	if (allRead)
	{
		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::handleClientRequest()\n"
			"Parsing tables, readData = %d", readData));

		if (version != ACS_CS_ImmMapper::MODEL_VERSION)
		{
			ACS_CS_FTRACE((ACS_CS_Master_TRACE, LOG_LEVEL_WARN,
					"ACS_CS_Master::handleClientRequest()\n"
					"Model Version mismatch (Master Version %#0x, Slave Version %#0x). Aborting readTables().",
												ACS_CS_ImmMapper::MODEL_VERSION, version));

			return ACS_CS_TCPClient::OK;
		}

		ACS_CS_ImModel entry;

		try {
			boost::archive::binary_iarchive ia(in);
			ia >> entry;
		}
		catch (boost::archive::archive_exception& e)
		{
			std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::handleClientRequest()\n"
					"Failed to receive slave request. Serialization exception occurred: %s.", e.what()));

			return ACS_CS_TCPClient::OK;
		}
		catch (boost::exception& )
		{
			std::cout << "Boost Exception Occurred!" << std::endl;
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::handleClientRequest()\n"
					"Failed to receive slave request. Serialization exception occurred."));

			return ACS_CS_TCPClient::OK;
		}


		if(entry.size() > 0)
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::handleClientRequest()\n"
				"Successfully received entry"));

			int errNo = 0;
			bool isValid = false;
			ACS_CC_ReturnType saved = ACS_CC_FAILURE;

			 // Add entry to map, so that we know from which client it arrived when sendToAllSlaves()
			ACS_CS_ImModel * newEntry = new ACS_CS_ImModel(entry);
			incomingEntryLock.acquire();
			incomingEntries.insert(entryMap::value_type(newEntry, tcpClient));
			incomingEntryLock.release();

			ACS_CS_ImModel * modelCopy = ACS_CS_ImRepository::instance()->getModelCopy();

			// Check if the model is still valid with the new entries
			if(modelCopy)
			{
				ACS_CS_TRACE((ACS_CS_Master_TRACE,
						"ACS_CS_Master::handleClientRequest()\n"
						"Trying to apply new entry"));

				modelCopy->applySubset(newEntry);

				ACS_CS_TRACE((ACS_CS_Master_TRACE,
						"ACS_CS_Master::handleClientRequest()\n"
						"Entry applied. Validating new model"));
				isValid = modelCopy->isValid(errNo);

			}

			if(isValid){
				ACS_CS_ImModelSaver saver(&entry);
				saved = saver.save("Transaction-Slave");
			}
			else {
				ACS_CS_FTRACE((ACS_CS_Master_TRACE, LOG_LEVEL_ERROR,
						"ACS_CS_Master::handleClientRequest()\n"
						"Could not apply and validate new entry. Validation result: %s",
						ACS_CS_ImUtils::getErrorText(errNo).c_str()));
			}

			if(saved == ACS_CC_FAILURE){
				ACS_CS_FTRACE((ACS_CS_Master_TRACE, LOG_LEVEL_ERROR,
						"ACS_CS_Master::handleClientRequest()\n"
						"Could not save new entry"));
			}

			if(isValid && saved == ACS_CC_SUCCESS)
			{
				ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::handleClientRequest()\n"
					"Successfully updated tables"));

				// Send OK to client
				ACS_CS_ReplicationData replicationData;
				replicationData.size = 0;
				replicationData.type = ACS_CS_NS::ENTRY_OK;
				replicationData.version = ACS_CS_ImmMapper::MODEL_VERSION;
				status = tcpClient->send(reinterpret_cast<char*>(&replicationData), sizeof(replicationData));
			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_Master_TRACE, LOG_LEVEL_ERROR,
					"ACS_CS_Master::handleClientRequest()\n"
					"Failed to update data model, error %d", saved));

				// This entry failed so we can remove it here, since we woun't send it to any slaves
				incomingEntryLock.acquire();
				incomingEntries.erase(newEntry);
				incomingEntryLock.release();
				delete newEntry;

				status = ACS_CS_TCPClient::Failed;
				// Send NOK to client
				ACS_CS_ReplicationData replicationData;
				replicationData.size = 0;
				replicationData.type = ACS_CS_NS::ENTRY_NOK;
				replicationData.version = ACS_CS_ImmMapper::MODEL_VERSION;
				tcpClient->send(reinterpret_cast<char*>(&replicationData), sizeof(replicationData));
			}

			if(modelCopy)
				delete modelCopy;
		}
		else
		{
			// Some error
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::handleClientRequest()\n"
				"Error: failed to receive entry"));

			status = ACS_CS_TCPClient::Failed;
		}

	}



	return status;
}


void ACS_CS_Master::sendOperation(const ACS_CS_ImModel *operation)
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		"ACS_CS_Master::sendOperation(ACS_CS_ImModel *)"));

	if (!operation)
		return;

	ACS_CS_Master::instance()->outModelQueueLock.acquire();

	ACS_CS_Master::instance()->outModelQueue.push_back(new ACS_CS_ImModel(*operation));

	ACS_CS_Master::instance()->outModelQueueLock.release();

	ACS_CS_Event::SetEvent(ACS_CS_Master::instance()->sendOperationEvent);
}


void ACS_CS_Master::checkOutQueue()
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		"ACS_CS_Master::sendOutQueue()"));

	size_t queueSize = 0;

	//ACS_CS_TableOperationEntry * entry = 0;
	ACS_CS_ImModel * entry = 0;

	do
	{
		outModelQueueLock.acquire();

		//queueSize =  outQueue.size();
		queueSize =  outModelQueue.size();

		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::checkOutQueue()\n"
			"queueSize = %d", queueSize));

		if (queueSize > 0)
		{
			entry = outModelQueue.front();
			outModelQueue.pop_front();
			queueSize--;
		}

		outModelQueueLock.release();

		if (entry)
		{
			sendToAllSlaves(entry);

			if(entry)
				delete entry;

			entry = 0;
		}
	}
	while (queueSize);

}


//void ACS_CS_Master::sendToAllSlaves(const ACS_CS_TableOperationEntry * entry)
void ACS_CS_Master::sendToAllSlaves(const ACS_CS_ImModel * model)
{
	ACS_CS_TRACE((ACS_CS_Master_TRACE,
		 "ACS_CS_Master::sendToAllSlaves()"));

	if (!model)
		return;

	//TEMPORARY DEBUG ///////////////////////////////////////////////
//	std::cout << "#################################################################################################" << std::endl;
//	std::cout << "########### MODEL MASTER ########################################################################" << std::endl;
//	std::cout << "#################################################################################################" << std::endl;
//	ACS_CS_ImModel* tmp = const_cast<ACS_CS_ImModel*>(model);
//	tmp->print();
//	std::cout << "#################################################################################################" << std::endl;
//	std::cout << "########### END MASTER ##########################################################################" << std::endl;
//	std::cout << "#################################################################################################" << std::endl;

	std::vector<ACS_CS_TCPClient *>::iterator it;

	// Loop through all slaves
	for (it = connectedSlaves.begin(); it != connectedSlaves.end(); ++it)
	{
		ACS_CS_TCPClient *tcpClient = *it;

		// Look in the incomingEntries map if this slave was the one who sent this entry in the first place
		// If it was we will skip this slave since he has it already
		bool skipClient = false;
		entryMap::iterator mapit;
		incomingEntryLock.acquire();
		for (mapit = incomingEntries.begin(); mapit != incomingEntries.end(); ++mapit)
		{
			ACS_CS_ImModel * tmpEntry = mapit->first;
			ACS_CS_TCPClient * tmpClient = mapit->second;

			if (tmpEntry && tmpClient)
			{

//				std::cout << "#################################################################################################" << std::endl;
//				std::cout << "########### MODEL SLAVE #########################################################################" << std::endl;
//				std::cout << "#################################################################################################" << std::endl;
//				tmpEntry->print();
//				std::cout << "#################################################################################################" << std::endl;
//				std::cout << "########### END SLAVE ###########################################################################" << std::endl;
//				std::cout << "#################################################################################################" << std::endl;


				// Compare clients and entries
				if ( (tcpClient == tmpClient)
					&& (*model == (*tmpEntry)) )
				{
					skipClient = true;
					incomingEntries.erase(mapit);
					delete tmpEntry;
					break;
				}
			}
		}
		incomingEntryLock.release();

		if (skipClient)
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::sendToAllSlaves(). Skipping Slave: %s", tcpClient->getFullAddressAsString().c_str()));
			std::cout << "@@@DBG: Skipping Slave " << tcpClient->getFullAddressAsString() << std::endl;
			continue;
		}

		// Create streams used to serialize data
		std::stringstream entryStream(std::ios::in | std::ios::out | std::ios::binary);
		std::stringstream sizeStream(std::ios::in | std::ios::out | std::ios::binary);

		//------------------------------------------

		try {
			boost::archive::binary_oarchive oa(entryStream);
			oa << * model;
		}
		catch (boost::archive::archive_exception& e)
		{
			std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::sendToAllSlaves()\n"
					"Failed to send entry to slaves. Serialization exception occurred: %s.", e.what()));
			return;
		}
		catch (boost::exception& )
		{
			std::cout << "Boost Exception Occurred!" << std::endl;
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
					"ACS_CS_Master::sendToAllSlaves()\n"
					"Failed to send entry to slaves. Serialization exception occurred."));
			return;
		}


		//------------------------------------------
//
//		   const char * fileName = "ACS_CS_ImModelInfo.serialized";
//		   ofstream out(fileName);
//		   boost::archive::text_oarchive oa2(entryStream);
//		   oa2 << * model;
//
		//------------------------------------------

		// Get total size of table data
		ACS_CS_ReplicationData replicationData;
		replicationData.size = (int) entryStream.tellp();
		replicationData.type = ACS_CS_NS::NewOperation;
		replicationData.version = ACS_CS_ImmMapper::MODEL_VERSION;
		//int tableDataSize = (int) tableStream.tellp();

		ACS_CS_TRACE((ACS_CS_Master_TRACE,
			"ACS_CS_Master::sendToAllSlaves()\n"
			"Data to send %d. Slave %s.", replicationData.size, tcpClient->getFullAddressAsString().c_str()));

		std::cout << "@@@DBG: Sending Replication Data to Slave " << tcpClient->getFullAddressAsString() << std::endl;

		// Write table data to other stream
		(void) sizeStream.write( reinterpret_cast<const char *> (&replicationData), sizeof(replicationData));

		// Create buffer to hold data to be sent over TCP
		char buffer[BUFSIZ];

		// Copy size of table data to buffer
		(void) sizeStream.read(buffer, sizeof(replicationData));

		// Start to copy tables to buffer, the buffer holds BUFSIZ but we have already copied
		// the value of tableDataSize and we should copy no more than what is actually stored
		// in the stream
		entryStream.read(buffer + sizeof(replicationData), std::min( (int) (BUFSIZ - sizeof(replicationData)), (int)replicationData.size));

		int dataCurrentlyInBuffer = (int) sizeof(replicationData) + std::min( (int) (BUFSIZ - sizeof(replicationData)), (int) replicationData.size);
		int tableDataCopied = (int) entryStream.gcount();
		int dataToSend = (int) sizeof(replicationData) + replicationData.size;

		ACS_CS_TCPClient::LinkStatus_t status = ACS_CS_TCPClient::OK;

		// Loop and send until everything in the table stream is sent
		while ( (dataToSend > 0) && (status == ACS_CS_TCPClient::OK) )
		{
			ACS_CS_TRACE((ACS_CS_Master_TRACE,
				"ACS_CS_Master::sendToAllSlaves()\n"
				"Sending %d bytes", dataToSend));

			// Send data stored in buffer
			status = tcpClient->send(buffer, dataCurrentlyInBuffer);

			if (status != ACS_CS_TCPClient::OK)
			{
				it = connectedSlaves.erase(it);
				--it;
				delete tcpClient;
			}
			else
			{
				// Update counter of sent data
				dataToSend -= dataCurrentlyInBuffer;

				if (tableDataCopied < replicationData.size)
				{
					(void) entryStream.read(buffer, std::min(BUFSIZ, replicationData.size - tableDataCopied));
					tableDataCopied += entryStream.gcount();
				}
			}
		}
	}
}

bool ACS_CS_Master::isIncomingEntry(const ACS_CS_ImBase* entry)
{
	bool compare = false;

	if (!entry)
		return compare;

	entryMap::iterator mapit;
	ACS_CS_Master::instance()->incomingEntryLock.acquire();
	for (mapit = ACS_CS_Master::instance()->incomingEntries.begin(); mapit != ACS_CS_Master::instance()->incomingEntries.end(); ++mapit)
	{
		ACS_CS_ImModel * tmpModel = mapit->first;

		if (tmpModel)
		{
			const ACS_CS_ImBase* obj = tmpModel->getObject(entry->rdn.c_str());

			if (obj && *obj == *entry)
			{
				compare = true;
				break;
			}
		}
	}
	ACS_CS_Master::instance()->incomingEntryLock.release();

	return compare;
}

void ACS_CS_Master::finalize()
{
	instance_::close();
}
