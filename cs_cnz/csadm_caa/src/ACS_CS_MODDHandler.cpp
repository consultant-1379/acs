

//	-
//	Copyright Ericsson AB 2011. All rights reserved.
//	-

// ACS_CS_MODDHandler
#include "ACS_CS_MODDHandler.h"

#include <iostream>

#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_MODD_Header.h"
#include "ACS_CS_MODD_Change.h"
#include "ACS_CS_MODD_ChangeResponse.h"
#include "ACS_CS_MODD_Revert.h"
#include "ACS_CS_MODD_RevertResponse.h"
#include "ACS_CS_DHCP_Configurator.h"
//#include "ACS_CS_Internal_Table.h"
#include "ACS_CS_Common_Util.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Common_Util.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Timer.h"
#include "ACS_CS_Trace.h"

#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImEntryIdHelper.h"
#include "ACS_CS_ImModelSaver.h"


ACS_CS_Trace_TDEF(ACS_CS_MODDHandler_TRACE);

using namespace ACS_CS_Protocol;
using std::string;
using std::map;
using std::vector;

namespace
{
	// Textual representations of the MODD protocol result values.
   const char SuccessDescription[]     = "Boot image filename successfully changed";
   const char FailureDescription[]     = "It was not possible to perform the requested Change operation right now, try again later";
   const char NoSuchEntryDescription[] = "No match for the provided MAC address was found";
   const char BusyDescription[]        = "Boot image filename change was not processed, resources were locked";

}


ACS_CS_RevertTimer::ACS_CS_RevertTimer(ACS_CS_MODDHandler * handler,
		const Default_DHCP_Entry * entry, uint16_t timeout)
	: handler(handler),
	  entry(entry),
	  timeout(timeout),
	  timerHandle(-1),
	  cancelHandle(-1)
{}


int ACS_CS_RevertTimer::exec()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::exec()\n"
		"Entering function"));

	// Open shutdown event used to signal that the whole service shall shutdown
	ACS_CS_EventHandle shutdownEvent = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	cancelHandle = ACS_CS_Event::CreateEvent(true, false, 0);

	if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_RevertTimer::exec()\n"
			"Error: Cannot open shutdown event"));
	}

	// Create the timer and set it to expire after timeout seconds
	timerHandle = ACS_CS_Timer::CreateTimer(true, 0);

	if ( ! ACS_CS_Timer::SetTimer(timerHandle, timeout) )
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::exec()\n"
			"Error: Failed to set timer"));
	}

	ACS_CS_EventHandle arr[3];
	arr[0] = shutdownEvent;
	arr[1] = cancelHandle;
	arr[2] = timerHandle;

	// Wait for the
	int eventIndex = ACS_CS_Event::WaitForEvents(3, arr, 0);

	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::exec()\n"
			"Event received %d", eventIndex));

	// Event index 0 and 1 should just cause the thread to finish so we don't do anything with them
	// Event index 2 is the timer event we are waiting for
	if (eventIndex == 2)	// Timer event
	{
		// If thread has been deactivated while we were asleep
		// we don't do anything
		if ( !exit_)
			handler->performAutomaticRevert(*entry);
	}
	else if (eventIndex < 0) // Some failure
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::exec()\n"
			"Error: Failed to wait for event, result = %d", eventIndex));
	}

	// Cleanup old unused timers
	handler->timerCleanup();

	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::exec()\n"
		"Leaving function"));

	return 0;
}


void ACS_CS_RevertTimer::cancelRevertTimer()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::cancelRevertTimer()"));

	ACS_CS_Event::SetEvent(cancelHandle);
}


// Class ACS_CS_MODDHandler 

ACS_CS_MODDHandler::ACS_CS_MODDHandler(uint32_t bootIpEthA, uint32_t bootIpEthB, APZ_Type type):
		bootIpEthA(bootIpEthA),
		bootIpEthB(bootIpEthB),
        apzType(type)

{
	cout << "MODD Handler COnstructor " << apzType << std::endl;
}


ACS_CS_MODDHandler::~ACS_CS_MODDHandler()
{
	timerCleanup();
}


int ACS_CS_MODDHandler::handleRequest(ACS_CS_PDU *pdu)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::handleRequest()\n"
		"Entering method\n"));

	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleRequest()\n"
			"Error: Invalid PDU\n"));

		return -1;
	}

	int error = 0;

	const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU

	if (header)
	{
		// Try to acquire lock
		// only one caller accepted at a time
		int result = _lock.tryacquire();

		if (result != -1)
		{
			// Check which primitive that has been received and call handler function
			if (header->getPrimitiveId() == Primitive_MODD_Change)
			{
				error = handleChange(pdu);
			}
			else if (header->getPrimitiveId() == Primitive_MODD_Revert)
			{
				error = handleRevert(pdu);
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
					"(%t) ACS_CS_MODDHandler::handleRequest()\n"
					"Error: Unknown primitive, id = %d\n",
					header->getPrimitiveId()));

				error = -1;
			}

			// Release lock to enable new caller
			_lock.release();
		}
		else
		{
			// Send busy response
			error = createResponse(pdu,
				header->getPrimitiveId(),
				ACS_CS_Protocol::MODD_Result_Busy,
				BusyDescription);
		}
	}

	return error;
}


