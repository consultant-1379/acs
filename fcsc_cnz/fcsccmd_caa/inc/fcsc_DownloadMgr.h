/*=================================================================== */
/**
   @file   FCSC_DOWNLOADMGR_H_

   @brief Header file for SoftwareUpdate Module.

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

/*=====================================================================
              DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef FCSC_DOWNLOADMGR_H_
#define FCSC_DOWNLOADMGR_H_
/*====================================================================
              INCLUDE DECLARATION SECTION
==================================================================== */
#ifndef FCSC_PCF_COMMON_H
#include "fcsc_Common.h"
#endif

using namespace std;

struct FtpFile
{
        const char* filename;
        FILE * stream;
};

/*class FtpGet
{
        public:

};
*/

/*=====================================================================
                       CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The fcsc_DownloadMgr contains the functionality for downloading the SDPs from OSS.
*/
/*=================================================================== */
class fcsc_DownloadMgr
{ /*=====================================================================
                    PRIVATE DECLARATION SECTION
    ==================================================================== */
    private :
     //ACE_INT32 exec_cmd(string cmd, const ACE_TCHAR *mode = "w");
	/*=================================================================== */
	      /**

	        @brief           Method used to write the data into file

	        @par             None

	        @pre             None

	        @post            None

	        @param                   buffer

	        @param                   size

	        @param                   nmeb

	        @param                   stream

	        @return          None

            @exception       None
          */
    /*=================================================================== */
     static size_t my_fwrite(void * buffer,size_t size,size_t nmeb, void *stream);
    /*=====================================================================
                          PUBLIC DECLARATION SECTION
     ==================================================================== */
    public :
     /*=====================================================================
                          CLASS CONSTRUCTOR
       ==================================================================== */
     /*=================================================================== */
          /**

            @brief           default constructor for fcsc_DownloadMgr

            @par             None

            @pre             None

            @post            None

            @param           None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     fcsc_DownloadMgr();
     /*=================================================================== */
          /**

            @brief           constructor for fcsc_DownloadMgr for initializing the downloadMgr as per protocol

            @par             None

            @pre             None

            @post            None

            @param           protocol

            @return          None

            @exception       None
          */
     /*=================================================================== */
     fcsc_DownloadMgr(ACE_TCHAR *protocol);
     /*===================================================================
                             PUBLIC METHOD
       =================================================================== */
     /*=================================================================== */
          /**

            @brief           Method used to print the downloaded path depending on protocol

            @par             None

            @pre             None

            @post            None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     void printDownloadPath();
     /*=================================================================== */
          /**

            @brief           Method used to cleanup the download folder

            @par             None

            @pre             None

            @post            None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     ACE_INT32 cleanDownloadFolder();
     /*=================================================================== */
          /**

             @brief           Method used to download the SDPs from OSS

             @par             None

             @pre             None

             @post            None

             @param           url
                                    url location where OSS is hosting the SDPs

             @param           uid
                                    user id for the OSS site

             @param           pwd
                                    password for OSS site

             @param           files
                                    list of files to be downloaded

             @return          0 on success
                                            Fail otherwise

             @exception       None
          */
      /*=================================================================== */
     int download(string url,string uid,string pwd,vector<SdpRecord>files);

};

#endif /* FCSC_DOWNLOADMGR_H_ */

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

