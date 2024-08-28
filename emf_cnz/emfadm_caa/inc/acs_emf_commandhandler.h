/*=================================================================== */
/**
   @file   acs_emf_aeh.h

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A         DD/MM/YYYY  XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_EMF_COMMANDHANDLER_H
#define ACS_EMF_COMMANDHANDLER_H
/*=====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <saImmOi.h>
#include <saImm.h>
#include <poll.h>
#include <ace/ACE.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include "acs_emf_dvdhandler.h"
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include "acs_emf_param.h"
#include "acs_emf_defs.h"
#include "acs_emf_cmd.h"
#include <ACS_DSD_MacrosConstants.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The acs_emf_commandhandler used for handle the emfcopy admin operations .
 */
/*=================================================================== */
// these define are used by acs_emf_commandhandler only
#define CH_MEDIA_STATE_UNAVAILABLE 0
#define CH_MEDIA_STATE_ENABLED 1
#define CH_MEDIA_STATE_DISABLED 2

class acs_emf_commandhandler :public acs_apgcc_objectimplementereventhandler_V3, public ACE_Task_Base
{
	/*=====================================================================
	                      PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*===================================================================
						  PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
							   PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/*=================================================================== */
	/**
			@brief       Default constructor for acs_emf_commandhandler

			@par         None

			@pre         None

			@post        None

			@param       pDVDHandler
							DVDHandler pointer

			@param       endEvent
							ACE Event Handler pointer

			@param       szObjName

			@param       szImpName

			@param       enScope

			@exception   None
	 */
	/*=================================================================== */
	acs_emf_commandhandler (ACS_EMF_MEDIAHandler *pMediaHandler, ACE_HANDLE endEvent,string szImpName);
	/*=====================================================================
								   CLASS DESTRUCTOR
	 ==================================================================== */
	/*=================================================================== */
	/**
			 @brief       Default destructor for acs_emf_commandhandler

			 @par         None

			 @pre         None

			 @post        None

			 @exception   None
	 */
	/*=================================================================== */
	~acs_emf_commandhandler();

	/*=================================================================== */
	/**
			 @brief         This method is inherited from base class and overridden by our class.

			 @param         oiHandle      :	ACS_APGCC_OiHandle

			 @param         ccbId         : ACS_APGCC_CcbId

			 @param         className     : const char pointer

			 @param         parentname    : const char pointer

			 @param  		attr          : ACS_APGCC_AttrValues

			 @return        ACS_CC_ReturnType : Sucess/failure
	 */
	/*=================================================================== */
	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	/*=================================================================== */
	/**
			 @brief        This method is inherited from base class and overridden by our class.This method is get invoked when IMM object deleted.

			 @param         oiHandle      :	ACS_APGtheMediaOwnerCC_OiHandle

			 @param         ccbId         : ACS_APGCC_CcbId

			 @param         objName

			 @return        ACS_CC_ReturnType : Success/failure
	 */
	/*=================================================================== */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	/*=================================================================== */
	/**
			 @brief        This method is inherited from base class and overridden by our class.
						   This method is get invoked when IMM object's attribute modify.

			 @param        oiHandle      :	ACS_APGCC_OiHandle

			 @param        ccbId         : ACS_APGCC_CcbId

			 @param        objName

			 @param        attrMods      : ACS_APGCC_AttrModification

			 @return        ACS_CC_ReturnType : Success/failure

	 */
	/*=================================================================== */
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	/*=================================================================== */
	/**
			 @brief        complete method: This method is inherited from base class and overridden by our class.

			 @param        oiHandle      :	ACS_APtheMediaOwnerGCC_OiHandle

			 @param        ccbId         : ACS_APGCC_CcbId

			 @return        ACS_CC_ReturnType : Success/failure

	 */
	/*=================================================================== */
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/*=================================================================== */
	/**
			 @brief        abort method: This method is inherited from base class and overridden by our class.

			 @param        oiHandle      :	ACS_APGCC_OiHandle

			 @param        ccbId         : ACS_APGCC_CcbId

	 */
	/*=================================================================== */
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/*=================================================================== */
	/**
			 @brief        apply method: This method is inherited from base class and overridden by our class.

			 @param        oiHandle      :	ACS_APGCC_OiHandle

			 @param        ccbId         : ACS_APGCC_CcbId

	 */
	/*=================================================================== */
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/*=================================================================== */
	/**
			 @brief        updateRuntime method: This method is inherited from base class and overridden by our class.

			 @param        objName          : const char pointer

			 @param        attrName         : const char pointer

	 */
	/*=================================================================== */
	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

