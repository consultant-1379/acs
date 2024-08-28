//****************************************************************
//
// NAME
//   ACAMSD_MTAP_Message.C
// 
// COPYRIGHT
//   COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//   All rights reserved.
//
//   The Copyright to the computer program(s) herein 
//   is the property of Ericsson Utvecklings AB, Sweden.
//   The program(s) may be used and/or copied only with 
//   the written permission from Ericsson Utvecklings AB or in 
//   accordance with the terms and conditions stipulated in the 
//   agreement/contract under which the program(s) have been 
//   supplied.

//  DOCUMENT NO
//    CAA 109 0312

//  AUTHOR 
//    EAB/UZ/DG Carl Johannesson

// DESCRIPTION
//   This class can interpret a message received via the MTAP
//   protocol and convert that to the format which is used by the
//   Message Store when storing it to disk.
//
//           Protocol version < 3:
//           ---------------------
//              The MTAP-format is as follows:
//              * [0]:      Put-record primitive identifier
//              * [1]:      MTAP file number
//              * [2]-[3]:  MTAP individual
//              * [4]-[7]:  MTAP message number
//              * [8] -     The message data (can be empty)
//
//              The disk-format is as follows:
//              * [0]-[7]:  Message Store message number
//              * [8]-[11]: Message size (bytes)
//              * [12] -    The message data
//              * 4 bytes:  Offset to start of this message
//              * 4 bytes:  Message-End-Tag
//
//           Protocol version > 2:
//           ---------------------
//              The MTAP-format is as follows:
//              * [0]:      Put-record primitive identifier
//              * [1]:      MTAP file number
//              * [2]-[3]:  MTAP individual
//              * [4]-[7]:  MTAP message number
//              * [8]    :  Version number
//              * [9]    :  Primitive format = Additional data
//              * [10] -    The message data (can be empty)
//
//              The disk-format is as follows:
//              * [0]-[7]:  Message Store message number
//              * [8]-[11]: Message size (bytes)
//              * [12] -    The message data
//              * 1 byte:   Primitive format
//              * 7 bits:   Version number
//              * 17 bits:  Offset to start of this message
//              * 4 bytes:  Message-End-Tag

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO     DATE        NAME       DESCRIPTION
//    A          2000-05-10  uabcajn    product release
//    B          2004-03-12  uabcajn    rogue wave replaced by STL.
//****************************************************************
#include "acs_aca_ms_const_values.h"
#include "acs_aca_utility.h"
#include "acs_aca_logger.h"
#include "acs_aca_msd_mtap_message.h"

//****************************************************************
// Constants
//****************************************************************

// Number of bytes own buffer is longer than MTAP put-record size
const int TotalSizeOffset = 28;
//DSDheader(4)+MsgNo(8)+size(4)+offset(4)+error resp(4)+eom(4)

const int DSDHeaderSize = 4;

// Element length of put-record-response
const int ResponseSize = 9; //const unsigned long ResponseSize = 9;

// Index in own buffer to MTAP file number
const int MTAP_FileNumberOffset = 5;

// Index in own buffer to MTAP message number
const int MTAP_MsgNumberOffset = 8;

// Index in own buffer to ErrorCode of put-record-response primitive
const int ErrorCodeOffset = 12;

// Index in own buffer to where MsgNumber begins
const int MsgNumberOffset = 16;

// Index in own buffer to where size-indicator of disk-data begins
const int SizeOffset = 24;