int ACS_CS_MODDHandler::handleChange(ACS_CS_PDU *pdu)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::handleChange()\n"
		"Entering method\n"));

	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleChange()\n"
			"Error: Invalid PDU\n"));

		return -1;
	}

	// Get header and body from PDU
	const ACS_CS_MODD_Header * header = 0;

	if (pdu && pdu->getHeader())
	   header = dynamic_cast<const ACS_CS_MODD_Header *>(pdu->getHeader()); // Get header from PDU

	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_MODD_Change * change = 0;

	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleChange()\n"
			"Error: Invalid header or primitive, header = %d, primitive = %d\n",
			header,
			primitive));

		return -1;
	}

	// Check primitive type and cast to correct type
	if (primitive->getPrimitiveType() == Primitive_MODD_Change)
	{
		change = dynamic_cast<const ACS_CS_MODD_Change *> (primitive);

		if (change == 0)
		{
			ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
				"(%t) ACS_CS_MODDHandler::handleChange()\n"
				"Error: Cannot cast primitive\n"));

			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleChange()\n"
			"Error: Wrong primitive type, type = %d\n",
			primitive->getPrimitiveType()));

		return -1;
	}

	int error = 0;
	ACS_CS_Protocol::MODD_Result_Code resultCode = ACS_CS_Protocol::MODD_Result_Success;
	string description = SuccessDescription;

	// Get values from primitive
	char mac[6] = {0};
	error = change->getMacAddress(mac, 6);

	// The path provided from MODD request is not complete we need to add /boot prefix to it
	string bootImage = "/boot" + change->getBootImage();
    
    std::string macString = ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*) mac, 6);

	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
				"(%t) ACS_CS_MODDHandler::handleChange()\n"
				"Primitive MAC Address: %s, Boot Image: %s\n",
				macString.c_str(), bootImage.c_str()));
	

	if (error == 0)
	{
		// Get matching entry
		const Default_DHCP_Entry * dhcpEntry = getDhcpEntry(mac, 6);

		//APZ21233
		if (dhcpEntry) // Entry exists
		{

			ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
						"(%t) ACS_CS_MODDHandler::handleChange()\n"
						"DHCP Entry Found. Host Name: %s, Boot Image: %s\n",
						dhcpEntry->hostName.c_str(), dhcpEntry->bootImage.c_str()));

			// Set timeout if requested
			uint16_t timeout = change->getTimeout();

			if (timeout > 0)
			{
				// If timeout is requested, create one
				createRevertTimer(dhcpEntry, timeout);
			}
			else
			{
				// Otherwise cancel any existing timeout for this entry
				cancelRevertTimer(dhcpEntry);
			}

			// Create DHCP info
			uint32_t bootIp = dhcpEntry->network == ACS_CS_Protocol::Eth_A ? bootIpEthA : bootIpEthB;
			ACS_CS_DHCP_Info info;
			getDhcpInfo(&info, dhcpEntry, bootIp);
			info.setBootPath(bootImage);

			// and update DHCP server
			ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);
			bool result = dhcp->connect();

			if (result)
				result = (apzType == APZ21250)? dhcp->removeNormalDhcpBoard(&info): dhcp->removeClientDhcpBoard(&info);

			if (result)
				result = (apzType == APZ21250)? dhcp->addNormalAPZ21250DhcpBoard(&info): dhcp->addClientAPZ2123XDhcpBoard(&info);

			dhcp->disconnect();

			// Set result code in case of failure
			if (result)
			{
				description = SuccessDescription;
				resultCode = ACS_CS_Protocol::MODD_Result_Success;

				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
						"(%t) ACS_CS_MODDHandler::handleChange()\n"
						"DHCP Entry Changed. Host Name: %s, Boot Path: %s\n"
						"Returning MODD Success",
						info.getHostName().c_str(), info.getBootPath().c_str()));
			}
			else
			{
				description = FailureDescription;
				resultCode = ACS_CS_Protocol::MODD_Result_Other_Failure;


				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
						"(%t) ACS_CS_MODDHandler::handleChange()\n"
						"DHCP Entry Change Failed. Host Name: %s, Boot Path: %s\n"
						"%s\n"
						"Returning MODD Failure!\n",
						info.getHostName().c_str(), info.getBootPath().c_str(),
						description.c_str()));
			}
		}

		//APZ21250
//		else if (macFound)
//		{
//			// Set timeout if requested
//			uint16_t timeout = change->getTimeout();
//
//			Default_DHCP_Entry entry;
//			entry.hostName = info.getHostName();
//
//			if (timeout > 0)
//			{
//				// If timeout is requested, create one
//				createRevertTimer(entry, timeout);
//			}
//			else
//			{
//				// Otherwise cancel any existing timeout for this entry
//				cancelRevertTimer(entry);
//			}
//
//			// Update DHCP info
//			info.setBootPath(bootImage);
//
//			// and update DHCP server
//			ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);
//			bool result = dhcp->connect();
//
//			if (result)
//				result = dhcp->removeClientDhcpBoard(&info);
//
//			if (result)
//				result = dhcp->addNormalAPZ21250DhcpBoard(&info);
//
//			dhcp->disconnect();
//
//			// Set result code in case of failure
//			if (result)
//			{
//				description = SuccessDescription;
//				resultCode = ACS_CS_Protocol::MODD_Result_Success;
//
//				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
//						"(%t) ACS_CS_MODDHandler::handleChange()\n"
//						"DHCP Entry Changed. Host Name: %s, Boot Path: %s\n"
//						"Returning MODD Success",
//						info.getHostName().c_str(), info.getBootPath().c_str()));
//			}
//			else
//			{
//				description = FailureDescription;
//				resultCode = ACS_CS_Protocol::MODD_Result_Other_Failure;
//
//
//				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
//						"(%t) ACS_CS_MODDHandler::handleChange()\n"
//						"DHCP Entry Change Failed. Host Name: %s, Boot Path: %s\n"
//						"%s\n"
//						"Returning MODD Failure!\n",
//						info.getHostName().c_str(), info.getBootPath().c_str(),
//						description.c_str()));
//			}
//		}
		else // No entry with this MAC
		{
			description = NoSuchEntryDescription;
			resultCode = ACS_CS_Protocol::MODD_Result_No_Such_Entry;

			ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
					"(%t) ACS_CS_MODDHandler::handleChange()\n"
					"DHCP Entry NOT Found.\n"
					"%s\n"
					"Returning MODD No Such Entry",
					description.c_str()));
		}
	}
	else // Error retrieving MAC
	{
		description = FailureDescription;
		resultCode = ACS_CS_Protocol::MODD_Result_Other_Failure;

		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
				"(%t) ACS_CS_MODDHandler::handleChange()\n"
				"Error Retrieving MAC.\n"
				"%s\n"
				"Returning MODD Other Failure",
				description.c_str()));
	}

	// Create response
	error = createResponse(pdu, ACS_CS_Protocol::Primitive_MODD_ChangeResponse, resultCode, description);

	return error;
}


