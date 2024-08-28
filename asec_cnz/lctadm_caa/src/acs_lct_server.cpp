//******************************************************************************
//
// NAME
//      ACS_LCT_server.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//     Class method implementation for Object Implementor for Hardening service            
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//       XSATDEE
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//			2012-25-04      XFURULL         welcomemessage system 
//							improvement(OP#345) on all ports
//
//******************************************************************************  
/*
   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -------------------------------------------------------------
   N/A       DD/MM/YYYY    XSATDEE     APG43 on Linux
   ============================================================= */


/*=====================================================================
                INCLUDE DECLARATION SECTION
  =====================================================================*/

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Task.h>
#include <ace/INET_Addr.h>
#include <acs_lct_dsdserver.h>
#include <acs_lct_server.h>
#include <saImmOi.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ACS_APGCC_Util.H>
#include <acs_prc_api.h>
#include <ace/Process.h>
#include <ace/Event.h>
#include <acs_apgcc_omhandler.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <acs_lct_tra.h>
#include <acs_lct_adm.h>
//#include "acs_lct_struct_define.h"

/*=====================================================================
                DEFINE DECLARATION SECTION
 =====================================================================*/
#define LCTSERVICE
using namespace std;
imm_replica immrep;
ACS_LCT_Server *hardeningServerObj = 0;
string hardeningDnName = "";
string tsUsersDnName = "";
string securityhandlingDnName = "";
/*=================================================================== 
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*=====================================================================
   ROUTINE: ACS_LCT_Server
  =====================================================================*/

ACS_LCT_Server::ACS_LCT_Server()
:
		m_poCommandHandler(0),
		m_poCommandHandler1(0),
                m_poCommandHandler2(0)
{
	//Is_terminated = false;
	hardeningThreadId=0;
	tsUsersThreadId=0;
        securityhandlingThreadId=0;
	hardeningServerObj=this;
	m_ShutdownInprogress = false;	
}

/*=====================================================================
   ROUTINE: ACS_LCT_Server
  =====================================================================*/

ACS_LCT_Server::ACS_LCT_Server(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	m_poCommandHandler=0;
	m_poCommandHandler1=0;
	m_poCommandHandler2=0;
	hardeningThreadId=0;
	tsUsersThreadId=0;
        securityhandlingThreadId=0;
	hardeningServerObj=this;
	//Is_terminated = false;

	passiveToActive=0;
	m_AdmObj = 0;
	m_ShutdownInprogress = false;	
}

/*void ACS_LCT_Server::sig_hardening_handler(int signum) {
	ACE_UNUSED_ARG(signum);
	DEBUG("%s", "received sighup signal.. invoking thread shutdown for hardening");
	//hardeningServerObj->shutdown_hardening();

}
void ACS_LCT_Server::sig_tsusers_handler(int signum) {
	ACE_UNUSED_ARG(signum);
	DEBUG("%s", "received sighup signal.. invoking thread shutdown for tsusers");
	//hardeningServerObj->shutdown_tsusers();
 }

void ACS_LCT_Server::sig_securityhandling_handler(int signum) {
        ACE_UNUSED_ARG(signum);
        DEBUG("%s","received sighup signal.. invoking thread shutdown for securityhandling");
} */
/*=====================================================================
   ROUTINE: ~ACS_LCT_Server
  =====================================================================*/

ACS_LCT_Server::~ACS_LCT_Server()
{	
	DEBUG("%s","Inside ACS_LCT_Server destructor");
}

/*=====================================================================
   ROUTINE: startup
  =====================================================================*/

bool ACS_LCT_Server::startup()
{
	bool ret=true, fin_ret=true;
	struct stat st;
	//Check configap
	ret=configapCheck();

	// UPdate values of all attributes from IMM (not needed anymore since rollback functionality is beeing taken care in the script)
	ret=updateFromImm("inactivityTimer");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("lockoutDuration");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("lockoutBadCount");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("maximumAccountAge");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("maximumPasswordAge");
        if (ret == false) { fin_ret=false; }
	ret=updateFromImm("minimumPasswordAge");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("minimumPasswordLength");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("passwordHistorySize");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditUserLevelMessages");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditSystemDaemons");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditSyslog");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditCronMessages");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditFTPServer");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditSSHServer");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("auditTelnetServer");
	if (ret == false) { fin_ret=false; }
	ret=updateFromImm("welcomeMessage");
	if (ret == false) { fin_ret=false; }
	if (stat(COM_LOCK, &st) == 0)
	{
		char tmpCmd[180]={'\0'};
		strcpy(tmpCmd,"rm -f ");
		strcat(tmpCmd,COM_LOCK);
		ACE_OS::system(tmpCmd);
	}

	// Commit changes to audit policies (this is needed to avoid too many rsyslogd restarts")
	const std::string tmp_cmd = "hardening -f";
	ACE_OS::system(tmp_cmd.c_str());

	return fin_ret;
}

