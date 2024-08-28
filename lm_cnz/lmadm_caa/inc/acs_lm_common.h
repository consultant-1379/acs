//****************************************************************************
//
//  NAME
//     ACS_LM_Common.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson  AB, Sweden.
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
//	    190 89-CAA XXX XXXX
//
//  AUTHOR 
//     2008-11-28 by XCSSAGO  PA1
//
//  SEE ALSO 
//     -
//
//****************************************************************************

#ifndef ACS_LM_Common_H
#define ACS_LM_Common_H
#include <stdio.h>
//#include <process.h>
//#include <lm.h>
//#include <clusapi.h>
//#include <resapi.h>
//#include <psapi.h>
//#include <setupapi.h>
//#include <ctime>
//#include <accctrl.h>
//#include <aclapi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
//#include <shlobj.h>
#include <string>
//#include <io.h>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <iomanip>
#include <ace/ACE.h>
//#include "acs_lm_tracemacro.h"
//#include "acs_lm_trace.h"
#include <ACS_TRA_trace.h>
#include "acs_lm_defs.h"
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_Util.H>
//#include <ACS_PRC_Process.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Thread_Manager.h>
//#include "ACS_ExceptionHandler.h"

extern bool fpChangeClusterIP;
extern bool fpChangeNodeName;
/* LM Maintenance Mode */
extern bool emOnInMaintenance; 
extern bool isFromAutoCease; 
extern bool graceOnInMaintenance;
extern ACE_INT64 maintenanceEndTime;
 
#define PAR_ERROR						0
//#define LM_DEFAULT_IP_VALUE              ACE_TEXT("0")

/**
 *      @brief          ThrExitHandler
 *                      Class for handling thread exit.
 *
 */
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ThrExitHandler : public ACE_At_Thread_Exit
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

	        		@brief		init

	        		@return		bool
	 */
	/*=================================================================== */
        static bool init()
        {
                ThrExitHandler *thrExitHandler = new ThrExitHandler();
                if( thrExitHandler == 0 )
                {
                        return false;
                }
                thrExitHandler->setThrStatusInMap(ACE_Thread_Manager::instance()->thr_self(), true);
                int result = ACE_Thread_Manager::instance()->at_exit( thrExitHandler );

                if( result == -1 )
                {
                	return false;
                }
                else
                {
                        return true;
                }
        }
        /*=================================================================== */
        /**

        		@brief		CheckNoThreads         :Method to check the no of threads

        		@return		bool
         */
        /*=================================================================== */
        static bool CheckNoThreads()
        {
                theThrMutex.acquire();

                map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
                while ( itr != theThrStatusMap.end())
                {
                        if( itr->second == true )
                        {
                                theThrMutex.release();
                                return false;
                        }
                        ++itr;
                }
                theThrMutex.release();
                return true;
        }
        /*=================================================================== */
        /**

               		@brief		cleanup

               		@return		void
         */
        /*=================================================================== */
        static void cleanup()
        {
                theThrMutex.acquire();
		if( !theThrStatusMap.empty())
                {
                        theThrStatusMap.clear();
                }
                theThrMutex.release();
                theThrMutex.remove();
        }
        /*=================================================================== */
        /**

                  @brief		CheckAllFuncThreads

                  @return	    bool
         */
        /*=================================================================== */
#if 0
        static bool CheckAllFuncThreads( void )
        {

                //Lock the mutex.
                theThrMutex.acquire();

                if( theThrStatusMap.empty())
                {
                        theThrMutex.release();
                        return false;   //No Thread has been spawned till now.
                }

                //Check if any thread has exited or not.
                for( map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
                        itr != theThrStatusMap.end();  ++itr )
                {
                        if( itr->second == false )
                        {
                                theThrMutex.release();
                                return false;
                        }
                }
		theThrMutex.release();

		// Sending always Functional Threads status as true - Temporary Fix.
		// Sometimes setting Runtime impl is failing that is causing Functional thread exit.
		// This will be removed in Sprint21.
                return true;
        }