int ACS_CS_MODDHandler::handleRevert(ACS_CS_PDU *pdu)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::handleRevert()\n"
		"Entering method\n"));

	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleRevert()\n"
			"Error: Invalid PDU\n"));

		return -1;
	}

	// Get header and body from PDU
	const ACS_CS_MODD_Header * header = 0;

	if (pdu && pdu->getHeader())
	   header = dynamic_cast<const ACS_CS_MODD_Header *>(pdu->getHeader()); // Get header from PDU

	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_MODD_Revert * revert = 0;

	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleRevert()\n"
			"Error: Invalid header or primitive, header = %d, primitive = %d\n",
			header,
			primitive));

		return -1;
	}

	// Check primitive type and cast to correct type
	if (primitive->getPrimitiveType() == Primitive_MODD_Revert)
	{
		revert = dynamic_cast<const ACS_CS_MODD_Revert *> (primitive);

		if (revert == 0)
		{
			ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
				"(%t) ACS_CS_MODDHandler::handleRevert()\n"
				"Error: Cannot cast primitive\n"));

			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleRevert()\n"
			"Error: Wrong primitive type, type = %d\n",
			primitive->getPrimitiveType()));

		return -1;
	}

	int error = 0;
	ACS_CS_Protocol::MODD_Result_Code resultCode = ACS_CS_Protocol::MODD_Result_Success;
	string description = SuccessDescription;

	// Get values from primitive
	char mac[6] = {0};
	revert->getMacAddress(mac, 6);

    std::string macString = ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*) mac, 6);
    
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::handleRevert()\n"
			"Primitive MAC Address: %s\n",
			macString.c_str()));


	if (error == 0)
	{
		// Get matching entry
		const Default_DHCP_Entry * dhcpEntry = getDhcpEntry(mac, 6);

		if (dhcpEntry) // Entry exists
		{

			ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
					"(%t) ACS_CS_MODDHandler::handleRevert()\n"
					"DHCP Entry Found. Host Name: %s, Boot Image: %s\n",
					dhcpEntry->hostName.c_str(), dhcpEntry->bootImage.c_str()));


			// Cancel any existing timer
			cancelRevertTimer(dhcpEntry);

			// Create DHCP info
			uint32_t bootIp = dhcpEntry->network == ACS_CS_Protocol::Eth_A ? bootIpEthA : bootIpEthB;
			ACS_CS_DHCP_Info info;
			getDhcpInfo(&info, dhcpEntry, bootIp);

			// and update DHCP server
			ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);
			bool result = dhcp->connect();

			if (result)
				result = (apzType == APZ21250)? dhcp->removeNormalDhcpBoard(&info): dhcp->removeClientDhcpBoard(&info);

			if (result)
				result = (apzType == APZ21250)? dhcp->addNormalAPZ21250DhcpBoard(&info): dhcp->addClientAPZ2123XDhcpBoard(&info);

			dhcp->disconnect();

			// Set result code in case of failure
			if (result)
			{
				description = SuccessDescription;
				resultCode = ACS_CS_Protocol::MODD_Result_Success;

				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
						"(%t) ACS_CS_MODDHandler::handleRevert()\n"
						"DHCP Entry Changed. Host Name: %s, Boot Path: %s\n"
						"Returning MODD Success",
						info.getHostName().c_str(), info.getBootPath().c_str()));
			}
			else
			{
				description = FailureDescription;
				resultCode = ACS_CS_Protocol::MODD_Result_Other_Failure;

				ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
						"(%t) ACS_CS_MODDHandler::handleRevert()\n"
						"DHCP Entry Change Failed. Host Name: %s, Boot Path: %s\n"
						"%s\n"
						"Returning MODD Failure!\n",
						info.getHostName().c_str(), info.getBootPath().c_str(),
						description.c_str()));
			}
		}
		else // No entry with this MAC
		{
			description = NoSuchEntryDescription;
			resultCode = ACS_CS_Protocol::MODD_Result_No_Such_Entry;

			ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
					"(%t) ACS_CS_MODDHandler::handleRevert()\n"
					"DHCP Entry NOT Found.\n"
					"%s\n"
					"Returning MODD No Such Entry",
					description.c_str()));
		}
	}
	else // Error retrieving MAC
	{
		description = FailureDescription;
		resultCode = ACS_CS_Protocol::MODD_Result_Other_Failure;

		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
				"(%t) ACS_CS_MODDHandler::handleRevert()\n"
				"Error Retrieving MAC.\n"
				"%s\n"
				"Returning MODD Other Failure",
				description.c_str()));
	}

	// Create response
	error = createResponse(pdu, ACS_CS_Protocol::Primitive_MODD_RevertResponse, resultCode, description);

	return error;
}


int ACS_CS_MODDHandler::createResponse (ACS_CS_PDU *pdu,
		ACS_CS_Protocol::CS_Primitive_Identifier type,
		ACS_CS_Protocol::MODD_Result_Code result,
		std::string description)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::createResponse()\n"
		"Entering method\n"));

	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::createResponse()\n"
			"Error: Invalid PDU\n"));

		return -1;
	}

	ACS_CS_MODD_Header * header = new ACS_CS_MODD_Header();   // Create header

	if (header == 0)
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::createResponse()\n"
			"Error: Invalid header\n"));

		return -1;
	}

	const ACS_CS_MODD_Header * oldHeader = 0;

	if (pdu && pdu->getHeader())
		oldHeader = dynamic_cast<const ACS_CS_MODD_Header *>(pdu->getHeader()); // Get header from PDU

	CS_Version_Identifier version = Version_1;


	// Get version from incoming package
	if (oldHeader)
	{
		version = oldHeader->getVersion();
	}

	ACS_CS_Primitive * body = 0;

	// Create body
	if (type == ACS_CS_Protocol::Primitive_MODD_ChangeResponse)
	{
		ACS_CS_MODD_ChangeResponse * response = new ACS_CS_MODD_ChangeResponse();
		response->setResultCode(result);
		response->setDescription(description);
		body = response;
	}
	else if (type == ACS_CS_Protocol::Primitive_MODD_RevertResponse)
	{
		ACS_CS_MODD_RevertResponse * response = new ACS_CS_MODD_RevertResponse();
		response->setResultCode(result);
		response->setDescription(description);
		body = response;
	}


	if (body == 0)
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::createResponse()\n"
			"Error: Invalid body\n"));

		delete header;
		return -1;
	}

	// Set all header values
	header->setVersion(version);
	header->setPrimitive(type);

	// Set length
	//header->setTotalLength(header->getHeaderLength() + body->getLength());
	header->setTotalLength(body->getLength());

	// Update PDU
	pdu->setHeader(header);
	pdu->setPrimitive(body);

	return 0;
}


void ACS_CS_MODDHandler::addAPZ2123xEntry()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::addAPZ2123xEntry()"));

	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();

	addRemoveHwcEntry(DEFAULT_HWC_CPUBA, tempModel, true);
	addRemoveHwcEntry(DEFAULT_HWC_CPUBB, tempModel, true);
	addRemoveHwcEntry(DEFAULT_HWC_MAUBA, tempModel, true);

	ACS_CS_ImModelSaver saver(tempModel);
	saver.save("APZ2123xEntries");

	delete tempModel;
}

void ACS_CS_MODDHandler::addHwcClassicEntries()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::addHwcClassicEntries()"));

	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();

	addRemoveHwcEntry(DEFAULT_HWC_CPUBA, tempModel, true);
	addRemoveHwcEntry(DEFAULT_HWC_CPUBB, tempModel, true);
	addRemoveHwcEntry(DEFAULT_HWC_MAUBA, tempModel, true);

	ACS_CS_ImModelSaver saver(tempModel);
	saver.save("ClassicEntries");

	delete tempModel;
}

void ACS_CS_MODDHandler::clearAPZ2123xEntry()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::clearAPZ2123xEntry()"));

	ACS_CS_DHCP_Info ipn0;
	ACS_CS_DHCP_Info ipn1;
	ACS_CS_DHCP_Info ipn2;
	ACS_CS_DHCP_Info ipn3;

	getDhcpInfo(&ipn0, &DEFAULT_DHCP_IPN0, bootIpEthA);
	getDhcpInfo(&ipn1, &DEFAULT_DHCP_IPN1, bootIpEthB);
	getDhcpInfo(&ipn2, &DEFAULT_DHCP_IPN2, bootIpEthA);
	getDhcpInfo(&ipn3, &DEFAULT_DHCP_IPN3, bootIpEthB);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Remove entries from HWC table
//	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();
//
//	addRemoveHwcEntry(DEFAULT_HWC_CPUBA, tempModel, false);
//	addRemoveHwcEntry(DEFAULT_HWC_CPUBB, tempModel, false);
//	addRemoveHwcEntry(DEFAULT_HWC_MAUBA, tempModel, false);
//
//	ACS_CS_ImModelSaver saver(tempModel);
//	saver.save("ClearAPZ2123xEntries");
//
//	delete tempModel;


	// Remove leases in DHCP server
	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN0.uid.c_str(), DEFAULT_DHCP_IPN0.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn0);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN1.uid.c_str(), DEFAULT_DHCP_IPN1.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn1);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN2.uid.c_str(), DEFAULT_DHCP_IPN2.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn2);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN3.uid.c_str(), DEFAULT_DHCP_IPN3.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn3);

}



