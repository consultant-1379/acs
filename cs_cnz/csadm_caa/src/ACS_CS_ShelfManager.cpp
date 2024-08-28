/*
 * ACS_CS_ShelfManager.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: estevol
 */

#include "ACS_CS_ShelfManager.h"

#include "ACS_CS_Util.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Registry.h"

#include "ACS_CS_Trace.h"


ACS_CS_Trace_TDEF(ACS_CS_ShelfManager_TRACE);
#include <iostream>


using std::string;
using std::ostringstream;
using namespace ACS_CS_NS;

ACS_CS_ShelfManager::ACS_CS_ShelfManager(ACS_CS_ServiceHandler* ParentService):
handleSNMPRequestEvent(0),
shutdownEvent(0),
serviceCs(0),
queueUpdated(false),
outQueueEmpty(false)
{
	if (ParentService !=0x0)
		serviceCs = ParentService;/*so that it is possible to use it without CS service,
			to facilitate easier testing */

	// This event is signaled when the service must shutdown
	shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);

	if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_ShelfManager_TRACE,
				"(%t) ACS_CS_ShelfManager::ACS_CS_ShelfManager()\n"
				"Error: Cannot open shutdown event\n"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Open Event",
				"Cannot open event",
				"");
	}

	// This event is signaled when there is SNMP data to send
	handleSNMPRequestEvent = ACS_CS_Event::CreateEvent(false, false, NULL);

	if (handleSNMPRequestEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_ShelfManager_TRACE,
				"(%t) ACS_CS_ShelfManager::ACS_CS_ShelfManager()\n"
				"Error: Cannot open shutdown event\n"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				"Open Event",
				"Cannot open event",
				"");
	}

}

ACS_CS_ShelfManager::~ACS_CS_ShelfManager()
{

	ACS_CS_Event::CloseEvent(handleSNMPRequestEvent);
	handleSNMPRequestEvent = 0;

}


void ACS_CS_ShelfManager::cancelClient(const ACS_CS_DHCP_Info & infoA, const ACS_CS_DHCP_Info & infoB) {

	dhcpQueueLock.start_writing();

	for (size_t i = normalDhcpVector.size(); i > 0; i--)
	{
		if (normalDhcpVector[i - 1].getMagAddress() == infoA.getMagAddress()
				&& normalDhcpVector[i - 1].getSlot() == infoA.getSlot()
				&& (normalDhcpVector[i - 1].getNetwork() == infoA.getNetwork()
						|| normalDhcpVector[i - 1].getNetwork() == infoB.getNetwork()) )
		{
			//delete normalDhcpVector[i - 1];
			normalDhcpVector.erase(normalDhcpVector.begin() + i - 1);
		}

	}

	dhcpQueueLock.stop_writing();

	if(normalDhcpVector.size() == 0)
		outQueueEmpty = true;

	queueUpdated = true;

}

void ACS_CS_ShelfManager::removeCanceledClients()
{
	dhcpQueueLock.start_writing();
	cout<<"size of canceled client"<<canceledClients.size()<<endl;
	while (!canceledClients.empty())
	{
		ACS_CS_DHCP_Info info = canceledClients.front();
		canceledClients.pop();

		for (size_t i = normalDhcpVector.size(); i > 0; i--)
		{
			if (normalDhcpVector[i - 1].getMagAddress() == info.getMagAddress()
					&& normalDhcpVector[i - 1].getSlot() == info.getSlot()
					&& normalDhcpVector[i - 1].getNetwork() == info.getNetwork() )
			{
				cout<<"size of normal vector size "<<normalDhcpVector.size()<<endl;
				//delete normalDhcpVector[i - 1];
				normalDhcpVector.erase(normalDhcpVector.begin() + i - 1);
				
				cout<<"size of normal vector size after delete "<<normalDhcpVector.size()<<endl;
				break;
			}

		}
	}
	dhcpQueueLock.stop_writing();

	ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_ShelfManager::removeCanceledClients()\n"
			" normalDhcpVector=%d",normalDhcpVector.size()));

	if(normalDhcpVector.size() == 0)
		outQueueEmpty = true;

}


