/*=================================================================== */
/**
   @file   ACS_JTP_CriticalSection.h

   @brief  Header file for APJTP module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_CriticalSection.

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
#if !defined(_ACS_JTP_CriticalSection_H_)
#define _ACS_JTP_CriticalSection_H_

#include <ace/ACE.h>

/*=================================================================== */
	/**
		@class		ACS_JTP_CriticalSection

		@brief		This is the class for critical section.
	**/
/*=================================================================== */
class ACS_JTP_CriticalSection{

	/*=====================================================================
								CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**

			@brief			Client constructor

			@pre

			@post

			@return			none

			@exception		none
		 */
	public:inline ACS_JTP_CriticalSection():_sysCS(){

		ACE_OS::thread_mutex_init(&_sysCS);
	}

	/*===================================================================
		                        CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief			Destructor

			@pre			none

			@post			none

			@return			none

			@exception		none
		*/
	/*=================================================================== */
	public:inline ~ACS_JTP_CriticalSection(){

		ACE_OS::thread_mutex_destroy(&_sysCS);
	}

	/*===================================================================
	                           PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
		/**
			@brief			This method is used to enter the critical section.

			@pre			none

			@post			none

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	public:inline void enter(){

		ACE_OS::thread_mutex_lock(&_sysCS);
	}

	/*=================================================================== */
		/**
			@brief			This method is used to leave the critical section.

			@pre			none

			@post			none

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	public:inline void leave(){

		ACE_OS::thread_mutex_unlock(&_sysCS);
	}
	/*===================================================================
		                       PRIVATE DECLARATION SECTION
	================================================================== */
	private:
	/*===================================================================
		                       PRIVATE ATTRIBUTE
	=================================================================== */
	ACE_thread_mutex_t _sysCS;
};
#endif
