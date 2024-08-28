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

#include "ACS_CS_Slave.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <vector>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"

#include "ACS_CS_TableOperation.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_HostFile.h"
#include "ACS_CS_Trace.h"

#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImSerialization.h"

ACS_CS_Trace_TDEF(ACS_CS_Slave_TRACE);

using std::vector;
using std::stringstream;
using ACS_CS_NS::ACS_CS_ReplicationType;
using ACS_CS_NS::ACS_CS_ReplicationData;


ACS_CS_Slave::ACS_CS_Slave():
		tcpClient(0),
		shutdownEvent(0),
		inputStream(0),
		masterRequest(0),
		updateRequest(0),
		masterResult(false),
		updatingTables(false),
		readyForRequest(false)
{
	replicationData = new ACS_CS_ReplicationData();
	memset(replicationData, 0, sizeof(*replicationData));

	// Open shutdown event used to signal that the whole service shall shutdown
	shutdownEvent = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"(%t) ACS_CS_Slave::ACS_CS_Slave()"
				"Error: Cannot open shutdown event"));
	}

	// Create event used to signal that the master has replied to a table operation
	masterReplyEvent = ACS_CS_Event::CreateEvent(false, false, 0);

	if (masterReplyEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"(%t) ACS_CS_Slave::ACS_CS_Slave()"
				"Error: Cannot create master reply event"));
	}

	// Create event used to signal that the master has replied to a table operation
	masterRequestEvent = ACS_CS_Event::CreateEvent(false, false, 0);

	if (masterRequestEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"(%t) ACS_CS_Slave::ACS_CS_Slave()"
				"Error: Cannot create master request event"));
	}

}

ACS_CS_Slave::~ACS_CS_Slave()
{
	if (tcpClient)
	{
		tcpClient->close();
		delete tcpClient;
	}

	if (inputStream)
	{
		delete inputStream;
	}

	if (replicationData)
		delete replicationData;

	if (masterRequest)
	{
		delete masterRequest;
		masterRequest = 0;
	}

	if (masterReplyEvent)
		ACS_CS_Event::CloseEvent(masterReplyEvent);

	if (masterRequestEvent)
		ACS_CS_Event::CloseEvent(masterRequestEvent);
}

ACS_CS_Slave* ACS_CS_Slave::instance()
{
    return instance_::instance();
}

void ACS_CS_Slave::start()
{
	ACS_CS_Slave::instance()->activate();
}

void ACS_CS_Slave::stop()
{
	ACS_CS_Slave::instance()->deActivate();
}


int ACS_CS_Slave::exec()
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::exec()"));

	ACS_CS_TCPClient::LinkStatus_t linkStatus = ACS_CS_TCPClient::NotConnected;

	while (!exit_)
	{
		// If no TCP client or client not ok
		if ( !tcpClient || linkStatus != ACS_CS_TCPClient::OK)
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::exec() Not connected, connecting"));
			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_WARN,"ACS_CS_Slave::exec() Not connected, connecting..."));

			connect();
		}

		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
			"ACS_CS_Slave::exec()"
			"WaitForEvents"));

		// Create array with events
		ACS_CS_EventHandle handleArr[3];
		handleArr[0] = shutdownEvent;
		handleArr[1] = masterRequestEvent;

		if (tcpClient)
			handleArr[2] = tcpClient->getHandle();

		// If TCP client present, we have three events, otherwise two events
		int handleArrSize = (tcpClient != 0 ? 3 : 2);

		// If connection OK, no timeout, else use short timeout so that a reconnection will occur
		int timeOut = (linkStatus == ACS_CS_TCPClient::OK ? 0 : ACS_CS_NS::Daemon_Timeout);

		//ready for hadle request
		this->readyForRequest = (linkStatus == ACS_CS_TCPClient::OK ? true : false);

		int eventIndex = ACS_CS_Event::WaitForEvents(handleArrSize, handleArr, timeOut);

		//restore default value
		this->readyForRequest = false;

		if (eventIndex == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::exec() Error from WaitForEvents"));
			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::exec() ERROR from WaitForEvents closing socket..."));

			close();
		}
		else if (eventIndex == 0) // Shutdown event
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,
					"ACS_CS_Slave::exec()"
					"Shutdown event received"));

			exit_ = true;
		}
		else if (eventIndex == 1) // Master request event
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::exec() Master request event received"));
			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO,"ACS_CS_Slave::exec() MASTER request event received"));

			// Send request
			linkStatus = send();

		}
		else if (eventIndex == 2)  //TCP client event
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::exec() TCP client read received"));
			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO,"ACS_CS_Slave::exec() TCP client READ received"));
			// Read response
			linkStatus = read();
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"ACS_CS_Slave::exec()"
				"Error: unknown event %d", eventIndex));
		}
	}

	close();

	return 0;
}

