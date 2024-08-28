//
//  NAME
//     ACS_LM_Common_Util.h
//
//  COPYRIGHT
//     Ericsson AB, 2015. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
//       
//  DOCUMENT NO
//          
//
//  AUTHOR 
//     2015-04-21 by XSUDYAG  PA1
//
//  SEE ALSO 
//     -
//
//****************************************************************************
#ifndef ACS_LM_Common_Util_H
#define ACS_LM_Common_Util_H
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <iomanip>
#include <ace/ACE.h>
#include <ACS_TRA_trace.h>
#include "acs_lm_defs.h"
#include "acs_lm_common.h"
#include <ACS_APGCC_CLibTypes.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_Util.H>

class ACS_LM_Common_Util
{
public:

     /**
     *  @param aClusterIPAddress                : string
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool getClusterIPAddress(std::string& aClusterIPAddress);
    /*=================================================================== */
    /**
     *  @param clusterIPv4				: string
     *  @param clusterIpv6				: string
     *  @return bool 					: true/false
     **/
    /*=================================================================== */
    static bool getClusterIPAddress_R2(std::string &clusterIPv4, std::string &clusterIPv6);
    /**
     *  @param aFingerPrint             : string
     *  @param theOmHandlerPtr          : OmHandler
     *  @return bool : true/false
     **/
    /*=================================================================== */
    static bool generateFingerPrint(string& aFingerPrint,OmHandler*  theOmHandlerPtr);
    /*=================================================================== */
    /**
         *  @param aFingerPrint             : string
         *  @param theOmHandlerPtr          : OmHandler
         *  @return bool : true/false
         **/
    /*=================================================================== */
    static bool generateFingerPrintForLicenseM(string& IPv4Fingerprint, string& IPv6Fingerprint,OmHandler*  theOmHandlerPtr);
    /*=================================================================== */
    /**
         * @brief ACS_LM_Common  :Default constructor
         *
         */
    /*=================================================================== */
    ACS_LM_Common_Util();
    /*=================================================================== */
    /**
    * @brief
    * destructor
    */
   /*=================================================================== */
   ~ACS_LM_Common_Util();
   /*=================================================================== */

private:
        

};

#endif

