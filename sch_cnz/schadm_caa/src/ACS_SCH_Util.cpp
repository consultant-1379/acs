//******************************************************************************//
//  NAME
//     ACS_SCH_Util.cpp
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
//     2017-07-31  XMALRAO updated the file with common functions that are used
//     2017-04-14  XBHABHE
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include "ACS_SCH_Util.h"
#include "ACS_SCH_ImUtils.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_SCH_ClusterCPObserver.h"
#include "ACS_SCH_Trace.h"
ACS_SCH_Trace_TDEF(ACS_SCH_UTIL_TRACE);
int ACS_SCH_Util::_scaling=COMPLETED;
//string ACS_SCH_Util::_BCName="";
//ACS_SCH_EventHandle ACS_SCH_Util::_scalingShutdownEventHandle = 0;

int ACS_SCH_Util::getScalingStatus()
{
	return _scaling;
}

void ACS_SCH_Util::setScalingStatus(int val)
{
	_scaling=val;
}

/*string ACS_SCH_Util::getCurrentBCName()
{
  return _BCName;
}

void ACS_SCH_Util::setCurrentBCName(string val)
{
  _BCName=val;
}*/

/*============================================================================
*   ROUTINE: getRoleName
*   =============================================================================*/
string ACS_SCH_Util::getRoleName(string CRName)
{
	if(CRName.find("BC") != std::string::npos)
		return ACS_SCH_ImmMapper::BCROLE;

	else if(CRName.find("AP") != std::string::npos)
		return ACS_SCH_ImmMapper::APROLE;

	else if(CRName.find("CP") != std::string::npos)
		return ACS_SCH_ImmMapper::CPROLE;

	else if(CRName.find("IPLB") != std::string::npos)
		return ACS_SCH_ImmMapper::IPLBROLE;

	else
	{
		ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Unknown ROLE identified.created role object with empty value"));
		return string("UNKNOWN");
	}
}

/*============================================================================
*   ROUTINE: isVMScalable 
*   =============================================================================*/
bool ACS_SCH_Util::isVMScalable(string CRName)
{
	if(CRName.find("BC") != std::string::npos)
		return true;

	else
		return false;
}

/*===================================================================
 ROUTINE: getManagedElement
=================================================================== */
std::string ACS_SCH_Util::getManagedElement()
{
	vector<ACS_CC_ValuesDefinitionType> AttrList;
  ACS_CC_ImmParameter paramToFind;
  std::string managedElement("");
  std::vector<std::string> ListManagedElement;
  //retrieve the managed element value
	OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for getManagedElement function."));
    return managedElement;
  }

  if (ACS_CC_SUCCESS == immHandler.getClassInstances("ManagedElement",ListManagedElement))
  {
    std::string managedElementId=ListManagedElement[0];
    paramToFind.attrName = const_cast<char *>(ACS_SCH_ImmMapper::networkManagedElementIdAttr.c_str());
    ACS_CC_ReturnType status = immHandler.getAttribute (managedElementId.c_str(), &paramToFind );
    if(ACS_CC_SUCCESS == status)
    {
      int count=paramToFind.attrValuesNum;
      managedElement=std::string(reinterpret_cast<char*>(paramToFind.attrValues[--count]));
      ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Getting ME name successful"));
    }
    else
    {
      ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Failed to find managed element ID"));
    }
  }

	immHandler.Finalize();
  return managedElement;
}

/*============================================================================
  ROUTINE: getCpIdList
=============================================================================*/
ACS_CS_API_NS::CS_API_Result ACS_SCH_Util::getCpIdList(CpIdentityList & cpIdListOut, std::string const & group) 
{
	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Fetching cp group info"));
	// Retrieve a list of CP identities for all CPs of a group
   ACS_CS_API_IdList idList;
   ACS_CS_API_NS::CS_API_Result const result(CsCpGroupInstance().get()->getGroupMembers(ACS_CS_API_Name(group.c_str()), idList));
   if (ACS_CS_API_NS::Result_Success == result)
   {
      // Covert ACS_CS_API_IdList to CpIdentityList
        for (int i = 0; static_cast<int> (idList.size()) > i; ++i)
        {
            cpIdListOut.push_back(idList[i]);
         }
   }
   return result;
}

