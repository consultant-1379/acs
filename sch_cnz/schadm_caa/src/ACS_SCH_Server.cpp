//******************************************************************************//
//  NAME
//     ACS_SCH_Server.cpp
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
//  	 2017-07-12  XMALRAO update the class file fr design changes and sanity checks
//     2017-03-04  XBHABHE
//
//  SEE ALSO
//     -
//
//******************************************************************************
#include <algorithm>
#include "ACS_SCH_Server.h"
#include "ACS_SCH_CrmObserver.h"
#include "ACS_SCH_ClusterCPObserver.h"
#include "ACS_SCH_ImUtils.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_SCH_Trace.h"
#include "ACS_SCH_ScalingCommandInterface.h"
#include "ACS_SCH_Util.h"

ACS_SCH_Trace_TDEF(ACS_SCH_SERVER_TRACE); 
/*============================================================================
  ROUTINE: ACS_SCH_Server
 ============================================================================ */
ACS_SCH_Server::ACS_SCH_Server():
 ACS_SCH_logging(0),
  m_CRRHandler(0),
  m_RoleHandler(0),
  m_CrmHandler(0),
  m_CrmObserver(0),
  m_clusterCpObserver(0),
  isMultipleCPSystem(false),
  shutdownService(false)
{
	shutdownEvent = ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SHUTDOWN);
}


/*============================================================================
  ROUTINE: ~ACS_SCH_Server
 ============================================================================ */
ACS_SCH_Server::~ACS_SCH_Server()
{
	if(m_clusterCpObserver != NULL)
	{
		delete m_clusterCpObserver;
		m_clusterCpObserver = NULL;
	}
	if(m_CrmObserver != NULL)
	{
		delete m_CrmObserver;
		m_CrmObserver = NULL;
	}

  if(m_CRRHandler != NULL)
  {
    delete m_CRRHandler;
    m_CRRHandler = NULL;
  }
  if(m_RoleHandler != NULL)
  {
    delete m_RoleHandler;
    m_RoleHandler = NULL;
  }
  if(m_CrmHandler  != NULL)
  {
    delete m_CrmHandler ;
    m_CrmHandler  = NULL;
  }

	immHandle.Finalize();
}

/*============================================================================
  ROUTINE: svc()
 ============================================================================ */
int ACS_SCH_Server::svc()
{
	//Initialize immHandle to perform IMM operations
	if(ACS_CC_SUCCESS != immHandle.Init())
  {
    syslog(LOG_INFO, "[ACS_SCH_Server] Initialization of immHandle is failed");
		return false;
  }
	
	bool isMultipleCPSystem = true;
	ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
	if(isMultipleCPSystem)
	{

		//Modify the rules and roles to make scaling model visible to ldap users
		if(modifyRulesAndRoles()!= ACS_CC_SUCCESS)
		{
			syslog(LOG_INFO,"SCH failed to modify rules and roles for ldap user");
			return false;
		}

		if(startWorkerThreads())
		{
			int eventIndex = ACS_SCH_Event::WaitForEvents(1,&shutdownEvent,0);
			if (0 == eventIndex)
			{
				syslog(LOG_INFO,"[ACS_SCH_Server]received shut down event");
			}

		}
		stopWorkerThreads();
		return true;
	}
	else
	{
		//Adding new rule to hide scaling model for ldapuser
		hideCrMSchema();	

		syslog(LOG_INFO, "acs_schd : Service applicable only to MCP ");
		return false;
	}
}



/*============================================================================
  ROUTINE: startWorkerThreads
 ============================================================================ */
