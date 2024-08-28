/*
 * ACS_SCH_CrmObserver.cpp
 *
 *  Created on: Mar 23, 2017
 *      Author: xsabjha
 */
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <boost/filesystem.hpp>
#include "ACS_SCH_CrmObserver.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_Trace.h"
#include "ACS_SCH_ScalingCommandInterface.h"
#include "ACS_CS_API.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

#define DEPLOY_TIME 1
#define SCALING	2
#define SCALING_ONGOING 3

using namespace std;
namespace fs = boost::filesystem; 

//Static varaible declarations
string ACS_SCH_CrmObserver::_threadParam="";


ACS_SCH_Trace_TDEF(ACS_SCH_CRMOBSERVER_TRACE);
/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver
 ============================================================================ */

//! Constructor
ACS_SCH_CrmObserver::ACS_SCH_CrmObserver( std::string ClassName,
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
}

/*============================================================================
        ROUTINE: ~ACS_SCH_CrmObserver
 ============================================================================ */
ACS_SCH_CrmObserver::~ACS_SCH_CrmObserver()
{
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
  if(theCRMHandler != 0)
  {
    delete theCRMHandler;
    theCRMHandler=0;
  }
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::svc
 ============================================================================ */
ACE_INT32  ACS_SCH_CrmObserver::svc()
{
		theCRMHandler = 0;
		theCRMHandler = new acs_apgcc_oihandler_V3();
		try
		{
				if ( theCRMHandler != 0)
				{
				      for(int i=0;i<10;i++)
        				{	
                        			if(ACS_CC_SUCCESS != theCRMHandler->addClassImpl(this, theClassName.c_str()))
                        			{
							ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Adding of Observer implementer for ComputeResource failed with error %d - %s retrying...",getInternalLastError(),getInternalLastErrorText()));
							if (imm_error_code::ERR_IMM_ALREADY_EXIST == getInternalLastError())
							{
								removeImplementer();
							}
                            				sleep(1);
                        			}
                        			else
                        			{
                                        		ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Adding of Observer implementer for ComputeResource is successful"));
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
							ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Reactor open failed for CrmObserver"));
							return -1;

			}
			dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
			theReportReactorPtr->run_reactor_event_loop();
		}

    return 0;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::updateRuntime
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_CrmObserver::updateRuntime(const char* p_objName, const char** p_attrName)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] in updateruntime callback ..!!!\n"));
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	(void) p_objName;
	(void) p_attrName;
	return result;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::adminOperationCallback
 ============================================================================ */
void ACS_SCH_CrmObserver::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] in adminopeartion callback ..!!!\n"));
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::stop
 ============================================================================ */
ACE_INT32 ACS_SCH_CrmObserver::shutdown()
{
	if( true == m_isObjImplAdded )
	{
		try
		{
				if ( theCRMHandler != 0)
				{
					if(removeImplementer() ==0)
					{
						ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Removal of Observer Implementer successful for CrmObserver"));
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
           
/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::removeImplementer
 ============================================================================ */
ACE_INT32 ACS_SCH_CrmObserver::removeImplementer()
{
	 if(ACS_CC_FAILURE == theCRMHandler->removeClassImpl(this, theClassName.c_str()))
         {
                ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Removal of Observer Implementer failed for CrmObserver"));
		return -1;
	 }
	return 0;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::create
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_CrmObserver::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] create callback!!!\n"));
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"compute resource obsserver: create callback received"));

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout<<"OI handle"<<oiHandle<<endl;
	cout<<"CCBID"<<ccbId<<endl;
	cout << " object Name: " << className << endl;
	cout << " parent name: " << parentname << endl;

	char crId[64];
  memset(crId,0,64);
	int i=0;

  while( attr[i])
  {
    cout<<"attr[i]->attrName :"<<attr[i]->attrName<<endl;
    switch( attr[i]->attrType)
    {
      case ATTR_STRINGT:
         if(ACE_OS::strcmp(attr[i]->attrName, ACS_SCH_ImmMapper::computeResourceIdAttr.c_str()) == 0 && attr[i]->attrValuesNum)
         {
           ACE_OS::strcpy(crId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
         }
				 break;

			default:
				break;
    }
		i++;
  }

	/*Fetch bc name from id
 	* Input: computeResourceId=BC0
	* Output: BC0 */
	string temp(crId);
  size_t pos=temp.find("=");
  string bcName=temp.substr(pos+1,temp.length()-pos-1);

	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"%s object created under Equipment",bcName.c_str()));

  if(bcName.empty())
  {
    //This piece of code is to handle the bug in CS, which creates a dummy object with no name.
    ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Got Null value as blade name, creation of object is ignored"));
		return ACS_CC_FAILURE;
  }

  //Fetch the role name BC_ROLE or CP_ROLE or AP_ROLE
  std::string roleName = ACS_SCH_Util::getRoleName(bcName);
  if((roleName == "UNKNOWN"))
  {
         ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Unknown VM is detected . Creation of ComputeResourceRole object ignored."));
	 return ACS_CC_FAILURE;
  }

  string vAPZ_name=ACS_SCH_Util::getManagedElement();
	if(ACS_SCH_Util::isVMScalable(roleName)== false)
	{
		//VM create is NON sclable
		createCRRObject(bcName,vAPZ_name,roleName,DEPLOY_TIME);
		return ACS_CC_SUCCESS;
	}
		
  //verify quorum existence to segregate deploytime and scalingtime
  CpIdentityList cpIdList;
  std::string S_OP_GROUP_NAME = "OPGROUP";
  ACS_SCH_Util::getCpIdList(cpIdList, S_OP_GROUP_NAME);
  if(cpIdList.size() == 0)
  {
  	  //Deployment case and no need to check blade count
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Creating %s during DEPLOY TIME", bcName.c_str()));
      createCRRObject(bcName,vAPZ_name,roleName,DEPLOY_TIME);
  }
	else
	{
		
	 	//spawn a thread to perform scaleout
		if(	ACS_SCH_CrmObserver::_threadParam=="")
			ACS_SCH_CrmObserver::_threadParam=bcName;

		ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Test 0:thread param is %s",_threadParam.c_str()));
		const ACE_TCHAR* thread_name="schScalingProcedureThread";
		ACE_THR_FUNC fun=ACE_THR_FUNC(&ACS_SCH_CrmObserver::updateCRR);

		ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(fun,
		0,
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
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"UNABLE to spawn thread, SCALEOUT IS FAILED. "));		
			return ACS_CC_FAILURE;
		}
	}

	return ACS_CC_SUCCESS;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::deleted
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_CrmObserver::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] In delete callback\n"));

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;


	ACS_CC_ReturnType result = ACS_CC_FAILURE;
	return result;

}
/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::modify
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_CrmObserver::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **/*attrMods*/)
{

	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] In modify callback\n"));
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	return result;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::complete
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_CrmObserver::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] In complete callback\n"));

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "COMPLETECallback " << endl;
	cout << endl;

	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	return result;
}

