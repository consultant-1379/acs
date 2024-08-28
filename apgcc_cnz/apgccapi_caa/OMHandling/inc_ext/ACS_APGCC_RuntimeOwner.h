/*
 * ACS_APGCC_RuntimeOwner.h
 *
 *  Created on: Oct 6, 2010
 *      Author: xfabron
 */

#ifndef ACS_APGCC_RUNTIMEOWNER_H_
#define ACS_APGCC_RUNTIMEOWNER_H_


#include <vector>
#include "saImmOi.h"
#include "ACS_CC_Types.h"


class acs_apgcc_runtimeowner_impl;


using namespace std;

/** @class ACS_APGCC_RuntimeOwner.h
 *	@brief ACS_APGCC_RuntimeOwner class
 *	@author xfabron (Fabio Ronca)
 *	@date 2010-10-06
 *	@version R1A
 *
 *	ACS_APGCC_RuntimeOwner Class detailed description
 */
class ACS_APGCC_RuntimeOwner {

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_APGCC_RuntimeOwner Default constructor
	 *
	 *	ACS_APGCC_RuntimeOwner Constructor of class
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_RuntimeOwner();

	/** @brief ACS_APGCC_RuntimeOwner copy constructor
	 *
	 *	ACS_APGCC_RuntimeOwner copy Constructor of class
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_RuntimeOwner(ACS_APGCC_RuntimeOwner &runTimeImp);


	/** @brief ACS_APGCC_RuntimeOwner Destructor
	 *
	 *	ACS_APGCC_RuntimeOwner Destructor of class
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_APGCC_RuntimeOwner();


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
	 * @brief updateCallback method
	 * updateCallback method: This is a virtual method to be implemented by the Designer when extending
	 * the base class. This method will be called as a callback when an non-cached attribute of a runtime Object
	 * is updated.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the update is requested.
	 *
	 * @param  p_attrName:	the name of the attribute for which values must be updated.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	virtual ACS_CC_ReturnType updateCallback(const char* p_objName, const char* p_attrName)=0;


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

	ACS_APGCC_RuntimeOwner& operator=(const ACS_APGCC_RuntimeOwner &runOwner);

	acs_apgcc_runtimeowner_impl *runtimeOwnerImpl;



};



#endif /* ACS_APGCC_RUNTIMEOWNER_H_ */

