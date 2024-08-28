/*=================================================================== */
/**
	@file		acs_chb_thread_handler.h

	@brief		Header file for MTZ  module.

				This module contains all the declarations useful to
				specify the class.

	@version 	1.0.0
**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/11/2010    XTANAGG    Initial Release
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_Threadhandler_H
#define ACS_CHB_Threadhandler_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_chb_file_handler.h>
#include <ace/Thread_Manager.h>
#include <ace/Event.h>
#include <ace/OS_NS_sys_stat.h>
#include <iostream>
using namespace std;

/*====================================================================
					CONSTANT DECLARATION SECTION
==================================================================== */
/**
 *	@brief		ACS_CHB_success
 */
const bool ACS_CHB_success	= false;

/**
 *	@brief		ACS_CHB_error
 */
const bool ACS_CHB_error 	= true;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/*===================================================================*/
/**
	@brief		ACS_CHB_Threadhandler

				The ACS_CHB_Threadhandler contains a class to monitor file changes.
				When the monitored file has been changed the ReadZoneFile() method
				of the ACS_CHB_filehandler class is called.
*/
/*=================================================================== */
class ACS_CHB_Threadhandler {


	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/**
	 *	@brief		The constructor of the class.
	 *
	 * 	@param		pfilehandler
	 * 				Pointer to ACS_CHB_filehandler object.
	 *
	 *  @return		none.
	 */
	ACS_CHB_Threadhandler(ACS_CHB_filehandler* pfilehandler);
	
	/*=====================================================================
							CLASS DESTRUCTORS
	==================================================================== */
	/**
	 *	@brief		The destructor of the class.
	 *
	 *	@return		none.
	 */
	~ACS_CHB_Threadhandler();

	/*===================================================================
						PRIVATE DECLARATION SECTION
	=================================================================== */
private:
	
	/**
	 *	@brief			Starts a new thread in CheckCFGfile() method.
	 *
	 * 	@param			pfilehandler
	 * 					Pointer to ACS_CHB_filehandler object.
	 *
	 *	@return			bool
	 *					ACS_CHB_success
	 *					ACS_CHB_error
	 */
	bool StartThread(ACS_CHB_filehandler* pfilehandler);
	
	/**
	 *	@brief			This method monitors for file change notifications.
	 *
	 * 	@param			p
	 *					Pointer to own object.
	 *
	 *	@return			void*
	 */
	static void* CheckCFGfile(void* p);
	
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */
	/**
	 * 	@brief			Handle to Thread stop event.
	 */
	ACE_Event* pThreadStopEvent;

	/**
	 * 	@brief			Pointer to ACS_CHB_filehandler
	 */
	ACS_CHB_filehandler* pf;

	/**
	*	@brief		Thread Group Id for the ACS_CHB_Threadhandler thread.
	*/
	ACE_INT32 threadGrpId;

	/**
	*	@brief	Thread Id
	*/
	ACE_thread_t	threadId;
		
};

#endif