bool ACS_CS_MODDHandler::addRemoveHwcEntry(const Default_HWC_Entry &entry, ACS_CS_ImModel* tempModel, bool add)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::addRemoveHwcEntry()"));

	const ACS_CS_ImModel *readModel = ACS_CS_ImRepository::instance()->getModelCopy();


	string shelfAddress = "";
	string shelfRdn = "";
	string bladeRdn = "";

	char strShelf[INET_ADDRSTRLEN];
	//find/create shelf
	uint32_t address = entry.magazine;
	address = ntohl(address);
	inet_ntop(AF_INET, &(address), strShelf, INET_ADDRSTRLEN);

	shelfAddress = strShelf;

	shelfRdn = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	stringstream ss;
	ss << entry.slot;

	bladeRdn = ACS_CS_ImmMapper::ATTR_CP_BLADE_ID + "=" + ss.str() + "," + shelfRdn;


	ACS_CS_ImCpBlade *cpBlade = new ACS_CS_ImCpBlade;

	cpBlade->functionalBoardName = static_cast<FunctionalBoardNameEnum>(entry.fbn);
	cpBlade->side = static_cast<SideEnum>(entry.side);
	cpBlade->slotNumber = entry.slot;
	cpBlade->systemNumber = entry.sysNum;
	cpBlade->systemType = static_cast<SystemTypeEnum>(entry.sysType);

	cpBlade->ipAliasEthA = "0.0.0.0";
	cpBlade->ipAliasEthB = "0.0.0.0";
	cpBlade->aliasNetmaskEthA = "0.0.0.0";
	cpBlade->aliasNetmaskEthB = "0.0.0.0";

	cpBlade->sequenceNumber = -1;
	cpBlade->dhcpOption = DHCP_NONE;
	cpBlade->type = CPBLADE_T;
	cpBlade->rdn = bladeRdn;
	ACS_CS_ImEntryIdHelper::calculateEntryIdNumber(readModel, CPBLADE_T, cpBlade->entryId);


	cpBlade->action = ACS_CS_ImBase::CREATE;
	// Get IP addresses
	char str[INET_ADDRSTRLEN];
	uint32_t ipaddrA = 0;

	if (entry.fbn == ACS_CS_Protocol::FBN_CPUB)
	{
		ipaddrA = CS_ProtocolChecker::getCPaddress(entry.side, ACS_CS_Protocol::Eth_A);
	}
	else if (entry.fbn == ACS_CS_Protocol::FBN_MAUB)
	{
		ipaddrA = CS_ProtocolChecker::getMAUaddress(ACS_CS_Protocol::Eth_A);
	}

	ipaddrA = ntohl(ipaddrA);
	inet_ntop(AF_INET, &(ipaddrA), str, INET_ADDRSTRLEN);

	cpBlade->ipAddressEthA = str;


	uint32_t ipaddrB = 0;

	if (entry.fbn == ACS_CS_Protocol::FBN_CPUB)
	{
		ipaddrB = CS_ProtocolChecker::getCPaddress(entry.side, ACS_CS_Protocol::Eth_B);
	}
	else if (entry.fbn == ACS_CS_Protocol::FBN_MAUB)
	{
		ipaddrB = CS_ProtocolChecker::getMAUaddress(ACS_CS_Protocol::Eth_B);
	}

	ipaddrB = ntohl(ipaddrB);
	inet_ntop(AF_INET, &(ipaddrB), str, INET_ADDRSTRLEN);

	cpBlade->ipAddressEthB = str;


	//Do we already have that blade?
	ACS_CS_ImBase *baseCpBlade = 0;
	ACS_CS_ImCpBlade *theBlade = 0;

	baseCpBlade = tempModel->getObject(bladeRdn.c_str());
	if (baseCpBlade)
		theBlade = dynamic_cast<ACS_CS_ImCpBlade*>(baseCpBlade);
	else {
		baseCpBlade = readModel->getObject(bladeRdn.c_str());

		if (baseCpBlade)
			theBlade = dynamic_cast<ACS_CS_ImCpBlade*>(baseCpBlade);
	}


	if(false == add)// remove the blade
	{
		if(NULL == theBlade) // we are supposed to remove the blade but it does not exist
		{
			delete cpBlade;
			delete readModel;
			return true;
		}

		cpBlade->action = ACS_CS_ImBase::DELETE;
		cpBlade->rdn = theBlade->rdn;
		tempModel->addObject(cpBlade);
	} else
	{// add the blade
		if(NULL != theBlade) // the blade already exists, we do not need to add it but it may need to be modified
		{

			if (cpBlade->side != theBlade->side || cpBlade->systemNumber != theBlade->systemNumber
					|| cpBlade->systemType != theBlade->systemType || cpBlade->functionalBoardName != theBlade->functionalBoardName)

				cpBlade->action = ACS_CS_ImBase::MODIFY;
			else {
				delete cpBlade;
				delete readModel;
				return true;
			}
		}

		ACS_CS_ImBase* baseShelf = 0;
		ACS_CS_ImShelf *theShelf = 0;

		baseShelf = tempModel->getObject(shelfRdn.c_str());
		if (baseShelf)
			theShelf = dynamic_cast<ACS_CS_ImShelf*>(baseShelf);
		else {
			baseShelf = readModel->getObject(shelfRdn.c_str());

			if (baseShelf)
				theShelf = dynamic_cast<ACS_CS_ImShelf*>(baseShelf);
		}


		if(NULL == theShelf)
		{
			theShelf = new ACS_CS_ImShelf();
			theShelf->type = SHELF_T;
			theShelf->action = ACS_CS_ImBase::CREATE;
			theShelf->address = shelfAddress;
			theShelf->axeShelfId = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress;
			theShelf->rdn = shelfRdn;

			//ADD THE SHELF HERE!
			tempModel->addObject(theShelf);
		}

		//cpBlade->action = ACS_CS_ImBase::CREATE;
		cpBlade->axeCpBladeId = ACS_CS_ImmMapper::ATTR_CP_BLADE_ID + "=" + ss.str();
		//cpBlade->rdn = bladeRdn;


		tempModel->addObject(cpBlade);

	}

	delete readModel;
	return true;
}


void ACS_CS_MODDHandler::updateDhcpAPZ2123X()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::updateDhcpAPZ2123X()"));

	ACS_CS_DHCP_Info ipn0;
	ACS_CS_DHCP_Info ipn1;
	ACS_CS_DHCP_Info ipn2;
	ACS_CS_DHCP_Info ipn3;

	getDhcpInfo(&ipn0, &DEFAULT_DHCP_IPN0, bootIpEthA);
	getDhcpInfo(&ipn1, &DEFAULT_DHCP_IPN1, bootIpEthB);
	getDhcpInfo(&ipn2, &DEFAULT_DHCP_IPN2, bootIpEthA);
	getDhcpInfo(&ipn3, &DEFAULT_DHCP_IPN3, bootIpEthB);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN0.uid.c_str(), DEFAULT_DHCP_IPN0.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn0);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN1.uid.c_str(), DEFAULT_DHCP_IPN1.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn1);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN2.uid.c_str(), DEFAULT_DHCP_IPN2.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn2);

	if (dhcp->existsClientHost(DEFAULT_DHCP_IPN3.uid.c_str(), DEFAULT_DHCP_IPN3.uid.length()))
		dhcp->removeClientDhcpBoard(&ipn3);


	dhcp->addClientAPZ2123XDhcpBoard(&ipn0);
	dhcp->addClientAPZ2123XDhcpBoard(&ipn1);
	dhcp->addClientAPZ2123XDhcpBoard(&ipn2);
	dhcp->addClientAPZ2123XDhcpBoard(&ipn3);
}



