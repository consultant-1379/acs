/*============================================================== */
/**
   @file   acs_asec_runtimeObject.h

   @brief  


   @version 1.0.0
*/
/*
   HISTORY




   PR           DATE      INITIALS    DESCRIPTION
   -------------------------------------------------------------
  N/A       03/03/2015    XFURULL   AXE IO, CBC cipher handling in csadm implementation
  N/A       DD/MM/YYYY     NS       Initial Release  
   ============================================================= */

/*==============================================================
                          DIRECTIVE DECLARATION SECTION
================================================================ */

#ifndef ACS_ASEC_RUNTIMEOBJECT_H
#define ACS_ASEC_RUNTIMEOBJECT_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "fstream"
#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Task.h"
#include "saAis.h"
//#include "ACS_APGCC_RuntimeOwner.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_objectimplementerinterface_V2.h"
#include "ACS_TRA_Logging.h"

#define LINE_SIZE 200

/*===============================================================
                       CLASS DECLARATION SECTION
================================================================ */

/*===================================================================*/
/**   @class ACS_ASEC_RuntimeHandler acs_asec_runtimeObject.h
       *       @brief ACS_ASEC_RuntimeHandler class
        *       @author xsonnan
        *       @date 2011-02-25
        *       @version
        *
        *      ACS_ASEC_csadm class is used as the object manager class
        */
/*=================================================================== */


class ACS_ASEC_RuntimeHandler {


 /*===================================================================
                        PRIVATE DECLARATION SECTION
  =================================================================== */

private:

		void getPorts(string service,string &portnum);

	        char* theASECInfoClassName;
                char* theASECInfoParentName;
		char FTP_STATE[LINE_SIZE];
		char TELNET_STATE[LINE_SIZE];
		char MTS_STATE[LINE_SIZE];
		char CBC_STATE[LINE_SIZE];
		vector<string> public_ipaddr_vector;
		vector<string> public_v6_ipaddr_vector;
		vector<string> ps_ipaddr_vector;
		vector<string> vlan_ipaddr_vector;
		vector<string> vlan_mipaddr_vector;
		ACS_TRA_Logging log;
		string public_eth_interface;
		string ps_eth_interface;
		
		int populateInterface();		
		int launchCmd(string cmd, string& result);
                
 /*===================================================================
                        PUBLIC DECLARATION SECTION
=================================================================== */
	
public:

 	/*=====================================================================
                        CLASS CONSTRUCTORS
        ==================================================================== */
        /*=================================================================== */
        /**

                @brief          ACS_ASEC_RuntimeHandler class constructor

                                It is an empty constructor

                @return          void

                @exception       none
        */
        /*=================================================================== */


        ACS_ASEC_RuntimeHandler(); 

	 /*===================================================================
                        CLASS DESTRUCTOR
        =================================================================== */
        /*=================================================================== */
        /**

                 @brief         ACS_ASEC_RuntimeHandler class destructor

                                It is an empty destructor


                 @return          void

                 @exception       none
        */
        /*=================================================================== */

        ~ACS_ASEC_RuntimeHandler();

	 /*===================================================================
                           PUBLIC METHOD
        =================================================================== */
        /*=================================================================== */
        /**
                 @brief  createRuntimeObjectsforASECCONFIGCLASS() method

                        It is the method used for creating the runtime objects.

		@param protocol 
			It takes the protocol value 

		@param state
			It takes the state of the protocol

                @return      bool 

                @exception   none
        */
        /*=================================================================== */

        bool createRuntimeObjectsforASECCONFIGCLASS(string,string); 

	 /*=================================================================== */
        /**
                 @brief  isObjectExistsASECCONFIGCLASS() method

                        It is the method used for creating the runtime objects.

                @param object dn 
                        It takes the dn name of the object 

                @param myobjectavailable
                        It gives the availability status

                @return      bool

                @exception   none
        */
        /*=================================================================== */

	bool isObjectExistsASECCONFIGCLASS(string &,bool &);

	 /*=================================================================== */
        /**
                 @brief  deleteRuntimeObject() method

                        It is the method used for deleting  the runtime objects.

                @param object dn
                        It takes the dn name of the object


                @return      bool

                @exception   none
        */
        /*=================================================================== */


	bool deleteRuntimeObject(const char*);

	 /*=================================================================== */
        /**
                 @brief  modifyRuntimeAttribute() method

                        It is the method used for modifying the attributes of the runtime objects.

                @param object dn
                        It takes the dn name of the object

		@param attribute
			It takes the attribute name that needs to be modified

		@param value
			It takes the changed value

                @return      bool

                @exception   none
        */
        /*=================================================================== */


	bool modifyRuntimeAttribute(char* aObjectDN , char* attribute,char* value);
	
	  /*=================================================================== */
        /**
                 @brief  immstatus() method

                        It is the method used for getting the status of the imm.

                @return     void 

                @exception   none
        */
        /*=================================================================== */

	
	void immstatus( int adminOpid, fstream &);
	
	/*=================================================================== */
        /**
                 @brief  GetParams() method

                        It is the method used for getting the parameter.
		
		@param dn name

		@param parameter value

                @return     bool

                @exception   none
        */
        /*=================================================================== */


	
	bool GetParams(string,char*);


	 /*=================================================================== */
        /**
                 @brief  printadminOpid_4() method

                        It is the method used for printing the status of the protocols.


                @return     void

                @exception   none
        */
        /*=================================================================== */

	
	void printadminOpid_4(fstream &);
	void printadminOpid_1(fstream &);


	 /*=================================================================== */
        /**
                 @brief  immupdate() method

                        It is the method used for updating the imm.


                @return     void

                @exception   none
        */
        /*=================================================================== */


	void immupdate();

	int checkPS();

	//inline ACS_CC_ReturnType updateCallback(const char*, const char*){return ACS_CC_SUCCESS; };

         /*=================================================================== */
        /**
                 @brief  check_teaming_status() method

                        It is the method used for checking if RE already defined.


                @return     int - -1 = failed to get status, 0 = not configured, 1 = already defined

                @exception   none
        */
        /*=================================================================== */

        int checkTeamingStatus();

				int apgOamAccess();

				int isvAPG();

};
#endif
