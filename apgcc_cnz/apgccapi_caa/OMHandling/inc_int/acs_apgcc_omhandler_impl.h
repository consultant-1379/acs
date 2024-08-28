/*
 * acs_apgcc_omhandlerimpl.h
 *
 *  Created on: Mar 2, 2011
 *      Author: xfabron
 */

#ifndef ACS_APGCC_OMHANDLERIMPL_H_
#define ACS_APGCC_OMHANDLERIMPL_H_


#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include "ACS_CC_Types.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include "ace/OS.h"
#include "ACS_TRA_Logging.h"

using namespace std;

class acs_apgcc_omhandler_impl {
public:
	acs_apgcc_omhandler_impl();

	acs_apgcc_omhandler_impl(acs_apgcc_omhandler_impl *omHanlderImp);

	virtual ~acs_apgcc_omhandler_impl();

	/**
	 *
	 * Init method: used to Initialize Interaction between the calling process and IMM.
	 * Each application that intends to use OmHandler class must call this method to perform any action and
	 * call anyone of the other methods. The p_registeredImpl parameter is used to specify if all further modifies performed by the initializing Application have to be performed on IMM Objects only when an IMM object Implementer is active or not. At init time the appication has to choose if it intends to perform actions being sure that an Object Implementer is running for the rrelated Object or not.
     * @param p_registeredImpl: a flag indicating if all further modify performed by the application
     * have to be performed only if an Object Implementer is running for the modified object or not.
     * Possible values are NO_REGISTERED_OI or REGISTERED_OI.
     * Calling the Init method with NO_REGISTERED_OI argument the application registers to IMM in such a way that all modify on objects are performed regardless of whether an Object Implementer is registered on the related object or not.
     * Calling the Init method with REGISTERED_OI argument, the application registers to IMM in such a way that all modify on objects are performed only if an Object Implementer is running for it otherwise the modify Fails.
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType Init (int p_registeredImpl = NO_REGISTERED_OI );



	/**
	 * Finalize method: used to finalize interaction with IMM and release all handler to it.
	 * After calling this method no other operation on IMM could be performed.
	 * No parameter required
	 */
	ACS_CC_ReturnType Finalize ();


	/**
	 *
	 * defineClass method: used to define a new class in IMM. Provided input parameter are
	 * m_className the name of the class that has to be created;
	 * m_classCategory the category of the class it can be CONFIGURATION or RUNTIME;
	 * m_attrList a vector of ACS_CC_AttrDefinitionType elements containing the definition of each attribute
	 * in the class that is beeing defined
	 * @param m_className the Name of the class to be defined.
	 * @param m_classCategory the class category. Admitted Values are CONFIGURATION or RUNTIME
	 * @param m_attrList a vector of ACS_CC_AttrDefinitionType elements containing the definition of Attributes
	 * belonging to the class.
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType defineClass (char* m_className, ACS_CC_ClassCategoryType m_classCategory,vector<ACS_CC_AttrDefinitionType> m_attrList);


	/*Added For Object creation*/
	/**
	 * createObject method: used to create an object in IMM structure. Provided input parameter are
	 * m_className the name of the class that the creating Object will implement; m_parentName the
	 * name of the Object, already existing in IMM tree, that will be the parent of the now creating
	 * Object; m_attrValuesList a vector of ACS_CC_ValuesDefinitionType elements containing the value
	 * of each attributwe of the class.
	 * @param m_className      : the Name of the class implemented by the Object.
	 * @param m_parentName     : the Name of the IMM Object that will be the parent of the creating OBJ
	 * @param m_attrValuesList : a vector of ACS_CC_ValuesDefinitionType elements containing the Attributes Values
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType createObject (const char* m_className, const char* m_parentName, vector<ACS_CC_ValuesDefinitionType> m_attrValuesList );


	/**
	 * createObject method: used to create an object in IMM structure. Provided input parameter are
	 * m_className the name of the class that the creating Object will implement; m_parentName the
	 * name of the Object, already existing in IMM tree, that will be the parent of the now creating
	 * Object; m_attrValuesList a vector of ACS_CC_ValuesDefinitionType elements containing the value
	 * of each attributwe of the class.
	 * @param p_className      : the Name of the class implemented by the Object.
	 * @param p_parentName     : the Name of the IMM Object that will be the parent of the creating OBJ
	 * @param p_attrValuesList : a vector of ACS_CC_ValuesDefinitionType elements containing the Attributes Values
	 * @param p_transactionName: the string that specify the name of transaction to which the creation request belongs
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType createObject (const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList, std::string p_transactionName);


	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted.
	 * @param m_objectName the full DN of the object in IMM tree to be deleted
	 */
	ACS_CC_ReturnType deleteObject (const char* m_objectName);

	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted  and the scope. According to the value of
	 * p_scope parameter, this method can be used to delete: only an object having no child, the object
	 * and related direct child (having no child),the object and all the related subtree.  .
	 * @param m_objectName the full DN of the object in IMM tree to be deleted
	 * @param p_scope			: the scope of delete operation.
	 *  If ACS_APGCC_ONE is provided the operation requests to delete only the specified object
	 *  If ACS_APGCC_SUBLEVEL is provided the operation requests to delete the specified object and direct child
	 *  If ACS_APGCC_SUBTREE is provided the operation requests to delete the specified object and all the subtree
	 */
	ACS_CC_ReturnType deleteObject (const char* m_objectName,ACS_APGCC_ScopeT p_scope);



	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted.
	 * @param p_objectName 	   : the full DN of the object in IMM tree to be deleted
	 * @param p_transactionName: the string that specify the name of transaction to which the deletion request belongs
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType deleteObject (const char* p_objectName, std::string p_transactionName);

	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted, the transaction name and the scope. According to the value of
	 * p_scope parameter, this method can be used to delete: only an object having no child, the object
	 *  and related direct child (having no child),the object and all the related subtree.
	 *
	 * @param p_objectName 	   : the full DN of the object in IMM tree to be deleted
	 * @param p_transactionName: the string that specify the name of transaction to which the deletion request belongs
	 * @param p_scope			: the scope of delete operation.
	 *  If ACS_APGCC_ONE is provided the operation requests to delete only the specified object
	 *  If ACS_APGCC_SUBLEVEL is provided the operation requests to delete the specified object and direct child
	 *  If ACS_APGCC_SUBTREE is provided the operation requests to delete the specified object and all the subtree
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType deleteObject (const char* p_objectName, std::string p_transactionName, ACS_APGCC_ScopeT p_scope);



