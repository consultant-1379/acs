/*=================================================================== */
/**
        @file          acs_aca_msd_mtap_message.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the MCS_ALD_AldaAdm class.

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
#ifndef ACAMSD_MTAP_Message_H
#define ACAMSD_MTAP_Message_H
/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
//#include "ACAMSD_MTAP_Protocol.H"
#include "acs_aca_msd_mtap_protocol.h"
#include "acs_aca_defs.h"
#include "acs_aca_common.h"
/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MTAP_Message {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for  ACAMSD_MTAP_Message class.

        @param          channel

        @param          buffer

        @param          length

        @param          version

        @param          primitive
 

=================================================================== */
	ACAMSD_MTAP_Message (
			unsigned channel,
			const unsigned char * buffer,
			unsigned long length,
			unsigned char version,
			ACAMSD_MTAP_Protocol::Primitive primitive);
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMSD_MTAP_Message ();
/*===================================================================

        @brief          getChannel

        @return         unsigned

=================================================================== */
	inline unsigned getChannel () const { return myChannel; }
/*===================================================================

        @brief          putChannel

        @return         void

=================================================================== */
	inline void putChannel (unsigned channel) { myChannel = channel; }
/*===================================================================

        @brief          getMtapFileNumber

        @return         unsigned char

=================================================================== */
	unsigned char getMtapFileNumber () const;
/*===================================================================

        @brief          getMtapNumber

        @return         unsigned long

=================================================================== */
	unsigned long getMtapNumber () const { return myMtapNum; } // MTAP message number
/*===================================================================

        @brief          getMsgNumber

        @return         ULONGLONG

=================================================================== */
	ULONGLONG getMsgNumber () const { return myMsgNum; }
/*===================================================================

        @brief          putMsgNumber

        @param          msgNum

        @return         void

=================================================================== */
	void putMsgNumber (ULONGLONG msgNum);
/*===================================================================

        @brief          getDataLength

        @return         unsigned int

=================================================================== */
	unsigned int getDataLength () const;
/*===================================================================

        @brief          getData

        @return         unsigned char

=================================================================== */

	const unsigned char * getData () const;       // The file format
/*===================================================================

        @brief          getResponse
                         Build an MTAP put-record-response primitive

        @param          buffer

        @param          result

        @return         int

=================================================================== */
	int getResponse (unsigned char * & buffer, ACAMSD_MTAP_Protocol::ErrorCode result = ACAMSD_MTAP_Protocol::NoError);
/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACAMSD_MTAP_Message class.


=================================================================== */
	ACAMSD_MTAP_Message ();
/*===================================================================

        @brief          Constructor for ACAMSD_MTAP_Message class.

        @param          anotherMsg


=================================================================== */
	ACAMSD_MTAP_Message (const ACAMSD_MTAP_Message & anotherMsg);

	const ACAMSD_MTAP_Message & operator= (const ACAMSD_MTAP_Message & anotherMsg);
/*=====================================================================
                                DATA DECLARATION SECTION
==================================================================== */
/*===================================================================

        @brief          myChannel
                         MTAP channel used for this msg

=================================================================== */
	unsigned myChannel; // MTAP channel used for this msg
/*===================================================================

        @brief          myBufferSize
                         Size of own buffer

=================================================================== */
	unsigned int myBufferSize; // Size of own buffer
/*===================================================================

        @brief           myBuffer

=================================================================== */
	unsigned char * myBuffer;
/*===================================================================

        @brief           myMtapNum

=================================================================== */

	unsigned long myMtapNum;
/*===================================================================

        @brief           myMsgNum

=================================================================== */
	ULONGLONG myMsgNum;
};

#endif
