/*
 * ACS_CS_PTBPeerHandler.cpp
 *
 *  Created on: Nov 25, 2011
 *      Author: XPLODAM
 */

#include "ACS_CS_PTBPeerHandler.h"

#include <ace/SOCK_Connector.h>

#include "ACS_CS_Trace.h"

using namespace std;

ACS_CS_Trace_TDEF(ACS_CS_PTBPeerHandler_TRACE);


ACS_CS_PTBPeerHandler::ACS_CS_PTBPeerHandler(PTBPeer side, const std::string &peerAddress, short port) :
		ptbSide(side)
{
	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
		"ACS_CS_PTBPeerHandler::ACS_CS_PTBPeerHandler(PTBPeer, const std::string &, short): address=%s, port=%d", peerAddress.c_str(), port));


	stringstream ss;
	ss << peerAddress << ":" << port;

	address = ACE_INET_Addr(ss.str().c_str(), AF_INET);

	newMACAddressEvent = ACS_CS_Event::CreateEvent(false, false, (const char*) NULL);
}

ACS_CS_PTBPeerHandler::~ACS_CS_PTBPeerHandler()
{
	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
		"ACS_CS_PTBPeerHandler::~ACS_CS_PTBPeerHandler()"));

	ACS_CS_Event::CloseEvent(newMACAddressEvent);
}

void ACS_CS_PTBPeerHandler::init()
{
	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
		"ACS_CS_PTBPeerHandler::init()"));

	CPTREADMAUREG::CPSide cpSide = ptbSide == PTBA ? CPTREADMAUREG::CP_A : CPTREADMAUREG::CP_B;

	msgFIFO.push(CPTREADMAUREG(CPTREADMAUREG::PCIH_TWIN, cpSide));
	msgFIFO.push(CPTREADMAUREG(CPTREADMAUREG::PCIH_OWN, cpSide));
	msgFIFO.push(CPTREADMAUREG(CPTREADMAUREG::CPSB_ETH1, cpSide));
	msgFIFO.push(CPTREADMAUREG(CPTREADMAUREG::CPSB_ETH0, cpSide));
}

int ACS_CS_PTBPeerHandler::exec()
{
	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE, "ACS_CS_PTBPeerHandler::exec()"));

	// We expect write failures to occur but we want to handle them where
	// the error occurs rather than in a SIGPIPE handler.
	signal(SIGPIPE, SIG_IGN);

	const int MACSIZE = 12;
	const int SLEEP_BEFORE_NEXT_CHECK = 10;

	ACE_Time_Value timeout(3);
	ACE_SOCK_Connector connector;

	bool connected = false;

	if(-1 == connector.connect(stream, address, &timeout))
	{
		ACS_CS_FTRACE((ACS_CS_PTBPeerHandler_TRACE, LOG_LEVEL_ERROR,
				"ACS_CS_PTBPeerHandler::exec(): connecting to %s failed",
				address.get_host_addr())
		);
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
				"ACS_CS_PTBPeerHandler::exec() "
				"Connected to %s", address.get_host_addr())
		);
		connected = true;
	}

	CPTREADMAUREGR cptr;
	struct timespec rem;
	struct timespec req;
	req.tv_sec = 1;
	req.tv_nsec = 0;

	int totalMessages = msgFIFO.size();
	int currentMessage = 0;
	bool changeDetected = false;

	while(!exit_)
	{

		if (!connected)
		{
			if(-1 == connector.connect(stream, address, &timeout))
			{
				ACS_CS_FTRACE((ACS_CS_PTBPeerHandler_TRACE, LOG_LEVEL_ERROR,
						"ACS_CS_PTBPeerHandler::exec(): reconnecting to %s failed. Retrying...",
						address.get_host_addr())
				);
			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_PTBPeerHandler_TRACE, LOG_LEVEL_ERROR,
						"ACS_CS_PTBPeerHandler::exec(): reconnected to %s",
						address.get_host_addr())
				);
				connected = true;
			}
		}

		int sent = -1;

		// Get and prepare message
		CPTREADMAUREG cpt = msgFIFO.front();
		if (connected)
		{
			currentMessage++;
			char *data = new char[cpt.size()];
			memcpy(data, cpt.message(), cpt.size());

			ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
					"ACS_CS_PTBPeerHandler::exec() "
					"Sending signal %s", getSignalString(cpt.orderCode()).c_str())
			);

			// Send the signal
			sent = stream.send(data, cpt.size(), 0);
			delete[] data;

			//move the cpt to the end of the queue
			msgFIFO.pop();
			msgFIFO.push(cpt);
		}

		switch(sent)
		{
		case 0:
			ACS_CS_FTRACE((ACS_CS_PTBPeerHandler_TRACE, LOG_LEVEL_ERROR,
					"ACS_CS_PTBPeerHandler::exec(): "
					"Send signal %s failed. Stream send returned 0.", getSignalString(cpt.orderCode()).c_str(), sent));

			if(!exit_)
				nanosleep(&req, &rem);

			continue;
			break;

		case -1:
			ACS_CS_FTRACE((ACS_CS_PTBPeerHandler_TRACE, LOG_LEVEL_ERROR,
					"ACS_CS_PTBPeerHandler::exec(): "
					"Send signal %s failed. Stream send returned -1 (not connected). Trying to reconnect..",
					getSignalString(cpt.orderCode()).c_str())
			);

			stream.close();
			connected = false;

			if(!exit_)
				nanosleep(&req, &rem);

			continue;
			break;

		default:
			ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
					"ACS_CS_PTBPeerHandler::exec(): "
					"send signal %s OK. Length of sent data is %d", getSignalString(cpt.orderCode()).c_str(), sent));
		}

		ACE_Time_Value ts(3, 0);
		CPTREADMAUREGR cptr;
		const size_t dataSize = sizeof(CPTREADMAUREGR);
		char *cptdata = reinterpret_cast<char *>(&cptr);
		size_t sizeReceived = 0;
		ssize_t ret;

		do
		{
			ret = stream.recv_n(cptdata + sizeReceived, dataSize - sizeReceived, &timeout, &sizeReceived);
		} while(sizeReceived < sizeof(CPTREADMAUREGR) && ret > 0);

		if(ret <= 0){
			ACS_CS_FTRACE((ACS_CS_PTBPeerHandler_TRACE, LOG_LEVEL_ERROR,
					"ACS_CS_PTBPeerHandler::exec(): "
					"Retrieving answer to signal %s failed. Stream send returned %d", getSignalString(cpt.orderCode()).c_str(), ret));

			stream.close();
			connected = false;
		}
		else{

			ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
					"ACS_CS_PTBPeerHandler::exec(): "
					"Retrieved answer to signal %s with fault code %d. MAC address: %s", getSignalString(cpt.orderCode()).c_str(), (int)cptr.faultCode(),  cptr.MACAddress()));

