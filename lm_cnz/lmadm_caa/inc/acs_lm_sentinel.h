//******************************************************************************
//
//  NAME
//     ACS_LM_Sentinel.h
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
//     2009-01-06 by XCSSAGO PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#ifndef _ACS_LM_SENTINEL_H_
#define _ACS_LM_SENTINEL_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include <ace/ACE.h>
#include <list>
#include <ace/Event.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <lserv.h>  //Include file for Sentinel RMS
static std::list<LkData*> connL,discL;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_Sentinel
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
			@brief       Constructor for ACS_LM_Sentinel

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Sentinel(const std::string& curPath,
			const std::string& bkpPath1,
			const std::string& bkpPath2);
	/*=================================================================== */
	/**
			@brief       Destructor for ACS_LM_Sentinel

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_Sentinel();
	/*=================================================================== */
	/**
	 *  @brief  getCheckSum

	 *  @return unsigned short
	 */
	/*=================================================================== */
	ACE_UINT32 getCheckSum();
	/*=================================================================== */
	/**
	 *  @brief  isLkfValid					:Method to check the LKF file
	 *
	 *  @param aNodeValidated    			: bool
	 *
	 *  @param bNodeValidated 				: bool
	 *
	 *  @return bool 		  				:true/false
	 */
	/*=================================================================== */
	bool isLkfValid(bool& aNodeValidated, bool& bNodeValidated);
	/*=================================================================== */
	/**
	 *  @brief  getCheckSumBackup

	 *  @return unsigned short
	 */
	/*=================================================================== */
	ACE_UINT32 getCheckSumBackup();
    /*=================================================================== */
    /**
     *  @brief  isLkfValidBackup			:Method to check the LKF file
     *
     *  @param aNodeValidated    			: bool
     *
     *  @param bNodeValidated 				: bool
     *
     *  @return bool 		  				:true/false
     */
    /*=================================================================== */
    bool isLkfValidBackup(bool& aNodeValidated, bool& bNodeValidated);
    /*=================================================================== */
	/**
	 *  @brief  isEmergencyKeyValid         :Check the validation of Emergency key

	 *  @return bool 						: true/false
	 */
	/*=================================================================== */
	bool isEmergencyKeyValid();
	/*=================================================================== */
	/**
	 *  @brief  isEmergencyModeAllowed
	 *
	 *  @return bool : true/false
	 */
	/*=================================================================== */
	bool isEmergencyModeAllowed();
	/*=================================================================== */
	/**
	 *  @brief  isTestModeAllowed				:Check that whether is it possible
	 *  										 to change the testmode
	 *
	 *  @return bool : true/false
	 */
	/*=================================================================== */
	bool isTestModeAllowed();
	/*=================================================================== */
	/**
	 * @brief  getFingerPrint				:Method to get the finger print
	 *
	 *  @return string
	 */
	/*=================================================================== */
	std::string getFingerPrint();
	/*=================================================================== */
	/**
	 * 	@brief  getFingerprint				: Method to get the hashed fingerprint from sentinel
	 *	@param isDualStack					: Flag to differentiate single stack and dual stack configs
	 *  @return string
	 */
	/*=================================================================== */
	std::string getFingerprint(bool isDualStack);
	/*=================================================================== */
	/**
	 *  @brief  getAllLks					:Method to get the LKF
	 *
	 *  @param 	lklist 						: list
	 *
	 *  @param 	daysToExpire 				: const int
	 *
	 *  @param  mapls						:bool
	 *
	 *  @return bool 						: true/false
	 */
	/*=================================================================== */
	bool getAllLks(std::list<LkData*>& lklist, const int daysToExpire = 0,bool mapls = false);//lm_clean_up
	//LMcleanup_start
	/*=================================================================== */
	/**
	 *  @brief  getAllvendorInfo			:To get the information of the vendor
	 *
	 *  @param 	info 						: char
	 *
	 *  @return bool   						: list
	 */
	/*=================================================================== */
	std::list< pair<std::string,std::string> > getAllvendorInfo(char* info);
	/*=================================================================== */
	/**
	 *  @brief  isitConnfeature			:
	 *
	 *  @param 	feature 					: char
	 *
	 *  @return bool   						: true/false
	 */
	/*=================================================================== */
	bool isitConnfeature(const char* feature);
	/*=================================================================== */
	/**
	 *  @brief  isitDisconnfeature			:
	 *
	 *  @param 	feature 					: char
	 *
	 *  @return bool   						: true/false
	 */
	/*=================================================================== */
	bool isitDisconnfeature(const char* feature);

	//LMcleanup_end

	/*=================================================================== */
	/**
	 *  @brief  getValidLks					:Method to get the valid LKF
	 *
	 *  @param 	lklist 						: list
	 *
	 *  @param 	install 					: bool
	 *
	 *  @param  isLminstCmd					: bool
	 *
	 *  @param  disclklist					: list
	 *
	 *  @param  connlklist					: list
	 *
	 *  @return bool   						: true/false
	 */
	/*=================================================================== */
	//bool getValidLks(std::list<LkData*>& lklist, bool install = false);

	bool getValidLks(std::list<LkData*>& lklist, bool install = false, bool isLminstCmd = false, std::list<LkData*>& disclklist = discL ,std::list<LkData*>& connlklist = connL);//lm_clean_up
	//bool getValidLks(std::list<LkData*>& lklist, bool install = false, bool isLminstCmd = false, std::list<LkData*> &disclklist.assign(std::list<LkData*>(),0), std::list<LkData*> &connlklist.assign(std::list<LkData*>(),0));//lm_clean_up


	//bool getValidLks(std::list<LkData*>& lklist, bool install = false, bool isLminstCmd = false, std::list<LkData*> &disclklist);//lm_clean_up

	//bool getValidLks(std::list<LkData*>& lklist, bool install = false, bool isLminstCmd = false, std::list<LkData*> &connlklist);//lm_clean_up
	/*=================================================================== */
	/**
	 *  @brief  getValidLks					:Method to install the LKF
	 *
	 *  @param fromLkfPath 					: const string
	 *
	 *  @param aNodeValidated 				: bool
	 *
	 *  @param bNodeValidat ed 				: bool
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode installLkf(const std::string& fromLkfPath, bool& aNodeValidated, bool& bNodeValidated);
	/*=================================================================== */
	/**
	 *  @brief  getValidLks
	 *
	 *  @return bool 						: true/false
	 */
	/*=================================================================== */
	bool backup();
	/*=================================================================== */
	/**
	 *  @brief  restore
	 *
	 *  @return bool :true/false
	 */
	/*=================================================================== */
	bool restore();
	/*=================================================================== */
	/**
	 *  @brief  initFromBackup
	 *
	 *  @param aNodeValidated 				: bool
	 *
	 *  @param bNodeValidated 				: bool
	 *
	 *  @param newLkf 						: bool
	 *
	 *  @param checkSum 					: ACE_UINT8
	 *
	 *  @return bool 						:true/false
	 */
	/*=================================================================== */
	bool initFromBackup(bool& aNodeValidated, bool& bNodeValidated, bool newLkf, ACE_UINT32 checkSum=0);
	/*=================================================================== */
	/**
	 *  @brief  init
	 *
	 *  @param aNodeValidated 				: bool
	 *
	 *  @param bNodeValidated 				: bool
	 *
	 *  @param newLkf 					: bool
	 *
	 *  @param checkSum 					: ACE_UINT8
	 *
	 *  @param LoadedBkpFile				: bool 
	 *
	 *  @return bool 					:true/false
	 */
	/* LoadedBkpFile argument has been added to fix the TR-HU93530 */
	/*=================================================================== */
	bool init(bool& aNodeValidated, bool& bNodeValidated, bool newLkf, ACE_UINT32 checkSum=0, bool LoadedBkpFile=false );
	/*=================================================================== */
	/**
	 *  @brief  clean

	 *  @return void
	 */
	/*=================================================================== */
	void clean();
	/*=================================================================== */
	/**
	 *  @brief  finishInstallation
	 *
	 *  @param success 						: bool
	 *
	 *  @param aNodeInitialized 			: bool
	 *
	 *  @param bNodeInitialized 			: bool
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void finishInstallation(bool success, bool& aNodeInitialized, bool& bNodeInitialized,ACE_UINT32 checkSum=0);
	/*=================================================================== */
	/**
	 *  @brief  getCustomExValue
	 *
	 *  @param pCustomExTable 			: VLScustomEx
	 *
	 *  @param pulCount 				: ACE_UINT64
	 *
	 *  @return ACE_INT64
	 */
	/*=================================================================== */
	static ACE_INT64 getCustomExValue(VLScustomEx* pCustomExTable, ACE_UINT64 * pulCount);
	/*=================================================================== */
	/**
	 *  @brief  isLKExpired				:Method to find the expiration date os LKF
	 *
	 *  @param lkExpTime 				: ACE_INT64
	 *
	 *  @return bool 					: true/false
	 */
	/*=================================================================== */
	bool isLKExpired(ACE_INT64 lkExpTime);
	/*=================================================================== */
	/**
	 *  @brief  isLKExpired				:Method to generate the fingerprint

	 *  @return bool 					: true/false
	 */
	/*=================================================================== */
	bool generateFingerPrint();
	bool generateFingerprint_DualStack();		// New method to handle Dual Stack configuration in Virtual Environment
	static VLScustomEx customExList[VLS_MAX_CUSTOMEX_COUNT];
	static int cached;
	static int customExSize;
	static bool isVirtualNode;
	static bool immIPv4FPRequested;
	static bool immIPv6FPRequested;
	/*=================================================================== */
	/**
	 *  @brief  setInternalOMhandler
	 *
	 *  @param  aOmHandlerPtr 			: OmHandler pointer
	 *
	 *  @return void
	 */
	/*=================================================================== */
	static void setInternalOMhandler(OmHandler*  aOmHandlerPtr);

	static std::string getHashedFingerprintFromLKF(const std::string lkfPath);
