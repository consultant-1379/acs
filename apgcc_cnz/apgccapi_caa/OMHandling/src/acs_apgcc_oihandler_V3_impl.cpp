/*
 * acs_apgcc_oihandler_V3_impl.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_oihandler_V3_impl.h"
#include "acs_apgcc_objectimplementereventhandler_V3.h"
#include "acs_apgcc_error.h"

#include <iostream>
//da controllare #include "acs_apgcc_oihandler_impl.h"
// #include "ACS_APGCC_ObjectImplementer_EventHandler.h"
#include "ACS_TRA_trace.h"



namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_oihandler_V3"), const_cast<char *>("C300"));
}



map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *> acs_apgcc_oihandler_V3_impl::implementerList;
ACE_Recursive_Thread_Mutex acs_apgcc_oihandler_V3_impl::_acs_apgcc_Mutex_implementerList;


acs_apgcc_oihandler_V3_impl::acs_apgcc_oihandler_V3_impl(){

	log.Open("APGCC");

}



acs_apgcc_oihandler_V3_impl::acs_apgcc_oihandler_V3_impl(acs_apgcc_oihandler_V3_impl *oiHanlderImp){

	ACE_UNUSED_ARG(oiHanlderImp);
	log.Open("APGCC");

}



ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::addObjectImpl(acs_apgcc_objectimplementerinterface_V3 *obj){

	SaAisErrorT errorCode;

	SaVersionT version;

	/** Set the IMM version: **/
	version.releaseCode  = 'A';
	version.majorVersion =  2;
	//version.minorVersion =  1;
	//version.minorVersion =  11; /*Ticket 1904*/
	version.minorVersion =  14; /*To increase OI timeout*/

	SaImmOiCallbacksT_2 immCallbacks;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/
	immCallbacks.saImmOiCcbObjectCreateCallback = acs_apgcc_oihandler_V3_impl::CreateCallback;
	immCallbacks.saImmOiCcbObjectModifyCallback = acs_apgcc_oihandler_V3_impl::ModifyCallback;
	immCallbacks.saImmOiCcbObjectDeleteCallback = acs_apgcc_oihandler_V3_impl::DeleteCallback;
	immCallbacks.saImmOiCcbCompletedCallback = acs_apgcc_oihandler_V3_impl::CompletedCallback;
	immCallbacks.saImmOiCcbApplyCallback = acs_apgcc_oihandler_V3_impl::ApplyCallback;
	immCallbacks.saImmOiCcbAbortCallback = acs_apgcc_oihandler_V3_impl::AbortCallback;

	immCallbacks.saImmOiRtAttrUpdateCallback = acs_apgcc_oihandler_V3_impl::OiRtAttUpdateCallback;

	immCallbacks.saImmOiAdminOperationCallback = acs_apgcc_oihandler_V3_impl::adminOperationCallback;

	SaImmOiHandleT immOiHandle;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks , &version);

	if ( errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addObjectImpl():saImmOiInitialize_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	/**Set the immOiHandle returned by the IMM Service in the instance of
	 * acs_apgcc_objectimplementerinterface_V3 passed as parameter**/

	/* The following line has been moved after. Only if all interaction with IMM are successful
	 * the immhandle is set into obj */

	//obj->setImmOiHandle(immOiHandle);

	SaImmOiImplementerNameT implementerName;

	std::string impName = obj->getImpName();
	implementerName = const_cast<char*>(impName.c_str());

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	errorCode = saImmOiImplementerSet(immOiHandle, implementerName);

	if( errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addObjectImpl():saImmOiImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		/*TR HR34245 and HR37848: START - Added to release the oihandle in case of failure */
		errorCode = saImmOiFinalize(immOiHandle);

		if (errorCode != SA_AIS_OK){

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_oihandler_V3::addObjectImpl():saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

		}
		/*TR HR34245 and HR37848: END - Added to release the oihandle in case of failure */

		return ACS_CC_FAILURE;
	}

	SaNameT objName;
	objName.length = obj->getObjName().length();
	objName.value[obj->getObjName().copy(reinterpret_cast<char *>(objName.value), SA_MAX_NAME_LENGTH)] = '\0';

	SaImmScopeT scope = (SaImmScopeT) obj->getScope();

	/** this function informs the IMM Service that the object identified by the scope
	 * and obname parameters are implemented by the Object implementer whose name has
	 * been associated with the handle immOihandle**/
	errorCode = saImmOiObjectImplementerSet(immOiHandle, &objName, scope);

	if( errorCode != SA_AIS_OK ){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addObjectImpl():saImmOiObjectImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		/*TR HR34245 and HR37848: START - Added to release the oihandle in case of failure */
		Finalize(immOiHandle);
		/*TR HR34245 and HR37848: END - Added to release the oihandle in case of failure */

		return ACS_CC_FAILURE;
	}

	SaSelectionObjectT selObj;

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj);

	if( errorCode != SA_AIS_OK ){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addObjectImpl():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/* Moved here */
	obj->setImmOiHandle(immOiHandle);

	obj->setSelObj(selObj);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

	/** Add a new element at the map**/
	implementerList[obj->getImmOiHandle()] = obj;

	return ACS_CC_SUCCESS;
}



ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::addClassImpl(acs_apgcc_objectimplementerinterface_V3 *obj, const char * className){

	SaAisErrorT errorCode;

	SaVersionT version;

	/** Set the IMM version: **/
	version.releaseCode  = 'A';
	version.majorVersion =  2;
	//version.minorVersion =  1;
	version.minorVersion =  11;

	SaImmOiCallbacksT_2 immCallbacks;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/
	immCallbacks.saImmOiCcbObjectCreateCallback = acs_apgcc_oihandler_V3_impl::CreateCallback;
	immCallbacks.saImmOiCcbObjectModifyCallback = acs_apgcc_oihandler_V3_impl::ModifyCallback;
	immCallbacks.saImmOiCcbObjectDeleteCallback = acs_apgcc_oihandler_V3_impl::DeleteCallback;
	immCallbacks.saImmOiCcbCompletedCallback = acs_apgcc_oihandler_V3_impl::CompletedCallback;
	immCallbacks.saImmOiCcbApplyCallback = acs_apgcc_oihandler_V3_impl::ApplyCallback;
	immCallbacks.saImmOiCcbAbortCallback = acs_apgcc_oihandler_V3_impl::AbortCallback;

	/**TO TEST*/
	immCallbacks.saImmOiRtAttrUpdateCallback = acs_apgcc_oihandler_V3_impl::OiRtAttUpdateCallback;

	immCallbacks.saImmOiAdminOperationCallback = acs_apgcc_oihandler_V3_impl::adminOperationCallback;

	SaImmOiHandleT immOiHandle;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks , &version);

	if ( errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addClassImpl():saImmOiInitialize_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	/* The following line has been moved after. Only if all interaction with IMM are successful
	 * the immhandle is set into obj */

	//obj->setImmOiHandle(immOiHandle);

	SaImmOiImplementerNameT implementerName;
	std::string impName = obj->getImpName();

	implementerName = const_cast<char*>(impName.c_str());

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	errorCode = saImmOiImplementerSet(immOiHandle, implementerName);

	if( errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addClassImpl():saImmOiImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		/*TR HR34245 and HR37848: START - Added to release the oihandle in case of failure */
		errorCode = saImmOiFinalize(immOiHandle);

		if (errorCode != SA_AIS_OK){

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_oihandler_V3::addClassImpl():saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

		}
		/*TR HR34245 and HR37848: END - Added to release the oihandle in case of failure */

		return ACS_CC_FAILURE;
	}

	/**This function informs the IMM serivice that all the objects that are istances of the
	 * object class whose name is specified by className parameter are implemented by
	 * Object Implmeneter whose name has associated with the handle immOiHandle**/
	errorCode = saImmOiClassImplementerSet(immOiHandle, const_cast<char *>(className));

	if( errorCode != SA_AIS_OK ){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addClassImpl():saImmOiClassImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*TR HR34245 and HR37848: START - Added to release the oihandle in case of failure */
		Finalize(immOiHandle);
		/*TR HR34245 and HR37848: END - Added to release the oihandle in case of failure */

		return ACS_CC_FAILURE;
	}

	SaSelectionObjectT selObj;

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj);

	if( errorCode != SA_AIS_OK ){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addClassImpl():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/* The above line moved here */
	obj->setImmOiHandle(immOiHandle);

	obj->setSelObj(selObj);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

	/** Add a new element at the map**/
	implementerList[obj->getImmOiHandle()] = obj;

	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::addMultipleClassImpl(acs_apgcc_objectimplementerinterface_V3 *p_obj, vector<string> p_classNameVec){

//	ACE_UNUSED_ARG(p_obj);
//	ACE_UNUSED_ARG(p_classNameVec);

	SaAisErrorT errorCode;

	if(p_obj == 0){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE 7");

		log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	if((int)p_classNameVec.size() == 0){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE 7");

		log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	SaVersionT version;

	/** Set the IMM version: **/
	version.releaseCode  = 'A';
	version.majorVersion =  2;
	//version.minorVersion =  1;
	version.minorVersion =  11;

	SaImmOiCallbacksT_2 immCallbacks;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/
	immCallbacks.saImmOiCcbObjectCreateCallback = acs_apgcc_oihandler_V3_impl::CreateCallback;
	immCallbacks.saImmOiCcbObjectModifyCallback = acs_apgcc_oihandler_V3_impl::ModifyCallback;
	immCallbacks.saImmOiCcbObjectDeleteCallback = acs_apgcc_oihandler_V3_impl::DeleteCallback;
	immCallbacks.saImmOiCcbCompletedCallback = acs_apgcc_oihandler_V3_impl::CompletedCallback;
	immCallbacks.saImmOiCcbApplyCallback = acs_apgcc_oihandler_V3_impl::ApplyCallback;
	immCallbacks.saImmOiCcbAbortCallback = acs_apgcc_oihandler_V3_impl::AbortCallback;

	/**TO TEST*/
	immCallbacks.saImmOiRtAttrUpdateCallback = acs_apgcc_oihandler_V3_impl::OiRtAttUpdateCallback;

	immCallbacks.saImmOiAdminOperationCallback = acs_apgcc_oihandler_V3_impl::adminOperationCallback;

	SaImmOiHandleT immOiHandle;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks , &version);

	if ( errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():saImmOiInitialize_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	/* The following line has been moved after. Only if all interaction with IMM are successful
	 * the immhandle is set into obj */

	//p_obj->setImmOiHandle(immOiHandle);

	SaImmOiImplementerNameT implementerName;
	std::string impName = p_obj->getImpName();

	implementerName = const_cast<char*>(impName.c_str());

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	errorCode = saImmOiImplementerSet(immOiHandle, implementerName);

	if( errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():saImmOiImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		errorCode = saImmOiFinalize(immOiHandle);

		if (errorCode != SA_AIS_OK){

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_oihandler_V3::Finalize():addMultipleClassImpl:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

		}

		return ACS_CC_FAILURE;
	}

	for(int i=0; i<(int)p_classNameVec.size(); i++){

		/**This function informs the IMM serivice that all the objects that are istances of the
		 * object class whose name is specified by className parameter are implemented by
		 * Object Implmeneter whose name has associated with the handle immOiHandle**/
		errorCode = saImmOiClassImplementerSet(immOiHandle, const_cast<char*>(p_classNameVec[i].c_str()));

		if( errorCode != SA_AIS_OK ){
			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():saImmOiClassImplementerSet:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			for(int j=0; j<i; j++){
				errorCode = saImmOiClassImplementerRelease(immOiHandle, const_cast<char *>(p_classNameVec[j].c_str()));
				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():saImmOiClassImplementerRelease:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}

			Finalize(immOiHandle);

			return ACS_CC_FAILURE;
		}
	}

	SaSelectionObjectT selObj;

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj);

	if( errorCode != SA_AIS_OK ){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::addMultipleClassImpl():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/* The above line moved here */
	p_obj->setImmOiHandle(immOiHandle);

	p_obj->setSelObj(selObj);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

	/** Add a new element at the map**/
	implementerList[p_obj->getImmOiHandle()] = p_obj;

	return ACS_CC_SUCCESS;

}



ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::removeObjectImpl(acs_apgcc_objectimplementerinterface_V3 *obj){

	SaAisErrorT errorCode;

	SaNameT objName;
	objName.length = obj->getObjName().length();
	objName.value[obj->getObjName().copy(reinterpret_cast<char *>(objName.value), SA_MAX_NAME_LENGTH)] = '\0';

	SaUint64T handle=obj->getImmOiHandle();

	SaImmScopeT scope = (SaImmScopeT) obj->getScope();

	/** This function informs the IMM Service that the implementer whose name is
	 * associated  with the IMM handle must no longer be considered as the
	 * implementer of the set of object identified by scope and the name to whith
	 * obname ponts  **/
	errorCode = saImmOiObjectImplementerRelease(handle, &objName, scope);

	if (errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::removeObjectImpl():saImmOiObjectImplementerRelease:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/** If the object obj is instance of class ACS_APGCC_ObjectImplementer_EventHandler is necessary
	 * to call the method release_handle() to remove the handle from teh reactor**/
	//V3 ACS_APGCC_ObjectImplementer_EventHandler * p = dynamic_cast<ACS_APGCC_ObjectImplementer_EventHandler *>(obj);
	acs_apgcc_objectimplementereventhandler_V3 * p = dynamic_cast<acs_apgcc_objectimplementereventhandler_V3 *>(obj);
	if(p){
		p->release_handle();
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

	/** Remove the obj element from the map**/
	implementerList.erase(obj->getImmOiHandle());

	/** this method release all the handle retrieved during the initialization  **/
	Finalize(obj->getImmOiHandle());

	return ACS_CC_SUCCESS;
}



ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::removeClassImpl(acs_apgcc_objectimplementerinterface_V3 *obj, const char * className ){

	SaAisErrorT errorCode;

	SaUint64T handle = obj->getImmOiHandle();

	/** This function informs the IMM Service that the implementer whose name is associated
	 * with the handle immOiHandle must not be anymore as the implementer of the object that
	 * are instances of the object class whose name is specified by className**/
	errorCode = saImmOiClassImplementerRelease(handle, const_cast<char *>(className));

	if (errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::removeClassImpl():saImmOiClassImplementerRelease:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/** If the object obj is instance of class ACS_APGCC_ObjectImplementer_EventHandler is necessary
	 * to call the method release_handle() to remove the handle from teh reactor**/
	acs_apgcc_objectimplementereventhandler_V3 * p = dynamic_cast<acs_apgcc_objectimplementereventhandler_V3 *>(obj);
	if(p){
		p->release_handle();
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

	/** Remove the obj element from the map**/
	implementerList.erase(obj->getImmOiHandle());

	/** this method release all the handle retrieved during the initialization  **/
	Finalize(obj->getImmOiHandle());

	return ACS_CC_SUCCESS;

}


//TO BE DEFINE
ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::removeMultipleClassImpl(acs_apgcc_objectimplementerinterface_V3 *p_obj, vector<string> p_classNameVec ){

//	ACE_UNUSED_ARG(p_obj);
//	ACE_UNUSED_ARG(p_classNameVec);

	SaAisErrorT errorCode;

	if(p_obj == 0){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE 7");

		log.Write("acs_apgcc_oihandler_V3::removeMultipleClassImpl():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	SaUint64T handle = p_obj->getImmOiHandle();

	if((int)p_classNameVec.size() == 0){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE 7");

		log.Write("acs_apgcc_oihandler_V3::removeMultipleClassImpl():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	for(int i=0; i<(int)p_classNameVec.size(); i++){

		/** This function informs the IMM Service that the implementer whose name is associated
		 * with the handle immOiHandle must not be anymore as the implementer of the object that
		 * are instances of the object class whose name is specified by className**/
		errorCode = saImmOiClassImplementerRelease(handle, const_cast<char *>(p_classNameVec[i].c_str()));

		if (errorCode != SA_AIS_OK){
			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_oihandler_V3::removeMultipleClassImpl():saImmOiClassImplementerRelease:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

	}

	/** If the object obj is instance of class ACS_APGCC_ObjectImplementer_EventHandler is necessary
	 * to call the method release_handle() to remove the handle from teh reactor**/
	acs_apgcc_objectimplementereventhandler_V3 * p = dynamic_cast<acs_apgcc_objectimplementereventhandler_V3 *>(p_obj);
	if(p){
		p->release_handle();
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

	/** Remove the obj element from the map**/
	implementerList.erase(p_obj->getImmOiHandle());

	/** this method release all the handle retrieved during the initialization  **/
	Finalize(p_obj->getImmOiHandle());

	return ACS_CC_SUCCESS;

}


ACS_CC_ReturnType acs_apgcc_oihandler_V3_impl::Finalize (SaUint64T immHandle ){

	SaAisErrorT errorCode;

	/* clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer for the name previously associated
	 *
	 * immOiHandle is the handle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 function
	 * */
	errorCode = saImmOiImplementerClear(immHandle);

	if (errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::Finalize():saImmOiImplementerClear:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/* closes the association represented by the immOiHandle parameter between the
	 * invoking process and the information Model
	 *
	 * immOiHandle is the ahandle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 and witch identifies this particular initization of the
	 * IMM service
	 * */
	errorCode = saImmOiFinalize(immHandle);

	if (errorCode != SA_AIS_OK){
		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_oihandler_V3::Finalize():saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}



SaAisErrorT acs_apgcc_oihandler_V3_impl::CreateCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,
		const SaImmClassNameT className, const SaNameT *parentName, const SaImmAttrValuesT_2 **attr){

	ACS_CC_ReturnType errorCode;

	/**Find in the map the implementer associated at the immOiHandle**/

	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return SA_AIS_ERR_BAD_OPERATION; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return SA_AIS_ERR_BAD_OPERATION;
			}
		}
		/*END TR HQ36463*/
	}

	char *cName = new char[strlen(className)+1];
	strcpy(cName, className);

	char *pName = new char[(parentName->length)+1];
	ACE_OS::memcpy(pName, parentName->value, parentName->length);
	pName[parentName->length] = '\0';

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;
	while(attr[i]){
		i++;
		dim++;
	}

	/*Prepares data to be returned to calling the create function of implementer*/
	ACS_APGCC_AttrValues *myAttributeList[dim+1];
	ACS_APGCC_AttrValues oneAttr[dim];


	i=0;
	while(attr[i]){

		oneAttr[i].attrName = attr[i]->attrName;

		oneAttr[i].attrType = (ACS_CC_AttrValueType)(attr[i]->attrValueType);
		oneAttr[i].attrValuesNum = attr[i]->attrValuesNumber;
		oneAttr[i].attrValues = new void *[attr[i]->attrValuesNumber];

		if(attr[i]->attrValueType == SA_IMM_ATTR_SASTRINGT){
			for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(attr[i]->attrValues[j]));
				oneAttr[i].attrValues[j] = reinterpret_cast<char*>(stringVal);
			}

		}else if (attr[i]->attrValueType == SA_IMM_ATTR_SANAMET){
			for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				SaNameT* localSaName;
				localSaName = reinterpret_cast<SaNameT*>(attr[i]->attrValues[j]);
				oneAttr[i].attrValues[j] = localSaName->value;
			}

		}else {
			for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				oneAttr[i].attrValues[j] = attr[i]->attrValues[j];
			}
		}

		myAttributeList[i] = &(oneAttr[i]);
		i++;
	}

	/**the myAttributeList must be a null terminated array**/
	myAttributeList[dim] = 0;

	/**call the create function of Implementer**/
	errorCode = obj->create(immOiHandle, ccbId, cName, pName, myAttributeList);

	for(int i=0; i<dim; i++){
		delete[] oneAttr[i].attrValues;
	}

	delete[] cName;
	delete[] pName;

//OI_OM_ERROR_COM/*return code*/
//	SaAisErrorT retVal;
//	retVal = obj->getExitCode();
//	//cout<< "CREATE obj->getExitCode()= "<<retVal<<endl;
///*return code end*/

	if(errorCode != ACS_CC_SUCCESS){
		/**
		 * TO ADD: related to OpenSaf Ticket 1904
		 * When new immsv API saImmOiCcbSetErrorString() will be available
		 * obj->getExitCodeString(); should be called to retrieve the string to send then the new
		 * API saImmOiCcbSetErrorString () should be called.
		 */

		/*for compliance OpenSaf 1904 start*/
		SaAisErrorT retVal;
		retVal =saImmOiCcbSetErrorString (immOiHandle, ccbId, const_cast<char*>((obj->getExitCodeString()).c_str()) );
		//cout<< "After saImmOiCcbSetErrorString : string to be set: "<<obj->getExitCodeString()<<endl;

		/*for compliance OpenSaf 1904 end*/

		return SA_AIS_ERR_BAD_OPERATION;
		//return retVal;

	}else
		return SA_AIS_OK;

}



SaAisErrorT acs_apgcc_oihandler_V3_impl::DeleteCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,
		const SaNameT *objectName){

	ACS_CC_ReturnType errorCode;

	/**Find in the map the implementer associated at the immOiHandle**/
	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return SA_AIS_ERR_BAD_OPERATION; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return SA_AIS_ERR_BAD_OPERATION;
			}
		}
		/*END TR HQ36463*/
	}

	char *objName = new char[(objectName->length)+1];
	ACE_OS::memcpy(objName, objectName->value, objectName->length);
	objName[objectName->length] = '\0';

	/**call the deleted function of Implementer**/
	errorCode = obj->deleted(immOiHandle, ccbId, objName);

	delete[] objName;

//	/*return code*/
//		SaAisErrorT retVal;
//		retVal = obj->getExitCode();
//		//cout<< " DELETE obj->getExitCode()= "<<retVal;
//	/*return code end*/


	if(errorCode != ACS_CC_SUCCESS){
		/**
		 * TO ADD: related to OpenSaf Ticket 1904
		 * When new immsv API saImmOiCcbSetErrorString() will be available
		 * obj->getExitCodeString(); should be called to retrieve the string to send then the new
		 * API saImmOiCcbSetErrorString () should be called.
		 */
		/*for compliance OpenSaf 1904 start*/
		SaAisErrorT retVal;
		retVal = saImmOiCcbSetErrorString (immOiHandle, ccbId, const_cast<char*>((obj->getExitCodeString()).c_str()) );

		//cout<< "After saImmOiCcbSetErrorString : string to be set: "<<obj->getExitCodeString()<<endl;

		/*for compliance OpenSaf 1904 end*/
		return SA_AIS_ERR_BAD_OPERATION;
		//return retVal;

	}else
		return SA_AIS_OK;

}



SaAisErrorT acs_apgcc_oihandler_V3_impl::ModifyCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,
		const SaNameT *objectName,
		const SaImmAttrModificationT_2 **attrMods){

	ACS_CC_ReturnType errorCode;

	/**Find in the map the implementer associated at the immOiHandle**/

	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return SA_AIS_ERR_BAD_OPERATION; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return SA_AIS_ERR_BAD_OPERATION;
			}
		}
		/*END TR HQ36463*/
	}

	char *objName = new char[(objectName->length)+1];
	ACE_OS::memcpy(objName, objectName->value, objectName->length);
	objName[objectName->length] = '\0';

	/**Calculate the size of array of attribute descriptor**/
	int dim=0;
	int i=0;
	while(attrMods[i]){
		//std::cout<<"In while::ModifyCallback :: attrMods[i] = "<<attrMods[i]<<std::endl;
		i++;
		dim++;
		//std::cout<<"n while::ModifyCallback :: i="<<i<<" dim="<<dim<<std::endl;
	}

	/*Prepares data to be returned to calling the modify function of implementer*/
	ACS_APGCC_AttrModification *myAttrMods[dim+1];
	ACS_APGCC_AttrModification attr[dim];

	i=0;

	while(attrMods[i]){
		//std::cout<<"In while2::ModifyCallback :: attrMods["<<i<<"] = "<<attrMods[i]<<std::endl;
		if (attrMods[i]->modType == SA_IMM_ATTR_VALUES_ADD){
			attr[i].modType = ACS_APGCC_ATTR_VALUES_ADD;
		}else if (attrMods[i]->modType == SA_IMM_ATTR_VALUES_DELETE){
			attr[i].modType = ACS_APGCC_ATTR_VALUES_DELETE;
		}else {
			attr[i].modType = ACS_APGCC_ATTR_VALUES_REPLACE;
		}

		attr[i].modAttr.attrName = attrMods[i]->modAttr.attrName;
		//std::cout<<"In while2::ModifyCallback :: attrMods["<<i<<"]->modAttr.attrName = "<<attrMods[i]->modAttr.attrName<<std::endl;

		attr[i].modAttr.attrType = (ACS_CC_AttrValueType)(attrMods[i]->modAttr.attrValueType);
		attr[i].modAttr.attrValuesNum = attrMods[i]->modAttr.attrValuesNumber;
		//TR HP24467 start
		//attr[i].modAttr.attrValues = new void *[attr->modAttr.attrValuesNum];
		attr[i].modAttr.attrValues = new void *[attr[i].modAttr.attrValuesNum];
		//TR HP24467 end

		if(attrMods[i]->modAttr.attrValueType == SA_IMM_ATTR_SASTRINGT){
			//TR HP24467 start
			//for (unsigned int j = 0; j <  attr->modAttr.attrValuesNum; j++){
			for (unsigned int j = 0; j <  attr[i].modAttr.attrValuesNum; j++){
			//TR HP24467 end
				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(attrMods[i]->modAttr.attrValues[j]));
				attr[i].modAttr.attrValues[j] = reinterpret_cast<char*>(stringVal);
			}

		}else if (attrMods[i]->modAttr.attrValueType == SA_IMM_ATTR_SANAMET){
			//TR HP24467 start
			for (unsigned int j = 0; j <  attr[i].modAttr.attrValuesNum; j++){
			//TR HP24467 end
				SaNameT *parSaName = 0;//new SaNameT;
				parSaName = (reinterpret_cast<SaNameT*>(attrMods[i]->modAttr.attrValues[j]));
				attr[i].modAttr.attrValues[j] = reinterpret_cast<char *>(parSaName->value);
			}

		}else {
			//Problem Struct Password
			//for (unsigned int j = 0; j <  attr->modAttr.attrValuesNum; j++){
			for (unsigned int j = 0; j < attr[i].modAttr.attrValuesNum; j++){
			//end Problem struct Password
				//std::cout<<"In while2 for loop::ModifyCallback j = "<<j<<std::endl;
				attr[i].modAttr.attrValues[j] = attrMods[i]->modAttr.attrValues[j];
			}

		}

		myAttrMods[i] = &attr[i];
		i++;

	}

	/**the myAttrMods must be a null terminated array**/
	myAttrMods[dim] = 0;

	/**call the modify function of Implementer**/
	errorCode = obj->modify(immOiHandle, ccbId, objName, myAttrMods );


	/*to free allocate memory*/
	delete[] objName;

	for (int i=0; i < dim; i++ ){
		delete [] attr[i].modAttr.attrValues;
	}
