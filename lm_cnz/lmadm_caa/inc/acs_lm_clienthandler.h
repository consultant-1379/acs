#ifndef _ACS_LM_CLIENTHANDLER_H_
#define _ACS_LM_CLIENTHANDLER_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include "acs_lm_cmdserver.h"
#include "acs_lm_sentinel.h"
#include <acs_lm_testlkf.h>
#include "acs_lm_persistent.h"
#include "acs_lm_eventhandler.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

class ACS_LM_ClientHandler
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
	/**
				@brief       Default constructor for ACS_LM_ClientHandler

				@par         None

				@pre         None

				@post        None

				@param       sentinel
								ACS_LM_Sentinel pointer

				@param       eventHandler
								ACE Event Handler pointer

				@param       persistFile
								ACS_LM_Persistent pointer

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_ClientHandler(ACS_LM_Sentinel* sentinel, ACS_LM_Persistent* persistFile, ACS_LM_EventHandler* eventHandler);
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_LM_ClientHandler

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_ClientHandler();

	//*=================================================================== */
	/**
			@brief       handleClient

			@par         None

			@pre         None

			@post        None

			@param      cmdServer              :ACS_LM_CmdServer

			 @return     void
	 */
	/*=================================================================== */
	void handleClient(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
			@brief      handleLkInstViaIMM  	:Install the LK via IMM

			@par         None

			@pre         None

			@post        None

			@param       aLKFFilePath			: string

			@return 	 ACS_LM_AppExitCode 					: true/false
	 */
	/*=================================================================== */
	//bool handleLkInstViaIMM(string aLKFFilePath);
	ACS_LM_AppExitCode handleLkInstViaIMM(string aLKFFilePath);
	//*=================================================================== */
	/**
			@brief      handleLkEmStartViaIMM  	:Install the LK via IMM

			@par         None

			@pre         None

			@post        None

			@param       aLKFFilePath			: string

			@return 	 ACS_LM_AppExitCode 					: true/false
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode handleLkEmStartViaIMM();
	//*=================================================================== */
	/* LM Maintenance Mode */
	/**
                        @brief      handleLkMaintenanceStartViaIMM       : Set the LM to MaintenanceMode

                        @par         None

                        @pre         None

                        @post        None

                        @return      ACS_LM_AppExitCode                                     : true/false
         */
        /*=================================================================== */
        ACS_LM_AppExitCode handleLkMaintenanceStartViaIMM(bool);
	/*=================================================================== */
	/* exportLicenseKeyFile */
        /**
                        @brief      handleLkExportLKFViaIMM       : Copy the current LKF from internal path to "/license_file" folder. 

                        @par         None

                        @pre         None

                        @post        None
			
			@param       nodeFingerPrint: string

                        @return      ACS_LM_AppExitCode                                     : true/false
         */
        /*=================================================================== */
        ACS_LM_AppExitCode handleLkExportLKFViaIMM(std::string);
	/*=================================================================== */
	/**
				@brief      getEmergencyCount :Method used to count the no. of emergency mode.
											   Maximum no of emergency count is 2.

				@return     int
	 */
	/*=================================================================== */
	unsigned int getEmergencyCount();
	//*=================================================================== */
	/**
				@brief      getLMMode			:Method used to get the LM mode

				@param  	aGraceMode			:bool

				@param  	aEmergencyMode		:bool

				@param  	aTestMode			:bool
				
				@param          aMaintenanceMode                 :bool

				@return 	void
	 */
	/*=================================================================== */

	void getLMMode(bool & aGraceMode,bool & aEmergencyMode,bool & aTestMode,bool & aMaintenanceMode);  /* LM Maintenance Mode 'aMaintenanceMode' param added */ 
	//*=================================================================== */
	/**
					@brief      getVirginModeStatus	:Method used to get the
													 virgin mode status

					@return 	true/false          :bool
	 */
	/*=================================================================== */

	bool getVirginModeStatus();
	//*=================================================================== */
	/**
					@brief      getEmergencyModeEndDate :Method used to find the
														 end date for Emergency
														 mode.

					@return		ACE_INT64
	 */
	/*=================================================================== */
	ACE_INT64 getEmergencyModeEndDate();
	//*=================================================================== */
	/* LM Maintenance Mode */
	/**
                                        @brief      getMaintenanceModeEndDate :Method used to find the end date for Maintenance mode.

                                        @return         ACE_INT64
         */
        /*=================================================================== */
        ACE_INT64 getMaintenanceModeEndDate();

	/* Dispaly Fingerprint in hashed format */
        /*===================================================================== */
        /* Finger Print in Hashed Format*/
        /**
                                        @brief      getHashedFingerPrint :Method used to get the finger print in hashed format.

                                        @return     string
         */
        /*=================================================================== */
        std::string getHashedFingerPrint();
        std::string getHashedFingerprint(bool isDualStack);
        /*=================================================================== */
	/* Send LK's when Maintenance Mode unlocked                           */
        /*=================================================================== */
        /**
         *  @brief sendLKsThread
         *
         *  @param pArgs  : void pointer
         *
         *  @return ACE_UINT64
         */
        /*=================================================================== */
	static ACE_UINT64 sendLKsThread(void* pArgs);
	/*=================================================================== */
        /**
         *  @brief  sendLKsThreadFunc
         *
         *  @param  args : void pointer
         *
         *  @return ACE_THR_FUNC_RETURN
         */
        /*=================================================================== */
        static ACE_THR_FUNC_RETURN sendLKsThreadFunc(void* args);
	/*=====================================================================
						 PRIVATE DECLARATION SECTION
	 ==================================================================== */
private:


	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	//*=================================================================== */
	/**
				@brief      handleFpprint

				@param  	cmdServer               :ACS_LM_CmdServer

				@return 	void
	 */
	/*=================================================================== */
	void handleFpprint(ACS_LM_CmdServer& cmdServer);

	//*=================================================================== */
	/**
				@brief      handleTestLkAdd

				@param       cmdServer			:ACS_LM_CmdServer

				@param   	 cmdReceived        :ACS_LM_Cmd

				@return 	void
	 */
	/*=================================================================== */

	void handleTestLkAdd(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived);
	//*=================================================================== */
	/**
					@brief      handleTestLkRm

					@param       cmdServer			:ACS_LM_CmdServer

					@param   	 cmdReceived        :ACS_LM_Cmd

					@return 	void
	 */
	/*=================================================================== */
	void handleTestLkRm(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived);
	//*=================================================================== */
	/**
					@brief      handleTestLkLs

					@param       cmdServer			:ACS_LM_CmdServer

					@return 	void
	 */
	/*=================================================================== */
	void handleTestLkLs(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
					@brief      handleTestLkLs

					@param       cmdServer			:ACS_LM_CmdServer

					@return 	void
	 */
	/*=================================================================== */
	void handleTestLkAct(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
					@brief      handleLkInst

					@param       cmdServer			:ACS_LM_CmdServer

					@param   	 cmdReceived        :ACS_LM_Cmd

					@return 	void
	 */
	/*=================================================================== */
	void handleLkInst(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived);
	//*=================================================================== */
	/**
					@brief      handleLkLs

					@param       cmdServer			:ACS_LM_CmdServer

					@param   	 cmdReceived        :ACS_LM_Cmd

					@return 	void
	 */
	/*=================================================================== */
	void handleLkLs(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived);
	//*=================================================================== */
	/**
					@brief      handleLkEmStart

					@param       cmdServer			:ACS_LM_CmdServer

					@return 	void
	 */
	/*=================================================================== */
	void handleLkEmStart(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
					@brief      handleLkMapLs

					@param       cmdServer			:ACS_LM_CmdServer

					@return 	void
	 */
	/*=================================================================== */
	void handleLkMapLs(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
					@brief      handleTestLkDeAct

					@param       cmdServer			:ACS_LM_CmdServer

					@return 	void
	 */
	/*=================================================================== */
	void handleTestLkDeAct(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
					@brief      handleShowLicense

					@param       cmdServer			:ACS_LM_CmdServer

					@return 	void
	 */
	/*=================================================================== */
	void handleShowLicense(ACS_LM_CmdServer& cmdServer);
	//*=================================================================== */
	/**
					@brief      handleLmDataLock

					@return 	void
	 */
	/*=================================================================== */
	void handleLmDataLock();
	/*=================================================================== */
	/*===================================================================== */
	/* Decide which FP to use in filename for exporting LKF*/
	/**
	            @param      dualStackFP	: Comma separated Dual Stack Fingerprint string

	            @return     string		: Selected Fingerprint for LKF filename
	 */
	/*=================================================================== */
	void getFPDualStackLKFExport(const std::string lkfPath, std::string &selectedFingerprint);
private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	ACS_LM_Sentinel* sentinel;
	ACS_LM_Persistent* persistFile;
	ACS_LM_EventHandler* eventHandler;
	ACS_LM_TestLkf* testLkf;
	ACE_HANDLE hThread;  

};

#endif
