//==============================================================================
//
// NAME
//      ACS_PRC_API.h
//
//  COPYRIGHT Ericsson AB, Sweden 2003.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
// .DESCRIPTION 
// 	This program contains methods for
//	reboot node
//	switch node
//  stop groups and resources
//  start groups and resources
//	add/remove resources and groups
//	read node status
//
// DOCUMENT NO
//	190 89-CAA 109 0520 Ux
//
// AUTHOR 
// 	2003-05-14 by EAB/UKY/GB LMGR
//
// SEE ALSO 
// 	-
//
//==============================================================================

#ifndef ACS_PRC_API_IMPL_H
#define ACS_PRC_API_IMPL_H

#include <iostream>
#include <string.h>
#include <saImmOm.h>
#include <saClm.h>
#include <vector>
#include "acs_prc_types.h"
#include "ACS_TRA_Logging.h"

using namespace std;
// Reason for the reboot or stopResource or stopGroup

const int rebootLimit       = 0;	// Only one reboot / 30 min
const int cyclicRebootLimit = 1;	// After second reboot within 30 min=>manual

#define ACS_APGCC_NO_ERR "NO_ERROR"
#define ACS_APGCC_ERR_UNKNOW "ERR_UNKNOW"
#define ACS_APGCC_ERR_IMM_LIBRARY "ERR_IMM_LIBRARY"
#define ACS_APGCC_ERR_IMM_VERSION "ERR_IMM_VERSION"
#define ACS_APGCC_ERR_IMM_INIT "ERR_IMM_INIT"
#define ACS_APGCC_ERR_IMM_TIMEOUT "ERR_IMM_TIMEOUT"
#define ACS_APGCC_ERR_IMM_TRY_AGAIN "ERR_IMM_TRY_AGAIN"
#define ACS_APGCC_ERR_IMM_INVALID_PARAM "ERR_INVALID_PARAMETER"
#define ACS_APGCC_ERR_IMM_NO_MEMORY "ERR_IMM_NO_MEMORY"
#define ACS_APFCC_ERR_IMM_BAD_HANDLE "ERR_IMM_BAD_HANDLE"
#define ACS_APGCC_ERR_IMM_BUSY "ERR_IMM_BUSY"
#define ACS_APGCC_ERR_IMM_ACCESS "ERR_IMM_ACCESS"
#define ACS_APGCC_ERR_IMM_NOT_EXIST "ERR_ELEMENT_NOT_EXIST"
#define ACS_APGCC_ERR_IMM_NAME_TOO_LONG "ERR_NAME_TOO_LONG"
#define ACS_APGCC_ERR_IMM_EXIST "ERR_ELEMENT_ALREADY_EXIST"
#define ACS_APGCC_ERR_IMM_NO_SPACE "ERR_IMM_NO_SPACE"
#define ACS_APGCC_ERR_IMM_INTERRUPT "ERR_IMM_INTERRUPT"
#define ACS_APGCC_ERR_NAME_NOT_FOUND "ERR_NAME_NOT_FOUND"
#define ACS_APGCC_ERR_IMM_NO_RESOURCES "ERR_IMM_NO_RESOURCES"
#define ACS_APGCC_ERR_IMM_NOT_SUPPORTED "ERR_IMM_NOT_SUPPORTED"
#define ACS_APGCC_ERR_IMM_BAD_OPERATION "ERR_IMM_BAD_OPERATION"
#define ACS_APGCC_ERR_IMM_FAILED_OPERATION "ERR_IMM_FAILED_OPERATION"
#define ACS_APGCC_ERR_IMM_MESSAGE_ERROR "ERR_IMM_MESSAGE_ERROR"
#define ACS_APGCC_ERR_IMM_QUEUE_FULL "ERR_IMM_QUEUE_FULL"
#define ACS_APGCC_ERR_IMM_QUEUE_NOT_AVAILABLE "ERR_IMM_QUEUE_NOT_AVAILABLE"
#define ACS_APGCC_ERR_IMM_BAD_FLAGS "ERR_IMM_BAD_FLAGS"
#define ACS_APGCC_ERR_IMM_TOO_BIG "ERR_IMM_TOO_BIG"
#define ACS_APGCC_ERR_IMM_NO_SECTIONS "ERR_IMM_NO_SECTION"
#define ACS_APGCC_ERR_IMM_NO_OP "ERR_IMM_NO_OP"
#define ACS_APGCC_ERR_IMM_REPAIR_PENDING "ERR_IMM_REPAIR_PENDING"
#define ACS_APGCC_ERR_IMM_NO_BINDINGS "ERR_IMM_NO_BINDINGS"
#define ACS_APGCC_ERR_IMM_UNAVAILABLE "ERR_IMM_UNAVAILABLE"
#define ACS_APGCC_ERR_IMM_CAMPAIGN_ERR_DETECTED "ERR_IMM_CAMPAIGN_ERR_DETECTED"
#define ACS_APGCC_ERR_IMM_CAMPAIGN_PROC_FAILED "ERR_IMM_CAMPAIGN_PROC_FAILED"
#define ACS_APGCC_ERR_IMM_CAMPAIGN_CANCELED "ERR_IMM_CAMPAIGN_CANCELED"
#define ACS_APGCC_ERR_IMM_CAMPAIGN_FAILED "ERR_IMM_CAMPAIGN_FAILED"
#define ACS_APGCC_ERR_IMM_CAMPAIGN_SUSPENDED "ERR_IMM_CAMPAIGN_SUSPENDED"
#define ACS_APGCC_ERR_IMM_CAMPAIGN_SUSPENDING "ERR_IMM_CAMPAIGN_SUSPENDING"
#define ACS_APGCC_ERR_IMM_ACCESS_DENIED "ERR_IMM_ACCESS_DENIED"
#define ACS_APGCC_ERR_IMM_NOT_READY "ERR_IMM_NOT_READY"
#define ACS_APGCC_ERR_IMM_DEPLOYMENT "ERR_IMM_DEPLOYMENT"
#define ACS_APGCC_ERR_NOT_FOUND "ELEMENT_NOT_FOUND"
#define ACS_APGCC_ERR_TRANSACTION_NOT_FOUND "TRANSACTION_NOT_FOUND"
#define ACS_APGCC_ERR_IMM_SAVE "IMM-SAVE_FAILED"
#define ACS_APGCC_ERR_GENERIC "GENERIC_ERROR"
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";
bool isSWM20();

