/*=================================================================== */
/**
   @file   acs_nsf_biostime_recovery.h

   @brief Header file for NSF module.

          This module contains all the declarations useful to
          specify the NSF_Biostime_Recovery clas.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.

==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_NSF_BIOSTIME_RECOVERY_H
#define ACS_NSF_BIOSTIME_RECOVERY_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>

/*=====================================================================
                          DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Log file path
 */
/*=================================================================== */
#define LOG_FILE_PATH "/tmp/biostimerecovery_powerloss.log"

// BEGIN  TR:H053106
/*=================================================================== */
/**
   @brief NSF Timestamp file path
 */
/*=================================================================== */
#define ACS_NSF_TIMESTAMP_FILEPATH "/opt/ap/acs/etc/nsf/conf/"
/*=================================================================== */
/**
   @brief NSF Timestamp file name
 */
/*=================================================================== */
#define ACS_NSF_TIMESTAMP_FILENAME "NSF_Timestamp.txt"
// END  TR:H053106
/*=================================================================== */
/**
 * @brief NSF Restore Flag file path
 */
#define ACS_NSF_RESTORE_FLAG_FILEPATH_NODEA "/storage/clear/acs-nsf/acs_nsf_restore_flag_a"
#define ACS_NSF_RESTORE_FLAG_FILEPATH_NODEB "/storage/clear/acs-nsf/acs_nsf_restore_flag_b"
/*=================================================================== */
/**
   @brief Sleep time duration
 */
/*=================================================================== */
#define SLEEP_TIME 30

// BEGIN  TR:H053106
/**
 * @brief	NODE_NAME
 */
/*=================================================================== */
enum NODE_NAME
{
	NODE_A=1,    //!< NODE_A
	NODE_B=2, 	 //!< NODE_B
	UNDEFINED=3  //!< UNDEFINED
};
// END  TR:H053106

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @class     NSF_Biostime_Recovery

      @brief     This class the time correction of the OS at bootup
 */
/*=================================================================== */
class NSF_Biostime_Recovery 
{
        /*=====================================================================
                                        PUBLIC DECLARATION SECTION
        ==================================================================== */
public:
        /*=====================================================================
                                        CLASS CONSTRUCTOR
        ==================================================================== */
        /*=================================================================== */
        /**
              @brief     Constructor for NSF_Biostime_Recovery class

                             Constructor for the Biostime recovery class.

        */
        /*=================================================================== */
	NSF_Biostime_Recovery();

        /*=====================================================================
                                CLASS DESTRUCTOR
        ==================================================================== */
        /*=================================================================== */
        /**
                @brief     Destructor for NSF_Biostime_Recovery class.

                             Destructor for the Biostime recovery class.

        */
        /*=================================================================== */
	~NSF_Biostime_Recovery();


        /*===================================================================
                                   PUBLIC METHODS
        =================================================================== */
        /*=================================================================== */
        /**
                @brief     Checks if the Timestamp file is existing or not.

                @par       Deprecated
                           never

                @return    Return Status
				0	Success
				< 0	Failure
				> 0	Warning (No more operation to be done)

                @exception none
        */
        /*=================================================================== */
	int CheckForTimestampFile();

        /*=================================================================== */
        /**
                @brief     Gets the time from the NSF timestamp file.

                @par       Deprecated
                           never

                @return    Return Status
                                True    Success
                                False   Failure

                @exception none
        */
        /*=================================================================== */
        bool GetFileTime();

        /*=================================================================== */
        /**
                @brief     Checks for root privileges to adjust the system time.

                @par       Deprecated
                           never

                @return    Return Status
                                True    Success
                                False   Failure

                @exception none
        */
        /*=================================================================== */
	bool CheckForPrivileges();

        /*=================================================================== */
        /**
                @brief     Compares the current system time with NSF timestamp file

                @par       Deprecated
                           never

                @return    Return Status
                                True    Success
                                False   Failure

                @exception none
        */
        /*=================================================================== */
	bool CheckCurrentTimeWithFileTime();

        /*=================================================================== */
        /**
                @brief     Gets the current system time.

                @par       Deprecated
                           never

                @return    void

                @exception none
        */
        /*=================================================================== */
	void GetCurrentTime();

        /*=================================================================== */
        /**
                @brief     Sleeps the process in case of B-node.

                @par       Deprecated
                           never

                @return    void

                @exception none
        */
        /*=================================================================== */
        void TimeoutByHostname();

        /*=================================================================== */
        /**
                @brief     Opens the log file for logging.

                @par       Deprecated
                           never

                @return    void

                @exception none
        */
        /*=================================================================== */
	void OpenLogFile();

        /*=================================================================== */
        /**
                @brief     Adds the log message to the log file.

                @par       Deprecated
                           never

				@param	   message
						   Message to be added to the log file.

                @return    void

                @exception none
        */
        /*=================================================================== */
	void AddToLogFile(std::string message);

        /*=================================================================== */
        /**
                @brief     Closes the log file.

                @par       Deprecated
                           never

                @return    void

                @exception none
        */
        /*=================================================================== */
	void CloseLogFile();

	// BEGIN  TR:H053106
	/**
	 * @brief	getNode
	 * @return
	 */
	/*=================================================================== */
	NODE_NAME getNode();
	NODE_NAME getNodeid();
		/*=================================================================== */
	// END  TR:H053106
private:
	/*=====================================================================
	                                        PRIVATE DECLARATION SECTION
	  ==================================================================== */
	/*===================================================================
                                   PRIVATE ATTRIBUTES
        =================================================================== */
        /*=================================================================== */
        /**
                @brief   NSF Timestamp time

        */
        /*=================================================================== */
	int savedTime;

	/*=================================================================== */
        /**
                @brief   Current systemt tile

        */
        /*=================================================================== */
	int systemTime;

	/*=================================================================== */
        /**
                @brief   NSF Timestamp file fd.

        */
        /*=================================================================== */
	ACE_HANDLE  fd;


};

#endif 
