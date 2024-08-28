/*
 * acs_apgcc_oihandler_V3_impl.h
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_OIHANDLER_V3_IMPL_H_
#define ACS_APGCC_OIHANDLER_V3_IMPL_H_

#include <map>
#include "saImmOi.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"

#include "acs_apgcc_objectimplementerinterface_V3.h"

#include "ace/OS.h"
#include <ace/Condition_T.h>

#include "ACS_TRA_Logging.h"

using namespace std;

/** @class acs_apgcc_oihandler_impl acs_apgcc_oihandler_V3_impl.h
 *	@brief acs_apgcc_oihandler_V3_impl class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2011-08-30
 *	@version R1A
 *
 *	acs_apgcc_oihandler_V3_impl Class detailed description
 */

class acs_apgcc_oihandler_V3_impl {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_oihandler_V3_impl Default constructor
	 *
	 *	ACS_APGCC_OiHandler Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V3_impl();


	/** @brief acs_apgcc_oihandler_V3_impl Default constructor
	 *
	 *	ACS_APGCC_OiHandler Copy Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V3_impl(acs_apgcc_oihandler_V3_impl *oiHanlderImp);


	/** @brief acs_apgcc_oihandler_V3_impl Destructor
	 *
	 *	ACS_APGCC_OiHandler Destructor of class
	 *
	 *	@remarks Remarks
	 */
	~acs_apgcc_oihandler_V3_impl();


	//===========//
	// Functions //
	//===========//

	/**	@brief addObjectImpl method
	 *	addObjectImpl method: Initialize the IMM connection and setting for the object specified by objName attribute
	 *	of obj, an object Implementer.
	 *	This function initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *
	 *  @param obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType addObjectImpl(acs_apgcc_objectimplementerinterface_V3 *obj);


	/**	@brief removeObjectImpl method
	 *	removeObjectImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addObjectImpl must no longer be considered as the implementer of the set of objects
	 *	identified by scope attribute and objcetName attribute of obj parameter.
	 *
	 *  @param obj A pointer of instance of acs_apgcc_objectimplementerinterface_V3
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType removeObjectImpl(acs_apgcc_objectimplementerinterface_V3 *obj );


	/**	@brief addClassImpl method
	 *	addClassImpl method: Initialize the IMM connection and setting for all the object that are
	 *	instances of the object class whose name is specified by clasName parameter an Object
	 *	implementer whose name is specified by implName attribute of obj .
	 *
	 *  @param obj A pointer of instance of acs_apgcc_objectimplementerinterface_V3
	 *
	 *  @param className Object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType addClassImpl(acs_apgcc_objectimplementerinterface_V3 *obj, const char * className);


	/**	@brief addMultipleClassImpl method
	 *	addMultipleClassImpl method: Initialize the IMM connection and setting for all the object that are
	 *	instances of the object classes whose name is specified by p_classNameVec parameter an Object
	 *	implementer whose name is specified by implName attribute of obj .
	 *
	 *  @param p_obj A pointer of instance of acs_apgcc_objectimplementerinterface_V3
	 *
	 *  @param p_classNameVec Vector of string of object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType addMultipleClassImpl(acs_apgcc_objectimplementerinterface_V3 *p_obj, vector<string> p_classNameVec);


	/**	@brief removeClassImpl method
	 *	removeClassImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addClassImpl must not be considered anymore as the implementer of the objects that are instances
	 *	of the object class whose name is specified by className attribute
	 *
	 *  @param obj A pointer of instance of acs_apgcc_objectimplementerinterface_V3
	 *
	 *  @param className Object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType removeClassImpl(acs_apgcc_objectimplementerinterface_V3 *obj, const char * className );


	/**	@brief removeMultipleClassImpl method
	 *	removeMultipleClassImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addMultipleClassImpl must not be considered anymore as the implementer of the objects that are instances
	 *	of the classes whose name is specified by p_classNameVec vector attribute
	 *
	 *  @param p_obj A pointer of instance of acs_apgcc_objectimplementerinterface_V3
	 *
	 *  @param p_classNameVec Object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType removeMultipleClassImpl(acs_apgcc_objectimplementerinterface_V3 *p_obj, vector<string> p_classNameVec );


	/**
	 * getErrorText retrieve the error text string related to the error identifier provided as input
	 * @param p_errorId    : (Input) the error identifier
	 * @return std::string : the text string related to p_errorId
	 */
	std::string getErrorText( int p_errorId);

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
private:

