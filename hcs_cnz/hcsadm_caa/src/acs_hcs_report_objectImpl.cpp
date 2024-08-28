/*=================================================================== */
/**
   @file acs_hcs_report_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XMALRAO       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_report_objectImpl.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"
#include "acs_hcs_transferqueue.h"
#include "acs_hcs_global_ObjectImpl.h"
#include <ACS_CS_API.h>

using namespace std;

string acs_hcs_report_ObjectImpl::completeReportRdn = "";
string acs_hcs_report_ObjectImpl::tqRdn = "";
bool acs_hcs_report_ObjectImpl::tqCreated = false;
//bool tq_modify_flag = false ;

/*===================================================================
   ROUTINE: acs_hcs_report_ObjectImpl
=================================================================== */
acs_hcs_report_ObjectImpl::acs_hcs_report_ObjectImpl(string ClassName,string szImpName,ACS_APGCC_ScopeT enScope ) 
:acs_apgcc_objectimplementereventhandler_V3(  ClassName,szImpName,enScope ), theClassName(ClassName)
{
	DEBUG("%s"," Entering acs_hcs_report_ObjectImpl::acs_hcs_report_ObjectImpl");
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	m_poTPReactorImpl = 0;
	m_poReactor = 0;
	m_poTPReactorImpl = new ACE_TP_Reactor();

        if( m_poTPReactorImpl == 0 )
        {
                ERROR("%s", "Memory allocation failed for m_poTPReactorImpl");
        }

        m_poReactor = new ACE_Reactor(m_poTPReactorImpl);
        if( m_poReactor == 0 )
        {
                ERROR("%s", "Memory allocation failed for m_poReactor");
        }

	if( theOiHandlerPtr == 0 )
	{
		ERROR("%s", "Memory allocation failed for  acs_apgcc_oihandler_V3");
	}

	DEBUG("%s"," Exiting acs_hcs_report_ObjectImpl::acs_hcs_report_ObjectImpl");
}//End of acs_hcs_report_ObjectImpl

/*===================================================================
   ROUTINE: ~acs_hcs_report_ObjectImpl
=================================================================== */
acs_hcs_report_ObjectImpl::~acs_hcs_report_ObjectImpl()
{
	DEBUG("%s"," Entering acs_hcs_report_ObjectImpl::~acs_hcs_report_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		theOiHandlerPtr->removeClassImpl(this,ACS_HC_REPORT_CLASS_NAME);
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}

        if( m_poReactor != 0  )
        {
                delete m_poReactor;
                m_poReactor = 0;
        }

        if( m_poTPReactorImpl != 0 )
        {
                delete m_poTPReactorImpl;
                m_poTPReactorImpl = 0;
        }

	DEBUG("%s"," Exiting acs_hcs_report_ObjectImpl::~acs_hcs_report_ObjectImpl");
}//End of ~acs_hcs_report_ObjectImpl

/*===================================================================
   ROUTINE: setImpl
=================================================================== */
ACS_CC_ReturnType acs_hcs_report_ObjectImpl::setImpl()
{

#if 0
        for (int i=0; i < 10; ++i)
        {
                theOiHandlerPtr->removeClassImpl(this, ACS_HC_REPORT_CLASS_NAME);
                ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_REPORT_CLASS_NAME);

                if (errorCode == ACS_CC_FAILURE)
                {
                        ERROR("Set implementer %s for the object %s failed",getImpName().c_str(), getObjName().c_str());
                        int intErr = getInternalLastError();
                        ERROR("Failed in addClassImpl with error : %s", getInternalLastErrorText());
                        for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
                        if ( i >= 9 )
                        {
                                DEBUG("addClassImpl failed with ErrCode = %d",intErr);
                                return ACS_CC_SUCCESS;
                        }
                        else
						{
                                continue;
						}
                }
                else
                {
                        DEBUG("Success: Set implementer %s for the object %s",getImpName().c_str(), getObjName().c_str());
                        m_isClassImplAdded = true;
                        return ACS_CC_SUCCESS;
                }
        }