	/*=================================================================== */
	/**
			 @brief        Method used to handle the admin operation performed by emfcopy command

			 @param			oiHandle
									contains OIHanlde
			 @param			invocation
									contains invocation
			 @param			p_objName
									contains IMtheMediaOwnerM Objectname
			 @param			operationId
									contains Operation Id
			 @param			paramList

	 */
	/*=================================================================== */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

	/*=================================================================== */
	/**
			 @brief        Method used to initializes the object Implementer functions of the IMM service for the invoking process
						   and registers the callback function.

			 @param			poReactor

			 @return        ACE_INT32

	 */
	/*=================================================================== */
	ACE_INT32 register_object();

	/*=================================================================== */
	/**
			 @brief       Method used to start theMediaOwnerthe dvd operation porcess

			 @par         None

			 @pre         None

			 @post        None

			 @param       lpvThistheMediaOwner

			 @exception   None
	 */
	/*=================================================================== */
	static ACE_THR_FUNC_RETURN EmfMediaOperationProc(void * lpvThis);

        /*=================================================================== */
        /**
                         @brief       Method used to perform unlockMedia operation porcess

                         @par         None

                         @pre         None

                         @post        None

                         @param       lpvThistheMediaOwner

                         @exception   None
         */
        /*=================================================================== */
	static ACE_THR_FUNC_RETURN EmfUnlockMediaOperationProc(void* lpvThis);


        /*=================================================================== */
        /**
                         @brief       Method used to invoke unlockMedia admin callback

                         @par         None

                         @pre         None

                         @post        None

                         @param       lpvThistheMediaOwner

                         @exception   None
         */
        /*=================================================================== */
	int EmfUnlockMediaInvoke();
        /*=================================================================== */
        /**
                         @brief       Method used to invoke modifyMediaStateInvoke() if DVD is empty.

                         @par         None

                         @pre         None

                         @post        None

                         @param       lpvThistheMediaOwner

                         @exception   None
         */
        /*=================================================================== */
        static ACE_THR_FUNC_RETURN EmfUnlockMediaIfDVDIsEmpty(void* lpvThis);

        /*=================================================================== */
        /**
                         @brief       Method used to invoke modifyMediaState() if DVD is empty.

                         @par         None

                         @pre         None

                         @post        None

                         @param       lpvThistheMediaOwner

                         @exception   None
         */
        /*=================================================================== */
        ACS_CC_ReturnType modifyMediaStateInvoke();

	/*=================================================================== */
		/**
			 @brief      performErrChkForTODVDOper

			 @par        None
			 @pre        None

			 @post       None

			 @param      label : string

			 @param      verify : string

			 @param      overwrite : stringtheMediaOwner

			 @param      noOfFiles : int

			 @param      fileList : vector<std::string>

			 @exception   None
	 */
	/*=================================================================== */
	ACE_INT32 performErrChkForTOMediaOper(std::string label,
			/*std::string verify,*/
			/* std::string overwrite,*/
			int noOfFiles,
			std::vector<std::string> fileList,
			bool formatOption);
	/*=================================================================== */
	/**
			 @brief      performErrChkForFROMDVDOper

			 @par        None

			 @pre        None

			 @post       NonetheMediaOwner

			 @param      fileList : vector<std::string>

			 @param      copyAllDataFlag : bool

			 @exception   NonetheMediaOwner
	 */
	/*=================================================================== */
	ACE_INT32 performErrChkForFromMediaOper(std::vector<std::string>& fileList,bool copyAllDataFlag);

	/*=================================================================== */
	/**
			 @brief      validateFileNamesForTODVD

			 @par        None

			 @pre        None

			 @post       None

			 @param      fileList : vector<std::string>

			 @exception   None
	 */
	/*=========================================theMediaOwner========================== */
	ACE_INT32 validateFileNamesForTOMedia(std::vector<std::string> fileList);

