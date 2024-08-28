/*
 * acs_apgcc_adminoperationimplementer_impl.h
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATIONIMPLEMENTER_IMPL_H_
#define ACS_APGCC_ADMINOPERATIONIMPLEMENTER_IMPL_H_

#include <map>
#include <vector>
#include <stdlib.h>
#include <iostream>

#include "ace/OS.h"
#include "saImmOi.h"
#include "ACS_CC_Types.h"

#include "ACS_TRA_Logging.h"
#include "acs_apgcc_adminoperationtypes.h"

using namespace std;

class acs_apgcc_adminoperationimplementer;

class acs_apgcc_adminoperationimplementer_impl {
public:
	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_adminoperationimplementer_impl Default constructor
	 *
	 *	acs_apgcc_adminoperationimplementer_impl Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_adminoperationimplementer_impl();

	/** @brief acs_apgcc_adminoperationimplementer_impl Copy constructor
	 *
	 *	acs_apgcc_adminoperationimplementer_impl Copy Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_adminoperationimplementer_impl(acs_apgcc_adminoperationimplementer_impl *adminOperationImpl);

	//============//
	// Destructor //
	//============//

	virtual ~acs_apgcc_adminoperationimplementer_impl();

	//===========//
	// Functions //
	//===========//

	/**	@brief init method
	 *	init method: perform initialization of IMM interaction.
	 *	Initialize the IMM connection and registers the invoking process as Object implementer
	 *	having the name which is specified in the implementerName parameter.
	 *
	 *  @param p_implementerName the name of the Object Implementer
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType init(std::string impName, acs_apgcc_adminoperationimplementer *p_adminOperationImpl);

	/**	@brief setObjectImplemented method
	 *	setObjectImplemented method: set the process as object implementer for a particulat IMM Object
	 *	setObjectImplemented so that when an invoke admin operation is called on it, the callback specified extending the
	 *	setObjectImplemented base class is called by IMM service.
	 *
	 *  @param p_objName the name of the Object to be implemented. When an application invokes an
	 *  administrative operation on this object, the callback specified in the virtual method adminOperationCallback ()
	 *  will be called.
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType setObjectImplemented(std::string p_objName);

	/**	@brief setObjectImplemented method
	 *	setObjectImplemented method: set the process as object implementer for a particulat IMM Object
	 *	setObjectImplemented so that when an invoke admin operation is called on it, the callback specified extending the
	 *	setObjectImplemented base class is called by IMM service.
	 *
	 *  @param p_className the name of the Class to be implemented. When an application invokes an
	 *  administrative operation on an object instance of this IMM  class object, the callback specified in the virtual method adminOperationCallback ()
	 *  will be called.
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType setClassImplemented(std::string p_className);

	/**
	 * @brief dispatch method
	 * dispatch method: This method is used to dispatch callbacks on the Admin Operation Implementer
	 *
	 * @param  p_flag:			flag that specify the callback execution behavior of the dispatcher.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType dispatch(ACS_APGCC_DispatchFlags p_flag);


	/**
	 * @brief finalize method
	 * finalize method: used to finalize interaction with IMM.
	 * The method clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer.
	 * After calling this method no other operation on IMM and that involves Object implementer could be performed.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	//ACS_CC_ReturnType finalize();

	/**	@brief releaseObjectImplemented method
	 *	releaseObjectImplemented method: release the process as object implementer for a particular IMM Class
	 *	releaseObjectImplemented so that when an invoke admin operation is called on it, the callback specified extending the
	 *	releaseObjectImplemented base class is no more called by IMM service.
	 *
	 *  @param p_objName the name of the Object to be released as object implemented.
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType releaseObjectImplemented(std::string p_objName);

	/**	@brief releaseClassImplemented method
	 *	releaseClassImplemented method: release the process as object implementer for a particulat IMM Object
	 *	releaseClassImplemented so that when an invoke admin operation is called on it, the callback specified extending the
	 *	releaseClassImplemented base class is called by IMM service.
	 *
	 *  @param p_className the name of the Class to be implemented. When an application invokes an
	 *  administrative operation on an object instance of this IMM  class object, the callback specified in the virtual method adminOperationCallback ()
	 *  will be called.
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType releaseClassImplemented(std::string p_className);

	/**
	 * @brief getSelObj method
	 * getSelObj method: This method return the operating system handle that the invoking process
	 * can use to detect pending callbacks
	 *
	 * @return int. The operating system handle to be used to dispatch pending callbacks
	 */
	int getSelObj() const ;

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
	 * @brief adminOperationResult method: this method has to be used by the class expanding the base class to return
	 * the result of administrative operation to IMM.
	 *
	 * @param oiHandle: input param the oi handle
	 *
	 * @param invocation: input param the invocation
	 *
	 * @param result : input param the result to be passed to IMM as administrative operation result.
	 *
	 * @return ACS_CC_ReturnType . On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType  adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result);


private:

	/**
	 * @brief finalize method
	 * finalize method: used to finalize interaction with IMM.
	 * The method clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer.
	 * After calling this method no other operation on IMM and that involves Object implementer could be performed.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType finalize();




	static void adminOperationCallback(SaImmOiHandleT immOiHandle, SaInvocationT invocation,
			const SaNameT *objectName,
			SaImmAdminOperationIdT operationId,
			const SaImmAdminOperationParamsT_2 **params);

	/**
	 * setImmError method: this method is used to set the error returned by ImmService
	 *
	 * @param p_errorCode :	integer that represents the error
	 */
	void setInternalError(int p_errorCode);

	/**Attributes**/
	static map<SaUint64T, acs_apgcc_adminoperationimplementer *> implementerList;

	acs_apgcc_adminoperationimplementer *runtimeOwn;

	ACS_APGCC_OiHandle immOiHandle;

	SaSelectionObjectT selObj;

	ACS_TRA_Logging log;

	int errorCode;




};

#endif /* ACS_APGCC_ADMINOPERATIONIMPLEMENTER_IMPL_H_ */
