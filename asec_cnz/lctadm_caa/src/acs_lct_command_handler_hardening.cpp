//******************************************************************************
//
// NAME
//      ACS_LCT_CommandHandler_Hardening.cpp
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
#include <ACS_APGCC_Util.H>
#include <acs_lct_command_handler_hardening.h>
#include <acs_lct_tra.h>
using namespace std;

//ACS_LCT_CommandHandler_Hardening *gHardenObj = NULL;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*=====================================================================
   ROUTINE: ACS_LCT_CommandHandler_Hardening
  =====================================================================*/

ACS_LCT_CommandHandler_Hardening::ACS_LCT_CommandHandler_Hardening(string szObjName,
		string szImpName,
		ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( szObjName,szImpName,enScope )
{
	(void)CONFIG_IMM_CLASS_NAME;
	(void)CONFIG_IMM_CLASS_NAME1;
	(void)CONFIG_IMM_CLASS_NAME2;
	DEBUG("%s","Inside Hardening Command Handler constructor");
	m_TPReactorImpl = 0;
	m_poReactor = 0;
	m_TPReactorImpl = new ACE_TP_Reactor();
	if(m_TPReactorImpl != 0)
	{
		m_poReactor = new ACE_Reactor(m_TPReactorImpl);
	}
	//gHardenObj = this;
}

/*=====================================================================
   ROUTINE: ~ACS_LCT_CommandHandler_Hardening
  =====================================================================*/

ACS_LCT_CommandHandler_Hardening::~ACS_LCT_CommandHandler_Hardening()
{
	DEBUG("%s","Inside Hardening Command Handler destructor");
	if(m_poReactor != 0)
	{
		delete m_poReactor;
		m_poReactor=0;
		DEBUG("%s","Deleted hardening po reactor");
	}

	if(m_TPReactorImpl != 0)
	{
		delete m_TPReactorImpl;
		m_TPReactorImpl=0;
		DEBUG("%s","Deleted hardening po reactor implementer");
	}
	//gHardenObj = NULL;
}

/*=====================================================================
   ROUTINE: svc
  =====================================================================*/

ACE_INT32 ACS_LCT_CommandHandler_Hardening::svc()
{
	DEBUG("%s","ACS_LCT_CommandHandler_Hardening::svc()");
	try{
		ACS_CC_ReturnType errorCode = oiHandler.addObjectImpl(this);
		if ( errorCode == ACS_CC_FAILURE )
		{
			ACS_CC_ReturnType ErrorCode = oiHandler.removeObjectImpl(this);
			if ( ErrorCode == ACS_CC_FAILURE ) {
				DEBUG("%s","No Registered ObjectImplementer for Hardening");
			}
			else {
				DEBUG("%s","Registered ObjectImplementer for Hardening Is Unregistered");
			}

			ERROR("%s","Command Handler SVC failed : Internal program error. Exit code: 26");
			// try registering with the oiHandler five times
			// before we give up.
			for (int i=0; i<10; i++){
				errorCode = oiHandler.addObjectImpl(this);
				if ( errorCode != ACS_CC_FAILURE ) {
					DEBUG("%s","Hardening Command Handler SVC success Eventually");
					break;
				}

				ACE_Time_Value selectTime; 
				selectTime.set(1,0); //sleep for a second
				while((ACE_OS::poll(0,0,selectTime) == -1) && (errno == EINTR))
				{
					continue;
				}

				//DEBUG("%s","Hardening Before Remove ObjectImplementer");
				//errorCode = oiHandler.removeObjectImpl(this);
				//DEBUG("%s %d","Hardening After Remove ObjectImplementer, errorCode:",errorCode);
			}
			if ( errorCode == ACS_CC_FAILURE ){
				ERROR("%s","addObjectImpl registration failed.");
				return -1;
                        }
		}
		else{
			DEBUG("%s","Command Handler SVC success for hardening");
		}
	}catch(...)
	{
		DEBUG("%s","Error occurred while creating or deleting the objectImplementer:CommandHandler_Hardening");
	}

	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();
	return 0;
}

// Code added for error handling in removeObjectImplementer for TR HQ56525
/*=====================================================================
   ROUTINE: shutdown
  =====================================================================*/

ACE_INT32 ACS_LCT_CommandHandler_Hardening::shutdown()
{
	DEBUG("%s","Inside shutdown method of Hardening command handler");
	m_poReactor->end_reactor_event_loop();
	DEBUG("%s","Ended reactor event loop of hardening");
	//Start of HR30134
	int rcode=0;
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

ACS_CC_ReturnType ACS_LCT_CommandHandler_Hardening::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s %ld %ld %s %s %s","Inside create callback", oiHandle, ccbId, className, parentName, attr);
	return ACS_CC_FAILURE;
}

/*=====================================================================
   ROUTINE: deleted
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_Hardening::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s %ld %ld %s","Inside deleted callback", oiHandle, ccbId, objName);
	return ACS_CC_FAILURE;
}

/*=====================================================================
   ROUTINE: modify
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_Hardening::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	DEBUG("%s %ld %ld %s","Inside modify callback", oiHandle, ccbId, objName);
	ACS_CC_ReturnType enResult= ACS_CC_SUCCESS;
	int attr_cnt=0;
	bool cluster_flag=false, local_flag=false, wrong_attr=false;
	string value,val1;
	ACE_UINT32 new_val=0;
	ACE_INT32 myErrCode=0;
	string cmd,myErrMsg,tmp_str,attrs,attr_name, cluster_attr;

	while ( attrMods[attr_cnt] )
	{
		stringstream out;

		switch ( attrMods[attr_cnt]->modAttr.attrType )
		{
		case ATTR_UINT32T:
		{
			//if (attrMods[attr_cnt]->modAttr.attrValuesNum == 0)
			//        wrong_attr=true;
			//else
			//{
			new_val=*(reinterpret_cast<ACE_UINT32 *>(attrMods[attr_cnt]->modAttr.attrValues[0]));
			out << new_val;
			value=out.str();
			//}
			break;
		}
		case ATTR_INT32T:
		{
			//if (attrMods[attr_cnt]->modAttr.attrValuesNum == 0)
			//        wrong_attr=true;
			//else
			//{
			new_val=*(reinterpret_cast<ACE_INT32 *>(attrMods[attr_cnt]->modAttr.attrValues[0]));
			out << new_val;
			value=out.str();
			//}
			break;
		}
		case ATTR_STRINGT:
		{
			//if (attrMods[attr_cnt]->modAttr.attrValuesNum == 0)
			//        wrong_attr=true;
			//else
			//{
			val1=reinterpret_cast<const char *>(attrMods[attr_cnt]->modAttr.attrValues[0]);
			//value=val1.substr(0,MAX_BYTES);
			//value[MAX_BYTES]='\0';
			value=val1;
			if (attrMods[attr_cnt]->modAttr.attrValuesNum==0)
			{
				DEBUG("%s","modify callback: string null");
				value="";
			}
			//}
			break;
		}
		default:
			break;
		}

		attr_name=attrMods[attr_cnt]->modAttr.attrName;

		if (attr_name.compare("inactivityTimer")==0)
		{
			//cluster_attr+=attr_name+ ":" + value + "~";
			cluster_attr+=" -t "+ value;
			cluster_flag=true;
			DEBUG("%s" "%s","cluster flag set for attribute: ", attr_name.c_str());
			//flag_inactivityTimer=1;
		}
		/*else if (attr_name.compare("welcomeMessage")==0)
		{
			unsigned int i,flag=0;
			for (i=0;i<value.length();i++)
			{
				if(value[i] == '<') {
					char *carr_string=NULL;
					carr_string=&value[i];
					if (strncmp(carr_string,"<BR>",4) != 0){
						flag=1;
					}
				}
				else if(value[i] == '&')
					flag=1;
			}

			if (flag==1)
			{
				DEBUG("%s","Welcome message contains invalid special characters. Aborting..");
				myErrCode=15;
				myErrMsg="Unreasonable value: Welcome message contains invalid special characters";
				setExitCode(myErrCode,myErrMsg);
				return ACS_CC_FAILURE;
			}
			else
			{   
				cluster_attr+=" -w \"" + value + "\"";
				DEBUG("%s" "%s","cluster flag set for attribute: ", attr_name.c_str());
				cluster_flag=true;
				flag_welcomeMessage=1;
			}
		}*/
		else
		{
			// Considering that all the remaining are audit policies
			string chkAudit=attr_name.substr(0,5);
			if (( strcmp(chkAudit.c_str(),"audit") == 0 ))
			{
				local_flag=true;
				attrs+=attr_name+ ":" + value + "~";
				DEBUG("%s" "%s","local flag set for attribute: ", attr_name.c_str());
			}
			else
				DEBUG("%s" "%s","nothing defined for attribute: ", attr_name.c_str());
		}

		attr_cnt++;

	} // End of while

	DEBUG("%s" "%s","modify callback: attr names:",attr_name.c_str());
	if (wrong_attr)
	{
		DEBUG("%s","modify callback: Wrong attribute called");
		return ACS_CC_FAILURE;
	}

	if(cluster_flag)
	{
		DEBUG("%s","modify callback:Modifying cluster config files");

		cmd="hardening";
		char command1[cmd.length()+cluster_attr.length()];
		sprintf(command1, "%s %s" ,cmd.c_str(),cluster_attr.c_str());
		bool rCode=dsdObj->modify_config_file(command1, myErrCode, myErrMsg);
		if (!rCode)
		{
			DEBUG("%s","modify callback: Error while modifying cluster configuration file");
			setExitCode(myErrCode,myErrMsg);
			return ACS_CC_FAILURE;
		}
		else
		{
			DEBUG("%s","modify_config_file: Cluster Configuration files successfully modified");
			//sts="success";
		}

	}

	if(local_flag)
	{
		// Modify configuration file according to parameter passed on active node
		DEBUG("%s","modify callback:Modifying config files on local node");
		cmd="hardening";
		string opt="-a";
		char command2[cmd.length()+attrs.length()];
		sprintf(command2, "%s %s %s" ,cmd.c_str(),opt.c_str(),attrs.c_str());
		bool ret_val = dsdObj->modify_config_file(command2, myErrCode, myErrMsg);
		if (!ret_val)
		{
			DEBUG("%s","modify callback: Linux configuration file not modified on active node.");
			setExitCode(myErrCode,myErrMsg);
			return ACS_CC_FAILURE;
		}
		bool dsdReturnVal=true;
		DEBUG("%s","modify callback: Linux Config file modified successfully on active node");

		// checking the state of passive node
		string cmd1;
		cmd1="/usr/bin/prcstate -l | grep passive | grep -c up";
		char buffer[100]={0},command[400];
		sprintf(command,cmd1.c_str());
		FILE* pipe=popen(command,"r");
		string node_state;
		DEBUG("%s","After pipe open to check passive node status");
		if (!pipe)
		{
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
		//			sts="success";
					enResult = ACS_CC_SUCCESS;
				}
				else
				{
					DEBUG("%s","modify callback: Operations in passive node failed");
					dsdReturnVal=false;
					setExitCode(myErrCode,myErrMsg);
				}
				dsdObj->closeDSDClient();
			}
			else
			{
				DEBUG("%s","modify callback: DSD Client start failed");
				dsdReturnVal=false;
				myErrCode=117;
				myErrMsg="Error Connecting to server";
				setExitCode(myErrCode,myErrMsg);
			}

			if(dsdReturnVal==false)
			{
				enResult = ACS_CC_FAILURE;
			}

		} // if passive node is up
	} // If local flag is set

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

