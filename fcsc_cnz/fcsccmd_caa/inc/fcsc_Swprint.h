/*=================================================================== */
/**
   @file   _FCSC_SWPRINT__

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
/*=====================================================================
                       DIRECTIVE DECLARATION SECTION
  ==================================================================== */
#ifndef _FCSC_SWPRINT__
#define _FCSC_SWPRINT_
/*=====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include<string>
#include<iostream>
#include<memory.h>
using namespace std;
//#include "ACS_SW_Defines.h"
#include "ACS_CC_Types.h"
#include "OmHandler.h"
#include <syslog.h>
#include <saAis.h>


#define ADMIN_OM_HANDLER 0
#define APGCC_OM_HANDLER 1


typedef enum {

FCSC_PREPARE = 1,
FCSC_ACTIVATE,
FCSC_FALLBACK,
FCSC_COMMIT,

}OPER_ID ;

#define SW_CONFIG_OBJ_DNAME "safAPGSWUpdate=SWUpdateInfo,safApp=safImmService"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The fcsc_AdminOperations represents for admin operations an object  .
*/
/*=================================================================== */
class fcsc_AdminOperations
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

	             @brief           Default constructor for fcsc_AdminOperations

	             @par             None

	             @pre             None

	             @post            None

	             @return          None

	             @exception       None
	          */
		 /*=================================================================== */
			fcsc_AdminOperations() { }
		 /*===================================================================
							CLASS DESTRUCTOR
		   =================================================================== */
		 /*=================================================================== */
			 /**

			   @brief           Destructor of fcsc_AdminOperations .

			   @par             None

			   @pre             None

			   @post            None

			   @return          None

			   @exception       None
			 */
		 /*=================================================================== */
			~fcsc_AdminOperations(){ }
		/*=================================================================== */
			  /**

				 @brief           This method is used to get the admin operation for an object of
								  fcscactivate

				 @par             None

				 @pre             None

				 @post            None

				 @param           obj
												 Contains admin object

				 @return          return adminoperation for fcscactivate

				 @exception       None
					*/
		 /*=================================================================== */
			static int fcsc_activate_operation(fcsc_AdminOperations &);
		/*=================================================================== */
			  /**

				 @brief           This method is used to get the admin operation for an object of
								  fcscprepare

				 @par             None

				 @pre             None

				 @post            None

				 @param           obj
												 Contains admin object

				 @return          return adminoperation for fcscprepare

				 @exception       None
					*/
		 /*=================================================================== */
			static int fcsc_prepare_operation(fcsc_AdminOperations &);
		/*=================================================================== */
			  /**

				 @brief			  This method is used to get the admin operation for an object of
								  fcscprepare

				 @par             None

				 @pre             None

				 @post            None

				 @param           obj
												Contains admin object

				 @return          return adminoperation for fcscprepare

				 @exception       None
					*/
		 /*=================================================================== */
			static int fcsc_fallback_operation(fcsc_AdminOperations &);
		/*=================================================================== */
			  /**

				 @brief			  Method used to print the software versions of both the Nodes

				 @par             None

				 @pre             None

				 @post            None

				 @param           object
												 Contains admin object

				 @return		  Software version

				 @exception       None
					*/
		 /*=================================================================== */
			int fcsc_sw_label_print(fcsc_AdminOperations &);


        private:
		/*=====================================================================
							  PRIVATE DECLARATION SECTION
		  ==================================================================== */
		/*===================================================================
							   PRIVATE METHOD
		 =================================================================== */

		 /*=================================================================== */
			  /**

				 @brief           Method is used to get the value of an attribute of an object from IMM

				 @par             None

				 @pre             None

				 @post            None

				 @param           dn
										   Contains Object Name

				 @param           attrName
										   Contains attribute Name
				 @param           attrValue
										   Contains attribute Value

				 @return          0 on success
											   Fail otherwise

				 @exception       None
			  */
		 /*=================================================================== */
                int getFcscAttribute(const char* dn, const char* attrName,char* attrValue);
             /*=================================================================== */
				  /**

					 @brief			 Method is used to invoke the admin operations based on operation Id

					 @par             None

					 @pre             None

					 @post            None

					 @param           i_dn
											   contains the obeject name

					 @param           i_operationId
											   contains the operation id

					 @param           i_params
											   contains the parameter

					 @param           i_timeout
											   contains the time

					 @return          0 on success
												   Fail otherwise

					 @exception       None
				  */
			 /*=================================================================== */
                SaAisErrorT invokeAdminOperation(const std::string & i_dn, unsigned int i_operationId ,
                                const SaImmAdminOperationParamsT_2 ** i_params, SaTimeT i_timeout );
         //       OmHandler fcscAttrHandler;
	//	 OmHandler omHandler;

              /*===================================================================
							  PRIVATE ATTRIBUTE
			   =================================================================== */

			 /*=================================================================== */
				   /**
					  @brief      contains the Handler
				   */
			 /*=================================================================== */
                // At a time one OM handler can be active
                int whichHandler;

};

#endif
