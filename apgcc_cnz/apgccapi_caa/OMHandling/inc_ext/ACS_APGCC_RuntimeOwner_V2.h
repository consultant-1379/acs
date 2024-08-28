/*
 * ACS_APGCC_RuntimeOwner_V2.h
 *
 *  Created on: Aug 31, 2011
 *      Author: xfabron
 */

#ifndef ACS_APGCC_RUNTIMEOWNER_V2_H_
#define ACS_APGCC_RUNTIMEOWNER_V2_H_


#include <vector>
#include "saImmOi.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"


class acs_apgcc_runtimeowner_impl_v2;


using namespace std;

/** @class ACS_APGCC_RuntimeOwner_V2.h
 *	@brief ACS_APGCC_RuntimeOwner_V2 class
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-08-31
 *	@version R1A
 *
 *	ACS_APGCC_RuntimeOwner_V2 Class detailed description
 */
class ACS_APGCC_RuntimeOwner_V2 {

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_APGCC_RuntimeOwner_V2 Default constructor
	 *
	 *	ACS_APGCC_RuntimeOwner_V2 Constructor of class
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_RuntimeOwner_V2();

	/** @brief ACS_APGCC_RuntimeOwner copy constructor
	 *
	 *	ACS_APGCC_RuntimeOwner copy Constructor of class
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_RuntimeOwner_V2(ACS_APGCC_RuntimeOwner_V2 &runTimeImp);


	/** @brief ACS_APGCC_RuntimeOwner_V2 Destructor
	 *
	 *	ACS_APGCC_RuntimeOwner_V2 Destructor of class
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_APGCC_RuntimeOwner_V2();


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


	//===========//
	// Functions //
	//===========//

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


	/**
	 * @brief createRuntimeObj method
	 * createRuntimeObj method: used to create a runtime object in IMM structure.
	 *
	 * @param p_className      : the Name of the class implemented by the Object.
	 *
	 * @param p_parentName     : the Name of the IMM Object that will be the parent of the creating OBJ
	 *
	 * @param p_attrValuesList : a vector of ACS_CC_ValuesDefinitionType elements containing the Attributes Values
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType createRuntimeObj(const char* p_className, const char* p_parentName,
			vector<ACS_CC_ValuesDefinitionType> p_attrValuesList);


	/**
	 * @brief deleteRuntimeObj method
	 * deleteRuntimeObj method: used to delete a runtime object in IMM structure.
	 *
	 * @param p_objName the full DN of the runtime object in IMM tree to be deleted
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType deleteRuntimeObj(const char *p_objName);


	/**
	 * @brief modifyRuntimeObj method
	 * modifyRuntimeObj method: this method is used to modify the value of a attribute belonging
	 * to a runtime object.
	 *
	 *  @param  p_objName 		:	the full DN of the object containing the value to be retrieved
	 *
	 *  @param  p_attributeValue:	the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue);


	/**
	 * @brief dispatch method
	 * dispatch method: This method is used to dispatch callbacks on the Object Implementer
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
	ACS_CC_ReturnType  adminOperationResult(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, int result);

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
	 * @param outParamVector: output parameter vector
	 *
	 * @return ACS_CC_ReturnType . On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType  adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result, vector<ACS_APGCC_AdminOperationParamType> outParamVector);

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


	//===================//
	// Virtual Functions //
	//===================//
	/**
	 * @brief updateCallback method
	 * updateCallback method: This is a virtual method to be implemented by the Designer when extending
	 * the base class. This method will be called as a callback when an non-cached attribute of a runtime Object
	 * is updated.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the update is requested.
	 *
	 * @param  p_attrNames:	pointer to a null terminate array of attribute name for which values must be updated.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	virtual ACS_CC_ReturnType updateCallback(const char* p_objName, const char** p_attrNames)=0;


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
	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)=0;


private:

	ACS_APGCC_RuntimeOwner_V2& operator=(const ACS_APGCC_RuntimeOwner_V2 &runOwner);

	acs_apgcc_runtimeowner_impl_v2 *runtimeOwnerImpl;

};



#endif /* ACS_APGCC_RUNTIMEOWNER_V2_H_ */