class acs_prc_apiImplementation
{
public:

	acs_prc_apiImplementation();
	// Description:
	//		Constructor.

	  ~acs_prc_apiImplementation();
	// Description:
	//		Desctructor.
	 static bool is_swm_2_0;
	  bool isRebootPossible (reasonType reason);
	// Description:
	//		The method is used to check if it is possible to 
	//		initiate a reboot. Reboot is in some cases prohibited
	//		to avoid the risk of cyclic reboot.
	//		Following rules apply for reasonType:
	//		manualRebootInitiated and systemInitiated
	//			may always initiate a reboot
	//		causedByFailover, functionChange, referenceFC and softFunctionChange
	//			may initiate 2 reboot within 30 minutes.
	//		causedByError, userInitiated and causedByEvent
	//			may initiate 1 reboot within 30 minutes.
	// Parameters:
	//		reason			Reason for the reboot
	// Return value:
	//		true			It is possible to reboot
	//		false			Not possible to reboot

	  bool rebootNode (reasonType reason = userInitiated,const char* userInfo = "", bool reboot = true);
	//bool rebootNode (reasonType reason = userInitiated,const char* userInfo = "", bool reboot);
	// Description:
	//		Reboot a node
	// Parameters:
	//		reason			The reason for the reboot.
	//						The reason is stored in the ISP-log.
	//						The reason is also used to decide if
	//						reboot is possible. See method
	//						isRebootPossible for further info.
	//		userInfo		Optional user info to be stored in the ISP log
	// Return value:
	//		true			reboot ordered
	//		false			reboot failed. GetLastError may give more details.
	
	bool getSUList(const char* Node, vector<string>& SuList);

