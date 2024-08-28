/*=================================================================== */
/**
   @file   acs_chb_event_manager.h

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
   N/A       22/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_EVENT_MANAGER_H 
#define ACS_CHB_EVENT_MANAGER_H

/*=====================================================================
					FORWARD DECLARATION SECTION
==================================================================== */
class ACS_CHB_requestHandler;
class ACS_CHB_CPObjectList;
class ACS_CHB_clientHandler;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_eventManager


*/
/*=================================================================== */


class ACS_CHB_eventManager
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
	      @brief     Constructor for ACS_CHB_eventManager class.

		             Default constructor, used to initialize variables.


	*/
	/*=================================================================== */
  ACS_CHB_eventManager()
  {
	  requestFromClient=0;
	  serverName[40]='\0';
	  portNumber=0;
  };

  /*=====================================================================
  	                        CLASS DESTRUCTOR
  ==================================================================== */
  /*=================================================================== */
  /**
        @brief     Destructor for ACS_CHB_eventManager class.

  				 The destructor of the class.

  */
  /*=================================================================== */
  ~ACS_CHB_eventManager(){};

  /*=================================================================== */
  /**
        @brief     Split incomimg CP object message into item
				   and value parts.

        @param     CPObjectMessage
				   CP object message to decode.

        @param     CPObjectItem
				   Item part of message;

	@param     CPObjectValue
				   Value part of message;

        @return    ACS_CHB_OK is returned if conversion of message was successful,
				   ACS_CHB_FLASE with additional information from method getError...

   */
  /*=================================================================== */
  int decodeRequestFromHeartBeat(unsigned char CPObjectMessage[],
			         char CPObjectItem[],
				 char CPObjectValue[]
				);

  /*=================================================================== */
    /**
          @brief     Update corresponding CP object list item with new item value.

          @param     ptrCPObjectList
  				     Pointer to the head of the CP Object list.

 	  @param     CPObjectItem
  				     Item part of message;

  	  @param     CPObjectValue
  				     Value part of message;

          @return    void

     */
    /*=================================================================== */
  void updateCPObjectItem(ACS_CHB_CPObjectList *ptrCPObjectList,
			  char CPObjectItem[],
			  char CPObjectValue[]
			 );

  /*=====================================================================
  		                        PUBLIC DECLARATION SECTION
  	==================================================================== */

public:
  /*===================================================================
                            PUBLIC ATTRIBUTE
    =================================================================== */
    /*=================================================================== */
    /**
          @brief   pointer to internal object

     */
    /*=================================================================== */

        ACS_CHB_clientHandler *requestFromClient;

    /*=====================================================================
                            PRIVATE DECLARATION SECTION
    ==================================================================== */
protected:
    /*===================================================================
                             PUBLIC ATTRIBUTE
    =================================================================== */
    /*=================================================================== */
    /**
          @brief   serverName
     */
    /*=================================================================== */
	char serverName[40];

	/*=================================================================== */
	/**
	      @brief   portNumber
	 */
	/*=================================================================== */
	int portNumber;
};

#endif
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_DEBUG_H
#define ACS_CHB_DEBUG_H
/*=====================================================================
                        UNDEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  DEBUG_DECLARE

 */
/*=================================================================== */
#undef  DEBUG_DECLARE
/*=================================================================== */
/**
   @brief  DEBUG_INIT

 */
/*=================================================================== */
#undef  DEBUG_INIT
/*=================================================================== */
/**
   @brief  DEBUG_CLOSE

 */
/*=================================================================== */
#undef  DEBUG_CLOSE
/*=================================================================== */
/**
   @brief  DEBUG_MSG

 */
/*=================================================================== */
#undef  DEBUG_MSG
 
 
#ifdef CHB_DEBUG
 
// In debug version, compile DEBUG_MSG messages into calls to function
 
#include  <stdarg.h>
#include  <stdlib.h>
 
#define   DEBUG_DECLARE                                         \
                static FILE *fpDebugLog;                        \
                void log(const char *format, ...) {             \
                    va_list ap;                                 \
                    char    szMessage[1024];                    \
                    if (fpDebugLog == 0) return;                \
                    va_start(ap, format);                       \
                    vsprintf(szMessage, format, ap);            \
                    va_end(ap);                                 \
                    fprintf(fpDebugLog, szMessage);             \
                    fflush(fpDebugLog);                         \
                    }
 
#define   DEBUG_INIT(envVarName)                                \
                {                                               \
                char *pszEnv = getenv(envVarName);              \
                if (pszEnv && *pszEnv) {                        \
                    if (*pszEnv == 'd' || *pszEnv == 'D')       \
                        fpDebugLog = fdopen(1, "w");            \
                    else fpDebugLog = fopen(pszEnv, "w");       \
                    }                                           \
                }
 
#define   DEBUG_CLOSE                                           \
                if (fpDebugLog) fclose(fpDebugLog);
 
void      log(const char * format, ...);  // Write message to log file
 
#define   DEBUG_MSG(x)  x;
 
 
#else
 
// In a version without debug, exclude debug code from the compilation
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  DEBUG_DECLARE

 */
/*=================================================================== */
#define   DEBUG_DECLARE
/*=================================================================== */
/**
   @brief  DEBUG_INIT

 */
/*=================================================================== */
#define   DEBUG_INIT(envVarName)
/*=================================================================== */
/**
   @brief  DEBUG_CLOSE

 */
/*=================================================================== */
#define   DEBUG_CLOSE
/*=================================================================== */
/**
   @brief  DEBUG_MSG

 */
/*=================================================================== */
#define   DEBUG_MSG(x)
 
#endif
 
 
#endif