void ACS_CS_Slave::connect()
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::connect()"));

	close();

	// Wait a second before proceeding in case we have connection problems
	sleep(1);

	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream stream;

	int connected = -1;

	// Try one address at a time
	ACE_INET_Addr addr;

	if (ACS_CS_Registry::isTestEnvironment())
	{
		// Get IP address used in test environment
		uint32_t testIp = ACS_CS_HostFile::getIPAddress(ACS_CS_NS::AP1_FOR_TEST);

		if (testIp)
		{
			// If test address for AP1 is available
			char tempValueStr[INET_ADDRSTRLEN + 1];
			in_addr_t tempValue = htonl(testIp);
			tempValueStr[INET_ADDRSTRLEN] = 0;

			if ( inet_ntop(AF_INET, &tempValue, tempValueStr, INET_ADDRSTRLEN))
			{
				ACS_CS_TRACE((ACS_CS_Slave_TRACE,
					"ACS_CS_Slave::connect()"
					"AP1 in test environment at address %s", tempValueStr));

				std::stringstream ss;
				ss << tempValueStr << ":14014";

				ACS_CS_TRACE((ACS_CS_Slave_TRACE,
					"ACS_CS_Slave::connect()"
					"Connecting to %s", ss.str().c_str()));

				addr.set(ss.str().c_str());
				connected = connector.connect(stream, addr);
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_Slave_TRACE,
					"ACS_CS_Slave::connect()"
					"Failed to convert address for AP1 test"));
			}
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"ACS_CS_Slave::connect()"
				"Failed to find address for AP1 test"));
		}
	}

	if (connected != 0)
	{
		addr.set("192.168.169.1:14014");
		connected = connector.connect(stream, addr);
	}

	if (connected != 0)
	{
		addr.set("192.168.170.1:14014");
		connected = connector.connect(stream, addr);
	}

	if (connected != 0)
	{
		addr.set("192.168.169.2:14014");
		connected = connector.connect(stream, addr);
	}

	if (connected != 0)
	{
		addr.set("192.168.170.2:14014");
		connected = connector.connect(stream, addr);
	}

	if (connected == 0)
	{
		tcpClient = new ACS_CS_TCPClient(stream, addr);

		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::connect() Connected to %s", tcpClient->getFullAddressAsString().c_str()));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE,LOG_LEVEL_WARN,"ACS_CS_Slave::connect() Connected to %s", tcpClient->getFullAddressAsString().c_str()));

		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::connect() Creating inputstream"));

		inputStream = new stringstream(std::ios::in | std::ios::out | std::ios::binary);

		memset(replicationData, 0, sizeof(*replicationData));
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::connect() Error: Failed to connect"));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::connect() ERROR: Failed to connect"));
	}

	//HS35978 - begin
	//used to activate the KEEPALIVE option
	int optval = 1;
	stream.set_option(SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
	//HS35978 - end
}