#endif
	
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,"AxeHealthCheckReport");

        if ( errorCode == ACS_CC_FAILURE )
        {
                int intErr = getInternalLastError();
                cout<<"intErr:"<<intErr<<endl;
                cout<<"Error : Set implementer  for the object failure"<<endl;
                ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
                DEBUG("%s"," Exiting acs_hcs_report_ObjectImpl::svc Dispatch");
                return -1;
        }
        else
        {
                cout << "acs_hcs_report_ObjectImpl success" << endl;
                DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
        }
	

   	return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 acs_hcs_report_ObjectImpl::svc(  )
{
	DEBUG("%s"," Entering acs_hcs_report_ObjectImpl::svc Dispatch");
#if 0
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,"AxeHealthCheckReport");

	if ( errorCode == ACS_CC_FAILURE )
	{
		int intErr = getInternalLastError();	
		cout<<"intErr:"<<intErr<<endl; 
		cout<<"Error : Set implementer  for the object failure"<<endl;
		ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting acs_hcs_report_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{
		cout << "acs_hcs_report_ObjectImpl success" << endl;
		DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}
#endif
        
	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
        m_poReactor->run_reactor_event_loop();

	DEBUG("%s"," Exiting acs_hcs_report_ObjectImpl::svc Dispatch");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
void acs_hcs_report_ObjectImpl::shutdown()
{
    	DEBUG("%s", "Entering acs_hcs_report_ObjectImpl::shutdown");
    	cout << "In shutdown" << endl;

    	m_poReactor->end_reactor_event_loop();
	cout << "end loop called" << endl;
        if( theOiHandlerPtr != 0 )
        {
                //remove class implementer
                DEBUG("%s", "Calling removeClassImpl.");
                theOiHandlerPtr->removeClassImpl(this,ACS_HC_REPORT_CLASS_NAME);
        }

    	DEBUG("%s", "Leaving acs_hcs_report_ObjectImpl::shutdown");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_hcs_report_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
    	(void) oiHandle;
    	(void) ccbId;
    	(void) className;
    	(void) parentName;
    	(void) attr;

	cout<<"Entering acs_hcs_report_ObjectImpl::create"<<endl;
	cout<<"className :"<<className<<endl;
	cout<<"parentName :"<<parentName<<endl;

	char target[32]			= {0};
	char transferQueue[128]		= {0};
	char reportId[128]		= {0};
	int i = 0;

	Report_Parameters report_parameters ;  //:Malli
	memset(&report_parameters,0,sizeof(Report_Parameters));

	//check whether parameters inside struct are null or not

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          acs_hcs_report_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	cout<<"Before while"<<endl;
	while( attr[i])
        {
		cout<<"attr[i]->attrName :"<<attr[i]->attrName<<endl;
                switch( attr[i]->attrType)
                {
	                case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, transferQueueAttr) == 0 && attr[i]->attrValuesNum)
                        	{
			        	ACE_OS::strcpy(transferQueue, reinterpret_cast<char *>(attr[i]->attrValues[0]));
					cout<<"In transfer Queue loop"<<endl;
				}
				else if(ACE_OS::strcmp(attr[i]->attrName, targetAttr) == 0 && attr[i]->attrValuesNum)
                        	{
                                	ACE_OS::strcpy(target, reinterpret_cast<char *>(attr[i]->attrValues[0]));
					cout<<"In target loop"<<endl;
				}
				else if(ACE_OS::strcmp(attr[i]->attrName, reportIdAttr) == 0 && attr[i]->attrValuesNum)
				{
					ACE_OS::strcpy(reportId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
                                        cout<<"In reportId loop"<<endl;
				}
				break;

			 default:	
				break;
		}
		i++;
	}

	cout<<"After while"<<endl;
	cout<<"copying the values into structure"<<endl;

	strncpy(report_parameters.TransferQueue,transferQueue,sizeof(report_parameters.TransferQueue) - 1);
	strncpy(report_parameters.Target,target,sizeof(report_parameters.Target) - 1);

	        //we pass this value to apply call back
        string Id = reportId;
        string reportrdn = Id + "," + parentName ;
        completeReportRdn = reportrdn ;

	
    Report_Parameters *request;
    request =  new Report_Parameters(report_parameters);

	int retValue=createValidations(request);
	switch (retValue)
    	{
         	case ACS_HC_INVALID_TARGET :
            		strcpy(errorText,"Invalid target");
            		setErrorCode(ACS_HC_INVALID_TARGET);
			 if(request)
                                delete request;
            		return ACS_CC_FAILURE;

         	case ACS_HC_INVALID_TQ :
            		strcpy(errorText,"Invalid transfer queue");
            		setErrorCode(ACS_HC_INVALID_TQ);
			 if(request)
                                delete request;
            		return ACS_CC_FAILURE;

 		default:
               		break;
    	}
	
	//tq rdn
	getTqDN(request->TransferQueue);
	
	delete request;
	cout<<"Returning from create call back"<<endl;
	DEBUG("%s","Leaving acs_hcs_report_ObjectImpl::create"); 
	return ACS_CC_SUCCESS;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_hcs_report_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s","Entering acs_hcs_report_ObjectImpl::deleted");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_report_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	cout<<"Entering acs_hcs_report_ObjectImpl::deleted callback"<<endl;
	cout<<"objName :"<<objName<<endl;

	DEBUG("%s","Leaving acs_hcs_report_ObjectImpl::deleted"); 
	return ACS_CC_SUCCESS;
}//End of deleted
/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_hcs_report_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_report_ObjectImpl::modify");
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_report_ObjectImpl ObjectModifyCallback invocated           \n");
	DEBUG ("%s","-----------------------------------------------------\n");

	//if(acs_hcs_report_ObjectImpl::tqCreated)
	//	return ACS_CC_SUCCESS;

	char target[32]			= {0};
	char transferQueue[128]		= {0};
	bool tq_modify_flag = false;

	cout<<"             "<<objName;
	
	int i = 0;
	Report_Parameters mod_parameters;  //:Malli
	memset(&mod_parameters,0,sizeof(Report_Parameters));


	while( attrMods[i] )
    	{
		cout<<"attrMods[i]->modAttr.attrName :"<<attrMods[i]->modAttr.attrName<<endl;
		switch ( attrMods[i]->modAttr.attrType )
        	{
			case ATTR_STRINGT:
			
				 if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, targetAttr) == 0)
                		{
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(target,"");
					}
					else
					{
                    				ACE_OS::strcpy(target, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					}
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, transferQueueAttr) == 0 )
        		        {
					tq_modify_flag = true;
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(transferQueue,"");
					}
					else
					{
						ACE_OS::strcpy(transferQueue, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					}
				}
			 	break;
                 
			 default: 
				break;
		    }
		i++;
	}

	cout<<"After switch"<<endl;
    
	strncpy(mod_parameters.TransferQueue,transferQueue,sizeof(mod_parameters.TransferQueue) - 1);
	strncpy(mod_parameters.Target,target,sizeof(mod_parameters.Target) - 1);

	completeReportRdn = objName;

	
	Report_Parameters *request;
	request =  new Report_Parameters(mod_parameters); 

	int retValue=createValidations( request);
	switch (retValue)
    	{
				
         	case ACS_HC_INVALID_TARGET :
            		strcpy(errorText,"Invalid target");
            		setErrorCode(ACS_HC_INVALID_TARGET);
			if(request)
				delete request;
            		return ACS_CC_FAILURE;

         	case ACS_HC_INVALID_TQ :
           		strcpy(errorText,"Invalid transfer queue");
            		setErrorCode(ACS_HC_INVALID_TQ);
			 if(request)
                                delete request;
            		return ACS_CC_FAILURE;

			default :
				break;
	}

	if(tq_modify_flag)
	getTqDN(request->TransferQueue);

	delete request;
	cout<<"Returning from create call back"<<endl;

    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_report_ObjectImpl::modify");
	return ACS_CC_SUCCESS;
}//End of modify
/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_hcs_report_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_report_ObjectImpl::complete"); 

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_report_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	cout << "Complete returned" << endl;
	DEBUG("%s","Leaving acs_hcs_report_ObjectImpl::complete");

	return ACS_CC_SUCCESS;

}//End of complete
/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_hcs_report_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_report_ObjectImpl::abort");

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_report_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving acs_hcs_report_ObjectImpl::abort"); 
}//End of abort
/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_hcs_report_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering ACS_HC_Job_ObjectImpl::apply"); 

	(void)oiHandle;
	(void)ccbId;

	cout<<"In apply call back"<<endl;

		DEBUG ("%s","\n---------------------------------------------------\n");
    	DEBUG ("%s","           ACS_HC_Job_ObjectImpl CcbApplyCallback invocated           \n");
    	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG("%s","Leaving ACS_HC_Job_ObjectImpl::apply");
}//End of apply
/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_hcs_report_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_report_ObjectImpl::updateRuntime");
	(void) objName;
	(void) attrName;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_report_ObjectImpl CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_report_ObjectImpl::updateRuntime");

	return ACS_CC_FAILURE;
}//End of updateRuntime
/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_report_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
     	DEBUG("%s","Entering acs_hcs_report_ObjectImpl::adminOperationCallback");
	cout<<"Entering acs_hcs_report_ObjectImpl::adminOperationCallback"<<endl;
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;
	(void) p_objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_report_ObjectImpl adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	DEBUG("%s","Leaving acs_hcs_report_ObjectImpl::adminOperationCallback");
}//End of adminOperationCallback

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_hcs_report_ObjectImpl::setErrorCode(unsigned int code)
{
	cout<<"Entering acs_hcs_report_ObjectImpl::setErrorCode "<<endl;
    	setExitCode(code,errorText);
}

