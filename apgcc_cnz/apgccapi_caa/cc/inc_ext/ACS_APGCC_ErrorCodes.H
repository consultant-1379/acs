/*
 * ACS_APGCC_ErrorCodes.H
 *
 *  Created on: Apr 2, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_ERRORCODES_H_
#define ACS_APGCC_ERRORCODES_H_


// All defined error codes must be given an unique value, and
// the corresponding explaination text must be inserted in the
// file $ACS_APGCC/ACS_APGCC_Util.cpp. Make sure to update the No_Of_ErrorTexts
// counter also.
// N.B. codes 0 to 15 are reserved for command fault codes.


typedef enum {
   EC_OK                          =   0,
   EC_GENERALFAULT                =   1,
   EC_INCORRECTUSAGE              =   2,

   EC_APISNOTDEFINED              = 113,
   EC_ILLEGALCOMMAND              = 115,
   EC_ILLEGALOPTION               = 116,
   EC_CANNOTCONNECTTOSERVER       = 117,
   EC_CPISNOTDEFINED              = 118,
   EC_PARTLYEXECUTED              = 119,
   EC_ILLEGALCOMBINATIONOFOPTIONS = 120, // "Illegal combination of options"
   EC_UNREASONABLEVALUE           = 121, // "Unreasonable value"
   EC_ILLEGALOPTIONINCONF         = 122, // "Illegal option in this system configuration"
} ACS_APGCC_ErrorCode;


#endif /* ACS_APGCC_ERRORCODES_H_ */