/*=====================================================================
   ROUTINE: fetchDnOfRootObjFromIMM
  =====================================================================*/

bool ACS_LCT_Server::fetchDnOfRootObjFromIMM ()
{
	OmHandler omHandler;
	bool myResult;
	if (omHandler.Init() != ACS_CC_SUCCESS)
	{
		ERROR("%s","fetchDnOfRootObjFromIMM: OmHandler INIT Failed");
		ERROR("%s","Internal program error. Exit code: 26");
		return false;
	}

	std::vector<std::string> dnList;
	dnList.clear();
	if (omHandler.getClassInstances(CONFIG_IMM_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
	{   
		myResult = false;
		ERROR("%s","fetchDnOfRootObjFromIMM: getClassInstances failed");
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() > 1)
		{
			WARNING("%s","fetchDnOfRootObjFromIMM: More than 1 instance found");
			myResult = false;
		}
		else
		{
			//OK: Only one root node
			hardeningDnName = dnList[0].c_str();
			cout << hardeningDnName << endl;
			syslog (LOG_INFO, "hardening: DN name for hardening: %s", hardeningDnName.c_str());
			DEBUG("%s","fetchDnOfRootObjFromIMM: Fetching DN name");
			myResult = true;
		}
	}

	if(omHandler.Finalize() == ACS_CC_FAILURE)
	{
		ERROR("%s","fetchDnOfRootObjFromIMM: Init finalize failed");
		myResult = false;
	}

	OmHandler omHandler1;
	if (omHandler1.Init() != ACS_CC_SUCCESS)
	{
		ERROR("%s","fetchDnOfRootObjFromIMM: OmHandler INIT Failed");
		ERROR("%s","Internal program error. Exit code: 26");
		return false;
	}

	dnList.clear();
	if (omHandler1.getClassInstances(CONFIG_IMM_CLASS_NAME1, dnList) != ACS_CC_SUCCESS)
	{
		myResult = false;
		ERROR("%s","fetchDnOfRootObjFromIMM: getClassInstances failed");
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() > 1)
		{
			//WARNING: 0 (zero) or more than one node found
			WARNING("%s","fetchDnOfRootObjFromIMM: More than 1 instance found");
			myResult = false;
		}
		else
		{
			//OK: Only one root node
			tsUsersDnName = dnList[0].c_str();
			syslog (LOG_INFO, "hardening: DN name for TS users: %s", tsUsersDnName.c_str());
			DEBUG("%s","fetchDnOfRootObjFromIMM: Fetching DN name");
			myResult = true;
		}
	}

	if(omHandler1.Finalize() == ACS_CC_FAILURE)
	{
		ERROR("%s","fetchDnOfRootObjFromIMM: Init finalize failed");
		myResult = false;
	}

	OmHandler omHandler2;
        if (omHandler2.Init() != ACS_CC_SUCCESS)
        {
                ERROR("%s %s %s","fetchDnOfRootObjFromIMM: OmHandler INIT Failed:", securityhandlingDnName.c_str(), omHandler2.getInternalLastErrorText());
                ERROR("%s","Internal program error. Exit code: 26");
                return false;
        }

        dnList.clear();
        if (omHandler2.getClassInstances(CONFIG_IMM_CLASS_NAME2, dnList) != ACS_CC_SUCCESS)
        {   
                myResult = false;
                ERROR("%s %s %s","fetchDnOfRootObjFromIMM: getClassInstances failed", securityhandlingDnName.c_str(), omHandler2.getInternalLastErrorText());
        }
        else
        {
                //OK: Checking how much dn items was found in IMM
                if (dnList.size() > 1)
                {
                        //WARNING: 0 (zero) or more than one node found
                        WARNING("%s %s","fetchDnOfRootObjFromIMM: More than 1 instance found", securityhandlingDnName.c_str());
                        myResult = false;
                }
                else
                {
                        //OK: Only one root node
                        securityhandlingDnName = dnList[0].c_str();
			syslog (LOG_INFO, "hardening: DN name for securityhandling: %s ", securityhandlingDnName.c_str());
                        DEBUG("%s","fetchDnOfRootObjFromIMM: Fetching DN name");
                        myResult = true;
                }
        }

        if(omHandler2.Finalize() == ACS_CC_FAILURE)
        {
                ERROR("%s %s %s","fetchDnOfRootObjFromIMM: Init finalize failed", securityhandlingDnName.c_str(), omHandler2.getInternalLastErrorText());
                myResult = false;
	}
	return myResult;
}

