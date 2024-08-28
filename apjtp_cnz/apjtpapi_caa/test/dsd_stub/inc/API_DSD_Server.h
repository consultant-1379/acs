/*=================================================================== */
/**
   @file   ACS_DSD_Server.h

   @brief  Header file for DSD stubs for APJTP.

		   This module contains all the declarations useful to
           specify the class ACS_DSD_Server.

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
#ifndef _API_DSD_Server_h
#define _API_DSD_Server_h

/*====================================================================
					  INCLUDE DECLARATION SECTION
==================================================================== */
#include "API_DSD_Base.h"
#include "ACS_JTP_CriticalSectionGuard.h"
#include <vector>
#include <string>

/*=====================================================================
					  FORWARD DECLARATION SECTION
==================================================================== */
class ACS_DSD_Session;

extern ACS_JTP_CriticalSection g_reg;
/*=================================================================== */
	/**
		@class			ACS_DSD_Server

		@brief			This is the interface class for APJTP-DSD.
	**/
 /*=================================================================== */
class ACS_DSD_Server: public ACS_DSD_API_Base
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

			@param			sMode
							Service Mode.

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	ACS_DSD_Server(DSD_API::ServiceMode sMode=DSD_API::SERVICE_MODE_UNKNOWN);

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
	~ACS_DSD_Server();

	/*=================================================================== */
		/**
			@brief			This routine is used to allocate all the necessary
							resources to manage the listening on the service
							access point identification passed in the
							serviceId parameter.

			@pre			None

			@post			None

			@param			serviceId
							Service access point identification.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool open(const ACE_TCHAR * serviceId="0");

	/*=================================================================== */
		/**
			@brief			This routine is used to allocate all the necessary
							resources to manage the listening on the service
							access point identification passed in the
							serviceId parameter.

			@pre			None

			@post			None

			@param			serviceMode
							Mode of service, SOCKET, PIPE.
							Refer DSD_API::ServiceMode.

			@param			serviceId
							Service access point identification.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool open(DSD_API::ServiceMode serviceMode, const ACE_TCHAR * serviceId="0" );

	/*=================================================================== */
		/**
			@brief			This routine is used to register the service
							in the Service Registration table.

			@pre			None

			@post			None

			@param			serviceName
							Name of service.

			@param			serviceDomain
							Domain of service. Both serviceName and serviceDomain
							form a service symbolic name.

			@param			isLocal

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool publish(const ACE_TCHAR* serviceName, const ACE_TCHAR* serviceDomain, bool isLocal=false);

	/*=================================================================== */
		/**
			@brief			This routine is used to initializing a ACS_DSD_Session
							object and making it ready to send and receive data to/
							from client application.

			@pre			None

			@post			None

			@param			session
							ACS_DSD_Session object.

			@param			timeout
							Timeout.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool accept(ACS_DSD_Session& session, ACE_UINT32 timeout = 0);

	/*=================================================================== */
		/**
			@brief			This routine is used to close the service and
							deallocate all the resources.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void close(void);
	/*===================================================================
							PRIVATE DECLARATION SECTION
	=================================================================== */
private:

	DSD_API::ServiceMode svcMode;
	ACE_TCHAR* mServiceName;
	ACE_TCHAR* mServiceDomain;


};

#endif
