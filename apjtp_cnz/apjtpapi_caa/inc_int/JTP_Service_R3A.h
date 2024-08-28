/*=================================================================== */
/**
   @file   JTP_Service_R3A.h

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
#if !defined(_JTP_Service_R3A_H_)
#define _JTP_Service_R3A_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <JTP_Service_R2A.h>
#include <ACS_JTP_Job_R3A.h>
#include <ACS_JTP_Conversation_R3A.h>

/*=================================================================== */
/**
	@class			JTP_Service_R3A

	@brief			This is internal class for destinator interface.
**/
  /*=================================================================== */
class JTP_Service_R3A : public JTP_Service_R2A {
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
public:
	inline JTP_Service_R3A(char* Service) : JTP_Service_R2A(Service) {}


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
	inline virtual ~JTP_Service_R3A() {}


public:
	using JTP_Service_R2A::accept_new;

	/*=================================================================== */
		/**
			@brief			With this routine, incoming calls from initiators are
							treated. Type of call can be detected by out values of
							the object pointers.
							JTP_Service::accept_new  version R3A receives and administers
							conversation and job indications from remote side

			@pre			None

			@post			None

			@param			j
							Pointer to job object.

			@param			c
							Pointer to conversation.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool accept_new(ACS_JTP_Job_R3A* j,			//OUT
					ACS_JTP_Conversation_R3A* c	//OUT
					);

}; //end JTP_Service_R3A class

#endif //!defined(_JTP_Service_R3A_H_)