//	/*return code*/
//		SaAisErrorT retVal;
//		retVal = obj->getExitCode();
//		//cout<< " MODIFY obj->getExitCode()= "<<retVal;
//	/*return code end*/


	if(errorCode != ACS_CC_SUCCESS){
		/**
		 * TO ADD: related to OpenSaf Ticket 1904
		 * When new immsv API saImmOiCcbSetErrorString() will be available
		 * obj->getExitCodeString(); should be called to retrieve the string to send then the new
		 * API saImmOiCcbSetErrorString () should be called.
		 */

		/*for compliance OpenSaf 1904 start*/
		SaAisErrorT retVal;
		retVal = saImmOiCcbSetErrorString (immOiHandle, ccbId, const_cast<char*>((obj->getExitCodeString()).c_str()) );
		//cout<< "After saImmOiCcbSetErrorString : string to be set: "<<obj->getExitCodeString()<<endl;

		/*for compliance OpenSaf1904 end*/

		return SA_AIS_ERR_BAD_OPERATION;
		//return retVal;

	}else
		return SA_AIS_OK;
}



SaAisErrorT acs_apgcc_oihandler_V3_impl::CompletedCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId){


	ACS_CC_ReturnType errorCode;

	/**Find in the map the implementer associated at the immOiHandle**/
	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return SA_AIS_ERR_BAD_OPERATION; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return SA_AIS_ERR_BAD_OPERATION;
			}
		}
		/*END TR HQ36463*/
	}


	/**call the complete function of Implementer**/
	errorCode = obj->complete(immOiHandle, ccbId);

