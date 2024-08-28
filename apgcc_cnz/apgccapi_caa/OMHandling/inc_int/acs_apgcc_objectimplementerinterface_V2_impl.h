/*
 * acs_apgcc_objectimplementerinterface_V2_impl.h
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_V2_IMPL_H_
#define ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_V2_IMPL_H_
#include "ACS_CC_Types.h"
#include "saImmOi.h"
#include "string"

#include "ace/OS.h"
//#include "ACS_CC_Types.h"
#include "ACS_TRA_Logging.h"
#include <sstream>
#include "acs_apgcc_adminoperationtypes.h"

using namespace std;

/** @class acs_apgcc_objectimplementerinterface_impl acs_apgcc_objectimplementerinterface_impl.h
 *	@brief acs_apgcc_objectimplementerinterface_impl class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-13
 *	@version R1A
 *
 *	acs_apgcc_objectimplementerinterface_impl Class detailed description
 */

class acs_apgcc_objectimplementerinterface_V2_impl {
	//=========//
	// Friends //
	//=========//
	friend class acs_apgcc_oihandler_V2_impl;

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_objectimplementerinterface_impl Default constructor
	 *
	 *	ACS_APGCC_ObjectImplementerInterface Constructor
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_objectimplementerinterface_V2_impl();


	/** @brief acs_apgcc_objectimplementerinterface_V2_impl constructor
	 *
	 *	acs_apgcc_objectimplementerinterface_V2_impl Constructor this constructor is used to define a Object Implementer
	 *	for a class
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 */
	acs_apgcc_objectimplementerinterface_V2_impl(string p_impName );


	/** @brief acs_apgcc_objectimplementerinterface_V2_impl constructor
	 *
	 *	acs_apgcc_objectimplementerinterface_V2_impl Constructor this constructor is used to define a Object Implementer
	 *	for a object.
	 *
	 *	@param p_objName Name of the Object
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 *	@param p_scope Scope of operation. The value of this parameter can be:
	 *					ACS_APGCC_ONE: 		the scope of operation is the object designed by name to which
	 *								  		p_objName point.
	 *					ACS_APGCC_SUBLEVEL: the scope of the operation is the object designed by name to which
	 *										p_objName point and its direct children
	 *					ACS_APGCC_SUBTREE: 	the scope of the operation is the object designed by name to which
	 *										objName point and the entire subtree rooted at that object
	 *
	 */
	acs_apgcc_objectimplementerinterface_V2_impl(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );


	/** @brief acs_apgcc_objectimplementerinterface_V2_impl Destructor
	 *
	 *	acs_apgcc_objectimplementerinterface_V2_impl Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_apgcc_objectimplementerinterface_V2_impl();


	//===========//
	// Functions //
	//===========//
	/**	@brief getObjName method
	 *	getObjName method: return the object name for which has been defined the Object Implementer .
	 *
	 *  No parameter required
	 *
	 *	@return std::string. the object name
	 *
	 */
	std::string getObjName();


	/**	@brief setObjName method
	 *	setObjName method: set the object name for which are defining the Object Implementer .
	 *
	 *  @param p_objName Name of the Object
	 *
	 */
	void setObjName(std::string p_objName);


	/**	@brief getImpName method
	 *	getImpName method: return the implementer name.
	 *
	 *  No parameter required
	 *
	 *	@return std::string. the implementer name.
	 *
	 */
	std::string getImpName();


	/**	@brief setImpName method
	 *	setImpName method: set the name of implementer.
	 *
	 *  @param p_impName Name of the Implementer
	 *
	 */
	void setImpName(std::string p_impName);


	/**	@brief getScope method
	 *	getScope method: return the scope of the operation for which the Object Implementer is defined.
	 *
	 *  No parameter required
	 *
	 *	@return ACS_APGCC_ScopeT scope of operation.
	 *
	 */
	ACS_APGCC_ScopeT getScope();


	/**	@brief setScope method
	 *	setScope method: set the scope of the operation for which the Object Implementer is defined.
	 *
	 *  @param p_scope scope of the operation
	 *
	 */
	void setScope(ACS_APGCC_ScopeT p_scope);