/*======================================================================
		ROUTINE : Create job validations :Malli
========================================================================*/
ACE_INT32 acs_hcs_report_ObjectImpl::createValidations(Report_Parameters *request)
{
	DEBUG("%s","Entering acs_hcs_report_ObjectImpl::createJobValidations");
	cout<<"In create validations"<<endl;

	AcsHcs::TransferQueue tq;
	vector<ACS_APGCC_ImmAttribute *> attributes;
        ACS_CC_ReturnType returnCode;
        OmHandler immHandler;
	bool init_flag = false;
        std::vector<std::string> pd_dnList;
	

	if(strcmp(request->TransferQueue,"" )!=0)
	{
        	ACE_INT32 errcode;
	        AES_OHI_ExtFileHandler2* mySenderFile;
        	mySenderFile = new AES_OHI_ExtFileHandler2("ACS","acs_hcd");
	
	        switch (errcode = mySenderFile->attach()) //attach
        	{
        		case AES_OHI_NOPROCORDER:
                		delete mySenderFile;
	                        cout << "TQ not defined" << endl;
        	                strcpy(errorText,"Transfer queue not defined");
                	        setErrorCode(ACS_HC_INVALID_TQ);
                        	return ACS_HC_INVALID_TQ;
			case AES_OHI_NOSERVERACCESS:
				cout << "No AFP server access" << endl;
				strcpy(errorText,"No AFP server access");
				setErrorCode(ACS_HC_RC_NOAFPACESS);
				delete mySenderFile;
				return ACS_CC_FAILURE;
			case AES_OHI_NOERRORCODE:
				cout << "TQ defined" << endl;
				break;
			default:
				strcpy(errorText,"Internal error code");
				setErrorCode(ACS_HC_RC_INTPROGERR);
				delete mySenderFile;
				return ACS_CC_FAILURE;
		}

		//check that transfer queue is defined
		switch (mySenderFile->fileTransferQueueDefined(request->TransferQueue))
		{
			case AES_OHI_NOERRORCODE:
			    cout << "TQ defined" << endl;
			    break;
			case AES_OHI_NOSERVERACCESS:
			    cout << "No AFP server access" << endl;
			    strcpy(errorText,"No AFP server access");
			    setErrorCode(ACS_HC_RC_NOAFPACESS);
			    delete mySenderFile;
			    return ACS_CC_FAILURE;
			default:
			    cout << "TQ not defined" << endl;
			    strcpy(errorText,"Transfer queue not defined");
			    setErrorCode(ACS_HC_RC_TQUNDEF);
			    delete mySenderFile;
			    return ACS_HC_INVALID_TQ;
		}
		mySenderFile->detach(); //detach
	        delete mySenderFile;
	}

	// Validate the target
	if(strcmp(request->Target,"")!=0)
	{
		std::vector<std::string> cpNames;
		string target(request->Target);
		int cpId;
		getCPIDs(cpNames);

		for(cpId = 0; cpId < cpNames.size(); cpId++)	
		{
			cout << "cpNames: " << cpNames[cpId] << endl;
			if(target == cpNames[cpId])
			{
				break;
			}
		}


		if( cpId == cpNames.size())
		{
			return ACS_HC_INVALID_TARGET;
		}
	}
	//end of target Validation


	immHandler.Finalize(); 
        cout<<"Getting out form validations"<<endl;			
	DEBUG("%s","Leaving acs_hcs_report_ObjectImpl::createJobValidations");	
	return ACS_CC_SUCCESS;
}

