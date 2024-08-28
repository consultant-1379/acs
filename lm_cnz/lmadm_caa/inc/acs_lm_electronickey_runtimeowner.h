#ifndef ACS_LM_ELECTRONICKEY_OBJECTIMPLEMENTER_H
#define ACS_LM_ELECTRONICKEY_OBJECTIMPLEMENTER_H
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <saImmOi.h>
#include <saImm.h>

#include <poll.h>
#include <ace/ACE.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_lm_clienthandler.h>
#include <ACS_CC_Types.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include "acs_lm_defs.h"
#include "acs_lm_sentinel.h"

class ACS_LM_ElectronicKey_RuntimeOwner : public ACS_APGCC_RuntimeOwner_V2
{
	/*=====================================================================
                           PUBLIC DECLARATION SECTION
     ==================================================================== */
public:
	/*===================================================================
		   							   PUBLIC METHOD
	=================================================================== */
	//*=================================================================== */
	/** @brief ACS_LM_ElectronicKey_RuntimeOwner Default constructor
	 *
	 *      ACS_LM_ElectronicKey_RuntimeOwner Constructor of class
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	ACS_LM_ElectronicKey_RuntimeOwner();
	//*=================================================================== */
	/** @brief ACS_LM_ElectronicKey_RuntimeOwner Destructor
	 *
	 *      ACS_LM_ElectronicKey_RuntimeOwner Destructor of class
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	~ACS_LM_ElectronicKey_RuntimeOwner();
	//*=================================================================== */
	/**
	 * @brief updateCallback method
	 * updateCallback method: This is a base class method.
	 * This method will be called as a callback when an non-cached attribute of a runtime Object
	 * is updated.
	 *
	 * @param  objName							:   const char
	 * 												the distinguished name
	 * 												of the object for which the
	 * 												update is requested.
	 *
	 * @param  attrName							:  	const char
	 * 												the name of the attribute
	 * 											    for which values must be updated.
	 *
	 * @return ACS_CC_ReturnType                 : On success ACS_CC_SUCCESS
	 * 											   on Failure ACS_CC_FAILURE
	 */
	//*=================================================================== */
	ACS_CC_ReturnType updateCallback(const char* objName, const char** attrName);
	//*=================================================================== */
	/**
	 *  @brief adminOperationCallback
	 *
	 *  @param oiHandle 					: ACS_APGCC_OiHandle
	 *
	 *  @param invocation 					: ACS_APGCC_InvocationType
	 *
	 *  @param p_objName 					: const char pointer
	 *
	 *  @param operationId 					: ACS_APGCC_AdminOperationIdType
	 *
	 *  @param paramList 					: ACS_APGCC_AdminOperationParamType
	 *
	 *  @return void
	 **/
	//*=================================================================== */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_InvocationType invocation,
			const char* p_objName,
			ACS_APGCC_AdminOperationIdType operationId,
			ACS_APGCC_AdminOperationParamType**paramList);
	//*=================================================================== */
	/**
	 *  @brief initElectronicKeyLMRunTimeOwner

	 *  @return bool                              :true/false.
	 **/
	//*=================================================================== */
	bool initElectronicKeyLMRunTimeOwner();
	//*=================================================================== */
	/**
	 *  @brief finalizeElectronicKeyLMRunTimeOwner

	 *  @return bool                              :true/false.
	 **/
	//*=================================================================== */
	bool finalizeElectronicKeyLMRunTimeOwner();
	//*=================================================================== */
	/**
	 *  @brief setInternalOMhandler
	 *
		@param aOmHandlerPtr 					: OmHandler pointer

	 *  @return void
	 **/
	//*=================================================================== */
	void setInternalOMhandler(OmHandler*  aOmHandlerPtr);
	//*=================================================================== */
	/**
	 *  @brief createRuntimeObjectElectronicKey
	 *
	 *  @param aRDNName                          : string
	 *
	 *  @return ACS_CC_ReturnType
	 **/
	//*=================================================================== */
	ACS_CC_ReturnType createRuntimeObjectElectronicKey(std::string& aRDNName);
	//*=================================================================== */
	/**
	 *  @brief deleteRuntimeObjectElectronicKey
	 *
	 *  @param aRDNName                          : string
	 *
	 *  @return ACS_CC_ReturnType
	 **/
	//*=================================================================== */
	ACS_CC_ReturnType deleteRuntimeObjectElectronicKey(std::string& aRDNName);
	//*=================================================================== */
	/**
	 *  @brief setSentinelObjectPtr
	 *
	 *  @param aSentinelptr                      : ACS_LM_Sentinel
	 *
	 *  @return void
	 **/
	//*=================================================================== */
	void setSentinelObjectPtr(ACS_LM_Sentinel* aSentinelptr);
	//*=================================================================== */
	/**
	 *  @brief clearAllElectronicKeysRunTimeObjectsinIMM

	 *  @return bool							:true,false
	 **/
	//*=================================================================== */
	bool clearAllElectronicKeysRunTimeObjectsinIMM();
	//*=================================================================== */
	/**
	 *  @brief clearLisenceKeyList

	 *  @return bool							:true,false
	 **/
	//*=================================================================== */
	void clearLisenceKeyList();
	//*=================================================================== */
	/**
	 *  @brief createRuntimeObjectElectronicKeyObjects

	 *  @return bool							:true,false
	 **/
	//*=================================================================== */
	bool createRuntimeObjectElectronicKeyObjects();
	//*=================================================================== */
	/**
	 *  @brief getSerialNumberRDNObject
	 *
	 *  @param aOBJName 						: const char pointer
	 *
	 *  @return int
	 **/
	//*=================================================================== */
	string getSerialNumberRDNObject(const char* aOBJName);
	//*=================================================================== */
	/**
	 *  @brief getLicenseKeyObject
	 *
	 *  @param aLicenseSerialNumber 			: int
	 *
	 *  @return LkData
	 **/
	//*=================================================================== */
	LkData* getLicenseKeyObject(string aLicenseSerialNumber);
	//*=================================================================== */
	/**
	 *  @brief updateElectronicKeyRDNList

	 *  @return bool							:true,false
	 **/
	//*=================================================================== */
	bool updateElectronicKeyRDNList();
	//*=================================================================== */
	/** 	@brief daysToExpire method
	 *
	 *      calculate the number of days to expire
	 *
	 *      @param 	expDate
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	ACE_INT64 daysToExpire(string expDate);

private:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */
	OmHandler*  theOmHandlerPtr;
	ACS_LM_Sentinel* theSentinelptr;
	std::list<LkData*> theLKlist;
	std::list<std::string> theElectronicKeyRDNList;
};