#endif

        /*=================================================================== */
        /**

                          @brief		apply

                          @return	    bool
         */
        /*=================================================================== */
        virtual void apply(void)
        {
                if( !theThrStatusMap.empty())
                {
                        setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
                }
        }




private:
        /*=================================================================== */
        /**

                 @brief		setThrStatusInMap

               	 @param	    thrId                    : ACE_thread_t

               	 @param    isThrAlive                : bool

                 @return	void
         */
        /*=================================================================== */
        void setThrStatusInMap( ACE_thread_t thrId, bool isThrAlive)
        {
                //Acquire the Mutex
                theThrMutex.acquire();

                map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.find( thrId );
                if ( itr  != theThrStatusMap.end() )
                {
                        theThrStatusMap.erase( itr );
                }
                theThrStatusMap.insert(std::pair<ACE_thread_t, bool>(thrId, isThrAlive));
                theThrMutex.release();
        }

        static map<ACE_thread_t, bool> theThrStatusMap;
        static ACE_Recursive_Thread_Mutex  theThrMutex;
};



class ACS_LM_Common
{ 
public:

	//Fetching from common DLL
    /*=================================================================== */
    /**

             @brief		getDataDiskPath           :Method use to get the data disk path


           	 @param		logicalName               :string

           	 @param     pszPath                   :string

             @return	bool
     */
    /*=================================================================== */
	static bool getDataDiskPath(const std::string& logicalName,std::string& pszPath);
	/*=================================================================== */

	//static std::string getWindowsErrorText(ACE_UINT64 errorCode);



	/*=================================================================== */
	/**

	             @brief		getApplicationErrorText    :Mehod to use the error text

	           	 @param		appExitCode                :ACS_LM_AppExitCode

	             @return	string
	 */
	/*=================================================================== */
	static std::string getApplicationErrorText(const ACS_LM_AppExitCode appExitCode);
	static std::string getApplicationErrorText(const int appExitCode);

	/*=================================================================== */
	/**
		   @brief	createLmDir    :Mehod to use the error text

		   @param 	dir 		   : string

		   @return  bool 	       : true/false
	 **/
	/*=================================================================== */
	static bool createLmDir(const std::string& dir);
	/*=================================================================== */
	/**

		   @brief	getFileType    :Method to get file type

		   @param 	file 		   : string

		   @return ACS_LM_FileType
	 **/
	/*=================================================================== */
	static ACS_LM_FileType getFileType(const std::string& file);
	/*=================================================================== */
	/**
			@brief	getFileSize    :Method to get file size

			@param file 			: string

	 *  	@return ACE_INT64
	 **/
	/*=================================================================== */
	static ACE_INT64 getFileSize(const std::string& file);
	/*=================================================================== */
	/**
			@brief	getOtherNodeName    :Method to get name of the other node

		    @param otherNodeName        : string

	 *  	@return void
	 **/
	/*=================================================================== */
	static void getOtherNodeName(std::string& otherNodeName);
	/*=================================================================== */
	/**
	        @brief	getOtherNodeIp    :Method to get ip of the other node

            @return string
	 **/
	/*=================================================================== */
	static std::string getOtherNodeIp();
	/*=================================================================== */
	/**
	      @brief	getThisNodeIp    :Method to get ip of the node

	      @return string
	 **/
	/*=================================================================== */
	static std::string getThisNodeIp();
	/*=================================================================== */
	/**

		  @brief	constructDate    : Method to get ip of the node

	      @param    minutes         :  ACE_UINT64

	      @return string
	 **/
	/*=================================================================== */
	static std::string constructDate(const ACE_UINT64 minutes = 0);
	/*=================================================================== */
	/**
		@brief	restoreFile     : Method to restore the file

	 *  @param srcPath	         :  string
	 *
	 *  @param destPath	         :  string
	 *
	 *  @return bool 	         : true/false
	 **/
	/*=================================================================== */
	static bool restoreFile(const std::string& srcPath, const std::string& destPath);
	/*=================================================================== */
	/**
			  @brief backupFile     	: Method to backup the file

			  @param srcPath	 		: string

			  @param destPath	 		: string

			  @return bool 	     		: true/false
	 **/
	/*=================================================================== */
	static bool backupFile(const std::string& srcPath, const std::string& destPath);
	/*=================================================================== */
	/**
	    @brief isCommandAllowed


	    @return bool
	 **/
	/*=================================================================== */
	static bool isCommandAllowed();
	/*=================================================================== */
	/**

		 @brief isStringValid

		 @param srcStr 	: char pointer

	    @return bool 	: true/false
	 **/
	/*=================================================================== */
	static bool isStringValid(char* srcStr);
	/*=================================================================== */
	/**
	   @brief isParnameValid

	   @param srcStr 	: char pointer

	   @return bool 	: true/false

	 **/
	/*=================================================================== */
	static bool isParnameValid(char* srcStr);
	/*=================================================================== */
	/**
	   @brief isAlphanumeric

	   @param srcStr 	: char pointer

	   @return bool 	: true/false
	 **/
	/*=================================================================== */
	static bool isAlphanumeric(char* srcStr);