bool ACS_CS_MODDHandler::getDhcpInfo(ACS_CS_DHCP_Info * info,
		const Default_DHCP_Entry * entry,
		uint32_t bootIp) const
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::getDhcpInfo()"));

	if ( !info || !entry)
		return false;

	info->setBootPath(entry->bootImage);
	info->setComment(entry->description);
	info->setHostName(entry->hostName);
	info->setIP(entry->ipAddress);
	info->setMAC(entry->mac, 6); //leave it for protocol purposes
	//info->setClientId(entry->mac, 6);
	info->setClientId(entry->uid.c_str(), entry->uid.length());
	// Convert boot IP to C-string
	char addr[INET_ADDRSTRLEN];
	uint32_t naddr = htonl(bootIp);
	inet_ntop(AF_INET, &naddr, addr, sizeof(addr));

	info->setBootServer(addr);
	info->setAcAddress(bootIp);

	return true;
}


const Default_DHCP_Entry * ACS_CS_MODDHandler::getDhcpEntry(const char * mac, int size) const
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::getDhcpEntry()"));

	if (!mac || (size != 6))
		return false;
	//APZ21233 Entries
	if (memcmp(DEFAULT_DHCP_IPN0.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_IPN0;
	else if (memcmp(DEFAULT_DHCP_IPN1.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_IPN1;
	else if (memcmp(DEFAULT_DHCP_IPN2.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_IPN2;
	else if (memcmp(DEFAULT_DHCP_IPN3.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_IPN3;

	//APZ21250 Entries
	else if (memcmp(DEFAULT_DHCP_PCIH_A_Own.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_PCIH_A_Own;
	else if (memcmp(DEFAULT_DHCP_PCIH_B_Own.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_PCIH_B_Own;
	else if (memcmp(DEFAULT_DHCP_PCIH_A_Twin.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_PCIH_A_Twin;
	else if (memcmp(DEFAULT_DHCP_PCIH_B_Twin.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_PCIH_B_Twin;
	else if (memcmp(DEFAULT_DHCP_CPSB_A_Eth0.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_CPSB_A_Eth0;
	else if (memcmp(DEFAULT_DHCP_CPSB_A_Eth1.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_CPSB_A_Eth1;
	else if (memcmp(DEFAULT_DHCP_CPSB_B_Eth0.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_CPSB_B_Eth0;
	else if (memcmp(DEFAULT_DHCP_CPSB_B_Eth1.mac, mac, 6) == 0)
		return &DEFAULT_DHCP_CPSB_B_Eth1;

	else
		return 0;
}



void ACS_CS_MODDHandler::performAutomaticRevert(const Default_DHCP_Entry &entry)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::performAutomaticRevert()"));

	// Create DHCP info
	uint32_t bootIp = entry.network == ACS_CS_Protocol::Eth_A ? bootIpEthA : bootIpEthB;
	ACS_CS_DHCP_Info info;
	getDhcpInfo(&info, &entry, bootIp);

	// and update DHCP server
	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);
	bool result = dhcp->connect();

	if (result)
	{
		result = (apzType == APZ21250)? dhcp->removeNormalDhcpBoard(&info): dhcp->removeClientDhcpBoard(&info);
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::performAutomaticRevert()\n"
			"Error: Failed to connect to DHCP server"));
	}

	if (result)
	{
		result = (apzType == APZ21250)? dhcp->addNormalAPZ21250DhcpBoard(&info): dhcp->addClientAPZ2123XDhcpBoard(&info);
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::performAutomaticRevert()\n"
			"Error: Failed to remove board from DHCP server"));
	}

	if (!result)
	{
		ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::performAutomaticRevert()\n"
			"Error: Failed to add board to DHCP server"));
	}

	(void) dhcp->disconnect();
}


void ACS_CS_MODDHandler::createRevertTimer(const Default_DHCP_Entry * entry, uint16_t timeout)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::createRevertTimer()"));

	if ( !entry || !timeout )
		return;

	// See if there is a timer already for this entry
	maptype::iterator it = timerMap.find(entry->hostName);

	if (it != timerMap.end())
	{
		// get old timer
		ACS_CS_RevertTimer * timer = it->second;

		if (timer)
		{
			// Cancel it and save the pointer for later removal
			timer->deActivate();
			timer->cancelRevertTimer();

			if (timer->isFinished())
			{
				delete timer;
			}
			else
			{
				oldTimers.push_back(timer);
			}

			timerMap.erase(it);
		}
	}

	// Create new timer and save the pointer
	ACS_CS_RevertTimer * timer = new ACS_CS_RevertTimer(this, entry, timeout);
	timer->activate();
	timerMap.insert( maptype::value_type(entry->hostName, timer) );
}


void ACS_CS_MODDHandler::cancelRevertTimer(const Default_DHCP_Entry * entry)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::cancelRevertTimer()"));

	if (!entry)
		return;

	// See if there is a timer already for this entry
	maptype::iterator it = timerMap.find(entry->hostName);

	if (it != timerMap.end())
	{
		// Get timer
		ACS_CS_RevertTimer * timer = it->second;

		if (timer)
		{
			// Cancel it and save the pointer for later removal
			timer->deActivate();
			timer->cancelRevertTimer();

			if (timer->isFinished())
			{
				delete timer;
			}
			else
			{
				oldTimers.push_back(timer);
			}

			timerMap.erase(it);
		}
	}
}


void ACS_CS_MODDHandler::timerCleanup()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"ACS_CS_MODDHandler::timerCleanup()"));

	for (size_t i = oldTimers.size(); i > 0; i--)
	{
		if (oldTimers[i - 1]->isFinished())
		{
			delete oldTimers[i - 1];
			oldTimers.erase(oldTimers.begin() + i - 1);
		}
	}
}

void ACS_CS_MODDHandler::updateDhcpAPZ21250()
{

	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::updateDhcpAPZ21250()"));

	MACMap emptyMap;

	updateDhcpAPZ21250(emptyMap, ACS_CS_PTBPeerHandler::PTBA);
	updateDhcpAPZ21250(emptyMap, ACS_CS_PTBPeerHandler::PTBB);
}

void ACS_CS_MODDHandler::updateDhcpAPZ21250(const MACMap &macMap, ACS_CS_PTBPeerHandler::PTBPeer side)
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::updateDhcpAPZ21250(const MACMap &, ACS_CS_PTBPeerHandler::PTBPeer)"));

	ACS_CS_DHCP_Info pcih_own;
	ACS_CS_DHCP_Info pcih_twin;
	ACS_CS_DHCP_Info cpsb_eth0;
	ACS_CS_DHCP_Info cpsb_eth1;

	cout << "Entering Update DHCP for APZ21250 method" << std::endl;

	if(ACS_CS_PTBPeerHandler::PTBA == side)
	{
        MACMap::const_iterator it;
        
        char realMac[6] = {0};
        
        for(it = macMap.begin(); it != macMap.end(); ++it)
        {
            getRealMac(it->second.c_str(),realMac);
            
            if(it->first == CPTREADMAUREG::PCIH_TWIN)
            {
                DEFAULT_DHCP_PCIH_A_Twin.setMAC(realMac);
            }
            else if(it->first == CPTREADMAUREG::PCIH_OWN)
            {
                DEFAULT_DHCP_PCIH_A_Own.setMAC(realMac);
            }
            else if(it->first == CPTREADMAUREG::CPSB_ETH0)
            {
                DEFAULT_DHCP_CPSB_A_Eth0.setMAC(realMac);
            }
            else if(it->first == CPTREADMAUREG::CPSB_ETH1)
            {
                DEFAULT_DHCP_CPSB_A_Eth1.setMAC(realMac);
            }
        }

		getDhcpInfo(&pcih_own, &DEFAULT_DHCP_PCIH_A_Own, bootIpEthA);
		getDhcpInfo(&pcih_twin, &DEFAULT_DHCP_PCIH_A_Twin, bootIpEthB);
		getDhcpInfo(&cpsb_eth0, &DEFAULT_DHCP_CPSB_A_Eth0, bootIpEthA);
		getDhcpInfo(&cpsb_eth1, &DEFAULT_DHCP_CPSB_A_Eth1, bootIpEthB);
	}
    else
	{
        MACMap::const_iterator it;
        
        char realMac[6] = {0};
        
        for(it = macMap.begin(); it != macMap.end(); ++it)
        {
            getRealMac(it->second.c_str(),realMac);
            
            if(it->first == CPTREADMAUREG::PCIH_TWIN)
            {
                DEFAULT_DHCP_PCIH_B_Twin.setMAC(realMac);
            }
            else if(it->first == CPTREADMAUREG::PCIH_OWN)
            {
                DEFAULT_DHCP_PCIH_B_Own.setMAC(realMac);
            }
            else if(it->first == CPTREADMAUREG::CPSB_ETH0)
            {
                DEFAULT_DHCP_CPSB_B_Eth0.setMAC(realMac);
            }
            else if(it->first == CPTREADMAUREG::CPSB_ETH1)
            {
                DEFAULT_DHCP_CPSB_B_Eth1.setMAC(realMac);
            }
        }

		getDhcpInfo(&pcih_own, &DEFAULT_DHCP_PCIH_B_Own, bootIpEthB);
		getDhcpInfo(&pcih_twin, &DEFAULT_DHCP_PCIH_B_Twin, bootIpEthA);
		getDhcpInfo(&cpsb_eth0, &DEFAULT_DHCP_CPSB_B_Eth0, bootIpEthB);
		getDhcpInfo(&cpsb_eth1, &DEFAULT_DHCP_CPSB_B_Eth1, bootIpEthA);
	}

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_A_Own.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_own);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_A_Twin.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_twin);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_B_Own.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_own);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_B_Twin.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_twin);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_A_Eth0.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_eth0);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_A_Eth1.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_eth1);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_B_Eth0.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_eth0);

	//if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_B_Eth1.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_eth1);


//	MACMap::const_iterator it;
//
//	char *realMac = new char[6];
//
//	for(it = macMap.begin(); it != macMap.end(); ++it)
//	{
//		memset(realMac,0,6);
//		getRealMac(it->second.c_str(),realMac);
//
//		if(it->first == CPTREADMAUREG::PCIH_TWIN)
//		{
//			pcih_twin.setMAC(realMac, 6);
//		}
//		else if(it->first == CPTREADMAUREG::PCIH_OWN)
//		{
//			pcih_own.setMAC(realMac,6);
//		}
//		else if(it->first == CPTREADMAUREG::CPSB_ETH0)
//		{
//			cpsb_eth0.setMAC(realMac, 6);
//		}
//		else if(it->first == CPTREADMAUREG::CPSB_ETH1)
//		{
//			cpsb_eth1.setMAC(realMac, 6);
//		}
//	}
//
//	delete []realMac;

	dhcp->addNormalAPZ21250DhcpBoard(&pcih_own);
	dhcp->addNormalAPZ21250DhcpBoard(&pcih_twin);
	dhcp->addNormalAPZ21250DhcpBoard(&cpsb_eth0);
	dhcp->addNormalAPZ21250DhcpBoard(&cpsb_eth1);

	cout << "Exiting Update DHCP for APZ21250 method" << std::endl;

}



