/*
 * acs_apgcc_oihandler_V3.h
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_OIHANDLER_V3_H_
#define ACS_APGCC_OIHANDLER_V3_H_

#include "saImmOi.h"
//#include "ACS_APGCC_ObjectImplementerInterface.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"

#include "ace/OS.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"

class acs_apgcc_oihandler_V3_impl;

/** @class acs_apgcc_oihandler_V3 acs_apgcc_oihandler_V3.h
 *	@brief acs_apgcc_oihandler_V3 class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2011-08-30
 *	@version R1A
 *
 *	acs_apgcc_oihandler_V3 Class detailed description
 */

class acs_apgcc_oihandler_V3 {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_oihandler_V3 Default constructor
	 *
	 *	acs_apgcc_oihandler_V3 Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V3();


	/** @brief acs_apgcc_oihandler_V3 copy constructor
	 *
	 *	acs_apgcc_oihandler_V3 Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V3(acs_apgcc_oihandler_V3 &oi);


	/** @brief acs_apgcc_oihandler_V3 Destructor
	 *
	 *	acs_apgcc_oihandler_V3 Destructor of class
	 *
	 *	@remarks Remarks
	 */

	~acs_apgcc_oihandler_V3();

	//===========//
	// Functions //
	//===========//

	/**	@brief addObjectImpl method
	 *	addObjectImpl method: Initialize the IMM connection and setting for the object specified by objName attribute
	 *	of obj, an object Implementer.
	 *	This function initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *
	 *  @param obj A pointer of instance of acs_apgcc_objectimplementerinterface_V3
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

private:

	acs_apgcc_oihandler_V3& operator=(const acs_apgcc_oihandler_V3 &oiHandler);


	acs_apgcc_oihandler_V3_impl *oiHandlerimpl;

};



#endif /* ACS_APGCC_OIHANDLER_V3_H_ */
