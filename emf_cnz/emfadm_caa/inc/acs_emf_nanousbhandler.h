#ifndef ACS_EMF_NANOUSBHANDLER_H
#define ACS_EMF_NANOUSBHANDLER_H

#include "acs_emf_defs.h"
#include <acs_apgcc_omhandler.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <saAis.h>
#include <acs_emf_tra.h>
#include <acs_emf_mediahandler.h>
#include <ACS_DSD_MacrosConstants.h>
#include "acs_emf_dsdserver.h"
#include "acs_emf_cmd.h"
#include "acs_emf_cmdclient.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
      @brief  The ACS_EMF_NANOUSBHandler class handles nanousb operations for copy from nanousb,
			  copy to nanousb,media info and history info .
 */
/*=================================================================== */
class ACS_EMF_NANOUSBHandler : public ACS_EMF_MEDIAHandler
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
protected:

	//static ACE_INT32 cntValueForUSBINT;

public:
	/*=================================================================== */
	/**
			@brief       Default constructor for ACS_EMF_NANOUSBHandler

			@par         None

			@pre         None

			@post        None

			@param       None 

			@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_NANOUSBHandler(ACE_HANDLE endEvent);

	/*=================================================================== */
	/**
			@brief       Default destructor for ACS_EMF_NANOUSBHandler

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	~ACS_EMF_NANOUSBHandler();
	/*=================================================================== */
	/**
			@brief       Initializes EMFRtHistoryInfo Runtime Handler.

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
        /**
                        @brief       Methods used to Perform the operations(copy to nanousb,copy from nanousb) based on the data received.

                        @par         None

                        @pre         None

                        @post        None

                        @param          pData
                                                                COntains the EMF Data based on the operation performed

                        return          0 on success
                                                                        otherwise failed
                        @exception   None
         */
        /*=================================================================== */
        ACE_UINT32 mediaOperation(ACS_EMF_DATA* pData);
	/*=================================================================== */
        /*=================================================================== */
        /**
                        @brief       stopMediaOperation

                        @par         None

                        return       None
         */
        /*=================================================================== */

   // void stopMediaOperation();
	int checkForMedia();
	int mountMediaOnActiveNode();
	int unmountMediaOnActiveNode();
        /*=================================================================== */
        /**
                        @brief       ExportFiles                          :This method exports files to nano USB

                        @param      sysId 	                          :int32_t

		        @param      nodeSide                              :acs_dsd::NodeSideConstants

			@param      pData                                 :ACS_EMF_DATA

			return      ACE_UINT32
         */
        /*=================================================================== */

	ACE_UINT32 ExportFiles(int32_t sysId , acs_dsd::NodeSideConstants nodeSide, ACS_EMF_DATA* pData);
        /*=================================================================== */
        /**
                        @brief       ExportFiles                          :This method imports files from nano USB

                        @param      pData                                 :ACS_EMF_DATA

                        return      ACE_UINT32
         */
        /*=================================================================== */

	ACE_UINT32 ImportFiles(ACS_EMF_DATA* pData);
	ACE_INT32 syncMediaOnActive();
	ACE_INT32 syncMediaOnPassive();
	ACE_INT32 removePassiveShare();
	ACE_INT32 copyDataFromOtherMedia();
	ACE_INT32 syncPassiveMedia(acs_emf_cmd_ns::emf_cmdCode value);
	ACE_INT32 passiveShare();
	ACE_INT32 formatMediaInUnlock();
	ACE_UINT32 GetMediaInformation(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace);
	ACE_UINT32 fileListToCopyToMedia(std::string& fileList);
	ACE_INT32 sendCmdPassive(acs_emf_cmd_ns::emf_cmdCode value);
	ACE_INT32 enableMediaOnAccessibleNode(int localMediaState); 
	ACE_INT32 checkAndSyncMedia();
public:
        /*===================================================================
                                                  PUBLIC ATTRIBUTE
        =================================================================== */
	//bool m_stopSignal;
	static ACE_HANDLE m_stopHandlefornanoUSB;
private:

        /*===================================================================
                                                 PRIVATE ATTRIBUTE
        =================================================================== */
		 /*=================================================================== */
		 /**
		   		@brief   cntValue
		  */
		 /*=================================================================== */

};
#endif
