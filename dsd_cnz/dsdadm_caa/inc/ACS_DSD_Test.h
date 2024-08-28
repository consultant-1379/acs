#ifndef ACS_DSD_TEST_H_
#define ACS_DSD_TEST_H_

/** @file ACS_DSD_Test.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2013-04-18
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2013
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
 *	| 0.1    | 2013-04-18 | xcasale      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ConfigParams.h"

#include "ace/Task.h"


/** @class ACS_DSD_Test ACS_DSD_Test.h
 *	@brief ACS_DSD_Test class
 *	@author xcasale (Alessio Cascone)
 *	@date 2013-04-18
 *	@version 0.1
 *
 *	ACS_DSD_Test Class detailed description
 */
class ACS_DSD_Test : public ACE_Task_Base
{
public:
	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_DSD_Test Default constructor
	 *
	 *	ACS_DSD_Test Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline ACS_DSD_Test (const char * task_name, uint n_threads = 1)
	: ACE_Task_Base(), _task_name(), _n_threads(n_threads)
	{
		::strncpy(_task_name, task_name ?: "", TASK_NAME_SIZE_MAX);
	}


	//==============//
	// Destructor //
	//==============//

	/** @brief ~ACS_DSD_Test Destructor
	 *
	 *	ACS_DSD_Test Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~ACS_DSD_Test() {}


	//==============//
	// Functions 	//
	//==============//

	/** @brief svc method
	 *
	 *	svc method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int svc ();


	/** @brief start_activity method
		 *
		 *	start_activity method detailed description
		 *
		 *	@return Return Description
		 *	@remarks Remarks
		 */
	virtual int start_activity ();

	/** @brief start_activity method
		 *
		 *	stop_activity method detailed description
		 *
		 *	@param[in] wait_termination Indicates if the method must wait for thread termination
		 *
		 *	@return Return Description
		 *	@remarks Remarks
		 */
	virtual int stop_activity (bool wait_termination);


private:
	int test_SrvProt_StartupPrimitives();
	int test_SrvProt_ListPrimitives(int list_scope);

	char _task_name [TASK_NAME_SIZE_MAX];					// Task name
	uint32_t _n_threads;													// Number of threads active in the Task
};

#endif /* ACS_DSD_TEST_H_ */
