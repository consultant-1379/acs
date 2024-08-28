//******************************************************************************
//
// NAME
//      ACS_LCT_CommandHandler_TSUsers.cpp
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

/*=====================================================================
                INCLUDE DECLARATION SECTION
=====================================================================*/

#include <iostream>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <acs_lct_server.h>
#include <acs_lct_dsdserver.h>
#include <acs_apgcc_oihandler_V3.h>
//#include <ACS_APGCC_OiHandler.h>
#include <ACS_APGCC_Util.H>
#include <acs_lct_command_handler_tsusers.h>
#include <acs_lct_tra.h>
using namespace std;

//ACS_LCT_CommandHandler_TSUsers *gTsUserObj = NULL;
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*=====================================================================
   ROUTINE: ACS_LCT_CommandHandler_TSUsers
  =====================================================================*/

ACS_LCT_CommandHandler_TSUsers::ACS_LCT_CommandHandler_TSUsers(string szObjName,
		string szImpName,
		ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( szObjName,szImpName,enScope )
{
	(void)CONFIG_IMM_CLASS_NAME;
	(void)CONFIG_IMM_CLASS_NAME1;
	(void)CONFIG_IMM_CLASS_NAME2;
	//syslog(LOG_INFO, "Inside Command Handler for TS Users constructor");
	DEBUG("%s","Inside Command Handler for TS USersconstructor");
	m_TPReactorImpl = 0;
	m_poReactor = 0;
	m_TPReactorImpl = new ACE_TP_Reactor();
	if(m_TPReactorImpl != 0)
	{
		m_poReactor = new ACE_Reactor(m_TPReactorImpl);
	}
	//	gTsUserObj = this;
}

/*=====================================================================
   ROUTINE: ~ACS_LCT_CommandHandler_TSUsers
  =====================================================================*/

ACS_LCT_CommandHandler_TSUsers::~ACS_LCT_CommandHandler_TSUsers()
{
	DEBUG("%s","Inside TS User Command Handler destructor");
	if(m_poReactor != 0)
	{
		delete m_poReactor;
		m_poReactor=0;
		DEBUG("%s","Deleted TS User po reactor");
	}

	if(m_TPReactorImpl != 0)
	{
		delete m_TPReactorImpl;
		m_TPReactorImpl=0;
		DEBUG("%s","Deleted TS User po reactor implementer");
	}
	//	gTsUserObj = NULL;
}

/*=====================================================================
   ROUTINE: svc
  =====================================================================*/

ACE_INT32 ACS_LCT_CommandHandler_TSUsers::svc()
{
	DEBUG("%s","ACS_LCT_CommandHandler_TSUsers::svc()");
	try{
		ACS_CC_ReturnType errorCode = oiHandler.addObjectImpl(this);

		if ( errorCode == ACS_CC_FAILURE )
		{
			ACS_CC_ReturnType ErrorCode = oiHandler.removeObjectImpl(this);

			if ( ErrorCode == ACS_CC_FAILURE ) {
				DEBUG("%s","No Registered ObjectImplementer for TsUser");
			}
			else {
				DEBUG("%s","Registered ObjectImplementer for TsUser Is Unregistered");
			}

			ERROR("%s","Command Handler SVC failed for TS Users : Internal program error. Exit code: 26");
			// try registering with the oiHandler five times
			// before we give up.
			for (int i=0; i<10; i++){
				errorCode = oiHandler.addObjectImpl(this);
				if ( errorCode != ACS_CC_FAILURE ){
					DEBUG("%s","Command Handler SVC success for TS Users Eventually.");
					break;
				}
				ACE_Time_Value selectTime;
                		selectTime.set(1,0); //sleep for a second
                		while((ACE_OS::poll(0,0,selectTime) == -1) && (errno == EINTR))
                		{
                        		continue;
                		}

				//DEBUG("%s","TsUser Before Remove ObjectImplementer");
				//errorCode = oiHandler.removeObjectImpl(this);
				//DEBUG("%s %d","TsUser After Remove ObjectImplementer errorCode:",errorCode);
			}

			if ( errorCode == ACS_CC_FAILURE ){
				ERROR("%s","addObjectImpl registration failed.");
				return -1;
                        }
		}
		else
		{
			DEBUG("%s","Command Handler SVC success for TS users");
		}
	}catch(...)
	{
		DEBUG("%s","Error occurred while creating or deleting the objectImplementer:CommandHandler_TsUsers");
	}
	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();
	return 0;
}

/*=====================================================================
   ROUTINE: shutdown
  =====================================================================*/

ACE_INT32 ACS_LCT_CommandHandler_TSUsers::shutdown()
{
	DEBUG("%s","Inside shutdown method of TS User command handler");
	m_poReactor->end_reactor_event_loop();
	DEBUG("%s","Ended reactor event loop of tsuser");
	//Start of HR30134
	int rcode =0;
	/* removeObjectImp is returning Failure sometimes, so trying again */
	const ACE_INT32 ATTEMPTS=10;
	for (int i=0; i<ATTEMPTS; i++){
		rcode = oiHandler.removeObjectImpl(this);
			if ( rcode != 0 ){
				DEBUG("%s %d","oiHandler.removeObjectImpl() Failure -- rCode:", rcode);
				DEBUG("%s %d", "Attempt: ", i+1);
			}else { break;}
		ACE_Time_Value selectTime;
                selectTime.set(1,0); //sleep for a second
                while((ACE_OS::poll(0,0,selectTime) == -1) && (errno == EINTR))
                {
                        continue;
                }

	}
	if ( rcode == 0){
		DEBUG("%s", "oiHandler.removeObjectImpl() -- Success");
	}
	//End of HR30134
	return 0;
}

/*=====================================================================
   ROUTINE: create
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_TSUsers::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s %ld %ld %s %s %s","Inside create callback", oiHandle, ccbId, className, parentName, attr);
	return ACS_CC_FAILURE;
}

/*=====================================================================
   ROUTINE: deleted
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_TSUsers::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s %ld %ld %s","Inside deleted callback", oiHandle, ccbId, objName);
	return ACS_CC_FAILURE;
}

/*=====================================================================
   ROUTINE: modify
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_TSUsers::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_CC_ReturnType enResult= ACS_CC_SUCCESS;
	DEBUG("%s %ld %ld %s","Inside modify callback", oiHandle, ccbId, objName);
	int attr_cnt=0;
	bool wrong_attr=false;
	string value,val1, option;
	ACE_UINT32 new_val=0;
	ACE_INT32 myErrCode=0;
	string cmd,myErrMsg,tmp_str,attrs,attr_name,cluster_attr,local_attr;

	while ( attrMods[attr_cnt] )
	{
		stringstream out;
		attr_name=attrMods[attr_cnt]->modAttr.attrName;
		DEBUG("%s" "%s","modify callback: attr name: ",attr_name.c_str());

		switch ( attrMods[attr_cnt]->modAttr.attrType )
		{

		case ATTR_UINT32T:
		{
			//if (attrMods[attr_cnt]->modAttr.attrValuesNum == 0)
			//	wrong_attr=true;
			//else
			//{
			new_val=*(reinterpret_cast<ACE_UINT32 *>(attrMods[attr_cnt]->modAttr.attrValues[0]));
			out << new_val;
			value=out.str();
			DEBUG("%s" "%s","modify callback: attr value:UINT32: ",value.c_str());
			//}
			break;
		}
		case ATTR_INT32T:
		{
			//if (attrMods[attr_cnt]->modAttr.attrValuesNum == 0)
			//	wrong_attr=true;
			//else
			//{
			new_val=*(reinterpret_cast<ACE_INT32 *>(attrMods[attr_cnt]->modAttr.attrValues[0]));
			out << new_val;
			value=out.str();
			DEBUG("%s" "%s","modify callback: attr value:INT32: ",value.c_str());
			//}
			break;
		}
		default:
			wrong_attr=true;
			break;
		}

		attr_name = attrMods[attr_cnt]->modAttr.attrName;
		if (!(attr_name.compare("lockoutBadCount"))) {
			option = "-b";
			local_attr += " " + option + " " + value;
			DEBUG("%s", "Updating lockoutBadCount value into acs-apg-lockout-tsgroup file!");
		}
		else if (!(attr_name.compare("lockoutDuration"))) {
			option = "-d";
			local_attr += " " + option + " " + value;
			DEBUG("%s", "Updating lockoutDuration value into acs-apg-lockout-tsgroup file!");
		}
		else if (!(attr_name.compare("passwordHistorySize"))) {
			option = "-h";
			local_attr += " " + option + " " + value;
			DEBUG("%s", "Updating passwordHistorySize value into acs-apg-password-local file!");
		}
		else if (!(attr_name.compare("minimumPasswordLength"))) {
			option = "-m";
			local_attr += " " + option + " " + value;
			DEBUG("%s", "Updating minimumPasswordLength value into acs-apg-password-local file!");
		}
		else if (!(attr_name.compare("maximumAccountAge"))) {
			option = "-e";
			local_attr += " " + option + " " + value;
			DEBUG("%s", "Updating maximumAccountAge value!");
		}
		attr_cnt++;

	} // End of while

	DEBUG("%s" "%s","modify callback: attr names:",attr_name.c_str());
	if (wrong_attr)
	{
		DEBUG("%s","modify callback: Wrong attribute called");
		return ACS_CC_FAILURE;
	}

	// Modify configuration file according to parameter passed on active node
	DEBUG("%s","modify callback:Modifying config files on local node");
	cmd="hardening";
	char command2[cmd.length()+local_attr.length()];
	sprintf(command2, "%s %s" ,cmd.c_str(),local_attr.c_str());
	bool ret_val = dsdObj->modify_config_file(command2, myErrCode, myErrMsg);
	if (!ret_val)
	{
		DEBUG("%s","modify callback: Linux configuration file not modified on active node.");
		setExitCode(myErrCode,myErrMsg);
		return ACS_CC_FAILURE;
	}

	bool dsdReturnVal=true;
	//syslog(LOG_INFO, "modify callback: Linux Config file modified successfully on active node");
	DEBUG("%s","modify callback: Linux Config file modified successfully on active node");

	// checking the state of passive node
	string cmd1;
	cmd1="/usr/bin/prcstate -l | grep passive | grep -c up";
	char buffer[100]={0},command[400];
	sprintf(command,cmd1.c_str());
	//cout << "\n command: " << command;
	FILE* pipe=popen(command,"r");
	string node_state;
	//syslog(LOG_INFO, "After pipe open to check passive node status");
	DEBUG("%s","After pipe open to check passive node status");
	if (!pipe)
	{
		//syslog(LOG_INFO, "error in pipe");
		ERROR("%s","Pipe open failed in modify callback");
	}
	else
	{
		fgets(buffer, 100, pipe);
		node_state=buffer[0];
		pclose(pipe);
	}

	if (node_state.compare("1") != 0)
	{
		//syslog(LOG_ERR, "Passive node is down. Changes applied successfully on active node only.");
		INFO("%s","Passive node is down. Changes applied successfully on active node only");
		enResult = ACS_CC_SUCCESS;
	}
	else
	{
		INFO("%s","Passive node is up and running.");
		DEBUG("%s","modify callback: Starting DSD Client");
		bool ret_dsd= dsdObj->StartDSDClient();
		if ( ret_dsd )
		{
			DEBUG("%s","modify callback: DSD Client connected successfully");
			DEBUG("%s","modify callback: Calling DSD Client function");
			bool return_value = dsdObj->DSDClientfunc(command2,myErrCode,myErrMsg);
			if ( return_value )
			{
				DEBUG("%s","modify callback: Operations in passive node successfully completed");
				enResult = ACS_CC_SUCCESS;
			}
			else
			{
				DEBUG("%s","modify callback: Operations in passive node failed");
				dsdReturnVal=false;
				setExitCode(myErrCode,myErrMsg);
				enResult = ACS_CC_FAILURE;
			}
			dsdObj->closeDSDClient();
		}
		else
		{
			//syslog(LOG_INFO, "modify callback: DSD Client start failed");
			DEBUG("%s","modify callback: DSD Client start failed");
			dsdReturnVal=false;
			myErrCode=117;
			myErrMsg="Error Connecting to server";
			setExitCode(myErrCode,myErrMsg);
			enResult = ACS_CC_FAILURE;
		}

		if(dsdReturnVal==false)
		{
			//syslog(LOG_INFO, "modify callback: Rolling back changes on active node");
			enResult = ACS_CC_FAILURE;
		}

	} // if passive node is up

	if (enResult == ACS_CC_SUCCESS)
	{
		DEBUG("%s","modify callback: Updating the local structure with new IMM values");
		//syslog(LOG_INFO, "modify callback: Parameter [%s] modified",attr_name.c_str());
		//dsdObj->localUpdate(attrs);
	}
	DEBUG ("%s","End of modify callback.");

	return enResult;
}

/*=====================================================================
   ROUTINE: complete
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_TSUsers::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside complete callback", oiHandle, ccbId);
	return ACS_CC_SUCCESS;
}

/*=====================================================================
   ROUTINE: abort
  =====================================================================*/

void ACS_LCT_CommandHandler_TSUsers::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside abort callback", oiHandle, ccbId);

	DEBUG("%s","Rolling back file changes");
	string cmd="hardening";
	string attr="-r";
	string node_state;
	char command[cmd.length()+attr.length()];
	sprintf(command, "%s %s" ,cmd.c_str(),attr.c_str());
	int ret=dsdObj->launchCommand(command);
	if (ret==0)
	{
		DEBUG("%s","Rollback successful");
	}
	else
	{
		DEBUG("%s","Rollback failed");
	}

	DEBUG("%s","Leaving abort callback");
}

/*=====================================================================
   ROUTINE: apply
  =====================================================================*/

void ACS_LCT_CommandHandler_TSUsers::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside apply callback", oiHandle, ccbId);

	DEBUG("%s","Committing cluster file changes");

	string cmd="hardening";
	string attr="-c";
	string node_state;
	char command[cmd.length()+attr.length()];
	sprintf(command, "%s %s" ,cmd.c_str(),attr.c_str());
	int ret=dsdObj->launchCommand(command);

	if (ret==0)
	{
		DEBUG("%s","Commit successful");
	}
	else
	{	
		DEBUG("%s","Commit failed");
	}
	DEBUG("%s","Leaving apply callback");
}

/*=====================================================================
   ROUTINE: updateRuntime
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_TSUsers::updateRuntime(const char *objName, const char **attrName) 
{
	DEBUG("%s","Inside updateRuntime callback");
        (void)objName;
        (void)attrName; 

	return ACS_CC_SUCCESS;
}

void ACS_LCT_CommandHandler_TSUsers::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("%s %ld %s %ld %ld","adminOperationCallback", oiHandle, invocation, p_objName, operationId, paramList);

}


