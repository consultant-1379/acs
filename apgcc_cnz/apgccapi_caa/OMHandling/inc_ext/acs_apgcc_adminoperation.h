/*
 * ACS_APGCC_AdminOperation.h
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATION_H_
#define ACS_APGCC_ADMINOPERATION_H_

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include "ACS_CC_Types.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include "ace/OS.h"
#include "ACS_TRA_Logging.h"
#include "acs_apgcc_adminoperationtypes.h"


using namespace std;

class acs_apgcc_adminoperation_impl;

/** @class acs_apgcc_adminoperation acs_apgcc_adminoperation.h
 *	@brief acs_apgcc_adminoperation class provides IMM Administrative Operations
 *	@brief interface
 *	@author xpaomaz
 *	@date 2011-11-07
 *	@version R1A
 *
 *	acs_apgcc_adminoperation Class detailed description: this class can be used by APG application to
 *	acs_apgcc_adminoperation to call IMM Administrative Operations.
 */
class acs_apgcc_adminoperation {
public:
	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_adminoperation Default constructor
	 *
	 *	acs_apgcc_adminoperation Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_adminoperation();

	/** @brief acs_apgcc_adminoperation Copy constructor
	 *
	 *	acs_apgcc_adminoperation Copy Constructor of class
	 *
	 *	@remarks Remarks
	 */

	acs_apgcc_adminoperation(acs_apgcc_adminoperation& objRef);

	/** @brief acs_apgcc_adminoperation distructor
	 *
	 *	acs_apgcc_adminoperation distructor of class
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_apgcc_adminoperation();

	/**
	 *
	 * Init method: used to Initialize Interaction between the calling process and IMM.
	 * Each application that intends to use acs_apgcc_adminoperation class must call this method before performing
	 * any action.
     * @param : none
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType init ( );


	/**
	 * Finalize method: used to finalize interaction with IMM and release all handler to it.
	 * After calling this method no other operation on IMM could be performed using acs_apgcc_adminoperation.
	 * No parameter required
	 */
	ACS_CC_ReturnType finalize ();

	/**
	 *
	 * adminOperationInvoke: used to invoke an Administrative Operation on a particular IMM object.
     * @param[IN] : p_objName the name of the object on which the Administrative Operation is called
     * @param[IN] : continuationId the continuation ID.
     * @param[IN] : operationId the identifier of the Administritive operation.
     * @param[IN] : a vector of acs_apgcc_adminoperationParamType  to be used as parameters for the Administrative
     * operation.
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType adminOperationInvoke ( const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector, int* returnVal, long long int timeoutVal);

	/**
	 * getInternalLastError method: this method returns the error code of the last error occurred
	 *
	 * @return int. the error code of the last error occurred
	 */
	int getInternalLastError();


	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return char*. the message error of the last error occurred
	 */
	char* getInternalLastErrorText();

	/**
	 *
	 * adminOperationInvoke: used to invoke an Administrative Operation on a particular IMM object.
     * @param[IN] : p_objName the name of the object on which the Administrative Operation is called
     * @param[IN] : continuationId the continuation ID.
     * @param[IN] : operationId the identifier of the Administritive operation.
     * @param[IN] : a vector of acs_apgcc_adminoperationParamType  to be used as parameters for the Administrative
     * @param[OUT] : the result of the Administrative Operation
     * @param[OUT] : a vector of acs_apgcc_adminoperationParamType  to be used as parameters output for the Administrative
     * operation.
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType adminOperationInvoke ( const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector, int* returnVal, long long int timeoutVal, vector<ACS_APGCC_AdminOperationParamType> &outparamVector);

	/**
	 *
	 * adminOperationInvoke: used to invoke an Administrative Operation on a particular IMM object.
     * @param[OUT] : a vector of acs_apgcc_adminoperationParamType  to be used as parameters output for the Administrative
     * operation.
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType freeadminOperationReturnValue ( vector<ACS_APGCC_AdminOperationParamType> &outparamVector );


private:


	acs_apgcc_adminoperation& operator=(const acs_apgcc_adminoperation &adminOperation);

	acs_apgcc_adminoperation_impl *adminoperation_impl;

};

#endif /* ACS_APGCC_ADMINOPERATION_H_ */
