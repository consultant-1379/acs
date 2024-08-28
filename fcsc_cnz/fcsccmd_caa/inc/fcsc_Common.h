/*=================================================================== */
/**
   @file   FCSC_PCF_COMMON_H

   @brief Header file for Common functionality of SoftwareUpdate Module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */



#ifndef FCSC_PCF_COMMON_H
#define FCSC_PCF_COMMON_H
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "ace/OS_NS_stdio.h"
#include "ace/ACE.h"
#include "ace/Basic_Types.h"

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#define SDPNAME_TAG "filename"
#define RSTATE_TAG "rstate"
#define SIGNATURE_TAG "signature"
#define SDPSIZE_TAG "size"



using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The fcsc_Config represents for global paths, variables .
*/
/*=================================================================== */
class fcsc_Config
{
/*=====================================================================
                      PUBLIC DECLARATION SECTION
 ==================================================================== */

   public :
   /*===================================================================
	                  PUBLIC ATTRIBUTE
    =================================================================== */
	/*=================================================================== */
	   /**
	     @brief      contains the root directory path for APG(/data)
	   */
	/*=================================================================== */
	 const static string FCSC_ROOT_DIR;
	 /*=================================================================== */
	   /**
	     @brief      contains the download path depending on protocol used
	   */
	 /*=================================================================== */
	 static string FCSC_DOWNLOAD_PATH;
	 /*=================================================================== */
	   /**
	     @brief      contains the FTP download path
	   */
	 /*=================================================================== */
     const static string FCSC_FTP_PATH;
     /*=================================================================== */
       /**
         @brief      contains the SFTP download path
       */
     /*=================================================================== */
     const static string FCSC_SFTP_PATH;
     /*=================================================================== */
     	   /**
     	     @brief      contains the APG Repository path
     	   */
     /*=================================================================== */
     const static string FCSC_APG_REPOSITORY_PATH;
     /*=================================================================== */
     	   /**
     	     @brief      contains the name of the download directory
     	   */
     /*=================================================================== */
     const static string FCSC_DOWNLOAD_DIR_NAME;
     /*=================================================================== */
     	   /**
     	     @brief      contains the name of updated CSP folder in APG Repository
     	   */
     /*=================================================================== */
     const static string FCSC_APG_UPDCSP_DIR;
     /*=================================================================== */
      	   /**
       	     @brief      contains the name of current CSP folder in APG Repository
       	   */
     /*=================================================================== */
     const static string FCSC_APG_CURCSP_DIR;
     /*=================================================================== */
      	   /**
       	     @brief      contains the name of PCF xml file
       	   */
     /*=================================================================== */
     const static string FCSC_PCF_FILE_NAME;
     /*=================================================================== */
            /**
              @brief      contains the SDP integrity file path
            */
     /*=================================================================== */
     const static string FCSC_CHKSUM_FILE_PATH;
     /*=================================================================== */
            /**
              @brief      contains the SoftwareUpdate Config file path
            */
     /*=================================================================== */
     const static string FCSC_CONFIG_FILE_PATH;
     /*=================================================================== */
            /**
              @brief      contains the name of PCF bundle
            */
     /*=================================================================== */
     const static string FCSC_PCF_BUNDLE_NAME;
     /*===================================================================
                           PUBLIC METHOD
       =================================================================== */
     /*=================================================================== */
     /*=================================================================== */
          /**
             @brief       executes the command cmd using popen system call

             @par         None

             @pre         None

             @post        None

             @param       cmd
                                                   used to identify the command

             @param       mode
                                                   used as default read mode

             @return      0 on success
                                                   Fail otherwise.

             @exception   None
                 */
      /*=================================================================== */
     ACE_INT32 exec_cmd(string cmd, const char *mode = "r");
     /*=================================================================== */
          /**
             @brief       writes the configuration parameter 'parameter' & its value 'value to config file

             @par         None

             @pre         None

             @post        None

             @param       parameter
                                                name of config parameter

             @param       value
                                                value of config parameter

             @return      0 on success
                                                 Fail otherwise

             @exception   None
          */
      /*=================================================================== */
     ACE_INT32 writeToCfgFile(string parameter, string value);
     /*=================================================================== */
          /**
             @brief       reads the value of configuration parameter 'parameter' from config file

             @par         None

             @pre         None

             @post        None

             @param       parameter
                                               name of config parameter

             @param       value, out parameter
                                               value of config parameter

             @return      0 on success
                                               Fail otherwise

             @exception   None
          */
     /*=================================================================== */
     ACE_INT32 readFromCfgFile(const string&,string &);


};