	/*=================================================================== */
	/**
	    @brief getLmdataLockStatus

	 *  @return bool 	: true/false
	 **/
	/*=================================================================== */
	static bool getLmdataLockStatus();
	/*=================================================================== */
	/**
	   @brief setLmdataLockStatus

	   @param lmdataLock 	: bool

	   @return void
	 **/
	/*=================================================================== */
    static void setLmdataLockStatus(bool lmdataLock);
    static bool getLkfLockStatus();
        static void setLkfLockStatus(bool lkfLock);
	/*=================================================================== */
    /**
     *  @param file 	:  string

     *  @param offset 	: long

     *  @param len 		: ACE_UINT64
     *
     *  @return ACE_UINT8
     **/
	/*=================================================================== */
    static ACE_UINT32 getFileChecksum(const std::string& file, long offset, ACE_UINT64 len);

	/*=================================================================== */
    /**
	   @brief getMemoryCheckSum

       @param startAddress 	    : pointer

       @param len 				: ACE_UINT64

       @return ACE_UINT8
     **/
	/*=================================================================== */
    static ACE_UINT16 getMemoryCheckSum(void *startAddress, ACE_UINT64 len);

	/*=================================================================== */
    static ACE_UINT16 getLmdataMemoryCheckSum(std::string lmdataPath);
    /**
	   @brief strcpy

       @param src 		: const char pointer

       @param dest 	: char pointer

       @param destSize	: int

       @return void
     **/
	/*=================================================================== */
    static void strcpy(const char* src, char* dest, int destSize);

	/*=================================================================== */
    /**
      @brief toUpperCase

      @param str 		: char pointer

      @return void
     **/
	/*=================================================================== */
    static void toUpperCase(char* str);

	/*=================================================================== */
    /**
      @brief getFullPath

       @param path 		: const string

       @return string
     **/
	/*=================================================================== */
    static std::string getFullPath(const std::string& path);
	/*=================================================================== */
    /**
     * @brief encode8
     *  @param destBuf 		: ACE_TCHARACE_TCHAR
     *  @param destPos 		: ACE_UINT64
     *  @param value		: ACE_TCHAR
     *  @return void
     **/
	/*=================================================================== */
    static void encode8(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_TCHAR value);

