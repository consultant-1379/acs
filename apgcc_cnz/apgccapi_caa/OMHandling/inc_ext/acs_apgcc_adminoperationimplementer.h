/*
 * acs_apgcc_adminoperationimplementer.h
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATIONIMPLEMENTER_H_
#define ACS_APGCC_ADMINOPERATIONIMPLEMENTER_H_

#include "saImmOi.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"

class acs_apgcc_adminoperationimplementer_impl;

using namespace std;

/** @class acs_apgcc_adminoperationimplementer
 *	@brief acs_apgcc_adminoperationimplementer class
 *	@author
 *	@date 2011-13-07
 *	@version R1A
 *
 *	acs_apgcc_adminoperationimplementer This class
 *	acs_apgcc_adminoperationimplementer is intended to be used to implement the Admin Operation on IMM objects
 */

class acs_apgcc_adminoperationimplementer {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_adminoperationimplementer Default constructor
	 *
	 *	acs_apgcc_adminoperationimplementer Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_adminoperationimplementer();

	/** @brief acs_apgcc_adminoperationimplementer Copy constructor
	 *
	 *	acs_apgcc_adminoperationimplementer copy Constructor of class
	 *
	 *	@remarks Remarks
	 */

	acs_apgcc_adminoperationimplementer(acs_apgcc_adminoperationimplementer& admOperationImplementer);

	/** @brief acs_apgcc_adminoperationimplementer Destructor
	 *
	 *	acs_apgcc_adminoperationimplementer Destructor of class
	 *
	 *	@remarks Remarks
	 */

	virtual ~acs_apgcc_adminoperationimplementer();

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
	ACS_CC_ReturnType init(std::string p_implementerName);

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

	/**	@brief setClassImplemented method
	 *	setClassImplemented method: set the process as object implementer for a particulat IMM Class
	 *	setClassImplemented so that when an invoke admin operation is called on it, the callback specified extending the
	 *	setClassImplemented base class is called by IMM service.
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
	 * @brief finalize method
	 * finalize method: used to finalize interaction with IMM.
	 * The method clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer.
	 * After calling this method no other operation on IMM and that involves Object implementer could be performed.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	//ACS_CC_ReturnType finalize();

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
	 * @brief adminOperationResult method: this method has to be usd by the class expanding the base class to return
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

	//===================//
	// Virtual Functions //
	//===================//
	/**
	 * @brief adminOperationCallback method
	 * adminOperationCallback method: This is a virtual method to be implemented by the Designer when extending
	 * the base class. This method will be called as a callback to manage an administrative operation invoked, on the
	 * implemented object, using the adminOperationInvoke method of acs_apgcc_adminoperation class.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the administrative operation
	 * has to be managed.
	 *
	 * @param  oiHandle : input parameter,ACS_APGCC_OiHandle this value has to be used for returning the callback
	 *  result to IMM.
	 *
	 * @param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation
	 * of result function
	 *
	 * @param  p_objName: input parameter,the name of the object
	 *
	 * @param  operationId: input parameter, the administrative operation identifier
	 *
	 * @param paramList: a null terminated array of pointers to operation params elements. each element of the list
	 * is a pointer toACS_APGCC_AdminOperationParamType element holding the params provided to the Administretive operation..
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)=0;

private:

	acs_apgcc_adminoperationimplementer& operator=(const acs_apgcc_adminoperationimplementer &runOwner);

	acs_apgcc_adminoperationimplementer_impl* adminOperationImplementerImpl;


};

#endif /* ACS_APGCC_ADMINOPERATIONIMPLEMENTER_H_ */