//****************************************************************
// Constructor
//****************************************************************
ACAMSD_MTAP_Message::ACAMSD_MTAP_Message (
		unsigned channel,
		const unsigned char * buffer,
		unsigned long length,
		unsigned char version,
		ACAMSD_MTAP_Protocol::Primitive primitive)
	: myChannel(channel), myBufferSize(0), myBuffer(0), myMtapNum(0), myMsgNum(0) {

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	// The DSD header was removed by ACAMSD_MTAP_Protocol and is
	// therefore not included in 'buffer' (this was not necessary).
	// Before sending a buffer to DSD, the 4-bytes header must be
	// included (DSD will fill this four bytes).
	// NOTE: the buffer received from DSD contains the header, but
	// the size indicator excludes the four bytes.

	// Allocate own buffer
	myBufferSize = length + TotalSizeOffset - ((version < 3) ? 0 : 2);
	
	// Fill it as far as possible for now
	unsigned char * bufPtr = myBuffer = new unsigned char[myBufferSize];

	// Length indicator for response, bufPtr is moved by this method.
	ACAMSD_MTAP_Protocol::putElementLength(bufPtr, ResponseSize);

	// Put-record-response indicator, MTAP File Number, MTAP Individual, and MTAP Message Number.
	::memcpy(bufPtr, buffer, 8);

	// Get version number and primitive format
	//unsigned char primitiveFormat = 0;
	//unsigned char versionNumber = 0;
	unsigned long backOffset = 0;
	
	if (primitive == ACAMSD_MTAP_Protocol::PutExpedRecord) {}

	if ((version < 3) /*|| (primitive == ACAMSD_MTAP_Protocol::PutExpedRecord)*/) {
		// Default values for version number and primitive format already set size indicator
		bufPtr = myBuffer + SizeOffset;
		::memcpy(bufPtr, &length, 4);
		bufPtr += 4;

		// The message data
		if (length > 8) {
			::memcpy(bufPtr, buffer + 8, length - 8);
			bufPtr += length - 8;
		}

		// Offset to start of Msg
		backOffset = length + 4;
	} else {
		unsigned char primitiveFormat = 0;
		unsigned char versionNumber = 0;

		versionNumber = buffer[8];
		primitiveFormat = buffer[9];

		// Size indicator
		bufPtr = myBuffer + SizeOffset;
		unsigned long p_length = length - 2;
		::memcpy(bufPtr, &p_length, 4);
		bufPtr += 4;

		// The message data
		if (length > 10) {
			::memcpy(bufPtr, buffer + 10, length - 10);
			bufPtr += length - 10;
		}

		// Offset to start of Msg
		unsigned char mostSigbit = 0x00000001 & ((length + 2) >> 16);
		unsigned char middleByte = 0x000000FF & ((length + 2) >> 8);
		unsigned char leastSigByte = 0x000000FF & (length + 2);
		backOffset = primitiveFormat | (versionNumber << 9) | (mostSigbit << 8) | (middleByte << 16) | (leastSigByte << 24);
	}

	::memcpy(bufPtr, &backOffset, 4);
	bufPtr += 4;

	// End of Msg
	::memcpy(bufPtr, ACAMS_Msg_End_Mark, 4);

	// get the MTAP number.
	bufPtr = myBuffer + MTAP_MsgNumberOffset;
	::memcpy(&myMtapNum, bufPtr, 4);

/*
 * You are progemming in C++ not in java. You do not have
 * any garbage collector to trigger.
 *
	if (bufPtr != 0) {
		bufPtr = 0;
	}
*/

	// myMsgNum will be assigned later
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// Destructor
//****************************************************************
ACAMSD_MTAP_Message::~ACAMSD_MTAP_Message () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	delete[] myBuffer;
	myBuffer = 0;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// Return MTAP file number (used by daemon when creating a new
// file). The file number is a part of the file name.
//****************************************************************
unsigned char ACAMSD_MTAP_Message::getMtapFileNumber () const {
	return myBuffer[MTAP_FileNumberOffset];
}

//****************************************************************
// Set Message number (done by collection at adding a message).
//****************************************************************
void ACAMSD_MTAP_Message::putMsgNumber (unsigned long long msgNum) {
	setBufferFromUll(myMsgNum = msgNum, myBuffer + MsgNumberOffset);
}

//****************************************************************
// get data length of buffer that should be stored on disk.
//****************************************************************
unsigned int ACAMSD_MTAP_Message::getDataLength () const {
	return myBufferSize - MsgNumberOffset;
}
 
//****************************************************************
// get the actual data.
//****************************************************************
const unsigned char * ACAMSD_MTAP_Message::getData () const {
	return myBuffer + MsgNumberOffset;
}

//****************************************************************
// Build an MTAP put-record-response primitive and return the size 
// of it.
// The internal buffer already contains a put-record-response 
// primitive, except for the last byte (the ErrorCode).
//****************************************************************
int ACAMSD_MTAP_Message::getResponse (unsigned char * & buffer, ACAMSD_MTAP_Protocol::ErrorCode result) {
	myBuffer[ErrorCodeOffset] = result;
	buffer = myBuffer + DSDHeaderSize;
	return ResponseSize;
}
