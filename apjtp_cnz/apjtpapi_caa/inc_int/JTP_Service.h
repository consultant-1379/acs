/*=================================================================== */
/**
   @file   JTP_Service.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class JTP_Service.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/26/2010     XTANAGG        APG43 on Linux.
==================================================================== */
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_JTP_Service_H_)
#define _JTP_Service_H_

/*====================================================================
						INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_DSD_Session.h>
#include <ACS_DSD_Server.h>

/*=================================================================== */
/**
	@class			JTP_Service

	@brief			This is internal class for destinator interface.
**/
/*=================================================================== */
class JTP_Service {

public:
	/*=====================================================================
	                        DEFINE DECLARATION SECTION
	==================================================================== */
	/**
	 * @brief	SERVICE_NAME_MAX_LENGTH
	 * 		  	Maximum length of service name.
	 */
	static const size_t SERVICE_NAME_MAX_LENGTH = 31;

public:
	/*=====================================================================
							CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**

			@brief			Client constructor

			@param			Service
							JTP application name.

			@pre

			@post

			@return			None

			@exception		None
		*/
	JTP_Service(char* Service		//IN
				);

public:
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
	virtual ~JTP_Service();


public:
	/*=================================================================== */
		/**
			@brief			This routine reports an application to DSD with name,
							earlier provided in constructor call.

			@pre			None

			@post			None

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jidrepreq();


	/*=================================================================== */
		/**
			@brief			Will return JTP_HANDLEs to the JTP server application.

			@pre			None

			@post			None

			@param			noOfHandles
							Number of handles to cover.

			@param			handle
					        Pointer to an array of handles.

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool getHandles(int& noOfHandles,			//IN-OUT
					ACE_HANDLE*& handle			//IN-OUT
					);


	/*=================================================================== */
		/**
			@brief			Receive message from session.

			@pre			None

			@post			None

			@param			buf
							Pointer to message buffer.

			@param			nbytes
							no of bytes in message buffer

			@param			S1
							Pointer to session

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jtpRecvMsg(char* buf,			//OUT
					unsigned short nbytes, 		//OUT
					ACS_DSD_Session& S1		//IN
					);


public:
	/*====================================================================
						VARIABLE DECLARATION SECTION
	==================================================================== */
	/**
	 * @brief	ServiceName
	 * 			Name of service.
	 */
	char ServiceName[SERVICE_NAME_MAX_LENGTH + 1];

	/*===================================================================
						PROTECTED DECLARATION SECTION
	=================================================================== */
protected:
	/*===================================================================
						PROTECTED ATTRIBUTE
	=================================================================== */
	/**
	 * @brief	pServer
	 * 			pointer to ACS_DSD_Server
	 */
	ACS_DSD_Server* pServer;

	/**
	 * @brief	Buffer
	 */
	char Buffer[64000];

	/**
	 * @brief	readBufferLen
	 */
	int readBufferLen;

	/**
	 * @brief	bufferCursor
	 */
	int bufferCursor;
}; //end JTP_Service class

#endif //!defined(_JTP_Service_H_)
