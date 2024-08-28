//	Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_TCPClient.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_ServiceHandler.h"

#include <vector>
#include <sstream>

#include "ACS_CS_ServiceConnection.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_ServiceConnection_TRACE);

using std::vector;
using std::ostringstream;
using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;

const size_t BUFSIZE = 8192;


ACS_CS_ServiceConnection::ACS_CS_ServiceConnection (ACS_CS_TCPClient *client,
		ACS_CS_ServiceHandler *handler,
		ACS_CS_Protocol::CS_Protocol_Type protocolType)
      : serviceHandler(handler),
        shutdownEvent(0),
        exitThreadEvent(0),
        removeClosedConnectionsEvent(0),
        tcpClient(client),
        csParser(0),
        finished(false),
        protocolType(protocolType)
{
    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
            "(%t) ACS_CS_ServiceConnection::ACS_CS_ServiceConnection()\n"
            "Thread created\n"));

    // Open shutdown event used to signal that the whole service shall shutdown
    shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);

    if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
                "(%t) ACS_CS_ServiceConnection::ACS_CS_ServiceConnection()\n"
                "Error: Cannot open shutdown event\n"));

        ACS_CS_EVENT(Event_ProgrammingError, ACS_CS_EventReporter::Severity_Event,
                "Open Event",
                "Unable to open event",
                "");
    }

    exitThreadEvent = ACS_CS_Event::CreateEvent(true, false, (const char*) NULL);

    if (exitThreadEvent == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
                "(%t) ACS_CS_ServiceConnection::ACS_CS_ServiceConnection()\n"
                "Error: Cannot create shutdown thread event\n"));
    }

    removeClosedConnectionsEvent =
            ACS_CS_Event::OpenNamedEvent(EVENT_NAME_REMOVE_CLOSED_CONNECTIONS);

    if (removeClosedConnectionsEvent == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
                "(%t) ACS_CS_ServiceConnection::ACS_CS_ServiceConnection()\n"
                "Error: Could not open remove closed connections event\n"));
    }

    // Object to parse the TCP stream
    csParser = new ACS_CS_Parser(protocolType);

    // Get remote address
    remoteAddress = tcpClient->getFullAddressAsString();
}


ACS_CS_ServiceConnection::~ACS_CS_ServiceConnection()
{
    // Do not delete a Service Connection object without waiting for it first
    // The thread must finish before this connection object can be deleted

    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
            "(%t) ACS_CS_ServiceConnection::~ACS_CS_ServiceConnection()\n"
            "Thread destroyed\n"));

    if (tcpClient)
	{
		delete tcpClient;
	}

	if (csParser)
		delete csParser;

	ACS_CS_Event::CloseEvent(exitThreadEvent);
}