	/**
	 * getAttribute method: this method is used to retrieve the value of an attribute belonging to an IMM object.
	 * Required parameter are: m_objectName the DN name of the object containing the value that has to be retrieved; and
	 *  m_attribute: a pointer to the struct containing the name of the attribute to be retrieved
	 *  @param  m_objectName      :	the full DN of the object containing the value to be retrieved
	 *  @param  m_attributeValue :	the struct where the calling application has to set:
	 *  							attrName the name of the attribute to be retrieved
	 *  							attrType the type of the attribute to be retrieved
	 *                              and where the method returns the value for the attribute in the field:
	 *                              attrValues
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getAttribute (const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue );


	/**
	 * getAttribute method: this method is used to retrieve the value of a list of attributes belonging to an IMM object.
	 * Required parameter are: p_objectName the DN name of the object containing the value that has to be retrieved; and
	 *  p_attributeList: a vector of pointer to the struct containing the name of the attribute to be retrieved
	 *  @param  p_objectName     :	the full DN of the object containing the value to be retrieved
	 *  @param  p_attributeList  :	the vector of pointer at the structs ACS_CC_ImmParameter where the calling application has to set:
	 *  							attrName the name of the attribute to be retrieved
	 *                              and where the method returns the value for the attribute in the field:
	 *                              attrValues
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getAttribute (const char* p_objectName, std::vector<ACS_APGCC_ImmAttribute *> p_attributeList );


	/**
	 * getObject method: this method is used to retrieve the value of all attribute belonging to an IMM object.
	 * Required parameter is: p_object a pointer to an object  ACS_APGCC_ImmObject
	 *  @param  p_object         :	the struct where the calling application has to set:
	 *  							objName the dn of the object whose attributes should be retrieved
	 *                              and where the method returns the value for the attributes in the vector:
	 *                              attributes of type ACS_APGCC_ImmAttribute
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getObject(ACS_APGCC_ImmObject *p_object);


	/**
	 * getChildren method: this method is used to retrieve the rdn of all object that are children of the object having the name
	 * to which p_rootName point.
	 * Required parameter is:
	 *  @param  p_rootName :		pointer to the name of the root object for the search
	 *  @param 	p_scope	:			scope of the search:
	 *  							ACS_APGCC_SUBLEVEL	:	indicates that the scope of the operation is targeted to one
	 *														object and its direct children.
	 *  							ACS_APGCC_SUBTREE 	:	indicates that the scope of the operation is targeted to one
	 *														object and the entire subtree rooted at that object.
	 *	@param p_rdnList :			a vector of std::string elements containing the rdn of the object that maching the search criteria
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType getChildren(const char* p_rootName, ACS_APGCC_ScopeT p_scope, std::vector<std::string>* p_rdnList);


	/**
	 * getClassInstances method: this method is used to retrieve the dn of all object that are instances of the class having the name
	 * to which p_className point.
	 * Required parameter is:
	 *  @param  p_className :		pointer to the name of the class to which retrieve the instances
	 *	@param p_dnList :			a vector of std::string elements containing the dn of the object that maching the search criteria
	 *
	 *	@return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	ACS_CC_ReturnType getClassInstances(const char* p_className, std::vector<std::string> &p_dnList);



	/**
	 * modifyAttribute method: this method is used to modify the value of an attribute belonging to an IMM object.
	 * Required parameter are: m_objectName the DN name of the object containing the value that has to be changed; and
	 *  m_attributeValue: a pointer to the struct containing the name of the attribute to be changed
	 *  @param  m_objectName      :	the full DN of the object containing the value to be retrieved
	 *  @param  m_attributeValue :	the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType modifyAttribute(const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue );


	/**
	 * modifyAttribute method: this method is used to modify the value of an attribute belonging to an IMM object.
	 * Required parameter are: m_objectName the DN name of the object containing the value that has to be changed; and
	 *  m_attributeValue: a pointer to the struct containing the name of the attribute to be changed
	 *  @param  p_objectName      :	the full DN of the object containing the value to be retrieved
	 *  @param  p_attributeValue :	the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *  @param p_transactionName: a string that specify the name of transaction to which the change request belongs
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType modifyAttribute(const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue, std::string p_transactionName );


	/*End Added For Object creation*/