/*===================================================================
 *    ROUTINE: getImmStringAttribute
 * =================================================================== */
std::string ACS_SCH_Util::getImmStringAttribute(string dnName, string attributeName)
{
	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Fetching %s value... of %s ",attributeName.c_str(),dnName.c_str()));

  char *attribute = const_cast<char *>(attributeName.c_str());
  char value[128]={0};
  ACS_CC_ReturnType returnCode;
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName = attribute;

  OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm in getImmStringAttribute function."));
  }

  returnCode = immHandler.getAttribute(dnName.c_str(), &ImmParameter);
  if(returnCode == ACS_CC_SUCCESS)
  {
    if(ImmParameter.attrValuesNum)
    {
      strncpy(value,reinterpret_cast<char*>(ImmParameter.attrValues[0]),sizeof(value)-1);
      ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Got %s  value",attributeName.c_str()));
    }
  }
  else
  {
    cout<<"failed in getting the imm string attribute"<<endl;
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Failed in fetching %s of %s with error %s",attributeName.c_str(),dnName.c_str(),immHandler.getInternalLastErrorText()));
  }
  string ret_val(value);

  immHandler.Finalize();
  return ret_val;
}

/*===================================================================
 *    ROUTINE: getImmIntAttribute
 * =================================================================== */
int ACS_SCH_Util::getImmIntAttribute(string dnName, string attributeName)
{
	//ACS_SCH_Logger::log("Fetching "+attributeName+" value... of "+dnName ,SCH_makeOrigin(),LOG_LEVEL_INFO);

  char *attribute = const_cast<char *>(attributeName.c_str());
  ACS_CC_ReturnType returnCode;
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName = attribute;
  int value=0;

  OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for getImmIntAttribute function."));
  }

  returnCode = immHandler.getAttribute(dnName.c_str(), &ImmParameter);
  if(returnCode == ACS_CC_SUCCESS)
  {
    value = *(reinterpret_cast<int*>(ImmParameter.attrValues[0]));
    //ACS_SCH_Logger::log("Got "+attributeName+" value" ,SCH_makeOrigin(),LOG_LEVEL_INFO);
  }
  else
  {
    cout<<"failed in getting the imm int attribute"<<endl;
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Failed in fetching %s of %s with error",attributeName.c_str(),dnName.c_str(),immHandler.getInternalLastErrorText()));
  }

  immHandler.Finalize();
  return value;
}

/*===================================================================
 *    ROUTINE: updateImmNameAttribute
 * =================================================================== */
ACS_CC_ReturnType ACS_SCH_Util::updateImmNameAttribute(string dnName, string attr, string val)
{
	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_Server] updating %s for %s with %s",attr.c_str(),dnName.c_str(),val.c_str())); 

  char *attribute = const_cast<char *>(attr.c_str());
  ACS_CC_ReturnType returnCode;
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName=attribute;
  ImmParameter.attrType=ATTR_NAMET;
  ImmParameter.attrValuesNum=1;
  char* value = const_cast<char*>(val.c_str());
  void* value1[1] = { reinterpret_cast<char *>(value)};
  ImmParameter.attrValues = value1;

  OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for updateImmNameAttribute function."));
    return ACS_CC_FAILURE;
  }

	for(int i=0; i<20; i++)
	{
		returnCode = immHandler.modifyAttribute(dnName.c_str(),&ImmParameter);
		if(returnCode != ACS_CC_SUCCESS)
		{
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Failed in updating %s of %s with error %s",attr.c_str(),dnName.c_str(),immHandler.getInternalLastErrorText()));
			cout<<"Failure in updating attribute"<<attr<<endl;
			cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
			ACE_OS::sleep(1);
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_Server]  %s  updated successfully.",attr.c_str()));
			break;
		}
	}

  immHandler.Finalize();
  return returnCode;
}

/*===================================================================
 *    ROUTINE: updateImmIntAttribute
 * =================================================================== */
