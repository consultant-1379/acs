//******************************************************************************//
//  NAME
//     ACS_SCH_ComputeResourceRole.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//
//
//  AUTHOR
//  	 2017-07-31  XMALRAO updated the file with scaling changes
//     2017-04-14  XBHABHE
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include <iostream>
#include <string>
#include <ACS_APGCC_Util.H>
#include <ace/Signal.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ACS_APGCC_CommonLib.h>
#include <sstream>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_CrmObserver.h"
#include "ACS_SCH_ScalingCommandInterface.h"
#include "ACS_SCH_Event.h"

#define FAILED 				0
#define PASSED 				1
#define CLEANUP 				1
#define ROLLBACK 				2
#define SCALEIN 				3
#define SCALEOUT 				4
//#define CPSTATEOBSERVER 5
#define INVALID_ERR_VAL 				1
#define SCALING_ONGOING_VAL 		2
#define INVALID_MO_VAL 					3
#define QUORUM_NOT_EXIST_VAL 		4
#define INVALID_BLADE_COUNT_VAL 5
#define INVALID_BLADE_STATE_VAL 6
#define OP_NOT_VALID_VAL				7 
#define INVALID_BLADE_STATE_TEXT "Expected CP state is NONOP"
#define INVALID_BLADE_COUNT_TEXT "Quorum size is too small"
#define QUORUM_NOT_EXIST_TEXT "CP Quorum is not configured"
#define INVALID_MO_TEXT "Scaling is only for Single-Sided CPs"
#define SCALING_ONGOING_TEXT "Scaling is already in progress"
#define OP_NOT_VALID_TEXT "Operation not allowed"
#define INVALID_ERR_TEXT "Invalid attribute value"
#define DATA_DIR "/data/apz/data/"

//Static varaible declarations
string ACS_SCH_ComputeResourceRole::_threadParam="";
ACS_SCH_Trace_TDEF(ACS_SCH_COMPUTERESOURCEROLE_TRACE);
//! Constructor
ACS_SCH_ComputeResourceRole::ACS_SCH_ComputeResourceRole( std::string ClassName,
		std::string szImpName,
		ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)

{
	m_isObjImplAdded=false; 
	theReportReactorPtr = 0;
	theReportReactorImplPtr = 0;
	theReportReactorImplPtr = new ACE_TP_Reactor();
	if(theReportReactorImplPtr != 0)
	{
		theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
	}
	std::string ImpName=szImpName;
  if(ACS_CC_SUCCESS == immHandle.Init())
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"immHandle initialisation succeded inside ComputeResourceRole.cpp"));
  }
  else
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Initialization of OmHandler FAILED inside ComputeResourceRole.cpp!!!"));
  }

}
//-----------------------------------------------------------------------------
ACS_SCH_ComputeResourceRole::ACS_SCH_ComputeResourceRole( std::string ClassName,
		std::string szImpName,
		ACS_APGCC_ScopeT enScope,  ACE_Thread_Manager *aThrmgr )
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)

{
	this->thr_mgr(aThrmgr);
	theReportReactorPtr = 0;
	theReportReactorImplPtr = 0;
	theReportReactorImplPtr = new ACE_TP_Reactor();
	if(theReportReactorImplPtr != 0)
	{
		theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
	}

	m_isObjImplAdded=false;
	std::string ImpName=szImpName;
  if(ACS_CC_SUCCESS == immHandle.Init())
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"immHandle initialisation succeded inside ComputeResourceRole.cpp"));
  }
  else
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Initialization of OmHandler FAILED inside ComputeResourceRole.cpp!!!"));
  }


}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: ACS_SCH_ComputeResourceRole Destructor
=================================================================== */
ACS_SCH_ComputeResourceRole::~ACS_SCH_ComputeResourceRole()
{
  if(ACS_CC_SUCCESS == immHandle.Finalize())
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"immHandle is finalized inside ComputeResourceRole.cpp"));
  }
  else
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"immHandle finalizing failed inside ComputeResourceRole.cpp"));
  }

	if(theReportReactorPtr != 0)
	{
		delete theReportReactorPtr;
		theReportReactorPtr=0;
	}

	if(theReportReactorImplPtr != 0)
	{
		delete theReportReactorImplPtr;
		theReportReactorImplPtr=0;
	}
  if(theCRRHandler != 0)
  {
    delete theCRRHandler;
    theCRRHandler=0;
  }

}

