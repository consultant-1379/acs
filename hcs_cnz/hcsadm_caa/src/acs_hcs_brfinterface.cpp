//******************************************************************************
//
//  NAME
//     acs_hcs_brfinterface.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2013. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include <acs_hcs_brfinterface.h>
#include <acs_hcs_global_ObjectImpl.h>
#include <acs_hcs_healthcheckservice.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/eventfd.h>
using namespace std;

//AcsHcs::acs_hcs_brfimplementer* AcsHcs::acs_hcs_brfimplementer::theHcBrfcImpl = 0;
//AcsHcs::acs_hcs_brfthread* AcsHcs::acs_hcs_brfimplementer::theHcBrfcRuntimeHandlerThreadPtr = 0;

bool AcsHcs::acs_hcs_brfimplementer::stopRequested = false;
bool AcsHcs::acs_hcs_brfimplementer::brfThrCreated = false;

namespace AcsHcs
{
	bool acs_hcs_brfimplementer::isBrfInProgress = false;
	bool acs_hcs_brfthread::theIsStop = false;

	acs_hcs_brfimplementer::acs_hcs_brfimplementer()
	{
		theHcBrfcRuntimeHandlerThreadPtr = 0;
                theHcBrfcImpl = 0;
	}

	acs_hcs_brfimplementer::acs_hcs_brfimplementer(string p_impName ):acs_apgcc_objectimplementerinterface_V3(p_impName)
	{
	}