ACS_CS_TCPClient::LinkStatus_t ACS_CS_Slave::send()
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::send()"));

	if (!tcpClient)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
			"ACS_CS_Slave::send()"
			"Error: No TCP client"));

		return ACS_CS_TCPClient::NotConnected;
	}

	ACS_CS_ImModel * modelEntry = 0;

	lock.acquire();

	if (masterRequest)
		modelEntry = new ACS_CS_ImModel(*masterRequest);

	lock.release();

	if (!modelEntry)
		return ACS_CS_TCPClient::Failed;

	ACS_CS_TCPClient::LinkStatus_t status = ACS_CS_TCPClient::OK;

	// Create streams used to serialize data
	std::stringstream dataStream(std::ios::in | std::ios::out | std::ios::binary);
	std::stringstream sizeStream(std::ios::in | std::ios::out | std::ios::binary);

	//dataStream << *modelEntry;


	try {
		boost::archive::binary_oarchive oa(dataStream);
		oa << * modelEntry;
	}
	catch (boost::archive::archive_exception& e)
	{
		std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"ACS_CS_Slave::send()"
				"Failed to send request to Master. Serialization exception occurred: %s.", e.what()));
		delete modelEntry;
		return ACS_CS_TCPClient::Failed;
	}
	catch (boost::exception& )
	{
		std::cout << "Boost Exception Occurred!" << std::endl;
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"ACS_CS_Slave::send()"
				"Failed to send request to Master. Serialization exception occurred."));
		delete modelEntry;
		return ACS_CS_TCPClient::Failed;
	}

	// Get total size of data
	ACS_CS_ReplicationData replicationData;
	replicationData.size = (int) dataStream.tellp();
	replicationData.type = ACS_CS_NS::NewOperation;
	replicationData.version = ACS_CS_ImmMapper::MODEL_VERSION;
	//int dataSize = (int) dataStream.tellp();

	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::send()"
		"Data to send %d", replicationData.size));

	// Write table data to other stream
	(void) sizeStream.write( reinterpret_cast<const char *> (&replicationData), sizeof(replicationData));

	// Create buffer to hold data to be sent over TCP
	char buffer[BUFSIZ];

	// Copy size of table data to buffer
	(void) sizeStream.read(buffer, sizeof(replicationData));

	dataStream.read(buffer + sizeof(replicationData), std::min( (int) (BUFSIZ - sizeof(replicationData)), (int)replicationData.size));

	int dataCurrentlyInBuffer = (int) sizeof(replicationData) + std::min( (int) (BUFSIZ - sizeof(replicationData)), (int) replicationData.size);
	int dataCopied = (int) dataStream.gcount();
	int dataToSend = (int) sizeof(replicationData) + replicationData.size;

	// Loop and send until everything in the table stream is sent
	while ( (dataToSend > 0) && (status == ACS_CS_TCPClient::OK) )
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
			"ACS_CS_Slave::send()"
			"Sending %d bytes", dataToSend));

		// Send data stored in buffer
		status = tcpClient->send(buffer, dataCurrentlyInBuffer);
		// Update counter of sent data
		dataToSend -= dataCurrentlyInBuffer;

		if (dataCopied < replicationData.size)
		{
			dataCurrentlyInBuffer = std::min(BUFSIZ, replicationData.size - dataCopied);
			(void) dataStream.read(buffer, dataCurrentlyInBuffer);
			dataCopied += dataStream.gcount();
		}
	}

	delete modelEntry;

	return status;
}


ACS_CS_TCPClient::LinkStatus_t ACS_CS_Slave::read()
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::read()"));

	if (!tcpClient)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::send() Error: No TCP client"));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::send() ERROR: No TCP client"));

		return ACS_CS_TCPClient::NotConnected;
	}

	ACS_CS_TCPClient::LinkStatus_t status = ACS_CS_TCPClient::OK;

	ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::read() trying to read from %s", tcpClient->getFullAddressAsString().c_str()));
