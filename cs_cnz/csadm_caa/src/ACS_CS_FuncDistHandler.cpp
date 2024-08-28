/*
 * * @file ACS_CS_FuncDistHandler.cpp
 *	@brief
 *	Class method implementation for ACS_CS_FuncDistHandler.
 *
 *  This module contains the implementation of class declared in
 *  the ACS_CS_FuncDistHandler.h module
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
 *
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_CS_FuncDistHandler.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImMoveFunctionProgress.h"
#include "ACS_CS_Slave.h"
#include "ACS_CS_ImFunction.h"
#include "ACS_CS_ImApg.h"

#include "ACS_CS_Trace.h"

#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include <sstream>

ACS_CS_Trace_TDEF(ACS_CS_FuncDistHandler_TRACE);

namespace moveFunctionParameter {
		const char function[] = "function";
		const char apgNode[] = "ap";

		const char ap1Name[] = "AP1";
		const char ap2Name[] = "AP2";
	};

namespace moveFunctionErrorMsg {
		const char MOVE_SUCCESS[] = "Function %1% moved on %2%.";
		const char AP_NOT_DEFINED[] = "AP named %1% does not exist.";
		const char FUNCTION_NOT_DEFINED[] = "Function %1% is not defined.";
		const char FUNCTION_ALREADY_MOVEDON[] = "Function %1% is already on %2%.";
		const char GENERAL_ERROR[] = "An error occurred when executing the action.";
	};


namespace  {
		const char defaultFunction[] = "CHS";
		const char defaultAP1[] = "AP1";
		const char defaultAP2[] = "AP2";
	};

ACS_CS_FuncDistHandler::ACS_CS_FuncDistHandler()
: m_operationOngoing(false),
  m_functionName(),
  m_apName(),
  m_FunctionObject(NULL),
  m_ReportResult(NULL),
  m_CreateThreadRunning(false),
  m_mutex()
{


}

ACS_CS_FuncDistHandler::~ACS_CS_FuncDistHandler()
{
	if(m_CreateThreadRunning)
	{
		// Create Thread running, signal to stop
		m_CreateThreadRunning = false;
		// Wait to be sure that it is terminated
		usleep(500000);
	}
}

int ACS_CS_FuncDistHandler::executeActionMoveFunction(const unsigned int& functionIndex, const std::string& apName)
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	int result = SA_AIS_ERR_BUSY;
	// lock before check internal flag
	ACE_GUARD_RETURN(ACE_RW_Thread_Mutex, guard, m_mutex, result);

	if(!m_operationOngoing)
	{
		// No action ongoing
		ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d], starting svc thread", __FUNCTION__, __LINE__ ));

		m_ReportResult = new (std::nothrow) ACS_CS_ImMoveFunctionProgress();

		if(NULL != m_ReportResult)
		{
			m_apName = apName;
			functionIndexToFunctionName(functionIndex);
			// svc thread start
			if(activate( (THR_NEW_LWP| THR_DETACHED | THR_INHERIT_SCHED)) == 0)
			{
				result = SA_AIS_OK;
				// Set operation ongoing
				m_operationOngoing = true;
			}
		}
	}

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d], result:<%d>", __FUNCTION__, __LINE__, result ));
	return result;
}

int ACS_CS_FuncDistHandler::svc()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	uint32_t percentage = 0U;
	m_ReportResult->setProgressPercentage(percentage);
	m_ReportResult->setState(ACS_CS_ImMoveFunctionProgress::RUNNING);
	m_ReportResult->setResult(ACS_CS_ImMoveFunctionProgress::NOT_AVAILABLE);
	m_ReportResult->setTimeActionStarted();
	m_ReportResult->resetTimeActionCompleted();

	// Validate action parameters
	if(validateParameters())
	{
		// create a model object to handle creation and deletion of
		// function object into IMM
		ACS_CS_ImModel* modelChanges = new (std::nothrow) ACS_CS_ImModel();

		// Set Object to be deleted from IMM
		m_FunctionObject->setToBeDelete();

		// Add object to the model object
		modelChanges->addObject(m_FunctionObject);

		ACS_CS_ImFunction* newFunctionObject = dynamic_cast<ACS_CS_ImFunction*>( m_FunctionObject->clone());

		// Change its parent to the new one
		newFunctionObject->setParentDn(m_apName);
		// Set Object to be created in IMM
		newFunctionObject->setToBeCreate();

		ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d], move to ap:<%s>", __FUNCTION__, __LINE__, m_apName.c_str() ));

		// Add object to the model object
		modelChanges->addObject(newFunctionObject);

		ACS_CS_ImModelSaver modelSaver(modelChanges);

		// Apply all Changes in one transaction
		if( modelSaver.save("FuncDistMove") == ACS_CC_SUCCESS)
		{
			ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d], function moved", __FUNCTION__, __LINE__ ));
			percentage = 100U;
			m_ReportResult->setProgressPercentage(percentage);
			m_ReportResult->setResult(ACS_CS_ImMoveFunctionProgress::SUCCESS);
			// Assemble the formated error message
			boost::format successMsg(moveFunctionErrorMsg::MOVE_SUCCESS);
			successMsg.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
			successMsg % m_functionName % m_apName;

			m_ReportResult->setResultInfo(successMsg.str());
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d], function move failed", __FUNCTION__, __LINE__ ));
			m_ReportResult->setResult(ACS_CS_ImMoveFunctionProgress::FAILURE);
			m_ReportResult->setResultInfo(moveFunctionErrorMsg::GENERAL_ERROR);
		}

		m_ReportResult->setState(ACS_CS_ImMoveFunctionProgress::FINISHED);

		// Delete all added objects
		delete modelChanges;
	}


	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d]", __FUNCTION__, __LINE__ ));
	return 0;
}

int ACS_CS_FuncDistHandler::close(u_long flags)
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	// Clean-up data
	// lock before change internal flag
	ACE_Guard<ACE_RW_Thread_Mutex> flagLock(m_mutex);
	// Set operation completed
	m_operationOngoing = false;

	if(NULL != m_ReportResult)
	{
		m_ReportResult->setTimeActionCompleted();
		delete m_ReportResult;
		m_ReportResult = NULL;
	}

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d]", __FUNCTION__, __LINE__ ));
	return 0;
}

void ACS_CS_FuncDistHandler::initFunctionDistribution()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	if( !ACS_CS_ImMoveFunctionProgress::createProgressReportObject() )
	{
		ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d] Function Distribution init failed.", __FUNCTION__, __LINE__ ));
	}

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_FuncDistHandler::createAPobject()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	if( !createIMMObject() )
	{
		// Error on AP object creation
		// Schedule a thread to repeat it
		boost::thread createAPThread;
		boost::thread(&ACS_CS_FuncDistHandler::createAPObjectThread, this);
		createAPThread.detach();
	}

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d]", __FUNCTION__, __LINE__ ));

}

bool ACS_CS_FuncDistHandler::validateParameters()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	bool result = isAPGDefined() && isFunctionMoveableTo();

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d], parameters are <%s>", __FUNCTION__, __LINE__, (result ? "OK":"NOT OK") ));
	return result;
}

bool ACS_CS_FuncDistHandler::isAPGDefined()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool apgFound = false;

	// Get all defined APG Nodes
	std::set<const ACS_CS_ImBase*> apgNodeObjects;
	ACS_CS_ImRepository::instance()->getModel()->getObjects(apgNodeObjects, APGNODE_T);

	std::set<const ACS_CS_ImBase*>::const_iterator apgIter;

	// Loop through APG objects defined
	for(apgIter = apgNodeObjects.begin(); apgIter != apgNodeObjects.end(); ++apgIter)
	{
		std::string apgName;
		const ACS_CS_ImApg* apgObj = dynamic_cast<const ACS_CS_ImApg*>(*apgIter);

		// Check APG name
		if( (0 != apgObj) && apgObj->getAPGNodeName(apgName) )
		{
			if( m_apName.compare(apgName) == 0 )
			{
				// APG name has been defined
				apgFound = true;
				break;
			}
		}
	}

	if(!apgFound)
	{
		m_ReportResult->setResult(ACS_CS_ImMoveFunctionProgress::FAILURE);
		m_ReportResult->setState(ACS_CS_ImMoveFunctionProgress::FINISHED);

		// Assemble the formated error message
		boost::format errorMsg(moveFunctionErrorMsg::AP_NOT_DEFINED);
		errorMsg.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );

		errorMsg % m_apName;

		m_ReportResult->setResultInfo(errorMsg.str());
	}

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d], apg<%s> <%s>", __FUNCTION__, __LINE__, m_apName.c_str(), (apgFound ? "FOUND":"NOT FOUND") ));
	return apgFound;
}

bool ACS_CS_FuncDistHandler::isFunctionMoveableTo()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool functionMoveable = false;
	bool functionFound = false;
	// Get all defined Function
	std::set<const ACS_CS_ImBase*> functionObjects;
	ACS_CS_ImRepository::instance()->getModel()->getObjects(functionObjects, APFUNCTION_T);

	std::set<const ACS_CS_ImBase*>::const_iterator funcIter;
	// Loop through Function objects
	for(funcIter = functionObjects.begin(); funcIter != functionObjects.end(); ++funcIter)
	{
		const ACS_CS_ImFunction* functionObj = dynamic_cast<const ACS_CS_ImFunction*>(*funcIter);

		std::string serviceName;
		// Get service name
		if( (0 != functionObj) && functionObj->getServiceName(serviceName) )
		{
			// Function to move found
			ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d] function:<%s> defined", __FUNCTION__, __LINE__, serviceName.c_str() ));

			int comp = m_functionName.compare(serviceName);
			ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d] m_function:<%s, %d> , service:<%s, %d>, comp:<%d>", __FUNCTION__, __LINE__, m_functionName.c_str(), m_functionName.length(), serviceName.c_str(), serviceName.length(), comp ));

			// Check if equal
			if( m_functionName.compare(serviceName) == 0)
			{
				// Function to move found
				ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d] function:<%s> found", __FUNCTION__, __LINE__, m_functionName.c_str() ));

				functionFound = true;

				std::string apName;
				// Check if already moved on
				if( functionObj->getAPName(apName)
						&& (m_apName.compare(apName) != 0) )
				{
					// Now, It is on another APG, so it is moveable
					functionMoveable = true;
					// Get copy of object to move
					m_FunctionObject = dynamic_cast<ACS_CS_ImFunction*>(functionObj->clone());
				}
				else
				{
					ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "[%s@%d] function:<%s> already on APG:<%s>", __FUNCTION__, __LINE__, m_functionName.c_str(), apName.c_str() ));

					m_ReportResult->setResult(ACS_CS_ImMoveFunctionProgress::FAILURE);
					m_ReportResult->setState(ACS_CS_ImMoveFunctionProgress::FINISHED);

					// Assemble the formated error message
					boost::format errorMsg(moveFunctionErrorMsg::FUNCTION_ALREADY_MOVEDON);
					errorMsg.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );

					errorMsg % m_functionName % m_apName;

					m_ReportResult->setResultInfo(errorMsg.str());
				}

				break;
			}
		}
	}

	if(!functionFound)
	{
		m_ReportResult->setResult(ACS_CS_ImMoveFunctionProgress::FAILURE);
		m_ReportResult->setState(ACS_CS_ImMoveFunctionProgress::FINISHED);
		// Assemble the formated error message
		boost::format errorMsg(moveFunctionErrorMsg::FUNCTION_NOT_DEFINED);
		errorMsg.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );

		errorMsg % m_functionName;

		m_ReportResult->setResultInfo(errorMsg.str());
	}

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d], function:<%s> is <%s>", __FUNCTION__, __LINE__, m_functionName.c_str(), (functionMoveable ? "MOVEABLE":"NOT MOVEABLE") ));
	return functionMoveable;
}

void ACS_CS_FuncDistHandler::functionIndexToFunctionName(const unsigned int& functionIndex)
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	std::ostringstream tmpFunctionName;

	tmpFunctionName << defaultFunction ;

	if(functionIndex > 0)
		tmpFunctionName << functionIndex ;

	m_functionName = tmpFunctionName.str();

	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d], function:<%d> is <%s>", __FUNCTION__, __LINE__, functionIndex, m_functionName.c_str() ));
}

void ACS_CS_FuncDistHandler::createAPObjectThread()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool createResult;
	m_CreateThreadRunning = true;
	do
	{
		// wait before re-try
		usleep(500000);

		if (ACS_CS_Slave::isReadyForRequest())
		{
			// Create apgId=AP2 object
			createResult = createIMMObject();
		}

	}while(!createResult && m_CreateThreadRunning);

	m_CreateThreadRunning = false;
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Leaving in [%s@%d]", __FUNCTION__, __LINE__ ));
}


bool ACS_CS_FuncDistHandler::createIMMObject()
{
	ACS_CS_TRACE((ACS_CS_FuncDistHandler_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool result = false;

	OmHandler objManager;
	if( ACS_CC_SUCCESS == objManager.Init(REGISTERED_OI))
	{
		std::vector<ACS_CC_ValuesDefinitionType> objAttrList;

		const unsigned int bufferSize = 255;

		char tmpApgId[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(tmpApgId, bufferSize, "%s", ACS_CS_ImmMapper::ATTR_APGNODE_ID.c_str());

		char tmpRDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(tmpRDN, bufferSize, "%s=%s", tmpApgId, defaultAP2);


		// Fill the RDN attribute fields
		ACS_CC_ValuesDefinitionType attributeRDN;
		attributeRDN.attrName = tmpApgId;
		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;
		void* tmpValueRDN[1] = { reinterpret_cast<void*>(tmpRDN) };
		attributeRDN.attrValues = tmpValueRDN;

		objAttrList.push_back(attributeRDN);

		char tmpParantRDN[bufferSize] = {0};
		// Assemble the parent RDN value
		ACE_OS::snprintf(tmpParantRDN, bufferSize, "%s", ACS_CS_ImmMapper::RDN_FUNCTION_DIST.c_str());

		ACS_CC_ReturnType getResult = objManager.createObject(ACS_CS_ImmMapper::CLASS_APGNODE.c_str(), tmpParantRDN, objAttrList);
		result = true;

		if( ACS_CC_FAILURE == getResult)
		{
			int error = objManager.getInternalLastError();
			if(-14 != error)
			{
				result = false;
				ACS_CS_FTRACE((ACS_CS_FuncDistHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d], <%s> creation fails with error:<%d>", __FUNCTION__, __LINE__, tmpRDN, error ));
			}
		}

		objManager.Finalize();
	}

	ACS_CS_FTRACE((ACS_CS_FuncDistHandler_TRACE,LOG_LEVEL_WARN, "Leaving in [%s@%d], result<%s>", __FUNCTION__, __LINE__, (result ? "TRUE":"FALSE") ));
	return result;
}


void ACS_CS_FuncDistHandler::stopThread()
{
	m_CreateThreadRunning = false;
}
