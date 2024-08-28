/**
   @file    acs_rtr_statistics_impl.h

   @brief Header file for RTR module.
          It acts as OI for handling IMM Callbacks of RTR filebased object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012   XHARBAV   Initial Release
   N/A       18/12/2013   	QVINCON   	Re-factoring
 */

#ifndef ACS_RTR_STATISTICS_OBJECTIMPL_H
#define ACS_RTR_STATISTICS_OBJECTIMPL_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_apgcc_objectimplementereventhandler_V3.h>

#include <string>

class ACS_RTR_Server;

struct StatisticsInfo{
			uint64_t volumeRead;
			uint64_t volumeReported;
};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief ACS_RTR_Statistics_ObjectImpl
 * ACS_RTR_Statistics_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 */
class ACS_RTR_Statistics_ObjectImpl : public acs_apgcc_objectimplementereventhandler_V3
{
 public:

	/**
	 * @brief
	 * constructor with two parameters
	 */
	ACS_RTR_Statistics_ObjectImpl(ACS_RTR_Server* serverObject);

	/**
   	* @brief
   	* destructor
   	*/
	~ACS_RTR_Statistics_ObjectImpl();

	/**
	 *  create method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object created.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  className     : const char pointer
	 *  @param  parentname    : const char pointer
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	/**
	 *  deleted method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object deleted.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  objName
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	/**
	 *  modify method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  objName
	 *  @param  attrMods          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	/**
	 *  complete method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/**
	 *  abort method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/**
	 *  apply method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  objName          : const char pointer
	 *  @param  attrName         : const char pointer
	 */
	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

	/**
	 *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  invocation         : ACS_APGCC_InvocationType
	 *  @param  p_objName
	 *  @param  operationId
	 *  @param paramList
	 */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

	/** @brief getClassName method
	 *
	 * 	This method return the name of the IMM class.
	 *
	 *  @return const char pointer of the IMM class name
	 *
	 *  @remarks Remarks
	*/
	const char* getIMMClassName() const { return m_ImmClassName.c_str(); };
	
 private:

	/** @brief	updateRunTimeAttributes
	*
	*/
	void updateRunTimeAttributes(const char* objectDN, StatisticsInfo& statisticsInfo);

	/** @brief	getMessageStoreDN
	*
	*/
	bool getMessageDN(const std::string& objectDN, std::string& messageStoreDN);

	/** @brief	getMessageStoreName
	*
	*/
	bool getMessageStoreName(const std::string& messageStoreDN, std::string& messageStoreName);

	/** @brief m_ImmClassName
	 *
	 * The name of the IMM class.
	 *
	*/
	std::string m_ImmClassName;

	/**
	 * 	@brief	m_IsMultiCP
	 *
	 * 	Flag of system type (SCP/MCP)
	*/
	bool m_IsMultiCP;

	/**
	 * 	@brief	m_poRtrServer pointer to the RTR server object
	*/
	ACS_RTR_Server* m_poRtrServer;

};

#endif
