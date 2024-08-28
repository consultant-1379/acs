/*============================================================== */
/**
   @file   acs_asec_integrity.h

   @brief 
	  
	  
   @version 1.0.0
*/
/*
   HISTORY
   
   


   PR           DATE      INITIALS    DESCRIPTION
   -------------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
   ============================================================= */

/*==============================================================
		    DIRECTIVE DECLARATION SECTION
================================================================ */


#ifndef ACS_SEC_INTEGRETY_H
#define ACS_SEC_INTEGRETY_H

/*==============================================================
                    INCLUDE DECLARATION SECTION
================================================================ */


#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdint.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <ctype.h>
#include <vector>
#include "ACS_TRA_Logging.h"

using namespace std;

/*===============================================================
		  DIRECTIVE DECLARATION SECTION
================================================================= */

#define ACL_FILE "/opt/ap/acs/conf/acs_lct_acldata.conf"
//#define SHA1_FILE "/opt/ap/acs/conf/acs_lct_sha1verify.conf"
#define SHA1_FILE "/cluster/storage/system/config/acs_lct/acs_lct_sha1verify.conf"
#define LCTREPORT_FILE "/var/log/acs/integrityapreport.log"
#define SHA1_BASE_FILE "/opt/ap/acs/conf/acs_lct_integrityAP_base.conf"
#define LINE_SIZE 1024
#define CMD_LEN 100
#define MAX_EXTN_GRUP 20
#define LOG_SIZE 1000