	/*=================================================================== */
	/**
			 @brief      validateFileNamesForTODVD

			 @par        None

			 @pre        NoneOB

			 @post       None

			 @param      fileList : vector<std::string>

			 @param      copyAllDataFlag : bool

			 @exception   None
	 */
	/*===========================================theMediaOwner======================== */
	ACE_INT32 validateFileNameForFROMMedia(std::vector<std::string>& fileList, bool copyAllDataFlag);
	int checkForDVD();
        /*=================================================================== */
        /**
                         @brief      checkForMedia			:This method is used to check for media presence on the node
			
			 @return     int	

         */
        /*=================================================================== */

	int checkForMedia();
        /*=================================================================== */
        /**
                         @brief      mountDVDOnActiveNode                      :This method is used to mount DVD data on active node

                         @return     int

         */
        /*====================================================================== */

	int mountDVDOnActiveNode();
        /*=================================================================== */
        /**
                         @brief      checkAndMountMediaOnAccessibleNode		:This method is used to mount DVD data on active node

                         @return     int

         */
        /*====================================================================== */

	int checkAndMountMediaOnAccessibleNode(const ACS_DSD_Node &remoteNodeInfo);
        /*=================================================================== */
        /**
                         @brief     unmountDVDOnActiveNode                      :This method is used to unmount DVD data on active node

                         @param     strMountMode				:const std::string

			 @return     int

         */
        /*====================================================================== */

	int unmountDVDOnActiveNode(const std::string &strMountMode);
	/*=================================================================== */
        /**
                         @brief     unmountDVDOnActiveNodeAndDisable            :This method is used to unmount DVD data on active node and disable operation state

                         @param     strMountMode				:const std::string

			 @return     int

         */
        /*====================================================================== */

	int unmountDVDOnActiveNodeAndDisable(const std::string &strMountMode);

        /*=================================================================== */
        /**
                         @brief      mountMediaOnPassiveNode                     :This method is used to mount passive DVD data on active node

			 @return     int

         */
        /*====================================================================== */
	int mountMediaOnPassiveNode(const char * mountMode = NULL,const ACE_TCHAR* formatOption = NULL );
        /*=================================================================== */
        /**
                         @brief     umountnanoUSB                      :This method is used to unmount USB data

                         @return     int

         */
        /*====================================================================== */

	int umountnanoUSB();
        /*=================================================================== */
        /**
                         @brief     mountnanoUSB                      :This method is used to mount USB data

                         @return     int

         */
        /*====================================================================== */

	int mountnanoUSB(bool formatOption);
        /*=================================================================== */
        /**
                         @brief     unmountMediaOnPassiveNode                      :This method is used to unmount passive DVD data on active node

                         @param     strMountMode                                :const std::string
                         @param     rmFile					:const char*

                         @return     int

         */
        /*====================================================================== */

	int unmountMediaOnPassiveNode(const std::string &strMountMode, const char *rmFile=NULL);

        /*=================================================================== */
        /**
                @brief                  This method retrieves the value of "mediaState" IMM config attribute.
                @pre                    none
                @post                   none
                @return                 ACS_CC_ReturnType
                @exception              none
         */
        /*=================================================================== */
        ACS_CC_ReturnType getMediaState(int &mediaState);

        /*=================================================================== */
        /**
                         @brief     getMediaInfo                      :This method is used to get media info from partner node 

                         @param      mediaType                         :ACE_INT32
	
			 @param      freeSpace                         :ACE_UINT16

			 @param      usedSpace                         :ACE_UINT16

			 @param      totalSpace                        :ACE_INT32		 
	
			 @return     int

         */
        /*====================================================================== */

	int getMediaInfo(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace);
        /*=================================================================== */
        /**
                         @brief     mediaOperationOnOtherNode           

                         @param     lpvThis                             :void

                         @return    ACE_THR_FUNC_RETURN

         */
        /*====================================================================== */