bool ACS_SCH_Server::startWorkerThreads()
{
	bool result = true;
	string output_str;
	ACS_SCH_TRACE((ACS_SCH_SERVER_TRACE,"(%t) [ACS_SCH_Server] In startWorkerThreads function\n"));

	//creating the root element
	if(!CreateCrMObject())
	{
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"CrM objcet Creation FAILED, result is exiting from server"));
		return false;
	}

	//creating role objects
	if(!createRoleObjects())
	{
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Unable to create Role objects, result is exiting from server"));
    		return false;
	}

 	if(!setupCrmObserverIMMCallBacks())
 	{
		return false;
 	}
	  //set OI implementers for IMM call backs
        if(!setupCrmIMMCallBacks())
        {
                result = false;
        }
        else
        {
                if(!setupCRRIMMCallBacks())
                {
                        result = false;
                }
                else
                {
                        if(!setupRoleIMMCallBacks())
                        {
                                result = false;
                        }
                }
        }
        if(result != false)
        {
                if(!setupCPIMMCallBacks())
                {
                        result = false;
                }

        }

  	if(!sanityCheck())
  	{
    		if(!CreateObjects())
    		{
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"CRR and Role Objects creation failed"));
      			return false;
    		}
  	}
  	makeModelAlign();


	//Just to display the previous scaling status
	if(ACS_SCH_Util::getScalingStatus() != COMPLETED)
	{
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"SCH status: SCALING OPERATION IS ONGOING."));
		
	}
	else
	{		
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"SCH status: NO SCALING OPERATIONS."));
	}

	return result;
}

/*============================================================================
  ROUTINE: stopWorkerThreads
 ============================================================================ */
void ACS_SCH_Server::stopWorkerThreads()
{
	ACS_SCH_TRACE((ACS_SCH_SERVER_TRACE,"(%t) [ACS_SCH_Server] In stopWorkerThreads function\n"));
	//ACS_SCH_Util::setScalingStatus(COMPLETED); //Indicates shutdown triggered, but not scaling procedure completed

	if(m_CRRHandler!= NULL)
	{
		if(m_CRRHandler->shutdown() == -1)
		{
			syslog(LOG_INFO, "[ACS_SCH_Server] Returned from shutdown of COmputeResourceRole with -1");	
		}
		m_CRRHandler->wait();
	}
	if(m_RoleHandler!= NULL)
	{
		if(m_RoleHandler->shutdown() == -1)
		{
			syslog(LOG_INFO, "[ACS_SCH_Server] Returned from shutdown of Role with -1");
		}
		m_RoleHandler->wait();
	}

	if(m_CrmHandler!= NULL)
	{
		if(m_CrmHandler->shutdown() == -1)
		{
			syslog(LOG_INFO, "[ACS_SCH_Server] Returned from shutdown of CrM with -1");
		}
		m_CrmHandler->wait();
	}
  if(m_CrmObserver!= NULL)
	{
		if(m_CrmObserver->shutdown() == -1)
		{
			syslog(LOG_INFO, "[ACS_SCH_Server] Returned from shutdown of CrM with -1");
		}
		m_CrmObserver->wait();
	}

	if(m_clusterCpObserver!= NULL)
	{
		if(m_clusterCpObserver->shutdown() == -1)
		{
			syslog(LOG_INFO, "[ACS_SCH_Server] Returned from shutdown of CPCluster with -1");
		}
		m_clusterCpObserver->wait();
	}
}

/*===================================================================
   ROUTINE: setupCRRIMMCallBacks
=================================================================== */
bool ACS_SCH_Server::setupCRRIMMCallBacks()
{
	bool result;
		m_CRRHandler = new ACS_SCH_ComputeResourceRole(ACS_SCH_ImmMapper::CRRClassName,ACS_SCH_ImmMapper::CRRImplName,ACS_APGCC_ONE );
		if(m_CRRHandler!= NULL)
		{
			ACE_INT32 outputValue = m_CRRHandler->activate();
			if (ACTIVATE_FAILED == outputValue) //activation of ACS_SCH_ComputeResourceRole object failed
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Returned from svc method with -1 code"));
				result = false;
			}
			else
			{
				result = true;
			}
		}
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"setupCRRIMMCallBacks is successful"));
	return result;
}

/*===================================================================
   ROUTINE: setupRoleIMMCallBacks
=================================================================== */
bool ACS_SCH_Server::setupRoleIMMCallBacks()
{
	bool result;
		m_RoleHandler = new ACS_SCH_Role(ACS_SCH_ImmMapper::RoleClassName,ACS_SCH_ImmMapper::RoleImplName,ACS_APGCC_ONE );
		if(m_RoleHandler!= NULL)
		{
			ACE_INT32 outputvalue = m_RoleHandler->activate();
			if (ACTIVATE_FAILED == outputvalue)  ///activation of ACS_SCH_Role object failed
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"returned from svc method -1 code"));
				result = false;
			}
			else
			{
				result = true;
			}
		}
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"setupRoleIMMCallBacks is successful"));
	return result;
}