/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::abort
 ============================================================================ */
void ACS_SCH_CrmObserver::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] In abort callback\n"));

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "ABORTCallback " << endl;
}


/*============================================================================
        ROUTINE: ACS_SCH_CrmObserver::apply
 ============================================================================ */
void ACS_SCH_CrmObserver::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_TRACE((ACS_SCH_CRMOBSERVER_TRACE,"(%t) [ACS_SCH_CrmObserver] In apply callback\n"));

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "APPLYCallback " << endl;
	cout << endl;

}

	
/*============================================================================
 *         ROUTINE: ACS_SCH_CrmObserver::createCRRObject
 *============================================================================ */
void  ACS_SCH_CrmObserver::createCRRObject(string crname, string vAPZ_name,string roleName,int state)
{
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmObserver] creating %s object...",crname.c_str()));

	vector<ACS_CC_ValuesDefinitionType> AttrList;
  ACS_CC_ValuesDefinitionType idAttr;
  ACS_CC_ValuesDefinitionType adminStateAttr;
  ACS_CC_ValuesDefinitionType instantiationStateAttr;
  ACS_CC_ValuesDefinitionType operStateAttr;
  ACS_CC_ValuesDefinitionType usesAttr;
  ACS_CC_ValuesDefinitionType providesAttr;
	int admvalue=LOCKED;
	int insvalue=INSTANTIATING ;
	int opvalue=OPER_DISABLED;

	if(state == DEPLOY_TIME)
	{
		admvalue=UNLOCKED;
		insvalue=INSTANTIATED;
		opvalue=OPER_ENABLED;
	}
	else if(state == SCALING_ONGOING)
	{
		//Multiple VMs scaled out from infrastructure or else VM scaled out when a scaling operation is ongoing
		insvalue=INSTANTIATION_FAILED;
	}

  idAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::crrIdAttr.c_str());
  idAttr.attrType = ATTR_STRINGT;
  idAttr.attrValuesNum = 1;
	std::string crrname=ACS_SCH_ImmMapper::crrIdAttr+"="+crname+ACS_SCH_ImmMapper::ROLE;
  char* crrId = const_cast<char*>(crrname.c_str());
  void* value[1]={ reinterpret_cast<void*>(crrId)};
  idAttr.attrValues = value ;
  AttrList.push_back(idAttr);

  adminStateAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::adminStateAttr.c_str());
  adminStateAttr.attrType = ATTR_INT32T;
  adminStateAttr.attrValuesNum = 1;
  void* value1[1] = { reinterpret_cast<void*>(&admvalue)};
  adminStateAttr.attrValues =  value1;
  AttrList.push_back(adminStateAttr);

  instantiationStateAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::instantiateStateAttr.c_str());
  instantiationStateAttr.attrType = ATTR_INT32T;
  instantiationStateAttr.attrValuesNum = 1;
  void* value2[1] = {reinterpret_cast<void*>(&insvalue)};
  instantiationStateAttr.attrValues= value2;
  AttrList.push_back(instantiationStateAttr);

  operStateAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::operStateAttr.c_str());
  operStateAttr.attrType = ATTR_INT32T;
  operStateAttr.attrValuesNum = 1;
  void* value3[1] = {reinterpret_cast<void*>(&opvalue)};
  operStateAttr.attrValues =  value3;
  AttrList.push_back(operStateAttr);

	usesAttr.attrName = const_cast<char*>(ACS_SCH_ImmMapper::usesAttr.c_str());
  usesAttr.attrType = ATTR_NAMET;
  usesAttr.attrValuesNum = 1;
	std::string crdn=ACS_SCH_ImmMapper::CREqual+crname+ACS_SCH_ImmMapper::commaEquipment+vAPZ_name;;
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

	OmHandler immHandler;
  if(ACS_CC_SUCCESS != immHandler.Init())
  {
    ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Initialization of OmHandler FAILED inside createCRRObject function!!!"));
  }

	for(int i=0; i<10; i++)
	{
		if(ACS_CC_FAILURE == immHandler.createObject(ACS_SCH_ImmMapper::CRRClassName.c_str(),ACS_SCH_ImmMapper::CRRParentDn.c_str(),AttrList))
		{
			string err=immHandler.getInternalLastErrorText();
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Failed in creating %s object with %s retrying...",crname.c_str(),err.c_str()));

			if(err == "ERR_ELEMENT_ALREADY_EXIST")
			{
				ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Ignore the creation of %s object.",crname.c_str(),err.c_str()));
				break;
			}

			ACE_OS::sleep(1);
		}
		else
		{
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"%s created successfully.",crname.c_str()));
			break;
		}
	}
  AttrList.clear();
	immHandler.Finalize();
}

