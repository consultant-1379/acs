/*=================================================================== */
/**
   @file   acs_emf_param.h

   @brief Header file for EMF module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XRAMMAT   Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_EMF_SERVER_H
#define ACS_EMF_SERVER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_emf_common.h"
#include "acs_emf_dvdhandler.h"
#include "acs_emf_commandhandler.h"
#include "acs_emf_cmdserver.h"
#include <ace/Monotonic_Time_Policy.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include "acs_emf_tra.h"
#include "acs_emf_aeh.h"
#include "acs_emf_root_ObjectImpl.h"
#include "acs_emf_haservice.h"
class ACS_EMF_HAService;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
    @class              EMF_SERVICE_MODULE

    @brief              This class implements the EMF Service
**/
 /*=================================================================== */

// Class EMF_SERVICE_MODULE definition
class EMF_SERVICE_MODULE:/*public ACE_Event_Handler,*/public ACE_Task<ACE_SYNCH,ACE_Monotonic_Time_Policy>
//class EMF_SERVICE_MODULE:/*public ACE_Event_Handler,*/public ACE_Task<ACE_SYNCH>
{
	/*=============================================================================
	            PUBLIC DECLARATION SECTION
	  =============================================================================*/
public:
	/*===================================================================
	                                CLASS CONSTRUCTOR
	  =================================================================== */
	/*=================================================================== */
	/**
		@brief                  EMF Service constructor
		@pre					none
		@post					none
		@return                 none
		@exception              none
	 */
	/*=================================================================== */
	// Constructor
	EMF_SERVICE_MODULE();
	/*===================================================================
	                                CLASS DESTRUCTOR
	  =================================================================== */
	/*=================================================================== */
	/**
	    @brief                  Destructor
        @pre                    none
        @post                   none
        @return                 none
        @exception              none
	 */
	/*=================================================================== */
	~EMF_SERVICE_MODULE();


public:
	/*=============================================================================
								PUBLIC METHODS
	  =============================================================================*/

	/*=================================================================== */
	/**
                @brief                  This method starts the EMF server in active mode
                @pre                   	ha_obj 
                @post                   none
                @return                 int
                @exception              none
	 */
	/*=================================================================== */
	int active(ACS_EMF_HAService *ha_obj);
	/*=================================================================== */
	/**
	                @brief                  This method starts the EMF server in passive mode
	                @pre                   	ha_obj
	                @post                   none
	                @return                 int
	                @exception              none
	 */
	/*=================================================================== */
	int passive(ACS_EMF_HAService *ha_obj);
        
	/*=================================================================== */
        /**
                @brief                  This method stops EMF server
                @pre                    none
                @post                   none
                @return                 void
                @exception              none
         */
        /*=================================================================== */
	void stop();
	

	/*=================================================================== */
	/**
		@brief                  This method starts all the required threads
		                        for EMF server
		@pre                    none
		@post                   none
		@return                 void
		@exception              none
	 */
	/*=================================================================== */
	void Run();

	/*=================================================================== */
	/**
		@brief                  Start
		@pre                    none
		@post                   none
		@return                 void
		@exception              none
	 */
	/*=================================================================== */
	void Start(void);

	/**
	 * @brief Timeout handler for terminating interactive mode
	 * @param current_time Time that our event handler was selected for dispatching
	 * @param arg  Pointer passed at the time of registration
	 * @return
	 */
	int handle_timeout(const ACE_Time_Value &current_time, const void * arg);

	/**
	 * Signal handler function
	 * @brief This function handles interrupt and terminate signals.
	 *        If terminate signal encounters, then an event will be raised regarding the EMF monitor service termination.
	 *
	 */
	int handle_signal(ACE_INT32 signum, siginfo_t * siginfo=0, ucontext_t * ucontext=0);

public:

	/*=================================================================== */
	/**
		@brief                  This method spawns a thread with the
								given arguments
		@pre                    none
		@post                   none
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */
	static ACE_INT32 s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
			ACE_TCHAR* lpszThreadName,ACE_INT32 s32GrpId);
	/*=================================================================== */
	/**
		@brief                  This method starts a thread with the
								given arguments
		@pre                    none
		@post                   none
		@param                  ThreadFunc
		@param                  Arglist
		@param                  lpszThreadName
		@param                  s32GrpId
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */
	static ACE_INT32 StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
			void* Arglist,
			const ACE_TCHAR* lpszThreadName,
			ACE_INT32 s32GrpId = -1);
	// Thread startup functions
	/**
	 * @brief This function is thread call back function for emfcopy
	 *        after it is spawned
	 * @param lpvThis Pointer to EMF_SERVICE_MODULE
	 * @return ACE Thread function return Value
	 */
	static ACE_THR_FUNC_RETURN EmfCopyCommandHandlerProc(void* lpvThis);
	/**
	 * @brief This function is thread call back function for emf functional thread
	 *        after it is spawned
	 * @param aEMFPtr Pointer to EMF_SERVICE_MODULE
	 * @return ACE Thread function return Value
	 */
	static ACE_THR_FUNC_RETURN EMFThreadFunctionalMethod(void* aEMFPtr);
	static ACE_THR_FUNC_RETURN EMFPassiveThreadMethod(void* aEMFPtr);
	/*=================================================================== */
	/**
		@brief                  This method setups a emf functional thread
		                        required for emf HA servcie.
		@pre                    none
		@post                   none
		@param					aEMFPtr
		@return                 ACS_CC_ReturnType
		@exception              none
	 */
	/*=================================================================== */
	static ACS_CC_ReturnType setupEMFThreadinActive(EMF_SERVICE_MODULE * aEMFPtr);
	static ACS_CC_ReturnType setupEMFThreadinPassive(EMF_SERVICE_MODULE * aEMFPtr);
	bool setupIMMCallBacks( ACE_Reactor *poReactor );
	ACS_CC_ReturnType startEMFFunctionalityinActive();
	ACS_CC_ReturnType startEMFFunctionalityinPassive();
	/*=================================================================== */
	/**
		@brief                  This method gets application threadID.
		@pre                    none
		@post                   none
		@return                 ACE_thread_t
		@exception              none
	 */
	/*=================================================================== */
	ACE_thread_t getAppThreadId()
	{
		return applicationThreadId;
	}

	/*=================================================================== */
	/**
		@brief                  This method shutdowns emf service
		@pre                    none
		@post                   none
		@return                 int
		@exception              none
	 */
	/*=================================================================== */

	/**
	 * Shuts down EMF Monitor Service
	 */
	int shutdown();

	ACE_INT32 dsdServerClient(void);

