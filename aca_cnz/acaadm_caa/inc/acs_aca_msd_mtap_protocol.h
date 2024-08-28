/*=================================================================== */
/**
        @file           acs_aca_msd_mtap_protocol.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACAMSD_MTAP_Protocol class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       19/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACAMSD_MTAP_Protocol_H
#define ACAMSD_MTAP_Protocol_H

/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
//#include <WinSock2.h>

//#include "ACS_DSD_dsa2.H"
#include <list>

#include "ACS_DSD_Session.h"

/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MTAP_Protocol {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                ENUMERATION DECLARATION SECTION
==================================================================== */
/*===================================================================

   @brief  Primitive

===================================================================*/ 
	enum Primitive {
		PeerClosedConnection = -2,
		None = -1,
		Error = 0,
		PutRecord = 1,
		UnlinkFile = 2,
		UnlinkAllFiles = 3,
		EchoCheck = 4,
		GetParameter = 5,
		Synchronize = 6,
		Stop = 7,
		Skip = 8,
		Do_Will = 9,
		DoNot_WillNot = 10,
		PutExpedRecord = 11  //not defind in Design base.Need to check the real value while functionality do.
	};
/*===================================================================

   @brief  Parameter

===================================================================*/
	enum Parameter {
		WinSize = 0,
		NoOfConn = 1,
		NoOfRecords = 2,
		NoOfResends = 3,
		ResendTime = 4,
		ReconnTime = 5,
		FAV = 6,
		Echo = 7,
		RecordSize = 8
	};
/*===================================================================

   @brief  Option

===================================================================*/
	enum Option {
		Version = 0,
		Unknown = 255
	};
/*===================================================================

   @brief  ErrorCode

===================================================================*/
	enum ErrorCode {
		NoError = 0,
		AccessFileDenied = 1,
		BadFileNumber = 2,
		DiskSpaceExhaust = 6,
		MessageStoreFull = 7,
		RecordOutOfRange = 8,
		UnknownParameter = 10,
		RecordTooLarge = 11,
		IllegalCP_Site = 15,
		TooManyConnections = 16,
		UnspecifiedError = 255
	};

	friend class ACAMSD_MTAP_Message;
/*===================================================================

        @brief          getPrimitive
                         Read one MTAP primitive from the file descriptor. Store its data 
                         into the buffer and return the primitive identifier.

        @param          dsdptr

        @param          size

        @param          buffer

        @return         Primitive

=================================================================== */
	static Primitive getPrimitive (ACS_DSD_Session & dsdptr, unsigned long & size, unsigned char * buffer);
/*===================================================================

        @brief          sendEchoCheckResponse
                         Sending (including packing) and unpacking of primitives (the PutRecord and PutRecordResponse
                         primitives are dealt with in the ACAMSD_MTAP_Message class).

        @param          dsdptr

        @param          buffer

        @return         bool

=================================================================== */
	static bool sendEchoCheckResponse (ACS_DSD_Session & dsdptr, const unsigned char * buffer);
/*===================================================================

        @brief          sendError

        @param          dsdptr

        @param          errorCode

        @return         bool

=================================================================== */
	static bool sendError (ACS_DSD_Session & dsdptr, ErrorCode errorCode);
/*===================================================================

        @brief          unpackGetParameter

        @param          buffer

        @param          size

        @param          parameters

        @return         ErrorCode

=================================================================== */
	static ErrorCode unpackGetParameter (const unsigned char * buffer, unsigned long size, std::list<unsigned char> & parameters);
/*===================================================================

        @brief          sendGetParameterResponse

        @param          dsdptr

        @param          buffer

        @param          parameters

        @param          parameterValues

        @param          ErrorCode result

        @return         bool

=================================================================== */	
	static bool sendGetParameterResponse (
			ACS_DSD_Session & dsdptr,
			const unsigned char * buffer,
			std::list<unsigned char> & parameters,
			std::list<unsigned long> & parameterValues,
			ErrorCode result = NoError);
/*===================================================================

        @brief          unpackOptionNegotiation

        @param          buffer

        @param          size

        @param          optionValue

        @return         ErrorCode

=================================================================== */
	static ErrorCode unpackOptionNegotiation (
			const unsigned char * buffer,
			unsigned long size,
			Option & option,
			unsigned char & optionValue);
/*===================================================================

        @brief          sendOptionNegotiationResponse

        @param          dsdptr

        @param          buffer

        @param          optionValue

        @param          ErrorCode result

        @return         bool

=================================================================== */
	static bool sendOptionNegotiationResponse (
			ACS_DSD_Session & dsdptr,
			const unsigned char * buffer,
			unsigned char optionValue,
			ErrorCode result = NoError);
/*===================================================================

        @brief          sendStop

        @param          dsdptr

        @return         bool

=================================================================== */
	static bool sendStop (ACS_DSD_Session & dsdptr);
/*===================================================================

        @brief          send

        @param          dsdptr

        @param          buffer

        @param          len

        @return         int

=================================================================== */
	static int send (ACS_DSD_Session & dsdptr, unsigned char * buffer, unsigned short len);
/*===================================================================

        @brief          unpackSynchronize

        @param          buffer

        @param          size

        @param          fileNumber

        @param          recordNumber

        @return         ErrorCode

=================================================================== */
	static ErrorCode unpackSynchronize (
			const unsigned char *	buffer,
			unsigned long size,
			unsigned char & fileNumber,
			unsigned long & recordNumber);
/*===================================================================

        @brief          sendSynchronizeResponse

        @param          dsdptr

        @param          buffer

        @param          missingList

        @param          ErrorCode result

        @return         bool

=================================================================== */
	
	static bool sendSynchronizeResponse (
			ACS_DSD_Session & dsdptr,
			const unsigned char * buffer,
			std::list<unsigned long> & missingList,
			ErrorCode result = NoError);
/*===================================================================

        @brief          unpackUnlinkAllFiles

        @param          buffer

        @param          size

        @return         ErrorCode

=================================================================== */
	static ErrorCode unpackUnlinkAllFiles (const unsigned char * buffer, unsigned long size);
/*===================================================================

        @brief          unpackUnlinkFile

        @param          buffer

        @param          size

        @param          fileNumber

        @return         ErrorCode

=================================================================== */
	static ErrorCode unpackUnlinkFile (const unsigned char * buffer, unsigned long size, unsigned char & fileNumber);
/*===================================================================

        @brief          sendUnlink_X_Response

        @param          dsdptr

        @param          buffer

        @param          ErrorCode result

        @return         bool

=================================================================== */
	static bool sendUnlink_X_Response (ACS_DSD_Session & dsdptr, const unsigned char * buffer, ErrorCode result = NoError);
/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          getElementLength

        @param          buf

        @return         unsigned long

=================================================================== */
	static unsigned long getElementLength (const unsigned char * buf);
/*===================================================================

        @brief          putElementLength

        @param          buf

        @param          value

        @return         void

=================================================================== */
	static void putElementLength (unsigned char * & buf, unsigned long value);
/*===================================================================

        @brief          receive

        @param          dsdptr

        @param          buffer

        @param          len

        @param          bytesReceived  

        @return         bool

=================================================================== */
	static bool receive (ACS_DSD_Session & dsdptr, char * buffer, unsigned int len, int * bytesReceived);
/*===================================================================

        @brief          get4bytes

        @param          buf

        @return         unsigned long

=================================================================== */
	static unsigned long get4bytes (const unsigned char * buf);
/*===================================================================

        @brief          put4bytes

        @param          buf

        @param          value

        @return         void

=================================================================== */
	static void put4bytes (unsigned char * & buf, unsigned long value);
};

#endif
