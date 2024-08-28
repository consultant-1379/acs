/*=================================================================== */
/**
   @file   JTP_Service_R2A.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class JTP_Service_R2A.

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
#if !defined(_JTP_Service_R2A_H_)
#define _JTP_Service_R2A_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_DSD_Session.h>
#include <ACS_DSD_Server.h>
#include <JTP_Service.h>
#include <ACS_JTP_Job_R2A.h>
#include <ACS_JTP_Conversation_R2A.h>

/*=================================================================== */
	/**
		@class			JTP_Service_R2A

		@brief			This is internal class for destinator interface.
	**/
/*=================================================================== */
class JTP_Service_R2A : public JTP_Service {


public:

	/*=====================================================================
						CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**

			@brief			Client constructor

			@pre

			@post

			@param			Service
							JTP application name.

			@return			None

			@exception		None
		*/
	inline JTP_Service_R2A(char* Service) : JTP_Service(Service) {}


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
	inline virtual ~JTP_Service_R2A() {}


public:
	/*=================================================================== */
		/**
			@brief			With this routine, incoming calls from initiators are
							treated. Type of call can be detected by out values of
							the object pointers.

			@pre			None

			@post			None

			@param			j
							Pointer to job object.

			@param			c
							Pointer to conversation object.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool accept_new(ACS_JTP_Job_R2A* j,				//OUT
					ACS_JTP_Conversation_R2A* c		//OUT
					);
}; //end JTP_Service_R2A class

#endif //!defined(_JTP_Service_R2A_H_)
