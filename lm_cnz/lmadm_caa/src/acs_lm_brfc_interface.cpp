//******************************************************************************
//
//  NAME
//     acs_lm_brfc_interface.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
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
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2012-07-12 by XCSSUBM PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include<acs_bur_BrfAdminOperationId.h>
//#include "acs_lm_brfc_define.h"
#include "acs_lm_tra.h"
#include "acs_lm_defs.h"
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include "acs_lm_root_objectImplementer.h"
#include "acs_lm_server.h"
#include "acs_lm_brfc_interface.h"
#include <sys/eventfd.h>
#include <iostream>
#include <unistd.h>
#include <saImmOm.h> /* HX20968 */
using namespace std;

bool ACS_LM_BrfImplementer::isBrfInProgress = false;

ACS_LM_BrfImplementer::ACS_LM_BrfImplementer()
:therootObjectImplementer(NULL),
 theLmDir(""),
 theLMBrfcRuntimeOwnerPtr(0)
{
}

ACS_LM_BrfImplementer::ACS_LM_BrfImplementer(string p_impName ):acs_apgcc_objectimplementerinterface_V3(p_impName),
therootObjectImplementer(NULL),
 theLmDir(""),
 theLMBrfcRuntimeOwnerPtr(0)
{
}

ACS_LM_BrfImplementer::ACS_LM_BrfImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ):acs_apgcc_objectimplementerinterface_V3(p_objName, p_impName, p_scope),
		therootObjectImplementer(NULL),
		theLmDir(""),
		theLMBrfcRuntimeOwnerPtr(0)

{
}

//ACS_LM_BrfImplementer::setLmRootObject(void* rootImplementer)
void ACS_LM_BrfImplementer::setLmRootObject(acs_lm_root_objectImplementer* rootImplementer)
{
	//therootObjectImplementer = (acs_lm_root_objectImplementer*)rootImplementer;
	therootObjectImplementer = rootImplementer;
}


bool ACS_LM_BrfImplementer::getBrfStatus()
{
	return isBrfInProgress;
}

void ACS_LM_BrfImplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
	ACS_APGCC_InvocationType invocation,
	const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
	ACS_APGCC_AdminOperationParamType**paramList) 
{
	DEBUG("%s", "------------------------------------------------------------");
	DEBUG("%s", "                   adminOperationCallback called             ");
	DEBUG("%s", "------------------------------------------------------------");


	DEBUG("oiHandle = %d", oiHandle);
	DEBUG("invocation : %d", invocation);
	DEBUG("p_objName  : %d", p_objName);
	DEBUG("operationId: %d", operationId);

	//acs_lm_root_objectImplementer rootObjectImplementer;

	//bool lmActionProgressStatus = rootObjectImplementer.getLmActionStatus();

	bool lmActionProgressStatus = therootObjectImplementer->getLmActionStatus();

	if(!lmActionProgressStatus)
	{
		switch (operationId)
		{
				case   BRF_PARTICIPANT__PERMIT_BACKUP:
					isBrfInProgress = true;
					DEBUG("%s", "RECV: BRM_PARTICIPANT__PERMIT_BACKUP ");
					DEBUG("%s", "isBrfInProgress = true");
				break;
			
				case BRF_PARTICIPANT__COMMIT_BACKUP:
					DEBUG("%s", "RECV: BRM_PARTICIPANT__COMMIT_BACKUP");
					isBrfInProgress = false;
					DEBUG("%s", "isBrfInProgress = false");
				break;
			
				case BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:
					isBrfInProgress = true;
					DEBUG("%s", "RECV: BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP ");
					DEBUG("%s", "isBrfInProgress = true");
				break;
			
				case BRF_PARTICIPANT__CANCEL_BACKUP:
					isBrfInProgress = false;
					DEBUG("%s", "RECV: BRM_PARTICIPANT__CANCEL_BACKUP");
					DEBUG("%s", "isBrfInProgress = false");
				break;
		}
		
		/*start*/
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
				/* ONLY FOR requested parameter */
				case ATTR_UINT64T:
					//DEBUG(" attrName: %s, value: %ld", paramList[i]->attrName, <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues));
					//DEBUG("%s", " attrName: ");
					if(strcmp(paramList[i]->attrName,"requestId")==0)
					{
						requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
						//cout << "requestId assigned " << requestId << endl;
						DEBUG("requestId assigned %d", requestId);
					}
					break;
			
				default:
				break;
			}
			i++;
		}
	
		DEBUG("Received Operation with Identification = %d", operationId);

		int retVal =1;
