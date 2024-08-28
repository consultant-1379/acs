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

#include "acs_apgcc_omhandler.h"
#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Task.h"
#include "ACS_APGCC_RuntimeOwner.h"
#include "saAis.h"

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


class ACS_ASEC_RuntimeHandler:public ACS_APGCC_RuntimeOwner {


 /*===================================================================
                        PRIVATE DECLARATION SECTION
  =================================================================== */

private:

		void getPorts(string service,string &portnum);

	        char* theASECInfoClassName;
                char* theASECInfoParentName;




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


        inline ACS_ASEC_RuntimeHandler(){ 
			theASECInfoClassName = const_cast<char *>("ASECConfigData");
			theASECInfoParentName = const_cast<char*>("acsSecurityMId=1");
	};

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

        ~ACS_ASEC_RuntimeHandler(){};

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

	
	void immstatus();
	
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
                 @brief  printstatus() method

                        It is the method used for printing the status of the protocols.


                @return     void

                @exception   none
        */
        /*=================================================================== */

	
	void printstatus();


	 /*=================================================================== */
        /**
                 @brief  immupdate() method

                        It is the method used for updating the imm.


                @return     void

                @exception   none
        */
        /*=================================================================== */


	void immupdate();
	
	inline ACS_CC_ReturnType updateCallback(const char*, const char*){return ACS_CC_SUCCESS; };

};
#endif
