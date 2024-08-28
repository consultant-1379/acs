/*=================================================================== */
/**
   @file   acs_chb_cp_object_implementer.h

   @brief Header file for CHB module.

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
   N/A       10/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_CPOBJECT_IMPLEMENTER_H 
#define ACS_CHB_CPOBJECT_IMPLEMENTER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ace/ACE.h>
#include <acs_chb_cp_object.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_TRA_trace.h>


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_CP_object

 */
/*=================================================================== */


class ACS_CHB_CP_object_Implementer: public virtual ACS_CHB_Error
{
	/*=====================================================================
		                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
								 CLASS CONSTRUCTOR
	 ==================================================================== */
	/*=================================================================== */
		/**
		 @brief     Constructor for ACS_CHB_CP_object class.

					The constructor of the class. Allocates private variables CPObjectName
				    and CPObjectValue

		 @param		Name
					An enum values which denotes type of
                    object
		 */
	/*=================================================================== */
  ACS_CHB_CP_object_Implementer(CP_object Name);

  /*=====================================================================
    	                        CLASS DESTRUCTOR
    ==================================================================== */
    /*=================================================================== */
    /**
          @brief     Destructor for ACS_CHB_CP_object class.

    				 Closes open sockets and deletes allocated

    */
    /*=================================================================== */
  virtual ~ACS_CHB_CP_object_Implementer();

  /*=================================================================== */
    /**
          @brief     connect
					 Connects and registers the client as a subscriber on
					 a CP object service.

          @param     ReTries
					 The number of retries in the connect
					 attempt.

          @param     TimeToWait
					 Timeout (in seconds) before next connect attempt.
          @return    ACS_CHB_returnType
     			     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType connect(int ReTries,int TimeToWait);

  /*=================================================================== */
    /**
          @brief     disconnect
					 Disconnect current session.

          @return    void

     */
    /*=================================================================== */
  void disconnect();

  /*=================================================================== */
    /**
          @brief     get_status
					 Get status for current session.

         @return    Disconnected    -     Connection broken
					 Connected       -     Connection working

     */
    /*=================================================================== */
  State get_status();

  /*=================================================================== */
    /**
          @brief     get_fileDescriptor
					 Get filedescriptor to current session.
					 The returned file-descriptor may only be used in poll or select
					 statements to test for received notifications and/or errors.
					 The file-descriptor may not be closed, or used in read or
					 write statments.

          @return    -1  -  Not connected
					 Socket    - Current file-descriptor

     */
    /*=================================================================== */
  int get_fileDescriptor();

  /*=================================================================== */
    /**
          @brief     get_name
					 Get name of current CP object. Possible values are:
					 "APZ_type", "EX_side", "SB_status", "CP_status", "CP_connection",
					 "EX_Exchange_identity", "Last_Restart_Value", "Time_Zone_Alarm".
					 The method returns a pointer to a local variable in the object.
				     The caller is not allowed to change the contents of that variable.

          @return    char* -  Pointer to current CP object name.

     */
    /*=================================================================== */
  char *get_name();

  /*=================================================================== */
    /**
          @brief     get_value
					 Get current CP object value.
					 The method returns a pointer to a local variable in the object.
					 The caller is not allowed to change the contents of that variable.
					 The value may be changed by the next call of get_value.

          @return    0 - No value
					 char* - Pointer to current CP object value.

     */
    /*=================================================================== */
  char *get_value();


public:
  /*===================================================================
                            PUBLIC ATTRIBUTE
     =================================================================== */
    /*=================================================================== */
    /**
          @brief   statusOfConnection

     */
    /*=================================================================== */
  State statusOfConnection;

private:
  /*=================================================================== */
    /**
          @brief     connectToServer

          @param     server_addr
                     Buffer that holds communication related data.

	  @param     ReTries
		     The number of retries in the connection attempt.

	  @param     TimeToWait
		     Timeout period (in seconds) before the next attempt.

          @return    ACS_CHB_returnType
      		     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType connectToServer(struct sockaddr_in *server_addr,
				     int ReTries,
				     int TimeToWait);

  /*=================================================================== */
    /**
          @brief     registerClient
					 Register client as subscriber to the selected CP object item.

          @param     cpObjectName
					 The name of the CP object to subscribe on.

          @return    ACS_CHB_returnType
      			     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType registerClient(char cpObjectName[]);

  /*=================================================================== */
    /**
          @brief     getCPObjectNameAsString
					 Convert the given CP object name to string format for internal
					 purposes.

          @param     Name
					 The name of the CP object to subscribe on.

          @return    Pointer to CPObjectName

     */
    /*=================================================================== */
  char *getCPObjectNameAsString(CP_object Name);

  /*=================================================================== */
    /**
          @brief     readCPObjectValueFromServer
					 Read CP object value from server.

          @param     cpObjectValue

          @return    ACS_CHB_returnType
      			     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType readCPObjectValueFromServer(char cpObjectValue[]);

  /*=================================================================== */
    /**
          @brief     requestCPObjectValueFromServer
					 Read CP object value from server.

          @param     cpObjectValue

          @return    ACS_CHB_returnType
      			     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType requestCPObjectValueFromServer(char cpObjectValue[]);

  /*=================================================================== */
    /**
          @brief     checkExchangeFile

          @param     cpObjectValue

          @return    ACS_CHB_returnType
      			     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType checkExchangeFile(char cpObjectValue[]);



private:
  /*===================================================================
						PRIVATE ATTRIBUTE
 =================================================================== */
/*=================================================================== */
/**
	  @brief   validObjectName

 */
/*=================================================================== */
  CP_object validObjectName; // Never used
  /*=================================================================== */
  /**
		@brief   Socket

   */
  /*=================================================================== */
  ACE_HANDLE Socket;
  /*=================================================================== */
  /**
		@brief   CPObjectName

   */
  /*=================================================================== */
  char *CPObjectName;
  /*=================================================================== */
  /**
		@brief   CPObjectValue

   */
  /*=================================================================== */
  char *CPObjectValue;
  /*=================================================================== */
  /**
		@brief   Server

   */
  /*=================================================================== */
  char *Server;              // Never used
  /*=================================================================== */
  /**
		@brief   Port

   */
  /*=================================================================== */
  int Port;                  // Never used
};
#endif