//	/*return code*/
//		SaAisErrorT retVal;
//		retVal = obj->getExitCode();
//		//cout<< " COMPLETE obj->getExitCode()= "<<retVal;
//	/*return code end*/


	if(errorCode != ACS_CC_SUCCESS){
		/**
		 * TO ADD: related to OpenSaf Ticket 1904
		 * When new immsv API saImmOiCcbSetErrorString() will be available
		 * obj->getExitCodeString(); should be called to retrieve the string to send then the new
		 * API saImmOiCcbSetErrorString () should be called.
		 */

		/*for compliance 1904 start*/
		saImmOiCcbSetErrorString (immOiHandle, ccbId, const_cast<char*>((obj->getExitCodeString()).c_str()) );
		//cout<< "After saImmOiCcbSetErrorString : string to be set: "<<obj->getExitCodeString()<<endl;
		/*for compliance 1904 end*/

		return SA_AIS_ERR_BAD_OPERATION;
		//return retVal;
	}else
		return SA_AIS_OK;

}



void acs_apgcc_oihandler_V3_impl::AbortCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId){

	/**Find in the map the implementer associated at the immOiHandle**/
	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return;
			}
		}
		/*END TR HQ36463*/
	}

	/**call the abort function of Implementer**/
	obj->abort(immOiHandle, ccbId);
}