bool ACS_SCH_ComputeResourceRole::deleteErrorHandlerObject()
{
#if 0
	char myRDN[150];
	ACE_OS::sprintf(myRDN,"%s,%s",ACE_TEXT(ALDA_ERRORHANDLER_RDNOBJECTNAME),ACS_SSU_Common::dnOfSSURoot.c_str());
	if((theErrorRuntimeHandlerPtr->deleteRuntimeObj(myRDN)) == ACS_CC_FAILURE)
	{
		DEBUG ("%s \n","Failure in deleting Error runtime object");
		return false;
	}
#endif
	return true;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: createErrorHandlerObject
=================================================================== */
bool ACS_SCH_ComputeResourceRole::createErrorHandlerObject(ACS_APGCC_CcbId& aCCBId,unsigned int & aErrorId,std::string & aErrorText)
{
	(void) aCCBId;
	(void) aErrorId;
	(void) aErrorText;
	return true;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 ACS_SCH_ComputeResourceRole::svc()
{
	ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In svc function\n")); 
	theCRRHandler = 0;
	theCRRHandler = new acs_apgcc_oihandler_V3();
	try
	{
			if(ACS_CC_SUCCESS != immHandle.Init())
			{
				ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Initialization of OmHandler FAILED inside ComputeResourceRole.cpp!!!"));
				return -1;
			}
			if ( theCRRHandler != 0)
			{
			  for(int i=0;i<10;i++)
			  {
					if(ACS_CC_FAILURE == theCRRHandler->addClassImpl(this, theClassName.c_str()))
					{
									std::string errorText=immHandle.getInternalLastErrorText();
									ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Adding of implementer for CRR failed retrying...%s",errorText.c_str()));
									sleep(0.5);
					}
					else
					{
									ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"Adding of implementer for CRR is successful"));
									m_isObjImplAdded = true;
									break;
					}
				}
				if(m_isObjImplAdded ==false)
				{
								return -1;
				}	
			}
	}

	catch(...)
	{
	}
	if (theReportReactorPtr != 0)
	{
		if (0 == theReportReactorPtr->open(1))
		{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Reactor open failed for ComputeResourceRoleHandler"));
			return -1;

		}
		dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
		theReportReactorPtr->run_reactor_event_loop();
	}

	return 0;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In create function\n"));
	(void)oiHandle;
	(void) ccbId;
	(void) className;
	(void) parentName;
	(void) attr;

	char crrId[64]={0};
	int i=0;

	while( attr[i])
  {
		switch( attr[i]->attrType)
		{
			case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, ACS_SCH_ImmMapper::crrIdAttr.c_str()) == 0 && attr[i]->attrValuesNum)
        {
        	ACE_OS::strcpy(crrId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
        }	
				 break;

      default:
      	break;
    }
    i++;
  }

	vector<string> roles;
	string role_type=ACS_SCH_Util::getRoleName(crrId);
	if(getBCs(roles,role_type))
	{
		string temp(crrId);
		//obtain the value of role, input:computeResourceRoleId=BC0_Role, output:BC0_Role
		size_t pos= temp.find("=");
		string crrVal=temp.substr(pos+1,temp.length()-pos-1);

		//prepare the string  ComputeResourceRole=BC<n>_Role,CrM=1,SysM=1,SystemFunctions=1,ManagedElement=<ME_NAME>
		cout<<"Before dn prepare"<<endl;
		string crrdn=ACS_SCH_ImmMapper::crrIdAttr+"="+crrVal+","+ACS_SCH_ImmMapper::CRRParentDn;
		cout<<"After dn prepare"<<endl;
		roles.push_back(crrdn);

		//Add the new role to isProvides attribute
		bool rVal=false;
		for(int j=0; j<10; j++)
		{
			rVal=updateRoleObject(roles,role_type);
			if(rVal == true)
						break;
			ACE_OS::sleep(1);
		}
	}
	else
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Failed in updating the %s object",role_type.c_str()));
		return ACS_CC_FAILURE;
	}
	return ACS_CC_SUCCESS;
}

//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In deleted function\n"));
	(void) oiHandle;
	(void) ccbId;
	(void) objName;

	//Fetch Role name from dn
	string dn(objName);
	size_t pos1=dn.find("=");
	size_t pos2=dn.find("_Role");
	string bcName=dn.substr(pos1+1,pos2-pos1-1);

	//Get all BCs and remove the one we need to delete from the vector
	string role_type=ACS_SCH_Util::getRoleName(objName);
	vector<string> roles;
	getBCs(roles,role_type);
	int position=-1;
	for(unsigned int i=0; i<roles.size(); i++)
	{
		size_t found=roles[i].find(bcName.c_str());
		if(found!=std::string::npos)	
		{
			position=i;
			break;
		}
	}

	if(position != -1)
	{
		roles.erase(roles.begin()+position);
		//update the isProvided in Role object
		updateRoleObject(roles,role_type);
	}

	return ACS_CC_SUCCESS;
}