void ACS_CS_MODDHandler::updateDhcpAPZ21240()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
			"(%t) ACS_CS_MODDHandler::updateDhcpAPZ21240()"));

	ACS_CS_DHCP_Info eib_a;
	ACS_CS_DHCP_Info eic_a;
	ACS_CS_DHCP_Info eib_b;
	ACS_CS_DHCP_Info eic_b;

	getDhcpInfo(&eib_a, &DEFAULT_DHCP_EIB_A, bootIpEthA);
	getDhcpInfo(&eic_a, &DEFAULT_DHCP_EIC_A, bootIpEthB);
	getDhcpInfo(&eib_b, &DEFAULT_DHCP_EIB_B, bootIpEthB);
	getDhcpInfo(&eic_b, &DEFAULT_DHCP_EIC_B, bootIpEthA);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_EIB_A.mac, 6))
		dhcp->removeNormalDhcpBoard(&eib_a);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_EIC_A.mac, 6))
		dhcp->removeNormalDhcpBoard(&eic_a);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_EIB_B.mac, 6))
		dhcp->removeNormalDhcpBoard(&eib_b);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_EIC_B.mac, 6))
		dhcp->removeNormalDhcpBoard(&eic_b);

	dhcp->addNormalDhcpBoard(&eib_a);
	dhcp->addNormalDhcpBoard(&eic_a);
	dhcp->addNormalDhcpBoard(&eib_b);
	dhcp->addNormalDhcpBoard(&eic_b);

}

