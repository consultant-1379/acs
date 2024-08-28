/*=================================================================== */
/**
   @file   acs_emf_common.h

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_EMF_DVDHANDLER_H
#define ACS_EMF_DVDHANDLER_H

/*=====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_emf_defs.h>
#include <acs_apgcc_omhandler.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <saAis.h>
#include "acs_emf_tra.h"
#include "acs_emf_TScsiDev.h"
#include "acs_emf_mediahandler.h"
#include "acs_emf_dsdserver.h"
#include "acs_emf_cmd.h"
#include "acs_emf_cmdclient.h"

#define DVDHDLR_CHECK_ENDSIGNAL()			if(m_stopFlag == true){\
												ERROR(1,"%s","EMF Service has received Stop Signal!");\
												historyResult_text(FAILED);\
												lastOperation_error_text("DVD operation is interrupted because of service is stopped");

#define DVDHDLR_IF_ENDSIGNAL_EXIT()			return EMF_RC_NOK;}

#define DVDHDLR_EXIT_IF_ENDSIGNAL()		DVDHDLR_CHECK_ENDSIGNAL()\
															DVDHDLR_IF_ENDSIGNAL_EXIT()
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The ACS_EMF_DVDHandler class handles dvd operations for copy from dvd,
			  copy to dvd,media info and history info .
 */
/*=================================================================== */
class ACS_EMF_DVDHandler : public ACS_EMF_MEDIAHandler
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*=================================================================== */
	/**
			@brief       Default constructor for ACS_EMF_DVDHandler

			@par         None

			@pre         None

			@post        None

			@param       None 

			@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_DVDHandler(ACE_HANDLE endEvent);

	/*=================================================================== */
	/**
			@brief       Default destructor for ACS_EMF_DVDHandler

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	 ~ACS_EMF_DVDHandler();
	/*=================================================================== */
	/**
			@brief       Initializes EMFRtHistoryInfo Runtime Handler.

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	//void initEMFHistRuntimeHandler();
	/*=================================================================== */
	/**
			@brief       finalizes EMFRtHistoryInfo Runtime Handler.

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	//void finalizeEMFHistRuntimeHandler();
	/*=================================================================== */
	/**
			@brief       sets last history operation state to IDLE if it is PROCESSING. This situation
						 may arise in case of sudden reboots and if graceful shutdown of application
						 not happens

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	//void setLastHistOperStateToIDLEIfPROCESSING();
	/*=================================================================== */
	/**
   			@brief       Methods used to Perform the DVD operations(copy to dvd,copy from dvd,media info,owner info
						 history info) based on the data received.

   			@par         None

   			@pre         None

   			@post        None

   			@param		pData
								COntains the EMF Data based on the operation performed

			return		0 on success
									otherwise failed
   			@exception   None
	 */
	/*=================================================================== */
	ACE_UINT32 ImportFiles(ACS_EMF_DATA* pData);
	/*=================================================================== */
	/**
	   			@brief       Methods used to Perform the DVD operations(Export and Format and export) based on the data received
	   			             when DVD is connected to passive node.

	   			@par         None

	   			@pre         None

	   			@post        None

	   			@param		pData
									COntains the EMF Data based on the operation performed

				return		0 on success
										otherwise failed
	   			@exception   None
	 */
	/*=================================================================== */
	ACE_UINT32 ExportFiles(int32_t sysId, acs_dsd::NodeSideConstants nodeSide,ACS_EMF_DATA* pData);
	/*=================================================================== */
	/**
	   			@brief       Methods used to Perform the DVD operations(copy to dvd,copy from dvd,media info,owner info
							 history info) based on the data received when DVD is connected to Active.

	   			@par         None

	   			@pre         None

	   			@post        None

	   			@param		pData
									COntains the EMF Data based on the operation performed

				return		0 on success
										otherwise failed
	   			@exception   None
	 */
	/*=================================================================== */
	ACE_UINT32 mediaOperation(ACS_EMF_DATA* pData /*, ACE_HANDLE hPipe*/);
	/*==========================================create========================= */
	/**
			@brief       Methods used to get the node name of the node that owns the DVD device

			@par         std::string

			@pre         None

			@post        None

			return		0 on success
						otherwise failed

			@exception   None
	 */
	/*=================================================================== */
	ACE_UINT32 GetDVDOwner(std::string& str);
	/*=================================================================== */
	/**
				@brief       Methos used to get information about the media when DVD is connected to active node.

				@par         None

				@pre         None

				@post        None

				return		0 on success

										otherwise failed
				@exception   None
	 */
	/*=================================================================== */
	 ACE_UINT32 GetMediaInformation(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace);
	 ACE_UINT32 GetMediaInfoOnPassive(std::string& mediaType,std::string& freeSpace,std::string& usedSpace,std::string& totalSpace);
	 /*=================================================================== */
	 /**
                 @brief       Method to eject and reset the media.

                 @par         None

                 @pre         None

                 @post        None

                 return          0 on success
                                                                                                otherwise failed
                 @exception   None
	  */
	 /*=================================================================== */
	 ACE_INT32 resetMedia(int node);
         /*=================================================================== */
         /**
                 @brief       stopMediaOperation

                 @par         None

                 @return       None
                                  
          */
         /*=================================================================== */

	 //void stopMediaOperation();
         /*=================================================================== */
         /**
                 @brief       setDvdState

                 @par         iApgOwner					:int32_t

		 @par	      bDVDLocked				:int32_t	

                 @return       None

          */
         /*=================================================================== */
	
	 void setDvdState(int32_t iApgOwner, bool bDVDLocked);
         /*=================================================================== */
         /**
                 @brief       getDvdState

                 @par         iApgOwner                                 :int32_t

                 @par         bDVDLocked                                :int32_t

                 @return       None

          */
         /*=================================================================== */

	 void getDvdState(int32_t *piApgOwner, bool *pbDVDLocked);
         /*=================================================================== */
         /**
                 @brief       lastOperation_error_text

                 @par         None

                 @return       const char

          */
         /*=================================================================== */

	 inline const char * lastOperation_error_text () const { return _lastOperation_error_text ?: "SUCCESS"; }
         /*=================================================================== */
         /**
                 @brief       historyResult_text

                 @par         None

                 @return       const char

          */
         /*=================================================================== */

	 inline const char * historyResult_text () const { return _historyResult_text ?: "SUCCESS"; }
	 /*=================================================================== */
	/**
		@brief                  This method send msg to DVD owner to check media state and enable the media.
		@pre                    none
		@post                   none
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */

	ACE_INT32 enableMediaOnAccessibleNode(int localMediaState);
	/*=================================================================== */
	/**
		@brief                  This method mount media on DVD Attached node and setup NFS 
		@pre                    none
		@post                   none
		@return                 ACE_INT32
		@exception              none
	 */
	/*=================================================================== */
	ACE_INT32 mountMediaOnDVDOwner(int32_t SysId);