/*=====================================================================
   ROUTINE: configapCheck
  =====================================================================*/

bool ACS_LCT_Server::configapCheck()
{
	/*
        The configap -a command will be launched after every node reboot and 
	hence at all the following scenarios:
        - During Scratch Installation
        - During Backup and Restore
        - During Node Upgrade

	It will check for the existence of a /tmp/configap_a file and 
	launch the command if the tmp file does not exist. 
	The file will be created by configap -a command
	 */
	/*
        Apart from assigning ACLs,
        this command will also set the faillog value of tsadmin
        so that it never gets locked after node upgrade or B&R when faillog setting is deleted.
	 */

	string file_name="/tmp/configap_a";
	struct stat st;
	if (stat(file_name.c_str(), &st) == 0)
	{
		DEBUG("%s", "Configap test file exists");
	}
	else
	{
		DEBUG("%s", "Configap test file does not exist. Launching configap -a command...");
		int retCode=ACE_OS::system("configap -a & >/dev/null");
		if (retCode == -1){
			DEBUG("%s","system call failed while launching configap command");
			return false;
		}

		retCode=WEXITSTATUS(retCode);

		if (retCode != 0) {
			DEBUG("%s","Command execution failed while executing configap");
			return false;
		}
		else {
			DEBUG("%s","configap -a command launched successfully in the background. Please check the logs to verify the outcome of the command");
			syslog (LOG_INFO, "configap -a command launched successfully in the background. Please check the logs to verify the outcome of the command");
		}
	}
	return true;
}

/*=====================================================================
   ROUTINE: updateFromImm 
  =====================================================================*/