	/**
	 * removeClass method: used to remove the definition of a class in IMM. Provided input parameter is the
	 * Name of the Class to be deleted.
	 * @param m_className the Name of the class to be deleted.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType removeClass (char* m_className);


	/**
	 * applyRequest method: this method applies all request associated at the transaction identified by its name in the
	 * configuration change bundle (ccb) associated and finalize the ccb handle.
	 * @param p_transactionName the Name of the transaction.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType applyRequest(std::string p_transactionName);


	/**
	 * resetRequest method: this method remove all request associated at the transaction identified by its name in the
	 * configuration change bundle (ccb) associated and finalize the ccb handle.
	 * @param p_transactionName the Name of the transaction.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType resetRequest(std::string p_transactionName);


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
	 * getLastErrorText method: retrive by IMM the error code of error generated during a command execution
	 *
	 * @param p_parentDN :	 (Input) pointer to distinguish name of ErrorHandler Object's parents
	 * @param p_errorID :	 (Output) apg error code
	 * @param p_errorText :	 (Output) apg problem text
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getLastErrorText(std::string p_parentDN, int& p_errorId, std::string& p_errorText);

	/**
	 * geExitCode method: retrieve by immsv the error code and error message when an OM operation
	 * performed by an AP command fails.
	 *
	 * @param p_errorID :	 (Output) apg error code to be returned by the command
	 * @param p_errorText :	 (Output) apg problem text to be printed by the command
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getExitCode( int& p_errorId, std::string& p_errorText);

	/**
	 * geExitCode method: retrieve by immsv the error code and error message when an OM operation
	 * performed by an AP command fails in case of transaction actions.
	 *
	 * @param p_transactionName :(Input) an std::string holding the name of the transaction
	 * @param p_errorID         :(Output) an int.apg error code to be returned by the command
	 * @param p_errorText       :(Output) an std::string. apg problem text to be printed by the command
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType getExitCode(std::string p_transactionName, int& p_errorId, std::string& p_errorText);


	/**
	 * getCcbHandleValue method: treturns the ccbHandle value
	 *
	 *
	 *
	 * @return unsigne long long int. the ccbHandle value
	 */
	unsigned long long int getCcbHandleValue();


private:

	/**
	 * immSave method: this method is used to save the imm configuration
	 * No parameter required
	 */
	ACS_CC_ReturnType immSave();

	/**
	 * setImmError method: this method is used to set the error returned by ImmService
	 *
	 * @param p_errorCode :	integer that represents the error
	 */
	void setInternalError(int p_errorCode);


	/**
	 * getCcbHandle method: this method is used to get the ccb handle of transaction
	 *
	 * @param p_transactionName :	the name of transaction used to get the corrispondent ccb handle in the map
	 * @param p_newHandle :			the ccb handle of transaction
	 *
	 * @return int. On success return 0 on Failure return an errorCode
	 */
	int getCcbHandle(std::string p_transactionName, SaImmCcbHandleT * p_newHandle, SaImmAdminOwnerHandleT * p_ownerHandle);

	/**
	 * callSaImmOmCcbGetErrStr: this method is used to call saImmOmCcbGetErrorStrings function given the ccbId
	 * and to set in exitCodeStringRetrieved variable the found string
	 *
	 * @param p_ccbVal :	the ccb
	 *
	 * @return int. On success return 0 on Failure return -1
	 */
	int callSaImmOmCcbGetErrStr(SaImmCcbHandleT p_ccbVal);



//	int setOwnerHandle();
//
//
//	int removeOwnerHandle();

	/**Attributes**/
	SaImmHandleT immHandle;
	SaImmCcbHandleT ccbHandle;
	SaVersionT version;
	SaImmAdminOwnerHandleT ownerHandle;
	SaImmAccessorHandleT accessorHandle;
	ACS_TRA_Logging log;
	int errorCode;
	std::map<string, SaImmCcbHandleT> transactionList;
	std::map<string, SaImmAdminOwnerHandleT> transactionListOwnerHandle;
	//Added for ccb corruption management
	SaImmCcbFlagsT ccbFlagVal;

	std::string processName;
	/*added for Ticket 1904*/
	/* this is the value of error code string returned by Oi to be analized
	 * and decomposed in error code string and value. It is set by Om when
	 * creating/modifying/deleting on failure.
	 *
	 */
	std::string exitCodeStringRetrieved;
	/*end added for Ticket 1904*/

	/**Set the admistrativeOwner on the parent of object**/
	SaNameT objOwnedName;
	const SaNameT *objOwnedNameList[2];

};

#endif /* ACS_APGCC_OMHANDLERIMPL_H_ */
