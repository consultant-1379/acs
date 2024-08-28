/**
   @file    acs_rtr_filebased_objectImpl.h

   @brief Header file for RTR module.
          It acts as OI for handling IMM Callbacks of RTR filebased object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012   XHARBAV   Initial Release
   N/A       05/12/2013   QVINCON   Re-factoring
*/

#ifndef ACS_RTR_FILEBASED_OBJECTIMPL_H
#define ACS_RTR_FILEBASED_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "acs_apgcc_objectimplementereventhandler_V3.h"


#include <string>
#include <map>

class ACS_RTR_Server;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief ACS_RTR_FileBased_ObjectImpl
 * ACS_RTR_FileBased_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 */

class ACS_RTR_FileBased_ObjectImpl : public acs_apgcc_objectimplementereventhandler_V3
{
 public:

	/**
	 * @brief constructor
	 */
	ACS_RTR_FileBased_ObjectImpl(ACS_RTR_Server* serverObject);

    /**
   	* @brief destructor
   	*/
	~ACS_RTR_FileBased_ObjectImpl();

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
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType** paramList);
	
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

	/** @brief	validateFileTQ
	*
	*/
	bool validateFileTQ(const std::string& fileTqName);

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

	/** @brief	FileJobInfo
	 *
	 *  Struct of IMM FileBasedJob class elements
	 *
	 */
	struct FileJobInfo{
			std::string messageStoreName;
			uint32_t msgStoreRecordSize;
			std::string fileJobDN;
			bool cdrCounterFlag;
			uint32_t fileSize;
			int fixedFileRecordsFlag;
			bool hmacMd5;
			uint32_t fileHoldTime;
			uint32_t minFileSize;
			int32_t paddingChar;
			int32_t recordlength;
			std::string transferQueue;
			ImmAction action;

			FileJobInfo& operator=(const FileJobInfo& rvalue)
			{
				std::cout << " File Operator =" << std::endl;
				messageStoreName.assign(rvalue.messageStoreName);
				msgStoreRecordSize = rvalue.msgStoreRecordSize;
				fileJobDN.assign(rvalue.fileJobDN);
				cdrCounterFlag = rvalue.cdrCounterFlag;
				fileSize = rvalue.fileSize;
				fixedFileRecordsFlag = rvalue.fixedFileRecordsFlag;
				hmacMd5 = rvalue.hmacMd5;
				fileHoldTime = rvalue.fileHoldTime;
				minFileSize = rvalue.minFileSize;
				paddingChar = rvalue.paddingChar;
				recordlength = rvalue.recordlength;
				transferQueue.assign(rvalue.transferQueue);
				action = rvalue.action;
				return *this;
			};

	};



	/** @brief	getCurrentParamters
	*
	*/
	bool getCurrentParamters(FileJobInfo& currentSet);


	typedef std::multimap<ACS_APGCC_CcbId, FileJobInfo> operationTable;
	operationTable m_FileJobOperationTable;

    std::string m_ImmClassName;
	
	ACS_RTR_Server* m_poRtrServer;
};

#endif