bool ACS_LCT_Server::updateFromImm(string attr_name)
{
	string attr_val, value,fin_val;
	string domain_name=tsUsersDnName;
	string hardening_domain=hardeningDnName;
       	string securityhandling_domain=securityhandlingDnName; 
	OmHandler omHandler ;
	ACS_CC_ImmParameter paramToFind ;
	ACE_UINT32 val;
	bool fileUpdate=true;
	ACE_INT32 val_int;
	stringstream out;
	bool return_val=true;

	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		//cout << "OmHandler INIT Failed" << endl;
		ERROR("%s","update_from_imm: OmHandler INIT Failed");
		//ERROR("%s","Internal program error. Exit code: 26");
		return_val=false;
	}
	else
	{
		if (attr_name.compare("welcomeMessage") == 0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			//if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
                        if (omHandler.getAttribute(securityhandling_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				value = (reinterpret_cast<const char*>(*(paramToFind.attrValues)));
				attr_val=value;
				DEBUG("%s - %s ","welcomeMessage : ", attr_val.c_str());
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for welcomeMessage");
				}

			}
			else
			{
				ERROR("%s","OM getAttribute Failed for welcomeMessage");
				return_val=false;
			}
		}
		if (attr_name.compare("inactivityTimer")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				DEBUG("%s","OM getAttribute success for inactivityTimer");
				out << val;
				attr_val=out.str();
				//immrep.inactivityTimer=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for inactivityTimer");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for inactivityTimer");
				return_val=false;
			}
}	
		if(attr_name.compare("maximumAccountAge")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				DEBUG("%s","OM getAttribute success for maximumAccountAge");
				out << val;
				attr_val=out.str();
				//immrep.maximumPasswordAge=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for maximumAccountAge");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for maximumAccountAge");
				return_val=false;
			}
		}
		if(attr_name.compare("maximumPasswordAge")==0)
                {
                        paramToFind.attrName = (char*)attr_name.c_str();
                        if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
                        {
                                val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
                                DEBUG("%s","OM getAttribute success for maximumPasswordAge");
                                out << val;
                                attr_val=out.str();
                                //immrep.maximumPasswordAge=val;
                                fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
                                if (!fileUpdate)
                                {
                                        ERROR("%s","Initial configuration failed for maximumPasswordAge");
                                }
                        }
                        else
                        {
                                ERROR("%s","OM getAttribute Failed for maximumPasswordAge");
                                return_val=false;
                        }
                }

		if(attr_name.compare("minimumPasswordAge")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				DEBUG("%s","OM getAttribute success for minimumPasswordAge");
				out << val;
				attr_val=out.str();
				//immrep.minimumPasswordAge=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for minimumPasswordAge");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for minimumPasswordAge");
				return_val=false;
			}
		}
		if(attr_name.compare("minimumPasswordLength")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				DEBUG("%s","OM getAttribute success for minimumPasswordLength");
				out << val;
				attr_val=out.str();
				//immrep.minimumPasswordLength=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for minimumPasswordLength");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for minimumPasswordLength");
				return_val=false;
			}
		}
		if(attr_name.compare("passwordHistorySize")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				out << val;
				attr_val=out.str();
				//immrep.passwordHistorySize=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for passwordHistorySize");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for passwordHistorySize");
				//ERROR("%s","Internal program error. Exit code: 26");
				return_val=false;
			}
		}
		if(attr_name.compare("lockoutBadCount")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				out << val;
				attr_val=out.str();
				//immrep.lockoutBadCount=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for lockoutBadCount");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for lockoutBadCount");
				return_val=false;
			}
		}
		if(attr_name.compare("lockoutDuration")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(domain_name.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val = *(reinterpret_cast<ACE_UINT32*>(*(paramToFind.attrValues)));
				out << val;
				attr_val=out.str();
				//immrep.lockoutDuration=val;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for lockoutDuration");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for lockoutDuration");
				return_val=false;
			}
		}

		if(attr_name.compare("auditUserLevelMessages")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditUserLevelMessages=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				DEBUG("%s","auditUserLevelMessages updated");
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditUserLevelMessages");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditUserLevelMessages");
				return_val=false;
			}
		}
		if(attr_name.compare("auditSystemDaemons")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditSystemDaemons=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				DEBUG("%s","auditSystemDaemons updated");
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditSystemDaemons");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditSystemDaemons");
				return_val=false;
			}
		}
		if(attr_name.compare("auditSyslog")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditSyslog=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditSyslog");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditSyslog");
				return_val=false;
			}
		}
		if(attr_name.compare("auditCronMessages")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditCronMessages=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditCronMessages");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditCronMessages");
				return_val=false;
			}
		}
		if(attr_name.compare("auditFTPServer")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditFTPServer=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditTPDaemon");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditFTPDaemon");
				return_val=false;
			}
		}
		if(attr_name.compare("auditSSHServer")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditSSHServer=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditSSHMessages");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditSSHMessages");
				return_val=false;
			}
		}
		if(attr_name.compare("auditTelnetServer")==0)
		{
			paramToFind.attrName = (char*)attr_name.c_str();
			if (omHandler.getAttribute(hardening_domain.c_str(),&paramToFind) == ACS_CC_SUCCESS)
			{
				val_int = *(reinterpret_cast<ACE_INT32*>(*(paramToFind.attrValues)));
				out << val_int;
				attr_val=out.str();
				//immrep.auditTelnetServer=val_int;
				fileUpdate=dsdObj->modify_config_file(attr_name,attr_val);
				if (!fileUpdate)
				{
					ERROR("%s","Initial configuration failed for auditTelnetMessages");
				}
			}
			else
			{
				ERROR("%s","OM getAttribute Failed for auditTelnetMessages");
				return_val=false;
			}
		}
	}
	if (omHandler.Finalize() == ACS_CC_FAILURE)
	{
		ERROR("%s","OM Finalize failed");
		return_val=false;
	}
	return return_val;
}

/*=====================================================================
   ROUTINE: setupIMMCallBacks 
  =====================================================================*/

