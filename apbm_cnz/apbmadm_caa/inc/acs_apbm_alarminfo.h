
#ifndef HEADER_GUARD_CLASS__acs_apbm_alarminfo
#define HEADER_GUARD_CLASS__acs_apbm_alarminfo acs_apbm_alarminfo
#endif

/** @file acs_apbm_alarminfo.h
 *	@brief
 *	@author xassore
 *	@date 2012-06-29
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
*	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-06-29 | xassore      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_alarminfo


class acs_apbm_alarminfo
{
public:

	typedef enum {
		ACS_APBM_ALARMTYPE_NO_ALARMS,
		ACS_APBM_ALARMTYPE_NIC,
		ACS_APBM_ALARMTYPE_TEAM,
		ACS_APBM_ALARMTYPE_RAID,
		ACS_APBM_ALARMTYPE_DRBD,
		ACS_APBM_ALARMTYPE_BOARDREM,
		ACS_APBM_ALARMTYPE_BOARDFAULT,
		ACS_APBM_ALARMTYPE_SCBEVENT,
		ACS_APBM_ALARMTYPE_SBLINKDOWN,
		ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN,
		ACS_APBM_ALARMTYPE_DISKCONN,
		ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
		ACS_APBM_ALARMTYPE_THUMBDRIVE_MALFUNCTION, //Redesign as per TR-HS30773
		ACS_APBM_ALARMTYPE_LAG_DISABLED,           // introduced lag alarm
		ACS_APBM_ALARMTYPE_APUB_TEMPERATURE
	} acs_apbm_alarm_type_t;



public:

	inline __CLASS_NAME__(acs_apbm_alarm_type_t alarm_type, int32_t slot, int nic_num)
	: _alarm_type(alarm_type), _slot(slot), _nic_num(nic_num), _event_code(-1) {}

	inline __CLASS_NAME__(acs_apbm_alarm_type_t alarm_type, int32_t slot)
		: _alarm_type(alarm_type), _slot(slot), _nic_num(-1), _event_code(-1) {}

	inline __CLASS_NAME__()
		: _alarm_type(ACS_APBM_ALARMTYPE_NO_ALARMS), _slot(-1), _nic_num(-1), _event_code(-1) {}


   ~__CLASS_NAME__(){};

   inline  bool operator==(const __CLASS_NAME__& d) const {
	   return ((_alarm_type == d._alarm_type)&&(_slot == d._slot)&&(_nic_num == d._nic_num));
   }

   inline  acs_apbm_alarm_type_t get_alarm_type() const {return _alarm_type;}

   inline  int32_t get_slot() const {return _slot;}

   inline   int get_nic_num() const {return _nic_num;}
   inline   long get_event_code() const {return _event_code;}

   inline  void set_event_code(const long eventcode)  {_event_code = eventcode;}


private:
   acs_apbm_alarm_type_t _alarm_type;
   int32_t _slot;
   int _nic_num;
   long _event_code;


};
