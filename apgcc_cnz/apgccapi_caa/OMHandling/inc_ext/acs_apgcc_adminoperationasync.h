/*
 * acs_apgcc_adminoperationasync.h
 *
 *  Created on: Jul 10, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATIONASYNC_H_
#define ACS_APGCC_ADMINOPERATIONASYNC_H_

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

class acs_apgcc_adminoperationasync_impl;

using namespace std;



/** @class acs_apgcc_adminoperationasync acs_apgcc_adminoperationasync.h
 *	@brief acs_apgcc_adminoperationasync class provides IMM Administrative Operations Async implementation
 *	@brief interface
 *	@author xpaomaz
 *	@date 2011-11-07
 *	@version R1A
 *
 *	acs_apgcc_adminoperationasync Class detailed description: this class can be used by APG applications to
 *	acs_apgcc_adminoperationasync to call Asycronous IMM Administrative Operations.
 */

class acs_apgcc_adminoperationasync {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_adminoperationasync Default constructor
	 *
	 *	acs_apgcc_adminoperationasync Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_adminoperationasync();

	/** @brief acs_apgcc_adminoperationasync Copy constructor
	 *
	 *	acs_apgcc_adminoperationasync Copy Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_adminoperationasync(acs_apgcc_adminoperationasync& objRef);

	/** @brief acs_apgcc_adminoperationasync distructor
	 *
	 *	acs_apgcc_adminoperation distructor of class
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_apgcc_adminoperationasync();


	/**
	 *
	 * Init method: used to Initialize Interaction between the calling process and IMM.
	 * Each application that intends to use acs_apgcc_adminoperationasync class must call this method before performing
	 * any action.
     * @param : none
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType init ( );


	/**
	 * Finalize method: used to finalize interaction with IMM and release all handler to it.
	 * After calling this method no other operation on IMM could be performed using acs_apgcc_adminoperationasync.
	 * No parameter required
	 */
	ACS_CC_ReturnType finalize ();

	/**
	 *
	 * adminOperationInvokeAsync: used to invoke an Administrative Operation Asynchronous on a particular IMM object.
	 * It returns as soon as the IMM Service has registered the request for the Administrative Operation to be transmitted
	 * to the Administrative Operation Implementer.
	 * If IMM detects an error while registering the request, an error is
	 * returned and no further invocation of the callback (notifying the admin operatin execution) must be expected for this invocation.
	 * If no error is detected by IMM service while registering the request, the adminOperationInvokeAsync() invocation completes successfully , and
	 * later, invocation of the callback will indicate the success or failure of the requested admin operation on the target object.
	 * @param[IN] : invocation the name invocation identifier used to match the  invocation of adminOperationInvokeAsync with the corresponding invocation
	 *  of the related callback.
     * @param[IN] : p_objName the name of the object on which the Administrative Operation is called
     * @param[IN] : continuationId the continuation ID.
     * @param[IN] : operationId the identifier of the Administritive operation.
     * @param[IN] : a vector of acs_apgcc_adminoperationParamType  to be used as parameters for the Administrative
     * operation.
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 * If IMM detects an error while registering the request, an error is
	 * returned and no further invocation of the callback (notifying the admin operatin execution) must be expected for this invocation.
	 * If no error is detected by IMM service while registering the request, the adminOperationInvokeAsync() invocation completes successfully , and
	 * later, invocation of the callback will indicate the success or failure of the requested admin operation on the target object.
	 */
	ACS_CC_ReturnType adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation ,const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector);

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
	 * @brief dispatch method
	 * dispatch method: This method is used to dispatch callbacks on the Object Manager
	 *
	 * @param  p_flag:			flag that specify the callback execution behavior of the dispatcher.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType dispatch(ACS_APGCC_DispatchFlags p_flag);

	/**
	 * @brief getSelObj method
	 * getSelObj method: This method return the operating system handle that the invoking process
	 * can use to detect pending callbacks
	 *
	 * @return int. The operating system handle to be used to dispatch pending callbacks
	 */
	int getSelObj() const ;

	//===================//
	// Virtual Functions //
	//===================//
	/**
	 * @brief objectManagerAdminOperationCallback method
	 * objectManagerAdminOperationCallback method: This is a virtual method to be implemented by the Designer when extending
	 * the base class. This method will be called as a callback to notify an asynchronous administrative operation execution.
	 *
	 * @param invocation : the invocation Id used by the application calling the Admin Operation Asynchronous
	 *
	 * @param returnVal : the return value. This value is meaningful only if the error param is has value 1, in this case
	 * returnVal value indicates the value returned by the Admin Operation Implementer for the Admin Operation requested.
	 * this value is specific to the AdminOperation being performed and is valid only if error param is has value 1.
	 *
	 * @param error: indicates if the IMM Service succeded or not to invoke the admin Operation implementer.
	 *

	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	virtual void objectManagerAdminOperationCallback( ACS_APGCC_InvocationType invocation, int returnVal , int error )=0;



private:


	acs_apgcc_adminoperationasync& operator=(const acs_apgcc_adminoperationasync &adminOperationAsync);

	acs_apgcc_adminoperationasync_impl *adminoperationAsync_impl;


};

#endif /* ACS_APGCC_ADMINOPERATIONASYNC_H_ */
