/*
 * acs_chb_aadst_alarmHandler.h
 *
 *  Created on: Sep 23, 2014
 *      Author: xassore
 */
#include "acs_apgcc_omhandler.h"
#include "acs_aeh_evreport.h"

#ifndef ACS_CHB_AADST_ALARMHANDLER_H_
#define ACS_CHB_AADST_ALARMHANDLER_H_

#define AADST_A1_EVENT_CODE											9050 // TODO: verify if this value is usable
#define AADST_A2_EVENT_CODE											9051 // TODO: verify if this value is usable

#define EVENT_PROBLEM_TEXT_MAX_SIZE							256
#define AADST_EVENT_PROBLEM_TEXT_HEAD						"REASON\n"
#define AADST_EVENT_A2_PROBLEM_TEXT_TAIL				"UPDATE FAILED FOR 5 CONSECUTIVE DAYS"
#define AADST_EVENT_A1_PROBLEM_TEXT_TAIL				"UPDATE FAILED, 2 DAYS OR LESS TO SUMMER TIME CHANGE"

#define AADST_EVENT_CLASS_REFERENCE							ACS_AEH_OBJ_CLASS_REFERENZE_APZ

#define AADST_EVENT_OBJ_REFERENCE								"acs_chbheartbeatd/AADST"
#define AADST_EVENT_CAUSE												"AP AUTOMATIC DAYLIGHT SAVING TIME UPDATE FAILED"

#define AADST_PROCESS_NAME											"acs_chbheartbeatd"
#define ALARMINFO_IMM_CLASSNAME									"AlarmInfo"
#define ALARMINFO_EVENTCODE_IMM_ATTRNAME				"specificProblem"
#define ALARMINFO_CEASEPENDING_IMM_ATTRNAME			"ceasePending"


typedef enum {
			CHB_AADST_ALARM_ERROR = -1,
			CHB_AADST_NO_ALARMS = 0,
			CHB_AADST_ALARM_A1 = 1,
			CHB_AADST_ALARM_A2 = 2,
}aadst_alarm_type;

typedef enum {
			AADST_ALARM_STATE_ERROR = -1,
			AADST_ALARM_STATE_ACTIVE = 0,
			AADST_ALARM_STATE_PENDING = 1,
}aadst_alarm_state;

class acs_chb_aadst_alarmHandler {
public:

	inline acs_chb_aadst_alarmHandler(): _omHandlerInitialized (false){};
	virtual ~acs_chb_aadst_alarmHandler();

	int alarm_raise( aadst_alarm_type alarm_type);
	int change_alarm_severity_to_A1( void );
	aadst_alarm_type check_for_alarm_presence(void);

private:
	int get_alarm_state (const char *alarmObject_name);
	int alarm_A2_cease(void);
	int omHandler_init();
	bool _omHandlerInitialized;
	OmHandler _omHandler;
	acs_aeh_evreport _aeh_eventhandler;
};

#endif /* ACS_CHB_AADST_ALARMHANDLER_H_ */