void acs_apgcc_oihandler_V3_impl::ApplyCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId){

	/**Find in the map the implementer associated at the immOiHandle**/
	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return ; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return;
			}
		}
		/*END TR HQ36463*/
	}

	/**call the apply function of Implementer**/
	obj->apply(immOiHandle, ccbId);
}



SaAisErrorT acs_apgcc_oihandler_V3_impl::OiRtAttUpdateCallback(SaImmOiHandleT immOiHandle,
			const SaNameT *objectName,
			const SaImmAttrNameT *attributeNames){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_oihandler_V3_impl::OiRtAttUpdateCallback(SaImmOiHandleT immOiHandle, const SaNameT *objectName, const SaImmAttrNameT *attributeNames)");
		trace.ACS_TRA_event(1, buffer);
	}

	ACS_CC_ReturnType errorCode;


	/**Find in the map the implementer associated at the immOiHandle**/
	/*TR HQ36463*/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return SA_AIS_ERR_BAD_OPERATION; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return SA_AIS_ERR_BAD_OPERATION;
			}
		}
		/*END TR HQ36463*/
	}

	char *nameObj=new char[(objectName->length) + 1];
	ACE_OS::memset( nameObj, '\0', (objectName->length) +1 );
	ACE_OS::memcpy(nameObj, objectName->value, objectName->length );







	//save a local copy of pointer of attribute name
	const SaImmAttrNameT *myPtr = attributeNames;

	//number of attribute to update
	int numAttr = 0;
	while( *attributeNames != 0){
		numAttr++;
		attributeNames++;
	}

	//Instance of char* vector
	char *nameAttrs[numAttr+1];
	attributeNames = myPtr;

	int i = 0;
	while( *attributeNames != 0){
		nameAttrs[i] = *attributeNames;
		i++;
		attributeNames++;
	}
	//close the vector with a null pointer
	nameAttrs[numAttr] = 0;

	//call the updateCallback defined by application
	errorCode = obj->updateRuntime(nameObj, (const char**)(nameAttrs));

	delete []nameObj;

