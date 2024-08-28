/*=================================================================== */
/**
       @file          acs_aca_defs.h 

        @brief          Header file for ACA module.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       19/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_DEFS_H
#define ACS_ACA_DEFS_H
/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include<ace/ACE.h>
/*====================================================================
                                DEFINE DECLARATION SECTION
==================================================================== */
#define SERVICE_NAME       "ACS_ACA_Server"
#define SERVICE_TITLE      "ACS_ACA_Server"

#define CP_JTP_SERVICE              "ACACLI"
/*====================================================================
                                CONSTANT DECLARATION SECTION
==================================================================== */
const char * const
ACS_ACA_BindFailedText =  "Failed to bind listen socket";
const int ACS_ACA_BUFSIZE = 1024;
/*====================================================================
                                TYPEDEF DECLARATION SECTION
==================================================================== */
typedef const ACE_UINT32 ACS_ACA_SpecificProblem;
ACS_ACA_SpecificProblem ACS_ACA_BindFailedID =  2;
//Max cmd time out incase peers are not responsive for read/write
//licinst will take more time due to sentinel Initialization and clean up
typedef const void * LPCVIOD;
/*====================================================================
                                DEFINE DECLARATION SECTION
==================================================================== */
#define ERROR_NOT_ENOUGH_MEMORY 134;
#define MAX_WAIT_OBJECTS 64
#define WAIT_ABANDONED_0 0x00000080L
#define WAIT_OBJECT_0 0
#define WAIT_FAILED  0xFFFFFFFF
#define ACS_DSD_MAXBUFFER 65000

#endif