/*===================================================================
   ROUTINE: setupCrmIMMCallBacks
=================================================================== */
bool ACS_SCH_Server::setupCrmIMMCallBacks()
{
	bool result;
		m_CrmHandler = new ACS_SCH_CrmHandler(ACS_SCH_ImmMapper::CrmClassName,ACS_SCH_ImmMapper::CrmImplName,ACS_APGCC_ONE );
		if(m_CrmHandler!= NULL)
		{
			ACE_INT32 outputValue = m_CrmHandler->activate();
			if (ACTIVATE_FAILED == outputValue) //activation of ACS_SCH_CrmHandler object failed
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"returned from svc method with -1 code"));
				result = false;
			}
			else
			{
				result = true;	
			}
		}
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"setupCrmIMMCallBacks() is successful"));
	return result;
}
/*===================================================================
   ROUTINE: setupCrmObserverIMMCallBacks
=================================================================== */
bool ACS_SCH_Server::setupCrmObserverIMMCallBacks()
{
	bool result;
	m_CrmObserver = new ACS_SCH_CrmObserver(ACS_SCH_ImmMapper::classModel,ACS_SCH_ImmMapper::IMPLEMENTER_Model,ACS_APGCC_ONE );
	if(m_CrmObserver!= NULL)
	{
					ACE_INT32 outputValue = m_CrmObserver->activate();
					if (ACTIVATE_FAILED == outputValue) //activation of ACS_SCH_ComputeResourceRole object failed
					{
									ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Returned from svc method with -1 code"));
									result = false;
					}
					else
					{
									result = true;
					}
	}
	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"setupCrmObserverIMMCallBacks is successful"));
	return result;
}

/*===================================================================
   ROUTINE: setupCPIMMCallBacks
=================================================================== */
bool ACS_SCH_Server::setupCPIMMCallBacks()
{
        bool result;
				m_clusterCpObserver = new ACS_SCH_ClusterCPObserver(ACS_SCH_ImmMapper::clustercpModel,ACS_SCH_ImmMapper::CLUSTERCP_OBSERVER,ACS_APGCC_ONE );
				if(m_clusterCpObserver!= NULL)
				{
								ACE_INT32 outputValue = m_clusterCpObserver->activate();
								if (ACTIVATE_FAILED == outputValue) //activation of ACS_SCH_ComputeResourceRole object failed
								{
												ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Returned from svc method with -1 code"));
												result = false;
								}
								else
								{
												result = true;
								}
				}
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"clusterCP observer started is successfully."));
        return result;
}



/*===================================================================
   ROUTINE: CreateObjects
=================================================================== */

bool ACS_SCH_Server::CreateObjects()
{
	vector<std::string> crdnList;

	//Fetch the instances of ComputeResource under Equipment
  immHandle.getClassInstances(ACS_SCH_ImmMapper::classModel.c_str(), crdnList);
  if (crdnList.empty())
  {
	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Compute Resources are not created under Equipment..."));
    return true;  //indicates rdn list is empty
  }

  ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"Getting ME name to create CRR objects..."));
	std::string vAPZ_name=ACS_SCH_Util::getManagedElement();

	//Creating CRR objects for each CR object available under Equipment MO
	for (std::size_t n = 0; n < crdnList.size(); n++)
	{
		/*Fetch the CR value
		 * input: "computeResourceId=AP1A,AxeComputeResourceequipmentId=1"
		 * output: AP1A*/
		std::size_t pos1 = crdnList[n].find("=");      
		std::size_t pos2 = crdnList[n].find(",");
		std::string crvalue = crdnList[n].substr (pos1+1,pos2-(pos1+1));

		if(!crvalue.empty())
		{
			//Forming dn for CRR object
			std::string crrname=ACS_SCH_ImmMapper::crrIdAttr+"="+crvalue+ACS_SCH_ImmMapper::ROLE;

			//Fetch the role name BC_ROLE or CP_ROLE or AP_ROLE
			std::string roleName = ACS_SCH_Util::getRoleName(crvalue);
			if(roleName == "UNKNOWN") 
			{
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"cr_type is other than AP,BC,CP and IPLB. Creation of ComputeResourceRole object ignored"));
			cout<<"value of roleName is "<<roleName<<endl;
			continue;
			}

			//Create role object if object is other than AP, CP and BC
			//For AP,CP and BC role objects are already created
			if( (roleName.find(ACS_SCH_ImmMapper::BCROLE) == std::string::npos) &&
					(roleName.find(ACS_SCH_ImmMapper::CPROLE) == std::string::npos) &&
					(roleName.find(ACS_SCH_ImmMapper::APROLE) == std::string::npos))
			{
				if(!ACS_SCH_Util::createRoleObject(roleName,NON_SCALABLE))
					return false;
			}
			
			//creating CRR object
			createCRRObject(crrname,crvalue,vAPZ_name,roleName);
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Receive NULL value for Role, object creation ignored"));
		}
	}
	return true;
}	

