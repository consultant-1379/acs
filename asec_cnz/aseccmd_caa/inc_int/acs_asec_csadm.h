/*============================================================== */
/**
   @file   acs_asec_csadm.h

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


#ifndef ACS_ASEC_CSADM_H
#define ACS_ASEC_CSADM_H

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
#include <syslog.h>
#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "saAis.h"
//#include "ACS_APGCC_RuntimeOwner.h"
#include "ACS_TRA_Logging.h"

using namespace std;

	/*===============================================================
				  DIRECTIVE DECLARATION SECTION
	================================================================= */
#define LINE_SIZE 200
#define CMD_LEN 100
#define MAX_EXTN_GRUP 20
#define LOG_SIZE 1000
#define RDN_NAME "asecAdminId=1,acsSecurityMId=1"
#define TRUE 1
#define FALSE 0

typedef enum {
	INIT = 1,
	ALLOW = 2,
	BLOCK = 3,
	STATUS = 4
}operationId;

	/*===============================================================
				  CLASS DECLARATION SECTION
	================================================================= */
	/*===================================================================*/
	/**   @class ACS_ASEC_csadm acs_asec_csadm.h
      	*	@brief ACS_ASEc_csadm class
	*	@author xsonnan
	*	@date 2011-02-25
	*	@version      
	*
        *      ACS_ASEC_csadm class is used as the object manager class
	*/
	/*=================================================================== */
	//class ACS_ASEC_csadm: public OmHandler{
	class ACS_ASEC_csadm{

	/*=================================================================== 
                        PRIVATE DECLARATION SECTION
	=================================================================== */

	private:

	/*=================================================================== 
                        PRIVATE DECLARATION SECTION
	=================================================================== */

	/*=================================================================== */
   	/**

      		@brief          invokeAdminOperation

                       		It is used to invoke the admin operation

      		@param          i_dn
                       		It is a const string variable that contains the dn name. 

      		@param          i_operationId
                       		It is a unsigned int that contains operation id  

      		@param          i_params
                       		It contains the paramters of the admin method called.


      		@param          i_timeout
                       		It contains the timeout information. 

		@param 		aOIValidationReturnValue
				It contains the OI validation return value.
 
      		@return          SaAisErrorT

      		@exception       none
   	*/
	/*=================================================================== */
	



		SaAisErrorT invokeAdminOperation(const std::string & i_dn, unsigned int i_operationId ,
                                const SaImmAdminOperationParamsT_2 ** i_params, SaTimeT i_timeout,SaAisErrorT &aOIValidationReturnValue);
		

	 /*=================================================================== */
        /**

                @brief          CreateIMMParam

                                It is used to create the IMM paramters

                @param          aParam
                                It is the out parameter that contains the imm parameter 

                @param          arg
                                It is the in paramter that contains the argument for which 
				the imm parameter is to be generated 


                @return         void 


                @exception       none
        */
        /*=================================================================== */


		void CreateIMMParam(SaImmAdminOperationParamsT_2* & aParam , std::string arg);

	 /*=================================================================== */
        /**

                @brief          immutil_new_attrValue

                                It is used to generate the parameter value

                @param          str
                                It is the in parameter taking the argument value


                @return         void


                @exception       none
        */
        /*=================================================================== */

	
		void* immutil_new_attrValue(const char *str);


	/*=================================================================== 
                        PROTECTED METHODS
	=================================================================== */
	protected:

	/*=====================================================================
                        PUBLIC DECLARATION SECTION
	==================================================================== */
			
	public:
	
	/*====================================================================
                        PUBLIC ATTRIBUTES
	==================================================================== */
	
	

	/*=====================================================================
                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
   	/**

      		@brief          ACS_ASEC_csadm class constructor 

                		It is an empty constructor

      		@return          void

      		@exception       none
   	*/
	/*=================================================================== */

		ACS_ASEC_csadm(){};

	/*=================================================================== 
                        CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
   	/**

     		 @brief         ACS_ASEC_csadm class destructor

                	        It is an empty destructor


	         @return          void

      		 @exception       none
   	*/
	/*=================================================================== */
		
		~ACS_ASEC_csadm(){};

	/*=================================================================== 
                           PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
   	/**
     		 @brief  Usage() method
	  
			It is the method used for giving the usage error.     

      		@return      void

      		@exception   none
    	*/
	/*=================================================================== */
		void Usage(void);

	 /*=================================================================== */
        /**
                 @brief  confirm() method

                        It is the method used for getting the confirmation from
			console.

                @return  bool    


                @exception   none
        */
        /*=================================================================== */

		bool confirm(void);
	 /*=================================================================== */
        /**
                 @brief  callIMMAdminOp() method

                        It is the method used to call the corresponding admin method.

		@param string admin_method
		
		@param string arg

                @return void 


                @exception   none
        */
        /*=================================================================== */


		void callIMMAdminOp(const unsigned int oprId, std::string arg);
			
	 /*=================================================================== */
        /**
                @brief CheckActiveNode

                        It is used to check if the node is active.

                @return bool  


                @exception   none
        */
        /*=================================================================== */

		ACE_INT32 CheckActiveNode();		
		bool isCSADMServiceRunning(std::string aServiceName);
		int getExitCode(int &,int &);
		void displayErrorMessage();
		void signalHandler( int signum );	
	};

	class Lock{
		private:
			bool lock_acquired;
			void releaseLock();
			bool createLockfile();
		public:
			Lock();
			bool acquireLock();
			~Lock();
	};	

#endif /* end ACS_ASEC_CSADM_H*/

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





