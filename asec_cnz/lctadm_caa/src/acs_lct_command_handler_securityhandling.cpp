//******************************************************************************
//
// NAME
//      acs_lct_command_handler_securityhandling.cpp
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
//      Class method implementation for Object Implementor for Security handling 
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//      2012-25-04 XFURULL
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
#include <unistd.h>
//#include <ACS_APGCC_OiHandler.h>
#include <ACS_APGCC_Util.H>
#include <acs_lct_command_handler_securityhandling.h> 
#include <acs_lct_tra.h>
using namespace std;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*=====================================================================
   ROUTINE:ACS_LCT_CommandHandler_securityhandling 
  =====================================================================*/

ACS_LCT_CommandHandler_securityhandling::ACS_LCT_CommandHandler_securityhandling(string szObjName,
		                   string szImpName,
		                   ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( szObjName,szImpName,enScope )
{
	(void)CONFIG_IMM_CLASS_NAME;
	(void)CONFIG_IMM_CLASS_NAME1;
	(void)CONFIG_IMM_CLASS_NAME2;
	DEBUG("%s","Inside securityhandling Command Handler constructor");
        m_TPReactorImpl = 0;
        m_poReactor = 0;
        m_TPReactorImpl = new ACE_TP_Reactor();
        if(m_TPReactorImpl != 0)
        {
                m_poReactor = new ACE_Reactor(m_TPReactorImpl);
        }
}

/*=====================================================================
   ROUTINE: ~ACS_LCT_CommandHandler_securityhandling
  =====================================================================*/

ACS_LCT_CommandHandler_securityhandling::~ACS_LCT_CommandHandler_securityhandling()
{
        DEBUG("%s","Inside securityhandling Command Handler destructor");
        if(m_poReactor != 0)
        {
                delete m_poReactor;
                m_poReactor=0;
                DEBUG("%s","Deleted securityhandling po reactor");
        }

        if(m_TPReactorImpl != 0)
        {
                delete m_TPReactorImpl;
                m_TPReactorImpl=0;
                DEBUG("%s","Deleted securityhandling po reactor implementer");
        }
	//gHardenObj = NULL;
}

/*=====================================================================
   ROUTINE: svc
  =====================================================================*/

ACE_INT32 ACS_LCT_CommandHandler_securityhandling::svc()
{
	ACS_CC_ReturnType errorCode = oiHandler.addObjectImpl(this);
        if ( errorCode == ACS_CC_FAILURE )
        {
		ACS_CC_ReturnType ErrorCode = oiHandler.removeObjectImpl(this);
		if ( ErrorCode == ACS_CC_FAILURE ) {
			DEBUG("%s","No Registered ObjectImplementer for securityhandling");
		}
		else {
			DEBUG("%s","Registered ObjectImplementer for securityhandling Is Unregistered");
		}

		ERROR("%s","Command Handler SVC failed : Internal program error. Exit code: 26");
                // try registering with the oiHandler five times
                // before we give up.
                for (int i=0; i<10; i++){
                        errorCode = oiHandler.addObjectImpl(this);
                        if ( errorCode != ACS_CC_FAILURE ) {
                                DEBUG("%s","securityhandling Command Handler SVC success Eventually");
                                break;
                        }

                        ACE_Time_Value selectTime;
                	selectTime.set(1,0); //sleep for a second
                	while((ACE_OS::poll(0,0,selectTime) == -1) && (errno == EINTR))
                	{
                        	continue;
                	}

                        DEBUG("%s","securityhandling Before Remove ObjectImplementer");
                        errorCode = oiHandler.removeObjectImpl(this);
                        DEBUG("%s %d","securityhandling After Remove ObjectImplementer, errorCode:",errorCode);
                        ERROR("%s","addObjectImpl registration failed.");
                }

                if ( errorCode == ACS_CC_FAILURE )
                        return -1;
        }
        else
        {
		DEBUG("%s","Command Handler SVC success for securityhandling");
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

ACE_INT32 ACS_LCT_CommandHandler_securityhandling::shutdown()
{
        DEBUG("%s","Inside shutdown method of securityhandling command handler");
        m_poReactor->end_reactor_event_loop();
        DEBUG("%s","Ended reactor event loop of securityhandling");
        int rcode=0;
	rcode=oiHandler.removeObjectImpl(this);
	if ( rcode != 0 ){
		for (int i=0; i<10; i++){
			rcode=oiHandler.removeObjectImpl(this);
			if ( rcode == 0 ) {
        			DEBUG("%s %d","Removed securityhandling object implementer eventually rcode:", rcode);
				break;
			}
                ACE_Time_Value selectTime;
                selectTime.set(1,0); //sleep for a second
                while((ACE_OS::poll(0,0,selectTime) == -1) && (errno == EINTR))
                {
                        continue;
                }

			ERROR("%s","removeObjectImpl failed for securityhandling.");
	}
	if ( rcode != 0 )
		return -1;
	}
	DEBUG("%s %d","Removed securityhandling object implementer rcode:", rcode);	
        return 0;
}

/*=====================================================================
   ROUTINE: create
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_securityhandling::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s %ld %ld %s %s %s","Inside  securityhandling create callback", oiHandle, ccbId, className, parentName, attr);
	return ACS_CC_FAILURE;
}

/*=====================================================================
   ROUTINE: deleted
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_securityhandling::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s %ld %ld %s","Inside securityhandling deleted callback", oiHandle, ccbId, objName);
	return ACS_CC_FAILURE;
}

/*=====================================================================
   ROUTINE: modify
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_securityhandling::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
        DEBUG("%s %ld %ld %s","Inside securityhandling modify callback", oiHandle, ccbId, objName);
        int attr_cnt=0;
	bool isWelcomMsgSet=false;
        string strWelcomeMsg;
        string cmd, myErrMsg, attr_name, cluster_attr;

        while ( attrMods[attr_cnt] )
        {
                switch ( attrMods[attr_cnt]->modAttr.attrType )
                {
                        case ATTR_STRINGT:
			{
                                if (attrMods[attr_cnt]->modAttr.attrValuesNum != 0)
				{
					attr_name=attrMods[attr_cnt]->modAttr.attrName;

					if (attr_name.compare("welcomeMessage")==0)
					{
						strWelcomeMsg=reinterpret_cast<const char *>(attrMods[attr_cnt]->modAttr.attrValues[0]);
						
						cluster_attr+=" -w \"" + strWelcomeMsg + "\"";
						DEBUG("%s" "%s","cluster flag set for attribute: ", attr_name.c_str());
						isWelcomMsgSet=true;
					}
				}
                                else
				{
					DEBUG("%s","modify callback: attribute welcomeMessage is not set");
				}
                            	break;
			}
                        default:
                            break;
                }
                attr_cnt++;

        } // End of while

        if(isWelcomMsgSet)
        {
                DEBUG("%s","securityhandling modify callback:Modifying cluster config files");

                cmd="hardening";
        	ACE_INT32 myErrCode=0;
                char command1[cmd.length()+cluster_attr.length()];
                sprintf(command1, "%s %s" ,cmd.c_str(),cluster_attr.c_str());
                bool rCode=dsdObj->modify_config_file(command1, myErrCode, myErrMsg);
                if (!rCode)
                {
                        DEBUG("%s","securityhandling modify callback: Error while modifying cluster configuration file");
                        setExitCode(myErrCode,myErrMsg);
                        return ACS_CC_FAILURE;
                }
                else
                {
                        DEBUG("%s","securityhandling modify_config_file: Cluster Configuration files successfully modified");
                }

        }

        DEBUG ("%s","End of modify callback.");

        return ACS_CC_SUCCESS;

}


/*=====================================================================
   ROUTINE: complete
  =====================================================================*/

ACS_CC_ReturnType ACS_LCT_CommandHandler_securityhandling::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside complete callback", oiHandle, ccbId);
	return ACS_CC_SUCCESS;
}

/*=====================================================================
   ROUTINE: abort
  =====================================================================*/

void ACS_LCT_CommandHandler_securityhandling::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside abort callback", oiHandle, ccbId);
	DEBUG("%s","Rolling back file changes");

        string cmd="hardening";
	string attr="-r";
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

void ACS_LCT_CommandHandler_securityhandling::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s %ld %ld","Inside apply callback", oiHandle, ccbId);
        DEBUG("%s","Committing cluster file changes");

        string cmd="hardening";
        string attr="-c";
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

ACS_CC_ReturnType ACS_LCT_CommandHandler_securityhandling::updateRuntime(const char *objName, const char **attrName) 
{
	DEBUG("%s","Inside ACS_LCT_CommandHandler_securityhandling::updateRuntime callback");
	(void) objName;
	(void) attrName;
	return ACS_CC_SUCCESS;
}

 void ACS_LCT_CommandHandler_securityhandling::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
    DEBUG ("%s","\n---------------------------------------------------\n");
        DEBUG ("%s","           adminOperationCallback invocated           \n");
        DEBUG ("%s","---------------------------------------------------\n");
        DEBUG ("%s %ld %s %ld %ld","adminOperationCallback", oiHandle, invocation, p_objName, operationId, paramList);

}


