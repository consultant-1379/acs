/*=================================================================== */
/**
   @file   API_DSD_Session.h

   @brief  Header file for DSD stubs for APJTP.

		   This module contains all the declarations useful to
           specify the class ACS_DSD_Session.
           .
   @version N.N.N

   @documentno CAA 109 0870

   @copyright Ericsson AB,Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/28/2010     TA        APG43 on Linux.
==================================================================== */
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _API_DSD_Session_h
#define _API_DSD_Session_h

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "API_DSD_Base.h"
#include <string>

const ACE_UINT32 ACS_DSD_MAXBUFFER=65000;

/*=================================================================== */
	/**
		@class		ACS_DSD_Session

		@brief		This is the interface class for DSD session.
	**/
 /*=================================================================== */
class ACS_DSD_Session : public ACS_DSD_API_Base {
public:
	/*=====================================================================
						CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
		/**
			@brief			Constructor

			@pre

			@post

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	ACS_DSD_Session();

	/*===================================================================
					  CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**
			@brief			Destructor

			@pre			None

			@post			None

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	~ACS_DSD_Session();

	/*=================================================================== */
		/**
			@brief			This routine is used to send message.

			@pre			None

			@post			None

			@param			msg
							Message Buffer.

			@param			msgSize
							Size of Message.

			@param			timeout
							Timeout.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool send(const void* msg, ACE_UINT32 msgSize, ACE_UINT32 timeout = 0);

	/*=================================================================== */
		/**
			@brief			This routine is used to receive message.

			@pre			None

			@post			None

			@param			msg
							Message Buffer.

			@param			msgSize
							Size of Message.

			@param			timeout
							Timeout.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool recv(void* msg, ACE_UINT32& msgSize, ACE_UINT32 timeout= 0 );

	/*=================================================================== */
		/**
			@brief			This routine is used to close the session.

			@pre			None

			@post			None

			@return			void

			@exception		None
	*/
	/*=================================================================== */
	void close(void);

	/*===================================================================
							PRIVATE DECLARATION SECTION
	=================================================================== */
private:
	//Overloaded Assignment operator.
	const ACS_DSD_Session &operator=(const ACS_DSD_Session& session);

	//Copy Constructor.
	ACS_DSD_Session(ACS_DSD_Session& session);


};

#endif