//	ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO,"ACS_CS_Slave::read() trying to read from %s", tcpClient->getFullAddressAsString().c_str()));

	// Read data from TCP
	//int dataToRead = (int)replicationData->size - ((int) inputStream->tellp() - (int) inputStream->tellg());
	//int tmpSize = std::min(BUFSIZ, dataToRead);
	//char buffer[tmpSize];
	char buffer[BUFSIZ];
	int lastRead = 0;
	//status = tcpClient->read(buffer, tmpSize, lastRead);
	status = tcpClient->read(buffer, BUFSIZ, lastRead);

	if (status != ACS_CS_TCPClient::OK)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::read() Failed to read from %s, status %d", tcpClient->getFullAddressAsString().c_str(), status));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::read()  Failed to read from %s, status %d", tcpClient->getFullAddressAsString().c_str(), status));

		return status;
	}

	bool result = true;


	if (lastRead > 0)
	{
		// Write data to stream
		inputStream->write(buffer, lastRead);

		// Calculate how much data there is to parse from
		size_t availableData = (int) inputStream->tellp() - (int) inputStream->tellg();

		// TEST *******************
		std::cout << "ACS_CS_Slave::read() --> availableData=" << availableData << " - lastRead=" << lastRead << " - BUFSIZ=" << BUFSIZ << std::endl;

		do
		{
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,
				"ACS_CS_Slave::read()"
				"Available data %d", availableData));

			// If enough data for replicationData has been received
			if ( replicationData->version == ACS_CS_NS::UNSPECIFIED_VERSION
				&& availableData >= sizeof(ACS_CS_ReplicationData) )
			{
				(void) inputStream->read( reinterpret_cast<char *> (replicationData), sizeof(ACS_CS_ReplicationData) );

				ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::read() Replication type = %d", replicationData->type));
				ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO,"ACS_CS_Slave::read() Replication type = %d", replicationData->type));


				// TEST *******************
				std::cout << "ACS_CS_Slave::read() -> replicationData READ" << std::endl;
				std::cout << "ACS_CS_Slave::read() --> replicationData->size=" << replicationData->size << std::endl;
			}

			// Use replicationData to decide what to do
			if ( replicationData->type == ACS_CS_NS::NewTables )	// New set if tables (initial syncronization)
			{
				if (replicationData->version != ACS_CS_ImmMapper::MODEL_VERSION)
				{
					ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_WARN, "ACS_CS_Slave::read() Model Version mismatch (Master Version %s, Slave Version %s). Aborting readTables().",
							getVersionString(replicationData->version).c_str(), getVersionString(ACS_CS_ImmMapper::MODEL_VERSION).c_str()));
					result = false;
					resetStream();
				}
				else
				{
					result = readTables(replicationData->size);
				}
			}
			else if ( replicationData->type == ACS_CS_NS::NewOperation ) // New operation (e.g. hwcadd etc)
			{
				if (replicationData->version != ACS_CS_ImmMapper::MODEL_VERSION)
				{
					ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_WARN, "ACS_CS_Slave::read() Model Version mismatch (Master Version %s, Slave Version %s). Aborting readNewOperation().",
							getVersionString(replicationData->version).c_str(), getVersionString(ACS_CS_ImmMapper::MODEL_VERSION).c_str()));
					result = false;
					resetStream();
				}
				else
				{
					result = readNewOperation(replicationData->size);
				}
			}
			else if ( replicationData->type == ACS_CS_NS::ENTRY_OK )	// ACK on operation
			{
				this->masterResult = true;
				ACS_CS_Event::SetEvent(masterReplyEvent);
				result = true;
				resetStream();
			}
			else if ( replicationData->type == ACS_CS_NS::ENTRY_NOK )	// NAK on operation
			{
				this->masterResult = false;
				ACS_CS_Event::SetEvent(masterReplyEvent);
				result = true;
				resetStream();
			}

			availableData = (int) inputStream->tellp() - (int) inputStream->tellg();

			// TEST *******************
			std::cout << "ACS_CS_Slave::read() --> availableData=" << availableData << " - lastRead=" << lastRead << " - replicationData->size=" << replicationData->size << std::endl;

		}
		// While there is at least enough data to parse for replicationData (the header)
		while (replicationData->version == ACS_CS_NS::UNSPECIFIED_VERSION
			&& availableData >= sizeof(ACS_CS_ReplicationData) && result);
	}

	// If anything fails we consider this TCP connection failed
	if (!result)
		status = ACS_CS_TCPClient::Failed;

	ACS_CS_TRACE((ACS_CS_Slave_TRACE, "ACS_CS_Slave::read()  exiting read function from %s with status %d", tcpClient->getFullAddressAsString().c_str(), status));
	ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO, "ACS_CS_Slave::read()  exiting read function from %s with status %d", tcpClient->getFullAddressAsString().c_str(), status));

	return status;
}


