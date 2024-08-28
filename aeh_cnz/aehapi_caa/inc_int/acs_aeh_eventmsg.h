//========================================================================================
/** @file acs_aeh_eventmsg.h
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-18
 *	@version 1.0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *	DESCRIPTION
 *	The class acs_aeh_eventmsg represents the event message.
 *	It is used to store and handle the information that compose event message that
 *	will be send to the event log system.
 *	The attribute member of the class represents the information that compose event message.
 *
 *
 *
 *	SEE ALSO
 *	-
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2011-05-18 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 |            | xfabron      | Released for ITSTEAM2 sprint 12     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 */
//========================================================================================

#ifndef ACS_AEH_EVENTMSG_H_
#define ACS_AEH_EVENTMSG_H_

#include "string"

using namespace std;

//==============================================================================
//	Class declaration
//==============================================================================

class acs_aeh_eventmsg{

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
	acs_aeh_eventmsg();


	/** @brief constructor
	 *
	 *	acs_aeh_eventmsg Constructor of class
	 *
	 *	@param[in] p_processName:			The name of the reporting process.
	 *	@param[in] p_specificProblem:		The error code number of the problem.
	 *	@param[in] p_percSeverity:			The severity level of the problem according to AXE standard.
	 *	@param[in] p_probableCause:			Detailed explanation of the problem (same as an AXE slogan).
	 *	@param[in] p_objClassOfReference:	Contains the name of the reporting object according to AXE alarm standard. Can have
	 *				 						the following values: APZ, APT, PWR, EXT, EVENT.
	 *	@param[in] p_objectOfReference:		Shows in which instance in the object the fault was detected.
	 *	@param[in] p_problemData:			Free text description to be logged.
	 *	@param[in] p_problemText:			Free text description to be printed.
	 *	@param[in] p_manualCease:			true:  Manual cease allowed,
	 *		 								false: Manual cease not allowed
	 *
	 *	@remarks Remarks
	 */
	acs_aeh_eventmsg(const char * p_processName, long p_specificProblem,
					const char * p_percSeverity, const char * p_probableCause,
					const char * p_objClassOfReference, const char * p_objectOfReference,
					const char * p_problemData, const char * p_problemText,
					bool p_manualCease);


	/** @brief constructor
	 *
	 *	acs_aeh_eventmsg Constructor of class
	 *
	 *	@param[in] p_priority:				The priority of the event Message.
	 *	@param[in] p_nodeName:				The name of node where the event occurred.
	 *	@param[in] p_nodeStatus:			The state of node where the event occurred.
	 *	@param[in] p_userName:				The name of user logged on the node where node where the event occurred.
	 *	@param[in] p_processName:			The name of the reporting process.
	 *	@param[in] p_specificProblem:		The error code number of the problem.
	 *	@param[in] p_percSeverity:			The severity level of the problem according to AXE standard.
	 *	@param[in] p_probableCause:			Detailed explanation of the problem (same as an AXE slogan).
	 *	@param[in] p_objClassOfReference:	Contains the name of the reporting object according to AXE alarm standard. Can have
	 *				 						the following values: APZ, APT, PWR, EXT, EVENT.
	 *	@param[in] p_objectOfReference:		Shows in which instance in the object the fault was detected.
	 *	@param[in] p_problemData:			Free text description to be logged.
	 *	@param[in] p_problemText:			Free text description to be printed.
	 *	@param[in] p_manualCease:			true:  Manual cease allowed,
	 *		 								false: Manual cease not allowed
	 *
	 *	@remarks Remarks
	 */
	acs_aeh_eventmsg( const char* p_priority, const char* p_nodeName,
					const char * p_nodeStatus, const char * p_userName,
					const  char * p_processName, long p_specificProblem,
					const char * p_percSeverity, const char * p_probableCause,
					const char * p_objClassOfReference, const char * p_objectOfReference,
					const char * p_problemData, const char * p_problemText,
					bool p_manualCease);


	/** @brief copy constructor
	 *
	 *	Copy constructor
	 *
	 *	@param[in] 	p_msg acs_aeh_eventmsg object
	 *
	 *	@remarks 	-
	 */
	acs_aeh_eventmsg(const acs_aeh_eventmsg& p_msg);


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	virtual ~acs_aeh_eventmsg();


	/** @brief  Assignment operator.
	 *	Assignment operator.
	 *
	 *	@param[in] 	p_msg: an acs_aeh_eventmsg object
	 *	@return 	Reference to self
	 *	@remarks 	-
	 */
	acs_aeh_eventmsg& operator=(const acs_aeh_eventmsg& p_msg);


