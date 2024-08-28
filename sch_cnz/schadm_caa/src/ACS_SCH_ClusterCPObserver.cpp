/*
 * ACS_SCH_ClusterCPObserver.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: xmalrao
 */
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include "ACS_SCH_ClusterCPObserver.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_Trace.h"

ACS_SCH_Trace_TDEF(ACS_SCH_CLUSTERCPOBSERVER_TRACE);
/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver
 ============================================================================ */

ACS_SCH_ClusterCPObserver::ACS_SCH_ClusterCPObserver( std::string ClassName,
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
        ROUTINE: ~ACS_SCH_ClusterCPObserver
 ============================================================================ */

ACS_SCH_ClusterCPObserver::~ACS_SCH_ClusterCPObserver()
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
  if(theCPHandler != 0)
  {
    delete theCPHandler;
    theCPHandler=0;
  }
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::svc
 ============================================================================ */

ACE_INT32  ACS_SCH_ClusterCPObserver::svc()
{
		theCPHandler = 0;
		theCPHandler = new acs_apgcc_oihandler_V3();
		try
		{
			if ( theCPHandler != 0)
			{
				for(int i=0;i<10;i++)
                                {
                                        if(ACS_CC_SUCCESS != theCPHandler->addClassImpl(this, theClassName.c_str()))
                                        {
                                	        ACS_SCH_FTRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,LOG_LEVEL_ERROR,"Adding of Observer implementer for Cluster cp failed with error %d - %s retrying...",getInternalLastError(),getInternalLastErrorText()));
                                                if (imm_error_code::ERR_IMM_ALREADY_EXIST == getInternalLastError())
						{
							removeImplementer();
						}
						sleep(1);
                                        }
                                        else
                                        {
                                        	ACS_SCH_FTRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,LOG_LEVEL_INFO,"Adding of Observer implementer for Cluster cp is successful"));
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
						ACS_SCH_FTRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,LOG_LEVEL_ERROR,"Reactor open failed for Clustercpobserver"));
						return -1;
				}
				dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
				theReportReactorPtr->run_reactor_event_loop();
		}

		return 0;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::updateRuntime
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_ClusterCPObserver::updateRuntime(const char* p_objName, const char** p_attrName)
{
	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] in updateruntime callback ..!!!\n"));
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	(void) p_objName;
	(void) p_attrName;
	return result;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::adminOperationCallback
 ============================================================================ */
void ACS_SCH_ClusterCPObserver::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] in adminopeartion callback ..!!!\n"));
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::shutdown
 ============================================================================ */

ACE_INT32 ACS_SCH_ClusterCPObserver::shutdown()
{
	if( true == m_isObjImplAdded )
	{
			try
			{
				if ( theCPHandler != 0)
				{
					if(removeImplementer() ==0)
					{
						ACS_SCH_FTRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,LOG_LEVEL_INFO,"Removal of Observer Implementer successful for Cluster cp"));
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
        ROUTINE: ACS_SCH_ClusterCPObserver::removeImplementer
 ============================================================================ */
ACE_INT32 ACS_SCH_ClusterCPObserver::removeImplementer()
{
         if(ACS_CC_FAILURE == theCPHandler->removeClassImpl(this, theClassName.c_str()))
         {
                ACS_SCH_FTRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,LOG_LEVEL_ERROR,"Removal of Observer Implementer failed for ClusterCp observer"));
                return -1;
         }
        return 0;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::create
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_ClusterCPObserver::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
{
	(void)oiHandle;
	(void)ccbId;
	(void)attr;
	(void)className;
	(void)parentname;

	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] create callback!!!\n"));

	/*cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " object Name: " << className << endl;
	cout << " parent name: " << parentname << endl;*/

		
	return ACS_CC_SUCCESS;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::deleted
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_ClusterCPObserver::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] In delete callback\n"));

	/*cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;*/


	ACS_CC_ReturnType result = ACS_CC_FAILURE;
	return result;

}
/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::modify
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_ClusterCPObserver::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)ccbId;

	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] In modify callback\n"));
	/*cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;*/

	//It is not required to observe on cpState in case if there is no scaling operation
	/*if(ACS_SCH_Util::getScalingStatus()== COMPLETED)
	{
		return ACS_CC_SUCCESS;
	}*/

	int cpState,i=0;

	while( attrMods[i])
  {
    switch( attrMods[i]->modAttr.attrType)
    {
      case ATTR_INT32T:
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,ACS_SCH_ImmMapper::cpStateAttr.c_str()) == 0)
        {
					cpState=*((int*)(attrMods[i]->modAttr.attrValues[0]));
				}
				break;

			default:
				break;
		}
		i++;
	}

	if(cpState==ACTIVESTATE)
	{
		 /*Fetch bc name from id
   		* Input: "clusterCpId=BC10,cpClusterId=1,logicalMgmtId=1,AxeEquipmentequipmentMId=1"
   		* Output: BC10 */
		  string dn(objName);
		  size_t equal_pos=dn.find("=");
		  size_t comma_pos=dn.find(",");
  		string bcName=dn.substr(equal_pos+1,comma_pos-equal_pos-1);

			ACS_SCH_Util::updateModelForManualCQACI(bcName);
	
			/*string currentScalingBC=ACS_SCH_Util::getCurrentBCName();
			size_t found=currentScalingBC.find(bcName.c_str());
			if (found!=std::string::npos)
			{
				//Scaling procedure is completed for the BC
				ACS_SCH_FTRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_ClusterCPObserver]  cpstate is modified to active for scaling blade."));
				ACS_SCH_Util::updateModelForManualCQACI(bcName);
			}*/
	}

	return ACS_CC_SUCCESS;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::complete
 ============================================================================ */
ACS_CC_ReturnType ACS_SCH_ClusterCPObserver::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] In complete callback\n"));

	/*cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "COMPLETECallback " << endl;
	cout << endl;*/

	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	return result;
}

/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::abort
 ============================================================================ */
void ACS_SCH_ClusterCPObserver::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] In abort callback\n"));

	/*cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "ABORTCallback " << endl;*/
}


/*============================================================================
        ROUTINE: ACS_SCH_ClusterCPObserver::apply
 ============================================================================ */
void ACS_SCH_ClusterCPObserver::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	ACS_SCH_TRACE((ACS_SCH_CLUSTERCPOBSERVER_TRACE,"(%t) [ACS_SCH_ClusterCPObserver] In apply callback\n"));

	/*cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "APPLYCallback " << endl;
	cout << endl;*/

}
