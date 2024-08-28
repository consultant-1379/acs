/*=================================================================== */
/**
   @file   acs_aca_structured_request_builder.h 

   @brief  Header file for aca module.

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
#ifndef __ACS_ACA_STRUCTURED_REQUEST_BUILDER_H__
#define __ACS_ACA_STRUCTURED_REQUEST_BUILDER_H__
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aca_msg_builder.h"
#include "acs_aca_structured_request.h"


class ACS_ACA_MSDCMD_Stub;
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      ACS_ACA_StructuredRequestBuilder
 */
/*=================================================================== */
class ACS_ACA_StructuredRequestBuilder :
	public ACS_ACA_MSGBuilder
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
   @brief      Constructor for ACS_ACA_StructuredRequestBuilder class.
 */
/*=================================================================== */
	ACS_ACA_StructuredRequestBuilder(void);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_StructuredRequestBuilder(void);
/*=================================================================== */
/**
   @brief      setOptions

   @param      optmask

   @return     void
 */
/*=================================================================== */
	// to build MSDLS request
	void setOptions(unsigned short	optmask);
/*=================================================================== */
/**
   @brief      setCPSource

   @param      cpsource

   @return     void
 */
/*=================================================================== */
	void setCPSource(const char* cpsource);
/*=================================================================== */
/**
   @brief      setMSGStoreName

   @param      msname

   @return     void
 */
/*=================================================================== */
	void setMSGStoreName(const char* msname);
/*=================================================================== */
/**
   @brief      setData

   @param      data

   @return     void
 */
/*=================================================================== */
	void setData(const void* data);
/*=================================================================== */
/**
   @brief      setCpId

   @param      cpID

   @return     void
 */
/*=================================================================== */
	void setCpId(unsigned short cpID);
/*=================================================================== */
/**
   @brief      setError

   @param      error

   @return     void
 */
/*=================================================================== */
	// to build MSDLS response
	void setError(unsigned short error) { (void) error; };
/*=================================================================== */
/**
   @brief      setDataLength

   @param      data_lenght

   @return     void
 */
/*=================================================================== */
	void setDataLength(unsigned int data_lenght) { (void) data_lenght; };
/*=================================================================== */
/**
   @brief      setSeqNo

   @param      seq_no

   @return     void
 */
/*=================================================================== */
	void setSeqNo(unsigned int seq_no) { (void) seq_no; };
/*=================================================================== */
/**
   @brief      setSize

   @param      size

   @return     void
 */
/*=================================================================== */
	void setSize(unsigned int &size);
/*=================================================================== */
/**
   @brief      getMessage

   @return     void
 */
/*=================================================================== */

	// to get request or response built
	void* getMessage();
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
private:
	StructuredRequest* msg;
};

#endif
