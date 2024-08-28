/*============================================================== */
/**
   @file   acs_asec_ObjectImplementer.h

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


#ifndef ACS_ASEC_OBJECT_IMPLEMENTER_H
#define ACS_ASEC_OBJECT_IMPLEMENTER_H


/*===================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <ctype.h>
#include <vector>
#include <ctime>
#include <mntent.h>
#include <ace/Task_T.h>
#include "ace/ACE.h"


#include "ACS_CC_Types.h"
#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Task.h"
#include "saAis.h"
#include "acs_asec_RuntimeObject.h"
#include "acs_asec_dsdServer.h"
#include "ACS_TRA_Logging.h"
#include "acs_apgcc_objectimplementerinterface_V2.h"
#include "acs_apgcc_omhandler.h"

	/*===============================================================
				  DIRECTIVE DECLARATION SECTION
	================================================================= */
#define LINE_SIZE 200
#define NODE_ID_FILE "/etc/opensaf/slot_id"

using namespace std;

	/*===============================================================
				  CLASS DECLARATION SECTION
	================================================================= */

	/*===================================================================*/

        /**   @class ACS_ASEC_ObjImpl acs_asec_csadmOIHandler.h
        *       @brief ACS_ASEC_ObjImpl class
        *       @author xsonnan
        *       @date 2011-02-25
        *       @version
        *
        *      ACS_ASEC_ObjImpl class is used as the object implementer class
        */
        /*=================================================================== */

// Forwards
class Global;
class asecHAClass;

class ACS_ASEC_ObjImpl:public ACE_Task<ACE_SYNCH> {
	/*=====================================================================
                        PRIVATE DECLARATION SECTION
	==================================================================== */

	private:
		Global *m_global_instance;
		SaImmOiImplementerNameT implementerName;
		SaNameT csadmOiImplementerectName;
		ACS_TRA_Logging log;
		asecHAClass* m_haObj;

	/*=====================================================================
                        PROTECTED DECLARATION SECTION
	==================================================================== */

	protected:

	/*=====================================================================
                        PUBLIC DECLARATION SECTION
	==================================================================== */
			
	public:

	 /*=====================================================================
                        CLASS CONSTRUCTORS
        ==================================================================== */

	 /*=================================================================== */
        /**

                @brief          ACS_ASEC_ObjImpl class constructor

                                It is an empty constructor

                @return          void

                @exception       none
        */
        /*=================================================================== */

		ACS_ASEC_ObjImpl();		

	/*===================================================================
                        CLASS DESTRUCTOR
        =================================================================== */
        /*=================================================================== */
        /**

                 @brief         ACS_ASEC_ObjImpl class destructor

                                It is an empty destructor


                 @return          void

                 @exception       none
        */
        /*=================================================================== */


		~ACS_ASEC_ObjImpl();

	/*====================================================================
                        PUBLIC ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
   	/**   
     		 @brief   FileOutputImplName

               	 Detailed  It contains the implementer name
   	*/
	/*=================================================================== */

		static char* FileOutputImplName;

	 /*=================================================================== */
        /**
                 @brief   FileOutputId

                 Detailed  It contains the DN name  of admin method
        */
        /*=================================================================== */

                static char* FileOutputId;
	
	 /*=================================================================== */
        /**
                 @brief   FileOutputParentName

                 Detailed  It contains the IMM parent name
        */
        /*=================================================================== */

                static char* FileOutputParentName;
	
	 /*=================================================================== */
        /**
                 @brief   FileOutputClassName

                 Detailed  It contains the admin class name
        */
        /*=================================================================== */

                static char* FileOutputClassName;



	

	 /*===================================================================
                           PUBLIC METHOD
        =================================================================== */


	 /*=================================================================== */
        /**
                 @brief  saImmOiAdminOperationCallback() method

                        It is the callback method.

                @param  immOiHandle

                @param  invocation
	
		@param  objectName

		@param opId

		@param params

                @return void


                @exception   none
        */
        /*=================================================================== */

	
		static void saImmOiAdminOperationCallback(SaImmOiHandleT immOiHandle,
                                          SaInvocationT invocation,
                                          const SaNameT *objectName,
                                          SaImmAdminOperationIdT opId, const SaImmAdminOperationParamsT_2 **params);
       
	 /*=================================================================== */
        /**
                 @brief  launchCmd() method

                        It is the launch command method.

                @param  cmd
			The command string that needs to be issued

                @param  mode
			The mode of popen


                @return int 


                @exception   none
        */
        /*=================================================================== */


	        int launchCmd(string cmd, const char *mode);

	/*=================================================================== */
        /**
                @brief  register_csadmOiImplementerect() method

                        It registers the OI implementer with the IMM .


                @return SaAisErrorT

                @exception   none
        */
        /*=================================================================== */

		SaSelectionObjectT selObj;
        	SaImmOiHandleT immOiHandle;
		SaAisErrorT registerOiImplementer(void);
		void Oifinalize();
		bool setExitCodeforcsadm(int errorid, int errortextid);
		int GetNodeId();

		int svc();
		int close(u_long);
		ACE_HANDLE get_handle(void) const;
		int handle_input(ACE_HANDLE fd);
		int handle_close(ACE_HANDLE, ACE_Reactor_Mask /*mask*/);
		int open(asecHAClass*);
		int close();
};

	static const SaImmOiCallbacksT_2 callbacks={
               ACS_ASEC_ObjImpl::saImmOiAdminOperationCallback,
               NULL,
               NULL,
               NULL,
               NULL,
               NULL,
               NULL,
               NULL
        };

#endif /* end ACS_ASEC_OBJECT_IMPLEMENTER_H*/

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