bool ACS_CS_Slave::sendToMaster(ACS_CS_ImModel * model)
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::sendToMaster()"));

	if (!model)
		return false;

	ACS_CS_Slave::instance()->lock.acquire();

	ACS_CS_Slave::instance()->masterRequest = new ACS_CS_ImModel(*model);
	ACS_CS_Slave::instance()->masterResult = false;

	ACS_CS_Slave::instance()->lock.release();

	// Signal slave that there is work to do
	ACS_CS_Event::SetEvent(ACS_CS_Slave::instance()->masterRequestEvent);

	ACS_CS_EventHandle eventArr[1];
	eventArr[0] = ACS_CS_Slave::instance()->masterReplyEvent;

	// Wait for slave to respond
	int result = ACS_CS_Event::WaitForEvents(1, eventArr, ACS_CS_NS::Daemon_Timeout);

	bool returnVal = false;

	if (result == ACS_CS_EVENT_RC_ERROR || result == ACS_CS_EVENT_RC_TIMEOUT)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::sendToMaster(), Error: Failed to wait for result"));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::sendToMaster(), Error: Failed to wait for result"));

		//Reset the event in case it wasn't received
		ACS_CS_Event::ResetEvent(ACS_CS_Slave::instance()->masterRequestEvent);

	}
	else
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::sendToMaster(), Result %s", ACS_CS_Slave::instance()->masterResult ? "true" : "false"));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE,LOG_LEVEL_INFO,"ACS_CS_Slave::sendToMaster(), Result %s", ACS_CS_Slave::instance()->masterResult ? "true" : "false"));

		// Return the result
		returnVal = ACS_CS_Slave::instance()->masterResult;
	}

	// Delete temporary copy of Master Request
	ACS_CS_Slave::instance()->lock.acquire();

	delete ACS_CS_Slave::instance()->masterRequest;
	ACS_CS_Slave::instance()->masterRequest = 0;

	ACS_CS_Slave::instance()->lock.release();

	return returnVal;

}

bool ACS_CS_Slave::isUpdatingTables(const ACS_CS_ImBase* entry)
{
//	ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO,"ACS_CS_Slave::isUpdatingTables(%s) ", ACS_CS_Slave::instance()->updatingTables ? "true" : "false"));

	bool compare = false;

	if (!entry)
		return compare;

	if (!ACS_CS_Slave::instance()->updatingTables)
		return compare;

	ACS_CS_ImModel * masterTables = 0;

	ACS_CS_Slave::instance()->lock.acquire();

	if (ACS_CS_Slave::instance()->masterTables)
		masterTables = new ACS_CS_ImModel(*ACS_CS_Slave::instance()->masterTables);

	ACS_CS_Slave::instance()->lock.release();

	if (masterTables)
	{
		const ACS_CS_ImBase *obj = masterTables->getObject(entry->rdn.c_str());

		if (obj && *obj == *entry)
			compare = true;

		delete masterTables;
	}


	return compare;
}

