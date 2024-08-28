//=============================================================================
/**
 *  @file    hwver.cpp
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *
 *  @author 2010-12-15 by XSONNAN
 *
 *  @documentno
 *
 *  @brief  It is used to get hardware version of APG.
 *
 */
//=============================================================================


/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */


#include <ACS_APGCC_CommonLib.h>
#include <ace/ACE.h>
#include <iostream>
using namespace std;

/*===================================================================
                        CONSTANT DECLARATION SECTION
=================================================================== */

#define HWVER_LENGTH 64


/*=================================================================== */
/**
   @brief           Gives hardware version of APG.

   ACE_TMAIN:       Main Method

                    It uses GetHwVersion API to retrieve the data from IMM database

   @return          int

   **/
/*=================================================================== */

ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR **argv) {

        ACS_APGCC_CommonLib obj;
        ACS_APGCC_HWINFO hwInfo;
        ACS_APGCC_HWINFO_RESULT hwInfoResult;
        (void) argv;

        if (argc > 1){
                cout<< "Usage:  hwver" << endl;
                return 2;
        }


        obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );

        int rCode=0;
        if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
                switch(hwInfo.hwVersion){
                        case ACS_APGCC_HWVER_GEP1:
                                cout << "AP HARDWARE VERSION : GEP1\n";
                                break;
                        case ACS_APGCC_HWVER_GEP2:
                                cout << "AP HARDWARE VERSION : GEP2\n";
                                break;
                        case ACS_APGCC_HWVER_GEP5:
                                cout << "AP HARDWARE VERSION : GEP5\n";
                                break;
                        case ACS_APGCC_HWVER_VM:
                                cout << "AP HARDWARE VERSION : VM\n";
                                break;
                        case ACS_APGCC_HWVER_GEP7:
                                cout << "AP HARDWARE VERSION : GEP7\n";
                                break;
                        default:
                                cout<< "Undefined hardware version" << endl;
                                rCode=3;
                                break;
                }
                return rCode;
        }
}



