/*=================================================================== */
/**
        @file           acs_aca_msg_builder.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACS_ACA_MSGBuilder class.

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
#ifndef __ACS_ACA_MSGBUILDER_H__
#define __ACS_ACA_MSGBUILDER_H__
/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACS_ACA_MSGBuilder {
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
	ACS_ACA_MSGBuilder ();
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_MSGBuilder ();

	// to build MSDLS request
/*===================================================================

   @brief    setOptions

   @param    optmask

   @return   void

=================================================================== */
	virtual void setOptions (unsigned short optmask) = 0;
/*===================================================================

   @brief    setCPSource

   @param    cpsource

   @return   void

=================================================================== */
	virtual void setCPSource (const char * cpsource) = 0;
/*===================================================================

   @brief    setMSGStoreName

   @param    msname

   @return   void

=================================================================== */
	virtual void setMSGStoreName (const char * msname) = 0;

	// to build MSDLS response
/*===================================================================

   @brief    setError

   @param    error

   @return   void

=================================================================== */
	virtual void setError (unsigned short error) = 0;
/*===================================================================

   @brief    setDataLength

   @param    data_lenght

   @return   void

=================================================================== */
	virtual void setDataLength (unsigned int data_lenght) = 0;
/*===================================================================

   @brief    setData

   @param    data

   @return   void

=================================================================== */
	virtual void setData (const void * data) = 0;
/*===================================================================

   @brief    setCpId

   @param    cpID

   @return   void

=================================================================== */

	virtual void setCpId (unsigned short cpID = 0) = 0;
/*===================================================================

   @brief    setSeqNo

   @param    seq_no

   @return   void

=================================================================== */
	virtual void setSeqNo (unsigned int seq_no) = 0;
/*===================================================================

   @brief    setSize
               to set msg size

   @param    &size

   @return   void

=================================================================== */
	virtual void setSize (unsigned int & size) = 0;
/*===================================================================

   @brief    getMessage
               to get request or response built

   @return   void

=================================================================== */
	virtual void * getMessage () = 0;
};
#endif