	static ACE_THR_FUNC_RETURN mediaOperationOnOtherNode(void* lpvThis);
	ACE_INT16 removeNewCopiedFile(ACS_EMF_DATA* pData);
        /*=================================================================== */
        /**
                         @brief     USBOperationOnOtherNode

                         @param     pData                                :ACS_EMF_DATA

                         @return    ACE_INT16

         */
        /*====================================================================== */

	ACE_INT16 USBOperationOnOtherNode(ACS_EMF_DATA* pData);
        /*=================================================================== */
        /**
                         @brief     importFromOtherNode                 :this method is used to import files.

                         @param     lpvThis                             :void

                         @return    ACE_THR_FUNC_RETURN

         */
        /*====================================================================== */

	static ACE_THR_FUNC_RETURN importFromOtherNode(void *lpvThis);
        /*=================================================================== */
        /**
                         @brief     performMountOperation                

                         @param                                           :int

                         @return    int

         */
        /*====================================================================== */

	int performMountOperation(int);
        /*=================================================================== */
        /**
                         @brief     performUnmountOperation

                         @param                                           :int

                         @return    int

         */
        /*====================================================================== */

	int performUnmountOperation(int);
        /*=================================================================== */
        /**
                         @brief     get_errorString

                         @return    const char

         */
        /*====================================================================== */

	inline const char * get_errorString ()  { return errorString.c_str(); }
        /*=================================================================== */
        /**
                         @brief     adminoperationString

                         @param     opId                                   :ACS_APGCC_AdminOperationIdType

                         @return    const char

         */
        /*====================================================================== */

	const char * adminoperationString (ACS_APGCC_AdminOperationIdType  opId);

        /*=================================================================== */
        /**
                         @brief        Method used to invoke unlock_media action

                         @param                 p_objName 		contains IMtheMediaOwnerM Objectname

                         @return                 ACE_INT32

         */
        /*=================================================================== */
        ACS_CC_ReturnType unlockMedia();

        /*=================================================================== */
        /**
                         @brief     setOperationalState

                         @param                                             :ACE_INT32

                         @return    none

         */
        /*====================================================================== */

	static void setOperationalState(ACE_INT32);
        /*=================================================================== */
        /**
                         @brief     getMediaOwner

                         @return    ACE_UINT16

         */
        /*====================================================================== */

	static ACE_UINT16 getMediaOwner();
        /*=================================================================== */
        /**
                         @brief     svc

                         @return    int

         */
        /*====================================================================== */

	int svc();
        /*=================================================================== */
        /**
                         @brief     shutdown

                         @return    none

         */
        /*====================================================================== */

	void shutdown();

        /*=================================================================== */
        /**
        		@brief   theMediaState
         */
        /*=================================================================== */

