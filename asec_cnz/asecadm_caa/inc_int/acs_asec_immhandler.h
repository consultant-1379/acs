/*
 * * @file acs_asec_immhandler.h
 *	@brief
 *	Header file for ACS_ASEC_ImmHandler class.
 *  This module contains the declaration of the class ACS_ASEC_ImmHandler.
 *
 *	@author enungai
 *	@date 2019-03-21
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2019
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2019-03-21 | enungai      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ASEC_IMMHANDLER_H_
#define ACS_ASEC_IMMHANDLER_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/Task.h>
#include <acs_apgcc_oihandler_V3.h>
#include <acs_asec_oi_ipsecConnection.h>
#include <map>
#include <list>
#include <string>

#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"



/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
//class ACS_ASEC_OI_IpsecConnection;
class ACS_ASEC_OI_GsnhConnection;


class ACE_Barrier;
class ACS_TRA_trace;


/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class ACS_ASEC_ImmHandler: public ACE_Task_Base {

 public:

	/**
		@brief	Constructor of ACS_ASEC_ImmHandler class
	*/
	ACS_ASEC_ImmHandler();

	/**
		@brief	Destructor of ACS_ASEC_ImmHandler class
	*/
	virtual ~ACS_ASEC_ImmHandler();

	/**
	   @brief  	Run by a daemon thread
	*/
	virtual int svc(void);

	/**
	   @brief  	This method initializes a task and prepare it for execution
	*/
	virtual int open(void *args = 0);

	/**
	   @brief  	This method get the stop handle to terminate the svc thread
	*/
	void getStopHandle(int& stopFD){stopFD = m_StopEvent;};

	/**
	   @brief  	This method get the svc thread state
	*/
	bool getSvcState() const {return svc_run;};


	/**
	 * 	@brief	acs_asec_immhandlerTrace: trace object
	*/
	ACS_TRA_trace* acs_asec_immhandlerTrace;

	int close();
	ACS_ASEC_OI_IpsecConnection* m_oi_ipsecConnection;
	ACS_ASEC_OI_IpsecConnection* getOi();
	int getIkeversion() { return p_ikeversion;};
	int p_ikeversion;
	string p_stopRacoon;
	string p_startRacoon;
	string p_stopStrongswan;
	string p_startStrongswan;
	string p_startSetkey;
	string p_stopSetkey;
	int p_timer;
        int p_restartFlag;
	void clearIkev2Data();
	int loadIkev2Connections();
	bool executeCommand(const std::string& command, std::string& output);
	int getIkeVersion();
        int getStrongswanStatus();
        void readIpsecConnFromFile();
        void writeIpsecConnToFile();
        int getIpsecStatus(std::string connName);
        virtual int handle_timeout(const ACE_Time_Value& tv, const void*);


 private:

	/**
	   @brief  		This method register the OI
	*/
	bool registerImmOI();

	/**
	   @brief  	m_oiHandler
	*/
	acs_apgcc_oihandler_V3 m_oiHandler;

	/**
	   @brief  	m_oi_IpsecConnection: IPSEC connection OI
	*/
	//ACS_ASEC_OI_IpsecConnection* m_oi_ipsecConnection;
	
	/**
	   @brief  	m_oi_GsnhConnection: IPSEC connection OI
	*/
	ACS_ASEC_OI_GsnhConnection* m_oi_gsnhConnection;


	/**
	   @brief  	m_StopEvent
	*/
	int m_StopEvent;

	/**
	   @brief  	svc_run: svc state flag
	*/
	bool svc_run;

	ACS_TRA_Logging ASEC_Log;
};

#endif /* ACS_ASEC_IMMHANDLER_H_ */
