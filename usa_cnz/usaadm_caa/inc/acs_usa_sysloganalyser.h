
#ifndef ACS_USA_SYSLOGANALYZOR_H
#define ACS_USA_SYSLOGANALYZOR_H


#include <iostream>
#include <ace/Task_T.h>
#include "ace/ACE.h"
#include "sys/poll.h"
#include "ACS_APGCC_AmfTypes.h"
#include "acs_usa_types.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_control.h"
#include "acs_usa_error.h"
#include <ace/Sig_Handler.h>
/*===============================================================
		DIRECTIVE DECLARATION SECTION
=================================================================*/

#define USA_PIPE "/var/run/ap/acs_usafifo"
#define USA_TIMESTAMP_FILE "/opt/ap/acs/bin/usa.tmstamp"
#define USA_LOGANALYSER_RUNNING "/opt/ap/acs/bin/usa.analyser.running"

#define SIG_IGNORE_EVENTS ACE_SIGRTMIN+1
#define SIG_RESUME_EVENTS ACE_SIGRTMIN+2

using namespace std;

// Forwards
class Global;
class usaHAClass;
class LocalUserInfo;
class ACS_USA_AUT_THREAD;
//class ACS_USA_Control;

/** @class acs_usa_sysloganalyzor 
 * 	@brief 	ACS_USA_SLA class
 * 	@author	
 * 	@date
 * 	@version
 * 	acs_usa_class class detailed description
 */	
class ACS_USA_SLA:public ACE_Task<ACE_SYNCH> {

	private:

		Global *m_global_instance;
		usaHAClass* m_haObj;
		LocalUserInfo *userInfo;
		ACS_USA_AUT_THREAD* usaAdhObj;
		long m_timerid;
		long m_ignrEvntsTimerid;
		void setHandle(ACE_HANDLE);
		ACE_HANDLE _handle;
		bool m_haMode;
		bool m_loganalyser_file;
		int p_open();
		int p_close();
		int _startupAnalysis();
		int _shortTermAnalysis();
		int _initialize();
		int _setTimer();
		void dispatch();
		ACS_USA_Control *m_cntlObj;
		ACS_USA_AnalysisObject *m_analysisObj;
		int createLogAnalyserFile();
		void unlink_loganalyser_file();
		int executeMmlCommand (const std::string bc_name, const std::string& command, std::string& output);
		void createAdhInterfaceObject();
		string getCurrentTime();
        
	public:
		ACS_USA_SLA();
		virtual ~ACS_USA_SLA();
		ACE_Sig_Handler sig_shutdown_;
		int svc();
		int close(u_long);
		int handle_timeout(const ACE_Time_Value&, const void*);
		int handle_input(ACE_HANDLE fd);
		int handle_signal(int signum,siginfo_t *,ucontext_t *);
		ACE_HANDLE get_handle(void) const;
		int open(usaHAClass*); 
		int close();
		static void collectUsaStopTime();
		ACS_USA_ReturnType loadConfig();
		LocalUserInfo* userDetails()
		{
			return userInfo;
		}
};
class LocalUserInfo 
{
	std::string username;
	int monitoingStatus;
public:
	LocalUserInfo(std::string usr="NONE", int status = 0)
	{
		username = usr;
		monitoingStatus = status;
	}
	
	std::string getUserName()
	{
		return this->username;
	}

	int getMonitoingStatus()
	{
		return this->monitoingStatus;
	}
	
	void setUserName(std::string usr)
	{
		username = usr;
	}
	
	void setMonitoingStatus(int status)
	{
		monitoingStatus = status;
	}
};

class ACS_USA_AUT_THREAD: public ACE_Task_Base
{
public:
	ACS_USA_AUT_THREAD();
	int svc();
	~ACS_USA_AUT_THREAD();
	void start();
	void triggerObjCreation();
	int createAdhInterfaceObject();
	string getCurrentTime();
};
#endif /* end ACS_USA_SYSLOGANALYZOR_H */

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------
