/*============================================================== */
/**
   @file   acs_asec_dsdserver.h

   @brief this is the dsd server file header


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

/*==============================================================
                          INCLUDE DECLARATION SECTION
================================================================ */

#ifndef ACS_ASEC_DSDSERVER_H
#define ACS_ASEC_DSDSERVER_H

#include <string>
#include <vector>
#include "ACS_DSD_Server.h"
#include "ACS_DSD_Client.h"
#include "ACS_APGCC_AmfTypes.h"
#include <unistd.h>
#include <syslog.h>
#include "ace/Task_T.h"
#include "ace/OS.h"
#include "ACS_TRA_Logging.h"
#include "acs_prc_api.h"
#include <ace/Monotonic_Time_Policy.h>

using namespace std;

#define NODE_ID_FILE "/etc/opensaf/slot_id"
#define RELATIVETIME(x) (ACE_OS::time(NULL) + x)

/*===============================================================
                       CLASS DECLARATION SECTION
================================================================= */
/*===================================================================*/
        /**   @class ACS_ASEC_dsdserver acs_asec_dsdserver.h
        *       @brief ACS_ASEC_dsdserver class
        *       @author xsonnan
        *       @date 2011-02-25
        *       @version
        *
        *      ACS_ASEC_dsdserver class is used as the dsd handler class
        */
/*=================================================================== */
class Global;

class ACS_ASEC_DSDSrv: public ACE_Task<ACE_SYNCH, ACE_Monotonic_Time_Policy>

{

/*==============================================================
               PRIVATE DECLARATION SECTION
================================================================ */
	private:
		ACS_TRA_Logging log;
		ACS_DSD_Session* Session_server;
		string serviceName;
		Global *m_global_instance;

/*==============================================================
               PUBLIC DECLARATION SECTION
================================================================ */

	public:

	/*=====================================================================
                        CLASS CONSTRUCTORS
        ==================================================================== */
        /*=================================================================== */
        /**

                @brief          ACS_ASEC_dsdserver class constructor


                @return          void

                @exception       none
        */
        /*=================================================================== */


		ACS_ASEC_DSDSrv();

	/*===================================================================
                        CLASS DESTRUCTOR
        =================================================================== */
        /*=================================================================== */
        /**

                 @brief         ACS_ASEC_dsdserver class destructor

                 @return          void

                 @exception       none
        */
        /*=================================================================== */


		~ACS_ASEC_DSDSrv();

	 /*===================================================================
                           PUBLIC ATTRIBUTES
         =================================================================== */

	  ACS_DSD_Server* dsdServObj;   //DSD
	  int shutDownDSD;

        /*===================================================================
                           PUBLIC METHOD
        =================================================================== */
        /*=================================================================== */
        /**
                 @brief  GetNodeId() method

                        It is the method used to find the node.

                @return      int

                @exception   none
        */
        /*=================================================================== */

	int GetNodeId();
	
	/*=================================================================== */
        /**
                 @brief  DSDServerfunc() method

                        It is the DSD server function

                @return      ACE_UINT32

                @exception   none
        */
        /*=================================================================== */
		int svc();
		int close(u_long);
		int open();
		int close();
		bool publish();
		bool accept();
                bool StopDSDServer();
		int launchCmd(string cmd, const char *mode);
};

#endif //end ACS_ASEC_DSDSERVER_H
