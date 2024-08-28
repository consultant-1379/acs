/*============================================================== */
/**
   @file   acs_hardening_dsdserver.h

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

#ifndef ACS_LCT_DSDSERVER_H
#define ACS_LCT_DSDSERVER_H

#include <string>
#include <vector>
#include "ACS_DSD_Server.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"
#include "acs_lct_tra.h"

using namespace std;

#define NODE_ID_FILE "/etc/opensaf/slot_id"
#define MAX_BYTES 2048

/*===============================================================
                       CLASS DECLARATION SECTION
================================================================= */
/*===================================================================*/
        /**   @class ACS_HARDENING_dsdserver acs_hardening_dsdserver.h
        *       @brief ACS_HARDENING_dsdserver class
        *       @author xsonnan
        *       @date 2011-02-25
        *       @version
        *
        *      ACS_HARDENING_dsdserver class is used as the dsd handler class
        */
/*=================================================================== */
class ACS_LCT_dsdServer;

extern ACS_LCT_dsdServer * dsdObj;

class ACS_LCT_dsdServer

{

/*==============================================================
               PRIVATE DECLARATION SECTION
================================================================ */
	private:

/*==============================================================
               PUBLIC DECLARATION SECTION
================================================================ */

	public:

	/*=====================================================================
                        CLASS CONSTRUCTORS
        ==================================================================== */
        /*=================================================================== */
        /**

                @brief          ACS_HARDENING_dsdserver class constructor


                @return          void

                @exception       none
        */
        /*=================================================================== */


		ACS_LCT_dsdServer();

	/*===================================================================
                        CLASS DESTRUCTOR
        =================================================================== */
        /*=================================================================== */
        /**

                 @brief         ACS_HARDENING_dsdserver class destructor

                 @return          void

                 @exception       none
        */
        /*=================================================================== */


		~ACS_LCT_dsdServer();

	 /*===================================================================
                           PUBLIC ATTRIBUTES
         =================================================================== */

	  ACS_DSD_Server* dsdServObj;   //DSD
      ACS_DSD_Client* dsdCliObj;    //DSD
	  ACS_DSD_Session * Session_client;
	  ACS_DSD_Session * Session_server;
	  int flag;

        /*===================================================================
                           PUBLIC METHOD
        =================================================================== */
        /*=================================================================== */
        /**
                 @brief  FindNode() method

                        It is the method used to find the node.

                @return      int

                @exception   none
        */
        /*=================================================================== */

	  int FindNode();
	
	/*=================================================================== */
        /**
                 @brief  DSDServerfunc() method

                        It is for DSD server functionality

                @return      ACE_UINT32

                @exception   none
        */
        /*=================================================================== */

		bool DSDServerfunc();

/*=================================================================== */
        /**
                 @brief  DSDClientfunc() method

                        It is for DSD client functionality

                @return      ACE_UINT32

                @exception   none
        */
/*=================================================================== */

		bool DSDClientfunc(string, int &aErrCode, string &aErrMsg);

	/*=================================================================== */
        /**
                 @brief  StartDSDServer() method

                        It is the start DSD server

                @return      bool 

                @exception   none
        */
        /*=================================================================== */
		
		bool StartDSDServer();
	
	/*=================================================================== */
        /**
                 @brief  StartDSDClient() method

                        It is the start DSD client function

                @return      ACE_INT32

                @exception   none
        */
        /*=================================================================== */
		
		bool StartDSDClient();
	
	 /*=================================================================== */
        /**
                 @brief   CheckActiveNode() method

                        It is used to check if the node is active

                @return      bool

                @exception   none
        */
        /*=================================================================== */

		int CheckActiveNode();

		/*=================================================================== */
		        /**
		                 @brief   instantiate() method

		                        It is used to instantiate the server and client objects at the start

		                @return      ACE_INT32

		                @exception   none
		        */
		/*=================================================================== */
		bool instantiate();

		/*=================================================================== */
				        /**
				                 @brief   getFlag() method

				                        It is used to get value of flag variable

				                @return      ACE_INT32

				                @exception   none
				        */
		/*=================================================================== */
		//ACE_INT32 getFlag();

		/*=================================================================== */
				        /**
				                 @brief   setFlag() method

				                        It is used to set value of flag variable

				                @return      void

				                @exception   none
				        */
		/*=================================================================== */
		void setFlag(int a);
		//ACS_LCT_dsdServer * getHardeningInstance();

		/*=================================================================== */
				        /**
				                 @brief   modify_config_file() method

				                        It is used to modify configuration files

				                @return      ACE_INT32

				                @exception   none
				        */
		/*=================================================================== */
		//bool modify_config_file(string fin_name, string fin_val, int &aErrCode, string &aErrMsg);
		bool modify_config_file(char* cmd, int &aErrCode, string &aErrMsg);
		bool modify_config_file(string fin_name, string fin_val);
		bool rollbackNodeFileChanges();
		bool commitNodeFileChanges();
		bool localUpdate(string);
		int launchCommand(char *cmd_string);
		void closeDSDClient();
		void ShutdownDSD();
		//acs_dsd::SERVER_STATE_CLOSED server_state;
		//bool startup();
		
};

#endif //end ACS_ASEC_DSDSERVER_H