/*=================================================================== */
          /**
              @brief    structure for the Sdp Information
          */
/*=================================================================== */
typedef struct
{
   string rState;
   string signature;
   unsigned long long size;
}SdpInfo;

/*=================================================================== */
          /**
              @brief    structure for Sdp Record
          */
/*=================================================================== */
typedef struct record
{
   string sdpName;
   string rState;
   string signature;
   unsigned long long size;
}SdpRecord;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  Common utility for parsing PCF
*/
/*=================================================================== */
class PCF_Util
{
   protected:
	 /*===================================================================
	                    PROTECTED ATTRIBUTE
	   =================================================================== */
	 /*=================================================================== */
	      /**
	             @brief  list an new/updated Sdps
	      */
	 /*=================================================================== */
     vector<SdpRecord> updatedSdps;
     /*=================================================================== */
 	      /**
     	         @brief  map of sdpName & SdpInfo to hold the parsed PCF file information
     	  */
     /*=================================================================== */
     map<string, SdpInfo> Packages;
     /*===================================================================
                              PROTECTED METHODS
       =================================================================== */
     /*=================================================================== */
          /**

              @brief           Parse the PCF XML

              @par             None

              @pre             None

              @post            None

              @param           fileName
                                              contains the PCF file path

              @param           useMap
                                              if true, uses the map Packages for holding the SDP information.
                                              if false, used for SDP lookup from already populated Packages map.
              @return          0 on success
                                              Fail otherwise

              @exception       None
          */
     /*=================================================================== */
     ACE_INT32  parsePCF(const char* fileName, bool useMap=false);
     /*=================================================================== */
          /**

              @brief           recursively traverse the PCF xml tree, used by parsePCF internally

              @par             None

              @pre             None

              @post            None

              @param           a_node
                                           contains xml tree node to be traversed

              @param           useMap
                                           if true, uses the map Packages for holding the SDP information.
                                           if false, used for SDP lookup from already populated Packages map.

              @return          0 on success

              @exception       None
               */
      /*=================================================================== */

     void printElementNames(xmlNode * a_node, bool useMap);
     /*=====================================================================
                          PUBLIC DECLARATION SECTION
       ==================================================================== */
   public:
     /*=====================================================================
                          CLASS CONSTRUCTORS
       ==================================================================== */
     /*=================================================================== */
          /**

            @brief           Default constructor for PCF_Util

            @par             None

            @pre             None

            @post            None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     PCF_Util();
     /*=================================================================== */
          /**
             @brief       Returns the list of new/updated SDPs

             @par         None

             @pre         None

             @post        None

             @param       SdpRecord
                                       SDP list to be populated, out parameter

             @return      0 on success
                                        Fail otherwise

             @exception   None
          */
     /*=================================================================== */
     ACE_INT32 getUpdatedSDPList(vector<SdpRecord> &);
};

/*=====================================================================
                                                        CLASS DECLARATION SECTION
  ==================================================================== */
/*===================================================================*/
       /**
          @brief  The SDP_IntegrityCheck represents for sdp integrity check.
       */
/*=================================================================== */
class SDP_IntegrityCheck
{
	/*=====================================================================
	                   PUBLIC DECLARATION SECTION
	 ==================================================================== */
	public:

	 /*=====================================================================
	                      CLASS CONSTRUCTORS
	   ==================================================================== */
	 /*=================================================================== */
	      /**

             @brief           Default constructor for SDP_IntegrityCheck

             @par             None

             @pre             None

             @post            None

             @return          None

             @exception       None
          */
	 /*=================================================================== */
		SDP_IntegrityCheck(){}
     /*=================================================================== */
	      /**

	         @brief           creates the SDP checksum file to be used by sha1sum

	         @par             None

	         @pre             None

	         @post            None

             @return          0 on success
                                                       Fail otherwise

             @exception       None
          */
	/*=================================================================== */
		ACE_INT32 createSDPChksumFile();
	/*=================================================================== */
	      /**

	         @brief           Checks the integrity of each SDP, uses sha1sum & createSDPChksumFile()

  	         @par             None

	         @pre             None

	         @post            None

	         @return          0 on success
	                                        Fail otherwise

	         @exception       None
	      */
	/*=================================================================== */
		ACE_INT32 checkSDPIntegrity();
};

#endif
//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2001-2006
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------