/*===================================================================
   ROUTINE: CreateCRRObject
=================================================================== */
void ACS_SCH_Server::createCRRObject(std::string crrname,std::string crvalue, std::string vAPZ_name, std::string roleName)
{
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"Creating %s...",crrname.c_str()));

		vector<ACS_CC_ValuesDefinitionType> AttrList;
		ACS_CC_ValuesDefinitionType idAttr;
		ACS_CC_ValuesDefinitionType adminStateAttr;
		ACS_CC_ValuesDefinitionType instantiationStateAttr;
		ACS_CC_ValuesDefinitionType operStateAttr;
		ACS_CC_ValuesDefinitionType usesAttr;
		ACS_CC_ValuesDefinitionType providesAttr;

		idAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::crrIdAttr.c_str());
		idAttr.attrType = ATTR_STRINGT;
		idAttr.attrValuesNum = 1;
		char* crrId = const_cast<char*>(crrname.c_str());
		void* value[1]={ reinterpret_cast<void*>(crrId)};
		idAttr.attrValues = value ;
		AttrList.push_back(idAttr);

		adminStateAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::adminStateAttr.c_str());
		adminStateAttr.attrType = ATTR_INT32T;
		adminStateAttr.attrValuesNum = 1;
		int admvalue=UNLOCKED ;   //UNLOCKED;
		void* value1[1] = { reinterpret_cast<void*>(&admvalue)};
		adminStateAttr.attrValues =  value1;
		AttrList.push_back(adminStateAttr);

		instantiationStateAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::instantiateStateAttr.c_str());
		instantiationStateAttr.attrType = ATTR_INT32T;
		instantiationStateAttr.attrValuesNum = 1;
		int insvalue=INSTANTIATED ;   //INSTANTIATED;
		void* value2[1] = {reinterpret_cast<void*>(&insvalue)};
		instantiationStateAttr.attrValues= value2;
		AttrList.push_back(instantiationStateAttr);

		operStateAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::operStateAttr.c_str());
		operStateAttr.attrType = ATTR_INT32T;
		operStateAttr.attrValuesNum = 1;
		int opvalue=OPER_ENABLED;
		void* value3[1] = {reinterpret_cast<void*>(&opvalue)};
		operStateAttr.attrValues =  value3;
		AttrList.push_back(operStateAttr);

		usesAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::usesAttr.c_str());
		usesAttr.attrType = ATTR_NAMET;
		usesAttr.attrValuesNum = 1;
		std::string crdn=ACS_SCH_ImmMapper::CREqual+crvalue+ACS_SCH_ImmMapper::commaEquipment+vAPZ_name;
		char* crDn = const_cast<char*>(crdn.c_str());
		void* usesValue[1]={ reinterpret_cast<void*>(crDn)};
		usesAttr.attrValues = usesValue;
		AttrList.push_back(usesAttr);

		providesAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::providesAttr.c_str());
		providesAttr.attrType = ATTR_NAMET;
		providesAttr.attrValuesNum = 1;
		std::string providesVal=ACS_SCH_ImmMapper::RoleEqual+roleName+ACS_SCH_ImmMapper::commaCrMRoot+vAPZ_name;
		char* provVal= const_cast<char*>(providesVal.c_str());
		void* value4[1]={ reinterpret_cast<void*>(provVal)};
		providesAttr.attrValues = value4;
		AttrList.push_back(providesAttr);

		for(int i=0; i<10; i++)
		{
			if(ACS_CC_SUCCESS != immHandle.createObject(ACS_SCH_ImmMapper::CRRClassName.c_str(),ACS_SCH_ImmMapper::CRRParentDn.c_str(),AttrList))
			{
				int error = immHandle.getInternalLastError();
				if(imm_error_code::ERR_IMM_ALREADY_EXIST == error)
				{
					//Do nothing object already exist and no need to create again.
					break;
				}
				else	
				{
					string err(immHandle.getInternalLastErrorText());
					ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"FAILED in creating %s object with error %s retrying...",crrname.c_str(),err.c_str()));	
					ACE_OS::sleep(1);
				}
			}
			else
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"%s created successfully",crrname.c_str()));
				break;
			}
		}
		AttrList.clear();
}

