#ifndef _ACS_LM_EVENTHANDLER_H_
#define _ACS_LM_EVENTHANDLER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
//#include "ACS_AEH_EvReport.h"
#include "acs_lm_common.h"
#include "acs_lm_alarmstore.h"
#include <acs_aeh_evreport.h>
#include "acs_lm_tra.h"

#define SERVICE_NAME		"ACS_LM_Server"
#define OBJECT_CLASS_NAME	"APZ" 
#define OBJECT_REFERENCE	"APZ License Manager"   /* TR -HT53260. "License" word spelling corrected */ 
#define ACS_LM_EVENT_ID_BASE 10801
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_EventHandler
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
	struct LmEvent
	{	
		int specificProblem;
		std::string perceivedSeverity;
		std::string probableCause;
		std::string problemText;
		std::string problemData;		

		//friend std::ostream& operator<<(std::ostream& out, const LmEvent event)
		friend std::ostream& operator<<(std::ostream& out, const LmEvent& event)
		{
			out<<event.probableCause<<" ("<<event.perceivedSeverity<<")";
			return out;
		}
	};

	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_EventHandler

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_EventHandler();
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_EventHandler

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================*/
	~ACS_LM_EventHandler();

	enum AlarmId
	{
		ACS_LM_ALARM_LK_EXPIRE_A2       = ACS_LM_EVENT_ID_BASE,
				ACS_LM_ALARM_LK_FILE_MISS       = ACS_LM_EVENT_ID_BASE+1,
				ACS_LM_ALARM_EM_STATE_UNAVAIL   = ACS_LM_EVENT_ID_BASE+2,
				ACS_LM_ALARM_LK_EXPIRE_A3       = ACS_LM_EVENT_ID_BASE+3,
				ACS_LM_ALARM_EM_STATE_LOCK_RESET= ACS_LM_EVENT_ID_BASE+4,
				ACS_LM_ALARM_NEW_LK_INSTALL     = ACS_LM_EVENT_ID_BASE+5,
				ACS_LM_ALARM_EM_MODE_ACT        = ACS_LM_EVENT_ID_BASE+6,
				ACS_LM_ALARM_EM_MODE_DEACT      = ACS_LM_EVENT_ID_BASE+7,
				ACS_LM_ALARM_TEST_MODE_ACT      = ACS_LM_EVENT_ID_BASE+8,
				ACS_LM_ALARM_TEST_MODE_DEACT    = ACS_LM_EVENT_ID_BASE+9,
				ACS_LM_ALARM_PERSISTENT_FAULT   = ACS_LM_EVENT_ID_BASE+10,
				ACS_LM_ALARM_JTP_CONN_FAULT     = ACS_LM_EVENT_ID_BASE+11,
				/* LM Maintenance Mode */
				ACS_LM_ALARM_MAINTENANCE_MODE_ACT     = ACS_LM_EVENT_ID_BASE+12,  
				ACS_LM_ALARM_MAINTENANCE_MODE_DEACT   = ACS_LM_EVENT_ID_BASE+13,
				ACS_LM_ALARM_MAINTENANCE_MODE	= ACS_LM_EVENT_ID_BASE+14,

	};
	/*=================================================================== */
	/**
	 *  @brief  verifyAndRaiseAlarm
	 *
	 *  @param  lkList 								: list<LkData*>&
	 *
	 *  @param isServiceStartup 					: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void verifyAndRaiseAlarm(std::list<LkData*>& lkList, bool isServiceStartup=false);
	/*=================================================================== */
	/**
	 *  @brief  raiseAlarm
	 *
	 *  @param alarmId 						: ACE_UINT16
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void raiseAlarm(ACE_UINT16 alarmId);
	/*=================================================================== */
	/**

	 *  @brief  clearAll
	 *
	 *  @param alarmId 						: ACE_UINT16
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void ceaseAlarm(ACE_UINT16 alarmId);
	/*=================================================================== */
	/**
	 *  @brief  clearAll

	 *  @return void
	 **/
	/*=================================================================== */
	void clearAll();
	/*=================================================================== */
	/**
	 *  @brief  clearLists

	 *  @return void
	 **/
	/*=================================================================== */
	void clearLists();
	/*=================================================================== */
	/**
	 *  @brief reportEvent
	 *
	 *  @param event 						: LmEvent
	 *
	 *  @param manualCease 					: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	static void reportEvent(LmEvent event, bool manualCease = false);
	static LmEvent lmEvents[];
private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	=================================================================== */
	ACS_LM_AlarmStore* alarmStore;
	/*=================================================================== */
	/**
	 *  @brief daysLeftToExpire
	 *
	 *  @param lkExpTime 						: tm
	 *
	 *  @return long
	 **/
	/*=================================================================== */
	long daysLeftToExpire(struct tm& lkExpTime);
	/*=================================================================== */
	/**
	 *  @brief  compare
	 *
	 *  @param first 						: LkData
	 *
	 *  @param second					    : LkData
	 *
	 *  @return ACE_INT16
	 **/
	/*=================================================================== */
	static ACE_INT16 compare(LkData* first, LkData* second);
	static acs_aeh_evreport * theAEHObjPtr;

private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	 =================================================================== */
	ACS_LM_EventHandler(const ACS_LM_EventHandler&); // disabled.
	const ACS_LM_EventHandler& operator=(const ACS_LM_EventHandler&); // disabled.
};
#endif