//		if(operationId == BRM_PARTICIPANT__PERMIT_BACKUP)
//		{
//			if(!isBrfInProgress)
//			{
//				retVal = 0;
//			}
//		}
		int retAdminOperationResult = this->adminOperationResult( oiHandle , invocation, retVal );

		DEBUG("Called adminOperationResult with retVal = %d, returned = %d", retVal, retAdminOperationResult);

		/*if(false)
        	       retVal = this->ResponseToBrfc(requestId, BRF_ERROR_CONFLICT, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
		else
		       DEBUG("%s", ">>>PREPARING REPORT ACTION RESULT DATA");
	               retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
		*/

		DEBUG("%s", "Called adminOperationResult with retVal = ");
		//HERE YOU CAN RESPONDE WITH ACTION RESULT OR ACTION IN PROGRESS
		//BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS simulation
		if (operationId == BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP)
		{
			DEBUG("%s", ">>PREPARING REPORT ACTION PROGRESS DATA");
			retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS);

#if 0
			/************************* Copying Peristent File to PSA *******************************/
                        std::string curLmDir;

                       // if(ACS_LM_Common::copyFileToBuffer(LM_PSO_DISK_PATH,curLmDir)== false)
                        if(ACS_LM_Common::getDataDiskPath("ACS_DATA",curLmDir)== false)
                        {
                                ERROR("ACS_LM_Server::run() %s","ACS_LM_Common::getDataDiskPath(ACS_DATA) FAILED");
                                return;
                        }
                        curLmDir += "/lm";

			std::string persistFile = theLmDir + "/lmdata";
			
			std::string curPersistPath;
			curPersistPath = curLmDir + "/lmdata";
			if(ACS_LM_Common::isFileExists(persistFile))
			{
				if(ACS_LM_Common::deleteFile(persistFile))
				{
					DEBUG("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the BackupPath");
				}
				else
				{
					DEBUG("ACS_LM_Sentinel::installLkf() %s","ERROR!!! LKF file not removed from the BackupPath");
				}
			}

			bool myRestoreResult = ACS_LM_Common::restoreFile(curPersistPath, persistFile);
			if(!myRestoreResult)
			{
				DEBUG("%s", "Restoring Persistent file to PSA Failed!!!");
			}
			/**************************************************************************************/

			/************************* Copying LKF File to PSA *******************************/
                        std::string lkfFile = theLmDir + "/lservrc";

                        std::string curLKFPath;
                        curLKFPath = curLmDir + "/lservrc";
                        if(ACS_LM_Common::isFileExists(lkfFile))
                        {
                        	if(ACS_LM_Common::deleteFile(lkfFile))
                        	{
                        		DEBUG("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the BackupPath");
                        	}
                        	else
                        	{
                        		DEBUG("ACS_LM_Sentinel::installLkf() %s","ERROR!!! LKF file not removed from the BackupPath");
                        	}
                        }
                        myRestoreResult = ACS_LM_Common::restoreFile(curLKFPath, lkfFile);
                       if(!myRestoreResult)
                        {
                                DEBUG("%s", "Restoring License Key File to PSA Failed!!!");
                        }
			/***********************************************************************************/

                        /************************* Copying Alarm Data File to PSA *******************************/
                        std::string alarmFile = theLmDir + "/lmalarmdata";

                        std::string curAlarmPath;
                        curAlarmPath = curLmDir + "/lmalarmdata";

                        if(ACS_LM_Common::isFileExists(alarmFile))
                        {
                        	if(ACS_LM_Common::deleteFile(alarmFile))
                        	{
                        		DEBUG("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the BackupPath");
                        	}
                        	else
                        	{
                        		DEBUG("ACS_LM_Sentinel::installLkf() %s","ERROR!!! LKF file not removed from the BackupPath");
                        	}
                        }
                        bool myRestoreResult3 = ACS_LM_Common::restoreFile(curAlarmPath, alarmFile);
                       if(!myRestoreResult)
                        {
                                DEBUG("%s", "Restoring LM Alarm Data to PSA Failed!!!");
                        }
                        /***********************************************************************************/
