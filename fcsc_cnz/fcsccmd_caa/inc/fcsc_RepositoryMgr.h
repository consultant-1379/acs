/*=================================================================== */
/**
   @file   FCSC_APG_REP_H

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
#ifndef FCSC_APG_REP_H
#define FCSC_APG_REP_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#ifndef FCSC_PCF_COMMON_H
#include "fcsc_Common.h"
#endif

#include <sys/statvfs.h>

using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The APG_RepositoryMgr class represents
                          for APG Repository Management System .
*/
/*=================================================================== */
class APG_RepositoryMgr : public PCF_Util 
{
    /*=====================================================================
                  PROTECTED DECLARATION SECTION
    ==================================================================== */
   protected:
	/*===================================================================
	              PROTECTED ATTRIBUTE
	  =================================================================== */

	/*=================================================================== */
	      /**
	         @brief      contains the source sdp list
	       */
	/*=================================================================== */
      vector<string> src_sdp_list;
      /*===================================================================
                                                      PROTECTED METHOD
        =================================================================== */

      /*=================================================================== */
         /**

                @brief           method used to copy the source file to destination

                @par             None

                @pre             None

                @post            None

                @param                   srcFile
                                                contains source file path

                @param                   destFile
                                                contains destination file path

                @return          Pass on success
                                                 Fail otherwise.

                @exception       None
         */
      /*=================================================================== */
      bool copyFile(string srcFile, string destFile);
      /*=================================================================== */
          /**

             @brief          To copy SDPs to APG Repository.
							 copies list of SDPs present in src_sdp_list from src to dest folder;
							 common for both updated SDPs(downloaded) as wells as the remaining SDPs to be copied from the current_CSP folder.


             @par             None

             @pre             None

             @post            None

             @param           srcPath
                                        Path where source SDPs are residing

             @param           destPath
                                        APG repository path

             @return          0 on success
                                          Fail otherwise

             @exception       None
          */
      /*=================================================================== */
      ACE_INT32 cpTo_Repository(string srcPath, string destPath);
   /*=====================================================================
                      PUBLIC DECLARATION SECTION
   ==================================================================== */
   public:
      /*=====================================================================
                      CLASS CONSTRUCTORS
        ==================================================================== */
      /*=================================================================== */
                /**

                       @brief           constructor for APG_RepositoryMgr

                       @par             None

                       @pre             None

                       @post            None

                       @param                   pcfFilePath

                       @return          None

                       @exception       None
                */
      /*=================================================================== */
      APG_RepositoryMgr(const ACE_TCHAR* pcfFilePath);
      /*=================================================================== */
                 /**

                        @brief           constructor for APG_RepositoryMgr

                        @par             None

                        @pre             None

                        @post            None

                        @param                   sdp_list
                                                         contains sdp list

                        @param                   bcreate
                                                         flag for creation of updated_CSP directory
                        @return          None

                        @exception       None
                 */
      /*=================================================================== */
      APG_RepositoryMgr(vector<SdpRecord> sdp_list, bool bcreate = true);
      /*===================================================================
                        CLASS DESTRUCTOR
       =================================================================== */
      /*=================================================================== */
                 /**

                        @brief           Destroys the APG_RepositoryMgr object.

                        @par             None

                        @pre             None

                        @post            None

                        @return          None

                        @exception       None
                 */
      /*=================================================================== */
      ~APG_RepositoryMgr();
      /*=================================================================== */
                 /**

                        @brief           Returns list of SDPs needs to be copied from current CSP to update CSP

                        @par             None

                        @pre             None

                        @post            None

                        @return          0 on success
                                                      Fail otherwise

                        @exception       None
                 */
      /*=================================================================== */
      ACE_INT32 get_SrcSdpList();
      /*=================================================================== */
                /**

                       @brief           copies list of SDPs present in pkgList from src to dest folder

                       @par             None

                       @pre             None

                       @post            None

                       @param           srcPath
										src folder path

                       @param           destPath
										dest folder path

                       @return          0 on success
                                                     Fail otherwise

                       @exception       None
                */
      /*=================================================================== */
      ACE_INT32 cpTo_APGRepository(string srcPath, string destPath);
      /*=================================================================== */
                 /**

                        @brief           Method used to check the free space in the disk

                        @par             None

                        @pre             None

                        @post            None

                        @param           updatedSdps

                        @return          None

                        @exception       None
                 */
       /*=================================================================== */
      bool checkFreeSpace(vector<SdpRecord> &updatedSdps);
};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The CoreMW_RepositoryMgr class represents
              for CoreMW Repository Management System .
*/
/*=================================================================== */
class CoreMW_RepositoryMgr : public PCF_Util
{
  /*=====================================================================
                   PRIVATE DECLARATION SECTION
  ==================================================================== */
  private:
	/*===================================================================
	               PRIVATE ATTRIBUTE
	 =================================================================== */

    /*=================================================================== */
	           /**
	                  @brief      contains the filename
	           */
    /*=================================================================== */
     ACE_TCHAR *fileName;
   public:
     /*=====================================================================
                    CLASS CONSTRUCTOR
       ==================================================================== */
     /*=================================================================== */
               /**

                      @brief           constructor for CoreMW_RepositoryMgr

                      @par             None

                      @pre             None

                      @post            None

                      @return          None

                      @exception       None
               */
     /*=================================================================== */
     CoreMW_RepositoryMgr();
     /*===================================================================
                   CLASS DESTRUCTOR
       =================================================================== */
     /*=================================================================== */
              /**

                     @brief           Destroys the CoreMW_RepositoryMgr object.

                     @par             None

                     @pre             None

                     @post            None

                     @return          None

                     @exception       None
              */
     /*=================================================================== */
     ~CoreMW_RepositoryMgr();
     /*===================================================================
                      PUBLIC METHOD
      =================================================================== */

     /*=================================================================== */
                /**

                       @brief           method used to cleanup the coreMW repository

                       @par             None

                       @pre             None

                       @post            None

                       return           None

                       @exception       None
                */
      /*=================================================================== */
     ACE_INT32 CoreMW_Cleanup();
     /*=================================================================== */
               /**

                      @brief           method used for import the coreMW repository

                      @par             None

                      @pre             None

                      @post            None

                      return           None

                      @exception       None
               */
     /*=================================================================== */
     ACE_INT32 CoreMW_Import();
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