bool ACS_LCT_Server::setupIMMCallBacksForHardening()
{
	DEBUG("%s","Inside setupIMMCallbacksForHardening function");
	string dn_name(hardeningDnName);
	//dn_name = "AxeHardeninghardeningMId=1";
	cout << dn_name << endl;
	m_poCommandHandler = new ACS_LCT_CommandHandler_Hardening(dn_name,
			"acs_hardening_cfg_handler",
			ACS_APGCC_ONE );
	if (m_poCommandHandler == 0) {
		DEBUG("%s","Inside setupIMMCallbacksForHardening function, failed");
		return false;
	}
	int out=m_poCommandHandler->svc();

#if 0
	if (m_poCommandHandler != 0) {
		delete m_poCommandHandler;
		m_poCommandHandler=0;
		DEBUG("%s","Deleted Command Handler for Hardening");
	}
#endif

	if (out == -1) {
		ERROR("%s","Command Handler SVC failed");
		return false;
	}
	return true;
}

bool ACS_LCT_Server::setupIMMCallBacksForTsUsers()
{
	DEBUG("%s","Inside setupIMMCallbacksForTsUsers function");
	string dn_name(tsUsersDnName);
	cout << dn_name << endl;
	m_poCommandHandler1 = new ACS_LCT_CommandHandler_TSUsers(dn_name,
			"acs_tsusers_cfg_handler",
			ACS_APGCC_ONE );
	if (m_poCommandHandler1 == 0) {
		DEBUG("%s","Inside setupIMMCallbacksForTsUsers function, failed");
		return false;
	}

	int out=m_poCommandHandler1->svc();
#if 0
	if (m_poCommandHandler1 != 0) {
		delete m_poCommandHandler1;
		m_poCommandHandler1=0;
		DEBUG("%s","Deleted Command Handler for TS user");
	}
#endif

	if (out == -1) {
		ERROR("%s","Command Handler SVC failed");
		return false;
	}

	return true;
}

bool ACS_LCT_Server::setupIMMCallBacksForsecurityhandling()
{
   DEBUG("%s","Inside setupIMMCallBacksForsecurityhandling function");
   string dn_name(securityhandlingDnName);
   m_poCommandHandler2= new ACS_LCT_CommandHandler_securityhandling(dn_name,"acs_security_cfg_handler",ACS_APGCC_ONE);
   if (m_poCommandHandler2 == 0) {
                DEBUG("%s","Inside setupIMMCallbacksForSecurityhandling function, failed");
                return false;
        }

   int out=m_poCommandHandler2->svc();
   if (out == -1) {
           ERROR("%s","securityhandling Command Handler SVC failed");
           if (m_poCommandHandler2 != 0) {
                delete m_poCommandHandler2;
                m_poCommandHandler2=0;
                DEBUG("%s","Deleted Command Handler for Securityhandling");
        }

           return false;
   }

   return true;
}

/*=====================================================================
   ROUTINE: hardening_thread
  =====================================================================*/

ACE_THR_FUNC_RETURN hardening_thread(void *ptr)
{
	cout << "Inside hardening thread" << endl;
	ACS_LCT_Server *hardeningObj = (ACS_LCT_Server*) ptr;
	hardeningObj->startHardeningFunctionality();
	return 0;
}

/*=====================================================================
   ROUTINE: tsUsers_thread
  =====================================================================*/

ACE_THR_FUNC_RETURN tsUsers_thread(void *ptr)
{
	ACS_LCT_Server *tsUsersObj = (ACS_LCT_Server*) ptr;
	tsUsersObj->startTsUsersFunctionality();
	return 0;
}

/*=====================================================================
   ROUTINE: securityhandling_thread
  =====================================================================*/

ACE_THR_FUNC_RETURN securityhandling_thread(void *ptr)
{
        ACS_LCT_Server *securityhandlingObj = (ACS_LCT_Server*) ptr;
        securityhandlingObj->startsecurityhandlingFunctionality();
        return 0;
}

