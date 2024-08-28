/*
 * * @file ACS_CS_ImMoveFunctionProgress.h
 *	@brief
 *	Header file for ACS_CS_ImMoveFunctionProgress class.
 *  This module contains the declaration of the class ACS_CS_ImMoveFunctionProgress.
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-05-27
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
 *	| 1.0.0  | 2013-05-27 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CS_IMMOVEFUNCTIONPROGRESS_H_
#define ACS_CS_IMMOVEFUNCTIONPROGRESS_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include <stdint.h>
/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */

class ACS_CS_ImFunction;
class OmHandler;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

class ACS_CS_ImMoveFunctionProgress
{
 public:

	enum ActionResultType{
		SUCCESS = 1,
		FAILURE,
		NOT_AVAILABLE
	};

	enum ActionStateType{
		CANCELLING = 1,
		RUNNING,
		FINISHED,
		CANCELLED
	};

	ACS_CS_ImMoveFunctionProgress();

	virtual ~ACS_CS_ImMoveFunctionProgress();

	/**
	 * 	@brief  create the object ProgressReport structure
	*/
	static bool createProgressReportObject();

	/**
	 * 	@brief  Update ProgressPercentage attribute of ReportResult structure
	*/
	void setProgressPercentage(uint32_t progressPercentage);

	/**
	 * 	@brief  Update Result attribute of ReportResult structure
	*/
	void setResult(ActionResultType result);

	/**
	 * 	@brief  Update ResultInfo attribute of ReportResult structure
	*/
	void setResultInfo(const std::string& resultInfo);

	/**
	 * 	@brief  Update State attribute of ReportResult structure
	*/
	void setState(ActionStateType state);

	/**
	 * 	@brief  Update TimeActionStarted attribute of ReportResult structure
	*/
	void setTimeActionStarted();

	/**
	 * 	@brief  Update TimeActionCompleted attribute of ReportResult structure
	*/
	void setTimeActionCompleted();

	/**
	 * 	@brief  Reset TimeActionCompleted attribute of ReportResult structure
	*/
	void resetTimeActionCompleted();

	/**
	 * 	@brief  Update TimeOfLastStatusUpdate attribute of ReportResult structure
	*/
	void setTimeOfLastStatusUpdate();

 private:

		/**
	 * 	@brief  Initialize internal data
	*/
	bool initInternalData();

	/**
	 * 	@brief  Update an attribute of ReportResult structure
	*/
	void setReportAttribute(char* attribute, const std::string& value);

	/**
	 * 	@brief  Update an attribute of ReportResult structure
	*/
	void setReportAttribute(char* attribute, int value);

	/**
	 * 	@brief  Update an attribute of ReportResult structure
	*/
	void setReportAttribute(char* attribute, uint32_t value);

	/**
	 * 	@brief  Get date and time string
	*/
	void getDateAndTime(std::string& curDateTime);

	/**
	 * 	@brief	m_ObjManager
	 */
	OmHandler* m_ObjManager;

	/**
		 * 	@brief	m_ObjManagerInitialized
	 */
	bool m_ObjManagerInitialized;


};

#endif /* ACS_CS_IMMOVEFUNCTIONPROGRESS_H_ */