/*===================================================================
 *    ROUTINE:getBCsCount
 *=================================================================== */
bool ACS_SCH_CrmObserver::getBCsCount(vector<string>& bcRoles,int &bladeCount)
{
  char *attribute = const_cast<char *>(ACS_SCH_ImmMapper::isProvidedByAttr.c_str());
  ACS_CC_ReturnType returnCode;
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName=attribute;
	
	OmHandler immHandler;
	if(ACS_CC_SUCCESS != immHandler.Init())
  {
    ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Initialization of OmHandler FAILED inside getBCsCount function!!!"));
  }
	
	returnCode = immHandler.getAttribute(ACS_SCH_ImmMapper::bcRoleDn.c_str(), &ImmParameter);
	if(returnCode == ACS_CC_SUCCESS)
  {
		bladeCount=ImmParameter.attrValuesNum;

    for(int i=0; i<bladeCount; i++)
    {
      char temp[128]={0};
      strncpy(temp,reinterpret_cast<char*>(ImmParameter.attrValues[i]),sizeof(temp)-1);
			bcRoles.push_back(temp);
    }	
	}
	else
	{
		ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_CrmObserver] Failed in getting isProvided attribute of role object"));
	}
	immHandler.Finalize();
	return true;
}

/*===================================================================
   ROUTINE: getTrafficLeaderBcName
 =================================================================== */