//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In modify function\n"));
	(void) oiHandle;
	(void) ccbId;

	cout<<"CRR obj name is"<<objName<<endl;
	string dn(objName);
	
	//check whether modify operation is performed on AP or CP or BC
	size_t pos=dn.find("BC");
	if(pos == std::string::npos)
	{
		//Trying to modify AP/CP MOs, not valid
		strcpy(errorText,INVALID_MO_TEXT);
    setErrorCode(INVALID_MO_VAL);
    return ACS_CC_FAILURE;
	}

	int i=0;
	int adminState_value = LOCKED;
	//int insState_value = INSTANTIATING;
	int previous_insState_value = -1; 
	char providesVal[256]  = {0};
	bool isScaleinTrigger=false;
	bool isadminModified=false;
	int bladeState=0;

	//operator is able to modify only adminState and provides for this class MO
	while( attrMods[i])
	{
		switch( attrMods[i]->modAttr.attrType)
		{
			case ATTR_INT32T: //check it is ATTR_UINT32T
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,ACS_SCH_ImmMapper::adminStateAttr.c_str()) == 0)
				{
					adminState_value = *((int*)(attrMods[i]->modAttr.attrValues[0]));
					int previous_val=ACS_SCH_Util::getImmIntAttribute(dn,ACS_SCH_ImmMapper::adminStateAttr);
					if((previous_val == adminState_value)||(previous_val==SHUTTINGDOWN))
          {
            //operator is trying to modify the admin state with same value
            //or service is trying to update adminstate value in case of failure scenarios
            return ACS_CC_SUCCESS;
          }
					else if((adminState_value==UNLOCKED)&&(ACS_SCH_Util::getImmIntAttribute(dn,ACS_SCH_ImmMapper::operStateAttr)==OPER_ENABLED))
          {
            //cpCluster observer thread is trying to modify the adminState, no validation is required
						//Because in this case first operationalstate is modified
            return ACS_CC_SUCCESS;
          }

					bladeState=getBladeStatus(dn);
					previous_insState_value = ACS_SCH_Util::getImmIntAttribute(dn,ACS_SCH_ImmMapper::instantiateStateAttr);
					if( (adminState_value== LOCKED) || ((adminState_value==UNLOCKED)&&(bladeState!=PASSIVESTATE)&&(previous_insState_value!=INSTANTIATED))||((adminState_value==SHUTTINGDOWN)&&(bladeState!=NONOPSTATE)))
					{
						/*Modifying the adminState to LOCKED is not allowed,LOCKED is allowed only at the time of object creation
						adminState can be UNLOCKED only when blade is in PASSIVE
						adminState can be SHUTTINGDOWN only when blade is in NONOP*/
						ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"Trying to modify admin state with wrong value"));
						strcpy(errorText,INVALID_ERR_TEXT);
						setErrorCode(INVALID_ERR_VAL);
						return ACS_CC_FAILURE;
					}
					isadminModified=true;
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,ACS_SCH_ImmMapper::instantiateStateAttr.c_str()) == 0)
				{
					//insState_value= *((int*)(attrMods[i]->modAttr.attrValues[0]));
          return ACS_CC_SUCCESS;
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,ACS_SCH_ImmMapper::operStateAttr.c_str()) == 0)
        {
          return ACS_CC_SUCCESS;
        }
				break;

			case ATTR_NAMET:
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,ACS_SCH_ImmMapper::providesAttr.c_str()) == 0)
				{
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						//no provides given by operator
						isScaleinTrigger=true;
					}
					else
					{
						string val=ACS_SCH_ImmMapper::bcroleDn+ACS_SCH_Util::getManagedElement();
						ACE_OS::strcpy(providesVal, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
						if(strcmp(providesVal,val.c_str())==0)
						{
							//Service is trying to restore the value. This will come in case scale-in is failed
							ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"provides value restored by service."));
							return ACS_CC_SUCCESS;
						}
						else
						{
							strcpy(errorText,INVALID_ERR_TEXT);
							setErrorCode(INVALID_ERR_VAL);
							return ACS_CC_FAILURE;
						}
					}
				}
				break;

			default:
				break;
		}
		i++;
	}

	//Trying to modify both adminstate and provides, which is not valid
	if( isadminModified && isScaleinTrigger )
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Trying to modify both adminstate and provides."));
    return ACS_CC_FAILURE;
	}

	if(isScaleinTrigger)
  {
    //Scale-in is triggered
    return(triggerProcedure(dn));
  }

	if( (adminState_value == UNLOCKED) && (bladeState == PASSIVESTATE ))
	{
		//Indicate adminstate as unlocked, so that scaleout finalization will be done
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Spawning thread to perform scaleout..."));
		if(spawnScalingProcedureThread(dn,SCALEOUT) == ACS_CC_FAILURE)
  	{
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Unable to spawn procedure thread for scaleout finalization."));
    	return ACS_CC_FAILURE;
  	}
	}

	return ACS_CC_SUCCESS;
}

//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In complete function\n"));
ACS_CC_ReturnType result = ACS_CC_SUCCESS;
(void) oiHandle;
(void) ccbId;
return result;
}

//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: abort
=================================================================== */