ACS_CC_ReturnType ACS_LCT_CommandHandler_Hardening::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside complete callback", oiHandle, ccbId);
	return ACS_CC_SUCCESS;
}

/*=====================================================================
   ROUTINE: abort
  =====================================================================*/

void ACS_LCT_CommandHandler_Hardening::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside abort callback", oiHandle, ccbId);
	DEBUG("%s","Rolling back file changes");

	string cmd="hardening";
	string attr="-r";
	string node_state;
	char command[cmd.length()+attr.length()];
	sprintf(command, "%s %s" ,cmd.c_str(),attr.c_str());
	int ret=dsdObj->launchCommand(command);
	if (ret==0){
		DEBUG("%s","Rollback successful");
	}
	else{
		DEBUG("%s","Rollback failed");
	}

	DEBUG("%s","Leaving abort callback");
}

/*=====================================================================
   ROUTINE: apply
  =====================================================================*/

void ACS_LCT_CommandHandler_Hardening::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside apply callback", oiHandle, ccbId);
	DEBUG("%s","Committing cluster file changes");

	string cmd="hardening";
	string attr="-c";
	string node_state;
	char command[cmd.length()+attr.length()];
	sprintf(command, "%s %s" ,cmd.c_str(),attr.c_str());
	int ret=dsdObj->launchCommand(command);
	if (ret==0){
		DEBUG("%s","Commit successful");
	}
	else{
		DEBUG("%s","Commit failed");
	}
	DEBUG("%s","Leaving apply callback");
}

/*=====================================================================
   ROUTINE: updateRuntime
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_Hardening::updateRuntime(const char *objName, const char **attrName) 
{
	DEBUG("%s ","Inside updateRuntime callback");
        (void)objName;
        (void)attrName;   

	return ACS_CC_SUCCESS;
}

void ACS_LCT_CommandHandler_Hardening::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG ("%s %ld %s %ld %ld","adminOperationCallback", oiHandle, invocation, p_objName, operationId, paramList);

}