//			if(cptr.faultCode() != CPTREADMAUREGR::NoFault)
//				continue;

			// Format 0xAABBCCDDEEFF to AABBCCDDEEFF
			char mac[MACSIZE+1];
			memcpy((void *)mac, (void *)&cptr.MACAddress()[2], MACSIZE);
			mac[MACSIZE] = 0;

			CPTREADMAUREG::OrderCode orderCode = cptr.orderCode();

			// If not already stored - store it in macMap and signal to ServiceHandler to update leases
			if(macMap[orderCode].compare(mac) != 0){

				ACE_Guard<ACE_Mutex> guard(mutex);
				macMap[orderCode] = mac;

				ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
						"ACS_CS_PTBPeerHandler::exec(): Added new MAC address to macMap: %s", macMap[orderCode].c_str()));

				changeDetected = true;
			}

			// When all messages in message queue has been fetched - check if we had any changes since last time
			if(currentMessage >= totalMessages){

				if(changeDetected){
					ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
							"ACS_CS_PTBPeerHandler::exec(): setting newMACAddressEvent event with macMap of size %d", macMap.size()));

					changeDetected = false;
					ACS_CS_Event::SetEvent(newMACAddressEvent);
				}

				currentMessage = 0;

				for(int i=0; i < SLEEP_BEFORE_NEXT_CHECK && !exit_; i++)
					nanosleep(&req, &rem);
			}
		}

		if(!exit_)
			nanosleep(&req, &rem);
	}

	stream.close();

	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE, "ACS_CS_PTBPeerHandler::exec(): exit"));

	return 0;
}

bool ACS_CS_PTBPeerHandler::readCPTREADMAUREGR(CPTREADMAUREGR *cpt)
{
//	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
//		"ACS_CS_PTBPeerHandler::readCPTREADMAUREGR(CPTREADMAUREGR *)"));

	const size_t dataSize = sizeof(CPTREADMAUREGR);
	char *data = reinterpret_cast<char *>(cpt);
	size_t sizeReceived = 0;
	ACE_Time_Value timeout(1);
	ssize_t ret;

	do
	{
		ret = stream.recv_n(data + sizeReceived, dataSize - sizeReceived, &timeout, &sizeReceived);
	} while(sizeReceived < sizeof(CPTREADMAUREGR) && ret > 0 && exit_ == false);

	return (ret > 0);
}

bool ACS_CS_PTBPeerHandler::rememberMac(const CPTREADMAUREGR &cptr)
{
//	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
//		"ACS_CS_PTBPeerHandler::rememberMac(const CPTREADMAUREGR &)"));

	CPTREADMAUREG::OrderCode orderCode = cptr.orderCode();
	const unsigned long int macSize = 7;
	char mac[macSize];

	// The received MAC address is on the form 0xaabbccddeeff (string).
	memcpy((void *)mac, (void *)cptr.MACAddress()[2], macSize - 1);
	mac[macSize - 1] = 0;

	ACE_Guard<ACE_Mutex> guard(mutex);

	if(macMap[orderCode] == mac)
		return false;

	macMap[orderCode] = mac;

	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
		"ACS_CS_PTBPeerHandler::rememberMac(const CPTREADMAUREGR &): "
		"new MAC: %s", mac));

	return true;
}

MACMap ACS_CS_PTBPeerHandler::getMacMap()
{
	ACS_CS_TRACE((ACS_CS_PTBPeerHandler_TRACE,
		"ACS_CS_PTBPeerHandler::getMacMap()"));

	ACE_Guard<ACE_Mutex> guard(mutex);
	return macMap;
}

string ACS_CS_PTBPeerHandler::getSignalString(int signal)
{
	string name;

	switch(signal){
		case CPTREADMAUREG::PCIH_TWIN: name = "PCIH_TWIN"; break;
		case CPTREADMAUREG::PCIH_OWN: name = "PCIH_OWN"; break;
		case CPTREADMAUREG::CPSB_ETH1: name = "CPSB_ETH1"; break;
		case CPTREADMAUREG::CPSB_ETH0: name = "CPSB_ETH0"; break;
		default: name = "UNDEFINED";
	}

	return name;
}