#endif

			DEBUG("%s", ">>>PREPARING REPORT ACTION RESULT DATA ");
			//AT the end you must responde with a Report Action Result
			retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
		}
		else
		{
			DEBUG("%s", ">>>PREPARING REPORT ACTION RESULT DATA");
			retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
		}
	}
	else
	{
               /*start*/
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
                                /* ONLY FOR requested parameter */
                                case ATTR_UINT64T:
                                        //DEBUG(" attrName: %s, value: %ld", paramList[i]->attrName, <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues));
                                        //DEBUG("%s", " attrName: ");
                                        if(strcmp(paramList[i]->attrName,"requestId")==0)
                                        {
                                                requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
                                                //cout << "requestId assigned " << requestId << endl;
                                                DEBUG("requestId assigned %d", requestId);
                                        }
                                        break;

                                default:
                                break;
                        }
                        i++;
                }

        		int retVal1 =1;
        		int retAdminOperationResult = this->adminOperationResult( oiHandle , invocation, retVal1 );
        		DEBUG("The result of this->adminOperationResult :%d", retAdminOperationResult);
        		DEBUG("%s", ">>>LM ACTION IN PROGRESS");
                DEBUG("%s", ">>>REJECTING BACKUP REQUEST FROM BRFC");
                int retVal = this->ResponseToBrfc(requestId, BRF_ERROR_CONFLICT, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
                DEBUG("The result of this->ResponseToBrfc :%d", retVal);
	}
} /* end method */


ACS_CC_ReturnType ACS_LM_BrfImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_LM_BrfImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_LM_BrfImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	(void)attrMods;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_LM_BrfImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	return ACS_CC_SUCCESS;
}

void ACS_LM_BrfImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

void ACS_LM_BrfImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

ACS_CC_ReturnType ACS_LM_BrfImplementer::updateRuntime(const char* p_objName, const char** p_attrName)
{
	(void)p_objName;
	(void)p_attrName;
	return ACS_CC_SUCCESS;
}

//void MyImplementer_4::setImpl(MyImplementer_4 *pImpl)
//{
//	theLMBrfcRuntimeOwnerPtr = pImpl;
//	theIsStop = false;
//}