void  ACS_SCH_ComputeResourceRole::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In abort function\n"));
(void) oiHandle;
(void) ccbId;
}

//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: apply
=================================================================== */
void ACS_SCH_ComputeResourceRole::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In apply function\n"));
(void) oiHandle;
(void) ccbId;
}

//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::updateRuntime(const char *objName, const char **attrName)
{
(void) attrName;
(void) objName;
return ACS_CC_SUCCESS;
}
//-----------------------------------------------------------------------------
/*===================================================================
 ROUTINE: setErrorText
=================================================================== */
bool ACS_SCH_ComputeResourceRole::setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText)
{
(void) ccbId;
(void) aErrorId;
(void) ErrorText;
return true;
}

/*=====================================================================
 ROUTINE: adminOperationCallback
=====================================================================*/

void ACS_SCH_ComputeResourceRole::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
	const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
ACS_SCH_TRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,"(%t) [ACS_SCH_ComputeResourceRole] In adminOperationCallback function\n"));
(void) oiHandle;
(void) invocation;
(void) p_objName;
(void) operationId;
(void) paramList;
}
/*=====================================================================
 ROUTINE: shutdown
=====================================================================*/

ACE_INT32 ACS_SCH_ComputeResourceRole::shutdown()
{
if( true == m_isObjImplAdded )
{
	try
	{
			if ( theCRRHandler != 0)
			{
				if(ACS_CC_FAILURE == theCRRHandler->removeClassImpl(this, theClassName.c_str()))
				{
					std::string errorText=immHandle.getInternalLastErrorText();
					ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Removal of Implementer failed for CRR %s",errorText.c_str()));
				}
				else
				{
					ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"Removal of Implementer successful for CRR"));
					m_isObjImplAdded = false;
				}
			}
	}
	catch(...)
	{
	}
}

if (theReportReactorPtr != 0)
{
	theReportReactorPtr->end_reactor_event_loop();
}
if (m_isObjImplAdded != false)
	return -1;
else
	return 0;
}


/*===================================================================
*    ROUTINE:getBCs
*=================================================================== */
bool ACS_SCH_ComputeResourceRole::getBCs(vector<string>& roles, string role_type)
{
	string dnName=ACS_SCH_ImmMapper::roleIdAttr+"="+role_type+","+ACS_SCH_ImmMapper::crmIdAttr+"=1";
	char *attribute = const_cast<char *>(ACS_SCH_ImmMapper::isProvidedByAttr.c_str());
	ACS_CC_ReturnType returnCode;
	ACS_CC_ImmParameter ImmParameter;
	ImmParameter.attrName=attribute;

	for(int n=0; n<10; ++n)
	{
		returnCode = immHandle.getAttribute(dnName.c_str(), &ImmParameter);
		cout<<"Return code is : "<<immHandle.getInternalLastError()<<endl;
		if(returnCode == ACS_CC_SUCCESS)
		{
			int numberOfBlades=ImmParameter.attrValuesNum;
			for(int i=0; i<numberOfBlades; i++)
			{
				char temp[128]={0};
				strncpy(temp,reinterpret_cast<char*>(ImmParameter.attrValues[i]),sizeof(temp)-1);
				roles.push_back(temp);
			}
			return true;	
		}
		else if(immHandle.getInternalLastError() == -12)
		{
			cout<<"IPLBs are not yet created"<<endl;
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_ComputeResourceRole] IPLBs are not yet created"));

			if(!ACS_SCH_Util::createRoleObject(role_type,NON_SCALABLE))
				    return false;
			else
				return true;
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_ComputeResourceRole] Failed in getting isProvided attribute of role object"));
			ACE_OS::sleep(1);
		}
	}
	return false;
}

/*===================================================================
 ROUTINE: updateRoleObject
=================================================================== */
bool ACS_SCH_ComputeResourceRole::updateRoleObject(vector<string>& roles, string role_type)
{
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_ComputeResourceRole] updating Role object..."));

	string dnName=ACS_SCH_ImmMapper::roleIdAttr+"="+role_type+","+ACS_SCH_ImmMapper::crmIdAttr+"=1";
	char *attribute = const_cast<char *>(ACS_SCH_ImmMapper::isProvidedByAttr.c_str());
	ACS_CC_ReturnType returnCode;
	ACS_CC_ImmParameter ImmParameter;
	ImmParameter.attrName=attribute;
	ImmParameter.attrType=ATTR_NAMET;
	int size=roles.size();
	ImmParameter.attrValuesNum=size;

	void* value[size];
	cout<<"In update function"<<endl;
	for(int i=0; i<size; i++)
	{
		value[i]=reinterpret_cast<void *>(const_cast<char *>(roles[i].c_str()));
	}
	ImmParameter.attrValues = value;

	returnCode = immHandle.modifyAttribute(dnName.c_str(),&ImmParameter);
	if(returnCode == ACS_CC_FAILURE)
	{
		cout<<"Failed in updating isprovided by in Role object"<<endl;
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_ComputeResourceRole] Failed in updating isProvided attribute of role object"));
		return false;
	}
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"%s role object updated successfully",role_type.c_str()));
	return true;
}