/*===================================================================
   ROUTINE: createRoleObjects
=================================================================== */
bool ACS_SCH_Server::createRoleObjects()
{
	if(!ACS_SCH_Util::createRoleObject(ACS_SCH_ImmMapper::APROLE,NON_SCALABLE))
			return false;

  if(!ACS_SCH_Util::createRoleObject(ACS_SCH_ImmMapper::CPROLE,NON_SCALABLE))
		return false;

  if(!ACS_SCH_Util::createRoleObject(ACS_SCH_ImmMapper::BCROLE,SCALABLE))
		return false;

	return true;
}

/*===================================================================
   ROUTINE: createRoleObject
=================================================================== 
bool ACS_SCH_Server::createRoleObject(string rolename,int scalability)
{
	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Checking the existence of %s object...",rolename.c_str()));

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
		
	for(int i=0; i<10; i++)
	{
		if(ACS_CC_SUCCESS != immHandle.createObject(ACS_SCH_ImmMapper::RoleClassName.c_str(),ACS_SCH_ImmMapper::CRRParentDn.c_str(), AttrList))
		{
			int error = immHandle.getInternalLastError();
			string err_text(immHandle.getInternalLastErrorText());
			if(imm_error_code::ERR_IMM_ALREADY_EXIST == error)
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"%s object already exists.",rolename.c_str()));
				//return true;
				roleCreated=true;
				break;
			}
			else
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Failed in creating %s object with eror: %s retrying...",rolename.c_str(),err_text.c_str()));
				//return false;
				roleCreated=false;
				ACE_OS::sleep(1);
			}
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"%s object is not present.",rolename.c_str()));
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"%s created successfully",rolename.c_str()));
			roleCreated=true;
			break;
			//return true;
		}
	}

	return roleCreated;
}*/

/*===================================================================
   ROUTINE: CreateCrMObject
=================================================================== */
bool ACS_SCH_Server::CreateCrMObject()
{
	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"Checking the existence of CrM object..."));

	bool result;
	vector<ACS_CC_ValuesDefinitionType> AttrListCrM;
	ACS_CC_ValuesDefinitionType dnAttr;
	ACS_CC_ValuesDefinitionType roleAssignmentAttr;

	dnAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::crmIdAttr.c_str());
	dnAttr.attrType = ATTR_STRINGT;
	dnAttr.attrValuesNum = 1;
	char* crmId = const_cast<char*>(ACS_SCH_ImmMapper::CRRParentDn.c_str());
  void* value[1]={ reinterpret_cast<void*>(crmId)};
  dnAttr.attrValues = value ;
	AttrListCrM.push_back(dnAttr);

	roleAssignmentAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::autoRoleAssignmentAttr.c_str());
	roleAssignmentAttr.attrType = ATTR_INT32T;
	roleAssignmentAttr.attrValuesNum = 1;
	int roleassvalue=OPER_ENABLED ;    
	void* value1[1]={reinterpret_cast<void*>(&roleassvalue)};
	roleAssignmentAttr.attrValues = value1;
	AttrListCrM.push_back(roleAssignmentAttr);

	char * CrMParentDn =0;

	for(int i=0; i<10; i++)
	{
		immHandle.createObject(ACS_SCH_ImmMapper::CrmClassName.c_str(),CrMParentDn, AttrListCrM);

		if(ACS_CC_SUCCESS != immHandle.createObject(ACS_SCH_ImmMapper::CrmClassName.c_str(),CrMParentDn, AttrListCrM))
		{
			int error = immHandle.getInternalLastError();
			string err_text(immHandle.getInternalLastErrorText());
			if(imm_error_code::ERR_IMM_ALREADY_EXIST == error)
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"CrM object is already exists"));
				result= true;
				break;
			}
			else
			{
				ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Unable to create CrM object, IMM error code is:%s retrying...",err_text.c_str()));
				result= false;
				ACE_OS::sleep(1);
			}
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"CrM object is not present."));
			ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"CrM object is created successfully"));
			result=true;
			break;
		}
	}

	AttrListCrM.clear();
	return result;
}