int ACS_LM_BrfImplementer::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode)
{
	DEBUG("%s","Entering ACS_LM_BrfImplementer::ResponseToBrfc() ");
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

	if( responseCode  == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT )
	{
		ACS_APGCC_AdminOperationParamType thirdElem;
		
		//create first Element of parameter list
		firstElem.attrName =firstAttName;
		firstElem.attrType=ATTR_UINT64T;
		// return requestId previously checked
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
	else if (responseCode == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS)
	{
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
	std::string strAttr = "SaImmAttrAdminOwnerName";
	char * lmservername = "acs_lmserverd";
	string aParameterValue = "";
	const std::string strObj(dnObjName1);
	long long int timeOutVal_30sec = 30*(1000000000LL);
	int res1 = admOp.init();

	if (res1 != ACS_CC_SUCCESS)
	{
		ERROR("%s", "ERROR ::admOp.init()FAILED");
		ERROR("ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText());
		return -1;
	}
	int returnValue1 = 1;
	int retry = 0;

	/* Invoke operation  */
	while (admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec) == ACS_CC_FAILURE)
	{
		ERROR("%s", "ERROR ::admOp.adminOperationInvoke()FAILED");
		ERROR("ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText());

		if (admOp.getInternalLastError()!=((-1)*SA_AIS_ERR_EXIST)) break;
		/*Wait at least one second before invoke administrative operations in order to synchronize whit BRFC*/
		sleep(1);
		if (++retry > 60) break;
	}
	ERROR("%s","Exiting of while loop");
	res1 = admOp.finalize();
	fetchParameterIMM(strObj,strAttr,aParameterValue);/* start of HX20968 */
	if(strcmp(aParameterValue.c_str(),lmservername)== 0)
	clearAdminOwner();                                /* end of HX20968 */

		if (res1 != ACS_CC_SUCCESS)
		{
			ERROR("%s", "ACS_LM_BrfImplementer ERROR ::admOp.finalize()FAILED");
			ERROR("ACS_LM_BrfImplementer ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText())
			return -1;
		}
	DEBUG("%s","Leaving ACS_LM_BrfImplementer::ResponseToBrfc() ");
	return 0;
}


/*=================================================================
        ROUTINE: ACS_LM_BrfcThread
=================================================================== */
ACS_LM_BrfcThread::ACS_LM_BrfcThread()
{
        theBrfImplementer = NULL;
        theIsStop = false;
        m_isObjImplAdded = false;
	unsigned int initval = 0U;
	m_StopEvent = eventfd(initval,0);
}//end of ACS_LM_BrfcThread

/*=================================================================
        ROUTINE: ACS_LM_BrfcThread destructor
=================================================================== */
ACS_LM_BrfcThread::~ACS_LM_BrfcThread()
{
	// Close the stop event, usually inside the object destructor
	ACE_OS::close(m_StopEvent);
}//end of destructor

/*
ACS_LM_BrfcThread::setRootObject(acs_lm_root_objectImplementer* rootImplementer)
{
	theLMBrfcRuntimeOwnerPtr = rootImplementer;
}
*/

/*=================================================================
        ROUTINE: setImpl
=================================================================== */
//void ACS_LM_BrfcThread::setImpl(ACS_LM_BrfImplementer *pImpl, acs_lm_root_objectImplementer* rootImplementer)
void ACS_LM_BrfcThread::setImpl(ACS_LM_BrfImplementer *pImpl)
{
        DEBUG("%s", "ACS_LM_BrfcThread::setImpl() Entered %u",pImpl);

        const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-LM,brfParticipantContainerId=1";
        const char *impName = "ACS_LM_BRFCImpl";
        ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

        theBrfImplementer = pImpl;

	theBrfImplementer->setObjName(dnObjName);
	theBrfImplementer->setImpName(impName);
	theBrfImplementer->setScope(scope);

	//theBrfImplementer->setRootObject(rootImplementer);
	//START OF HY67258
	while(!theIsStop)//Implementing the same design as in the case of other ACS blocks like ALOG.
        {
		if(oiHandler.addObjectImpl(theBrfImplementer) != ACS_CC_SUCCESS)
                {
                        //Get the error code
			ERROR("ACS_LM_BrfcThread::setImpl() : Error occurred while adding object implementer for %s retrying again",dnObjName);
                        usleep(3000000);
			continue;
                }
                else
                {
			DEBUG("%s","addObjectImpl successful exiting the while loop");
                        m_isObjImplAdded = true;
                        break;
                }
	}//END OF HY67258	
	theIsStop = false;

	DEBUG("%s", "ACS_LM_BrfcThread::setImpl() Leaving");
}//end of setImpl


/*=================================================================
ROUTINE: stop
=================================================================== */
bool ACS_LM_BrfcThread::stop()
{
	DEBUG("%s","ACS_LM_BrfcThread::stop() Entered")
		// Signal the Stop event
		ACE_UINT64 stopEvent=1;

	// Signal to server to stop
	ssize_t	numByte = ACE_OS::write(m_StopEvent, &stopEvent, sizeof(ACE_UINT64));

	if(sizeof(ACE_UINT64) != numByte)
	{
		ERROR("%s","Error occurred while signaling stop event ");
		ERROR("%s","Leaving ACS_LM_BrfcThread::stop...");
		return false;
	}
	theIsStop=true;        //TR HZ42765
	DEBUG("%s","Leaving ACS_LM_BrfcThread::stop() ");
	return true;
}//end of stop

/*=================================================================
ROUTINE: stop
=================================================================== */
void ACS_LM_BrfcThread::removeImpl(ACS_LM_BrfImplementer *pImpl)
{
	DEBUG("%s","ACS_LM_BrfcThread::removeImpl() Entered, %u",pImpl);
	if(m_isObjImplAdded)
	{
		if(pImpl !=0)
		{
			//theBrfImplementer = pImpl;
			const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-LM,brfParticipantContainerId=1";
			for( int i=0;i < 10; i++)
			{
				if(oiHandler.removeObjectImpl(theBrfImplementer)==ACS_CC_FAILURE)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if(i==9)
					{
						ERROR("ACS_LM_BrfcThread::removeImpl() : Error occured while removing object implementer for %s",dnObjName);
						break;
					}
					else
						continue;
				}

				else
				{
					m_isObjImplAdded = false;
					break;
				}
			}
		}
	}
	DEBUG("%s","Leaving ACS_LM_BrfcThread::removeImpl() ");


}


bool ACS_LM_BrfImplementer::fetchParameterIMM(string RDN,string ParameterName,string & aParameterValue) /* start of HX20968 */
{
	DEBUG("%s","Entering fetchParameterIMM() ");
	aParameterValue = "";
	OmHandler theOmHandlerPtr;
	if( theOmHandlerPtr.Init() == ACS_CC_FAILURE )
	{
		DEBUG("%s","fetchParameterIMM::Init() is failed");
		return false;
	}
	ACS_CC_ImmParameter paramToFind;

	paramToFind.attrName = (char*)ParameterName.c_str();
	if(theOmHandlerPtr.getAttribute( RDN.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
	{
		if(paramToFind.attrValuesNum != 0) {
			char* pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			if(ACE_OS::strcmp(pszAttrValue , "") != 0)
			{
				std::string myStr(pszAttrValue);
				aParameterValue= myStr;
				DEBUG("fetchParameterIMM::getAttribute successful and Value of attribute is %s",myStr.c_str());
			}
		}
		else
			DEBUG("%s","fetchParameterIMM::getAttribute successful and Value of attribute is NULL");
	}
	else
	{//ERROR :Not able to read the parameter from the IMM
		DEBUG("%s","fetchParameterIMM::getAttribute is failed");
		if( theOmHandlerPtr.Finalize() == ACS_CC_FAILURE )
			DEBUG("%s","fetchParameterIMM::Finalize()  is failed");
		return false;
	}
	if( theOmHandlerPtr.Finalize() == ACS_CC_FAILURE )
	{
		DEBUG("%s","fetchParameterIMM::Finalize()  is failed");
		return false;
	}
	DEBUG("%s","Leaving fetchParameterIMM()");
	return true;
}/* end of HX20968 */

void ACS_LM_BrfImplementer::clearAdminOwner() /* start of HX20968 */
{
	const char *dnObjName1 = "brfParticipantContainerId=1";
	SaVersionT version;
	SaImmHandleT immHandle;
	SaAisErrorT errorCode;
	version.releaseCode  = 'A';
	version.majorVersion =  2;
	version.minorVersion = 14; // Support IMM callback timeout
	errorCode = saImmOmInitialize(&immHandle, 0, &version);
	DEBUG("clearAdminOwner()::saImmOmInitialize error code =%d",errorCode);
	if(errorCode == SA_AIS_OK) {
		SaNameT objNameToSendOp = {0,{0}};
		objNameToSendOp.length = strlen (dnObjName1);
		memcpy(objNameToSendOp.value, dnObjName1, objNameToSendOp.length);
		const SaNameT *objOwnedNameList[] = {&objNameToSendOp, 0};
		DEBUG("%s","clearAdminOwner()::Clearing the admin owner as it is still present");
		errorCode = saImmOmAdminOwnerClear(immHandle, objOwnedNameList,SA_IMM_ONE);
		DEBUG("clearAdminOwner()::saImmOmAdminOwnerClear error code =%d",errorCode);
		errorCode = saImmOmFinalize(immHandle);
		DEBUG("clearAdminOwner()::saImmOmFinalize error code = %d",errorCode);
	}
} /* end of HX20968 */

/*=================================================================
ROUTINE: svc
=================================================================== */
int ACS_LM_BrfcThread::svc(void)
{
	DEBUG("%s", "ACS_LM_BrfcThread::svc() Entered");
	std::string strAttr = "SaImmAttrAdminOwnerName"; /* start of HX20968 */
	char * lmservername = "acs_lmserverd";
	string aParameterValue = "";
	std::string strObj  = "brfParticipantContainerId=1";
	theBrfImplementer->fetchParameterIMM(strObj,strAttr,aParameterValue);
	if(strcmp(aParameterValue.c_str(),lmservername)== 0)
	theBrfImplementer->clearAdminOwner();            /* end of HX20968 */

	// Create a fd to wait for request
	const nfds_t nfds = 2;
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_StopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = theBrfImplementer->getSelObj();
	fds[1].events = POLLIN;

	ACE_INT32 poolResult;
	ACS_CC_ReturnType result;

	// waiting for IMM requests or stop
	while(true)
	{
		poolResult = ACE_OS::poll(fds, nfds);

		if( 0 == poolResult )
		{
			if(errno == EINTR)
			{
				continue;
			}
			ERROR("ACS_LM_BrfcThread::svc(), exit after error=%s", strerror(errno) );
			ERROR("%s", "ACS_LM_BrfcThread::svc() Leaving svc(), exit after poll error");
			break;
		}

		if(fds[0].revents & POLLIN)
		{
			// Received a stop request from server
			DEBUG("%s", "ACS_LM_BrfcThread::svc() Leaving svc(), received a stop request from server");
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			// Received a IMM request on a CpVolume
			result = theBrfImplementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			if(ACS_CC_SUCCESS != result)
			{
				ERROR("%s", "ACS_LM_BrfcThread::svc() Leaving svc(), error on BrfImplementer dispatch event");
			}
			continue;
		}
	}
	return 0;
}//end of svc
