/*=================================================================== */
/**
   @file   API_DSD_Base.h

   @brief  Header file for DSD stubs for APJTP.

		   This module contains all the declarations useful to
           specify the class ACS_DSD_API_Base.
           .
   @version N.N.N

   @copyright Ericsson AB,Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/28/2010     TA        APG43 on Linux.
==================================================================== */
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _API_DSD_Base_h
#define _API_DSD_Base_h

/*====================================================================
						INCLUDE DECLARATION SECTION
==================================================================== */
#include "API_Namespace.h"
#include "ace/Log_Msg.h"
#include <iostream>
#include <fstream>
using namespace std;

/*=================================================================== */
	/**
		@class			ACS_DSD_API_Base

		@brief			This is the interface class for APJTP-DSD.
	**/
 /*=================================================================== */
class ACS_DSD_API_Base
{

public:
	/*===================================================================
						CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief           Destructor

			@pre             none

			@post            none

			@return          none

			@exception       none
		*/
	/*=================================================================== */
	virtual ~ACS_DSD_API_Base();

	/*=================================================================== */
		/**
			@brief			This is the routine that is to be used
							to get Node information.

			@param			node
							DSD node.

			@param			nodeSide
							NODE_SIDE_LOCAL, NODE_SIDE_REMOTE.

			@pre			none

			@post			none

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	void getNode(DSD_API::ACS_DSD_Node& node, DSD_API::NodeSide nodeSide);

	/*=================================================================== */
		/**
			@brief			This routine is used to get the last error.

			@pre			none

			@post			none

			@return			ACE_INT32
							Error No.

			@exception		None
		 */
	/*=================================================================== */
	ACE_INT32 getLastError(void) const;

	/*=================================================================== */
		/**
			@brief			This routine is to used
							to get the last error description.

			@pre			none

			@post			none

			@return			ACE_TCHAR*
							Error description.

			@exception		none
		*/
	/*=================================================================== */
	const ACE_TCHAR* getLastErrorText(void) const;

	/*=================================================================== */
		/**
			@brief			This routine is to used to set the
							last error no and description.

			@pre			None

			@post			None

			@param			err
							Error number

			@param			errText
							Error description.

			@return			ACE_TCHAR*
							Error description.

			@exception		None
		*/
	/*=================================================================== */
	void setError(ACE_INT32 err, const ACE_TCHAR* errText);

	/*=================================================================== */
		/**
			@brief			This routine is to used
							to get the HANDLES.

			@pre			None

			@post			None

			@param			handles
							Handle List

			@param			handle_count
							Count of handles.

			@return			ACE_TCHAR*
							Error description.

			@exception		None
		*/
	/*=================================================================== */
	void getHandles(ACE_HANDLE*& handles, ACE_INT32& handleCount);

	DSD_API::MsgType mMsgType;
	/*===================================================================
							PRIVATE DECLARATION SECTION
	=================================================================== */
private:

	/*===================================================================
							PRIVATE ATTRIBUTE
	=================================================================== */
	ACE_INT32 error;
	ACE_TCHAR* errorText;
	ACE_HANDLE internalHandleList[2];

	/*===================================================================
							PROTECTED DECLARATION SECTION
	=================================================================== */
protected:
	/*=====================================================================
							CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**

			@brief           	Constructor

			@pre

			@post

			@return          	none

			@exception       	none
		*/
	ACS_DSD_API_Base();


};	/* end ACS_DSD_Base */

#endif