/*===================================================================
 ROUTINE: scaleinPreconditions
=================================================================== */
int ACS_SCH_ComputeResourceRole::scaleinPreconditions(string objName)
{
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"checking the preconditions for scale-in..."));

	//This check is done at the startup of modify,so commented here 
	/*if(!isBladeScalable())
  {
    message=(string)SCALING_ONGOING_TEXT;
    errcode=SCALING_ONGOING_VAL;
    return FAILED;
  }*/

	if(!ACS_SCH_Util::isModelAligned())
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"All scaling operations are not completed, atleast one BC is in INSTANTIATION_FAILED state"));
		strcpy(errorText,OP_NOT_VALID_TEXT);
		setErrorCode(OP_NOT_VALID_VAL);
		return FAILED;
	}
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"NO BC with INSTANTIATION_FAILED state"));

	if(!isQuorumExist())
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Quorum doesn't exist, precondition failed"));
		strcpy(errorText,QUORUM_NOT_EXIST_TEXT);
		setErrorCode(QUORUM_NOT_EXIST_VAL);
		return FAILED;
	}
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Quorum exist."));

	if(getBladeCount()<4)
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Less than 3 baldes, pecondition failed."));
		strcpy(errorText,INVALID_BLADE_COUNT_TEXT);
		setErrorCode(INVALID_BLADE_COUNT_VAL);
		return FAILED;
	}
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"More than 3 blades are available."));

	if(getBladeStatus(objName) != NONOPSTATE)
  {
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Blade is not in NONOP state, precondition failed."));
    strcpy(errorText,INVALID_BLADE_STATE_TEXT);
    setErrorCode(INVALID_BLADE_STATE_VAL);
    return FAILED;
  }
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Blade is NONOP state"));

	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"checking the ongoing scaling operations..."));
	if(ACS_SCH_Util::getScalingStatus()!= COMPLETED)
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"A scaling operation is already ongoing, precondition failed"));
		strcpy(errorText,SCALING_ONGOING_TEXT);
		setErrorCode(SCALING_ONGOING_VAL);
		return FAILED;
	}
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"NO scaling operation is ongoing"));
	
	return PASSED;
}

/*===================================================================
   ROUTINE: getBladeCount
=================================================================== */
int ACS_SCH_ComputeResourceRole::getBladeCount()
{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Checking the number of blades..."));
		int numberOfBlades;
		char *attribute = const_cast<char *>(ACS_SCH_ImmMapper::isProvidedByAttr.c_str());
		ACS_CC_ReturnType returnCode;
		ACS_CC_ImmParameter ImmParameter;
		ImmParameter.attrName=attribute;
		returnCode = immHandle.getAttribute(ACS_SCH_ImmMapper::bcRoleDn.c_str(), &ImmParameter);
		if(returnCode == ACS_CC_SUCCESS)
		{
						numberOfBlades=ImmParameter.attrValuesNum;
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Unable to fetch the number of blades."));
			return -1;
		}
		return numberOfBlades;
}

/*===================================================================
   ROUTINE: isQuorumExist 
=================================================================== */
bool ACS_SCH_ComputeResourceRole::isQuorumExist()
{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"checking the Quorum existence..."));
		bool result= false;
		CpIdentityList cpIdList;
		std::string S_OP_GROUP_NAME = "OPGROUP";
		ACS_SCH_Util::getCpIdList(cpIdList, S_OP_GROUP_NAME);
		if(cpIdList.size() != 0)
		{
						result= true;
		}
		return result;
}

/*===================================================================
   ROUTINE: getBladeStatus
=================================================================== */
int ACS_SCH_ComputeResourceRole::getBladeStatus(string objName)
{
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"checking Blade Status..."));
	/*ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();
	if (cpTable)
	{
		 ACS_CS_API_IdList cpIds;
		 ACS_CS_API_NS::CS_API_Result result;
		 result = cpTable->getCPList(cpIds);

		 //CP State
		 int cpState;
		 size_t pos1=objName.find("=BC");
		 std::string bcNumber=objName.substr(pos1+3,1);
		 int bcnumber = atoi(bcNumber.c_str());
		 result = cpTable->getState(cpIds[bcnumber], cpState);
		 if (result == ACS_CS_API_NS::Result_Success)
		 {
			 	cout<<"Blade state is: "<<cpState<<endl;
				char a[128]={0};
				sprintf(a,"bc%d state is obtained as %d",bcnumber,cpState);
				string temp(a);
				ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,temp.c_str()));
				return cpState;
		 }
		 else
		 {
				cout<<"getState ERROR" <<  endl;
				ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Unable to get Blade Status."));
				return -1;
		 }
	}
	else
		return -1;*/


	//Fetching the value from IMM
	/*Fetch bc name from id
      * Input: "clusterCpId=BC10,cpClusterId=1,logicalMgmtId=1,AxeEquipmentequipmentMId=1"
      * Output: BC10 */
  size_t equal_pos=objName.find("=");
  size_t comma_pos=objName.find(ACS_SCH_ImmMapper::ROLE.c_str());
  string bcName=objName.substr(equal_pos+1,comma_pos-equal_pos-1);	

	//Form dn to obtain cpState
	string dn= ACS_SCH_ImmMapper::clusterCpIdAttr+"="+bcName+","+ACS_SCH_ImmMapper::clusterDn;

	int cpState=ACS_SCH_Util::getImmIntAttribute(dn,ACS_SCH_ImmMapper::cpStateAttr);

	//This piece of code is just for logging purpose
	char a[128]={0};
  sprintf(a,"%s state is obtained as %d",bcName.c_str(),cpState);
  string temp(a);
  ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,temp.c_str()));

	return cpState;
}

