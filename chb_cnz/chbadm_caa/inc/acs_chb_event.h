/*=================================================================== */
/**
   @file   acs_chb_event.h

   @brief Header file for CHB module.

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
   N/A       21/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_EVENT_H
#define ACS_CHB_EVENT_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include<time.h>

#include "acs_chb_heartbeat_def.h"
/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief PROBLEMDATASIZE

          Max size of problem data string
 */
/*=================================================================== */

const int PROBLEMDATASIZE = 192;

/*=================================================================== */
/**
   @brief ACS_CHB_Cause_APfault


 */
/*=================================================================== */

ACS_AEH_ProbableCause
ACS_CHB_Cause_APfault =		"AP INTERNAL FAULT";
/*=================================================================== */
/**
   @brief ACS_CHB_Cause_ClockNotSync

 */
/*=================================================================== */
ACS_AEH_ProbableCause
ACS_CHB_Cause_ClockNotSync =	"AP SYSTEM CLOCK NOT SYNCHRONIZED";

					// APZ alarm text
/*=================================================================== */
/**
   @brief ACS_CHB_EVENT_STG


 */
/*=================================================================== */
const char* const ACS_CHB_EVENT_STG   = "EVENT";//const char* ACS_CHB_EVENT_STG   = "EVENT";//
/*=================================================================== */
/**
   @brief ACS_CHB_ALARM_STG


 */
/*=================================================================== */
const char* const ACS_CHB_ALARM_STG   = "A2";
/*=================================================================== */
/**
   @brief ACS_CHB_CEASING_STG


 */
/*=================================================================== */
const char* const ACS_CHB_CEASING_STG = "CEASING";

//------------------------------------------------------------------------------
// Problem text strings 
//------------------------------------------------------------------------------
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemJTP


 */
/*=================================================================== */
ACS_AEH_ProblemText ACS_CHB_Text_ProblemJTP		=	"Problem communicating with JTP";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemAPMA


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemAPMA						=	"Problem communicating with APMA";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemPHA


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemPHA							=	"Problem reading IMM";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemClock


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemClock						=	"Problem with clock synchronizing";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProcTerm


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProcTerm							=	"Process terminated";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_UndefLink


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_UndefLink							=   "Link missing between tmz and TZ";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemCluster


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemCluster						=	"Problem communicating with Cluster";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemNode


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemNode						=	"CP reported node link problem";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemCS


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemCS						    =	"Problem communicating with CS";
/*=================================================================== */
/**
   @brief ACS_CHB_Text_ProblemAPJTP


 */
/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemAPJTP					   =	"Problem communicating with APJTP";
/*=================================================================== */

/*=================================================================== */
ACS_AEH_ProblemText
ACS_CHB_Text_ProblemDSD					   =	"Problem communicating with DSD";
/*=================================================================== */
/**
   @brief ACS_CHB_EmptySTring


 */
/*=================================================================== */
ACS_AEH_ObjectOfReference
ACS_CHB_EmptySTring								= "";

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_Event


*/
/*=================================================================== */

class ACS_CHB_Event
{
	/*=====================================================================
		                        PUBLIC DECLARATION SECTION
	==================================================================== */
	public:
	/*=====================================================================
		                        CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Constructor for ACS_CHB_Event class.

		             Process name is set to ProcessName:pid

		  @param     ProcessName
					 name of process.


	*/
	/*=================================================================== */
	  ACS_CHB_Event (const char* ProcessName);
	  /*=====================================================================
	  	                        CLASS DESTRUCTOR
	  ==================================================================== */
	  /*=================================================================== */
	  /**
	        @brief     Destructor for ACS_CHB_ClientHandler class.

	  				 The destructor of the class. Deletes all allocated resources.

	  */
	  /*=================================================================== */
	  virtual ~ACS_CHB_Event();
	  /*=================================================================== */
	  /**
	        @brief     The method is used to report an event.

	        @param     specificProblem

  		@param     percSeverity

  		@param     probableCause

  		@param     problemData

  		@param     problemText

  		@param     objOfRef

	        @return    void

	   */
	  /*=================================================================== */
	  void event (ACS_AEH_SpecificProblem specificProblem,
              ACS_AEH_PercSeverity    percSeverity,
              ACS_AEH_ProbableCause   probableCause,
              ACS_AEH_ProblemData     problemData,
              ACS_AEH_ProblemText     problemText,
	  ACS_AEH_ObjectOfReference objOfRef = ACS_CHB_EmptySTring);																		

  
	//  const char *msgTypeStr (ACS_JTP_MsgType msgType);
	  // Description:
	  //    The method converts the msgType to a string char value.
	  // Parameters:
	  //    msgType:IN                A ACS_JTP_MsgType value
	  // Return value:
	  //    char*                     Pointer to a describing string

	//  const char *returnTypeStr (ACS_JTP_ReturnType rcode);
	  // Description:
	  //    The method converts the rcode to a string char value.
	  // Parameters:
	  //    rcode:IN                  A ACS_JTP_ReturnType value
	  // Return value:
	  //    char*                     Pointer to a describing string
	  
	  /*=================================================================== */
          /**
                @brief     The method is used to decide whether to check if this event should be reported or not.
                                           It's used to check if the event has been reported within the last 429 sec.

                @param     specificProblem
                                           event code to check

                @return    bool
                                           true-the event must not to be reported if it happened within the last 429 sec.
                                           false-the event must be reported always.

           */
          /*=================================================================== */
	  bool checkForFrequency(ACS_AEH_SpecificProblem specificProblem);

	  /*=================================================================== */
          /**
                @brief     The method checks whether this particular event has been reported recently (within
                                           the last 429 seconds) and if so, suppress the event reporting. One tick
                                           of 'dwHighDateTime' is equal to roughly 429 seconds.

                @param     specificProblem:
                                           event code to check

                @return    bool
                                           true-suppress event.
                                           false-not too frequent, go ahead and report event.

           */
          /*=================================================================== */
	  bool tooFrequent(ACS_AEH_SpecificProblem specificProblem);

	  /*=====================================================================
	  		                        PRIVATE DECLARATION SECTION
	  	==================================================================== */
	private:
	  /*===================================================================
	                          PRIVATE ATTRIBUTE
	   =================================================================== */
	  /*=================================================================== */
	  /**
	        @brief   dwHighDateTimeEvents
	   */
	  /*=================================================================== */
		time_t dwHighDateTimeEvents[100];
		//ACS_AEH_EvReport evObject;
	  /*=================================================================== */
	  /**
	        @brief   process
	   */
	  /*=================================================================== */
		char process [32];
};

#endif