void acs_hcs_report_ObjectImpl::getCPIDs(std::vector<std::string>& cpNames)
{
                ACS_CS_API_CP * cp = 0;
                try
                {
                        if (!(cp = ACS_CS_API::createCPInstance()))
                        {
                                cout << "createCPInstance failed" << endl;
                        }
                }
                catch(...)
                {
                        cout << "Exception caught" << endl;
                }

                ACS_CS_API_IdList cpList;
                int result;

                try
                {
                        result = (int) cp->getCPList(cpList);
                }
                catch (...)
                {
                        cout << "getCPList failed" << endl;
                }

                try
                {
                        ACS_CS_API::deleteCPInstance(cp);
                }
                catch (...)
                {
                        cout << "deleteCPInstance failed 1" << endl;
                }

                if (result != 0)
                {
                        cout << "getCPList failed to get IDs" << endl;
                        try
                        {
                                ACS_CS_API::deleteCPInstance(cp);
                        }
                        catch(...)
                        {
                                cout << "deleteCPInstance failed 2" << endl;
                        }
                }
                else cout << "Got the CP IDs " << cpList.size() << endl;

                unsigned cpId = 0;
                char cpName[200];
                for (unsigned i = 0; (i < cpList.size()); i++)
                {
                        cpId = cpList[i++];
			cout << "cpID: " << cpId << endl;

                        if (((int)cpId >= 0) && (cpId < 1000)) // Blades
                        {
                                char strCpId[6];
                                sprintf(strCpId,"%u",cpId);
                                strcpy(cpName, "BC");
                                strcat(cpName, strCpId);
                                cpNames.push_back(cpName);
                        }
                        else if ((cpId >= 1000) && (cpId < 2000)) // CP
                        {
				cpId = cpId - 1000;
                                char strCpId[6];
                                sprintf(strCpId,"%u",cpId);
                                strcpy(cpName, "CP");
                                strcat(cpName, strCpId);
                                cpNames.push_back(cpName);
                        }
                }
}