/*============================================================================
*   ROUTINE: sanityCheck() 
* =============================================================================*/
bool ACS_SCH_Server::sanityCheck()
{
	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"Performing Sanity checks..."));

	vector<std::string> crdnList;
	vector<std::string> crrdnList;

  //Fetch the class instances from CR
  immHandle.getClassInstances(ACS_SCH_ImmMapper::classModel.c_str(),crdnList);
	if(crdnList.empty())
  {
    //MI case, no sanity checks required, create all CRR and Role objects
    ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"MI case...No objects under Equipment"));
    return true;
  }

	//Fetch the instances under CRR
  immHandle.getClassInstances(ACS_SCH_ImmMapper::CRRClassName.c_str(), crrdnList);
  if(crrdnList.empty())
  {
    //MI case, no sanity checks required, create all CRR and Role objects
    ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"MI case"));
    return false;
  }

	if(crdnList.size() == crrdnList.size())
	{
		//Not MI case, but both CRR and CR has equal number of objects
		//Now verify whether both are aligned. Ensure that CRR always align with CR
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"CRR objects are already present, Checking the alignment with compute resources..."));
		return (isCRRAlignWithCR(crdnList,crrdnList));
	}
	else
	{
		//count mismatch, all objects under CR should be recreated again under CRM
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Sanity check failed."));
		return false;
	}
}

/*============================================================================
*   ROUTINE: getListOfRoles
* =============================================================================*/
void ACS_SCH_Server::getListOfRoles(vector<string>& listOfdns,set<string> &listOfRoles, bool isCRR)
{
	size_t pos1, pos2;
	string val;

	for(unsigned int i=0; i<listOfdns.size(); i++)
	{
		pos1= listOfdns[i].find("=");

		if(!isCRR)
			pos2=listOfdns[i].find(",");
		else
			pos2=listOfdns[i].find(ACS_SCH_ImmMapper::ROLE.c_str());	
	
		val=listOfdns[i].substr(pos1+1,pos2-(pos1+1));
		listOfRoles.insert(val);
	}
}

/*============================================================================
 * *   ROUTINE: isCRRAlignWithCR
 * * =============================================================================*/
bool ACS_SCH_Server::isCRRAlignWithCR(vector<string>&crdnList, vector<string>& crrdnList)
{
	set<string> listOfCRRoles, listOfCRRRoles;
	getListOfRoles(crdnList,listOfCRRoles,false);
	getListOfRoles(crrdnList,listOfCRRRoles,true);

	if(listOfCRRoles == listOfCRRRoles)
	{
		//Both are aligned
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"Both CRR and CR are aligned, No need to create objects "));
		return true;
	}

	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"Sanity check failed."));
	return false;
}

/*============================================================================
 *   ROUTINE: makeModelAlign
 *=============================================================================*/