	acs_hcs_brfimplementer::acs_hcs_brfimplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): acs_apgcc_objectimplementerinterface_V3(p_objName, p_impName, p_scope)
	{
	}

        //void acs_hcs_brfimplementer::createBrfcThread()
	int acs_hcs_brfimplementer::svc()
        {
		DEBUG("%s","Entering  acs_hcs_brfimplementer::svc ");
		cout << "retry thread launched" << endl;
                createParticipant();
                createBrfcRunTimeOwner();
                initializeBrfcRunTimeOwner();
		cout << "Retry thread exited" << endl;
		DEBUG("%s","Leaving acs_hcs_brfimplementer::svc");
		return 0;
        }

        int acs_hcs_brfimplementer::createParticipant()
        {
		DEBUG("%s","Entering acs_hcs_brfimplementer::createParticipant");
                char attrdn[] = "brfPersistentDataOwnerId";
                char attrVersion[] = "version";
                char attrBackupType[] = "backupType";
                char *className = const_cast<char*>("BrfPersistentDataOwner");
                ACS_CC_ReturnType returnCode;
                OmHandler immHandler;

                returnCode = immHandler.Init();
                if (returnCode != ACS_CC_SUCCESS)
                {
                        return -1;
                }

                char* nomeParent = const_cast<char*>("brfParticipantContainerId=1");

                //Create attributes list
                vector < ACS_CC_ValuesDefinitionType > AttrList;
                ACS_CC_ValuesDefinitionType attributeRDN;
                ACS_CC_ValuesDefinitionType attributeVersion;
                ACS_CC_ValuesDefinitionType attributeBackupType;

                attributeRDN.attrName = attrdn;
                attributeRDN.attrType = ATTR_STRINGT;
                attributeRDN.attrValuesNum = 1;
                char* rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ERIC-APG-ACS-HC");
                void* value[1] = { reinterpret_cast<void*>(rdnValue) };
                attributeRDN.attrValues = value;

                attributeVersion.attrName = attrVersion;
                attributeVersion.attrType = ATTR_STRINGT;
                attributeVersion.attrValuesNum = 1;
                char* strValue = const_cast<char*>("1.0");
                void* valueStr[1] = { reinterpret_cast<void*>(strValue) };
                attributeVersion.attrValues = valueStr;

                attributeBackupType.attrName = attrBackupType;
                attributeBackupType.attrType = ATTR_INT32T;
                attributeBackupType.attrValuesNum = 1;
                int intAttributeBackupType = 1;
                void* valueInt[1] = { reinterpret_cast<void*>(&intAttributeBackupType) };
                attributeBackupType.attrValues = valueInt;

                AttrList.push_back(attributeRDN);
                AttrList.push_back(attributeVersion);
                AttrList.push_back(attributeBackupType);

		do
		{
			returnCode = immHandler.createObject(className, nomeParent, AttrList);
			if (returnCode == ACS_CC_SUCCESS)
			{
				DEBUG("%s", "Successful in creating the BRFC object for HC");
				break;
			}
			else if(immHandler.getInternalLastError() == -14)	
			{
				//Object already exists, no need to create it again
				cout << "Create object failed : " << immHandler.getInternalLastError() << endl;	
				DEBUG("%s", "Failure in creating the BRFC object for HC");
				break;
			}
			sleep(1); // Wait for a while and retry
		}while(!acs_hcs_brfimplementer::stopRequested);

		cout << "Came out of loop" << endl;
		
                /* Close the connection with IMM, and reset all handlers */
                returnCode = immHandler.Finalize();
                if (returnCode != ACS_CC_SUCCESS)
                {
                        return -1;
                }
		cout << "Returning create" << endl;
		DEBUG("%s","return from acs_hcs_brfimplementer::createParticipant");
                return 0;
        }

        int acs_hcs_brfimplementer::deleteParticipant()
        {
                //ACS_CC_ReturnType returnCode;
		cout << "In deleteP" << endl;
		DEBUG("%s","Entering acs_hcs_brfimplementer::deleteParticipant");
                if (theHcBrfcRuntimeHandlerThreadPtr != 0)
                {
			DEBUG("%s","Entering in if loop of acs_hcs_brfimplementer::deleteParticipant");
			cout << "stopping now" << endl;
                        if(!theHcBrfcRuntimeHandlerThreadPtr->stop())
                                DEBUG("%s","BRF thread could not be signalled");
                        //acs_hcs_brfthread::theIsStop = true;
                        theHcBrfcRuntimeHandlerThreadPtr->deleteImpl(theHcBrfcImpl);

                }
                return 0;
        }

        void acs_hcs_brfimplementer::createBrfcRunTimeOwner()
        {
		if(!acs_hcs_brfimplementer::stopRequested)
		{
			DEBUG("%s","Entering createBrfcRunTimeOwner");
			theHcBrfcRuntimeHandlerThreadPtr = new acs_hcs_brfthread();
			theHcBrfcImpl = new acs_hcs_brfimplementer();
			DEBUG("%s","Leaving createBrfcRunTimeOwner");
		}
        }

        bool acs_hcs_brfimplementer::initializeBrfcRunTimeOwner()
        {
		DEBUG("%s","Entering acs_hcs_brfimplementer::initializeBrfcRunTimeOwner");
		if(!acs_hcs_brfimplementer::stopRequested)
		{
			if(theHcBrfcRuntimeHandlerThreadPtr)
			{
				DEBUG("%s","Entering into if loop of acs_hcs_brfimplementer::initializeBrfcRunTimeOwner");
				theHcBrfcRuntimeHandlerThreadPtr->setImpl(theHcBrfcImpl);
				theHcBrfcRuntimeHandlerThreadPtr->activate();
				acs_hcs_brfimplementer::brfThrCreated = true;

			}
			DEBUG("%s","Leaving acs_hcs_brfimplementer::initializeBrfcRunTimeOwner with TRUE");
			return true;
		}
		DEBUG("%s","Leaving acs_hcs_brfimplementer::initializeBrfcRunTimeOwner with false");
		return false;
        }

	bool acs_hcs_brfimplementer::getBrfStatus()
	{
		DEBUG("%s","Entering acs_hcs_brfimplementer::getBrfStatus");
		return isBrfInProgress;
	}

	void acs_hcs_brfimplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
			ACS_APGCC_AdminOperationParamType**paramList)
	{
		DEBUG("%s","Entering acs_hcs_brfimplementer:: adminOperationCallback");
		(void)p_objName;
		int dim=0;
		int i=0;
		while(paramList[i])
		{
			i++;
			dim++;
		}

		unsigned long long requestId;
		i = 0;
		while( paramList[i] )
		{
			switch ( paramList[i]->attrType )
			{
				// ONLY FOR requested parameter
				case ATTR_UINT64T:
					if(strcmp(paramList[i]->attrName,"requestId")==0)
					{
						requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
					}
					break;

				default:
					break;
			}
			i++;
		}

		/*retVal = true => backup can be permitted
		  retVal = false => backup cannot be permitted */
		int retVal = HealthCheckService::isBrfcPermitted();
		int retAdminOperationResult = this->adminOperationResult(oiHandle, invocation, retVal);
		DEBUG("Called adminOperationResult with retVal = %d, returned = %d", retVal, retAdminOperationResult);

		switch (operationId)
                {
                        case BRM_PARTICIPANT__PERMIT_BACKUP:
				if(!retVal)
				{
					retVal = this->ResponseToBrfc(requestId,BRF_ERROR_CONFLICT, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
				}
				else
				{
					isBrfInProgress = true;
					retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
				}
                                break;

                        case BRM_PARTICIPANT__COMMIT_BACKUP:
                                isBrfInProgress = false;
				retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
                                break;

                        case BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:
				isBrfInProgress = true;
				retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
                                break;

                        case BRM_PARTICIPANT__CANCEL_BACKUP:
                                isBrfInProgress = false;
				retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
                                break;
                }

	}


	ACS_CC_ReturnType acs_hcs_brfimplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
	{
		DEBUG("%s","In acs_hcs_brfimplementer::create");
		(void)oiHandle;
		(void)ccbId;
		(void)className;
		(void)parentName;
		(void)attr;
		return ACS_CC_SUCCESS;
	}

	ACS_CC_ReturnType acs_hcs_brfimplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		DEBUG("%s","In acs_hcs_brfimplementer::deleted");
		(void)oiHandle;
		(void)ccbId;
		(void)objName;
		return ACS_CC_SUCCESS;
	}

	ACS_CC_ReturnType acs_hcs_brfimplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		DEBUG("%s","In acs_hcs_brfimplementer::modify");
		(void)oiHandle;
		(void)ccbId;
		(void)objName;
		(void)attrMods;
		return ACS_CC_SUCCESS;
	}

	ACS_CC_ReturnType acs_hcs_brfimplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		(void)oiHandle;
		(void)ccbId;
		return ACS_CC_SUCCESS;
	}

	void acs_hcs_brfimplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		(void)oiHandle;
		(void)ccbId;
	}

	void acs_hcs_brfimplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		(void)oiHandle;
		(void)ccbId;
	}

	ACS_CC_ReturnType acs_hcs_brfimplementer::updateRuntime(const char* p_objName, const char** p_attrName)
	{
		(void)p_objName;
		(void)p_attrName;
		return ACS_CC_SUCCESS;
	}

	int acs_hcs_brfimplementer::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode)
	{
		DEBUG("%s","Entering acs_hcs_brfimplementer::ResponseToBrfc");
		acs_apgcc_adminoperation admOp;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
		ACS_APGCC_AdminOperationParamType firstElem;
		ACS_APGCC_AdminOperationParamType secondElem;

		char firstAttName[]= "requestId";
		char secondAttNameVal1[]= "resultCode";
		char secondAttNameVal2[]= "progressCount";
		char thirdAttName[]= "message"; 
		unsigned long long requestIdValue = requestId;
		int brfStatusVal = brfStatus; // BRF_SUCCESS
		char* strValue = const_cast<char*>("");
		void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
		int intAttrValue = 50; // 50 % 
		if( responseCode  == BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT )
		{
			DEBUG("%s","In acs_hcs_brfimplementer::ResponseToBrfc at if loop");
			ACS_APGCC_AdminOperationParamType thirdElem;

			//create first Element of parameter list
			firstElem.attrName =firstAttName;
			firstElem.attrType=ATTR_UINT64T;
			firstElem.attrValues=reinterpret_cast<void*>(&requestIdValue);

			// create second Element of parameter list
			secondElem.attrName =secondAttNameVal1;
			secondElem.attrType=ATTR_INT32T ;
			secondElem.attrValues=reinterpret_cast<void*>(&brfStatusVal);

			//create the third Element of parameter list
			thirdElem.attrName =thirdAttName;
			thirdElem.attrType=ATTR_STRINGT ;
			thirdElem.attrValues=valueStr;

			//insert into the vector
			vectorIN.push_back(firstElem);
			vectorIN.push_back(secondElem);
			vectorIN.push_back(thirdElem);
		}
		else if (responseCode == BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS)
		{
			DEBUG("%s","In acs_hcs_brfimplementer::ResponseToBrfc at else if loop");
			//create first Element of parameter list
			firstElem.attrName =firstAttName;
			firstElem.attrType=ATTR_UINT64T;
			firstElem.attrValues=reinterpret_cast<void*>(&requestIdValue);

			// create second Element of parameter list
			secondElem.attrName =secondAttNameVal2;
			secondElem.attrType=ATTR_UINT32T ;
			secondElem.attrValues=reinterpret_cast<void*>(&intAttrValue);

			//insert into the vector
			vectorIN.push_back(firstElem);
			vectorIN.push_back(secondElem);
		}

		const char *dnObjName1 = "brfParticipantContainerId=1";
		long long int timeOutVal_30sec = 30*(1000000000LL);

		int res1 = admOp.init();

		if (res1 != ACS_CC_SUCCESS)
		{
			return -1;
		}

		int returnValue1 = 1;
		/*Wait at least one second before invoke administrative operations in order to synchronize with BRFC*/
		sleep(1);
		/* Invoke operation  */
		int retry = 0;
		while (admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec) == ACS_CC_FAILURE)
		{
			DEBUG("%s","In acs_hcs_brfimplementer::ResponseToBrfc, adminoperation failed");
			std::string tempErrMsg ="ERROR CODE = ";
			tempErrMsg += admOp.getInternalLastError();
			tempErrMsg += " ERROR MESSAGE = " ;
			tempErrMsg+= admOp.getInternalLastErrorText();

			if (admOp.getInternalLastError()!=((-1)*SA_AIS_ERR_EXIST)) 
				break;

			sleep(1);
			if (++retry > 60) 
				return -1;
		}

		if (admOp.finalize() != ACS_CC_SUCCESS)
		{
			return -1;
		}
		return 0;
	}


	acs_hcs_brfthread::acs_hcs_brfthread()
	{
		DEBUG("%s","In acs_hcs_brfthread::acs_hcs_brfthread constructor");
		theBrfHcImplementer = NULL;
		theIsStop = false;
		unsigned int initval = 0U;
		m_StopEvent = eventfd(initval,0);
		cout << "m_StopEvent: " << m_StopEvent << endl;
	}

	acs_hcs_brfthread::~acs_hcs_brfthread()
	{
		DEBUG("%s","In acs_hcs_brfthread::~acs_hcs_brfthread");
		ACE_OS::close(m_StopEvent);
		theBrfHcImplementer = NULL;
	}

	void acs_hcs_brfthread::setImpl(acs_hcs_brfimplementer *pImpl)
	{
		DEBUG("%s","In acs_hcs_brfthread::setImpl");
		ACS_CC_ReturnType returnCode;
		const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-HC,brfParticipantContainerId=1";
		const char *impName = "ACS_Brfc_HC_Impl";
		ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

		theBrfHcImplementer = pImpl;

		theBrfHcImplementer->setObjName(dnObjName);
		theBrfHcImplementer->setImpName(impName);
		theBrfHcImplementer->setScope(scope);

		do
		{
			returnCode = oiHandler.addObjectImpl(theBrfHcImplementer);
			if(returnCode == ACS_CC_FAILURE)
			{
				DEBUG("%s","Brf implementer set failure");
				cout << "Implementer set failure" << endl;
			}
			else
			{
				DEBUG("%s","Brf implementer set success");
				cout << "Implementer set success" << endl;
				break;
			}
			sleep(1); // wait for a while to retry
		}while(!acs_hcs_brfimplementer::stopRequested);
	}

	void acs_hcs_brfthread::deleteImpl(acs_hcs_brfimplementer *pImpl)
	{
		DEBUG("%s","In acs_hcs_brfthread::deleteImpl");
		(void)pImpl;
		for( int i=0;i < 3; i++)
		{
			DEBUG("%s","Trying to delete implementer");
			if ( pImpl ) //theBrfHcImplementer)
			{
				if(oiHandler.removeObjectImpl(pImpl) == ACS_CC_FAILURE)
				{
					DEBUG("%s","Failed in removing brf object implementer");
					sleep(0.2);
					continue;
					//for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
				}
				else
				{	
					DEBUG("%s","Brf Implementer deleted successfully");
					break;
				}
			}
			else
			{
				DEBUG("%s","pointer deleted before removing brf object implementer");
				break;
			}
		}
	}

	bool acs_hcs_brfthread::stop()
	{
		cout << "In stop" << endl;
		DEBUG("%s", "acs_hcs_brfthread::stop() entered");
		ACE_UINT64 stopEvent=1;
		cout << "Writing on " << m_StopEvent << endl;
		ssize_t numByte = ACE_OS::write(m_StopEvent, &stopEvent, sizeof(ACE_UINT64));
		cout << "After write" << endl;
		if(sizeof(ACE_UINT64) != numByte)
		{
			DEBUG("%s", "Error occurred while signaling stop event");
			return false;
		}
		cout << "acs_hcs_brfthread::stop() exiting" << endl;
		DEBUG("%s", "acs_hcs_brfthread::stop() exiting");
		return true;
	}

	int acs_hcs_brfthread::svc(void)
	{
		DEBUG("%s","In acs_hcs_brfthread::svc");
		if(theBrfHcImplementer == 0)
		{
			return -1;
		}
		const nfds_t nfds = 2;
		struct pollfd fds[nfds];
		ACE_OS::memset(fds, 0 , sizeof(fds));

		fds[0].fd = m_StopEvent;
		fds[0].events = POLLIN;
		
		fds[1].fd = theBrfHcImplementer->getSelObj();
		fds[1].events = POLLIN;

		ACE_INT32 poolResult;
		ACS_CC_ReturnType result;
		
		ACE_Time_Value selectTime;
		selectTime.set(1);
		while(true)
		{
			poolResult = ACE_OS::poll(fds, nfds,selectTime);
			
			if (poolResult == 0)
			{
			}
			else if (poolResult == -1)
			{
			}

			if(fds[0].revents & POLLIN)
			{
				// Exit the thread
				cout << "stop signalled for brf thread" << endl;
				DEBUG("%s", "stop signalled for brf thread");
				break; 
			}
			
			if(fds[1].revents & POLLIN)
			{
				if(theBrfHcImplementer) 
				{
					result = theBrfHcImplementer->dispatch(ACS_APGCC_DISPATCH_ALL);
				}
				if(result != ACS_CC_SUCCESS)
				{
					DEBUG("%s", "Error on BrfImplementer dispatch event");			
				}
			}
		}
		return 0;
	}
}
