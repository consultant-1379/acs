/*=================================================================== */
/**
   @file   acs_aca_message_store_interface.h

   @brief Header file for MSA API.

	This module contains all the declarations useful to
	specify the class ACS_ACA_MessageStoreInterface. This 
	class is intended to provide some utility functions to client
	applications.


   COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, 2011.
	All rights reserved.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       16/08/2011     XTANAGG       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_MESSAGE_STORE_INTERFACE_H
#define ACS_ACA_MESSAGE_STORE_INTERFACE_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>
#include <map>
#include <ace/ACE.h>
#include <ACS_CS_API.h>
#include <ACS_APGCC_ApplicationManager.h>
#include <unistd.h>
#include <ace/Task.h>
#include <ace/OS_NS_poll.h>
#include <ace/ACE.h>
#include <ace/Process.h>
#include <ace/Event.h>
#include <ACS_APGCC_CommonLib.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_ACA_MessageStoreInterface 
                 
*/
/*=================================================================== */
class ACS_ACA_MessageStoreInterface : public ACS_CS_API_CpTableObserver
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

	@brief           ACS_ACA_MessageStoreInterface

	*/
/*=================================================================== */
	
	ACS_ACA_MessageStoreInterface() : 
		theSubscriptionMgr(ACS_CS_API_SubscriptionMgr::getInstance()),
		_errorCode(0), 
		_errorDescription()
	{
		subscribeToCsServices ();
	}

/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */

/*=================================================================== */
	 /**

	   @brief	~ACS_ACA_MessageStoreInterface

	 */
/*=================================================================== */
	virtual ~ACS_ACA_MessageStoreInterface(){
		unSubscribeToCsServices ();
	}

	
	static const int ACA_NO_ERROR = 0;

	static const int ACA_ERR_CHECKING_MULTIPLE_CP = -1;
	static const int ACA_ERR_CS_API_FAILURE = -2;
	static const int ACA_ERR_COMMON_DLL_API_FAILURE = -3;
	static const int ACA_ERR_CANNOT_GET_EVENT_HANDLE = -4;

	static const char CP_FIRST_CONTACT_EVENT_PREFIX_NAME[];

	/*=================================================================== */
	   /**

	      @brief           getCPsForName

	      @param           msName

	      @param           cpNames

	      @return          bool

	      @exception       none
	   */
	/*=================================================================== */
	bool getCPsForName(const std::string & msName, std::vector<unsigned> & cpNames);
	/*=================================================================== */
	   /**

	      @brief           getHandle

	      @param           msName

	      @return          ACE_event_t

	      @exception       none
	   */
	/*=================================================================== */
	ACE_event_t getHandle(const std::string & msName);
	/*=================================================================== */
	   /**

	      @brief           getLastError

	      @return          int
			       _errorCode

	      @exception       none
	   */
	/*=================================================================== */
	inline int getLastError() const { return _errorCode; }
	/*=================================================================== */
	   /**

	      @brief           getLastErrorDescription

	      @return          std::string
			       Error Description

	      @exception       none
	   */
	/*=================================================================== */
	inline const std::string & getLastErrorDescription() const { return _errorDescription; }
	/*=================================================================== */
	   /**

	      @brief           clearLastError

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */
	inline void clearLastError() { _errorCode = 0; _errorDescription.clear(); }

	/*=================================================================== */
	   /**

	      @brief           update

	      @param           aObservee

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */
	virtual void update(const ACS_CS_API_CpTableChange & aObservee);

	
	/*=================================================================== */
	   /**

	      @brief           theSubscriptionMgr
	   */
	/*=================================================================== */

	ACS_CS_API_SubscriptionMgr * theSubscriptionMgr;

	/*=================================================================== */
	   /**

	      @brief           subscribeToCsServices

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */
	void subscribeToCsServices ();
	/*=================================================================== */
	   /**

	      @brief           unSubscribeToCsServices

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */
	void unSubscribeToCsServices ();
	/*=================================================================== */
	/*===================================================================
                        PRIVATE DECLARATION SECTION
	=================================================================== */

private:
	/*=================================================================== */
	   /**

	      @brief           fxCheckMultipleCPSystem

	      @return          int

	      @exception       none
	   */
	/*=================================================================== */
	int fxCheckMultipleCPSystem();
	/*=================================================================== */
	   /**

	      @brief           fxGetCpIds

	      @return          int

	      @exception       none
	   */
	/*=================================================================== */
	int fxGetCpIds();
	/*=================================================================== */
	   /**

	      @brief           fxGetPath

	      @param           cpSystemId

	      @param           basePath

	      @return          int

	      @exception       none
	   */
	/*=================================================================== */
	int fxGetPath(unsigned cpSystemId, std::string & basePath);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		_errorCode
	*/
	/*=================================================================== */
	int _errorCode;
	/*=================================================================== */
	/**
		@brief		_errorDescription
	*/
	/*=================================================================== */
	std::string _errorDescription;
	/*=================================================================== */
	/**
		@brief		_isMultipleCPSystem
	*/
	/*=================================================================== */
	static bool _isMultipleCPSystem;
	/*=================================================================== */
	/**
		@brief		firstTimeMulCP
	*/
	/*=================================================================== */
	static bool firstTimeMulCP;
	/*=================================================================== */
	/**
		@brief		firstTimeCPID
	*/
	/*=================================================================== */
	static bool firstTimeCPID;
	/*=================================================================== */
	/**
		@brief		_cpMap
	*/
	/*=================================================================== */
	static std::map<unsigned, std::string> _cpMap;
	/*=================================================================== */
	/**
		@brief		_namedEventMap
	*/
	/*=================================================================== */
	static std::map<std::string, ACE_event_t> _namedEventMap;
	/*=================================================================== */
};
#endif