//****************************************************
//****************************************************
//****************************************************

//	char *nameAttr=*attributeNames;
//
//	errorCode = obj->updateRuntime(nameObj, nameAttr);



	//****************************************************
	//****************************************************
	//****************************************************

	if(errorCode != ACS_CC_SUCCESS){
		return SA_AIS_ERR_BAD_OPERATION;
	}else
		return SA_AIS_OK;

}

/*adminoperation callback*/
void acs_apgcc_oihandler_V3_impl::adminOperationCallback(
		SaImmOiHandleT immOiHandle,
		SaInvocationT invocation,
		const SaNameT *objectName,
		SaImmAdminOperationIdT operationId,
		const SaImmAdminOperationParamsT_2 **params){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_oihandler_V3_impl::adminOperationCallback(SaImmOiHandleT immOiHandle,SaInvocationT invocation,const SaNameT *objectName,SaImmAdminOperationIdT operationId,const SaImmAdminOperationParamsT_2 **params)");
		trace.ACS_TRA_event(1, buffer);
	}


	/**Find in the map the implementer associated at the immOiHandle**/
	//acs_apgcc_objectimplementerinterface_V3 *obj = (acs_apgcc_objectimplementerinterface_V3 *)(acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle)->second);

	acs_apgcc_objectimplementerinterface_V3 *obj;

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_implementerList (_acs_apgcc_Mutex_implementerList);

		map<SaUint64T,acs_apgcc_objectimplementerinterface_V3 *>::iterator listIterator;
		listIterator = acs_apgcc_oihandler_V3_impl::implementerList.find(immOiHandle);
		/*check if found*/
		if (listIterator == acs_apgcc_oihandler_V3_impl::implementerList.end())
			return; /* not found: no implementer found for provided immOiHandle */
		else {
			/*found: associate to obj the pointer to Implementer*/
			if ( (obj = (acs_apgcc_objectimplementerinterface_V3 *)(listIterator->second)) ==0 ){
				/*found immOiHandle in map but related Implementer pointer is 0 so delete it from the map*/
				/** Remove the obj element from the map**/
				//implementerList.erase(immOiHandle);
				return;
			}
		}
	}

	/*set the object name to be passed*/
	char *nameObj = new char[(objectName->length) + 1];
	memset( nameObj, '\0', (objectName->length) +1 );
	memcpy(nameObj, objectName->value, objectName->length );

	/*create the list of params*/

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;
	while(params[i]){
		i++;
		dim++;
	}

	/*Prepares data to be provided to adminOperationCallback function*/
	ACS_APGCC_AdminOperationParamType *myAttributeList[dim+1];
	ACS_APGCC_AdminOperationParamType oneAttr[dim];

	i=0;
	while(params[i]){

		oneAttr[i].attrName = params[i]->paramName;

		oneAttr[i].attrType = (ACS_CC_AttrValueType)(params[i]->paramType);



		if(params[i]->paramType == SA_IMM_ATTR_SASTRINGT){

				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(params[i]->paramBuffer));
				oneAttr[i].attrValues = reinterpret_cast<char*>(stringVal);


		}else if (params[i]->paramType == SA_IMM_ATTR_SANAMET){
			//for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				SaNameT* localSaName;
				localSaName = reinterpret_cast<SaNameT*>(params[i]->paramBuffer);
				oneAttr[i].attrValues = localSaName->value;
			//}

		}else {
			//for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				oneAttr[i].attrValues = params[i]->paramBuffer;
			//}
		}

		myAttributeList[i] = &(oneAttr[i]);
		i++;
	}

	/**the myAttributeList must be a null terminated array**/
	myAttributeList[dim] = 0;

	/*end create list of params*/

	obj->adminOperationCallback(immOiHandle, (ACS_APGCC_InvocationType) invocation, nameObj, (ACS_APGCC_AdminOperationIdType)operationId, myAttributeList);

	delete []nameObj;

	/*to here*/
}
/*end adminoperation callback **/