void ACS_CS_MODDHandler::addStaticGESBEntries()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::addStaticGESBEntries()"));

	ACS_CS_DHCP_Info gesb_i_a_eth0;
	ACS_CS_DHCP_Info gesb_e_a_eth0;
	ACS_CS_DHCP_Info gesb_e_b_eth0;
	ACS_CS_DHCP_Info gesb_i_b_eth0;

	ACS_CS_DHCP_Info gesb_i_a_eth1;
	ACS_CS_DHCP_Info gesb_e_a_eth1;
	ACS_CS_DHCP_Info gesb_e_b_eth1;
	ACS_CS_DHCP_Info gesb_i_b_eth1;

	getDhcpInfo(&gesb_i_a_eth0, &DEFAULT_DHCP_GESB_I_A_ETH0, bootIpEthA);
	getDhcpInfo(&gesb_e_a_eth0, &DEFAULT_DHCP_GESB_E_A_ETH0, bootIpEthA);
	getDhcpInfo(&gesb_e_b_eth0, &DEFAULT_DHCP_GESB_E_B_ETH0, bootIpEthA);
	getDhcpInfo(&gesb_i_b_eth0, &DEFAULT_DHCP_GESB_I_B_ETH0, bootIpEthA);

	getDhcpInfo(&gesb_i_a_eth1, &DEFAULT_DHCP_GESB_I_A_ETH1, bootIpEthB);
	getDhcpInfo(&gesb_e_a_eth1, &DEFAULT_DHCP_GESB_E_A_ETH1, bootIpEthB);
	getDhcpInfo(&gesb_e_b_eth1, &DEFAULT_DHCP_GESB_E_B_ETH1, bootIpEthB);
	getDhcpInfo(&gesb_i_b_eth1, &DEFAULT_DHCP_GESB_I_B_ETH1, bootIpEthB);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_A_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_I_A_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_a_eth0);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_A_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_E_A_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_a_eth0);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_B_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_E_B_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_b_eth0);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_B_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_I_B_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_b_eth0);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_A_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_I_A_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_a_eth1);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_A_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_E_A_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_a_eth1);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_B_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_E_B_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_b_eth1);

	if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_B_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_I_B_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_b_eth1);


	char oldClientId[20] = {0};
	char *realClientId = new char[20];
	memset(realClientId,0,20);

	int size = 0;

	gesb_i_a_eth0.getClientId(oldClientId, gesb_i_a_eth0.getClientIdLength());
	size = gesb_i_a_eth0.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_i_a_eth0.setClientId(realClientId, size);

	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_e_a_eth0.getClientId(oldClientId, gesb_e_a_eth0.getClientIdLength());
	size = gesb_e_a_eth0.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_e_a_eth0.setClientId(realClientId, size);

	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_e_b_eth0.getClientId(oldClientId, gesb_e_b_eth0.getClientIdLength());
	size = gesb_e_b_eth0.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_e_b_eth0.setClientId(realClientId, size);

	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_i_b_eth0.getClientId(oldClientId, gesb_i_b_eth0.getClientIdLength());
	size = gesb_i_b_eth0.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_i_b_eth0.setClientId(realClientId, size);


	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_i_a_eth1.getClientId(oldClientId, gesb_i_a_eth1.getClientIdLength());
	size = gesb_i_a_eth1.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_i_a_eth1.setClientId(realClientId, size);

	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_e_a_eth1.getClientId(oldClientId, gesb_e_a_eth1.getClientIdLength());
	size = gesb_e_a_eth1.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_e_a_eth1.setClientId(realClientId, size);

	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_e_b_eth1.getClientId(oldClientId, gesb_e_b_eth1.getClientIdLength());
	size = gesb_e_b_eth1.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_e_b_eth1.setClientId(realClientId, size);

	memset(realClientId,0,20);
	memset(oldClientId,0,20);
	gesb_i_b_eth1.getClientId(oldClientId, gesb_i_b_eth1.getClientIdLength());
	size = gesb_i_b_eth1.getClientIdLength()/2;
	getRealMac(oldClientId,realClientId,size);
	gesb_i_b_eth1.setClientId(realClientId, size);


	dhcp->addClientDhcpBoard(&gesb_i_a_eth0);
	dhcp->addClientDhcpBoard(&gesb_e_a_eth0);
	dhcp->addClientDhcpBoard(&gesb_e_b_eth0);
	dhcp->addClientDhcpBoard(&gesb_i_b_eth0);

	dhcp->addClientDhcpBoard(&gesb_i_a_eth1);
	dhcp->addClientDhcpBoard(&gesb_e_a_eth1);
	dhcp->addClientDhcpBoard(&gesb_e_b_eth1);
	dhcp->addClientDhcpBoard(&gesb_i_b_eth1);

	delete[] realClientId;

}

void ACS_CS_MODDHandler::clearAPZ21240Entry()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::clearAPZ21240Entry()"));

	ACS_CS_DHCP_Info eib_a;
	ACS_CS_DHCP_Info eic_a;
	ACS_CS_DHCP_Info eib_b;
	ACS_CS_DHCP_Info eic_b;

	getDhcpInfo(&eib_a, &DEFAULT_DHCP_EIB_A, bootIpEthA);
	getDhcpInfo(&eic_a, &DEFAULT_DHCP_EIC_A, bootIpEthB);
	getDhcpInfo(&eib_b, &DEFAULT_DHCP_EIB_B, bootIpEthB);
	getDhcpInfo(&eic_b, &DEFAULT_DHCP_EIC_B, bootIpEthA);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Remove entries from HWC table
//	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();
//
//	addRemoveHwcEntry(DEFAULT_HWC_CPUBA, tempModel, false);
//	addRemoveHwcEntry(DEFAULT_HWC_CPUBB, tempModel, false);
//	addRemoveHwcEntry(DEFAULT_HWC_MAUBA, tempModel, false);
//
//	ACS_CS_ImModelSaver saver(tempModel);
//	saver.save("ClearAPZ21240Entries");
//
//	delete tempModel;

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_A_Own.mac, 6))
		dhcp->removeNormalDhcpBoard(&eib_a);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_A_Twin.mac, 6))
		dhcp->removeNormalDhcpBoard(&eic_a);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_B_Own.mac, 6))
		dhcp->removeNormalDhcpBoard(&eib_b);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_B_Twin.mac, 6))
		dhcp->removeNormalDhcpBoard(&eic_b);

}

void ACS_CS_MODDHandler::clearAPZ21250Entry()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::clearAPZ21250Entry()"));

	ACS_CS_DHCP_Info pcih_a_own;
	ACS_CS_DHCP_Info pcih_a_twin;
	ACS_CS_DHCP_Info pcih_b_own;
	ACS_CS_DHCP_Info pcih_b_twin;

	ACS_CS_DHCP_Info cpsb_a_eth0;
	ACS_CS_DHCP_Info cpsb_a_eth1;
	ACS_CS_DHCP_Info cpsb_b_eth0;
	ACS_CS_DHCP_Info cpsb_b_eth1;

	getDhcpInfo(&pcih_a_own, &DEFAULT_DHCP_PCIH_A_Own, bootIpEthA);
	getDhcpInfo(&pcih_a_twin, &DEFAULT_DHCP_PCIH_A_Twin, bootIpEthB);
	getDhcpInfo(&pcih_b_own, &DEFAULT_DHCP_PCIH_B_Own, bootIpEthA);
	getDhcpInfo(&pcih_b_twin, &DEFAULT_DHCP_PCIH_B_Twin, bootIpEthB);

	getDhcpInfo(&cpsb_a_eth0, &DEFAULT_DHCP_CPSB_A_Eth0, bootIpEthA);
	getDhcpInfo(&cpsb_a_eth1, &DEFAULT_DHCP_CPSB_A_Eth1, bootIpEthB);
	getDhcpInfo(&cpsb_b_eth0, &DEFAULT_DHCP_CPSB_B_Eth0, bootIpEthA);
	getDhcpInfo(&cpsb_b_eth1, &DEFAULT_DHCP_CPSB_B_Eth1, bootIpEthB);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Remove entries from HWC table
