#ifndef _ACS_LM_LKSENDER_H_
#define _ACS_LM_LKSENDER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */

#include "acs_lm_common.h"
#include <acs_lm_jtp_conversation.h>
#include "acs_lm_sentinel.h"
#include "acs_lm_persistent.h"
#include "acs_lm_eventhandler.h"

/* For new CP versions (i.e, after CM003 release) AP receives the protocol value as '2' from CP */
#define NEW_VER_CP_PROTOCOL	2  

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_LkSender
{
	/*=====================================================================
							 PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*===================================================================
	   						  PUBLIC ATTRIBUTE
	 =================================================================== */

	/*===================================================================
	   							   PUBLIC METHOD
	 =================================================================== */
	/*=================================================================== */
	/**
	 *  @brief sendLks
	 *
	 *  @param senderType 					: LkSenderType
	 *
	 *  @param sentinel 					: ACS_LM_Sentinel
	 *
	 *  @param persistent 					: ACS_LM_Persistent
	 *
	 *  @param eventHandler					: ACS_LM_EventHandler
	 *
	 *  @param node 						: JtpNode
	 *
	 *  @return ACS_LM_AppExitCode
	 **/
	/*=================================================================== */
	static ACS_LM_AppExitCode sendLks(LkSenderType senderType,
			ACS_LM_Sentinel* sentinel,
			ACS_LM_Persistent* persistent,
			ACS_LM_EventHandler* eventHandler,
			JtpNode* node=NULL);
	/*=================================================================== */
	/**
	 *  @brief cancel
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	static void cancel();
private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_LkSender

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_LkSender();
	static ACS_LM_JTP_Conversation* jtpConv;
	//TR-HS41812 Created Mutex for process synchronization during data transfer from AP to CP.
	static ACE_Recursive_Thread_Mutex senddataMutex; 
};

#endif