	/*=================================================================== */
    /**
     * @brief decode8
     *  @param srcBuf 		: const ACE_TCHAR
     *  @param srcPos 		: ACE_UINT64
     *  @return ACE_TCHAR
     **/
	/*=================================================================== */
    static ACE_TCHAR decode8(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos);
	/*=================================================================== */
    /**
     *  @brief encode16
     *  @param destBuf 		: ACE_TCHAR
     *  @param destPos 		: ACE_UINT64
     *  @param value		: ACE_UINT8
     *  @return void
     **/
	/*=================================================================== */
    static void encode16(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT16 value);
    static void encodechecksum16(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT32 value);
	/*=================================================================== */
    /**
     *  @brief decode16
     *  @param srcBuf 		: const ACE_TCHAR
     *  @param srcPos 		: ACE_UINT64
     *  @return ACE_UINT8
     **/
	/*=================================================================== */
    static ACE_UINT16 decode16(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos);
    static ACE_UINT32 decodechecksum16(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos);
	/*=================================================================== */
    /**
     *  @brief encode32
     *  @param destBuf 		: ACE_TCHAR
     *  @param destPos 		: ACE_UINT64
     *  @param value		: ACE_UINT64
     *  @return void
     **/
	/*=================================================================== */
	/* Input argumnet (value) data type changed from ACE_UNIT64 to ACE_UINT32,
           to avoid switching from License Controlled mode to Grace mode during UP installation*/ 
    static void encode32(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT32 value);
	/*=================================================================== */
    /**
     *  @brief decode32
     *  @param srcBuf 		: const ACE_TCHAR
     *  @param srcPos 		: ACE_UINT64
     *  @return ACE_UINT64
     **/
	/*=================================================================== */
    static ACE_UINT64 decode32(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos);
	/*=================================================================== */
    /**
     *  @brief encode64
     *  @param destBuf 		: ACE_TCHAR
     *  @param destPos 		: ACE_UINT64
     *  @param value		: ACE_UINT64
     *  @return void
     **/
	/*=================================================================== */
    static void encode64(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT64 value);
	/*=================================================================== */
    /**
     *  @brief decode64
     *  @param srcBuf 		: const ACE_TCHAR
     *  @param srcPos 		: ACE_UINT64
     *  @return ACE_UINT64
     **/
	/*=================================================================== */
    static ACE_UINT64 decode64(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos);
	/*=================================================================== */
    /**
     *  @brief encodeString120
     *  @param destBuff 		: ACE_TCHAR
     *  @param destPos 		: ACE_UINT64
     *  @param srcStr		: const string
     *  @return void
     **/
	/*=================================================================== */
    static void encodeString120(ACE_TCHAR* destBuff, ACE_UINT64& destPos, const std::string& srcStr);
	/*=================================================================== */
    /**
     * @brief  decodeString120
     *  @param srcBuf 		: const ACE_TCHAR
     *  @param srcPos 		: ACE_UINT64
     *  @return string
     **/
	/*=================================================================== */
    static std::string decodeString120(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos);
	/*=================================================================== */
    /**
     * @brief copyBuffer
     *  @param destBuf 		: ACE_TCHAR
     *  @param destPos 		: ACE_UINT64
     *  @param srcBuf		: const ACE_TCHAR
     *  @param srcPos		: ACE_UINT64
     *  @param srcLen		: ACE_UINT64
     *  @return void
     **/
	/*=================================================================== */
    static void copyBuffer(ACE_TCHAR* destBuf, ACE_UINT64 destPos, const ACE_TCHAR* srcBuf, ACE_UINT64 srcPos, ACE_UINT64 srcLen);
	/*=================================================================== */
    /**
     * @brief isDir
     *  @param path 		: string
     *  @return bool
     **/
	/*=================================================================== */
    static bool isDir(std::string path);
	/*=================================================================== */
    /**
     * @brief getlatestFile
     *  @param path 		: string
     *  @param lservrc 		: string
     *  @return bool
     **/
	/*=================================================================== */
    static bool  getlatestFile(std::string path, std::string &lservrc);
	/*=================================================================== */
    /**
     * @brief GetDateTimeString
     *  @param srctLocalTime 		: time_t
     *  @return string
     **/
	/*=================================================================== */
    static std::string GetDateTimeString(time_t srctLocalTime);
	/*=================================================================== */
    /**
     * @brief s32Spawn
     *  @param ThreadFunc 		: ACE_THR_FUNC_RETURN
     *  @param Arglist 			: void pointer
     *  @param lpszThreadName 	: const ACE_TCHAR
     *  @param s32GrpId 		: ACE_INT32
     *  @return ACE_INT32
     **/
	/*=================================================================== */
    static ACE_INT32 s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
    		ACE_TCHAR* lpszThreadName,ACE_INT32 s32GrpId);
	/*=================================================================== */
    /**
     *  @param ThreadFunc 		: ACE_THR_FUNC_RETURN
     *  @param Arglist 			: void pointer
     *  @param lpszThreadName 	: const ACE_TCHAR
     *  @param s32GrpId 		: ACE_INT32
     *  @return ACE_INT32
     **/
	/*=================================================================== */
    static ACE_INT32 StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
    		void* Arglist,
    		const ACE_TCHAR* lpszThreadName,
    		ACE_INT32 s32GrpId = -1);

	/*=================================================================== */
    /**
     * 	@brief   fetchDnOfRootObjFromIMM
     *  @param aOMHandler 		: OmHandler
     *  @return int
     **/
	/*=================================================================== */
    static int fetchDnOfRootObjFromIMM (OmHandler* aOMHandler);
    static std::string dnOfLMRoot;
    /*=================================================================== */
    /**
     *  @param srcPath 		: string
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool deleteFile(std::string& srcPath);
    static bool doFileDelete(std::string path);
    /*=================================================================== */
    /**
     *  @param aFilePath 		: string
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool isFileExists(std::string& aFilePath);
    /**
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool isLKFExists();
    /*=================================================================== */
    /**
     *  @param aClusterIPAddress 		: string
     *  @param theOmHandlerPtr 		: OmHandler
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool getClusterIPAddress(std::string& aClusterIPAddress,OmHandler*  theOmHandlerPtr);
    /*=================================================================== */
    /**
     *  @param aNodeName 		: string
     *  @param theOmHandlerPtr 		: OmHandler
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool getNodeName(string& aNodeName,OmHandler*  theOmHandlerPtr);
    /*=================================================================== */
    /**
     *  @param aFingerPrint 		: string
     *  @param theOmHandlerPtr 		: OmHandler
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool generateFingerPrint(string& aFingerPrint,OmHandler*  theOmHandlerPtr);
    /*=================================================================== */
    /**
     *     @brief nodeStatus
     *
     *     @return int
     **/
     /*=================================================================== */

    static int nodeStatus();
	static bool getLicenseKeyFilePath( string &flePath);
        static bool getObsoluteFilePath(string, string &);
       static std::string getClusterIPfromConfFile();
    static void getClusterIPfromConfFile(std::string &clusterIPv4, std::string &clusterIPv6);
    /*=================================================================== */
    /**
	 * @brief ACS_LM_Common  :Default constructor
	 *
	 */
    /*=================================================================== */
	ACS_LM_Common();
	/*=================================================================== */
	/**
	 * @brief
	 * destructor
	 */
	/*=================================================================== */
	~ACS_LM_Common();

	static bool getPsaPath(const std::string&, std::string&);
	static int getApt();

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
    static int hwVersion;
	static std::string otherNodeName;
	static const ACE_UINT32 crc16table[256];
	/*=================================================================== */
	/**

	 *  create method:

	 *  @return int
	 **/
	/*=================================================================== */
	static int getHwVerOnOwn();
	/*=================================================================== */
	/**
	 *  create method:

	 *  @return int
	 **/
	/*=================================================================== */
	static int apNumber();
	/*=================================================================== */
	/**

	 *  @param crc	: ACE_UINT8
	 *  @param c	: ACE_UINT8
	 *  @return ACE_UINT8
	 **/
	/*=================================================================== */

	static ACE_UINT16 calculateCRC(ACE_UINT16 crc, ACE_UINT32 c);
	static ACE_UINT32 calculateLKfChecksumCRC(ACE_UINT32 crc, ACE_UINT32 c);

    	static bool lmdataLocked;
   	 static bool lkfLocked;


private: //Disabled
	//ACS_LM_Common();

};

#endif
