/*=================================================================== */
/**
   @file   ACS_DSD_Client.h

   @brief  Header file for DSD stubs for APJTP.

		   This module contains all the declarations useful to
           specify the class ACS_DSD_Client.

   @version N.N.N

   @documentno CAA 109 0870

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
#ifndef _API_DSD_Client_h
#define _API_DSD_Client_h

/*====================================================================
					  INCLUDE DECLARATION SECTION
==================================================================== */
#include "API_DSD_Base.h"
#include <string>
#include <vector>

/*=====================================================================
					  FORWARD DECLARATION SECTION
==================================================================== */
class ACS_DSD_Session;

/*=================================================================== */
	/**
		@class			ACS_DSD_Client

		@brief			This is the interface class for APJTP-DSD.
	**/
 /*=================================================================== */
class ACS_DSD_Client : public ACS_DSD_API_Base
{
public:
	/*=====================================================================
							CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**

			@brief			Constructor

			@pre

			@post

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	ACS_DSD_Client();

	/*===================================================================
					  CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief			Destructor

			@pre			None

			@post			None

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	~ACS_DSD_Client();

	/*=================================================================== */
		/**
			@brief			This routine is used to get the information
							about the nodes which will provide the
							desired service.

			@pre			None

			@post			None

			@param			serviceName
							Name of service.

			@param			serviceDomain
							Domain of Service

			@param			scope
							AP, BC, CP.

			@param			reachableNodes
							Vector of reachable nodes.

			@param			unReachableNodes
							Vector of unreachable nodes.

			@param			milliseconds
							Timeout

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool query(const ACE_TCHAR * serviceName,
			   const ACE_TCHAR * serviceDomain,
			   DSD_API::SystemType scope,
			   std::vector<DSD_API::ACS_DSD_Node>& reachableNodes,
			   std::vector<DSD_API::ACS_DSD_Node>& unReachableNodes,
			   ACE_UINT32 milliseconds = 0);

	/*=================================================================== */
		/**
			@brief			This routine is used to connect to the node
							providing desired service.

			@pre			None

			@post			None

			@param			session
							Pointer to a ACS_DSD_Session.

			@param			serviceName
							Name of service.

			@param			serviceDomain
							Domain of Service

			@param			nodes
							ACS_DSD_Node.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		 */
	/*=================================================================== */
	bool connect(ACS_DSD_Session& session,
				const ACE_TCHAR * serviceName,
				const ACE_TCHAR * serviceDomain,
				const DSD_API::ACS_DSD_Node* node);

	/*===================================================================
							PRIVATE DECLARATION SECTION
	=================================================================== */
private:

};	/* end ACS_DSD_Client */
#endif