class ACS_LM_ElectronicKey_RuntimeHandlerThread : public ACE_Task_Base
{
public :
	/*===================================================================
								 PUBLIC ATTRIBUTE
	=================================================================== */
	int m_StopEvent;

	/*===================================================================
			   							   PUBLIC METHOD
	=================================================================== */
	//*=================================================================== */
	/** @brief ACS_LM_ElectronicKey_RuntimeHandlerThread        :Default constructor
	 *
	 *   ACS_LM_ElectronicKey_RuntimeHandlerThread Constructor of class
	 **/
	//*=================================================================== */

	ACS_LM_ElectronicKey_RuntimeHandlerThread();
	//*=================================================================== */
	/** @brief ACS_LM_ElectronicKey_RuntimeHandlerThread constructor for initializing Implementer
	 *
	 *      ACS_LM_ElectronicKey_RuntimeHandlerThread Constructor of class
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	ACS_LM_ElectronicKey_RuntimeHandlerThread(ACS_LM_ElectronicKey_RuntimeOwner *pImpl);
	//*=================================================================== */
	/** @brief ACS_LM_ElectronicKey_RuntimeHandlerThread destructor
	 *
	 *      ACS_LM_ElectronicKey_RuntimeHandlerThread destructor of class
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	~ACS_LM_ElectronicKey_RuntimeHandlerThread();
	//*=================================================================== */
	/** 	@brief setImpl method
	 *
	 *      setImpl for setting implementer
	 *
	 *      @param 	pImpl							:ACS_LM_ElectronicKey_RuntimeOwner
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	void setImpl(ACS_LM_ElectronicKey_RuntimeOwner *pImpl);
	//*=================================================================== */
	/** 	@brief stop method
	 *
	 *      stop for stopping the thread
	 *
	 *      @remarks Remarks
	 */
	//*=================================================================== */
	bool stop();
	//*=================================================================== */
	/**
	 * @brief  svc

	 *  @return int
	 **/
	//*=================================================================== */
	int svc(void);

private:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */
	ACS_LM_ElectronicKey_RuntimeOwner *theElectronicKeyRuntimeOwnerImplementer;
	bool theIsStop;


};
// ACS_LM_ElectronicKey_RuntimeHandlerThread END
#endif



