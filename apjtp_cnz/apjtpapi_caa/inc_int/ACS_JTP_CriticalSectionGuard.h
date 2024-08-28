/*=================================================================== */
/**
   @file   ACS_JTP_CriticalSectionGuard.h

   @brief  Header file for APJTP module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_CriticalSectionGuard.

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
#if !defined(_ACS_JTP_CriticalSectionGuard_H_)
#define _ACS_JTP_CriticalSectionGuard_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
====================================================================*/
#include <ACS_JTP_CriticalSection.h>
/*=================================================================== */
	/**
		@class		ACS_JTP_CriticalSectionGuard

		@brief		This is the class for critical section.
	**/
/*=================================================================== */
class ACS_JTP_CriticalSectionGuard{
	/*=====================================================================
						CLASS CONSTRUCTORS
	==================================================================== */
	/**
	 * @brief	ACS_JTP_CriticalSectionGuard
	 * 			Class Constructor
	 */
	public:inline explicit ACS_JTP_CriticalSectionGuard(ACE_thread_mutex_t &pCS):_pCS(0),_pCriticalSection(0){
		ACE_OS::thread_mutex_lock(&pCS);
		_pCS = &pCS;
	}
	/*=====================================================================
						CLASS CONSTRUCTORS
	==================================================================== */
	/**
	 * @brief	ACS_JTP_CriticalSectionGuard
	 * 			Class Constructor.
	 */
	public:inline explicit ACS_JTP_CriticalSectionGuard(ACS_JTP_CriticalSection &pCriticalSection):_pCS(0),_pCriticalSection(0){
		pCriticalSection.enter();
		_pCriticalSection=&pCriticalSection;
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
	public:inline~ACS_JTP_CriticalSectionGuard(){
		if(_pCS)
			ACE_OS::thread_mutex_unlock(_pCS);
		if(_pCriticalSection)
			_pCriticalSection->leave();
	}
	/*===================================================================
						PRIVATE DECLARATION SECTION
	================================================================== */
	private:
	/*===================================================================
						PRIVATE ATTRIBUTE
	=================================================================== */
	ACE_thread_mutex_t* _pCS;
	ACS_JTP_CriticalSection* _pCriticalSection;
};
#endif//!defined(_ACS_JTP_CriticalSectionGuard_H_)