/*===================================================================
   ROUTINE: triggerProcedure 
=================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::triggerProcedure(string dnName)
{
	int procedureName;

	switch(ACS_SCH_Util::getImmIntAttribute(dnName,ACS_SCH_ImmMapper::instantiateStateAttr))
    {
      case INSTANTIATION_FAILED:
        //previous scale-out is failed
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"ROLLBACK TRIGGERED on %s",dnName.c_str()));
				procedureName=ROLLBACK;
	      break;

      case UNINSTANTIATION_FAILED:
        //previous scale-in is failed
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"CLEANUP TRIGGERED on %s",dnName.c_str()));
        procedureName=CLEANUP; 
				break;

			case INSTANTIATED:
				//scale-in triggered for the first time
				ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"SCALE IN TRIGGERED on %s",dnName.c_str()));
				if(scaleinPreconditions(dnName)==PASSED)
				{
					//all preconditions met and scale-in can be triggered
					//Spawning a thread to perform the scale-in operation
					ACS_SCH_Util::setScalingStatus(ONGOING);
					procedureName=SCALEIN;	
				}
				else
				{
					ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"SCALE IN FAILED"));
					return ACS_CC_FAILURE;
				}
				break;

			default:
				//In other states of instantiation state, no provides is not valid
				strcpy(errorText,SCALING_ONGOING_TEXT);
				setErrorCode(SCALING_ONGOING_VAL);
				return ACS_CC_FAILURE;
	}

	if(spawnScalingProcedureThread(dnName,procedureName) == ACS_CC_FAILURE)
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Unable to spawn procedure thread"));
		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}


/*===================================================================
 *    ROUTINE: spawnScalingProcedureThread
 *    =================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::spawnScalingProcedureThread(string dn, int procedure)
{
	ACS_SCH_ComputeResourceRole::_threadParam=dn;
	ACE_THR_FUNC fun;
	const ACE_TCHAR* thread_name="schScalingProcedureThread";
	void *threadParam=&dn;

	switch(procedure)
	{
		case CLEANUP:
			fun=ACE_THR_FUNC(&ACS_SCH_ComputeResourceRole::cleanup);
			break;

		case ROLLBACK:
			fun=ACE_THR_FUNC(&ACS_SCH_ComputeResourceRole::rollback);
			break;

		case SCALEIN:
			fun=ACE_THR_FUNC(&ACS_SCH_ComputeResourceRole::scalein);
			break;

		case SCALEOUT:
      fun=ACE_THR_FUNC(&ACS_SCH_ComputeResourceRole::scaleoutFinalize);
      break;

		/*case CPSTATEOBSERVER:
			fun=ACE_THR_FUNC(&ACS_SCH_Util::cpStateObserverThread);
			break;*/

		default:
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"INVALID PROCEDURE TRIGGERED"));
			return ACS_CC_FAILURE;
	}

	//Spawn the thread with corresponding function
  ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(fun,
      threadParam,
      THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED,
      0,
      0,
      ACE_DEFAULT_THREAD_PRIORITY,
      -1,
      0,
      ACE_DEFAULT_THREAD_STACKSIZE,
      &thread_name);

  if (threadHandle == -1)
	{
    return ACS_CC_FAILURE;
  }

	return ACS_CC_SUCCESS;
}

/*===================================================================
 *  *    ROUTINE: rollback
 *   *    =================================================================== */