private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	 =================================================================== */
	struct Key
	{
		std::string id;
		std::string ver;
	};

	static std::string theSid;
	/*=================================================================== */
	/**
	 *  @brief formatFingerPrint
	 *
	 *  @param lockCode        				: const char pointer
	 *
	 *  @return string
	 */
	/*=================================================================== */
	static std::string formatFingerPrint(const char* lockCode);
	/*=================================================================== */
	/**
	 *  @brief getHostDomainName
	 *
	 *  @param domName 						: string
	 *
	 *  @return bool 						: true/false
	 */
	/*=================================================================== */
	static bool getHostDomainName(std::string& domName);
	/*=================================================================== */
	/**
	 *  @brief fetchSid
	 *
	 *  @param sid 						: string
	 *
	 *  @return bool 					: true/false
	 */
	/*=================================================================== */
	static bool fetchSid(std::string& sid);
	static bool fetchSid(std::string& IPv4Fingerprint, std::string& IPv6Fingerprint);
	/*=================================================================== *//**
	 *  @brief getKeysFromLservrc
	 *
	 *  @param lservrcPath 				: const string
	 *
	 *  @param keys 					: vector
	 *
	 *  @return void
	 */
	/*=================================================================== */
	static void getKeysFromLservrc(const std::string& lservrcPath, std::vector<ACS_LM_Sentinel::Key>& keys);
	/*=================================================================== */
	/**
	 *  @brief tokenize
	 *
	 *  @param str 						: const string
	 *
	 *  @param tokens 					: vector

	 *  @param delimiters 				: const string
	 *
	 *  @return void
	 */
	/*=================================================================== */
	static void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");
	std::string fingerPrint;
	std::string theCurrentPath;
	std::string backupPath1;
	std::string backupPath2;
	ACE_UINT32 lkfCheckSum;
	ACE_HANDLE hThread;
	ACE_Event *  bkpThreadStopEvent;
	static OmHandler*  theOmHandlerPtr;

#ifdef ACS_LM_CUTE_TEST
 friend class CUTE_LM_Sentinel;
#endif

private: //Disable copy
	//*=================================================================== */
	/**
		@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_Sentinel(const ACS_LM_Sentinel&);
	//*=================================================================== */
	/**
		@brief      operator
	 */
	/*=================================================================== */
	ACS_LM_Sentinel operator=(const ACS_LM_Sentinel&);
};

#endif