private:
	ACS_DSD_Server *theEMFCmdServer;

	ACS_DSD_Client *theEMFCmdClient;

	ACS_DSD_Session* theEMFCmdSession;

//	ACE_HANDLE stopThreadFds;

	/*=================================================================== */
	/**
		@brief                  This method registers emfcopy command handler.
	    @pre                    none
		@post                   none
		@return                 void
		@exception              none
	 */
	/*=================================================================== */
	bool EmfCopyCommandHandler();
	/**
	 * Sets Timeout for interactive mode
	 * @param u64TimeOut
	 * @return TimerId
	 */
	ACE_INT32 setTimeoutForInteractiveMode( ACE_UINT64  u64TimeOut);
	/*=================================================================== */
	/**
		@brief                  This method resets  the timer.
		@pre                    none
		@post                   none
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */
	ACE_INT32 s32ResetTimer(ACE_INT32 s32Timer);
	/*=================================================================== */
	/**
		@brief                  This method sets shutdown handler for emf service.
		@pre                    none
		@post                   none
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */
	bool setupShutdownHandler( ACE_Reactor *poReactor );

	/*=================================================================== */
	/**
		@brief                  This method send msg to DVD owner to disable the media.
		@pre                    none
		@post                   none
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */
	ACE_INT32 disableMediaOnDvdOwnerNode(int nodeState);

public:

	/**
	 * @brief	static pointer of emf service module
	 */
	static EMF_SERVICE_MODULE * m_poServiceHandler;

private:

	/**
	 * @brief	m_pDVDHandler
	 */
	ACS_EMF_MEDIAHandler*       m_pMediaHandler;
	//ACS_EMF_DVDHandler*       m_pDVDHandler;
	/**
	 * @brief	m_poReactor
	 */
	ACE_Reactor *m_poReactor;
	/**
	 * @brief	thetpReactorImpl
	 */
	ACE_TP_Reactor *thetpReactorImpl;

	/**
	 * @brief	m_poObjectImpl
	 */
	ACS_EMF_ObjectImpl *m_poObjectImpl;

	/**
	 * @brief	adminOperator
	 */
	acs_emf_commandhandler* adminOperator;

	/**
	 * @brief	m_s32TimerId
	 */
	ACE_INT32 m_s32TimerId;

	/**
	 * @brief	applicationThreadId
	 */
	static ACE_thread_t applicationThreadId;
	/**
	 * @brief      *m_haObj
	 */
	ACS_EMF_HAService *m_haObj;

	ACS_EMF_CmdServer cmdServer;
	bool m_isStopSignalled;
	ACS_DSD_Node m_LocalNode;
};

#endif /* ACS_EMF_SERVER_H */
