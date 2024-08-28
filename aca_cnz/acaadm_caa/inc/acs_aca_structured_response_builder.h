/*=================================================================== */
/**
        @file          acs_aca_structured_response_builder.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACS_ACA_StructuredResponseBuilder class.

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
#ifndef __ACS_ACA_STRUCTURED_RESPONSE_BUILDER_H__
#define __ACS_ACA_STRUCTURED_RESPONSE_BUILDER_H__

/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aca_msg_builder.h"
#include "acs_aca_structured_response.h"

/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACS_ACA_StructuredResponseBuilder : public ACS_ACA_MSGBuilder {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief           Constructor for ACS_ACA_StructuredResponseBuilder class.

=================================================================== */
	ACS_ACA_StructuredResponseBuilder ();
/*====================================================================
                                CLASS DESSTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_StructuredResponseBuilder ();
/*===================================================================

        @brief           setOptions
                           to build MSDLS request

        @param           optmask

        @return          void

=================================================================== */

	// to build MSDLS request
	inline void setOptions (unsigned short optmask) { optmask = 0; }
/*===================================================================

        @brief           setCPSource

        @param           cpsource

        @return          void

=================================================================== */
	inline void setCPSource (const char * cpsource) { cpsource = 0; }
/*===================================================================

        @brief           setMSGStoreName

        @param           msname

        @return          void

=================================================================== */

	inline void setMSGStoreName (const char * msname) { msname = 0; }

	// to build MSDLS response
/*===================================================================

        @brief           setError

        @param           error

=================================================================== */
	void setError (unsigned short error);
/*===================================================================

        @brief           setDataLength

        @param           data_lenght

=================================================================== */
	void setDataLength (unsigned int data_lenght);
/*===================================================================

        @brief           setData

        @param           data

=================================================================== */


	void setData (const void * data);
/*===================================================================

        @brief           setSeqNo 

        @param           seq_no

=================================================================== */
	void setSeqNo (unsigned int seq_no);
/*===================================================================

        @brief           setCpId

        @param           cpID

=================================================================== */
	void setCpId (unsigned short cpID = 0);
/*===================================================================

        @brief           setSize
                          set response message size

        @param           &size

=================================================================== */
	// set response message size
	void setSize (unsigned int & size);
/*===================================================================

        @brief        getMessage
                        to get request or response built 

=================================================================== */
	// to get request or response built
	void * getMessage ();
/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
	StructuredResponse * msg;
};
#endif