void ACS_SCH_ComputeResourceRole::rollback(void *dn)
{
	(void)dn;
	string dnName=ACS_SCH_ComputeResourceRole::_threadParam;
	ACS_SCH_ComputeResourceRole::_threadParam="";

	/*//create the boot folder nad load MW dumps to avoid the re creation of CR from CS side
	string output_str;
	string bcName=ACS_SCH_Util::getBcNameFromDn(dnName);

	if(scaling_command_interface::ScalingCommandInterface::instance()->makeDumpBeLoadedByNewBlade(bcName,output_str))
	{
		ACS_SCH_Logger::log("Loading MW successful for "+bcName,SCH_makeOrigin(),LOG_LEVEL_INFO);
	}
	else
	{
		ACS_SCH_Logger::log("Loading MW failed for "+bcName,SCH_makeOrigin(),LOG_LEVEL_INFO);
		ACS_SCH_Logger::log("Output of command: "+output_str ,SCH_makeOrigin(),LOG_LEVEL_INFO);
	}	

	ACE_OS::sleep(10);*/

	//Delete CRR IMM object
  if(deleteCRRObject(dnName)==ACS_CC_SUCCESS)
  {
		//APG cleanup
	  apgCleanup(dnName);

		//In case of multiple saleouts roll back shall be triggered before actual scaling procedure is completed
		//This condition is to handle that scenario
		if(!ACS_SCH_Util::isScalingOngoing())
		{
	  	ACS_SCH_Util::setScalingStatus(COMPLETED);
		}
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"ROLLBACK is COMPLETED."));
  }
  else
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"ROLLBACK IS FAILED."));
		//update provides attribute
		ACS_SCH_Util::updateImmNameAttribute(dnName,ACS_SCH_ImmMapper::providesAttr.c_str(),ACS_SCH_ImmMapper::bcroleDn);
  }
}


/*===================================================================
 *  *    ROUTINE: apgCleanup 
 *   *    =================================================================== */
void ACS_SCH_ComputeResourceRole::apgCleanup(string dnName)
{
	size_t pos1=dnName.find("=");
	size_t pos2=dnName.find("_Role");
	string crvalue=dnName.substr(pos1+1,pos2-pos1-1);
	char command1[1024]={0};

	//1. Delete the BC3 link
	sprintf(command1,"rm -rf %s%s",DATA_DIR,crvalue.c_str());
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Deleting %s directory...",crvalue.c_str()));
	if(system(command1)== -1)
	{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Error while deleting %s directory",crvalue.c_str()));
	}
	//2. Deleting the 
	else
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"%s deleted successfully.",crvalue.c_str()));

		//2. Delete bc<n> directory
		char command2[1024]={0};
		int bcNumber;
		sscanf(crvalue.c_str(),"BC%d",&bcNumber);
		sprintf(command2,"rm -rf %sbc%d",DATA_DIR,bcNumber);
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Deleting bc directory..."));
		if(system(command2)== -1)
		{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Error while deleting bc directory"));
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"bc directory deleted successfully."));

			 //3. Deleting the soft link of blade number
			  ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Deleting the board number softlink..."));
				bcNumber=bcNumber+1;	
				char command3[1024]={0};		
				sprintf(command3,"rm -rf %sboard/m0_0_1_0_s%d", DATA_DIR,bcNumber);
				if(system(command3)==-1)
				{
					ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Error while deleting board number softlink."));
				}
				else
				{
				ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"board number deleted successfully."));
				}
		}
	}
}

/*===================================================================
 *    ROUTINE:deleteCRRObject 
 * =================================================================== */
ACS_CC_ReturnType ACS_SCH_ComputeResourceRole::deleteCRRObject(string objName)
{
		OmHandler immHandler;
		ACS_CC_ReturnType result;
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"Deleting %s ....",objName.c_str()));
		if(immHandler.Init() != ACS_CC_SUCCESS)
		{
			ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Unable to initialize imm."));
			return ACS_CC_FAILURE;
		}

		for(int i=0; i<20; i++)
		{
			result=immHandler.deleteObject(objName.c_str());
			if(result != ACS_CC_SUCCESS)
			{
				string error=immHandler.getInternalLastErrorText();
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Error while deleting object %s",error.c_str()));
				ACE_OS::sleep(1);
			}
			else
			{
				break;
			}
		}

		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"%s deleted successfully",objName.c_str()));
		immHandler.Finalize();
		return result;
}

/*===================================================================
 *  *    ROUTINE: cleanup
 *   *    =================================================================== */
void ACS_SCH_ComputeResourceRole::cleanup(void *dn)
{
	(void)dn;
  string dnName=ACS_SCH_ComputeResourceRole::_threadParam;
  ACS_SCH_ComputeResourceRole::_threadParam="";

  //Delete CRR IMM object
  if(deleteCRRObject(dnName)==ACS_CC_SUCCESS)
  {
    //APG cleanup
    apgCleanup(dnName);
    ACS_SCH_Util::setScalingStatus(COMPLETED);
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"CLEANUP is COMPLETED."));
  }
  else
  {
    ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"CLEANUP IS FAILED."));
    //update provides attribute
		ACS_SCH_Util::updateImmNameAttribute(dnName,ACS_SCH_ImmMapper::providesAttr.c_str(),ACS_SCH_ImmMapper::bcroleDn);
  }
}