	/**
	 * @brief Finalize method
	 * Finalize method: used to finalize interaction with IMM.
	 * The method clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer.
	 * After calling this method no other operation on IMM and that involves Object implementer could be performed.
	 *
	 * @param handle handle that identifies a particular initialization of the IMM Service
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType Finalize (SaUint64T handle );


	static map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *> implementerList;
	static ACE_Recursive_Thread_Mutex _acs_apgcc_Mutex_implementerList;


	/**
	 * @brief CreateCallback method
	 * CreateCallback method: this method is used by the IMM Service to enable an Object Implementer
	 * to validate and register a change request being added to a CCB identified by ccbId.
	 * The change request is a creation request for a configuration object of a class that is
	 * implemented by the process implementing the callback
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @param className Object class name.
	 *
	 * @param parentName Pointer to the name of the parent of the new object
	 *
	 * @param attr Pointer to a NULL-terminated array of pointers to attribute descriptor.
	 *
	 * @return SaAisErrorT. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT CreateCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,
			const SaImmClassNameT className,
			const SaNameT *parentName,
			const SaImmAttrValuesT_2 **attr);

	/**
	 * @brief DeleteCallback method
	 * DeleteCallback method: this method is used by the IMM Service to enable an Object Implementer
	 * to validate and memorize a deletion request being added to a CCB identified by ccbId.
	 * The deletion request is a request to delete object that are implemented by the process that
	 * provided the callback function
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @param objectName Pointer to the object name.
	 *
	 * @return ACS_CC_ReturnType. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT DeleteCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,
			const SaNameT *objectName);


	/**
	 * @brief ModifyCallback method
	 * ModifyCallback method: this method is used by the IMM Service to enable an Object Implementer
	 * to validate and memorize a change request being added to a CCB identified by ccbId.
	 * The change request is a request to modify configuration attributes of a configuration object
	 * implemented by the process implementing the callback
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @param objectName Pointer to the object name.
	 *
	 * @param attrMods Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modification to perform
	 *
	 * @return ACS_CC_ReturnType. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT ModifyCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,
			const SaNameT *objectName,
			const SaImmAttrModificationT_2 **attrMods);


	/**
	 * @brief CompletedCallback method
	 * CompletedCallback method: this method is used by the IMM Service to inform an Object Implementer
	 * that the CCB identified by ccbId is now complete. The implementer process must check that
	 * the sequence of change requests contained in the CCB is valid and that non error will be generated
	 * when these changes are applied
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @return ACS_CC_ReturnType. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT CompletedCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId);


	/**
	 * @brief AbortCallback method
	 * AbortCallback method: this method is used by the IMM Service to inform an Object Implementer
	 * that the CCB identified by ccbId is aborted, so the object implementer can remove all change
	 * request memorized for this CCB
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 */
	static void AbortCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId);


	/**
	 * @brief ApplyCallback method
	 * ApplyCallback method: this method is used by the IMM Service to inform an Object Implementer
	 * that the CCB identified by ccbId has been applied by IMM Service
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 */
	static void ApplyCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId);


	/**
	 * @brief OiRtAttUpdateCallback method
	 * OiRtAttUpdateCallback method: The IMM service invokes this callback fuction to request at the
	 * Object Implementer to update the values of some non-cached attribute of a runtime object.
	 *
	 * @param  immOiHandle:	the object implementer handle. This is an Input Parameter provided by IMMSV Application
	 *
	 * @param  objectName:	the distinguished name of the object for which the update is requested.
	 *
	 * @param  attributeNames:	pointer to a NULL-terminated array of pointers of attribute name for which values must be updated.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	static SaAisErrorT OiRtAttUpdateCallback(SaImmOiHandleT immOiHandle, const SaNameT *objectName,
				const SaImmAttrNameT *attributeNames);

	/*callback adminoperation*/

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

	int errorCode;
	ACS_TRA_Logging log;




};

#endif /* ACS_APGCC_OIHANDLER_V3_IMPL_H_ */
