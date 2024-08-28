//********************************************************************************
//
// NAME
// acs_rtr_gohblockreporter.h
//
// COPYRIGHT Marconi S.p.A, Italy 2007.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-06 by XLANSRI
//
//********************************************************************************
/*=================================================================== */
/**
   @file     acs_rtr_gohblockreporter.h 

   @brief    Header file for rtr module.

             This module contains all the declarations useful to
             specify the class.

   @version  1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       23/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_RTR_GOHBLOCKREPORTER_H_
#define _ACS_RTR_GOHBLOCKREPORTER_H_
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_rtr_gohreporter.h"
#include "acs_rtr_blockreport.h"
#include "acs_rtr_blockbuild.h"
#include "acs_rtr_systemconfig.h"
#include "acs_rtr_manager.h"
#include "acs_aca_message_store_interface.h"

#include <string>
#include <map>
#include <list>
#include <sys/time.h>
#include "ace/ACE.h"
#include <ace/Recursive_Thread_Mutex.h>

using namespace std;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief       ACS_RTR_GOHBlockReporter
                     Inherited publically from ACS_RTR_GOHReporter class.
*/
/*=================================================================== */
class ACS_RTR_Manager;

class ACS_RTR_GOHBlockReporter : public ACS_RTR_GOHReporter {
/*=====================================================================
                        PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                        TYPEDEF DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief   commitFileMap_t
                   Stores the commit files
*/
/*=================================================================== */
	typedef map<std::string, ACS_RTR_BlockToCommitStore*> commitFileMap_t;
/*=====================================================================
                        ENUMERATION DECLARATION SECTION
==================================================================== */
    enum { // indexes in the HANDLE array
    	stop_pos = 0,	// 0 termination request handle
		rtrrm_pos, 		// 1 rtrrm command issued
		elem_pos		// 2 output queue handle
	};
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief   RTRBREP_NO_ERROR
*/
/*=================================================================== */
	static const int RTRBREP_NO_ERROR = 0; //NO error
	//Error constants
/*===================================================================*/
/**
      @brief   BREP_ERR_NEW_COMMITMAP
*/
/*=================================================================== */
	static const int BREP_ERR_NEW_COMMITMAP = -1;
/*===================================================================*/
/**
      @brief   BREP_ERR_CREATE_EVENT
*/
/*=================================================================== */
	static const int BREP_ERR_CREATE_EVENT = -2;
/*===================================================================*/
/**
      @brief   BREP_ERR_ACA_COMMIT
*/
/*=================================================================== */
	static const int BREP_ERR_ACA_COMMIT = -3;
/*===================================================================*/
/**
      @brief   BREP_MAX_RETRIES_ON_ERROR
*/
/*=================================================================== */
	static const int BREP_MAX_RETRIES_ON_ERROR = 30;
/*===================================================================*/
/**
      @brief   BREP_MAX_RETRIES_ON_SEND
*/
/*=================================================================== */
	static const int BREP_MAX_RETRIES_ON_SEND = 1;
/*===================================================================*/
/**
      @brief   BREP_WAIT_ON_COMMIT_ERROR
*/
/*=================================================================== */
	static const int BREP_WAIT_ON_COMMIT_ERROR = 200;//3000;
/*===================================================================*/
/**
      @brief   BREP_WAIT_ON_TERMINATE_ERROR
*/
/*=================================================================== */
	static const int BREP_WAIT_ON_TERMINATE_ERROR = 100;//500;
/*===================================================================*/
/**
      @brief   BREP_WAIT_ON_DETACH_ERROR
*/
/*=================================================================== */
	static const int BREP_WAIT_ON_DETACH_ERROR = 100;//500;
/*===================================================================*/
/**
      @brief   BREP_NOT_ALIGNED_DELAY
*/
/*=================================================================== */
	static const int BREP_NOT_ALIGNED_DELAY = 10;
/*===================================================================*/
/**
	  @brief   TRANSACTION_END_DELAY
*/
/*=================================================================== */
	static const int TRANSACTION_END_DELAY = 800;
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================*/
/**
      @brief   Constructor for ACS_RTR_GOHBlockReporter class.

      @param   transferQueue

      @param   queue

      @param   eventHandler

      @param   statptr

      @param   rtrManager 
*/
/*=================================================================== */
	ACS_RTR_GOHBlockReporter(std::string transferQueue,
							ACS_RTR_OutputQueue * queue,
							RTR_Events* eventHandler,
							RTR_statistics* statptr,
							ACS_RTR_Manager * rtrManager);
/*=====================================================================
                        CLASS DESTRUCTOR
==================================================================== */
/*===================================================================*/
/**
      @brief   Destructor for ACS_RTR_GOHBlockReporter class.
*/
/*=================================================================== */
	virtual ~ACS_RTR_GOHBlockReporter();
/*===================================================================*/
/**
      @brief   setRtrrmEvent

      @return  bool 
*/
/*=================================================================== */	
	bool setRtrrmEvent();