/*===================================================================
 *  *    ROUTINE: scalein
 *   *    =================================================================== */
void ACS_SCH_ComputeResourceRole::scalein(void *dn)
{
	(void)dn;
  string dnName=ACS_SCH_ComputeResourceRole::_threadParam;
  ACS_SCH_ComputeResourceRole::_threadParam="";
	bool rVal=false;

	//1. Modify the imm state before executing mml commands
	if(ACS_SCH_Util::updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::instantiateStateAttr,UNINSTANTIATING)== ACS_CC_SUCCESS)	
	{
		if(ACS_SCH_Util::updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::adminStateAttr,SHUTTINGDOWN)==ACS_CC_SUCCESS)
		{
			//2. Execute mml commands to remove blade
			size_t pos1=dnName.find("=");
			size_t pos2=dnName.find(ACS_SCH_ImmMapper::ROLE.c_str());
			string crvalue=dnName.substr(pos1+1,pos2-pos1-1);
			string output_string;
			if(scaling_command_interface::ScalingCommandInterface::instance()->takeBladeOutOfQuorum(crvalue, output_string) == false)
			{
							ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Failed to take blade out of quorum. %s",output_string.c_str()));
			}
			else
			{
				ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Blade removed successfully from quorum. %s",output_string.c_str()));
				//3. Delete CRR object
				if(deleteCRRObject(dnName)==ACS_CC_SUCCESS)
				{
					ACS_SCH_Util::setScalingStatus(COMPLETED);
					ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"SCALEIN PROCEDURE COMPLETED."));
					rVal=true;
				}	
			}
		}
	}

	if(!rVal)
		scaleinFailed(dnName);
}

/*===================================================================
 *  *    ROUTINE: scaleinFailed
 *   *    =================================================================== */
void ACS_SCH_ComputeResourceRole::scaleinFailed(string objName)
{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"SCALEIN PROCEDURE FAILED"));
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Updating model with corresponding values..."));

		ACS_SCH_Util::updateImmIntAttribute(objName,ACS_SCH_ImmMapper::instantiateStateAttr,UNINSTANTIATION_FAILED);
		ACS_SCH_Util::updateImmIntAttribute(objName,ACS_SCH_ImmMapper::adminStateAttr,UNLOCKED);
		string dn=ACS_SCH_ImmMapper::bcroleDn+ACS_SCH_Util::getManagedElement();
		ACS_SCH_Util::updateImmNameAttribute(objName,ACS_SCH_ImmMapper::providesAttr,dn);

		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Model updated successfully."));
}

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void ACS_SCH_ComputeResourceRole::setErrorCode(unsigned int code)
{
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"[ACS_SCH_ComputeResourceRole] Entering setErrorCode function"));
        setExitCode(code,errorText);
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"[ACS_SCH_ComputeResourceRole] Leaving setErrorCode function"));
}

/*===================================================================
 *  *    ROUTINE: scaleoutFinalize 
 *   *    =================================================================== */
void ACS_SCH_ComputeResourceRole::scaleoutFinalize(void *dn)
{
	ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_DEBUG,"adminState UNLOCKED, SCALEOUT finalization starts..."));

	(void)dn;
  string dnName=ACS_SCH_ComputeResourceRole::_threadParam;
  ACS_SCH_ComputeResourceRole::_threadParam="";

	//Execute CQASI command to make blade activie
	size_t pos1=dnName.find("=");
	size_t pos2=dnName.find("_Role");
	string crvalue=dnName.substr(pos1+1,pos2-pos1-1);
	std::string output_str;
	bool isBladeActivated=scaling_command_interface::ScalingCommandInterface::instance()->takeNewBladeToClusterCpStateActive(crvalue, output_str);
	if(isBladeActivated)
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_scaleoutFinalization] output of making the blade active is %s",output_str.c_str()));
		//All the below mentioned operations will be done in cpObserver thread, so it is not required to do here
		//modify the operational state as ENABLED
		/*ACS_SCH_Logger::log("Blade is now in active state",SCH_makeOrigin(),LOG_LEVEL_INFO);
		ACS_SCH_Util::updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::operStateAttr,OPER_ENABLED);
		ACS_SCH_Util::setScalingStatus(COMPLETED);
		ACS_SCH_Logger::log("SCALEOUT DONE SUCCESSFULLY.",SCH_makeOrigin(),LOG_LEVEL_INFO);*/
	}
	else
	{
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_ComputeResourceRole] new blade activation failed"));
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"Output of command: %s",output_str.c_str()));
		ACS_SCH_Util::updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::instantiateStateAttr,INSTANTIATION_FAILED);
		ACS_SCH_FTRACE((ACS_SCH_COMPUTERESOURCEROLE_TRACE,LOG_LEVEL_ERROR,"SCALEOUT FAILED"));
	}
}