void ACS_SCH_CrmObserver::getTrafficLeaderBcName (std::string& trafficLeaderBcName)
{
    ACS_CS_API_NS::CS_API_Result csResult;
    trafficLeaderBcName.clear();
    CPID cpId(0);
    csResult = ACS_CS_API_NetworkElement::getTrafficLeader(cpId);
    if (ACS_CS_API_NS::Result_Success == csResult)
    {
      std::ostringstream sstream;
      sstream << "BC" << cpId;
      trafficLeaderBcName = sstream.str();
    }
}

/*===================================================================
   ROUTINE: fileExists
 =================================================================== */
bool ACS_SCH_CrmObserver::fileExists(const string& fileName)
{
    string tName(fileName);
    bool result=false;

        try
        {
            fs::path p(tName.c_str());
            result = fs::exists(p);
        }
        catch (fs::filesystem_error e)
        {
	    ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Failed to get information for file %s", tName.c_str(),e.what()));
        }
	return result;
}

/*===================================================================
   ROUTINE: copyFile
 =================================================================== */
void ACS_SCH_CrmObserver::copyFile(const string& source, const string& dest)
{
    try
    {
        fs::path pfrom(source.c_str());
        fs::path psource(dest.c_str());
        fs::copy_file(pfrom,psource);
    }
    catch (fs::filesystem_error e)
    {
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Failed to copy file %s",source.c_str(),e.what()));
    }
}

/*===================================================================
   ROUTINE: updatebootOptsFile
 =================================================================== */
void ACS_SCH_CrmObserver::createbootOptsFile(string trafficLeaderBcName,string bcName)
{
    const std::string bootopts_file_format = "/data/apz/data/kernel_opts/bootopts_";

    // Convert upper to lower strings 
    transform(trafficLeaderBcName.begin(), trafficLeaderBcName.end(), trafficLeaderBcName.begin(), ::tolower);
    transform(bcName.begin(), bcName.end(), bcName.begin(), ::tolower);

    // Formation of trafiicleader bootopts file and new blade bootopts file 
    string trafficLeader_bootopts_file = bootopts_file_format+trafficLeaderBcName;
    string scaledout_bootopts_file =  bootopts_file_format+bcName;

    if ( fileExists(trafficLeader_bootopts_file))
    {
       copyFile(trafficLeader_bootopts_file, scaledout_bootopts_file);
    }
    else
    {
      ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"bootopts file not exist for trafficleader: %s", trafficLeader_bootopts_file.c_str()));
    }

    if ( fileExists(scaledout_bootopts_file))
    {
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"bootopts file successfully created for %s", bcName.c_str()));
    }	
}

/*===================================================================
 *  *    ROUTINE: scaleout
 *   *    =================================================================== */
void ACS_SCH_CrmObserver::scaleout(string bcName,string crrDn)
{
	string output_str;
	bool result=false;
	
	ACS_SCH_Util::printmtzln();

        if(scaling_command_interface::ScalingCommandInterface::instance()->executeCACLP(output_str))
        {
                ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Caclp: output is: %s",output_str.c_str()));
        }

	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Loading Mw for %s ...",bcName.c_str()));
	if(scaling_command_interface::ScalingCommandInterface::instance()->makeDumpBeLoadedByNewBlade(bcName, output_str))
	{
		ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Loading MW successful for %s",bcName.c_str()));
		if(scaling_command_interface::ScalingCommandInterface::instance()->introduceNewBladeIntoQuorum(bcName, output_str))
		{
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Output of command: %s",output_str.c_str()));
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"%s is added to quorum successfully",bcName.c_str()));
			ACS_SCH_Util::updateImmIntAttribute(crrDn,ACS_SCH_ImmMapper::instantiateStateAttr,INSTANTIATED);
			result=true;
		}
	}
	if(!result)
	{
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Output of command: %s",output_str.c_str()));
			ACS_SCH_Util::updateImmIntAttribute(crrDn,ACS_SCH_ImmMapper::instantiateStateAttr,INSTANTIATION_FAILED);
			ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"SCALEOUT IS FAILED"));
	}
}

/*===================================================================
*    ROUTINE: updateCRR 
* =================================================================== */
void ACS_SCH_CrmObserver::updateCRR(void* param)
{
	(void)param;

  string bcName=ACS_SCH_CrmObserver::_threadParam;
	ACS_SCH_CrmObserver::_threadParam="";


	//Fetch the managed element name
  string vAPZ_name=ACS_SCH_Util::getManagedElement();

	//Fetch the role name BC_ROLE or CP_ROLE or AP_ROLE
	string roleName = ACS_SCH_Util::getRoleName(bcName);

	performScaleout(bcName,vAPZ_name,roleName);
}