protected:
	         /*=================================================================== */
         /**
                 @brief       lastOperation_error_text

                 @par         new_ptr                                      :const char

                 @return       none

          */
         /*=================================================================== */
 
	inline void lastOperation_error_text (const char * new_ptr) { _lastOperation_error_text = new_ptr; }
         /*=================================================================== */
         /**
                 @brief       historyResult_text

                 @par         new_ptr                                      :const char

                 @return       none

          */
         /*=================================================================== */

	 inline void historyResult_text (const char * new_ptr) { _historyResult_text = new_ptr; }
	 
	 void update_imm(ACE_INT32 result);


private:

	//static ACE_INT32				 cntValue;

public:
	/*===================================================================
						  PUBLIC ATTRIBUTE
	=================================================================== */
	//bool m_stopFlag;


private:
	// True if the DVD is in used
	bool m_bDVDLocked;
	// AP that has required DVD
	int32_t m_iApgOwner;
	// Mutex
	ACE_Recursive_Thread_Mutex m_AceMutex;

	/*===================================================================
   						 PRIVATE ATTRIBUTE
   	=================================================================== */
	/*=================================================================== */
	/**
   		@brief   cntValue
	 */
	/*=================================================================== */
	//ACE_INT32				 cntValue;
	const char * _lastOperation_error_text;
	const char * _historyResult_text;
	ACE_HANDLE m_stopHandleforDVD;


};

#endif

