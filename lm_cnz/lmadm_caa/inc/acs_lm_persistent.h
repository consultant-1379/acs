
//******************************************************************************
//
//  NAME
//     ACS_LM_SERVER.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR 
//     2009-01-06 by XCSVEMU PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#ifndef _ACS_LM_PERSISTENT_H_
#define _ACS_LM_PERSISTENT_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_testlkf.h"
#include <ace/ACE.h>
#include <ace/Task.h>
#include <ace/Event.h>

/* Record length increased from 27 to 35 to store the 
   exported LKF time stamp                         */       
#define PERSIST_FIXED_RECORD_LEN 35 
/* Backward compatibility: From Release 1 to Release 2 --> The persist file size was 25 before Maintenance mode added */
#define PERSIST_FILE_SIZE_MAINTENANCEMODE 33   
/* Backward compatibility: From CM003 to CM004 --> The persist file size was 33 before exportLicenseKeyFile action added */ 
#define PERSIST_FILE_SIZE_EXPORTLKF 41
#define EMERGENCY_EXPIRY_DAYS 7  /* Emergency mode will be expired after 7 days from it's activated date */
#define MAINTENANCE_EXPIRY_DAYS 25 /* Maintenance mode will be deactivated automatically after 25 days from it's activated date */
#define EXPORTFILE_HOUSEKEEPING_EXPIRY_DAYS 30	/* Exported file should be deleted after 30 days from it's exported date */ 
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_TestLkf;
class ACS_LM_Persistent
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
	 ===================================================================*/
	/*=================================================================== */
	/**
			@brief       Default destructor for ACS_LM_Persistent

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_Persistent();
	/*=================================================================== */
	/**
	 *  @brief	getLkfChecksum

	 *  @return ACE_UINT8
	 **/
	/*=================================================================== */
	ACE_UINT32 getLkfChecksum();
	/*=================================================================== */
	//TR HV60364
	ACE_UINT16 getLmdataChecksum();
	//TR HV60364
	/**
	 *  @brief	setLkfChecksum
	 *
	 *  @param chesksum 					: ACE_UINT8
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void setLkfChecksum(ACE_UINT32 chesksum);
	/*=================================================================== */
	/**
	 *  @brief	getLmMode

	 *  @return ACE_UINT8
	 **/
	/*=================================================================== */
	ACE_UINT8 getLmMode();
	/*=================================================================== */
	/**
	 *  @brief	setLmMode
	 *
	 *  @param appMode 						: ACS_LM_AppMode
	 *
	 *  @param activate 					: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void setLmMode(ACS_LM_AppMode appMode, bool activate);
	/*=================================================================== */
	/**
	 *  @brief	getEmergencyCounter

	 *  @return ACE_UINT8
	 **/
	/*=================================================================== */
	ACE_UINT8 getEmergencyCounter();
	/*=================================================================== */
	/**
	 *  @brief	setEmergencyCounter
	 *
	 *  @param emCount 					: ACE_INT16
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void setEmergencyCounter(ACE_INT16 emCount);
	/*=================================================================== */
	/**
	 *  @brief	isEmergencyModeExpired
	 *
	 *  @return bool
	 **/
	/*=================================================================== */
	bool isEmergencyModeExpired (); 
	/*=================================================================== */
	/* LM Maintenance Mode */
	/**
         *  @brief      isMaintenanceModeExpired
         *
         *  @return bool
         **/
        /*=================================================================== */
        bool isMaintenanceModeExpired();
        /*=================================================================== */
	/* LM Maintenance Mode */
        /**
         *  @brief      setMaintenanceUnlockToLock
         *
         *  @return void
         **/
	/*=================================================================== */
	void setMaintenanceUnlockToLock();
	 /*=================================================================== */
	/**
	 *  @brief	updateInitializedNodeStatus
	 *
	 *  @param aNodeInitialized 				: bool
	 *
	 *  @param bNodeInitialized 				: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void updateInitializedNodeStatus(bool &aNodeInitialized, bool& bNodeInitialized);
	/*=================================================================== */
	/**
	 *  @brief	setInitializedNodeStatus
	 *
	 *  @param aNodeInitialized 				: bool
	 *
	 *  @param bNodeInitialized 				: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void setInitializedNodeStatus(bool aNodeInitialized, bool bNodeInitialized);
	//void getInitializedNodeStatus(unsigned char &aNodeInitialized, unsigned char &bNodeInitialized);
	/*=================================================================== */
	/**
	 *  @brief	getInitializedNodeStatus
	 *
	 *  @param aNodeInitialized 				: bool
	 *
	 *  @param bNodeInitialized 				: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void getInitializedNodeStatus(bool &aNodeInitialized, bool &bNodeInitialized);
	/*=================================================================== */
	/**
	 *  @brief	getInitializedNodeStatus
	 *
	 *  @return bool 							: true/false
	 **/
	/*=================================================================== */
	bool isLmInVirginMode();
	/*=================================================================== */
	/**
	 *  @brief	disableVirginMode
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void disableVirginMode();
	/*=================================================================== */
	/**
	 *  @brief	restore
	 *
	 *  @return bool 							: true/false
	 **/
	/*=================================================================== */
	bool restore();
	/*=================================================================== */
	/**
	 *  @brief	deletePersistentFile
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void deletePersistentFile();
	/*=================================================================== */
	/**
	 *  @brief	reLoad
	 *
	 *  @param emergencyCounter 				: ACE_INT16
	 *
	 *  @param lkfChecksum 						: ACE_UINT8
	 *
	 *  @param mode								: ACE_INT16
	 *
	 *  @param isVirginMode 					: bool
	 *
	 *  @param aNodeInitialized 				: bool
	 *
	 *  @param bNodeInitialized 				: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */

	void reLoad(ACE_INT16 emergencyCounter, ACE_UINT32 lkfChecksum, ACE_INT16 mode,
			bool isVirginMode, bool aNodeInitialized, bool bNodeInitialized);

	/*=================================================================== */
	/**
	 *  @brief	testLkFile
	 *
	 *  @return ACS_LM_TestLkf
	 **/
	/*=================================================================== */
	ACS_LM_TestLkf* testLkFile();
	/*=================================================================== */
	/**
	 *  @brief	readOrigPersistentData
	 *
	 *  @param path 							: string
	 *
	 *  @param buffer  							: ACE_TCHAR
	 *
	 *  @param size								: ACE_UINT64
	 *
	 *  @return bool
	 **/
	/*=================================================================== */
	bool readOrigPersistentData(std::string path, ACE_TCHAR * buffer, ACE_UINT64 size);
	/*=================================================================== */
	/**
	 *  @brief	writeOrigPersistentData
	 *
	 *  @param path 						: string
	 *
	 *  @param buffer  						: ACE_TCHAR
	 *
	 *  @param size							: ACE_UINT64
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void writeOrigPersistentData(std::string path, ACE_TCHAR* buffer, ACE_UINT64 size);
	/*=================================================================== */
	/**
	 *  @brief	commit
	 *
	 *  @return bool 						: true/false
	 **/
	/*=================================================================== */
	bool commit(); 
	/*=================================================================== */
	/**
	 *  @brief	load
	 *
	 *  @param curPath 				: const string
	 *
	 *  @param bkpPath1 				: const string
	 *
	 *  @param bkpPath2				: const string
	 *
	 *  @param LoadedBkpFile 			: bool 
	 *
	 *  @param forceLoad				: bool
	 *
	 *  @return ACS_LM_Persistent
	 **/
	/* LoadedBkpFile argument has been added to fix the TR-HU93530 */
	/*=================================================================== */
	static ACS_LM_Persistent* load(const std::string& curPath,
			const std::string& bkpPath1,
			const std::string& bkpPath2,
			bool LoadedBkpFile=false,
			bool forceLoad=false);
	/*=================================================================== */
	/**
	 *  @brief	reloadOrigPersistentData
	 *
	 *  @return void
	 **/
	/*============================================crc16======================= */
	void reloadOrigPersistentData();
	/*=================================================================== */
	/**
	 *  @brief	getEmergencyModeEndDate
	 *
	 *  @return ACE_INT64
	 **/
	/*=================================================================== */
	ACE_INT64 getEmergencyModeEndDate();
	/*=================================================================== */
	/* LM Maintenance Mode */
	/**
         *  @brief      getMaintenanceModeEndDate
         *
         *  @return ACE_INT64
         **/
        /*=================================================================== */
        ACE_INT64 getMaintenanceModeEndDate();
        /*=================================================================== */
        /* Export file - House keeping:					      */
	/*=================================================================== */
        /**
         *  @brief      isExportLKFExpired				      *
         *  @return	bool
         **/
        /*======================================================================*/
	bool isExportLKFExpired();
	/*===================================================================   */
        /**
         *  @brief      calculateExportFileExpiryDate			        *
         *  @return	void
         **/
        /*======================================================================*/
	void calculateExportFileExpiryDate();
        /*======================================================================*/
        /**
         *  @brief      isModeExpired						*
         *  @return	bool	
         **/
        /*======================================================================*/
	bool isModeExpired(time_t);
        /*======================================================================*/
        /**
         *  @brief      isLKFExists						*
         *  @return	bool	
         **/
        /*======================================================================*/
	bool isLKFExists();
        /*======================================================================*/
        /**
         *  @brief      isLKFExists						*
         *  @return	bool	
         **/
        /*======================================================================*/
	void displayExpiryDate(ACE_INT64 );	
        /*======================================================================*/