/*=====================================================================
   ROUTINE: startFunctionality
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_Server::startFunctionality()
{
	//if(signal(SIGSEGV,handle_signal)==SIG_ERR){
	//       DEBUG("%s %s","Hardening: Unable to register the SIGSEGV signal:",strerror(errno));
	//}

	//syslog(LOG_INFO, "Inside startFunctionality function");
	DEBUG("%s","Inside startFunctionality function");
	syslog(LOG_INFO,"Inside startFunctionality function");
	INFO("%s","Hardening Application received ACTIVE state assignment");
	const ACE_TCHAR* thread_name = "hardeningAppThread";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&hardening_thread,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&hardeningThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	cout << "Hardening instance spawned" << threadHandle << endl;
	if (threadHandle == -1){
		ERROR("%s","startFunctionality: Error creating the hardening application thread");
		ERROR("%s","Internal program error. Exit code: 26");
		return ACS_CC_FAILURE;
	}
        ACE_OS::sleep(2);

	const ACE_TCHAR* thread1_name = "tsUsersAppThread";
	ACE_HANDLE threadHandle1 = ACE_Thread_Manager::instance()->spawn(&tsUsers_thread,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&tsUsersThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread1_name);
	cout << "TsUsers instance spawned" << threadHandle1 << endl;
	if (threadHandle1 == -1){
		ERROR("%s","startFunctionality: Error creating the TSUsers application thread");
		ERROR("%s","Internal program error. Exit code: 26");
		return ACS_CC_FAILURE;
	}

        ACE_OS::sleep(1);
	
        const ACE_TCHAR* thread2_name = "securityAppThread";
        ACE_HANDLE threadHandle2 = ACE_Thread_Manager::instance()->spawn(&securityhandling_thread,
                        (void *)this ,
                        THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
                        &securityhandlingThreadId,
                        0,
                        ACE_DEFAULT_THREAD_PRIORITY,
                        -1,
                        0,
                        ACE_DEFAULT_THREAD_STACKSIZE,
                        &thread2_name);

       if (threadHandle2 == -1){
                ERROR("%s","startFunctionality: Error creating the welcomemessage application thread");
                ERROR("%s","Internal program error. Exit code: 26");
                return ACS_CC_FAILURE;
        }
	
        return ACS_CC_SUCCESS;

}

/*=====================================================================
   Removing Object Implementer During SIGSEGV Signal Catch
  =====================================================================*/
# if 0
void ACS_LCT_Server::handle_signal(int signum){
	switch (signum) {
	case SIGSEGV:
		if(hardeningServerObj!= NULL){
			hardeningServerObj->shutdownApp();
			break;


	default:
		DEBUG("%s","Hardening: Other signal caught.");
		break;
		}
	}
}
# endif
/*=====================================================================
   ROUTINE: startHardeningFunctionality
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::startHardeningFunctionality()
{
	DEBUG("%s","Inside startHardeningFunctionality function");
	hardeningServerObj=this;

	bool res1=setupIMMCallBacksForHardening();
	if (res1)
		return ACS_APGCC_SUCCESS;
	else
		return ACS_APGCC_FAILURE;

}

/*=====================================================================
   ROUTINE: startTsUsersFunctionality
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::startTsUsersFunctionality()
{
	DEBUG("%s","Inside startTsUsersFunctionality function");
	hardeningServerObj=this;

	bool res1=setupIMMCallBacksForTsUsers();
	if (res1)
		return ACS_APGCC_SUCCESS;
	else
		return ACS_APGCC_FAILURE;
}

/*=====================================================================
   ROUTINE:startsecurityhandlingFunctionality 
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::startsecurityhandlingFunctionality()
{
	DEBUG("%s","Inside startsecurityhandlingFunctionality function");

    	bool res1=setupIMMCallBacksForsecurityhandling();
	if (res1)
		return ACS_APGCC_SUCCESS;
	else
		return ACS_APGCC_FAILURE;
}


/*=====================================================================
   ROUTINE: startDebugMode
  =====================================================================*/

