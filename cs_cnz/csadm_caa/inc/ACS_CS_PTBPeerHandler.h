/*
 * ACS_CS_PTBPeerHandler.h
 *
 *  Created on: Nov 25, 2011
 *      Author: XPLODAM
 */

#ifndef ACS_CS_PTBPEERHANDLER_H_
#define ACS_CS_PTBPEERHANDLER_H_

#include "ACS_CS_TCPClient.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_CPTSignals.h"

#include <ace/Mutex.h>
#include <ace/Guard_T.h>

#include <sstream>
#include <string>
#include <queue>
#include <deque>
#include <map>


typedef std::map<CPTREADMAUREG::OrderCode, std::string> MACMap;

static const u_int32 PTBAIPAddress = 0xc0a8a97f; // 192.168.169.127
static const char PTBAIPAddressStr[] = "192.168.169.127"; // 0xc0a8a97f
static const u_int16 PTBAPort      = 3000;

static const u_int32 PTBBIPAddress = 0xc0a8aa7f; // 192.168.170.127
static const char PTBBIPAddressStr[] = "192.168.170.127"; // 0xc0a8aa7f
static const u_int16 PTBBPort      = 3000;


class ACS_CS_PTBPeerHandler : public ACS_CS_Thread {
public:

	enum PTBPeer
	{
		PTBA,
		PTBB
	};

	ACS_CS_PTBPeerHandler(PTBPeer side, const std::string &peerAddress, short port);
	virtual ~ACS_CS_PTBPeerHandler();

	ACS_CS_EventHandle *getEvent() {return &newMACAddressEvent;}

	MACMap getMacMap();

	virtual void init();

	virtual int exec();

private:
	ACE_Mutex mutex;
	ACE_SOCK_Stream stream;
	ACS_CS_TCPClient *peer;
	ACE_INET_Addr address;
	PTBPeer ptbSide;

	string getSignalString(int signal);
	bool readCPTREADMAUREGR(CPTREADMAUREGR *cpt);
	bool rememberMac(const CPTREADMAUREGR &cptr);

	ACS_CS_EventHandle newMACAddressEvent;

	std::queue<CPTREADMAUREG> msgFIFO;
	MACMap macMap;
};

#endif /* ACS_CS_PTBPEERHANDLER_H_ */
