/*
 * ACS_CS_CRRProxyHandler.cpp
 *
 *  Created on: Apr 17, 2017
 *      Author: xlalkak
 */
#include <string>
#include <fstream>
#include <vector>
#include<iterator>


#include "ACS_CS_CRRProxyHandler.h"
#include "ACS_CS_Trace.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

ACS_CS_Trace_TDEF(ACS_CS_CRR_TRACE);

/*============================================================================
	ROUTINE: ACS_CS_CRRProxyHandler
 ============================================================================ */

ACS_CS_CRRProxyHandler::ACS_CS_CRRProxyHandler( std::string ClassName,
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
	ROUTINE: ~ACS_CS_CRRProxyHandler
 ============================================================================ */
ACS_CS_CRRProxyHandler::~ACS_CS_CRRProxyHandler()
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
  if(immCRRWorker != 0)
  {
    delete immCRRWorker;
    immCRRWorker=0;
  }
}


ACE_INT32  ACS_CS_CRRProxyHandler::svc()
{
	immCRRWorker = 0;
	immCRRWorker = new acs_apgcc_oihandler_V3();
	try
	{
			if (immCRRWorker != 0)
			{
				for(int i=0;i<20;i++)
				{
					if(ACS_CC_FAILURE == immCRRWorker->addClassImpl(this, theClassName.c_str()))
					{
							//std::string errorText=immHandle.getInternalLastErrorText();
							ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"Adding of Observer implementer for ComputeResourceRole failed. retrying..."));
							sleep(1);
					}
					else
					{
							ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"CRR observer added successfully."));
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
			ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"CRR observer failed in reactor open function."));
			return -1;
		}
		dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
		theReportReactorPtr->run_reactor_event_loop();
	}
	return 0;
}


ACE_INT32 ACS_CS_CRRProxyHandler::shutdown()
{
	if( true == m_isObjImplAdded )
	{
		try
		{
			if ( immCRRWorker != 0)
			{
				if(ACS_CC_FAILURE == immCRRWorker->removeClassImpl(this, theClassName.c_str()))
				{
					//std::string errorText=immHandle.getInternalLastErrorText();
					ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"Failed in removing CRR observer."));
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"CRR observer removed successfully."));
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


ACS_CC_ReturnType ACS_CS_CRRProxyHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **/*attr*/)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " oiHandle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << className << endl;
	cout << " parent name: " << parentname << endl;

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_CS_CRRProxyHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	if(!update_cs_model(objName))
	{
    ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"Unable to delete internal compute resouce requested by SCH, %s",objName));
    return ACS_CC_FAILURE;
  }

  return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_CS_CRRProxyHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **/*attrMods*/)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_CS_CRRProxyHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "COMPLETECallback " << endl;
	cout << endl;

	return ACS_CC_SUCCESS;
}

void ACS_CS_CRRProxyHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "ABORTCallback " << endl;
	cout << endl;
}

void ACS_CS_CRRProxyHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "APPLYCallback " << endl;
	cout << endl;

}

ACS_CC_ReturnType ACS_CS_CRRProxyHandler::updateRuntime(const char* /*p_objName*/, const char** /*p_attrName*/)
{
	printf("------------------------------------------------------------\n");
	printf("                   updateRuntime called                     \n");
	printf("------------------------------------------------------------\n");

	return ACS_CC_SUCCESS;
}


void ACS_CS_CRRProxyHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**/*paramList*/) {

	printf("------------------------------------------------------------\n");
	printf("                   adminOperationCallback called             \n");
	printf("------------------------------------------------------------\n");

	cout<<"oiHandle   : "<< oiHandle <<endl;
	cout<<"invocation : "<< invocation <<endl;
	cout<<"p_objName  : "<< p_objName <<endl;
	cout<<"operationId: "<< operationId <<endl;
	cout << " Implementing Admin Operation with ID = " << operationId << endl;

}


bool ACS_CS_CRRProxyHandler::update_cs_model(std::string obj)
{
  std::string uuid="";

  //parsing the bcName(in BCn format) from object name
  std::size_t equal_pos = obj.find_first_of("=");
  std::size_t underscore_pos = obj.find_first_of("_");
  string bcName=obj.substr(equal_pos+1,underscore_pos-equal_pos-1);
  string externalCRdn=(string)"computeResourceId=" + bcName + (string)",AxeComputeResourceequipmentId=1";

	//fetch uuid present under external compute resource
  if(ACS_CS_ImUtils::getImmAttributeString(externalCRdn, "uuid", uuid))
  {
    string internalCRdn= (string)"computeResourceId=" + uuid + (string)",crMgmtId=1,AxeEquipmentequipmentMId=1";
    int networkNum;
    if(getNetworkNum(internalCRdn,networkNum) ==false)
    {
      return false;
    }

    //1. Deleting all the networks associated to BC compute resource object
    for(int i=0;i<networkNum;i++)
    {
      stringstream temp_str;
      temp_str<<(i);
      std::string str = temp_str.str();
      string network_dn = (string)"id=network_"+ str+(string)"," + internalCRdn;
      if(ACS_CS_ImUtils::deleteImmObject(network_dn.c_str()) == true)
      {
        ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR," %s deleted successfully",network_dn.c_str()));
      }
      else
      {
        ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"Fail to delete %s with error code %d - %s ",network_dn.c_str(),getInternalLastError(), getInternalLastErrorText()));
        return false;
      }
    }

		//2.Delete the compute resource MO from CrMgmt(ManagedElement=vAPZ088,SystemFunctions=1,AxeFunctions=1,SystemComponentHandling=1,EquipmentM=1,CrMgmt=1)
    if(ACS_CS_ImUtils::deleteImmObject(internalCRdn.c_str()))
    {
      ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"%s ComputeResource object deleted successfully",internalCRdn.c_str()));
    }
    else
    {
      ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"Unable to delete %s error code %d - %s",internalCRdn.c_str(),getInternalLastError(), getInternalLastErrorText()));
      return false;
    }

  }
  else
  {
    return false;
  }

  return true;
}


/*===================================================================
 *    ROUTINE:getNetworkNum
 *=================================================================== */
bool ACS_CS_CRRProxyHandler::getNetworkNum(string dn,int &networkCount)
{
  string networkAttr= "network";
  char *attribute = const_cast<char *>(networkAttr.c_str());
  ACS_CC_ImmParameter ImmParameter;
  ImmParameter.attrName=attribute;
  OmHandler immHandler;

  if(ACS_CC_SUCCESS != immHandler.Init())
  {
    ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"immHandle initialization failed"));
    return false;
  }

  if(immHandler.getAttribute(dn.c_str(), &ImmParameter) != ACS_CC_SUCCESS)
  {
    ACS_CS_FTRACE((ACS_CS_CRR_TRACE,LOG_LEVEL_ERROR,"Unable to fetch the network information!!!. Error %d - %s",immHandler.getInternalLastError(),immHandler.getInternalLastErrorText()));
    immHandler.Finalize();
    return false;
  }

  networkCount=ImmParameter.attrValuesNum;
  immHandler.Finalize();
  return true;
}
