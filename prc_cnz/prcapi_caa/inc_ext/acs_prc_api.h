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

#ifndef ACS_PRC_API_H
#define ACS_PRC_API_H

#include <iostream>
#include <string.h>
#include <saImmOm.h>
#include <saClm.h>
#include <vector>
#include "acs_prc_types.h"

using namespace std;
class acs_prc_apiImplementation;

class ACS_PRC_API
{
public:

	ACS_PRC_API();
	// Description:
	//		Constructor.

	~ACS_PRC_API();
	// Description:
	//		Desctructor.

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

        //Changes start for TR HW49302
        bool getSUList_new(const char* Node_new, vector<string>& SuList_new);

        // Description:
        //              The method returns a list of Service Unit in the specified node.
        // Parameters:
        //              Node                    node name
        //              SuList_temp                  list of all Service Unit
        //
        // Return value:
        //              true                    A list is delivered
        //              false                   Error detected.

       
        int getSuState_new (vector<string>& mySuList, const char* SuName );
        int getSuState_new (vector<string>& mySuList, const char* SuName, const char* SuSide );

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
        //              0                               IMM handle successfully initialized
        //              Negative value                  Error detected.

        int omHandler_finalize();
 
        // Description:
        //              The method finalizes the IMM handle that helps in fetching the state of Service Unit.
        // Parameters:
        //              -
        // Return value:
        //              0                                IMM handle successfully finalized
        //              Negative value                   Error detected.


        //Changes END for TR HW49302

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

	acs_prc_apiImplementation *Api_Implementation;

protected :

	ACS_PRC_API( const ACS_PRC_API &){};

	inline ACS_PRC_API & operator=(const ACS_PRC_API &){ return *this; };

}; // End of class ACS_PRC_API

#endif