	/** @brief  Overloading Comparison operator==.
	 *	Comparison operator== overloading.
	 *
	 *	@param[in] 	p_msg: an acs_aeh_eventmsg object
	 *	@return 	bool
	 *	@remarks 	-
	 */
	bool operator==(const acs_aeh_eventmsg& p_msg);


	/** @brief  Overloading Comparison operator!=.
	 *	Comparison operator!= overloading.
	 *
	 *	@param[in] 	p_msg: an acs_aeh_eventmsg object
	 *	@return 	bool
	 *	@remarks 	-
	 */
	bool operator!=(const acs_aeh_eventmsg& p_msg);


	//===========//
	// Functions //
	//===========//

	/**	@brief setNodeName method
	 *	used to set the value of nodeName field.
	 *
	 *  @param[in] p_nodeName:	The name of node where the event occurred.
	 *
	 *	@return void
	 *
	 *	@remarks 	-
	 *
	 */
	void setNodeName(const char * p_nodeName);


	/**	@brief setPriorityMsg method
	 *	used to set the value of priority field of message.
	 *
	 *  @param[in] p_priority:	The priority of event message.
	 *
	 *	@return void
	 *
	 *	@remarks 	-
	 *
	 */
	void setPriorityMsg(const char * p_priority);


	/**	@brief setNodeStatus method
	 *	used to set the value of nodeStatus variable.
	 *
	 *  @param[in] p_nodeStatus:	The state of node where the event occurred.
	 *
	 *	@return void
	 *
	 *	@remarks 	-
	 *
	 */
	void setNodeStatus(const char * p_nodeStatus);


	/**	@brief setUserName method
	 *	used to set the value of userName variable.
	 *
	 *  @param[in] p_userName:	The name of user logged on the node where node where the event occurred.
	 *
	 *	@return void
	 *
	 *	@remarks 	-
	 *
	 */
	void setUserName(const char * p_userName);


	/**	@brief setProcessName method
	 *	used to set the value of processName variable.
	 *
	 *  @param[in] p_processName:	The name of the reporting process.
	 *
	 *	@return void
	 *
	 *	@remarks 	-
	 *
	 */
	void setProcessName(const char * p_processName);


	/**	@brief setSpecificProblem method
	 *	used to set the value of specificProblem variable.
	 *
	 *  @param[in] p_sepecificProblem:	The error code number of the problem.
	 *
	 *	@return void
	 *
	 */
	void setSpecificProblem(long p_sepecificProblem);


	/**	@brief setPercSeverity method
	 *	used to set the value of percSeverity variable.
	 *
	 *  @param[in] p_percSeverity:	The severity level of the problem according to AXE standard.
	 *
	 *	@return void
	 *
	 */
	void setPercSeverity(const char * p_percSeverity);


	/**	@brief setProbableCause method
	 *	used to set the value of probableCause variable.
	 *
	 *  @param[in] p_probableCause:	Detailed explanation of the problem (same as an AXE slogan).
	 *
	 *	@return void
	 *
	 */
	void setProbableCause(const char * p_probableCause);


	/**	@brief setObjClassOfReference method
	 *	used to set the value of objClassOfReference variable.
	 *
	 *  @param[in] p_objClassOfReference:	Contains the name of the reporting object according to AXE alarm standard. Can have
	 *										the following values: APZ, APT, PWR, EXT, EVENT.
	 *
	 *	@return void
	 *
	 */
	void setObjClassOfReference(const char * p_objClassOfReference);


	/**	@brief setObjectOfReference method
	 *	used to set the value of objectOfReference variable.
	 *
	 *  @param[in] p_objectOfReference:	Shows in which instance in the object the fault was detected.
	 *
	 *	@return void
	 *
	 */
	void setObjectOfReference(const char * p_objectOfReference);


	/**	@brief setProblemData method
	 *	used to set the value of problemData variable.
	 *
	 *  @param[in] p_problemData:	Free text description to be logged.
	 *
	 *	@return void
	 *
	 */
	void setProblemData(const char * p_problemData);


	/**	@brief setProblemText method
	 *	used to set the value of problemText variable.
	 *
	 *  @param[in] p_problemText:	Free text description to be printed.
	 *
	 *	@return void
	 *
	 */
	void setProblemText(const char * p_problemText);


	/**	@brief setManualCease method
	 *	used to set the value of manualCease variable.
	 *
	 *  @param[in] p_manualCease:	true:  Manual cease allowed,
	 *  							false: Manual cease not allowed
	 *
	 *	@return void
	 *
	 */
	void setManualCease(bool p_manualCease);


	/**	@brief getNodeName method
	 *	used to get the value of nodeName field.
	 *
	 *  @return std::string:	The name of node where the event occurred.
	 *
	 *	@remarks 	-
	 *
	 */
	std::string getNodeName();