bool ACS_LCT_Server::startDebugMode()
{
	bool return_val=true;
	ACS_CC_ReturnType myReturnErrorCode;
	int node_state=dsdObj->CheckActiveNode();
	if (node_state == 1)
	{
		DEBUG("%s","Inside active node");
		bool ret_dsd=true;
		if (ret_dsd)
		{
			DEBUG("%s","Starting hardening service on active node");

			myReturnErrorCode=startFunctionality();
			if(myReturnErrorCode != ACS_CC_FAILURE)
			{
				DEBUG("%s","Hardening service started");
				while (true) ;
				return_val=true;
			}
			else
			{
				DEBUG("%s","Hardening Service failed");
				ERROR("%s","Internal program error. Exit code: 26");
				return_val=false;
			}
		}
	}
	else if (node_state == 2)
	{
		bool Published=false;
		DEBUG("%s","Inside passive node");
		DEBUG("%s","Starting DSD server");
		dsdObj->instantiate();
		while (true) {
			if (!Published) {
				Published=dsdObj->StartDSDServer();
			}
			if (Published)
			{
				DEBUG("%s","DSD Server started");
				bool Sucessful= dsdObj->DSDServerfunc();
				if (!Sucessful)
				{
					ERROR("%s","DSD Server functionality failed");
					return_val=false;
				}
			} else
			{
				ERROR("%s","DSD Server Publish failed");
				return_val=false;
			}
		}
	}else
	{
		cout << "node state in server.cpp: " << node_state;
		ERROR("%s","Node state undefined");
		ERROR("%s","Internal program error. Exit code: 26");
		return_val=false;
	}
	return return_val;
}

/*=====================================================================
   ROUTINE: shutdown methods
  =====================================================================*/

ACE_INT32 ACS_LCT_Server::shutdown_hardening()
{
	DEBUG("%s","Shutting down hardening application");
	int ret=false;
	if (m_poCommandHandler != 0) {
		ret=m_poCommandHandler->shutdown();
		delete m_poCommandHandler;
		m_poCommandHandler=0;
		DEBUG("%s","Deleted Command Handler for Hardening");
	}

	return ret;
}
ACE_INT32 ACS_LCT_Server::shutdown_tsusers()
{
	DEBUG("%s","Shutting down tsuser loop");
	int ret=false;
	if (m_poCommandHandler1 != 0) {
		ret=m_poCommandHandler1->shutdown();
		delete m_poCommandHandler1;
		m_poCommandHandler1=0;
		DEBUG("%s","Deleted Command Handler for TS user");
	}
	return ret;
}

ACE_INT32 ACS_LCT_Server::shutdown_securityhandling()
{
   DEBUG("%s","Shutting down securityhandling loop");
   int ret=false;
   if (m_poCommandHandler2 != 0) {
	ret=m_poCommandHandler2->shutdown();
	delete m_poCommandHandler2;
	m_poCommandHandler2=0;
	DEBUG("%s","Deleted Command Handler for welcomemessage");
   }
   return ret;
}
ACE_INT32 ACS_LCT_Server::shutdown_lct_debug(){

	ACE_INT32 rCode=0;
	if (!Is_Active) 
		return rCode;

	DEBUG("%s","Shutting down hardening application");
	rCode=m_poCommandHandler->shutdown();

	DEBUG("%s","Shutting down tsuser loop");
	rCode=m_poCommandHandler1->shutdown();

	DEBUG("%s","Shutting down securityhandling loop");
	rCode=m_poCommandHandler2->shutdown();
	
	ACE_Thread_Manager::instance()->join(hardeningThreadId);
	ACE_Thread_Manager::instance()->join(tsUsersThreadId);
	ACE_Thread_Manager::instance()->join(securityhandlingThreadId);
	return rCode;
}

/*=====================================================================
   ROUTINE: performStateTransitionToActiveJobs
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState){
	(void) previousHAState;
	DEBUG("%s","ACS_LCT_Server::performStateTransitionToActiveJobs");
	m_ShutdownInprogress = false;
	if(ACS_APGCC_AMF_HA_STANDBY == previousHAState)
		passiveToActive = 1;
	return this->activateApp();
}

/*=====================================================================
   ROUTINE: performStateTransitionToPassiveJobs
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState) {
	(void) previousHAState;
	DEBUG("%s","ACS_LCT_Server::performStateTransitionToPassiveJobs");
	m_ShutdownInprogress = false;	
	return this->passifyApp();
}

/*=====================================================================
   ROUTINE: performStateTransitionToQueisingJobs
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	(void) previousHAState;
	return this->shutdownApp();
}

/*=====================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	(void) previousHAState;
	return this->shutdownApp();
}

/*=====================================================================
   ROUTINE: performComponentHealthCheck
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performComponentHealthCheck(void)
{
	INFO("%s","Hardening Application Component received healthcheck query!");
	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are ok.
	 */
	if( hardeningThreadId > 0 && ACE_Thread_Manager::instance()->testterminate(hardeningThreadId))
	{
		//syslog(LOG_INFO, "Component health Check failed for Hardeningd");
		INFO("%s","Component health Check failed for Hardeningd");
		return ACS_APGCC_FAILURE;
	}

	//syslog( LOG_INFO,"Health check succeeded for hardeningd");
	INFO("%s","Component health Check succeeded for Hardeningd");
	return ACS_APGCC_SUCCESS;
}