std::string acs_apgcc_oihandler_V3_impl::getErrorText(int p_errorId){

	std::string s;

			switch (p_errorId)
			{
				case 0:				s.assign("OK");
				break;
				case 1:				s.assign("Error when executing (general fault)");
				break;
				case 2:				s.assign("Incorrect Usage");
				break;
				case 113:			s.assign("AP is not defined");
				break;
				case 115:			s.assign("Illegal command in this system configuration");
				break;
				case 116:			s.assign("Illegal option in this system configuration");
				break;
				case 117:			s.assign("Unable to connect to server");
				break;
				case 118:			s.assign("CP is not defined");
				break;
				case 119:			s.assign("Command partially executed");
				break;
				default:;

			}
			return s;

}



acs_apgcc_oihandler_V3_impl::~acs_apgcc_oihandler_V3_impl(){

	log.Close();
}


void acs_apgcc_oihandler_V3_impl::setInternalError(int p_errorCode){
	errorCode = p_errorCode;
}

int acs_apgcc_oihandler_V3_impl::getInternalLastError(){
	return ((-1)*errorCode);
}

char* acs_apgcc_oihandler_V3_impl::getInternalLastErrorText(){
	if((errorCode > 0) && (errorCode < ACS_APGCC_MAX_ERROR_NUMBER)){
		return errorArray[errorCode];
	}

	return NULL;
}
