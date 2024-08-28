/*=================================================================== */
/**
   @file   ACS_JTP_Service_R2A.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_Service_R2A.

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
#if !defined(_ACS_JTP_Service_R2A_H_)
#define _ACS_JTP_Service_R2A_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "ACS_JTP_Job_R2A.h"
#include "ACS_JTP_Conversation_R2A.h"

/*=================================================================== */
/**
    @class		ACS_JTP_Service_R2A

    @brief		This is the interface class for service.
**/
/*=================================================================== */
class ACS_JTP_Service_R2A {
	/*=====================================================================
						   PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */
		/**

			@brief			Constructor

			@pre

			@post

			@param			Service
							JTP application name.

			@return			None

			@exception		None
	*/
	ACS_JTP_Service_R2A(char* Service		//IN
						);

	/*===================================================================
		                       CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief			Destructor of service object.

			@pre			None

			@post			None

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	virtual ~ACS_JTP_Service_R2A();

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
			@brief			With this routine, incoming calls from initiators are
							treated. Type of call can be detected by out values of
							the object pointers.

			@pre			None

			@post			None

			@param			j
							Object pointer to a possible calling JTP job.
							Can be NULL if jobs are not	expected to connect to the server.

			@param			c
							Object pointer to a possible calling JTP conversation.
							Can be NULL if conversations are not expected to connect
							to the server.

			@return			bool
			true			If one of job or conversation was connecting. Use getState
							method to determine type of call.
			false			a) If Service is not reported to the Directory Service with jidrepreq() method.
							b) If both job and conversation pointers are NULL.
							c) If only one of job or conversation pointer is set and "the other" type session-
							request is coming in.

			@exception		None
		*/
	/*=================================================================== */
	bool accept(ACS_JTP_Job_R2A* j,			//IN
				ACS_JTP_Conversation_R2A* c	//IN
				);

	/*=================================================================== */
		/**
			@brief			Will return JTP_HANDLEs to the JTP server application.

			@pre			None

			@post			None

			@param			noOfHandles
							Number of handles to cover.

			@param			hand
							Pointer to array of handles.

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
			*/
	/*=================================================================== */
	void getHandles(int& noOfHandles,	//IN-OUT
					JTP_HANDLE*& hand		//IN-OUT
					);
	/*===================================================================
						   PRIVATE DECLARATION SECTION
	=================================================================== */

private:
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */
	JTP_Service_R2A* Internal;
};

#endif //!defined(_ACS_JTP_Service_R2A_H_)
