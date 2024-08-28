/*=================================================================== */
/**
   @file   acs_chb_cp_object.h

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

#ifndef ACS_CHB_CPOBJECT_H
#define ACS_CHB_CPOBJECT_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_chb_error.h"

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ACS_CHB_returnType
 */
/*=================================================================== */
typedef long  ACS_CHB_returnType;
/*=================================================================== */
/**
   @brief StateEnum
 */
/*=================================================================== */
typedef enum State {
        NotValidObjectName = 0,
        Disconnected,
        Connected
}StateEnum;
/*=================================================================== */
/**
   @brief CP_objectEnum
 */
/*=================================================================== */
typedef enum CP_object {
        APZ_type,
        EX_side,
        SB_status,
        CP_status,
        CP_connection,
        EX_Exchange_identity,
        SB_Exchange_identity,
        Last_Restart_Value,
        Time_Zone_Alarm
}CP_objectEnum;

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ACS_CHB_TRUE
 */
/*=================================================================== */
const int ACS_CHB_TRUE  = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_FALSE
 */
/*=================================================================== */
const int ACS_CHB_FALSE = 0;
/*=================================================================== */
/**
   @brief ACS_CHB_OK
 */
/*=================================================================== */
const int ACS_CHB_OK    = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_FAIL
 */
/*=================================================================== */
const int ACS_CHB_FAIL  = 0;
/*=================================================================== */
/**
   @brief ACS_CHB_pollTimeout
                  Time period (in s) to wait for
          data indication from server.
 */
/*=================================================================== */
const int ACS_CHB_pollTimeout   = 1;

/*=================================================================== */
/**
   @brief ACS_CHB_BUFSIZE
                  Default size for CHB buffer variables.
 */
/*=================================================================== */
const int ACS_CHB_BUFSIZE = 1024;
/*=================================================================== */
/**
   @brief ACS_CHB_NUMBER_OF_TZ
 */
/*=================================================================== */
const int ACS_CHB_NUMBER_OF_TZ = 24;    // 0 <= TZ < 24
/*=================================================================== */
/**
   @brief ACS_CHB_serverPort
 */
/*=================================================================== */
const int ACS_CHB_serverPort = 12396;
                                                                                // Port for local communication between
                                                                                // ACS_CHB_HeartBeatChild.exe and
                                                                                // libacs_chb.dll
                                                                                //
                                                                                // May need to be changed if it
                                                                                // conflicts with an existing application.
//******************************************************************************
//      constant error types to be retreived by the inherited getError method.
//******************************************************************************
/*=================================================================== */
/**
   @brief ACS_CHB_communicationFailure
 */
/*=================================================================== */
const ACS_CHB_returnType   ACS_CHB_communicationFailure    = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_CPObjectAPISocketStartupFailedID
                  possible return codes from the getError
                  method.
 */
/*=================================================================== */
const ACS_CHB_returnType   ACS_CHB_CPObjectAPISocketStartupFailedID = 2;


class ACS_CHB_CP_object_Implementer;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_CP_object

 */
/*=================================================================== */
class ACS_CHB_CP_object: public virtual ACS_CHB_Error
{
  /*=====================================================================
   PUBLIC DECLARATION SECTION
   ==================================================================== */
public :
  /*=====================================================================
                         CLASS CONSTRUCTOR
   ==================================================================== */
  /*=================================================================== */
  /**
     @brief     Constructor for ACS_CHB_CP_object class.

                 The constructor of the class. Allocates private variables CPObjectName
                 and CPObjectValue

     @param         Name
                 An enum values which denotes type of
     object
   */
  /*=================================================================== */
      ACS_CHB_CP_object(CP_object Name);
   /*=====================================================================
                         CLASS DESTRUCTOR
   ==================================================================== */
  /*=================================================================== */
  /**
     @brief     Destructor for ACS_CHB_CP_object class.

                 Closes open sockets and deletes allocated

   */
  /*=================================================================== */
      ~ACS_CHB_CP_object();
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
      ACS_CHB_returnType connect(int ReTries, int TimeToWait);
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

private:
      /*=================================================================== */
      /**

              @brief          myCHB_cp_Object_Implementation
      */
      /*=================================================================== */
      ACS_CHB_CP_object_Implementer *myCHB_cp_Object_Implementation;

};
#endif