	/**	@brief getSelObj method
	 *	getSelObj method: return the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *  No parameter required
	 *
	 *	@return int the operating system handle.
	 *
	 */
	int getSelObj() const ;


	/**	@brief dispatch method
	 *	dispatch method: return the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *  @param p_flag: flags that specify the callback execution behavior of the dispatcher.
	 *  The value of this parameter can be:
	 *  	ACS_APGCC_DISPATCH_ONE
	 *		ACS_APGCC_DISPATCH_ALL
	 *		ACS_APGCC_DISPATCH_BLOCKING
	 *
	 *	@return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 */
	ACS_CC_ReturnType dispatch(ACS_APGCC_DispatchFlags p_flag);


	/**
	 * @brief modifyRuntimeObj method
	 * modifyRuntimeObj method: this method is used to modify the value of a runtime attribute belonging
	 * to a configuration object.
	 *
	 *  @param  p_objName: the full DN of the object containing the value to be retrieved
	 *
	 *  @param  p_attributeValue: the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue);

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



	/**	@brief setImmOiHandle method
	 *	setImmOiHandle method: set the immOiHandle with the value obtained by a IMM service.
	 *	This value is used also to identify a particular object Implementer in the static map that contain the
	 *	list of all object implementer
	 *
	 *  @param p_immOiHandle handle that identifies a particular initialization of the IMM Service
	 *
	 */
	void setImmOiHandle(SaUint64T p_immOiHandle);


	/**	@brief setSelObj method
	 *	setSelObj method: set the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *  @param p_selObj the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 */
	void setSelObj(SaSelectionObjectT p_selObj);


	SaUint64T getImmOiHandle();

	// the following lines added  for new error code communication between OI and OM. OI_OM_ERROR_COM
	/**	@brief setExitCode method
	 *
	 *  setExitCode method: method to be used to set exitCode for Commands acting as Object implementer.
	 *  This method has to be called by commands acting as OI before returning a call-back because of error
	 *  providing as input parameter the p_exitCode and the p_exitCodeMessage that are respectively the error code
	 *  and the related error message to be reported to OM using the CMW functions.
	 *
	 *  @param p_exitCode        : the exit code of the command
	 *
	 *  @param p_exitCodeMessage : the Error Message string of the command
	 *
	 *  @return void.
	 */

	void setExitCode(int p_exitCode, string p_exitCodeMessage);

	/**	@brief getExitCodeString method
	 * getExitCode method: method to be used to get exitCode String, in case of error, for Commands acting
	 * as Object implementer.
	 * the exit code is stored in exitCodeString class variable to be available for acs_apgcc_oihandler
	 * class to be passed to CMW API.
	 */
	string getExitCodeString();

// the following lines commented for new error code communication between OI and OM. OI_OM_ERROR_COM
	/**	@brief setExitCode method
	 *	setExitCode method: method to be used to set exitCode for Commands acting as Object implementer.
	 *	setExitCode the exit code is stored in exitCode class variable to be available for acs_apgcc_oihandler
	 *	setExitCode class to be returned after callback execution.
	 *
	 *  @param p_exitCode the exit code of the command
	 *
	 */
	void setExitCode(int p_exitCode);

	/**	@brief getExitCode method
	 *	getExitCode method: method to be used to get exitCode for Commands acting as Object implementer.
	 *	getExitCode the exit code is stored in exitCode class variable to be available for acs_apgcc_oihandler
	 *	getExitCode class to be returned after callback execution.
	 *
	 */
	SaAisErrorT getExitCode();

