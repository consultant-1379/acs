//========================================================================================
// 
// NAME
//		ACS_CS_API_Internal.H
//
// COPYRIGHT
//		 Ericsson AB 2010 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the	
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions 
//		stipulated in the agreement/contract under which the 
//		program(s) have been supplied.
//
// DESCRIPTION 
//		This header file contains the internally used definitions

//		-
//******************************************************************************
#ifndef ACS_CS_API_Internal_H
#define ACS_CS_API_Internal_H

#include <set>
#include <map>
#include <string>

#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_Util_Internal.h"

typedef ACS_CS_API_Util_Internal ACS_CS_API_Util;

typedef std::set<CPID> cpidSet;
typedef std::map<std::string, cpidSet*> stringCPIDSetMap;

#endif //ACS_CS_API_Internal_H