ACS_CC_ReturnType ACS_SCH_Util::updateImmIntAttribute(string dnName, string attr, int val)
{
 ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"updating %s attribute od %s",attr.c_str(),dnName.c_str()));

  char *attribute = const_cast<char *>(attr.c_str());
  ACS_CC_ReturnType returnCode;
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName=attribute;
  ImmParameter.attrValuesNum=1;
  void* value[1]={reinterpret_cast<void*>(&val)};
  ImmParameter.attrValues = value;

  OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for updateImmIntAttribute function."));
    return ACS_CC_FAILURE;
  }

  if(strcmp(attr.c_str(),ACS_SCH_ImmMapper::cpTypeAttr.c_str())==0)
    ImmParameter.attrType=ATTR_UINT32T;
  else
    ImmParameter.attrType=ATTR_INT32T;

	for(int i=0; i<20; i++)
	{
		returnCode = immHandler.modifyAttribute(dnName.c_str(),&ImmParameter);
		if(returnCode != ACS_CC_SUCCESS)
		{
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Failed in updating %s of %s with error %s",attr.c_str(),dnName.c_str(),immHandler.getInternalLastErrorText()));
			cout<<"Failure in updating attribute"<<attr<<endl;
			cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
			ACE_OS::sleep(1);
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"%s updated successfully.",attr.c_str()));
			break;
		}
	}

  immHandler.Finalize();
  return returnCode;
}

/*===================================================================
 *    ROUTINE: updateImmStringAttribute
 * =================================================================== */
ACS_CC_ReturnType ACS_SCH_Util::updateImmStringAttribute(string dnName, string attr, string val)
{
  ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_CrmObserver] updating %s for %s with %s",attr.c_str(),dnName.c_str(),val.c_str()));

  char *attribute = const_cast<char *>(attr.c_str());
  ACS_CC_ReturnType returnCode;
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName=attribute;
  ImmParameter.attrType=ATTR_STRINGT;
  ImmParameter.attrValuesNum=1;
  char* value = const_cast<char*>(val.c_str());
  void* value1[1] = { reinterpret_cast<char *>(value)};
  ImmParameter.attrValues = value1;

  OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for updateImmStringAttribute function."));
    return ACS_CC_FAILURE;
  }

	for(int i=0; i<20; i++)
	{
		returnCode = immHandler.modifyAttribute(dnName.c_str(),&ImmParameter);
		if(returnCode != ACS_CC_SUCCESS)
		{
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"Failed in updating %s of %s with error %s",attr.c_str(),dnName.c_str(),immHandler.getInternalLastErrorText()));
			cout<<"Failure in updating attribute"<<attr<<endl;
			cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
			ACE_OS::sleep(1);
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_CrmObserver]  %s updated successfully.",attr.c_str()));
			break;
		}
	}

  immHandler.Finalize();
  return returnCode;
}

/*============================================================================
 *         ROUTINE: updateModelForManualCQACI
 * ============================================================================ */
bool ACS_SCH_Util::updateModelForManualCQACI(string bcName)
{
	//Preparing dn from bcname
	string dnName=ACS_SCH_ImmMapper::crrIdAttr+"="+bcName+ACS_SCH_ImmMapper::ROLE+","+ACS_SCH_ImmMapper::CRRParentDn;

	if(getImmIntAttribute(dnName,ACS_SCH_ImmMapper::operStateAttr)==OPER_ENABLED)
		return false;

  ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Finalizing Scaling Procedure..."));

  //updating the model 
	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"CQACI command executed on %s",bcName.c_str()));
	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Updating the model... "));

	updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::operStateAttr,OPER_ENABLED);
  updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::adminStateAttr,UNLOCKED);
  updateImmIntAttribute(dnName,ACS_SCH_ImmMapper::instantiateStateAttr,INSTANTIATED);

	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Model updated successfully"));

  //UNLOCK the scaling procedure
  ACS_SCH_Util::setScalingStatus(COMPLETED);
	//ACS_SCH_Util::setCurrentBCName("");

	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"SCALEOUT DONE SUCCESSFULLY FOR %s",bcName.c_str()));

	return true;

	//Set the event to close the threads associated
	//ACS_SCH_EventHandle _scalingShutdownEventHandle= ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SCALING);
	/*if (_scalingShutdownEventHandle >= 0)
		    (void) ACS_SCH_Event::SetEvent(_scalingShutdownEventHandle);*/
}

/*============================================================================
 *         ROUTINE: isModelAligned()
 *============================================================================ */