private:
	//Persistent Data
	ACE_HANDLE hThread;
	ACS_LM_TestLkf* testLkf;

	ACE_UINT8 virginMode;
	ACE_UINT8 aNodeInitDone;
	ACE_UINT8 bNodeInitDone;
	ACE_UINT16 lmMode;
	ACE_UINT16 emergencyCounter;
	ACE_UINT32 lkfChecksum;
	ACE_INT64 emergencyModeTime;
        ACE_INT64 maintenanceModeTime; /* LM Maintenance Mode */ 
	ACE_INT64 exportFileExpiryTime; /* Export file - House keeping: */	
	ACE_UINT16 persistChecksum;

	std::string currentPath;
	std::string backupPath1;
	std::string backupPath2;


	//	static CRITICAL_SECTION writeMutex;
	static ACE_Recursive_Thread_Mutex  writeMutex;
	/*=================================================================== */
	/**
			@brief       Default constructor for ACS_LM_Persistent

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Persistent();
	/*=================================================================== */
	/**
	 *  @brief backup
	 *  @return bool : true/false
	 **/
	/*=================================================================== */
	bool backup();

	//	static ACE_UINT64 backupPfThread(void* args);
	//	static ACE_THR_FUNC_RETURN backupPfThreadFunc(void* args);

private:

#ifdef ACS_LM_CUTE_TEST
 friend class CUTE_LM_Persistent;
#endif
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_Persistent(const ACS_LM_Persistent&);
	//*=================================================================== */
	/**
						@brief      operator
	 */
	/*=================================================================== */
	ACS_LM_Persistent operator =(const ACS_LM_Persistent&);
};

#endif