	/*1963*/
	/**	@brief getCcbAugmentation method for Ticket 1963
	 * This method get the ccbHandle to be augmented by the calling oi with other operations.
	 *  @param oiHandle        : INPUT   the oiHandle provided by the callback
	 *
	 *  @param ccbId           : INPUT   the ccbId provided by the callback
	 *
	 *  @param ccbHandle       : OUTPUT  a pointer to ACS_APGCC_CcbHandle variable. once returned on success it contains the
	 *  pointer to the ccbHandle value to be used for augmentation. This ccbHandle to be augmented is valid only durig the callback upcall.
	 *
	 *  @param ACS_APGCC_AdminOwnerHandle       : OUTPUT  a pointer to adminOwnerHandle. once returned on success it contains the
	 *  pointer to the adminOwnerHandle. This adminOwnerHandle is valid only during the callback upcall.
	 *
	 *  @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getCcbAugmentationInitialize(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, ACS_APGCC_CcbHandle* ccbHandle, ACS_APGCC_AdminOwnerHandle* adminOwnwrHandler);

	/**	@brief createObjectAugmentCcb method for Ticket 1963
	 * This method augment the ccbHandle retrieved by calling the getCcbAugmentationInitialize() method creating a new Object .
     *
	 *  @param ccbHandle       					: INPUT  the  ACS_APGCC_CcbHandle
	 *
	 *  @param ACS_APGCC_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param p_className       				: INPUT  the  name of the class for which a new instance is to be created
	 *
	 *  @param p_parentName       				: INPUT  the Name of the parent object for the object to be created
	 *
	 *  @param p_attrValuesList					: INPUT the vector of ACS_CC_ValuesDefinitionType elements each one containing one
	 *  attribute of the new object to be created.
	 *
	 *  @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType createObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList);

	/**	@brief modifyObjectAugmentCcb  method for Ticket 1963.
	 * This method augment the ccbHandle retrieved by calling the getCcbAugmentationInitialize() method modifying an Object .
     *
	 *  @param ccbHandle       					: INPUT  the  ACS_APGCC_CcbHandle
	 *
	 *  @param ACS_APGCC_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param p_objectName       				: INPUT  the  DN of the object to be modified
	 *
	 *  @param p_parentName       				: INPUT  the Name of the parent object for the object to be created
	 *
	 *  @param p_attributeValue					: INPUT the ACS_CC_ImmParameter holding the proposed modify
	 *
	 *  @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType modifyObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue);

	/**	@brief modifyObjectAugmentCcb  method for Ticket 1963.
	 * This method augment the ccbHandle retrieved by calling the getCcbAugmentationInitialize() method deleting an Object .
     *
	 *  @param ccbHandle       					: INPUT  the  ACS_APGCC_CcbHandle
	 *
	 *  @param ACS_APGCC_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param m_objectName       				: INPUT  the  DN of the object to be deleted
	 *
	 *  @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType deleteObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* m_objectName);

	/**	@brief applyAugmentCcb  method for Ticket 1963.
	 * This method applies the ccbHandle retrieved by calling the getCcbAugmentationInitialize() It must be called by the OI augmenting the CCB before returning.
	 * If applyAugmentCcb fails the entire CCB originated by the OM (not only the augmented ccb) fails.
     *
	 *  @param ccbHandle       					: INPUT  the  ACS_APGCC_CcbHandle
	 *
	 *  @param ACS_APGCC_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param m_objectName       				: INPUT  the  DN of the object to be deleted
	 *
	 *  @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType applyAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle);

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
	const char* getInternalLastErrorText();

	/**end 1963*/

private:


	acs_apgcc_objectimplementerinterface_V2_impl(acs_apgcc_objectimplementerinterface_V2_impl &objImp);

	acs_apgcc_objectimplementerinterface_V2_impl& operator=(const acs_apgcc_objectimplementerinterface_V2_impl &objImp);


	//========//
	// Fields //
	//========//
	std::string objName;

	std::string impName;

	ACS_APGCC_ScopeT scope;

	SaUint64T immOiHandle;

	SaSelectionObjectT selObj;

	ACS_TRA_Logging log;

	//commented for OI_OM_ERROR_COM
	int exitCode;/*exit code for commands*/
	string exitCodeString;
	/*1963*/

	int errorCode;
	/**
	 * setImmError method: this method is used to set the error returned by ImmService
	 *
	 * @param p_errorCode :	integer that represents the error
	 */
	void setInternalError(int p_errorCode);
	/*end 1963*/


};

#endif /* ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_V2_IMPL_H_ */
