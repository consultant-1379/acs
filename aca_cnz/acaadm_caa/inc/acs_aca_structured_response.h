/*=================================================================== */
/**
        @file           acs_aca_structured_response.h

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the StructuredResponse class.

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
#ifndef __ACS_ACA_STRUCTUREDRESPONSE_H__
#define __ACS_ACA_STRUCTUREDRESPONSE_H__
/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acs_aca_msdls_api_struct.h"
/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class StructuredResponse {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

   @brief     Constructor for StructuredResponse class

=================================================================== */
	StructuredResponse ();
/*====================================================================
                                CLASS DESSTRUCTOR
==================================================================== */
	virtual ~StructuredResponse ();
/*===================================================================

   @brief      setError

   @param      error

   @return     void

=================================================================== */
	void setError (unsigned short error);
/*===================================================================

   @brief      setDataLength

   @param      data_length

   @return     void

=================================================================== */
	void setDataLength (unsigned int data_length);
/*===================================================================

   @brief      setData

   @param      data

   @return     void

=================================================================== */
	void setData (const void * data);
/*===================================================================

   @brief      setCpId

   @param      cpID

   @return     void

=================================================================== */
	void setCpId (unsigned short cpID = 0);
/*===================================================================

   @brief      setSeqNo

   @param      seq_no

   @return     void

=================================================================== */
	void setSeqNo (unsigned int seq_no);
/*===================================================================

   @brief      getDataLength

   @return     unsigned int

=================================================================== */
	unsigned int getDataLength ();

/*===================================================================

   @brief      getData

   @return     void*

=================================================================== */
	void * getData ();
/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */

private:
	aca_msdls_resp_t * msdls_resp;
};

#endif
