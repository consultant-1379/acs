/*=================================================================== */
/**
        @file          acs_aca_msg_director.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACS_ACA_MSGDirector class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       09/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef __ACS_ACA_MSGDIRECTOR_H__
#define __ACS_ACA_MSGDIRECTOR_H__
/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <stdlib.h>

#include "acs_aca_msg_builder.h"
#include "acs_aca_msdls_api_struct.h"
/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACS_ACA_MSGDirector {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor ACS_ACA_MSGDirector class.

=================================================================== */
	ACS_ACA_MSGDirector ();
/*====================================================================
                                CLASS DESSTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_MSGDirector ();
/*===================================================================

        @brief          buildStructuredRequest

        @param          builder

        @param          &reqSize

        @param          optmask

        @param          cpsource

        @param          msname

        @param          data

        @param          cpID

        @return         void*    

=================================================================== */
	static void * buildStructuredRequest (
			ACS_ACA_MSGBuilder * builder,
			unsigned int & reqSize, /* [OUT] passed to send */
			unsigned short optmask,
			const char * cpsource = 0,
			const char * msname = 0,
			const char * data = 0,
			unsigned short cpID = 0);
/*===================================================================

        @brief          buildStructuredResponse

        @param          builder

        @param          &respSize

        @param          seq_no

        @param          error

        @param          data_length

        @param          data

        @param          cpID

        @return         void*

=================================================================== */
	static void * buildStructuredResponse (
			ACS_ACA_MSGBuilder * builder,
			unsigned int & respSize, /* [OUT] passed to send */
			unsigned int seq_no,
			unsigned short error,
			unsigned int data_length,
			const char * data = 0,
			unsigned short cpID = 0);
};

#endif
