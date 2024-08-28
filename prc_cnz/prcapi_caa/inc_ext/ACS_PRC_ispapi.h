//========================================================================================
// 
// NAME
//		acs_prc_ispapi.h - include file for the ISP-log API
//
// COPYRIGHT
//		Ericsson AB 2004 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the	
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions 
//		stipulated in the agreement/contract under which the 
//		program(s) have been supplied.
//
// DESCRIPTION 
//		API for writing events to the ISP-log.	
//
// ERROR HANDLING
//		-
//
// DOCUMENT NO
//		190 89-CAA 109 0520 Ux 
//
// AUTHOR 
//		2004-02-10 by EAB/UZ/DH UABTSO
//
// REVISION
//		R1A	
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//
//		REV NO	DATE 	NAME		DESCRIPTION
//		R1A		040210  UABTSO		First version
//
// LINKAGE
//		-
//
// SEE ALSO 
//		-
//
//========================================================================================

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_PRC_ISPAPI_H
#define ACS_PRC_ISPAPI_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "acs_prc_api.h"
#include "acs_prc_types.h"
#include <string>
#include <fstream>
#include <time.h>
#include <sstream>
using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     Brief Description

                 The class contains the following methods used by the
                 ACS_PRC_ClusterControl to report ISP status
*/
/*=================================================================== */

class Ispapi_Implementation;

class Ispapi //: public ACS_AEH_Error
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	   /**

	      @brief           Class constructor

	                       The class constructor initializes the private
	                       variable "data" and "bufptr_"; moreover instantiates
	                       a new instance of Isplog class

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */
	Ispapi();

	/*===================================================================
	                        CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
	   /**

	      @brief           Class Destructor

	                       Delete the Isplog class instance

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */
	~Ispapi();

	/*===================================================================
	                           PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	   /**
	      @brief       Open the log file.

					   Try to open the ISP log file in order
					   to write information about In Service Performance event

	      @pre         none

	      @post        none

	      @return      bool true if successful, false otherwise

	      @exception   none
	    */
	/*=================================================================== */
	bool open();

	/*=================================================================== */
	   /**
	      @brief       Close the log file.

					   Try to close the ISP log file.

	      @pre         none

	      @post        none

	      @return      bool true if successful, false otherwise

	      @exception   none
	    */
	/*=================================================================== */
	bool close();

	/*=================================================================== */
	   /**

	      @brief           Report a node runlevel event

	                       When a runlevel event occurs, this method is used to
	                       write the information in the ISP log file

	      @pre             none

	      @post            none

	      @param           node
	                       Node name

	      @param           level
	                       Runlevel
							0 -				Node is stopped
							1 -				Node is running
							2 -				APM resources are partly running
							3 -				All APM resources are up and running
							4 -				Not all resources are running
							5 -				All resources are up and running
							unknownLevel -	Unknown runlevel

	      @param           reason
	                       Reason type

	      @param           info
	                       Miscellaneous information

	      @param           ftime
	                       Time for the event

	      @return          bool			true if successful, false otherwise
	   */
	/*=================================================================== */
	bool runLevelEvent(string node, runLevelType level, reasonType reason,
							string info, time_t ftime = TIME_NULL_);


	/*=================================================================== */
	   /**

	      @brief           Report a node runlevel event

	                       Detailed description

	      @pre             none

	      @post            none

	      @param           node
	                       Node name

	      @param           level
	                       Runlevel
							0 -				Node is stopped
							1 -				Node is running
							2 -				APM resources are partly running
							3 -				All APM resources are up and running
							4 -				Not all resources are running
							5 -				All resources are up and running
							unknownLevel -	Unknown runlevel

	      @param           reason
	                       Reason type

	      @param           order
	                       Sequence of syncronization
							first = 0x1
							last =  0x2

	      @return          bool			true if successful, false otherwise
	   */
	/*=================================================================== */

	bool nodeStateEvent(
							std::string node, nodeStateType state,
							reasonType reason, std::string info = "",
							time_t ftime = TIME_NULL_);

	/*=================================================================== */
	   /**

	      @brief           Report a node State Event

	                       When a node state event occurs, this method is used to
	                       write the information in the ISP log file

	      @pre             none

	      @post            none

	      @param           node
	                       Node name

	      @param           state
						   Node state
							active -			Node is active
							passive -			Node is passive
							unknownNodeState -  The state could not be deduced

	      @param           reason
	                       Reason type

	      @param           info
	                       Miscellaneous information

	      @param           ftime
	                       Time for the event

	      @return          bool			true if successful, false otherwise
	   */
	/*=================================================================== */

	bool SuStateEvent(
							std::string SuName, std::string node,
							resourceStateType state, reasonType reason,
							std::string info = "", time_t ftime = TIME_NULL_);

	/*=================================================================== */
	   /**

	      @brief           Report a Service Unit State change

	                       When a Service Unit state change event occurs, the
	                       information is stored in the ISP log file

	      @pre             none

	      @post            none

	      @param		   SuName
						   Service Unit name

	      @param           node
	                       Node name

	      @param           state
						   Service Unit state
							active -			SU is active
							passive -			SU is passive
							unknownNodeState -  The state could not be deduced

	      @param           reason
	                       Reason type

	      @param           info
	                       Miscellaneous information

	      @param           ftime
	                       Time for the event

	      @return          bool			true if successful, false otherwise
	   */
	/*=================================================================== */

	bool otherNodeShutDown(
							std::string node, reasonType reason, 
							std::string info = "", time_t ftime = TIME_NULL_);
	// Description:
	//		Reports that the other node has shut down
	// Parameters:
	//		node			Node		
	//		reason			Reason type
	//		info			Miscellaneous information
	//		ftime			Time for event (optional)
	// Return value:
	//		bool			true if successful, false otherwise
	// Misc.info
	//		In case the function failed, the ACS_AEH_Error API provides 
	//		detailed information about the fault.

	int getFirstEventLogTime ( std::string& time );

	// Description:
	//		return date of first event presents into ISP-log
	// Parameters:
	//		time			date in format yyyy-MM-DD hh:mm
	// Return value:
	//		int			0 if successful, -1 otherwise
	// Misc.info

	bool APStateEvent( int APstate, time_t ftime = TIME_NULL_);

	void setlocalNode( const char* localNode );

	void setremoteNode( const char* remoteNode );

	static const time_t TIME_NULL_;

	bool node_State_Runlevel_Sync ( std::string node, nodeStateType state, reasonType node_reason, runLevelType RunLEvel, reasonType runlevel_reason);

private:

	Ispapi_Implementation *Isp_Implementation;

};

#endif