//	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();
//
//	addRemoveHwcEntry(DEFAULT_HWC_CPUBA, tempModel, false);
//	addRemoveHwcEntry(DEFAULT_HWC_CPUBB, tempModel, false);
//	addRemoveHwcEntry(DEFAULT_HWC_MAUBA, tempModel, false);
//
//	ACS_CS_ImModelSaver saver(tempModel);
//	saver.save("ClearAPZ21250Entries");

//	delete tempModel;

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_A_Own.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_a_own);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_A_Twin.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_a_twin);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_B_Own.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_b_own);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_PCIH_B_Twin.mac, 6))
		dhcp->removeNormalDhcpBoard(&pcih_b_twin);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_A_Eth0.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_a_eth0);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_A_Eth1.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_a_eth1);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_B_Eth0.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_b_eth0);

	if (dhcp->existsNormalHost(DEFAULT_DHCP_CPSB_B_Eth1.mac, 6))
		dhcp->removeNormalDhcpBoard(&cpsb_b_eth1);

}

void ACS_CS_MODDHandler::clearGESBEntries()
{
	ACS_CS_TRACE((ACS_CS_MODDHandler_TRACE,
		"(%t) ACS_CS_MODDHandler::clearGESBEntries()"));

	ACS_CS_DHCP_Info gesb_i_a_eth0;
	ACS_CS_DHCP_Info gesb_e_a_eth0;
	ACS_CS_DHCP_Info gesb_e_b_eth0;
	ACS_CS_DHCP_Info gesb_i_b_eth0;

	ACS_CS_DHCP_Info gesb_i_a_eth1;
	ACS_CS_DHCP_Info gesb_e_a_eth1;
	ACS_CS_DHCP_Info gesb_e_b_eth1;
	ACS_CS_DHCP_Info gesb_i_b_eth1;

	getDhcpInfo(&gesb_i_a_eth0, &DEFAULT_DHCP_GESB_I_A_ETH0, bootIpEthA);
	getDhcpInfo(&gesb_e_a_eth0, &DEFAULT_DHCP_GESB_E_A_ETH0, bootIpEthA);
	getDhcpInfo(&gesb_e_b_eth0, &DEFAULT_DHCP_GESB_E_B_ETH0, bootIpEthA);
	getDhcpInfo(&gesb_i_b_eth0, &DEFAULT_DHCP_GESB_I_B_ETH0, bootIpEthA);

	getDhcpInfo(&gesb_i_a_eth1, &DEFAULT_DHCP_GESB_I_A_ETH1, bootIpEthB);
	getDhcpInfo(&gesb_e_a_eth1, &DEFAULT_DHCP_GESB_E_A_ETH1, bootIpEthB);
	getDhcpInfo(&gesb_e_b_eth1, &DEFAULT_DHCP_GESB_E_B_ETH1, bootIpEthB);
	getDhcpInfo(&gesb_i_b_eth1, &DEFAULT_DHCP_GESB_I_B_ETH1, bootIpEthB);

	ACS_CS_DHCP_Configurator *dhcp = ACS_CS_DHCP_Factory::instance()->createDhcpConfigurator(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_A_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_I_A_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_a_eth0);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_A_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_E_A_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_a_eth0);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_B_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_E_B_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_b_eth0);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_B_ETH0.uid.c_str(), DEFAULT_DHCP_GESB_I_B_ETH0.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_b_eth0);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_A_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_I_A_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_a_eth1);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_A_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_E_A_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_a_eth1);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_E_B_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_E_B_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_e_b_eth1);

	//if (dhcp->existsClientHost(DEFAULT_DHCP_GESB_I_B_ETH1.uid.c_str(), DEFAULT_DHCP_GESB_I_B_ETH1.uid.length()))
		dhcp->removeClientDhcpBoard(&gesb_i_b_eth1);

}


int ACS_CS_MODDHandler::normalize(char r)
{

    switch(r)
    {
        case 'F':
        case 'f':
            return 15;
        case 'E':
        case 'e':
        	return 14;
        case 'D':
        case 'd':
        	return 13;
        case 'C':
        case 'c':
        	return 12;
        case 'B':
        case 'b':
        	return 11;
        case 'A':
        case 'a':
        	return 10;
        default:
            return r-48;
    }


}

void ACS_CS_MODDHandler::hexCharToBinChar(unsigned char *input,char *out)
{

    unsigned char *output=(unsigned char*)malloc(6);
    memset(output,0,6);

    size_t t=0;
    int count=0;
    unsigned int ris=0;
    int index=0;


    for(t=0;t<strlen((char*)input);t++)
    {

		if (count == 0) {
			ris += 16 * normalize(input[t]);
			count++;
		} else {
			ris += (unsigned int) normalize(input[t]);
			output[index++] = ris;
			count = 0;
			ris = 0;
        }
    }

   for(int t=0;t<index;t++)
   {
         out[t]=output[t];
   }

   free(output);

}

void ACS_CS_MODDHandler::hexCharToBinChar(unsigned char *input,char *out, int size)
{

    unsigned char *output=(unsigned char*)malloc(size);
    memset(output,0,size);

    size_t t=0;
    int count=0;
    unsigned int ris=0;
    int index=0;


    for(t=0;t<strlen((char*)input);t++)
    {

		if (count == 0) {
			ris += 16 * normalize(input[t]);
			count++;
		} else {
			ris += (unsigned int) normalize(input[t]);
			output[index++] = ris;
			//printf("ris=%d\n", ris);
			count = 0;
			ris = 0;
        }
    }

   for(int t=0;t<index;t++)
   {
         out[t]=output[t];
   }

   free(output);

}


int ACS_CS_MODDHandler::getRealMac(std::string ss,char *out)
 {

	char str[14] = { 0 };

	sprintf(str, "%s", const_cast<char*> (ss.c_str()));

	unsigned char a[14] = { 0 };

	for (int t = 0; t < 14; t++) {
		a[t] = (unsigned char) (str[t]);
	}

	//printf("%s\n", a);

	memset(out, 0, 6);

	hexCharToBinChar(a, out);

	return 0;
}


int ACS_CS_MODDHandler::getRealMac(std::string ss,char *out, int size)
 {

	char * str=new char[size*2];
	memset(str, 0, size*2);

	sprintf(str, "%s", const_cast<char*> (ss.c_str()));

	unsigned char *a = new unsigned char[size*2];
	memset(a, 0, size*2);

	for (int t = 0; t < size*2; t++) {
		a[t] = (unsigned char) (str[t]);
	}

	hexCharToBinChar(a, out, size);

	delete [] str;
	delete [] a;

	return 0;
}


void ACS_CS_MODDHandler::clearImmEntries() {
	// Remove entries from HWC table
	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();

	addRemoveHwcEntry(DEFAULT_HWC_CPUBA, tempModel, false);
	addRemoveHwcEntry(DEFAULT_HWC_CPUBB, tempModel, false);
	addRemoveHwcEntry(DEFAULT_HWC_MAUBA, tempModel, false);

	ACS_CS_ImModelSaver saver(tempModel);
	saver.save("ClearImmEntries");

	delete tempModel;

}
