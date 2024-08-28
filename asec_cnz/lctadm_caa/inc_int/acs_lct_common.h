//======================================================================
//
// NAME
//      acs_hardening_common.h
//
// COPYRIGHT
//      Ericsson AB 2005 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with 
//      the written permission from Ericsson AB or in 
//      accordance with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
//      Server process for surveillance of the external network.
//      Based on 190 89-CAA 109 0500.
//
// DOCUMENT NO
//      190 89-CAA 109 0562
//
// AUTHOR
//      2004-06-28 by EAB/UZ/D
//CHANGES
//  A  060908  XCSNAKA  To implement the TR HG26029 (refer the IP:EAB/APZ-05:0752) 
//
//======================================================================


#if !defined(_ACS_LCT_Common_H_)
#define _ACS_LCT_Common_H_


#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <syslog.h>


#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>

#include "acs_apgcc_omhandler.h"

// Definitions
using namespace std;


//=============================================================================
//  Common routines
//=============================================================================



/*=================================================================== */
/**
    @class              ACS_NSF_Common

    @brief              This class implements the generic functions required for NSF.
            Also this class implements the call backs for HA. 
**/
 /*=================================================================== */


class ACS_LCT_Common
{


public :

/*=============================================================================
            PUBLIC DECLARATION SECTION
  =============================================================================*/


ACE_UINT32 setValue( string option, string attr, string value) ;
ACE_UINT32 setDefaultValue( string attr ) ;

/*=================================================================== 

    @brief                  Common Class constructor: Initializes all the data memebers used.
    @pre
    @post
    @return                 none
    @exception              none

  =================================================================== */

   ACS_LCT_Common() ;

/*=====================================================================
                                CLASS DESTRUCTORS
  ==================================================================== */

/*===================================================================

    @brief                  Common Class destructors
    @pre
    @post
    @return                 none
    @exception              none

  =================================================================== */

   ~ACS_LCT_Common() ;

	string getPublicdnName() ;

private:
	string dnObject;
	ACE_UINT32 default_maxPswdAge;
	ACE_UINT32 default_minPswdAge;
	ACE_UINT32 default_maxPswdLen ;
	ACE_UINT32 default_pswdHistSize;
	ACE_UINT32 default_auditKernel;
	ACE_UINT32 default_auditUserLevel;
	ACE_UINT32 default_auditSystemDaemons;
	ACE_UINT32 default_auditAuthSecurity;
	ACE_UINT32 default_auditSyslog ;
	ACE_UINT32 default_auditCron ;
	ACE_UINT32 default_auditLocal;
	ACE_UINT32 default_auditFTP;
	ACE_UINT32 default_auditSSH;
	ACE_UINT32 default_auditTelnet;
	ACE_UINT32 default_lockoutDuration;
	ACE_UINT32 default_lockoutBadCount;
	ACE_UINT32 default_inactivityTimer;
	string welcomeMessage;
};


#endif
