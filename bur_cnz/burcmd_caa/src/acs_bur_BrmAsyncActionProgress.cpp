/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2014
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file acs_bur_BrmAsyncActionProgress.cpp
 *
 * @brief
 * Maintains a loacal AsyncActionProgress structure
 *
 * @details
 * Creates a loacal AsyncActionProgress structure and copied the values into it
 * as exists in BrM Model.
 *
 * @author 
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 02/12/2011     EGIMARR       Initial Release
 * 10/02/2014     XVENJAM       Fix for TR HS28643
 ****************************************************************************/

#include "acs_bur_BrmAsyncActionProgress.h"
#include <ACS_TRA_trace.h>
#include "acs_bur_trace.h"

void acs_bur_BrmAsyncActionProgress::setOpCode(int code)
{
	// assign error code to opCode private attribute
	opCode = code;
}

void acs_bur_BrmAsyncActionProgress::setOpCode(int code,string functionName)
{
	// assign error code to opCode private attribute
	opCode = code;
	// assign function name to m_functionName private attribute
	m_functionName = functionName;
}

acs_bur_BrmAsyncActionProgress::acs_bur_BrmAsyncActionProgress(char *p)
{
	// Create a trace object for acs_bur_BrmAsyncActionProgress class
	aapTrace = new ACS_TRA_trace("acs_bur_BrmAsyncActionProgress");
	// Trace a message
	TRACE(aapTrace, "%s", "acs_bur_BrmAsyncActionProgress::Constructor()");
	// Create logger object
	burLog = new ACS_TRA_Logging();
	// Opens BUR logger
	burLog->Open("BUR");


	// Initialize id attribute
	id = "";
	// Initialize timeOfLastStatusUpdate attribute
	timeOfLastStatusUpdate = "";
	// Initialize timeActionCompleted attribute
	timeActionCompleted = "";
	// Initialize timeActionStarted attribute
	timeActionStarted = "";
	// Initialize actionId attribute
	actionId = 0;
	// Initialize state attribute
	state = 0;
	// Initialize result attribute
	result = 0;
	// Initialize resultInfo attribute
	resultInfo = "";
	// Initialize progressInfo attribute
	progressInfo = "";
	// Initialize progressPercentage attribute
	progressPercentage = 0;
	// Initialize additionalInfo attribute
	additionalInfo = "";
	// Initialize actionName attribute
	actionName = "";
	// Initialize opCode attributes
	opCode = 0;
	// Initialize m_functionName attributes
	m_functionName = "";

	// Define object for APGCC lib return code function
	ACS_CC_ReturnType res;

	// Create imm handler object to access at IMM
	OmHandler immHandle;

	// Define a buffer to work
	char *nameObject = p;

	// Set no error code for this instance
	setOpCode(NOERROR);

	// Initialize imm handler
	res = immHandle.Init();
	// Checks imm Initialize result
	if ( res != ACS_CC_SUCCESS ){
		// An error occured write into logger
		burLog->Write("acs_bur_BrmAsyncActionProgress: Error in Init() function",LOG_LEVEL_ERROR);
		// Set error code and function name
		setOpCode(INITERROR,"Init()");
		// exit
		return;
	}
	// Define the object attributes used as parameter
	ACS_APGCC_ImmAttribute attribute_1,attribute_2,attribute_3,attribute_4,attribute_5,attribute_6,attribute_7,attribute_8,attribute_9,attribute_10,attribute_11,attribute_12;

	// Define name for id field
	char attrdn[]= "id";
	// Define name for actionName field
	char attrActionName[]="actionName";
	// Define name for additionalInfo field
	char attrAdditionalInfo[]="additionalInfo";
	// Define name for progressInfo field
	char attrProgressInfo[]="progressInfo";
	// Define name for progressPercentage field
	char attrProgressPercentage[]="progressPercentage";
	// Define name for result field
	char attrResult[]="result";
	// Define name for resultInfo field
	char attrResultInfo[]="resultInfo";
	// Define name for state field
	char attrState[]="state";
	// Define name for actionId field
	char attrActionId[]="actionId";
	// Define name for timeActionStarted field
	char attrTimeActionStarted[]="timeActionStarted";
	// Define name for timeActionCompleted field
	char attrTimeActionCompleted[]="timeActionCompleted";
	// Define name for timeOfLastStatusUpdate field
	char attrTimeOfLastStatusUpdate[]="timeOfLastStatusUpdate";

	// assign dn name to attribute_1
	attribute_1.attrName = attrdn;
	// assign ActionName name to attribute_2
	attribute_2.attrName = attrActionName;
	// assign AdditionalInfo name to attribute_3
	attribute_3.attrName = attrAdditionalInfo;
	// assign ProgressInfo name to attribute_4
	attribute_4.attrName = attrProgressInfo;
	// assign ProgressPercentage name to attribute_5
	attribute_5.attrName = attrProgressPercentage;
	// assign Result name to attribute_6
	attribute_6.attrName = attrResult;
	// assign ResultInfo name to attribute_7
	attribute_7.attrName = attrResultInfo;
	// assign State name to attribute_8
	attribute_8.attrName = attrState;
	// assign ActionId name to attribute_9
	attribute_9.attrName = attrActionId;
	// assign TimeActionStarted name to attribute_10
	attribute_10.attrName = attrTimeActionStarted;
	// assign TimeActionCompleted name to attribute_11
	attribute_11.attrName = attrTimeActionCompleted;
	// assign TimeOfLastStatusUpdate name to attribute_12
	attribute_12.attrName = attrTimeOfLastStatusUpdate;

	// define container for all attributes
	std::vector<ACS_APGCC_ImmAttribute *> attributes;

	// Push attribute_1 into container
	attributes.push_back(&attribute_1);
	// Push attribute_2 into container
	attributes.push_back(&attribute_2);
	// Push attribute_3 into container
	attributes.push_back(&attribute_3);
	// Push attribute_4 into container
	attributes.push_back(&attribute_4);
	// Push attribute_5 into container
	attributes.push_back(&attribute_5);
	// Push attribute_6 into container
	attributes.push_back(&attribute_6);
	// Push attribute_7 into container
	attributes.push_back(&attribute_7);
	// Push attribute_8 into container
	attributes.push_back(&attribute_8);
	// Push attribute_9 into container
	attributes.push_back(&attribute_9);
	// Push attribute_10 into container
	attributes.push_back(&attribute_10);
	// Push attribute_11 into container
	attributes.push_back(&attribute_11);
	// Push attribute_12 into container
	attributes.push_back(&attribute_12);

	// Get all attribute values for DN=nameObject
	res = immHandle.getAttribute(nameObject, attributes);

	// Check operation result
	if ( res != ACS_CC_SUCCESS ){
		// Write an error to BUR logger
		burLog->Write("acs_bur_BrmAsyncActionProgress: Error in getAttribute() function",LOG_LEVEL_ERROR);
		// Set error code and function name
		setOpCode(GETATTRERROR,"getAttribute()");
		// release resource of imm handler
		immHandle.Finalize();
		return;
	}

	// Define index for attribute
	int i = 0;
	// Define a generic attribute name variable
	char attr_name[100];

	//Define a string of attribute name
	string strAttrName = "";

	// For every attribute into container
	while( i < (int)attributes.size() ){

		// retrieve attribute name string
		strAttrName = string(attributes[i]->attrName);

		// trace function name and attribute name
		TRACE(aapTrace, "%s %s", "acs_bur_BrmAsyncActionProgress::acs_bur_BrmAsyncActionProgress() read attribute ",strAttrName.c_str());

		// clear name attribute
		memset((void *)attr_name,(int)0,(size_t)100);

		// Copy from string to char structure
		strcpy( attr_name,strAttrName.c_str() );

		// Check if attribute value is modified
		if(attributes[i]->attrValuesNum != 0){		// TR HS28643
			// Test if attribute name is timeOfLastStatusUpdate
			if (strcmp(attr_name,"timeOfLastStatusUpdate")==0){
				// Assign timeOfLastStatusUpdate attribute
				timeOfLastStatusUpdate = string((const char *)(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is timeActionCompleted
			else if (strcmp(attr_name,"timeActionCompleted")==0){
				// Assign timeActionCompleted attribute
				timeActionCompleted = string(reinterpret_cast<char *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is timeActionStarted
			else if (strcmp(attr_name,"timeActionStarted")==0){
				// Assign timeActionStarted attribute
				timeActionStarted = string(reinterpret_cast<char *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is actionId
			else if (strcmp(attr_name,"actionId")==0){
				// Assign actionId attribute
				actionId = (* reinterpret_cast<int *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is state
			else if (strcmp(attr_name,"state")==0){
				// Assign state attribute
				state = *((int *)(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is resultInfo
			else if (strcmp(attr_name,"resultInfo")==0){
				// Assign resultInfo attribute
				resultInfo = string(reinterpret_cast<char *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is result
			else if (strcmp(attr_name,"result")==0){
				// Assign result attribute
				result = (* reinterpret_cast<int *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is progressInfo
			else if (strcmp(attr_name,"progressInfo")==0){
				// Assign progressInfo attribute
				progressInfo = string(reinterpret_cast<char *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is progressPercentage
			else if (strcmp(attr_name,"progressPercentage")==0){
				// Assign progressPercentage attribute
				progressPercentage = (* reinterpret_cast<int *>(attributes[i]->attrValues[0]));
			}
			// Test if attribute name is additionalInfo
			else if (strcmp(attr_name,"additionalInfo")==0){
				// Assign additionalInfo attribute
				for(int j = 0; j < attributes[i]->attrValuesNum; j++)
				{
					additionalInfo = additionalInfo + string(reinterpret_cast<char *>(attributes[i]->attrValues[j]));
				}
			}
			// Test if attribute name is actionName
			else if (strcmp(attr_name,"actionName")==0){
				// Assign actionName attribute
				actionName = (char *)(attributes[i]->attrValues[0]);
			}
			//if (reinterpret_cast<char *>(attributes[i]->attrValues[0]) != NULL)
			//	additionalInfo = string(reinterpret_cast<char *>(attributes[i]->attrValues[0]));
		}

		// Increment index for next parameter
		i++;
	}

	// Release resource for imm handler
	res = immHandle.Finalize();

	// Check operation result
	if ( res != ACS_CC_SUCCESS ){
		// Write error into BUR logger
		burLog->Write("acs_bur_BrmAsyncActionProgress: Error in Finalize() function",LOG_LEVEL_ERROR);
		// Set error code with function name
		setOpCode(FINALIZEERROR,"Finalize()");
		return ;
	}

}
//
acs_bur_BrmAsyncActionProgress::~acs_bur_BrmAsyncActionProgress(){
	// trace function
	TRACE(aapTrace, "%s ", "acs_bur_BrmAsyncActionProgress::~acs_bur_BrmAsyncActionProgress() ");
    // test if trace object is defined
	if (NULL != aapTrace)
    {
    	// release trace object
    	delete aapTrace;
    }
	// test if logger object is defined
    if (NULL != burLog)
    {
    	// release logger object
    	delete burLog;
    }
}

string acs_bur_BrmAsyncActionProgress::getActionName() const
{
	// retrieve actionName attribute value
	return actionName;
}

string	acs_bur_BrmAsyncActionProgress::getAdditionalInfo() const
{
	// retrieve additionalInfo attribute value
	return additionalInfo;
}

string acs_bur_BrmAsyncActionProgress::getProgressInfo() const
{
	// retrieve progressIfo attribute value
	return progressInfo;
}

int 	acs_bur_BrmAsyncActionProgress::getProgressPercentage() const
{
	// retrieve progressPercentage attribute value
	return progressPercentage;
}

int	acs_bur_BrmAsyncActionProgress::getResult() const
{
	// retrieve result attributes value
	return result;
}
int	acs_bur_BrmAsyncActionProgress::getState() const
{
	// retrieve state attributes value
	return state;
}
string	acs_bur_BrmAsyncActionProgress::getResultInfo() const
{
	// retrieve resultInfo attributes value
	return resultInfo;
}

int acs_bur_BrmAsyncActionProgress::getActionId() const
{
	// retrieve actionId attributes value
	return actionId;
}

string	acs_bur_BrmAsyncActionProgress::getTimeActionStarted() const
{
	// retrieve timeActionStarted attributes value
	return timeActionStarted;
}

string	acs_bur_BrmAsyncActionProgress::getTimeActionCompleted() const
{
	// retrieve timeActionCompleted attributes value
	return timeActionCompleted;
}

string	acs_bur_BrmAsyncActionProgress::getTimeOfLastStatusUpdate() const
{
	// retrieve timeOfLastStatusUpdate attributes value
	return timeOfLastStatusUpdate;
}

int	acs_bur_BrmAsyncActionProgress::getOpCode() const
{
	// retrieve opCode attributes value
	return opCode;
}

string	acs_bur_BrmAsyncActionProgress::getOpMessage() const
{
	// retrieve m_functionName attributes value
	return m_functionName;
}