bool ACS_CS_Slave::isUpdatingTables(const ACS_CS_ImModel * modelEntry)
{
	bool compare = false;

	if (!modelEntry)
		return compare;

	if (!ACS_CS_Slave::instance()->updatingTables)
		return compare;

	ACS_CS_ImModel * masterTables = 0;

	ACS_CS_Slave::instance()->lock.acquire();

	if (ACS_CS_Slave::instance()->masterTables)
		masterTables = new ACS_CS_ImModel(*ACS_CS_Slave::instance()->masterTables);

	ACS_CS_Slave::instance()->lock.release();

	if (masterTables && masterTables->operator==(*modelEntry))
		compare = true;

	if (masterTables)
		delete masterTables;

	return compare;
}


bool ACS_CS_Slave::readTables(int size)
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::readTables()"
		"size = %d", size));

	if (!inputStream || !size)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::readTables() Error: inputStream = %s, size = %d", inputStream ? "ptr" : "0", size));
		ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::readTables() Error: inputStream = %s, size = %d", inputStream ? "ptr" : "0", size));

		return false;
	}

	bool result = true;

	// Calculate how much data there is to parse from
	int availableData = (int) inputStream->tellp() - (int) inputStream->tellg();

	// TEST *******************
	std::cout << "ACS_CS_Slave::readTables() --> availableData=" << availableData << std::endl;

	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::readTables()"
		"Available data %d", availableData));

	// If enough table data has been read
	if ( availableData >= size )
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
			"ACS_CS_Slave::readTables()"
			"Parsing tables, readData = %d", availableData));

		ACS_CS_ImModel * model = new ACS_CS_ImModel();

		try {
			boost::archive::binary_iarchive ia(*inputStream);
			ia >> *model;
		}
		catch (boost::archive::archive_exception& e)
		{
			std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::readTables() Failed to read tables from Master. Serialization exception occurred: %s.", e.what()));
			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::readTables() Failed to read tables from Master. Serialization exception occurred: %s.", e.what()));
			delete model;
			resetStream();
			return false;
		}
		catch (boost::exception& )
		{
			std::cout << "Boost Exception Occurred!" << std::endl;
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::readTables() Failed to read tables from Master. Serialization exception occurred."));
			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::readTables() Failed to read tables from Master. Serialization exception occurred."));
			delete model;
			resetStream();
			return false;
		}
//		catch(...)
//		{
//			std::cout << "Generic Exception Occurred!" << std::endl;
//			ACS_CS_TRACE((ACS_CS_Slave_TRACE,"ACS_CS_Slave::readTables() Failed to read tables from Master. Serialization exception occurred."));
//			ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::readTables() Failed to read tables from Master. Serialization exception occurred."));
//			delete model;
//			resetStream();
//			return false;
//		}

		//Save tables on IMM
		ACS_CS_ImModel *oldModel = ACS_CS_ImRepository::instance()->getModelCopy();

		ACS_CS_ImModel *diffModel = oldModel->findDifferences(model);



		if (diffModel && diffModel->size() > 0)
		{

			//TEMPORARY DEBUG ///////////////////////////////////////////////
//			std::cout << "#################################################################################################" << std::endl;
//			std::cout << "########### DIFF MODEL   ########################################################################" << std::endl;
//			std::cout << "#################################################################################################" << std::endl;
//			diffModel->print();
//			std::cout << "#################################################################################################" << std::endl;
//			std::cout << "########### END DIFF   ##########################################################################" << std::endl;
//			std::cout << "#################################################################################################" << std::endl;

			ACS_CS_TRACE((ACS_CS_Slave_TRACE, "ACS_CS_Slave::readTables() Differences found, updatingTables set to true"));

			ACS_CS_Slave::instance()->lock.acquire();
			this->masterTables = diffModel;
			ACS_CS_Slave::instance()->lock.release();

			ACS_CS_Slave::instance()->updatingTables = true;

			//Something is changed on the Master. Update Slave Tables accordingly..
			ACS_CS_ImModelSaver saver(diffModel);
			ACS_CC_ReturnType saved = saver.save("Initial-Sync");

			ACS_CS_Slave::instance()->updatingTables = false;

			ACS_CS_Slave::instance()->lock.acquire();
			this->masterTables = 0;
			ACS_CS_Slave::instance()->lock.release();

			ACS_CS_TRACE((ACS_CS_Slave_TRACE, "ACS_CS_Slave::readTables() Differences found, updatingTables restored to false"));

			if(saved == ACS_CC_FAILURE){

				ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,"ACS_CS_Slave::readTables() Could not save new Tables, reload Model from IMM"));

				ACS_CS_ImRepository::instance()->reloadModel();

				result = false;
			}
		}

		delete oldModel;
		delete model;

		if (diffModel)
			delete diffModel;

		//if successfully saveModel()
		resetStream();

	}

	return result;

}