        static ACE_INT32 theMediaState;
        /*=================================================================== */

private:
        /*=================================================================== */
        /**
                         @brief     handle_unlockMedia 			  :This method performs unlock media action.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */
	int handle_unlockMedia(ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_unlockMedia_on_local_node      :This method performs unlock media action on active node.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_unlockMedia_on_local_node(ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_unlockMedia_on_local_node   :This method performs unlock media action on passive node.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_unlockMedia_on_remote_node(ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_lockMedia    		  :This method performs lock media action .

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_lockMedia(ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_syncMedia                      :This method performs sync action .

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_syncMedia (ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_exportToMedia                  :This method is used to export files to medis.

                         @param     label                                 :const char

                         @param     overwrite                             :const char 

			 @param     fileslist                             :const char

			 @param     formatMedia                           :bool

			 @return    int

         */
        /*====================================================================== */

	int handle_exportToMedia ( const char *label, const char * verify, const char *overwrite, const char *fileslist, bool formatMedia);
        /*=================================================================== */
        /**
                         @brief     handle_exportToMedia_on_local_node    :This method is used to export files to active node.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_exportToMedia_on_local_node(ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_exportToMedia_on_remote_node   :This method is used to export files to active node.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_exportToMedia_on_remote_node(ACS_EMF_DATA & paramList);
        /*=================================================================== */
        /**
                         @brief     handle_getMediaInfo   		  :This method is used to get media info from partner node

                         @return    none 

         */
        /*====================================================================== */

	int handle_getMediaInfo(/*ACS_EMF_DATA & paramList*/);
        /*=================================================================== */
        /**
                         @brief     setExitCode

			 @param     error				   :int

			 @param     text                                   :std::string

                         @return    none

         */
        /*====================================================================== */

	void setExitCode(int error, std::string text);
        /*=================================================================== */
        /**
                         @brief     setExitCode

                         @param     emf_rc_error                            :const int

                         @return    none

         */
        /*====================================================================== */

	void setExitCode(const int emf_rc_error);
        /*=================================================================== */
        /**
                         @brief     getExitCode

                         @return    int 

         */
        /*====================================================================== */

	int getExitCode();
        /*=================================================================== */
        /**
                         @brief     getExitCodeString

                         @return    string

         */
        /*====================================================================== */

	string getExitCodeString();
        /*=================================================================== */
        /**
                         @brief     get_rc_errorText

                         @param     emf_rc_error                            :const int

                         @return    char

         */
        /*====================================================================== */

	const char *get_rc_errorText(const int emf_rc_error);
        /*=================================================================== */
        /**
                         @brief     clearDVDStatus

                         @return    none 

         */
        /*====================================================================== */

	void clearDVDStatus();
       /*=================================================================== */
        /**
                         @brief     searchDVD

			 @param     piSystemId  		 	  :int32_t

			 @param     penmNodeState                         :acs_dsd::NodeSideConstants

			 @param     penmNodeSide                          :acs_dsd::NodeSideConstants

			 @param     piState				  :ACE_INT16

			 @param     pbIsLocal                             :bool

                         @return    bool

         */
        /*====================================================================== */

	bool searchDVD(int32_t* piSystemId, acs_dsd::NodeStateConstants* penmNodeState, acs_dsd::NodeSideConstants *penmNodeSide, ACE_INT16 *piState, bool *pbIsLocal);

        /*=================================================================== */
        /**
                         @brief     initDVD

                         @return    none

         */
        /*====================================================================== */

	void initDVD();
       /*=================================================================== */
        /**
                         @brief     handle_importFromMedia                :this method is used to import files from media.

                         @param     overwrite                             :const char

                         @param     filesList                             :const char

                         @param     destinationfolder                     :const char

                         @return    int

         */
        /*====================================================================== */


	int handle_importFromMedia(const char *overwrite, const char *fileslist, const char * destinationfolder);
       /*=================================================================== */
        /**
                         @brief     handle_importFromMedia_on_local_node  :this method is used to import files from media to active node.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_importFromMedia_on_local_node(ACS_EMF_DATA & paramList);
       /*=================================================================== */
        /**
                         @brief     handle_importFromMedia_on_local_node  :this method is used to import files from media to passive node.

                         @param     paramList                             :ACS_EMF_DATA

                         @return    int

         */
        /*====================================================================== */

	int handle_importFromMedia_on_remote_node(ACS_EMF_DATA & paramList);
    /*=================================================================== */
       /**
                        @brief     sendreceive_to_mediaOwner  :this method is used to send and receive from Media owner AP node

                        @param     cmdSend                             :ACS_EMF_Cmd
                        @param     cmdRecv                             :ACS_EMF_Cmd

                        @return    int

        */
    /*====================================================================== */
	ACE_INT32 sendreceive_to_mediaOwner(const ACS_EMF_Cmd & cmdSend, ACS_EMF_Cmd & cmdRecv);
	/*=================================================================== */
	    /**
	        @brief     update_imm_actionResult  : this method is used to update action result in IMM.

	        @param     result                        :emf_imm_result_t

	        @param     resultInfo                    :const char *

	        @return    none

	    */
	/*====================================================================== */

	void update_imm_actionResult(emf_imm_result_t result, const char *resultInfo);
	void set_imm_action_startingState(int opId);
	void update_imm_HistoryAttrs(const char *resultInfo);
	ACS_CC_ReturnType modifyMediaState(int mediaState);

        /*=================================================================== */
        /**
                          @brief  theMediaHandlePtr
         */
       /*=================================================================== */

	static ACS_EMF_MEDIAHandler* theMediaHandlePtr;
        /*=================================================================== */
        /**
                          @brief  theInstance
         */
       /*=================================================================== */

	static acs_emf_commandhandler* theInstance;
	/*=================================================================== */
	/**
			  @brief  contains the boolean value for operation progress
	 */
	/*=================================================================== */
	static bool operationProgress;
        /*=================================================================== */
        /**
                          @brief  errorString
         */
        /*=================================================================== */

	std::string errorString;
        /*=================================================================== */
        /**
                          @brief  errorCode
         */
        /*=================================================================== */

	int errorCode;
        /*=================================================================== */
        /**
                          @brief  isMediaEmpty
         */
        /*=================================================================== */

	bool isMediaEmpty;
        /*=================================================================== */
	/**
			  @brief  contains the ACE event pointer
	 */
	/*=================================================================== */
	//ACE_HANDLE stopThreadFds;

	/*=================================================================== */
	/**
						  @brief  contains the OI handler pointer
	 */
	/*=================================================================== */

	acs_apgcc_oihandler_V3 *theOiHandlerPtr;

        /*=================================================================== */
        /**
        @brief     theMediaStatusFlag

         */
        /*=================================================================== */
	static bool theMediaStatusFlag;
        /*=================================================================== */
        /**
        @brief   theFreeSpaceOnMedia
         */
        /*=================================================================== */

	ACE_UINT32 theFreeSpaceOnMedia;
        /*=================================================================== */
        /**
        @brief   theTotalSizeOfMedia
         */
        /*=================================================================== */

	ACE_UINT32 theTotalSizeOfMedia;
        /*=================================================================== */
        /**
        @brief   theUsedSpaceOnMedia
         */
        /*=================================================================== */

	ACE_UINT32 theUsedSpaceOnMedia;
        /*=================================================================== */
        /**
        @brief   theMediaType
         */
        /*=================================================================== */

	ACE_INT32 theMediaType;
        /*=================================================================== */
        /**
        @brief   theLastUpdatedTime
         */
        /*=================================================================== */

	std::string theLastUpdatedTime;
        /*=================================================================== */
        /**
        @brief   theOperationalState
         */
        /*=================================================================== */

	static ACE_INT32 theOperationalState;
        /*=================================================================== */
        /**
        @brief   theResultOfOperation
         */
        /*=================================================================== */

	std::string theResultOfOperation;
	// TR HR44823 - BEGIN
        /*=================================================================== */
        /**
        @brief   theMediumType
         */
        /*=================================================================== */

	std::string theMediumType;
	// TR HR44823 - END
        /*=================================================================== */
        /**
        @brief   theMediaOwner
         */
        /*=================================================================== */

	static ACE_UINT16 theMediaOwner;
        /*=================================================================== */
        /**
        @brief   m_poReactor
         */
        /*=================================================================== */

	ACE_Reactor * m_poReactor;
        /*=================================================================== */
        /**
        @brief   m_poTp_reactor
         */
        /*=================================================================== */

	ACE_TP_Reactor * m_poTp_reactor;

        /*=================================================================== */
        /**
        @brief   isStopSignaled
         */
        /*=================================================================== */
	static bool isStopSignaled;
        /*=================================================================== */
        /**
        @brief   thr_grp_id
         */
        /*=================================================================== */

	int thr_grp_id;
        /*=================================================================== */
        /**
        @brief   m_stopHandle
         */
        /*=================================================================== */

	static ACE_HANDLE m_stopHandle;

        /*=================================================================== */
        /**
        @brief   m_bIsDvdLocal
         */
        /*=================================================================== */

	bool m_bIsDvdLocal;
        /*=================================================================== */
        /**
        @brief   m_iSystemId
         */
        /*=================================================================== */

	int32_t m_iSystemId;
        /*=================================================================== */
        /**
        @brief   m_iRemoteDvdState
         */
        /*=================================================================== */

	ACE_INT16 m_iRemoteDvdState;
        /*=================================================================== */
        /**
        @brief   m_enmNodeState
         */
        /*=================================================================== */

	acs_dsd::NodeStateConstants m_enmNodeState;
        /*=================================================================== */
        /**
        @brief   m_enmNodeSide
         */
        /*=================================================================== */

	acs_dsd::NodeSideConstants m_enmNodeSide;
};

#endif
