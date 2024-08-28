/*=================================================================== */
/**
   @file acs_ssu_runtimeowner.h

   This module contains the implementation for ObjectImplementer for SSU FolderQuota Objects.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#ifndef ACS_SSU_RUNTIMEOWNER_H
#define ACS_SSU_RUNTIMEOWNER_H

#include <ace/ACE.h>
#include <ace/Task.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include "acs_ssu_folder_quota_mon.h"

class ACS_SSU_FolderQuotaMon;

class ACS_SSU_RuntimeOwner : public ACS_APGCC_RuntimeOwner_V2
{
public:
	/** @brief ACS_SSU_RuntimeHandler Default constructor
	*
	*	ACS_SSU_RuntimeHandler Constructor of class
	*
	*	@remarks Remarks
	*/
	ACS_SSU_RuntimeOwner();
	/** @brief ACS_SSU_RuntimeHandler Destructor
	*
	*	ACS_SSU_RuntimeHandler Destructor of class
	*
	*	@remarks Remarks
	*/
	~ACS_SSU_RuntimeOwner();
	/**
	* @brief updateCallback method
	* updateCallback method: This is a base class method.
	* This method will be called as a callback when an non-cached attribute of a runtime Object
	* is updated.
	*
	* @param  objName:	the distinguished name of the object for which the update is requested.
	*
	* @param  attrName:	the name of the attribute for which values must be updated.
	*
	* @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	*/
	ACS_CC_ReturnType updateCallback(const char* objName, const char** attr_name);
	/**
	* @brief adminOperationResult method: This method contains logic for admin actions.
	*
	* @param oiHandle: input param the oi handle
	*
	* @param invocation: input param the invocation
	*
	* @param result : input param the result to be passed to IMM as administrative operation result.
	*
	* @return ACS_CC_ReturnType . On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	*/
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
	/**
	* @brief setFolderQuotaMonPointer method: Sets Folder Quota monitor.
	*
	* @param aPtr: input param the ACS_SSU_FolderQuotaMon pointer
	* @return void.
	*/
	void setFolderQuotaMonPointer(ACS_SSU_FolderQuotaMon* aPtr);

private:
	/*=================================================================== */
	/** @brief   theFolderQuotaMonObject.
	*/
	/*=================================================================== */
	ACS_SSU_FolderQuotaMon* theFolderQuotaMonObject;
};


class ACS_SSU_RtHanThread : public ACE_Task_Base 
{
public :
	/** @brief ACS_SSU_RtHanThread Default constructor
	*
	*	ACS_SSU_RtHanThread Constructor of class
	*
	*	@remarks Remarks
	*/
	ACS_SSU_RtHanThread();
	/** @brief ACS_SSU_RtHanThread constructor for initializing Implementer
	*
	*	ACS_SSU_RtHanThread Constructor of class
	*
	*	@remarks Remarks
	*/
	ACS_SSU_RtHanThread(ACS_SSU_RuntimeOwner *impl);
	/** @brief ACS_SSU_RtHanThread destructor
	*
	*	ACS_SSU_RtHanThread destructor of class
	*
	*	@remarks Remarks
	*/
	~ACS_SSU_RtHanThread();
	/** @brief setImpl method
	*
	*	setImpl for setting implementer
	*
	*	@remarks Remarks
	*/
	void setImpl(ACS_SSU_RuntimeOwner *impl);
	/** @brief stop method
	*
	*	stop for stopping the thread
	*
	*	@remarks Remarks
	*/
	void stop();
	/** @brief svc method
	*/
	int svc(void);

private:

	/*=================================================================== */
	/** @brief   implementer.
	*/
	/*=================================================================== */
	ACS_SSU_RuntimeOwner *implementer;
	/*=================================================================== */
	/** @brief   isStop.
	*/
	/*=================================================================== */
	bool isStop;

};

#endif /* ACS_SSU_RUNTIMEOWNER_H */
