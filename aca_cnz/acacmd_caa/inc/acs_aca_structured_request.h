/*=================================================================== */
/**
   @file   acs_aca_error.h

   @brief Header file for aca module.

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
   N/A       28/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef __ACS_ACA_STRUCTUREDREQUEST_H__
#define __ACS_ACA_STRUCTUREDREQUEST_H__
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <stdlib.h>
#include <string.h>
#include "acs_aca_msdls_api_struct.h"
#include <ACS_TRA_trace.h>
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
class StructuredRequest
{
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
=================================================================== */
/*=================================================================== */
/**
   @brief       Constructor for StructuredRequest class.

 */
/*=================================================================== */
	StructuredRequest(void);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~StructuredRequest(void);
/*=================================================================== */
/**
   @brief       setOptions

   @param       optmask

   @return      void

 */
/*=================================================================== */
	// set field
	void setOptions(unsigned short optmask);
/*=================================================================== */
/**
   @brief       setCPSource

   @param       cpsource

   @return      void

 */
/*=================================================================== */
	void setCPSource(const char* cpsource);
/*=================================================================== */
/**
   @brief       setMSGStoreName

   @param       msname

   @return      void

 */
/*=================================================================== */
	void setMSGStoreName(const char* msname);
/*=================================================================== */
/**
   @brief       setData

   @param       data

   @return      void

 */
/*=================================================================== */
	void setData(const void* data);
/*=================================================================== */
/**
   @brief       SetSize

   @param       ReqSize

   @return      void

 */
/*=================================================================== */
	void SetSize(unsigned int& ReqSize);
/*=================================================================== */
/**
   @brief       setCpId

   @param       cpID

   @return      void

 */
/*=================================================================== */
	void setCpId(unsigned short cpID = 0);
/*=================================================================== */
/**
   @brief       getData 

   @return      void

 */
/*=================================================================== */

	void* getData();
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
	aca_msdls_req_t* msdls_req;
};

#endif
