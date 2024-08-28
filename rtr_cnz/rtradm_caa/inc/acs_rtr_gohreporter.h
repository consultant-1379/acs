/*=================================================================== */
/**
   @file  acs_rtr_gohreporter.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_RTR_GOHREPORTER_H_
#define _ACS_RTR_GOHREPORTER_H_
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_rtr_errorsupportbase.h"
#include "acs_rtr_events.h"
#include "acs_rtr_outputqueue.h"
#include "acs_rtr_statistics.h"
#include "acs_rtr_lib.h"
#include <string>

using namespace std;

namespace RTR_AlarmText
{
	const char problemText1[] = "CAUSE\nRECORD TRANSFER, DESTINATION\n\n\n\nTRANSFER QUEUE\ntransferQueueId=";
}

typedef ACE_Task< ACE_MT_SYNCH > Task;
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief     ACS_RTR_GOHReporter

 */
/*=================================================================== */

class ACS_RTR_GOHReporter : public Task , public ACS_RTR_ErrorSupportBase
{
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief     IDLE
 */
/*=================================================================== */
	static const int IDLE = 1;
/*=================================================================== */
/**
   @brief     ACTIVE
 */
/*=================================================================== */
	static const int ACTIVE = 2;
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief     Constructor for ACS_RTR_GOHReporter class.

   @param     transferQueue

   @param     queue

   @param     eventHandler

   @param     statptr
 */
/*=================================================================== */
	ACS_RTR_GOHReporter(std::string transferQueue,
						ACS_RTR_OutputQueue * queue,
						RTR_Events * eventHandler,
						RTR_statistics * statptr);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief     Destructor for ACS_RTR_GOHReporter class.
 */
/*=================================================================== */	
	inline virtual ~ACS_RTR_GOHReporter() {}
/*=================================================================== */
/**
   @brief     setRtrrmEvent

   @return    bool  
 */
/*=================================================================== */
	virtual bool setRtrrmEvent() = 0;

	virtual void fxStop() = 0;

	virtual int open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent)  = 0;

	bool isRunning() const { return (thr_count() > 0); };
/*=================================================================== */
/**
   @brief     getState

   @return    int
 */
/*=================================================================== */
	inline int getState() { return _state; }
/*=================================================================== */
/**
   @brief     setState

   @param     state 

   @return    void
 */
/*=================================================================== */
	inline void setState(int state) { _state = state; }
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
protected:
	//RTR definition/ACS_RTR_Manager level references - NO DELETE
/*=================================================================== */
/**
   @brief     _transferQueue
 */
/*=================================================================== */
	std::string _transferQueue;
/*=================================================================== */
/**
   @brief     _outputQueue
 */
/*=================================================================== */
	ACS_RTR_OutputQueue* _outputQueue;

/*=================================================================== */
/**
@brief     _eventHandler
*/
/*=================================================================== */
	RTR_Events* _eventHandler;
/*=================================================================== */
/**
 	 @brief     _statptr
*/
/*=================================================================== */
	RTR_statistics* _statptr;
/*=================================================================== */
/**
	@brief     _state
 */
/*=================================================================== */
	int _state;
/*=================================================================== */
/**
	@brief     m_sendBlockError
*/
/*=================================================================== */
	int m_sendBlockError ;

/*=================================================================== */
/**
   @brief     AttachTQerror
 */
/*=================================================================== */
	int m_attachFileTQerror ;

/*=================================================================== */
/**
   @brief     m_attachBlockTQerror
 */
/*=================================================================== */
	int m_attachBlockTQerror ;

/*=================================================================== */
/**
   @brief     m_sendFilesError
 */
/*=================================================================== */
	int m_sendFilesError;

	bool _gohStopRequested;

	ACE_HANDLE m_jobRemoved;

	ACE_HANDLE m_stopEvent;

};

#endif //_ACS_RTR_GOHREPORTER_H_