bool ACS_SCH_Util::isModelAligned()
{
  ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Checking whether model is aligned or not..."));

	bool rVal=true;
	OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for updateImmStringAttribute function."));
    return ACS_CC_FAILURE;
  }

	//Get the list of compute resource roles
	std::vector<std::string> crr_dnList;
	ACS_CC_ReturnType returnCode = immHandler.getClassInstances(ACS_SCH_ImmMapper::CRRClassName.c_str(), crr_dnList);
  if(returnCode == ACS_CC_SUCCESS)
  {
		for(unsigned int size=0; size<crr_dnList.size(); size++)
		{
			//check the instantiation state only for BCs, not for AP and CP roles
			if(getRoleName(crr_dnList[size])==ACS_SCH_ImmMapper::BCROLE)
			{
				if(getImmIntAttribute(crr_dnList[size],ACS_SCH_ImmMapper::instantiateStateAttr) == INSTANTIATION_FAILED)
				{
					rVal=false;
			ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Rollback required on %s",crr_dnList[size].c_str()));
					break;
				}
			}				
		}	
	}
	else
	{
		rVal=false;
 	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Unable to get the model state..."));
	}

	immHandler.Finalize();
	return rVal;
}

bool ACS_SCH_Util::isScalingOngoing()
{
	ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Checking whether any scaling procedure is ONGOING..."));

  bool rVal=false;
  OmHandler immHandler;
  if(immHandler.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for updateImmStringAttribute function."));
    return true;
  }

  //Get the list of compute resource roles
  std::vector<std::string> crr_dnList;
  ACS_CC_ReturnType returnCode = immHandler.getClassInstances(ACS_SCH_ImmMapper::CRRClassName.c_str(), crr_dnList);
  if(returnCode == ACS_CC_SUCCESS)
  {
    for(unsigned int size=0; size<crr_dnList.size(); size++)
    {
      //check the instantiation state only for BCs, not for AP and CP roles
      if(getRoleName(crr_dnList[size])==ACS_SCH_ImmMapper::BCROLE)
      {
        if(getImmIntAttribute(crr_dnList[size],ACS_SCH_ImmMapper::operStateAttr) == OPER_DISABLED)
        {
          rVal=true;
	  ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Rollback required on %s",crr_dnList[size].c_str()));
          break;
        }
      }
    }
  }
  else
  {
    rVal=true;
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Unable to get the model state..."));
  }

  immHandler.Finalize();
  return rVal;
}

string ACS_SCH_Util::getBcNameFromDn(string objName)
{
   /*Fetch bc name from id
      * Input: "clusterCpId=BC10_ROLE,cpClusterId=1,logicalMgmtId=1,AxeEquipmentequipmentMId=1"
      * Output: BC10 */
  size_t equal_pos=objName.find("=");
  size_t comma_pos=objName.find(ACS_SCH_ImmMapper::ROLE.c_str());
  string bcName=objName.substr(equal_pos+1,comma_pos-equal_pos-1);

  return bcName;
}

void ACS_SCH_Util::printmtzln()
{
	char buffer[1024] = {0};
        FILE *fd = popen("mtzln -p", "r");
        if (NULL == fd)
        {
		ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Error in popen"));
        }
        fread(buffer,1024, 1, fd);
        ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"mtzln -p output is:\n %s",buffer));
        pclose(fd);
}

