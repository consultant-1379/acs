//========================================================================================
// 
// NAME
//		ACS_CS_API.H - API towards the CS functionality
//
// COPYRIGHT
//		 Ericsson AB 2007 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the	
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions 
//		stipulated in the agreement/contract under which the 
//		program(s) have been supplied.
//
// DESCRIPTION 
//		This header file contains the public interface to CS. 

//
// ERROR HANDLING
//		Errors are indicated by the return value of the functions in the API
//
// DOCUMENT NO
//		
//
// AUTHOR 
//		2006-12-18 by EAB/AZA/TA XTBJEIV
//
// REVISION
//		R1A
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//		REV NO	DATE 	 NAME		     DESCRIPTION
//		R1A		061218 XTBJEIV      First version for APG43 on Linuix
//
// LINKAGE
//		libACS_CSAPI.so
//
// SEE ALSO 
//		-
//******************************************************************************
#ifndef ACS_CS_API_H
#define ACS_CS_API_H

#include "ACS_CS_API_R3.h"
#include "ACS_CS_API_FunctionDistribution_R1.h"
#include "ACS_CS_API_CP_R1.h"
#include "ACS_CS_API_CPGroup_R1.h"
#include "ACS_CS_API_HWC_R2.h"
#include "ACS_CS_API_NetworkElement_R1.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_QuorumData_R1.h"
#include "ACS_CS_API_SubscriptionMgr_R2.h"
#include "ACS_CS_API_CpTableObserver_R2.h"
#include "ACS_CS_API_NetworkElementObserver_R1.h"
#include "ACS_CS_API_OgObserver_R1.h"
#include "ACS_CS_API_OmProfilePhaseObserver_R1.h"
#include "ACS_CS_API_ClientObserver.h"
#include "ACS_CS_API_CpTableChange_R2.h"
#include "ACS_CS_API_NetworkElementChange_R1.h"
#include "ACS_CS_API_OgChange_R1.h"
#include "ACS_CS_API_OmProfileChange_R1.h"
#include "ACS_CS_API_HWCTableChange_R1.h"
#include "ACS_CS_API_NWT_R1.h"
#include "ACS_CS_API_NetworkTable_R1.h"

typedef ACS_CS_API_R3 ACS_CS_API;
typedef ACS_CS_API_Name_R1 ACS_CS_API_Name;
typedef ACS_CS_API_IdList_R1 ACS_CS_API_IdList;
typedef ACS_CS_API_NameList_R1 ACS_CS_API_NameList;
typedef ACS_CS_API_MacList_R1 ACS_CS_API_MacList;
typedef ACS_CS_API_CP_R1 ACS_CS_API_CP;
typedef ACS_CS_API_CPGroup_R1 ACS_CS_API_CPGroup;
typedef ACS_CS_API_FunctionDistribution_R1 ACS_CS_API_FunctionDistribution;
typedef ACS_CS_API_HWC_R2 ACS_CS_API_HWC;
typedef ACS_CS_API_NetworkElement_R1 ACS_CS_API_NetworkElement;
typedef ACS_CS_API_BoardSearch_R2 ACS_CS_API_BoardSearch;
typedef ACS_CS_API_QuorumData_R1 ACS_CS_API_QuorumData;
typedef ACS_CS_API_SubscriptionMgr_R2 ACS_CS_API_SubscriptionMgr;
typedef ACS_CS_API_CpTableObserver_R2 ACS_CS_API_CpTableObserver;
typedef ACS_CS_API_NetworkElementObserver_R1 ACS_CS_API_NetworkElementObserver;
typedef ACS_CS_API_OgObserver_R1 ACS_CS_API_OgObserver;
typedef ACS_CS_API_HWCTableObserver_R1 ACS_CS_API_HWCTableObserver;
typedef ACS_CS_API_OmProfilePhaseObserver_R1 ACS_CS_API_OmProfilePhaseObserver;
typedef ACS_CS_API_CpTableChange_R2 ACS_CS_API_CpTableChange;
typedef ACS_CS_API_CpTableData_R2 ACS_CS_API_CpTableData;
typedef ACS_CS_API_NetworkElementChange_R1 ACS_CS_API_NetworkElementChange;
typedef ACS_CS_API_OgChange_R1 ACS_CS_API_OgChange;
typedef ACS_CS_API_OmProfileChange_R1 ACS_CS_API_OmProfileChange;
typedef ACS_CS_API_HWCTableChange_R1 ACS_CS_API_HWCTableChange;
typedef ACS_CS_API_NWT_R1 ACS_CS_API_NWT;
typedef ACS_CS_API_NWT_BoardSearch_R1 ACS_CS_API_NWT_BoardSearch;
typedef ACS_CS_API_NetworkTable_R1 ACS_CS_API_NetworkTable;

#endif //ACS_CS_API_H

