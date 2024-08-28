/**
   @file    acs_rtr_blockbased_objectImpl.h

   @brief Header file for RTR module.
          It acts as OI for handling IMM Callbacks of RTR blockbased object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012   XHARBAV   Initial Release
   N/A       13/12/2013   QVINCON   Re-factoring
*/

#ifndef ACS_RTR_BLOCKBASED_OBJECTIMPL_H
#define ACS_RTR_BLOCKBASED_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_apgcc_objectimplementereventhandler_V3.h>

#include <string>
#include <map>

class ACS_RTR_Server;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief ACS_RTR_BlockBased_ObjectImpl
 * ACS_RTR_BlockBased_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 */

class ACS_RTR_BlockBased_ObjectImpl : public acs_apgcc_objectimplementereventhandler_V3
{

 public:

	/**
	 * @brief
	 * constructor with two parameters
	*/
	ACS_RTR_BlockBased_ObjectImpl( ACS_RTR_Server* serverObject);

    /**
   	* @brief
   	* destructor
   	*/
	~ACS_RTR_BlockBased_ObjectImpl();

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

	/** @brief	validateBlockTQ
	*
	*/
	bool validateBlockTQ(const std::string& blockTqName);

	/** @brief	getMessageStoreParameter
	*
	*/
	bool getMessageStoreParameter(const char* messageStoreDN, std::string& messageStoreName, uint32_t& msgStoreRecordSize);

	/** @brief	getMessageStoreName
	*
	*/
	bool getMessageStoreName(const char* messageStoreDN, std::string& messageStoreName);


	enum ImmAction {
		Create = 1,
		Delete,
		Modify
	};

	/** @brief	BlockJobInfo
	 *
	 *  Struct of IMM BlockBasedJob class elements
	 *
	*/
	struct BlockJobInfo{
			std::string messageStoreName;
			uint32_t msgStoreRecordSize;
			std::string blockJobDN;
			std::string transferQueue;
			uint32_t blockHoldTime;
			int32_t length;
			int32_t lengthType;
			int32_t paddingChar;
			ImmAction action;

			BlockJobInfo& operator=(const BlockJobInfo& rvalue)
			{
				messageStoreName.assign(rvalue.messageStoreName);
				msgStoreRecordSize = rvalue.msgStoreRecordSize;
				blockJobDN.assign(rvalue.blockJobDN);
				transferQueue.assign(rvalue.transferQueue);
				blockHoldTime = rvalue.blockHoldTime;
				length = rvalue.length;
				lengthType = rvalue.lengthType;
				paddingChar = rvalue.paddingChar;
				action = rvalue.action;
				return *this;
			};
	};

	typedef std::multimap<ACS_APGCC_CcbId, BlockJobInfo> operationTable;
	operationTable m_BlockJobOperationTable;

	/** @brief	getCurrentParamters
	*
	*/
	bool getCurrentParamters(BlockJobInfo& currentSet);

	/**
	* @brief	m_ImmClassName : the IMM class name
	*/
	std::string m_ImmClassName;
	
	ACS_RTR_Server* m_poRtrServer;

};

#endif
