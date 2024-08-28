/*
 * acs_alh_imm_data_handler.h
 *
 *  Created on: Nov 8, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_IMM_DATA_HANDLER_H_
#define ACS_ALH_IMM_DATA_HANDLER_H_

/** @file acs_alh_imm_data_handler
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-11-08
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 * DESCRIPTION
 *	The acs_alh_imm_data_handler class is responsible to manage the IMM data
 *
 *
 * ERROR HANDLING
 * -
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-08 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "acs_alh_imm_connection_handler.h"
#include "acs_alh_macroconfig.h"
#include "acs_alh_error.h"
#include "acs_alh_common.h"
#include "acs_alh_util.h"

#include "acs_apgcc_omhandler.h"


#define IMM_ATTR_NAME_MAX_SIZE   128

#define IMM_ALARM_INFO_CLASS_ATTR_MAXNUM 16	// max num of attribute for IMM SRT class


struct acs_alh_immAttributes{
	  char attr_name[IMM_ATTR_NAME_MAX_SIZE];
	  uint16_t attr_type;
	  uint16_t attr_num;

};

const int MAX_NUMBER_OF_VALUE_ATTR = NO_OF_CP_SIDES;


enum alh_imm_AlarmInfo_Attribute {
	AL_INFO_identity_rdn   		= 0,   		//  This must be the first value
	AL_INFO_process_name_attr  	= 1,
	AL_INFO_specProb_attr  		= 2,
	AL_INFO_probCause_attr 		= 3,
	AL_INFO_category_attr 		= 4,
	AL_INFO_obj_ref_attr 		= 5,
	AL_INFO_problemData_attr 	= 6,
	AL_INFO_problemText_attr 	= 7,
	AL_INFO_severity_attr 		= 8,
	AL_INFO_manualCease_attr 	= 9,
	AL_INFO_sendPriority_attr 	= 10,
	AL_INFO_cpAlarmRef_attr 	= 11,
	AL_INFO_retrCounter_attr 	= 12,
	AL_INFO_ceasePending_attr 	= 13,
	AL_INFO_acknowledge_attr 	= 14,
	AL_INFO_time_attr 			= 15 // This must be the last value
};


#define AL_INFO_IDENTITY_RDN_ATTR_TYPE  	ATTR_STRINGT
#define AL_INFO_PROCESS_NAME_ATTR_TYPE  	ATTR_STRINGT
#define AL_INFO_SPECIFIC_PROB_ATTR_TYPE 	ATTR_UINT32T
#define AL_INFO_PROBABLE_CAUSE_ATTR_TYPE    ATTR_STRINGT
#define AL_INFO_CATEGORY_ATTR_TYPE      	ATTR_STRINGT
#define AL_INFO_OBJ_REF_CAUSE_ATTR_TYPE     ATTR_STRINGT
#define AL_INFO_PROBLEM_DATA_ATTR_TYPE      ATTR_STRINGT
#define AL_INFO_PROBLEM_TEXT_ATTR_TYPE      ATTR_STRINGT
#define AL_INFO_CP_ALARM_REF_ATTR_TYPE      ATTR_UINT32T
#define AL_INFO_RETR_COUNTER_ATTR_TYPE   	ATTR_UINT32T
#define AL_INFO_CEASE_PENDING_ATTR_TYPE   	ATTR_INT32T
#define AL_INFO_SEND_PRIORITY_ATTR_TYPE   	ATTR_UINT32T
#define AL_INFO_ACKNOWLEDGE_ATTR_TYPE   	ATTR_INT32T
#define AL_INFO_TIME_TYPE   				ATTR_STRINGT
#define AL_INFO_SEVERITY_ATTR_TYPE   	  	ATTR_STRINGT
#define AL_INFO_MANUAL_CEASE_ATTR_TYPE   	ATTR_INT32T



struct alh_imm_attributes {
	char identity_rdn[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX];
	char process_name[ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX];
	char probable_cause[ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX];
	char category[ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX];
	char object_of_ref[ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX];
	char problem_data[ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX];
	char problem_text[ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX];
	char time[MAX_NUMBER_OF_VALUE_ATTR][ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX];
	char severity[ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX];
	unsigned int specificProblem;
	unsigned int cp_alarm_ref[MAX_NUMBER_OF_VALUE_ATTR];
	unsigned int retr_counter[MAX_NUMBER_OF_VALUE_ATTR];
	int cease_pending[MAX_NUMBER_OF_VALUE_ATTR];
	unsigned int send_priority;
	int manual_cease;
	int acknowledge[MAX_NUMBER_OF_VALUE_ATTR];

};


//========================================================================================
//	Class declarations for acs_alh_imm_data_handler
//========================================================================================
class acs_alh_imm_data_handler : public virtual acs_alh_error {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief Default constructor
	 *
	 *	Constructor of class
	 *
	 *	@remarks -
	 */
	acs_alh_imm_data_handler(acs_alh_imm_connection_handler *immConnectionHandler);


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	~acs_alh_imm_data_handler();


	//===========//
	// Functions //
	//===========//

	/** @brief  createAlarmListNode.
	 *
	 *	create the AlarmListNode object associated to local node in IMM.
	 *
	 *	@param[in] 	nodeName: The name of node used to identify the alarmListnode object in IMM.
	 *
	 *	@return 	int: error code
	 *	@remarks 	-
	 *
	 */
	int createAlarmListNode(const char *nodeName);


	/** @brief  removeAlarmListNode.
	 *
	 *	delete the AlarmListNode object and all its children associated to local node in IMM.
	 *
	 *	@param[in] 	nodeName: The name of node used to identify the alarmListnode object in IMM.
	 *
	 *	@return 	int: error code
	 *	@remarks 	-
	 *
	 */
	int removeAlarmListNode(const char *nodeName);



	int checkIfObjectIsPresent(const char *dnObject, const char* className);


	int addAlarmInfo(const AllRecord* allPtr);

	int getAlarmInToList(std::vector<AllRecord>* alarmList, int cpSide);

	//TODO
	//RENDERLO PRIVATE
	int setALarmInfoAttribute(alh_imm_AlarmInfo_Attribute attrib, void **valueAttr);

	int updateCeasePending(const char* rd_name, int value, int cpSide);

	int ackAlarm(const char* rd_name, unsigned short cpRef, int ack, int cpSide);

	int removeAlarmFromIMM(const char *rdName, bool separated);

	int unAckEvent(const char *rdName, int cpside);


private:

	int omHandler_init();

	void reset_attributes(void);

	time_t convertEventTime(std::string eventTime);

	OmHandler om_handler_;
	bool omHandlerInitialized_;
	acs_alh_util util_;
	acs_alh_imm_connection_handler * immConnectionHandler_;

	alh_imm_attributes alarmInfo_attr_;

	// public data
	vector<ACS_CC_ValuesDefinitionType> attrList_;
};


#endif /* ACS_ALH_IMM_DATA_HANDLER_H_ */
