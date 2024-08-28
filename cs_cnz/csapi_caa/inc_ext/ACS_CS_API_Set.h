//========================================================================================
// 
// NAME
//      ACS_CS_API_Set.H - Set API towards the CS functionality
//
// COPYRIGHT
//      Ericsson AB 2007 - All Rights Reserved.
//
//      The Copyright to the computer program(s) herein is the  
//      property of Ericsson AB, Sweden. The program(s) may be
//      used and/or copied only with the written permission from
//      Ericsson AB or in accordance with the terms and conditions 
//      stipulated in the agreement/contract under which the 
//      program(s) have been supplied.
//
// DESCRIPTION 
//      This header file contains the public set interface to CS. 

//
// ERROR HANDLING
//      Errors are indicated by the return value of the functions in the API
//
// DOCUMENT NO
//      
//
// AUTHOR 
//      2007-02-21 by EAB/AZA/TA XTBJEIV
//
// REVISION
//      R1A 
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//      REV NO  DATE     NAME            DESCRIPTION
//      R1A     070221 XTBJEIV      First version
//
// LINKAGE
//      libACS_CS_R1A_DMDN71.lib
//
// SEE ALSO 
//      -
//******************************************************************************

#ifndef ACS_CS_API_Set_H
#define ACS_CS_API_Set_H

//#include "ACS_CS_API_Set_R1.h"
#include "ACS_CS_API_Set_R3.h"
//#include "ACS_CS_API_QuorumData_R1.h"
#include "ACS_CS_API_CpData_R1.h"
//#include "ACS_CS_API_Common_R1.h"
//#include "ACS_CS_API_Set_Implementation.h"

//typedef ACS_CS_API_Set_R1 ACS_CS_API_Set;
typedef ACS_CS_API_Set_R3 ACS_CS_API_Set;

typedef ACS_CS_API_QuorumData_R1 ACS_CS_API_QuorumData;
typedef ACS_CS_API_CpData_R1 ACS_CS_API_CpData;

#endif //ACS_CS_API_Set_H