void ACS_CS_Slave::close()
{
	if (tcpClient)
	{
		tcpClient->close();
		delete tcpClient;
		tcpClient = 0;
	}

	if (inputStream)
	{
		delete inputStream;
		inputStream = 0;
	}
}

void ACS_CS_Slave::resetStream()
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::resetStream()"));

	if (inputStream)
	{
		stringstream * newStream = new stringstream(std::ios::in | std::ios::out | std::ios::binary);

		// Copy stream
		char buffer[BUFSIZ];
		size_t readData = 0;

		do
		{
			(void) inputStream->read(buffer, BUFSIZ);
			readData = inputStream->gcount();

			if (readData > 0)
				(void) newStream->write(buffer, readData);
		}
		while ( ! inputStream->eof());

		delete inputStream;
		inputStream = newStream;
		// Reset the start position for the current operation
		//positionOfLastEntry = inputStream->tellg();
	}
	else
	{
		inputStream = new stringstream(std::ios::in | std::ios::out | std::ios::binary);
	}

	if (replicationData)
	{
		// Reset the replication data (the header that is positioned first in the stream)
		memset(replicationData, 0, sizeof(*replicationData));
	}
}


bool ACS_CS_Slave::readNewOperation(int size)
{
	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::readNewOperation()"
		"size = %d", size));

	if (!inputStream || !size)
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
			"ACS_CS_Slave::readNewOperation()"
			"Error: inputStream = %s, size = %d",
			inputStream ? "ptr" : "0", size));

		return false;
	}

	bool result = true;
	int availableData = (int) inputStream->tellp() - (int) inputStream->tellg();

	ACS_CS_TRACE((ACS_CS_Slave_TRACE,
		"ACS_CS_Slave::readNewOperation()"
		"Available data %d", availableData));

	// If all table data read
	if ( availableData >= size )
	{
		ACS_CS_TRACE((ACS_CS_Slave_TRACE,
			"ACS_CS_Slave::readNewOperation()"
			"Parsing new table operation"));

		// Parse the operation
		ACS_CS_ImModel * model = new ACS_CS_ImModel();


		try {
			boost::archive::binary_iarchive ia(*inputStream);
			ia >> *model;
		}
		catch (boost::archive::archive_exception& e)
		{
			std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,
					"ACS_CS_Slave::readNewOperation()"
					"Failed to read operation from Master. Serialization exception occurred: %s.", e.what()));
			delete model;
			resetStream();
			return false;
		}
		catch (boost::exception& )
		{
			std::cout << "Boost Exception Occurred!" << std::endl;
			ACS_CS_TRACE((ACS_CS_Slave_TRACE,
					"ACS_CS_Slave::readNewOperation()"
					"Failed to read operation from Master. Serialization exception occurred."));
			delete model;
			resetStream();
			return false;
		}


		{
			// Perform update
			//ACS_CS_API_TableLoader tableLoader;

			ACS_CS_Slave::instance()->lock.acquire();

			this->updateRequest = model;
			//this->copyOfRequest = new ACS_CS_ImModel(*model);

			ACS_CS_Slave::instance()->lock.release();


			//TEMPORARY DEBUG ///////////////////////////////////////////////
//			std::cout << "#################################################################################################" << std::endl;
//			std::cout << "########### New OP MODEL   ######################################################################" << std::endl;
//			std::cout << "#################################################################################################" << std::endl;
//			model->print();
//			std::cout << "#################################################################################################" << std::endl;
//			std::cout << "########### END New OP   ########################################################################" << std::endl;
//			std::cout << "#################################################################################################" << std::endl;

			//Save Entry on IMM
			ACS_CS_ImModelSaver saver(model);
			ACS_CC_ReturnType saved = saver.save("Transaction-Master");

			if(saved == ACS_CC_FAILURE){

				ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_ERROR,
						"ACS_CS_Slave::readNewOperation()"
						"Could not save new entry"));
				result = false;
			}
			else
			{
				//ACS_CS_ImRepository::instance()->applySubset(model);
				result = true;
			}

			//(void) tableLoader.updateTables(&e);

			ACS_CS_Slave::instance()->lock.acquire();

			this->updateRequest = 0;