/*===============================================================
		  CLASS DECLARATION SECTION
================================================================= */


	class ACS_SEC_Integrity {

/*===============================================================
                  PRIVATE DECLARATION SECTION
================================================================= */


	private:

/*=================================================================== 
                        PRIVATE ATTRIBUTE
=================================================================== */

			
		typedef struct {
			char *f_file;
			char *f_owner;
			char *f_group;
			char *f_base_perm;
			char *f_spcl_perm;
			char *f_files_ignored;
		}ACS_SEC_BasicAcl;


		ACS_SEC_BasicAcl basic_acl;

		struct stat stat_buff;
		struct stat stat_sha1_buff;
		struct passwd  *pwd;
		struct group   *grp;

		
		static char *base_perm_arry[];	
		
		static char *spcl_perm_arry[];

		char *g_acl_str;

		ACS_TRA_Logging log;
		vector<string> skipDir_list;
		vector<string> AddFile_list;
		bool includeFile;
                

/*===================================================================
                        PRIVATE METHOD
=================================================================== */

/*=================================================================== */
        /**
                 @brief  extractAcls() method

                        It is used to extract the acls of the files.

		@param  acl_str

                @return      int

                @exception   none
        */
/*=================================================================== */

	
		int extractAcls(char* acl_str);

/*=================================================================== */
        /**
                 @brief  launchCmd() method

                        It is used to launch the command.

                @param  cmd
			this contains the command to be executed

		@param mode
			this contains the mode of running the command

		@param mode
			this is the out paramater of the functions
		

                @return      int

                @exception   none
        */
/*=================================================================== */


		int launchCmd(	string cmd,
				const char* mode,
				char *str);

/*=================================================================== */
        /**
                 @brief  count_tokens() method

                        It is used to count the number of tokens.

                @param  str
                        it contains the string which needs to be tokenised


                @return      int

                @exception   none
        */
/*=================================================================== */

		int count_tokens(const char* str);

/*=================================================================== */
        /**
                 @brief  verify_stat_func()  method

                        It is used to verify the acls.


                @return      int

                @exception   none
        */
/*=================================================================== */

		int verify_stat_func(void);
/*=================================================================== */
        /**
                 @brief  WriteReport()  method

                        It is used to write the reports.


                @return      void

                @exception   none
        */
/*=================================================================== */

		void WriteReport(void);

/*=================================================================== */
        /**
                 @brief  gettoken()  method

                        It is used to get the tokens.


                @return      char*

                @exception   none
        */
/*=================================================================== */


		char *gettoken(void);

/*=================================================================== */
        /**
                 @brief  seek_dir()  method

                        It is used to find out the directory.
	
		@param       vector list 
			     list of files found in system

		@param 	     sha1sum file
		
                @return      int

                @exception   none
        */
/*=================================================================== */


		int seek_dir(vector<string>& ,string ,bool ignoreDir=false);



/*=================================================================== */
        /**
                 @brief  CheckIfNew()  method

                        It is used to check is any new files exist.

                @param     file name  

                @return      bool

                @exception   none
        */
/*=================================================================== */

	
		bool CheckIfNew(string);	

/*=================================================================== */
        /**
                 @brief  check_acls()  method

                        It is used to check the acls of a file.

                @param     file name

                @return      int

                @exception   none
        */
/*=================================================================== */
	
		int check_acls(char* f);

/*=================================================================== */
        /**
                 @brief  scan_files()  method

                        It is used list the files starting with particular string entries.

                @param     vector list

		@param 	    path name

		@param     string value

		@param     length of the starting string

                @return      int

                @exception   none
        */
/*=================================================================== */


		int scan_files(vector<string>& mylist, string path,char* str,int len);

/*=================================================================== */
        /**
                 @brief  seek_files()  method

                        It is used list the files under a directory.

                @param     vector list

                @param      path name

                @return      int

                @exception   none
        */
/*=================================================================== */
		
		int seek_files(vector<string>& mylist, string path);




	protected:
/*===============================================================
                  PROTECTED DECLARATION SECTION
================================================================= */

			
	public:

/*===============================================================
                  PUBLIC DECLARATION SECTION
================================================================= */

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
        /**

                @brief          ACS_SEC_Integrity class constructor


                @return          void

                @exception       none
        */
/*=================================================================== */
		

		ACS_SEC_Integrity();

/*=================================================================== */
        /**

                @brief          ACS_SEC_Integrity class destructor


                @return          void

                @exception       none
        */
/*=================================================================== */

		~ACS_SEC_Integrity();
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
        /**
                 @brief  verify_sha1sum() method

                        It is used to verify the sha1sum of the files.

                @return      void

                @exception   none
        */
/*=================================================================== */
	
	
		void verify_sha1sum(void);
/*=================================================================== */
        /**
                 @brief  verify_acls() method

                        It is used to verify the acls of the files.

                @return      void

                @exception   none
        */
/*=================================================================== */

		void verify_acls(void);

/*=================================================================== */
        /**
                 @brief check_new_files() method

                        It is used to check for new files and directories

                @return      void

                @exception   none
        */
/*=================================================================== */

		void check_new_files(void);

/*=================================================================== */
        /**
                 @brief Usage() method

                        It is used to check for new files and directories

                @return      void

                @exception   none
        */
/*=================================================================== */


		void Usage(void);

/*=================================================================== */
        /**
                 @brief OpenFiles() method

                        It is used to open all the files

                @return      void

                @exception   none
        */
/*=================================================================== */


		int OpenFiles(void);

/*=================================================================== */
        /**
                 @brief CloseFiles() method

                        It is used to close  all the files

                @return      void

                @exception   none
        */
/*=================================================================== */
		
		int CloseFiles(void);	
		int verify_baseline_file(void);
	};

#endif /* end ACS_SEC_INTEGRETY_H*/

	//----------------------------------------------------------------------------
	//
	//  COPYRIGHT Ericsson AB 2010
	//
	//  The copyright to the computer program(s) herein is the property of
	//  ERICSSON AB, Sweden. The programs may be used and/or copied only
	//  with the written permission from ERICSSON AB or in accordance with
	//  the terms and conditions stipulated in the agreement/contract under
	//  which the program(s) have been supplied.
	//
	//----------------------------------------------------------------------------