	// Description:
	//		The method returns a list of Service Unit in the specified node.
	// Parameters:
	//		Node			node name
	//		SuList			list of all Service Unit
	//
	// Return value:
	//		true			A list is delivered
	//		false			Error detected.

        //Changes start for HW49302
        bool getSUList_new(const char* Node_new, vector<string>& SuList_new);

        // Description:
        //              The method returns a list of Service Unit in the specified node.
        // Parameters:
        //              Node                    node name
        //              SuList                  list of all Service Unit
        //
        // Return value:
        //              true                    A list is delivered
        //              false                   Error detected.

        int getSuState_new (vector<string>& mySuList, const char* SuName, const char* SuSide);
        int getSuState_new (vector<string>& mySuList, const char* SuName);

        // Description:
        //              The method returns a state of Service Unit.
        // Parameters:
        //              SuName                  Service Unit name
        //
        // Return value:
        //              Positive Integer                   A state is delivered
        //              Negative Integer                   Error detected.

        int omHandler_init();

        // Description:
        //              The method intializes the IMM handle that helps in fetching the state of Service Unit.
        // Parameters:
        //              -
        // Return value:
        //              0                                IMM handle successfully initialized
        //              Negative value                   Error detected.

        int omHandler_finalize();

        // Description:
        //              The method finalizes the IMM handle that helps in fetching the state of Service Unit.
        // Parameters:
        //              -
        // Return value:
        //              0                                IMM handle successfully finalized
        //              Negative value                   Error detected.


        //Changes END for TR HW49302

        // Description:
        //              The method returns error detected.
        // Parameters:
        //              -
        // Return value:
        //              error                   Error detected.
	int getSuState(const char* SuName);
	int getSuState(const char* SuName, const char* SuSide);

	// Description:
	//		The method returns a state of Service Unit.
	// Parameters:
	//		SuName			Service Unit name
	//
	// Return value:
	//		true			A name is delivered
	//		false			Error detected.

	time_t lastRebootTime();

	// Description:
	//		The method returns time in milliseconds from last reboot.
	// Parameters:
	//		-
	// Return value:
	//		time_t			milliseconds from last reboot
	//		-1				Error detected.

	int askForNodeState( );

	// Description:
	//		The method returns state of node.
	// Parameters:
	//		-
	// Return value:
	//		int				1 active
	//						2 passive
	//		-1				Error detected.

	int getLastError(void) const;

	// Description:
	//		The method returns error detected.
	// Parameters:
	//		-
	// Return value:
	//		error			Error detected.

	const char* getLastErrorText(void) const;

	// Description:
	//		The method returns error detected.
	// Parameters:
	//		-
	// Return value:
	//		error			Error detected.

	int askForNodeState( int Node );

	// Description:
	//		The method returns state of node.
	// Parameters:
	//		Node 			node
	// Return value:
	//		int				1 active
	//						2 passive
	//		-1				Error detected.
	int askForNodeState( string Node );

	// Description:
	//		The method returns state of node.
	// Parameters:
	//		Node			node name
	// Return value:
	//		int				1 active
	//						2 passive
	//		-1				Error detected.
private:

	int getHaState ( SaNameT objectName );
	//bool getCounter(const char* entryName, int &value);
	SaImmHandleT immHandle;
	std::string errorText;
	std::string strProcName;
	int error;
	void setError(int err, string errText);
	int generalReboot (const char* initiator,  reasonType reason,  string message, bool reboot, bool thisNode,bool ignoreFCH, bool spawnProcess,bool log);
	string addStrings(const char* s1,const char* s2,const char* s3, string s4,const char* s5);
	int load_process_information (pid_t & process_id, char * process_name, size_t size);
	bool executeReboot (string message,bool reboot,reasonType reason);
	bool rebootPRCMAN( );

protected :

	acs_prc_apiImplementation( const acs_prc_apiImplementation &){};

	inline acs_prc_apiImplementation & operator=(const acs_prc_apiImplementation &){ return *this; };

}; // End of class ACS_PRC_API

#endif
