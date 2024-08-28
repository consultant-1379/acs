/*
 * * @file ACS_CS_FuncDistHandler.h
 *	@brief
 *	Header file for ACS_CS_FuncDistHandler class.
 *  This module contains the declaration of the class ACS_CS_FuncDistHandler.
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-05-21
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-05-21 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CS_FUNCDISTHANDLER_H_
#define ACS_CS_FUNCDISTHANDLER_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ace/Singleton.h>
#include <ace/Task_T.h>
#include <ace/Synch.h>
#include <ace/RW_Thread_Mutex.h>

#include <string>
/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */

class ACS_CS_ImFunction;
class ACS_CS_ImMoveFunctionProgress;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

namespace moveFunctionParameter {
		extern const char function[];
		extern const char apgNode[];

		extern const char ap1Name[];
		extern const char ap2Name[];

	};

class ACS_CS_FuncDistHandler: public ACE_Task<ACE_MT_SYNCH>
{

 public:

	friend class ACE_Singleton<ACS_CS_FuncDistHandler, ACE_Recursive_Thread_Mutex>;

	/**
	 * 	@brief	This method initializes a task and prepare it for execution
	*/
	int executeActionMoveFunction(const unsigned int& functionIndex, const std::string& apName);

	/**
	 * 	@brief  Run by a daemon thread
	*/
	virtual int svc(void);

	/**
	   @brief  This method is called by ACE_Thread_Exit, as hook, on svc termination
	*/
	virtual int close(u_long flags = 0);

	/**
	   @brief  This method initialize the asycActionResult structure
	*/
	void initFunctionDistribution();

	/**
	   @brief  This method creates the Ap node object
	*/
	void createAPobject();

	/**
	   @brief  This method stop the execution thread
	 */
	void stopThread();

 private:

	/**
	 * 	@brief  Structure to store last action result
	*/


	/**
	 * 	@brief  Validate current action parameters
	*/
	bool validateParameters();

	/**
	 * 	@brief  Validate APG name parameters
	*/
	bool isAPGDefined();

	/**
	 * 	@brief  Validate Function name parameters
	*/
	bool isFunctionMoveableTo();

	/**
	 * 	@brief  Convert function index to the function name
	*/
	void functionIndexToFunctionName(const unsigned int& functionIndex);

	/**
	 * 	@brief  Worker thread to try again AP object creation
	*/
	void createAPObjectThread();

	/**
	 * 	@brief Create IMM Apg object
	*/
	bool createIMMObject();

	/**
	 * 	@brief	Constructor of ACS_CS_FuncDistHandler class
	*/
	ACS_CS_FuncDistHandler();

	/**
	 * 	@brief	Destructor of ACS_CS_FuncDistHandler class
	*/
	virtual ~ACS_CS_FuncDistHandler();

	/**
	 * 	@brief	m_operationOngoing
	 *
	 * 	Flag to indicates operation already ongoing
	*/
	bool m_operationOngoing;

	/**
	 * 	@brief	m_functionName
	 *
	 * 	Current Function to move
	*/
	std::string m_functionName;

	/**
	 * 	@brief	m_apName
	 *
	 * 	Current Ap where move function
	*/
	std::string m_apName;

	/**
	 * 	@brief	m_FunctionObject
	 *
	 * 	Current Function Object
	*/
	ACS_CS_ImFunction* m_FunctionObject;

	/**
	 * 	@brief	m_ReportResult
	 *
	 * 	Object to update reportProgess structure
	*/
	ACS_CS_ImMoveFunctionProgress* m_ReportResult;

	/**
	 * 	@brief	m_CreateThreadRunning
	 *
	 * 	Flag to indicate createAPObjectThread running
	*/
	volatile bool m_CreateThreadRunning;

	/**
	 * 	@brief	m_mutex
	 *
	 * 	Mutex for internal sync
	*/
	ACE_RW_Thread_Mutex m_mutex;
};

typedef ACE_Singleton<ACS_CS_FuncDistHandler, ACE_Recursive_Thread_Mutex> FuncDistHandler;
#endif
