/*
 * acs_apgcc_objectimplementerinterface_impl.h
 *
 *  Created on: Mar 3, 2011
 *      Author: xfabron
 */

#ifndef ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_IMPL_H_
#define ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_IMPL_H_

#include "saImmOi.h"
#include "string"

#include "ace/OS.h"
#include "ACS_CC_Types.h"
#include "ACS_TRA_Logging.h"
#include <sstream>



using namespace std;

/** @class acs_apgcc_objectimplementerinterface_impl acs_apgcc_objectimplementerinterface_impl.h
 *	@brief acs_apgcc_objectimplementerinterface_impl class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-13
 *	@version R1A
 *
 *	acs_apgcc_objectimplementerinterface_impl Class detailed description
 */

class acs_apgcc_objectimplementerinterface_impl{

	//=========//
	// Friends //
	//=========//
	friend class acs_apgcc_oihandler_impl;

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
	acs_apgcc_objectimplementerinterface_impl();


	/** @brief acs_apgcc_objectimplementerinterface_impl constructor
	 *
	 *	acs_apgcc_objectimplementerinterface_impl Constructor this constructor is used to define a Object Implementer
	 *	for a class
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 */
	acs_apgcc_objectimplementerinterface_impl(string p_impName );


	/** @brief acs_apgcc_objectimplementerinterface_impl constructor
	 *
	 *	acs_apgcc_objectimplementerinterface_impl Constructor this constructor is used to define a Object Implementer
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
	acs_apgcc_objectimplementerinterface_impl(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );


	/** @brief acs_apgcc_objectimplementerinterface_impl Destructor
	 *
	 *	acs_apgcc_objectimplementerinterface_impl Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_apgcc_objectimplementerinterface_impl();


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

private:


	acs_apgcc_objectimplementerinterface_impl(acs_apgcc_objectimplementerinterface_impl &objImp);

	acs_apgcc_objectimplementerinterface_impl& operator=(const acs_apgcc_objectimplementerinterface_impl &objImp);


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


};


#endif /* ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_IMPL_H_ */
