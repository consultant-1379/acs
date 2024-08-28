 /*=================================================================== */
        /**
           @file   FCSC_COMP_CHK_H

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
#ifndef FCSC_COMP_CHK_H
#define FCSC_COMP_CHK_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
//#ifndef FCSC_PCF_COMMON_H
#include "fcsc_Common.h"
//#endif

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The Sw_Compatibility_Chk class represents
                           for software compatibility check.
*/
/*=================================================================== */
class Sw_Compatibility_Chk
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
	          @brief      contains the current version
	       */
	 /*=================================================================== */
     vector<string> cur_version;
     /*=================================================================== */
                /**
                       @brief      contains the supported version
                */
     /*=================================================================== */
     vector<string> supported_versions;
     /*===================================================================
                       PRIVATE METHOD
       =================================================================== */

     /*=================================================================== */
          /**

             @brief           extract the value version from PCF & stores it in parsed_version

             @par             None

             @pre             None

             @post            None

             @param           version
                                       xml tag whose value is to be extracted

             @param           parsed_version
                                       out parameter, contains the value of version

             @return          0 on success
                                           Fail otherwise

             @exception       None
          */
     /*=================================================================== */
     ACE_INT32 parseVersion(const ACE_TCHAR* version, vector<string> &parsed_version);
     /*=================================================================== */
          /**

            @brief           recursively traverse the xml tree & get value of xml node version
                             internally used by parse_pcf();

            @par             None

            @pre             None

            @post            None

            @param           a_node
                                     current root fo the xml tree

            @param           tag
                                     xml tag to be searched

            @param           version_data
                                     value of xml tag

            @param           desired_content
                                     if specified, the value of xml tag should match with it

            @return          None

            @exception       None
         */
   /*=================================================================== */
     void get_elements(xmlNode * a_node, ACE_TCHAR* tag, vector<string> &version_data, ACE_TCHAR* desired_content );
   /*=====================================================================
                         PUBLIC DECLARATION SECTION
    ==================================================================== */
   public:
     /*=====================================================================
                         CLASS CONSTRUCTORS
      ==================================================================== */
     /*=================================================================== */
          /**

            @brief           Default constructor for Sw_Compatibility_Chk

            @par             None

            @pre             None

            @post            None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     Sw_Compatibility_Chk();
     /*===================================================================
                                        CLASS DESTRUCTOR
       =================================================================== */
     /*=================================================================== */
         /**

           @brief           Destructor of Sw_Compatibility_Chk object.

           @par             None

           @pre             None

           @post            None

           @return          None

           @exception       None
         */
     /*=================================================================== */
     ~Sw_Compatibility_Chk();
     /*=================================================================== */
          /**

             @brief           To parse the pcf file & get the value of tag

             @par             None

             @pre             None

             @post            None

             @param           file_name
                                             Contains PCF file path

             @param           tag
                                             Contains xml tag name

             @param           desired_content
                                             if specified, the value of tag should match the value of desired_content

             @return          list of values of tag

             @exception       None
                */
     /*=================================================================== */
     vector<string> parse_pcf(ACE_TCHAR* file_name , ACE_TCHAR* tag, ACE_TCHAR* desired_content=NULL );
     /*=================================================================== */
          /**

            @brief           Method used to compare the current version with supported version

            @par             None

            @pre             None

            @post            None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     ACE_INT32 wildCardCompare();
     /*=================================================================== */
          /**

            @brief           To verify the presence of campaign file in PCF

            @par             None

            @pre             None

            @post            None

            @return          None

            @exception       None
          */
     /*=================================================================== */
     ACE_INT32 verifyCampaign();
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
