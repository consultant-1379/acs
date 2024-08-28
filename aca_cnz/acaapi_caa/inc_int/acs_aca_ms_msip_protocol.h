/*=================================================================== */
/**
   @file   acs_aca_ms_msip_protocol.h

   @brief Header file for MSA API.

          This module contains all the declarations useful to
          specify the class ACAMS_MSIP_Protocol.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/01/2013     XHARBAV       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACAMS_MSIP_Protocol_H
#define ACAMS_MSIP_Protocol_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ACE.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACAMS_MSIP_Protocol
		 This class defines MSIP protocol-specific things.
		 The class is purely static.
*/
/*=================================================================== */
class ACAMS_MSIP_Protocol
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */

public:

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*==================================================================== 

	@brief		Primitive

//==================================================================== */
	enum Primitive
	{
		None = -1,
		ConnectionAccepted = 1,
		ConnectionRefused = 2,
		DeleteFile = 3,
		RequestMsg = 4,
		MsgAvailable = 5,
		ApplDisc = 6,
		CPSystemId = 7,
		CPSystemIdRequest = 8,
		CPSystemIdUnavailable = 9
	};
/*=================================================================== */

//	@brief		ErrorCode

/*==================================================================== */

	enum ErrorCode
	{
		NoError = 0,
		AnotherConnected = 1,
		UnspecifiedError = 2
	};

	static Primitive getPrimitive(ACE_HANDLE msip_socket,
					unsigned int & size,
					unsigned char * buffer);
/*=================================================================== */

/**
	@brief		sendConnectionAccepted

	@param		msip_socket

	@return		bool

*/
/*=================================================================== */
	static bool sendConnectionAccepted(ACE_HANDLE msip_socket);
/*=================================================================== */

/**
	@brief		sendConnectionRefused

	@param		msip_socket

	@param		message

	@return		bool

*/
/*=================================================================== */
	static bool sendConnectionRefused(ACE_HANDLE msip_socket,
					  const char * message = 0);
/*=================================================================== */

/**
	@brief		unpackConnectionRefused

	@param		buffer

	@param		size

	@param		message

	@return		ErrorCode

*/
/*=================================================================== */
	static ErrorCode unpackConnectionRefused(const unsigned char * buffer,
						 unsigned size,
						 std::string & message);
/*=================================================================== */

/**
	@brief		sendDeleteFile

	@param		msip_socket

	@param		fileName

	@return		bool

*/
/*=================================================================== */
	static bool sendDeleteFile(ACE_HANDLE msip_socket,
				   const std::string & fileName);
/*=================================================================== */

/**
	@brief		sendCPSystemId

	@param		msip_socket

	@param		cpSystemId

	@return		bool

*/
/*=================================================================== */
	static bool sendCPSystemId(ACE_HANDLE msip_socket, unsigned cpSystemId);
/*=================================================================== */

/**
	@brief		unpackCPSystemId

	@param		buffer

	@param		size

	@param		cpSystemId

	@return		ErrorCode

*/
/*=================================================================== */
	static ErrorCode unpackCPSystemId(const unsigned char * buffer, unsigned size, unsigned & cpSystemId);
/*=================================================================== */

/**
	@brief		sendCPSystemIdRequest

	@param		msip_socket

	@return		bool
*/
/*=================================================================== */
	static bool sendCPSystemIdRequest(ACE_HANDLE msip_socket);
/*=================================================================== */

/**
	@brief		sendCPSystemIdUnavailable

	@param		msip_socket

	@return		bool
*/
/*=================================================================== */
	static bool sendCPSystemIdUnavailable(ACE_HANDLE msip_socket);
/*=================================================================== */

/**
	@brief		unpackDeleteFile

	@param		buffer

	@param		size

	@param		fileName

	@return		ErrorCode
*/
/*=================================================================== */
	static ErrorCode unpackDeleteFile(const unsigned char * buffer, unsigned size, std::string & fileName);
/*=================================================================== */

/**
	@brief		sendRequestMsg

	@param		msip_socket

	@param		msgNum

	@return		bool
*/
/*=================================================================== */
	static bool sendRequestMsg(ACE_HANDLE msip_socket, unsigned long long msgNum);
/*=================================================================== */

/**
	@brief		unpackRequestMsg

	@param		buffer

	@param		size

	@param		msgNumber

	@return		ErrorCode
*/
/*=================================================================== */
	static ErrorCode unpackRequestMsg(const unsigned char * buffer, unsigned size, unsigned long long & msgNumber);
/*=================================================================== */

/**
	@brief		sendMsgAvailable

	@param		msip_socket

	@return		bool
*/
/*=================================================================== */
	static bool sendMsgAvailable(ACE_HANDLE msip_socket);
/*=================================================================== */

/**
	@brief		unpackMsgAvailable

	@param		buffer

	@param		size

	@return		ErrorCode
*/
/*=================================================================== */
	static ErrorCode unpackMsgAvailable(const unsigned char * buffer, unsigned size);
/*=================================================================== */

/**
	@brief		sendApplDisc

	@param		msip_socket

	@return		void
*/
/*=================================================================== */
	static void sendApplDisc(ACE_HANDLE msip_socket);
/*=================================================================== */

/**
	@brief		getMSIPportNo

	@param		name

	@return		int
*/
/*=================================================================== */
	static int getMSIPportNo(const char * name);
/*=================================================================== */

/**
	@brief		getMSIPportNo

	@param		site

	@param		name

	@return		int
*/
/*=================================================================== */
	static int getMSIPportNo(const char * site, const char * name);
/*=================================================================== */
};

#endif