/*===================================================================
   ROUTINE: createRoleObject
=================================================================== */
bool ACS_SCH_Util::createRoleObject(string rolename,int scalability)
{
  ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Checking the existence of %s object...",rolename.c_str()));

  vector<ACS_CC_ValuesDefinitionType> AttrList;
  ACS_CC_ValuesDefinitionType idAttr;
  ACS_CC_ValuesDefinitionType scalableAttr;

  idAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::roleIdAttr.c_str());
  idAttr.attrType = ATTR_STRINGT;
  idAttr.attrValuesNum = 1;
  string id= ACS_SCH_ImmMapper::roleIdAttr+"="+rolename;
  char* roleId = const_cast<char*>(id.c_str());
  void* value[1]={ reinterpret_cast<void*>(roleId)};
  idAttr.attrValues = value ;
  AttrList.push_back(idAttr);

  scalableAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::scalabilityAttr.c_str());
  scalableAttr.attrType = ATTR_INT32T;
  scalableAttr.attrValuesNum = 1;
  void* scalableValue[1] = {reinterpret_cast<void*>(&scalability)};
  scalableAttr.attrValues = scalableValue;
  AttrList.push_back(scalableAttr);

  int roleCreated=false;

	OmHandler immHandle;
  if(immHandle.Init() != ACS_CC_SUCCESS)
  {
    ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_DEBUG,"Unable to initialize imm for updateImmStringAttribute function."));
    return false;
  }

  for(int i=0; i<10; i++)
  {
    if(ACS_CC_SUCCESS != immHandle.createObject(ACS_SCH_ImmMapper::RoleClassName.c_str(),ACS_SCH_ImmMapper::CRRParentDn.c_str(), AttrList))
    {
      int error = immHandle.getInternalLastError();
      string err_text(immHandle.getInternalLastErrorText());
      if(imm_error_code::ERR_IMM_ALREADY_EXIST == error)
      {
        ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"%s object already exists.",rolename.c_str()));
        //return true;
        roleCreated=true;
        break;
      }
       else
      {
        ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"Failed in creating %s object with eror: %s retrying...",rolename.c_str(),err_text.c_str()));
        //return false;
        roleCreated=false;
        ACE_OS::sleep(1);
      }
    }
    else
    {
      ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_ERROR,"%s object is not present.",rolename.c_str()));
      ACS_SCH_FTRACE((ACS_SCH_UTIL_TRACE,LOG_LEVEL_INFO,"%s created successfully",rolename.c_str()));
      roleCreated=true;
      break;
      //return true;
    }
  }

	immHandle.Finalize();
  return roleCreated;
}

/*============================================================================
 *         ROUTINE: cpStateObserverThread
 * ============================================================================ */
/*void ACS_SCH_Util::cpStateObserverThread(void* dn)
{
	ACS_SCH_Logger::log("Inside ACS_SCH_Util::cpStateObserverThread function.",SCH_makeOrigin(),LOG_LEVEL_ERROR);

	(void)dn;
	(void) ACS_SCH_Event::CreateEvent(true, false, SCH_Util::EVENT_NAME_SCALING);
	ACS_SCH_EventHandle	shutdown= ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SHUTDOWN);
	_scalingShutdownEventHandle= ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SCALING);

	const int handleLen = 2;
  int *handle=new int[handleLen];
  handle[0]=shutdown;
	handle[1]=_scalingShutdownEventHandle;
	bool exit_thr=false;

  ACS_SCH_ClusterCPObserver Observer;
  if(Observer.start() == ACTIVATE_FAILED)
  {
    ACS_SCH_Logger::log("Adding observer implementer failed so shutting down.....",SCH_makeOrigin(),LOG_LEVEL_ERROR);
  }
  else
  {
    ACS_SCH_Logger::log("cp state observer started successfully.",SCH_makeOrigin(),LOG_LEVEL_ERROR);
		while(!exit_thr)
		{
    	ACS_SCH_Logger::log("Waiting for  scaling event...",SCH_makeOrigin(),LOG_LEVEL_ERROR);
    	int eventIndex = ACS_SCH_Event::WaitForEvents(handleLen,handle,0);
			if(-1 == eventIndex )
      {
        ACS_SCH_Logger::log("Received event error",SCH_makeOrigin(),LOG_LEVEL_ERROR);
        exit_thr=true;
        break;
      }
    	if (0 == eventIndex)
    	{
      	ACS_SCH_Logger::log("Service shutdown event received",SCH_makeOrigin(),LOG_LEVEL_ERROR);
				exit_thr=true; 
				break;
    	}
			if (1 == eventIndex)
      {
        ACS_SCH_Logger::log("Scaling shutdown event received",SCH_makeOrigin(),LOG_LEVEL_ERROR);
        exit_thr=true;
        break;
      }
		}
  }

  delete[] handle;
  Observer.stop();
	ACS_SCH_Event::CloseEvent(_scalingShutdownEventHandle);
  ACS_SCH_Logger::log("cp state observer stopped successfully",SCH_makeOrigin(),LOG_LEVEL_ERROR);
}*/