	void fxStop();

	virtual int open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent);

	bool getNotCommittedAcaMessages(std::string &cpname, unsigned int & nAcaMessages, short cpId);

protected :

        virtual int svc (void );

private:

	/**
      	  @brief   GetTickCount
      	  @return  unsigned
 	*/
	unsigned GetTickCount();

	/**
      	  @brief   fxDoWork
      	  @return  int
 	*/
	int fxDoWork();

	/**
      	  @brief   fxLifeCycle
      	  @param   cpQueue
      	  @return  bool
	*/
	bool fxLifeCycle(ACS_RTR_OutputQueue* cpQueue);

	/**
		  @brief   fxCreateBlockReporter
		  @param   cpname
		  @param   cpid
		  @return  RTRblockReporter
 	*/
	RTRblockReporter* fxCreateBlockReporter(string cpname, short cpid);

	/**
		  @brief   fxGetCommitFile
		  @param   cpname
		  @param   cpid
		  @return  ACS_RTR_BlockToCommitStore
 	*/
	ACS_RTR_BlockToCommitStore* fxGetCommitFile(string cpname, unsigned cpid);

	/**
      @brief   fxAttachPhase
      @param   pReporter
      @param   cpQueue
      @param   cpname
      @return  ACE_UINT32
 	*/
	ACE_UINT32 fxAttachPhase(RTRblockReporter* pReporter, ACS_RTR_OutputQueue * cpQueue, const char * cpname);

	/**
		  @brief   fxSendBlockSet
		  @param   bRep
		  @param   cpQueue
		  @param   cpname
		  @return  bool
 	*/
	bool fxSendBlockSet(RTRblockReporter* bRep, ACS_RTR_OutputQueue * cpQueue, const char * cpname);

	/**
      	  @brief   fxEndOfTransaction
      	  @param   pReporter
      	  @param   cpname
      	  @param   sentItem
      	  @param   cpQueue
      	  @return  bool
 	*/
	bool fxEndOfTransaction(RTRblockReporter* pReporter, string cpname, ACS_RTR_OutputQueue* cpQueue);

	bool fxForcedEndOfTransaction(RTRblockReporter* pReporter, string cpname, ACS_RTR_OutputQueue* cpQueue);

	/**
      	  @brief   fxDeleteBlockReporter
      	  @param   bRep
      	  @return  void
 	*/
	void fxDeleteBlockReporter(RTRblockReporter* bRep);

	/**
      	  @brief   fxDeleteItemsFromCpQueue
      	  @param   cpQueue
      	  @param   numOfItems
      	  @return  unsigned long long
	*/
	unsigned long long fxDeleteItemsFromCpQueue(ACS_RTR_OutputQueue* cpQueue, ACE_UINT32 numOfItems);

	/**
	      	  @brief   adaptiveCommitDelayEstimation
	      	  @param   lastSentBlock
	      	  @param   lastCommittedBlock
	      	  @param   timeDifference
	      	  @return  int
	*/
	ACE_UINT32 adaptiveCommitDelayEstimation(ACE_UINT32 lastSentBlock, ACE_UINT32 lastCommittedBlock, ACE_UINT32 timeDifference);

	/**
      	  @brief   _mgrCallback
	*/
	ACS_RTR_Manager* _mgrCallback;

	/**
	      	  @brief   commitFileMap
		*/
	commitFileMap_t* commitFileMap;

	/**
      	  @brief   _hDataToSend
	*/
	ACE_HANDLE _hDataToSend;

	/**
      	  @brief   _tqCriticalSection
	*/
	ACE_Recursive_Thread_Mutex _tqCriticalSection;

	/**
      	  @brief   _singleCpAttachCounterEnabled
	*/
	bool _singleCpAttachCounterEnabled;

	/**
      	  @brief   _isMultipleCPSystem
	*/
	bool _isMultipleCPSystem;
};

#endif // _ACS_RTR_GOHBLOCKREPORTER_H_