/*===================================================================
*    ROUTINE: performScaleout
* =================================================================== */
void ACS_SCH_CrmObserver::performScaleout(string bcName,string vAPZ_name,string roleName)
{
	if(scaleoutPreconditions())
	{
		ACS_SCH_Util::setScalingStatus(ONGOING);
    ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"SCALEOUT IS TRIGGERED FOR %s",bcName.c_str()));

		//Creating the CRR object
    createCRRObject(bcName,vAPZ_name,roleName,SCALING);

    //update the created CRR with traffic leader info
    string crrDn = ACS_SCH_ImmMapper::crrIdAttr+"="+bcName+ACS_SCH_ImmMapper::ROLE+","+ACS_SCH_ImmMapper::CRRParentDn;
    string trafficLeaderBcName="";
    getTrafficLeaderBcName(trafficLeaderBcName);
    string trafficLeaderBcDn=ACS_SCH_ImmMapper::clusterCpIdAttr+"="+trafficLeaderBcName+","+ACS_SCH_ImmMapper::clusterDn;
    string apzSystemValue=ACS_SCH_Util::getImmStringAttribute(trafficLeaderBcDn,ACS_SCH_ImmMapper::apzSystemAttr.c_str());
    int cpTypeValue=0;
    cpTypeValue=ACS_SCH_Util::getImmIntAttribute(trafficLeaderBcDn,ACS_SCH_ImmMapper::cpTypeAttr.c_str());

		if(trafficLeaderBcName.empty() || apzSystemValue.empty() || (cpTypeValue==0))
		{
				ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"Unable to get the traffic leader data. Scaling can't be performed.%s",bcName.c_str()));
				ACS_SCH_Util::updateImmIntAttribute(crrDn,ACS_SCH_ImmMapper::instantiateStateAttr,INSTANTIATION_FAILED);
				ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_ERROR,"SCALEOUT IS FAILED "));
		}
		else
		{
			//update cpCluster attribute with new values
			std::string clusterBCDn=ACS_SCH_ImmMapper::clusterCpIdAttr+"="+bcName+","+ACS_SCH_ImmMapper::clusterDn;
			ACS_SCH_Util::updateImmStringAttribute( clusterBCDn,ACS_SCH_ImmMapper::apzSystemAttr.c_str(),apzSystemValue);
			ACS_SCH_Util::updateImmIntAttribute( clusterBCDn,ACS_SCH_ImmMapper::cpTypeAttr.c_str(),cpTypeValue);

			//Loading MW and add blade to quorum
			scaleout(bcName,crrDn);

			//Create bootopts file for newly added blade
                        createbootOptsFile(trafficLeaderBcName,bcName);
		}
  }
  else
  {
    createCRRObject(bcName,vAPZ_name,roleName,SCALING_ONGOING);
	}
}

/*===================================================================
*    ROUTINE: scaleoutPreconditions
* =================================================================== */
bool ACS_SCH_CrmObserver::scaleoutPreconditions()
{
   ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmObserver] Executing preconditions for scaleout..."));

	//condition-1: No scaling operation ongoing
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Checking the ongoing scaling operations..."));
	if( ACS_SCH_Util::getScalingStatus()== ONGOING)
	{
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Another scaling operation is ongoing..."));
		return false;
	}
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"No scaling operation ongoing."));

	//condition-2: Balde count should be less than 64
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Checking the blade count..."));
	int bladeCount;
	vector<string> bcRoles;
	getBCsCount(bcRoles,bladeCount);
	if(!(bladeCount<64))
	{
		ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Blade count is already reached 64.."));
		return false;
	}
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Number of BCs are less than 64."));

	//condition-3: No CRR with state INSTANTIATION_FAILED
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Checking the instantiation states of all BCs."));
	if(!ACS_SCH_Util::isModelAligned())
	{
		ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"Atleast one CRR is in INSTANTIATION_FAILED state."));
		return false;
	}
	ACS_SCH_FTRACE((ACS_SCH_CRMOBSERVER_TRACE,LOG_LEVEL_INFO,"All preconditions are met."));

	return true;
}
