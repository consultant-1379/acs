/*
 * ACS_APGCC_ObjectImplementerInterface.h
 *
 *  Created on: Sep 2, 2010
 *      Author: xfabron
 */

#ifndef ACS_APGCC_OBJJECTIMPLEMENTERINTERFACE_H_
#define ACS_APGCC_OBJJECTIMPLEMENTERINTERFACE_H_

#include "saImmOi.h"
#include "ace/OS.h"
#include "ACS_CC_Types.h"


using namespace std;

class acs_apgcc_objectimplementerinterface_impl;

/** @class ACS_APGCC_ObjectImplementerInterface ACS_APGCC_ObjectImplementerInterface.h
 *	@brief ACS_APGCC_ObjectImplementerInterface class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-13
 *	@version R1A
 *
 *	ACS_APGCC_ObjectImplementerInterface Class detailed description
 */

class ACS_APGCC_ObjectImplementerInterface {

	//=========//
	// Friends //
	//=========//
	friend class acs_apgcc_oihandler_impl;

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_APGCC_ObjectImplementerInterface Default constructor
	 *
	 *	ACS_APGCC_ObjectImplementerInterface Constructor
	 *
	 *	@remarks Remarks
	 */
	ACS_APGCC_ObjectImplementerInterface();


	/** @brief ACS_APGCC_ObjectImplementerInterface constructor
	 *
	 *	ACS_APGCC_ObjectImplementerInterface Constructor this constructor is used to define a Object Implementer
	 *	for a class
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 */
	ACS_APGCC_ObjectImplementerInterface(string p_impName );


	/** @brief ACS_APGCC_ObjectImplementerInterface constructor
	 *
	 *	ACS_APGCC_ObjectImplementerInterface Constructor this constructor is used to define a Object Implementer
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
	ACS_APGCC_ObjectImplementerInterface(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );


	/** @brief ACS_APGCC_ObjectImplementerInterface Destructor
	 *
	 *	ACS_APGCC_ObjectImplementerInterface Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_APGCC_ObjectImplementerInterface();


	//===================//
	// Virtual Functions //
	//===================//

	/** @brief create method
	 *
	 *	create method: This is a virtual method to be implemented by the Designer when extending the base class.
	 *	This method will be called as a callback when an Object is created as instance of a Class for which the Application
	 *	has registered as Class Object Implementer.
	 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
	 *	proper actions.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the creation of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param className: the name of the class. When an object is created as instance of this class this method is
	 *	called if the application has registered as class implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param parentname: the name of the parent object for the object now creating.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues element pointers each one containing
	 *	the info about the attributes belonging to the now creating class.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)=0;


	/** @brief deleted method
	 *
	 *	deleted method: This is a virtual method to be implemented by the Designer when extending the base class.
	 *	This method will be called as a callback when deleting an Object for which the Application has registerd as
	 *	Object Implementer. All input parameters are input provided by IMMSV Application and have to be used by
	 *	the implementer to perform proper actions.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parametr provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the deletion of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param objName: the Distinguished name of the object that has to be deleted.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)=0;


	/** @brief modify method
	 *
	 *	modify method: This is a virtual method to be implemented by the Designer when extending the base class.
	 *	This method will be called as a callback when modifying an Object for which the Application has registered
	 *	as Object Implementer.
	 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
	 *	proper actions.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param objName: the Distinguished name of the object that has to be modified.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param attrMods: a NULL terminated array of pointers to ACS_APGCC_AttrModification elements containing
	 *	the information about the modify to perform. This is an Input Parametr provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)=0;


	/** @brief complete method
	 *
	 *	complete method: This is a virtual method to be implemented by the Designer when extending the base class.
	 *	This method will be called as a callback when a Configuration Change Bundle is complete and can be applied
	 *	regarding  an Object for which the Application has registered as Object Implementer.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)=0;


	/** @brief abort method
	 *
	 *	abort method: This is a virtual method to be implemented by the Designer when extending the base class.
	 *	This method will be called as a callback when a Configuration Change Bundle, regarding  an Object for which
	 *	the Application has registered as Object Implementer, has aborted because of the failure of previous
	 *	complete and can not be be applied. This method is called only if at least one complete method failed.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
	 *	registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)=0;


	/** @brief apply method
	 *
	 *	apply method: This is a virtual method to be implemented by the Designer when extending the base class.
	 *	This method will be called as a callback when a Configuration Change Bundle, regarding  an Object for which
	 *	the Application has registered as Object Implementer, is complete and can be applied.
	 *	This method is called only if all the complete method have been successfully executed.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
	 *	registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)=0;


	/** @brief updateRuntime method
	 *
	 *	updateRuntime method: This is a virtual method to be implemented by the Designer when extending the base class.
	 * 	This method will be called as a callback when modifying a runtime not-cached attribute of a configuration Object
	 * 	for which the Application has registered as Object Implementer.
	 * 	All input parameters are input provided by IMMSV Application and have to be used by the implementer
	 * 	to perform proper actions.
	 *
	 *  @param p_objName: the Distinguished name of the object that has to be modified.
	 *  				  This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @param p_attrName: the name of attribute that has to be modified.
	 *  				   This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char* p_attrName)=0;


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

	/**	@brief setExitCode method
	 *  setExitCode method:method to be used to set exitCode for Commands acting as Object implementer.
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

// the following lines commented for new error code communication between OI and OM.OI_OM_ERROR_COM
	/**	@brief setExitCode method
	 *	setExitCode method: method to be used to set exitCode for Commands acting as Object implementer.
	 *	setExitCode the exit code is stored in exitCode class variable to be available for acs_apgcc_oihandler
	 *	setExitCode class to be returned after callback execution.
	 *
	 *  @param p_exitCode the exit code of the command
	 *
	 */
	void setExitCode(int p_exitCode);

// the following lines commented for new error code communication between OI and OM.OI_OM_ERROR_COM
	/**	@brief getExitCode method
	 *	getExitCode method: method to be used to get exitCode for Commands acting as Object implementer.
	 *	getExitCode the exit code is stored in exitCode class variable to be available for acs_apgcc_oihandler
	 *	getExitCode class to be returned after callback execution.
	 *
	 */
	SaAisErrorT getExitCode();

protected:

	//========//
	// Fields //
	//========//
	std::string objName;
	std::string impName;
	ACS_APGCC_ScopeT scope;

private:


	acs_apgcc_objectimplementerinterface_impl *objectImplInt_impl;

	ACS_APGCC_ObjectImplementerInterface(ACS_APGCC_ObjectImplementerInterface &objImp);

	ACS_APGCC_ObjectImplementerInterface& operator=(const ACS_APGCC_ObjectImplementerInterface &objImp);


	//===========//
	// Functions //
	//===========//

	SaUint64T getImmOiHandle();


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




};


#endif /* ACS_APGCC_OBJECTIMPLEMENTERINTERFACE_H_ */