void ACS_CS_ShelfManager::insertIntoDhcpQueue (const ACS_CS_DHCP_Info & client)
{
	ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_ShelfManager::insertIntoClientOutQueue()\n"
			"client.getHostName %s normalDhcpVector=%d\n",client.getHostName().c_str(),normalDhcpVector.size()));

	printf("client.getHostName %s normalDhcpVector=%d...",client.getHostName().c_str(),(int)normalDhcpVector.size());
	std::vector<ACS_CS_DHCP_Info>::iterator i = std::find(normalDhcpVector.begin(), normalDhcpVector.end(), client);
        if(i==normalDhcpVector.end())
        {
	dhcpQueueLock.start_writing();
	normalDhcpVector.insert(normalDhcpVector.begin(), client);			// Add client to update queue
	dhcpQueueLock.stop_writing();
	
	outQueueEmpty = false;

	cout << "....ADDED" << endl;
	}
ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
                        "ACS_CS_ShelfManager::insertIntoDhcpQueue() exit\n"));	
}

void ACS_CS_ShelfManager::wakeUpQueue ()
{

	cout << "Signaling handleSNMPRequestEvent..." << endl;
	queueUpdated = true;

	if (!ACS_CS_Event::SetEvent(handleSNMPRequestEvent))		// Notify updater
	{
		ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_ERROR,
				"(%t) ACS_CS_ShelfManager::wakeUpQueue()\n"
				"Error: Cannot signal event\n"));

		cout << "Error: Cannot signal event\n";
	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
				"(%t) ACS_CS_ShelfManager::wakeUpQueue()\n"
				"Event Signaled\n"));

		cout << "...Event Signaled" << endl;
	}

	ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_ShelfManager::wakeUpQueue()\n"
			"exiting from method"));

}

int ACS_CS_ShelfManager::exec ()
{
	ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_ShelfManager::exec()\n"
			"Thread starting\n"));

	// Array to use with WaitForEvents
	ACS_CS_EventHandle handleArr[2] = {shutdownEvent, handleSNMPRequestEvent};

	while ( ! exit_ )	// Loop as long as exit_ is false (set to false by the deactivate function)
	{
		int timeout = updateTimeout();

		// Wait for tasks to handle
		int returnValue = ACS_CS_Event::WaitForEvents( 2, handleArr, timeout);

		if (returnValue == ACS_CS_EVENT_RC_ERROR)		// Event failed
		{
			ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_ERROR,
					"(%t) ACS_CS_ShelfManager::exec()\n"
					"Error: WaitForEvents\n"));

			ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
					"Waiting for socket",
					"Unable to wait for event.",
					"");
		}
		else if (returnValue == ACS_CS_EVENT_RC_TIMEOUT)	// Wait timeout
		{
			ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
					"(%t) ACS_CS_ShelfManager::exec()\n"
					"Event timeout\n"));

			checkOutQueue();				// Send first item in queue
			removeCanceledClients();
		}
		else if (returnValue == 0)			// Shutdown event
		{
			ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
					"(%t) ACS_CS_ShelfManager::exec()\n"
					"Shutdown event signaled. Closing down SNMP session.\n"));

			exit_ = true;					// Leave this thread function
		}
		else if (returnValue == 1)			// SNMP update event
		{
			ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
					"(%t) ACS_CS_ShelfManager::exec()\n"
					"SNMP event.\n"));

			checkOutQueue();				// Send first item in queue
			removeCanceledClients();
		}
	}

	cleanUp();

	ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_ShelfManager::exec()\n"
			"Thread finishing\n"));

	return 0;
}

void ACS_CS_ShelfManager::putAtBottomIntoDhcpQueue (const ACS_CS_DHCP_Info & client)
{

        ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
                        "(%t) ACS_CS_ShelfManager::putAtBottomIntoDhcpQueue()\n"
                        "client.getHostName %s normalDhcpVector=%d\n",client.getHostName().c_str(),normalDhcpVector.size()));
        printf("client.getHostName %s normalDhcpVector=%d...",client.getHostName().c_str(),(int)normalDhcpVector.size());
        std::vector<ACS_CS_DHCP_Info>::iterator i = std::find(normalDhcpVector.begin(), normalDhcpVector.end(), client);
        if(i !=normalDhcpVector.end())
        {
                dhcpQueueLock.start_writing();
                normalDhcpVector.erase(i);
                normalDhcpVector.push_back(client);
                dhcpQueueLock.stop_writing();

        }
        ACS_CS_FTRACE((ACS_CS_ShelfManager_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_ShelfManager::putAtBottomIntoDhcpQueue() exit\n"));
}

