/*
 * protocoltest.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: mann
 */

#include <iostream>
#include <string>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"

#include "ACS_CS_Protocol.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_Primitive.h"
#include "ACS_CS_ExceptionHeaders.h"

using std::endl;
using std::cout;

void testParser();
int sendData();
void getPduExample1(char * buffer, int size);
void printPduData(ACS_CS_PDU &pdu);


int main (int argc, char **argv) {

	testParser();
	//sendData();

	return 0;
}

int sendData() {

	char getRequest[] = "GET /index.htm HTTP/1.1\r\n\r\n";

	cout << getRequest << endl;

	ACE_INET_Addr srvr ("127.0.0.1:80");
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;

	if (-1 == connector.connect (peer, srvr)) {
		cout << "Could not connect" << endl;
		ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect"), 1);
	}

	int bc;
	char buf[64];

	peer.send_n (getRequest, strlen(getRequest));
	bc = peer.recv (buf, sizeof(buf));
	write (1, buf, bc);
	peer.close ();

	return 0;
}

void testParser() {
	int size = 22;
	char buffer[size];
	getPduExample1(buffer, size);

	ACS_CS_Parser parser;
	parser.newData(buffer, size);

	try {
		parser.parse();
	} catch (ACS_CS_Exception e) {
		cout << "Exception caught: " << e.description() << endl;
	}

	ACS_CS_PDU pdu;

	if (parser.getPDU(pdu)) {
		cout << "PDU parsed" << endl;
		printPduData(pdu);
	} else {
		cout << "No PDU" << endl;
	}
}

void printPduData(ACS_CS_PDU &pdu) {

	cout << "Header" << endl;
	cout << endl;
	cout << "Version: " << pdu.getHeader()->getVersion() << endl;
	cout << "Length: " << pdu.getLength() << endl;
	cout << "Scope: " << pdu.getHeader()->getScope() << endl;
	cout << "Primitive id: " << pdu.getHeader()->getPrimitiveId() << endl;
	cout << "Request id: " << pdu.getHeader()->getRequestId() << endl;
	cout << endl;
	cout << "Body" << endl;

	int bodyLength = pdu.getPrimitive()->getLength();
	char * body;

	if (bodyLength > 0) {
		body = new char[bodyLength];
		pdu.getPrimitive()->getBuffer(body, bodyLength);
	}

	for (int i = 0; i < bodyLength; i++) {

		cout << i << " " << ACS_CS_Protocol::CS_ProtocolChecker::binToString(body + i, 1) << endl;
	}
}

void getPduExample1(char * buffer, int size) {

	if (size != 22)
		return;

	// Header
	buffer[0] = 0x0;
	buffer[1] = 0x2;
	buffer[2] = 0x0;
	buffer[3] = 0x16;
	buffer[4] = 0x0;
	buffer[5] = 0x1;
	buffer[6] = 0x0;
	buffer[7] = 0xC;
	buffer[8] = 0x12;
	buffer[9] = 0x34;
	buffer[10] = 0x0;
	buffer[11] = 0x0;

	// Body
	buffer[12] = 0x0;
	buffer[13] = 0x1;
	buffer[14] = 0x0;
	buffer[15] = 0x8;
	buffer[16] = 0x0;
	buffer[17] = 0x4;
	buffer[18] = 0xC0;
	buffer[19] = 0xA8;
	buffer[20] = 0xA9;
	buffer[21] = 0x3C;
}
