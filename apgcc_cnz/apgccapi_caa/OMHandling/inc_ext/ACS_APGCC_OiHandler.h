/*
 * ACS_APGCC_OiHandler.h
 *
 *  Created on: Sep 2, 2010
 *      Author: xfabron
 */


#ifndef ACS_APGCC_OIHANDLER_H_
#define ACS_APGCC_OIHANDLER_H_


#include "saImmOi.h"
#include "ACS_APGCC_ObjectImplementerInterface.h"

#include "ace/OS.h"
#include "ACS_CC_Types.h"


class acs_apgcc_oihandler_impl;

/** @class ACS_APGCC_OiHandler ACS_APGCC_OiHandler.h
 *	@brief ACS_APGCC_OiHandler class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-12
 *	@version R1A
 *
 *	ACS_APGCC_OiHandler Class detailed description
 */
class ACS_APGCC_OiHandler {
	//===========//
	// Constants //
	//===========//
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_APGCC_OiHandler Default constructor
	 *
	 *	ACS_APGCC_OiHandler Constructor of class
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_OiHandler();


	/** @brief ACS_APGCC_OiHandler copy constructor
	 *
	 *	ACS_APGCC_OiHandler Constructor of class
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_OiHandler(ACS_APGCC_OiHandler &oi);


	/** @brief ACS_APGCC_OiHandler Destructor
	 *
	 *	ACS_APGCC_OiHandler Destructor of class
	 *
	 *	@remarks Remarks
	 */

	~ACS_APGCC_OiHandler();

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
	ACS_CC_ReturnType addObjectImpl(ACS_APGCC_ObjectImplementerInterface *obj);


	/**	@brief removeObjectImpl method
	 *	removeObjectImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addObjectImpl must no longer be considered as the implementer of the set of objects
	 *	identified by scope attribute and objcetName attribute of obj parameter.
	 *
	 *  @param obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType removeObjectImpl(ACS_APGCC_ObjectImplementerInterface *obj );


	/**	@brief addClassImpl method
	 *	addClassImpl method: Initialize the IMM connection and setting for all the object that are
	 *	instances of the object class whose name is specified by clasName parameter an Object
	 *	implementer whose name is specified by implName attribute of obj .
	 *
	 *  @param obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
	 *
	 *  @param className Object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType addClassImpl(ACS_APGCC_ObjectImplementerInterface *obj, const char * className);


	/**	@brief addMultipleClassImpl method
	 *	addMultipleClassImpl method: Initialize the IMM connection and setting for all the object that are
	 *	instances of the object classes whose name is specified by p_classNameVec parameter an Object
	 *	implementer whose name is specified by implName attribute of obj .
	 *
	 *  @param p_obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
	 *
	 *  @param p_classNameVec Vector of string of object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType addMultipleClassImpl(ACS_APGCC_ObjectImplementerInterface *p_obj, vector<string> p_classNameVec);


	/**	@brief removeClassImpl method
	 *	removeClassImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addClassImpl must not be considered anymore as the implementer of the objects that are instances
	 *	of the object class whose name is specified by className attribute
	 *
	 *  @param obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
	 *
	 *  @param className Object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType removeClassImpl(ACS_APGCC_ObjectImplementerInterface *obj, const char * className );


	/**	@brief removeMultipleClassImpl method
	 *	removeMultipleClassImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addMultipleClassImpl must not be considered anymore as the implementer of the objects that are instances
	 *	of the classes whose name is specified by p_classNameVec vector attribute
	 *
	 *  @param p_obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
	 *
	 *  @param p_classNameVec Object class name
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType removeMultipleClassImpl(ACS_APGCC_ObjectImplementerInterface *p_obj, vector<string> p_classNameVec );


	/**
	 * getErrorText retrieve the error text string related to the error identifier provided as input
	 * @param p_errorId    : (Input) the error identifier
	 * @return std::string : the text string related to p_errorId
	 */
	std::string getErrorText( int p_errorId);

private:

	ACS_APGCC_OiHandler& operator=(const ACS_APGCC_OiHandler &oiHandler);


	acs_apgcc_oihandler_impl *oiHandlerimpl;

};



#endif /* ACS_APGCC_OIHANDLER_H_ */