/*=====================================================================
   ROUTINE: performComponentTerminateJobs
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performComponentTerminateJobs(void)
{
	//syslog(LOG_INFO, "hardeningservice:performComponentTerminateJobs: My Application Component received terminate callback!!!");
	INFO("%s","performComponentTerminateJobs: Hardening Application Component received terminate callback");
	syslog(LOG_INFO, "Received terminate callback!!!");
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_LCT_Server::performComponentRemoveJobs(void)
{
	//syslog(LOG_INFO, "hardeningservice:performComponentRemoveJobs: Application Assignment is removed now");
	INFO("%s","performComponentRemoveJobs: Hardening application Assignment is removed now");
	syslog(LOG_INFO, "Application Assignment is removed now");
	return this->shutdownApp();
}

/*=====================================================================
   ROUTINE: performApplicationShutdownJobs
  =====================================================================*/

ACS_APGCC_ReturnType ACS_LCT_Server::performApplicationShutdownJobs()
{
	//syslog(LOG_ERR, "hardeningservice:performApplicationShutdownJobs: Shutting down the application");
	INFO("%s","performApplicationShutdownJobs: Shutting down the application");
	syslog(LOG_INFO, "Application is shutting down now");
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_LCT_Server::shutdownApp(){

	INFO("%s","ACS_LCT_Server :: In shutdownApp function");

	if(true == m_ShutdownInprogress) {
		return ACS_APGCC_SUCCESS;
	}
	else {
		if ( 0 != this->m_AdmObj){
			m_ShutdownInprogress = true;
			INFO("%s","ACS_LCT_Server :: In shutdownApp function IF condition");
			this->m_AdmObj->stop(); // This will initiate the application shutdown and will not return until application is stopped completely.
			this->m_AdmObj->wait();
			delete this->m_AdmObj;
			this->m_AdmObj=0;
		}
		return ACS_APGCC_SUCCESS;
	}
}

ACS_APGCC_ReturnType ACS_LCT_Server::activateApp() {
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	if ( 0 != this->m_AdmObj) {
		rCode = ACS_APGCC_SUCCESS;
		if (passiveToActive){
			INFO("%s", "State Transition happend, becomming Active Now");
			this->m_AdmObj->stop();
			this->m_AdmObj->wait();
			passiveToActive=0;
		}
	} else {
		ACE_NEW_NORETURN(this->m_AdmObj, ACS_LCT_Adm());
		if (0 == this->m_AdmObj) {
			syslog(LOG_ERR, "acs_lct_server: failed to create the instance");
		}
	}

	Is_Active=TRUE;
	if ( 0 != this->m_AdmObj) {
		INFO("%s", "ACS_LCT_Server::activateApp() If Active condition");
		int res = this->m_AdmObj->active(this); // This will start active functionality. Will not return until myCLass is running
		if (res < 0) {
			// Failed to start
			delete this->m_AdmObj;
			this->m_AdmObj = 0;
		} else {
			rCode = ACS_APGCC_SUCCESS;
		}
	}
	return rCode;
}

ACS_APGCC_ReturnType ACS_LCT_Server::passifyApp() {

	INFO("%s", "ACS_LCT_Server::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	Is_Active=FALSE;
	//        passiveToActive=1;
	if (0 != this->m_AdmObj) {
		rCode = ACS_APGCC_SUCCESS;
	} else {
		ACE_NEW_NORETURN(this->m_AdmObj, ACS_LCT_Adm());
		if (0 == this->m_AdmObj) {
			syslog(LOG_ERR, "acs_lct_server: failed to create the instance");
		}
		else {
			int res = this->m_AdmObj->passive(this); // This will start passive functionality and will not return until myCLass is running
			if (res < 0) {
				// Failed to start
				delete this->m_AdmObj;
				this->m_AdmObj = 0;
			} else {

				rCode = ACS_APGCC_SUCCESS;
			}
		}
	}
	return rCode;
}





//=====================================================================