//			delete this->copyOfRequest;
//			this->copyOfRequest = 0;

			ACS_CS_Slave::instance()->lock.release();

			delete model;

			resetStream();
		}
	}

	return result;
}

bool ACS_CS_Slave::isUpdatingEntry(const ACS_CS_ImModel * modelEntry)
{
	bool compare = false;

	if (!modelEntry)
		return compare;

	ACS_CS_ImModel * updateRequest = 0;

	ACS_CS_Slave::instance()->lock.acquire();

	if (ACS_CS_Slave::instance()->updateRequest)
		updateRequest = new ACS_CS_ImModel(*ACS_CS_Slave::instance()->updateRequest);

	ACS_CS_Slave::instance()->lock.release();

	//TEMPORARY DEBUG ///////////////////////////////////////////////
//	std::cout << "#################################################################################################" << std::endl;
//	std::cout << "########### Looking for ModelEntry   ############################################################" << std::endl;
//	std::cout << "#################################################################################################" << std::endl;
//	ACS_CS_ImModel * modelEn = const_cast<ACS_CS_ImModel*>(modelEntry);
//	modelEn->print();
//	std::cout << "#################################################################################################" << std::endl;
//	std::cout << "########### END Looking for ModelEntry  #########################################################" << std::endl;
//	std::cout << "#################################################################################################" << std::endl;

	if (updateRequest && updateRequest->operator==(*modelEntry))
		compare = true;

	if (updateRequest)
		delete updateRequest;

	return compare;
}

bool ACS_CS_Slave::isUpdatingEntry(const ACS_CS_ImBase * entry)
{
	bool compare = false;

	if (!entry)
		return compare;

	ACS_CS_ImModel * updateRequest = 0;

	ACS_CS_Slave::instance()->lock.acquire();

	if (ACS_CS_Slave::instance()->updateRequest)
		updateRequest = new ACS_CS_ImModel(*ACS_CS_Slave::instance()->updateRequest);

	ACS_CS_Slave::instance()->lock.release();

	if (updateRequest)
	{
		const ACS_CS_ImBase *obj = updateRequest->getObject(entry->rdn.c_str());

		if (obj && *obj == *entry)
			compare = true;
	}

	if (updateRequest)
		delete updateRequest;

	return compare;
}

void ACS_CS_Slave::finalize()
{
	instance_::close();
}

bool ACS_CS_Slave::isReadyForRequest()
{
//	ACS_CS_FTRACE((ACS_CS_Slave_TRACE, LOG_LEVEL_INFO,"ACS_CS_Slave::readyForRequest(%s) ", ACS_CS_Slave::instance()->readyForRequest ? "true" : "false"));
	return ACS_CS_Slave::instance()->readyForRequest;
}

std::string ACS_CS_Slave::getVersionString(int hexVersion)
{
	std::string version = "";

	if (hexVersion)
	{
		char buffer[32];
		sprintf(buffer, "%#10.8x", hexVersion);
		version = buffer;
	}

	return version;
}