int ACS_CS_ServiceConnection::exec ()
{
    ACS_CS_TCPClient::LinkStatus_t status;

    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
            "(%t) ACS_CS_ServiceConnection::exec()\n"
            "Thread started\n"));

    if ((csParser == 0) || (tcpClient == 0))	// Check for valid objects
    {
        ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
                "(%t) ACS_CS_ServiceConnection::exec()\n"
                "Error: csParser or tcpClient is null, csParser = %d, tcpClient = %d\n",
                csParser,
                tcpClient));

        return 1;
    }

    { //trace
    	char msg[128] = {0};
    	snprintf(msg, sizeof(msg) - 1,
    			"[%s::%s@%d] Connected to remote address: %s",
    			__FILE__, __FUNCTION__, __LINE__, remoteAddress.c_str());
    	ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE, "(%t) %s", msg));
    	//std::cout << "DBG: " << msg << std::endl;
    }

    int error = 0;

	vector<ACS_CS_PDU *> pduVector;	// Vector to hold PDUs

	ACS_CS_EventHandle handleArr[3] = {shutdownEvent, exitThreadEvent, tcpClient->getHandle()};

	while (!exit_)	// Set to true when the thread shall exit
	{

		// Different timeouts for CS and MODD protocol
		int timeout = (protocolType == ACS_CS_Protocol::Protocol_MODD ?
				MODD_Connection_Timeout : Connection_Timeout);

	    // Wait for shutdown and read events
		int eventIndex = ACS_CS_Event::WaitForEvents(3, handleArr, timeout); // Get index for event

		if (eventIndex == ACS_CS_EVENT_RC_ERROR)		// Event failed
		{
		    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
		            "(%t) ACS_CS_ServiceConnection::exec()\n"
		            "Error: WaitForEvents\n"));

			exit_ = true;
		}
		else if (eventIndex == ACS_CS_EVENT_RC_TIMEOUT)	// Connection timeout
		{
			ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
			        "(%t) ACS_CS_ServiceConnection::exec()\n"
			        "Error: Connection timeout. Disconnecting inactive client.\n"));

			exit_ = true;
		}
		else if (eventIndex == 0)			// Shutdown event
		{
			ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
			        "(%t) ACS_CS_ServiceConnection::exec()\n"
			        "Shutdown event signaled. Disconnecting client.\n"));

			exit_ = true;
		}
		else if (eventIndex == 1)           // Exit thread event
		{
		    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
		            "(%t) ACS_CS_ServiceConnection::exec()\n"
		            "Exit thread signaled. Disconnecting client.\n"));

		    exit_ = true;
		}
		else if (eventIndex == 2)			// Socket ready for read
		{
			if (exit_)
				break;

            int bytesRead = 0;
			char buffer[BUFSIZ] = {0};

			do
			{
			    status = tcpClient->read(buffer, BUFSIZE, bytesRead);

			    if (bytesRead > 0)
			    {
			        //------------------------------------------
			        // Debug printout

			        ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
			                "(%t) ACS_CS_ServiceConnection::exec()\n"
			                "Data received, bytes read = %d, data = 0x%s\n",
			                bytesRead,
			                CS_ProtocolChecker::binToString(buffer, bytesRead).c_str()	));

			        //------------------------------------------

			        error = csParser->newData(buffer, bytesRead); // Copy data to parser

			        if (error)
			        {
			            ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
			                    "(%t) ACS_CS_ServiceConnection::exec()\n"
			                    "Error: Cannot copy data to parser\n"));
			        }
			    }
			}
			while (bytesRead > 0);	// Loop as long as data is returned by the TCP connection

			if (status == ACS_CS_TCPClient::OK) // Data has been read and status is still OK
			{								    // We can go on and parse the data
				ACS_CS_PDU pdu;

				try
				{
					error = csParser->parse();			// Parse data
				}
				catch (ACS_CS_Exception ex)
				{
				    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
				            "(%t) ACS_CS_ServiceConnection::exec()\n"
				            "Error: Exception thrown by parser\n"));

				    if (protocolType == ACS_CS_Protocol::Protocol_CS)
				    {
						// Send bad format response and disconnect
						// This response has the version set to <Latest_Version> and the length set to 14
						// All other header fields are set to 0.
						// The body contains the result code bad format (13)

				    	char version = ACS_CS_Protocol::Latest_Version;

						char badFormatBuffer[] = {0x00, version, 0x00, 0x0E, 0x00, 0x00,
												  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
												  0x00, 0x0D};

						uint32_t badFormatLength = 14;

						status = tcpClient->send(badFormatBuffer, badFormatLength);

						ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
								"(%t) ACS_CS_ServiceConnection::exec()\n"
								"Bad format response sent, bytes sent = %d, data = 0x%s\n",
								badFormatLength,
								CS_ProtocolChecker::binToString(badFormatBuffer, badFormatLength).c_str() ));
				    }
				    else if (protocolType == ACS_CS_Protocol::Protocol_MODD)
				    {
				    	// Send failure and disconnect
						// This response has the version set to 1 and the length set to 13
						// All other header fields are set to 0.
						// The body contains the result code failure (3)

						char failureBuffer[] = { 0x00, 0x01, 0x00, 0x0D, 0x00, 0x00,
												 0x00, 0x00, 0x00, 0x03, 0x00, 0x01,
												 0x00};

						uint32_t failureLength = 13;

						status = tcpClient->send(failureBuffer, failureLength);

						ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
								"(%t) ACS_CS_ServiceConnection::exec()\n"
								"Failure response sent, bytes sent = %d, data = 0x%s\n",
								failureLength,
								CS_ProtocolChecker::binToString(failureBuffer, failureLength).c_str() ));
				    }

					exit_ = true;
					break;
				}

				if (error < 0)	// Unfixable error encountered, disconnect without response
				{
				    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
				            "(%t) ACS_CS_ServiceConnection::exec()\n"
				            "Error: Internal error in parser. Disconnecting.\n"));

				    ACS_CS_EVENT(Event_ProgrammingError, ACS_CS_EventReporter::Severity_Event,
							"Error Parsing Data",
							"Cannot parse incoming data",
							"");

					exit_ = true;
					csParser->flush();
					break;
				}

				unsigned int bytesToSend = 0;

				while (csParser->getPDU(pdu))		// Get all available full PDUs from parser
				{
				    if (serviceHandler->newRequest(&pdu) < 0) // Send PDUs to handler
				    {									 // The PDU is converted to a response
				        ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
				                "(%t) ACS_CS_ServiceConnection::exec()\n"
				                "Error: serviceHandler->handlePDU(pdu)\n"));
					}
					else	// PDU can be sent back
					{
					    ACS_CS_PDU * returnPdu = pdu.clone();

					    pduVector.push_back(returnPdu);			// Copy PDU to vector for outgoing data
						bytesToSend += returnPdu->getLength();	// Calculate total data size
					}
				}

				if (bytesToSend >= 12)						// If there is data to send
				{
					vector<ACS_CS_PDU *>::iterator it;
					char * outBuffer = new char[bytesToSend];	// Create buffer for outgoing PDUs
					int offset = 0;

					for (it = pduVector.begin(); it != pduVector.end(); ++it)	// Loop through vector
					{
						ACS_CS_PDU * currentPdu = *it;				// Get PDU
						int currentPduSize = currentPdu->getLength();	// Get PDU-size
						error = currentPdu->getBuffer(outBuffer + offset, currentPduSize); // Copy PDU data
						offset += currentPduSize;						// Update offset
						delete currentPdu;
					}

					pduVector.clear();									// Empty vector

					//------------------------------------------
					// Debug printout

					ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
					        "(%t) ACS_CS_ServiceConnection::exec()\n"
					        "Data sent, bytes sent = %d, data = 0x%s\n",
					        bytesToSend,
					        CS_ProtocolChecker::binToString(outBuffer, bytesToSend).c_str() ));

					//------------------------------------------

					// Send response to client
					offset = 0;
					size_t sizeSent = 0;
					status = tcpClient->send(outBuffer, bytesToSend, sizeSent);

					// If all data wasn't sent
					while (status == ACS_CS_TCPClient::Failed)
					{
					    offset += sizeSent;
					    sizeSent = 0;

						// Send remaining data
						status = tcpClient->send( (outBuffer + offset),
						                        (bytesToSend - offset),
						                        sizeSent);

					}

					delete [] outBuffer;
				}
			}
			else if (status == ACS_CS_TCPClient::NotConnected)      // Client has disconnected
			{
				exit_ = true;
			}
		}
	}

	tcpClient->close();                                             // Disconnect connection

	ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
	        "(%t) ACS_CS_ServiceConnection::exec()\n"
	        "Thread finished\n"));

	return 0;
}


void ACS_CS_ServiceConnection::disconnect ()
{
    if (tcpClient)
        tcpClient->close();

	exit_ = true;

	if (!ACS_CS_Event::SetEvent(exitThreadEvent))   // Notify thread
	{
	    ACS_CS_TRACE((ACS_CS_ServiceConnection_TRACE,
	            "(%t) ACS_CS_ServiceConnection::disconnect()\n"
	            "Error: Cannot signal event\n"));
	}
}


bool ACS_CS_ServiceConnection::hasFinished () const
{
   return finished;
}


void ACS_CS_ServiceConnection::finish ()
{
   finished = true;

   // notify the servicehandler that this thread is not running anymore
   ACS_CS_Event::SetEvent(removeClosedConnectionsEvent);
}