void acs_hcs_report_ObjectImpl::updateTQRDN()
{
	cout << "updateTQRDN" << endl;
        OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        returnCode=immHandler.Init();
        if(returnCode!=ACS_CC_SUCCESS)
        {
                cout<<"ERROR: init FAILURE!!!\n";
        }
        cout<<"After immHandler initialization"<<endl;
                cout<<"in tqrdn function"<<endl;
                char *objName = const_cast<char*>(completeReportRdn.c_str());
                ACS_CC_ImmParameter TQDN;
                TQDN.attrName = transferQueueDNAttr;
                TQDN.attrType=ATTR_NAMET;
                TQDN.attrValuesNum=1;
                char* dnValue = const_cast<char*>(tqRdn.c_str());
                void* value[1]={ reinterpret_cast<void*>(dnValue)};
                cout<<"Malli value is"<<value<<endl;
                TQDN.attrValues = value ;
                cout<<"In between"<<endl;
                cout<<objName<<endl;
                returnCode = immHandler.modifyAttribute(objName, &TQDN);
                cout<<"return code is"<<returnCode<<endl;
                cout<<"before return code"<<endl;
                if ( returnCode != ACS_CC_SUCCESS )
                {
                        cout << "ERROR: modifyAttribute FAILURE!!!\n";
                }
                if(returnCode == ACS_CC_FAILURE)
                {
                        cout<<"IMM modify attributr failure"<<std::endl;
                        cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
                        cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
                }
                else
                {
                        cout<<"Job is updated with transfer queuerdn"<<endl;
                }
                cout<<"After else"<<endl;

        cout<<"After job update"<<endl;
	tqCreated = false;
	tqRdn = "";
	completeReportRdn = "";
}

void acs_hcs_report_ObjectImpl::getTqDN(string tq)
{
        if(strcmp(tq.c_str(),"") != 0)
        {
                cout<<"Provided transferQueue is "<<tq<<endl;
                std::vector<std::string> pd_dnList;
                        char *classNamepd = const_cast<char*>("AxeDataTransferFileTransferQueue");
                        tq = "fileTransferQueueId=" + tq ;

                OmHandler immHandler;
                ACS_CC_ReturnType returnCode;


                returnCode=immHandler.Init();
                if(returnCode!=ACS_CC_SUCCESS)
                {
                        cout<<"ERROR: init FAILURE!!!\n";
                }
                returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);
                if(returnCode == ACS_CC_SUCCESS)
                {
                        cout<<"return code is successfull"<<endl;
                        int numDef  = pd_dnList.size();
                        cout<<"Number of tqs "<<numDef<<endl;
                        string temp;
                        int i;
                        for( i = 0; i < numDef; i++)
                        {
                                cout<<"Tq in loop "<<pd_dnList[i]<<endl;
                                size_t commaPos = pd_dnList[i].find_first_of(",");
                                string parseTQValue = pd_dnList[i].substr(0,commaPos);
                                cout<<"parseTQValue is "<<parseTQValue<<endl;
                                if(strcmp(tq.c_str(),parseTQValue.c_str())== 0)
                                {
                                        cout<<"Found rdn for tq"<<endl;
                                        break;
                                }
                        }
                        tqRdn = pd_dnList[i];
                        cout<<"DN need to populate is :"<<tqRdn<<endl;
                }
                else
                {
                        cout<<"Unable to get class instances"<<endl;
		 }
        }
	else
	{
		cout<<"    else part  "<<endl;
		tqRdn = "";
	}
	 tqCreated = true;
}