void ACS_SCH_Server::makeModelAlign()
{
	ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"Check whether any scaling operation is ongoing..."));

	vector<string> crrdnList;
	immHandle.getClassInstances(ACS_SCH_ImmMapper::CRRClassName.c_str(), crrdnList);

	ACS_SCH_Util::setScalingStatus(COMPLETED);
	for(unsigned int i=0; i<crrdnList.size(); i++)
	{
		//validation not required for AP and CP roles
		if(ACS_SCH_Util::getRoleName(crrdnList[i])!=ACS_SCH_ImmMapper::BCROLE)
				continue;

    ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"validating %s...",crrdnList[i].c_str()));

		STATE currentState,expectedState;
		//Get the current state of CRR
		currentState.set(ACS_SCH_Util::getImmIntAttribute(crrdnList[i],ACS_SCH_ImmMapper::instantiateStateAttr),
										ACS_SCH_Util::getImmIntAttribute(crrdnList[i],ACS_SCH_ImmMapper::operStateAttr),
										ACS_SCH_Util::getImmIntAttribute(crrdnList[i],ACS_SCH_ImmMapper::adminStateAttr));

		//Check whether previous scaling operations are pending on CRR
		if(!isValidationRequired(currentState,crrdnList[i]))
				continue;

		//update the model based on previous instantiation state
		switch(currentState.insStateVal)
		{
			case UNINSTANTIATING:
			case UNINSTANTIATION_FAILED:
			case UNINSTANTIATED: 
				{
					//fill the value of provides in case if it is missed
					string val=ACS_SCH_Util::getImmStringAttribute(crrdnList[i],ACS_SCH_ImmMapper::providesAttr);
					if(val.empty())
					{
						 ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"provides attribute is empty for %s",crrdnList[i].c_str()));
						string providesValue=ACS_SCH_ImmMapper::bcroleDn+ACS_SCH_Util::getManagedElement();
						ACS_SCH_Util::updateImmNameAttribute(crrdnList[i],ACS_SCH_ImmMapper::providesAttr,providesValue);
					}

					ACS_SCH_Util::setScalingStatus(ONGOING);

					expectedState.set(UNINSTANTIATION_FAILED,OPER_ENABLED,UNLOCKED);
					ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"previous scalein procedure is not completed. CLEANUP required for %s ",crrdnList[i].c_str()));
				}
				break;

			case INSTANTIATED:
			case INSTANTIATING:
			case INSTANTIATION_FAILED:
				{
					//check the current cp state
					string clusterDn=ACS_SCH_ImmMapper::clusterCpIdAttr+"="+ACS_SCH_Util::getBcNameFromDn(crrdnList[i])+","+ACS_SCH_ImmMapper::clusterDn;
					int cpState=ACS_SCH_Util::getImmIntAttribute(clusterDn,ACS_SCH_ImmMapper::cpStateAttr);
					
					if(cpState==ACTIVESTATE)
						expectedState.set(INSTANTIATED,OPER_ENABLED,UNLOCKED);
					else if(cpState==PASSIVESTATE)
					{
						ACS_SCH_Util::setScalingStatus(ONGOING);
						expectedState.set(INSTANTIATED,OPER_DISABLED,LOCKED);
						ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"scaling procedure is not completed for %s  Execute CQACI manually or use adminState hook to complete the scaling procedure",crrdnList[i].c_str()));
					}
					else
					{
						ACS_SCH_Util::setScalingStatus(ONGOING);
						expectedState.set(INSTANTIATION_FAILED,OPER_DISABLED,LOCKED);
						ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"previous scaleout procedure is not completed. ROLLBACK required for %s",crrdnList[i].c_str()));
					}
				}
				break;

			default:
				break;
		}
		updateCRRState(currentState,expectedState,crrdnList[i]);
		displayModelState(expectedState);
	}
  ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,"model validation completed "));

}

bool ACS_SCH_Server::isValidationRequired(STATE state,string dn)
{
	if((state.insStateVal==INSTANTIATED) && (state.operStateVal==OPER_ENABLED) && (state.admStateVal==UNLOCKED))
	{
		ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_INFO,"NO OPERATIONS pending on %s",dn.c_str()));
		return false;
	}
	return true;
}

void ACS_SCH_Server::displayModelState(STATE state)
{
	char log_msg[1024]={0};
	sprintf(log_msg,"\n\n Current State is : \n INSTANTIATIONSTATE=%d \n OPERATIIONALSTATE=%d \n ADMINSTATE=%d \n\n",state.insStateVal,state.operStateVal,state.admStateVal);
	string msg(log_msg);
  ACS_SCH_FTRACE((ACS_SCH_SERVER_TRACE,LOG_LEVEL_ERROR,msg.c_str()));
  
}