	/**	@brief getPriorityMsg method
	 *	used to get the value of priority field of message.
	 *
	 *  @return std::string:	The priority of event message.
	 *
	 *	@remarks 	-
	 *
	 */
	std::string getPriorityMsg();


	/**	@brief getNodeStatus method
	 *	used to get the value of nodeStatus variable.
	 *
	 *	@return std::string:	The state of node where the event occurred.
	 *
	 *	@remarks 	-
	 *
	 */
	std::string getNodeStatus();


	/**	@brief getUserName method
	 *	used to get the value of userName variable.
	 *
	 *  @return std::string:	The name of user logged opn the node where node where the event occurred.
	 *
	 *	@remarks 	-
	 *
	 */
	std::string getUserName();



	/**	@brief getProcessName method
	 *	used to get the value of processName variable.
	 *
	 *	@return std::string:	The name of the reporting process.
	 *
	 */
	std::string getProcessName();


	/**	@brief getSpecificProblem method
	 *	used to get the value of specificProblem variable.
	 *
	 *	@return long:	The error code number of the problem.
	 *
	 */
	long getSpecificProblem();


	/**	@brief getPercSeverity method
	 *	used to get the value of percSeverity variable.
	 *
	 *	@return std::string:	The severity level of the problem according to AXE standard.
	 *
	 */
	std::string getPercSeverity();


	/**	@brief getProbableCause method
	 *	used to get the value of probableCause variable.
	 *
	 *	@return std::string:	Detailed explanation of the problem
	 *
	 */
	std::string getProbableCause();


	/**	@brief getObjClassOfReference method
	 *	used to get the value of objClassOfReference variable.
	 *
	 *	@return std::string:	the name of the reporting object according to AXE alarm standard.
	 *
	 */
	std::string getObjClassOfReference();


	/**	@brief getObjectOfReference method
	 *	used to get the value of problemData variable.
	 *
	 *	@return std::string:	Free text description to be logged.
	 *
	 */
	std::string getObjectOfReference();


	/**	@brief getProblemData method
	 *	used to get the value of problemData variable.
	 *
	 *	@return std::string:	Free text description to be logged.
	 *
	 */
	std::string getProblemData();


	/**	@brief getProblemText method
	 *	used to get the value of problemText variable.
	 *
	 *	@return std::string:	Free text description to be printed.
	 *
	 */
	std::string getProblemText();


	/**	@brief getManualCease method
	 *	used to get the value of manualCease variable.
	 *
	 *	@return bool:	true if Manual cease allowed, false if Manual cease not allowed
	 *
	 */
	bool getManualCease();


	/**	@brief composeEventMsg method
	 *	used to compose the event message to send at event log.
	 *
	 *	@return std::string:	pointer to string of event message.
	 *
	 */
	std::string getEventMessage();


private:

	//===========//
	// Functions //
	//===========//

	/**	@brief myReplace method
	 *	find and replace in a string
	 *
	 *	@return std::string			the problem text without newline character
	 *
	 *	@remarks 	-
	 *
	 */
	void myReplace(std::string &str, const std::string &oldStr, const std::string &newStr);

	/**	@brief getUnFormatterProblemText method
	 *	used get the problem text field without new line character
	 *
	 *	@return std::string			the problem text without newline character
	 *
	 *	@remarks 	-
	 *
	 */
	std::string getUnFormatterProblemText();


	/**	@brief getUnFormatterProblemData method
	 *	used get the problem data field without new line character
	 *
	 *	@return std::string			the problem data without newline character
	 *
	 *	@remarks 	-
	 *
	 */
	std::string getUnFormatterProblemData();

	//========//
	// Fields //
	//========//

	// priority of message
	std::string priority;

	// the nodeName of the node where the problem is occurred.
	std::string nodeName;

	// the status of node where the problem is occurred.
	std::string nodeStatus;

	// Contains the name of user logged on the host where the problem is occurred.
	std::string userName;

	// The name of the reporting process.
	std::string processName;

	// The error code number of the problem.
	long specificProblem;

	// The severity level of the problem according to AXE standard.
	std::string percSeverity;

	// Detailed explanation of the problem (same as an AXE slogan).
	std::string probableCause;

	// Contains the name of the reporting object according to AXE alarm standard.
	std::string objClassOfReference;

	// Shows in which instance in the object the fault was detected.
	std::string objectOfReference;

	// Free text description to be logged.
	std::string problemData;

	// Free text description to be printed.
	std::string problemText;

	//  Manual cease flag
	bool manualCease;

	// Event message
	std::string eventMessage;


};


#endif /* ACS_AEH_EVENTMSG_H_ */