void ACS_SCH_Server::updateCRRState(STATE currentState,STATE expectedState,string dn)
{
	if(currentState.insStateVal != expectedState.insStateVal)
		ACS_SCH_Util::updateImmIntAttribute(dn,ACS_SCH_ImmMapper::instantiateStateAttr,expectedState.insStateVal);
	
	if(currentState.operStateVal != expectedState.operStateVal)
		ACS_SCH_Util::updateImmIntAttribute(dn,ACS_SCH_ImmMapper::operStateAttr,expectedState.operStateVal);

	if(currentState.admStateVal != expectedState.admStateVal)
		ACS_SCH_Util::updateImmIntAttribute(dn,ACS_SCH_ImmMapper::adminStateAttr,expectedState.admStateVal);
}

ACS_CC_ReturnType ACS_SCH_Server::modifyRulesAndRoles()
{
	string rule28="ruleId=AxeSysm_28,roleId=SystemAdministrator,localAuthorizationMethodId=1";
	string rule31="ruleId=AxeSysm_31,roleId=SystemReadOnly,localAuthorizationMethodId=1";

	if(ACS_SCH_Util::updateImmIntAttribute(rule28,"permission",7)== ACS_CC_SUCCESS)
	{
		return ACS_SCH_Util::updateImmIntAttribute(rule31,"permission",4);
	}

	return ACS_CC_FAILURE;
}

void ACS_SCH_Server::hideCrMSchema()
{
	//SCH creates a new rule to hide the existing AxeScalingM schema
		vector<ACS_CC_ValuesDefinitionType> AttrList;
   	ACS_CC_ValuesDefinitionType idAttr;
  	ACS_CC_ValuesDefinitionType userLabelAttr;
    ACS_CC_ValuesDefinitionType ruleDataAttr;
    ACS_CC_ValuesDefinitionType permissionAttr;

    idAttr.attrName = const_cast<char*>("ruleId");
    idAttr.attrType = ATTR_STRINGT;
    idAttr.attrValuesNum = 1;
    char* ruleId = const_cast<char*>("ruleId=AxeSysm_sch");
    void* value[1]={ reinterpret_cast<void*>(ruleId)};
    idAttr.attrValues = value ;
    AttrList.push_back(idAttr);

    permissionAttr.attrName = const_cast<char*>("permission");
    permissionAttr.attrType = ATTR_INT32T;
    permissionAttr.attrValuesNum = 1;
    int permissionValue=0 ;   //UNLOCKED;
    void* value1[1] = { reinterpret_cast<void*>(&permissionValue)};
    permissionAttr.attrValues =  value1;
    AttrList.push_back(permissionAttr);	

		userLabelAttr.attrName = const_cast<char*>("userLabel");
    userLabelAttr.attrType = ATTR_STRINGT;
    userLabelAttr.attrValuesNum = 1;
    char* userLabelVal = const_cast<char*>("AxeScalingM schema shall not be visible in SCP Nodes");
    void* value2[1]={ reinterpret_cast<void*>(userLabelVal)};
    userLabelAttr.attrValues = value2 ;
    AttrList.push_back(userLabelAttr);

		ruleDataAttr.attrName = const_cast<char*>("ruleData");
    ruleDataAttr.attrType = ATTR_STRINGT;
    ruleDataAttr.attrValuesNum = 1;
    char* ruleDataVal = const_cast<char*>("ManagedElement,SystemFunctions,SysM,Schema=AxeScalingM,*");
    void* value3[1]={ reinterpret_cast<void*>(ruleDataVal)};
    ruleDataAttr.attrValues = value3;
    AttrList.push_back(ruleDataAttr);

		for(int i=0; i<10; i++)
    {
      if(ACS_CC_SUCCESS != immHandle.createObject("Rule","roleId=SystemAdministrator,localAuthorizationMethodId=1",AttrList))
      {
        int error = immHandle.getInternalLastError();
        if(imm_error_code::ERR_IMM_ALREADY_EXIST == error)
        {
          //Do nothing object already exist and no need to create again.
					syslog(LOG_INFO,"Rule is already created to hide the AxeScalingM model");
          break;
        }
        else
        {
					syslog(LOG_ERR,"Failed in creating the rule to hide AxeScalingM model with error code %d. Retrying...",error);
          ACE_OS::sleep(1);
        }
      }
      else
      {
				syslog(LOG_INFO,"Rule to hide AxeScalingM model is successfully created");
        break;
      }
    }
}
