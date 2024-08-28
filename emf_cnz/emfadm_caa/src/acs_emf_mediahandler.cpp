#include <acs_emf_mediahandler.h>
#include <acs_emf_nanousbhandler.h>
#include <acs_emf_dvdhandler.h>
#include <acs_emf_common.h>
#include <acs_emf_commandhandler.h>
#include "acs_emf_execute.h"
#include <ctime>
#include <ace/ACE.h>
#include <stdio.h>
#include <sys/stat.h>
#include <mntent.h>
#include <iostream>
#include <string.h>

using namespace std;
//ACS_EMF_RuntimeHandler myEMFRunTimeHandler1;
ACE_INT32 ACS_EMF_MEDIAHandler::cntValue = 0;

ACS_EMF_MEDIAHandler::ACS_EMF_MEDIAHandler()
:m_stopFlag(false)
{
	m_mediaAttachedLocally=false;
}

ACS_EMF_MEDIAHandler::~ACS_EMF_MEDIAHandler()
{

}


ACS_EMF_MEDIAHandler* ACS_EMF_MEDIAHandler::create(int hwVersion,ACE_HANDLE endEvent)
{
	ACS_EMF_MEDIAHandler* mediahandler = NULL;
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::create - Entering");
	if(hwVersion >= 3)
	{
		//create obj of type nanousb
		mediahandler= new ACS_EMF_NANOUSBHandler(endEvent);
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::create - Create ACS_EMF_NANOUSBHandler object");
	}
	else
	{
		//create object of type dvd
		mediahandler= new ACS_EMF_DVDHandler(endEvent);
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::create - Create ACS_EMF_DVDHandler object");
	}    
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::create - Leaving");
	return mediahandler;
}


/*===================================================================
   ROUTINE: initEMFHistRuntimeHandler
=================================================================== */

void ACS_EMF_MEDIAHandler::initEMFHistRuntimeHandler()
{
	DEBUG(1,"%s", "ACS_EMF_MEDIAHandler::initEMFHistRuntimeHandler - Entering")
	DEBUG(1,"ACS_EMF_MEDIAHandler::initEMFHistRuntimeHandler - cntValue=[%d]",cntValue);
	if (cntValue > 20) cntValue = 20;
	INFO(1,"ACS_EMF_MEDIAHandler::initEMFHistRuntimeHandler - No of History Object Instances present are %d",cntValue);
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::initEMFHistRuntimeHandler - Leaving");
}//End of initEMFHistRuntimeHandler


/*===================================================================
	   ROUTINE: setLastHistOperStateToIDLEIfPROCESSING
=================================================================== */
void ACS_EMF_MEDIAHandler::setLastHistOperStateToIDLEIfPROCESSING()
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::setLastHistOperStateToIDLEIfPROCESSING - Entering");
	ACE_INT32 result;
	char retriveData[ACS_EMF_ARRAYSIZE];

	// For emfRtHistoryInfoId=1 object
	std::string dnName;
	dnName.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
	dnName.append(",");
	dnName.append(ACS_EMF_Common::parentDNofEMF);

	initEMFHistRuntimeHandler();

	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnName,(ACE_TCHAR*) theEMFHistoryInfoNodeState,retriveData)) == ACS_CC_SUCCESS)
	{
		if (ACE_OS::strcmp(retriveData,(ACE_TCHAR*)PROCESSING) == 0)
		{
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoNodeState,(ACE_TCHAR*)IDLE);
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoOperResult,(ACE_TCHAR*)FAILED);
		}
	}

	// For CurrentTransactionInfo
	dnName.clear();
	dnName.append(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
	dnName.append(",");
	dnName.append(ACS_EMF_Common::parentDNofEMF);

	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnName,(ACE_TCHAR*) theEMFHistoryInfoNodeState,retriveData)) == ACS_CC_SUCCESS)
	{
		if (ACE_OS::strcmp(retriveData,(ACE_TCHAR*)PROCESSING) == 0)
		{
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoNodeState,(ACE_TCHAR*)IDLE);
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoOperResult,(ACE_TCHAR*)FAILED);
		}
	}
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::setLastHistOperStateToIDLEIfPROCESSING - Leaving");
}//End of setLastHistOperStateToIDLEIfPROCESSING


/*===================================================================
   ROUTINE: isObjectExiststheEMFInfoClassName
=================================================================== */
bool ACS_EMF_MEDIAHandler::isObjectExiststheEMFInfoClassName(std::string aRdnName,std::string dn,bool& isObjectAvailable)
{
	bool myResult = true;
	ACS_CC_ImmParameter paramToFind;
	paramToFind.attrName = new char[30];
	ACE_OS::strcpy(paramToFind.attrName,aRdnName.c_str());
	if (immHandle.getAttribute(dn.c_str(), &paramToFind ) == ACS_CC_FAILURE)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::isObjectExiststheEMFInfoClassName - getAttribute failed");
		isObjectAvailable = false;
	}
	else
	{
		//INFO(1,"%s is already preset in IMM.No need to create it.",dn.c_str());
		isObjectAvailable = true;
	}

	delete [] paramToFind.attrName;
	return myResult;

}//End of isObjectExiststheEMFInfoClassName
/*===================================================================
   ROUTINE: createRuntimeObjectsforEmfHistoryInfoClass
=================================================================== */
bool ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass(std::string& attrRDN)
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - Entering");
	if(m_stopFlag)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - Leaving");
		return false;
	}

	ACE_INT32 numAttr = 9;
	bool flag = true;
	//The list of attributes
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	//the attributes
	ACE_TCHAR* newRDN = (ACE_TCHAR*)(attrRDN.c_str());
	DEBUG(1,"ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - RDN of the History Object that is going to be created is  %s",newRDN);
	ACS_CC_ValuesDefinitionType *attributes = new ACS_CC_ValuesDefinitionType[numAttr];
	ACE_TCHAR* valstr = (char *) "";
	ACE_INT32 num = 0;

	//Fill the theEMFHistoryInfoRDN Attribute
	attributes[0].attrName = (char *)theEMFHistoryInfoRDN;
	attributes[0].attrType = ATTR_STRINGT;
	attributes[0].attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(newRDN)};
	attributes[0].attrValues = valueRDN;
	AttrList.push_back(attributes[0]);

	//Fill the attribute theEMFHistoryInfoNodeName
	attributes[1].attrName =  (char *)theEMFHistoryInfoNodeName;
	attributes[1].attrType = ATTR_STRINGT;
	attributes[1].attrValuesNum = 1;
	void* valueString1[1]={reinterpret_cast<void*>(valstr)};
	attributes[1].attrValues = valueString1;
	AttrList.push_back(attributes[1]);

	//Fill the attribute theEMFHistoryInfoNodeState
	attributes[2].attrName =  (char *)theEMFHistoryInfoNodeState;
	attributes[2].attrType = ATTR_STRINGT;
	attributes[2].attrValuesNum = 1;
	void* valueString2[1]={reinterpret_cast<void*>(valstr)};
	attributes[2].attrValues = valueString2;
	AttrList.push_back(attributes[2]);

	//Fill the attribute theEMFHistoryInfoLastOperation
	attributes[3].attrName = (char *)theEMFHistoryInfoLastOperation;
	attributes[3].attrType = ATTR_INT32T;
	attributes[3].attrValuesNum = 1;
	void* valueString3[1]={reinterpret_cast<void*>(&num)};
	attributes[3].attrValues = valueString3;
	AttrList.push_back(attributes[3]);

	//Fill the attribute theEMFHistoryInfoOperResult
	attributes[4].attrName = (char *)theEMFHistoryInfoOperResult;
	attributes[4].attrType = ATTR_STRINGT;
	attributes[4].attrValuesNum = 1;
	void* valueString4[1]={reinterpret_cast<void*>(valstr)};
	attributes[4].attrValues =valueString4;
	AttrList.push_back(attributes[4]);

	//Fill the attribute theEMFHistoryInfoUser
	attributes[5].attrName = (char *)theEMFHistoryInfoUser;
	attributes[5].attrType = ATTR_STRINGT;
	attributes[5].attrValuesNum = 1;
	void* valueString5[1]={reinterpret_cast<void*>(valstr)};
	attributes[5].attrValues = valueString5;
	AttrList.push_back(attributes[5]);

	//Fill the attribute theEMFHistoryInfoStartTime
	attributes[6].attrName = (char *)theEMFHistoryInfoStartTime;
	attributes[6].attrType = ATTR_STRINGT;
	attributes[6].attrValuesNum = 1;
	void* valueString6[1]={reinterpret_cast<void*>(valstr)};
	attributes[6].attrValues = valueString6;
	AttrList.push_back(attributes[6]);

	//Fill the attribute theEMFHistoryInfoEndTime
	attributes[7].attrName = (char *)theEMFHistoryInfoEndTime;
	attributes[7].attrType = ATTR_STRINGT;
	attributes[7].attrValuesNum = 1;
	void* valueString7[1]={reinterpret_cast<void*>(valstr)};
	attributes[7].attrValues = valueString7;
	AttrList.push_back(attributes[7]);

	//Fill the attribute theEMFHistoryInfoFileName
	attributes[8].attrName = (char *)theEMFHistoryInfoFileName;
	attributes[8].attrType = ATTR_STRINGT;
	attributes[8].attrValuesNum = 1;
	void* valueString8[1]={reinterpret_cast<void*>(valstr)};
	attributes[8].attrValues = valueString8;
	AttrList.push_back(attributes[8]);
	if (immHandle.createObject(theEMFHistoryInfoClassName, ACS_EMF_Common::parentDNofEMF.c_str(), AttrList ) == ACS_CC_FAILURE)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - Creation of Runtime object for ExternalMediaHistory is Failed!!!");
		DEBUG(1,"ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - ERROR CODE: %d",immHandle.getInternalLastError());
		DEBUG(1,"ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - ERROR MESSAGE: %s",immHandle.getInternalLastErrorText());
		flag = false;
	}
	AttrList.clear();
	delete[] attributes;
	ACE_OS::sleep(1);
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createRuntimeObjectsforEmfHistoryInfoClass - Leaving");
	return flag;
}//End of createRuntimeObjectsforEmfHistoryInfoClass

/*===================================================================
   ROUTINE: copyObjToObj
=================================================================== */
bool ACS_EMF_MEDIAHandler::copyObjToObj(ACE_TCHAR* aObjectRdn1,ACE_TCHAR* aObjectRdn2)
{
	bool myObjectAvailable = false;
	ACE_INT32 result;
	std::string dnNameOffirstObj,dnNameOfSecondObj;

	dnNameOffirstObj.append(aObjectRdn1);
	dnNameOffirstObj.append(",");
	dnNameOffirstObj.append(ACS_EMF_Common::parentDNofEMF.c_str());

	dnNameOfSecondObj.append(aObjectRdn2);
	dnNameOfSecondObj.append(",");
	dnNameOfSecondObj.append(ACS_EMF_Common::parentDNofEMF.c_str());

	std::string rdnOfsecondObj;
	rdnOfsecondObj.append(aObjectRdn2);

	std::string rdnNameofSecondObj;
	rdnNameofSecondObj.append(theEMFHistoryInfoRDN);

	bool flag = true;
	char retriveData[ACS_EMF_ARRAYSIZE];
	bool  myIMMResult = isObjectExiststheEMFInfoClassName(rdnNameofSecondObj,dnNameOfSecondObj,myObjectAvailable);

	if (!myIMMResult)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::copyObjToObj - isObjectExiststheEMFInfoClassName execution failed");
	}

	if( !myObjectAvailable)
	{
		if(!createRuntimeObjectsforEmfHistoryInfoClass(rdnOfsecondObj))
		{
			ERROR(1,"%s","ACS_EMF_MEDIAHandler::copyObjToObj - createRuntimeObjectsforEmfHistoryInfoClass is failed");
			//return -1;
		}
	}
	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoNodeName,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *) dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoNodeName ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}

	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoNodeState,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoNodeState ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	int operType = 0;
	if((result = ACS_EMF_Common::fetchEMFHistoryIntegerAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoLastOperation,operType)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryIntegerAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoLastOperation , operType))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoOperResult,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoOperResult ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoUser,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoUser ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoStartTime,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoStartTime ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoEndTime,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoEndTime ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnNameOffirstObj,(char *)theEMFHistoryInfoFileName,retriveData)) == ACS_CC_SUCCESS)
	{
		if(!modifyEMFHistoryAttribute((char *)dnNameOfSecondObj.c_str(),(char *)theEMFHistoryInfoFileName ,retriveData))
		{
			flag = false;
		}
	}
	else
	{
		flag = false;
	}
	return flag;
}//End of copyObjToObj
/*===================================================================
   ROUTINE: modifyInitialHistoryAttrs
=================================================================== */
void ACS_EMF_MEDIAHandler::modifyInitialHistoryAttrs(ACE_UINT32 attrValue,std::string fileNames)
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyInitialHistoryAttrs - Entering");
	std::string dnName;
	dnName.append(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
	dnName.append(",");
	dnName.append(ACS_EMF_Common::parentDNofEMF);

	//emfNodeName
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoNodeName,(ACE_TCHAR*)ACS_EMF_Common::GetHostName());
	//emfNodeState
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoNodeState,(ACE_TCHAR*)PROCESSING);
	//emfUser
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoUser,(ACE_TCHAR*) ACS_EMF_Common::GetUserName());
	//emfOperstarttime
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoStartTime,(ACE_TCHAR*)ACS_EMF_Common::GetDateTimeString());
	//emfOperendtime
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*)theEMFHistoryInfoEndTime,(ACE_TCHAR*)"");
	//emfOperResult
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoOperResult,(ACE_TCHAR*)"");
	if(attrValue == EMF_CMD_TYPE_COPYFROMMEDIA)
	{
		//emfCurrentOrLastOperation
		modifyEMFHistoryIntegerAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*)theEMFHistoryInfoLastOperation,FROMDVD);

	}
	if((attrValue == EMF_CMD_TYPE_COPYTOMEDIA) ||(attrValue == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA))
	{
		//emfCurrentOrLastOperation
		modifyEMFHistoryIntegerAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoLastOperation,TODVD);

	}

	//emfFileName
	DEBUG(1,"ACS_EMF_MEDIAHandler::modifyInitialHistoryAttrs - List of FileNames Received: %s\n",fileNames.c_str());
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoFileName,(ACE_TCHAR*)(fileNames.c_str()));
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyInitialHistoryAttrs - Leaving");
}//End of modifyInitialHistoryAttrs

/*===================================================================
   ROUTINE: modifyEMFHistoryIntegerAttribute
=================================================================== */
bool ACS_EMF_MEDIAHandler::modifyEMFHistoryIntegerAttribute(char* aObjectDN , char* attrName,int value)
{
        ACS_CC_ImmParameter parToModify;
        ACS_CC_ReturnType enResult;
        ACE_TCHAR* dnName = const_cast<char*>(aObjectDN);

        parToModify.attrName = attrName;
        parToModify.attrType = ATTR_INT32T;
        parToModify.attrValuesNum = 1;

        parToModify.attrValues = new void*[1]; /* Modified due to Coverity Defect */
        parToModify.attrValues[0] = reinterpret_cast<void*>(&value);

        if( (enResult = immHandle.modifyAttribute(dnName, &parToModify)) != ACS_CC_SUCCESS )
        {
                ACE_TCHAR szText[ACS_EMF_MAX_LENGTH];
                snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
                                ACE_TEXT("%d:Error when modifying the IMM parameter \"%s\""),
								__LINE__,
								(ACE_TCHAR*)attrName);
				ERROR(1,"%s","ACS_EMF_DVDHandler::modifyEMFHistoryIntegerAttribute - Error when modifying the IMM parameter\n");
				delete[] parToModify.attrValues;
				parToModify.attrValues = 0;
				return false;
		}
		delete[] parToModify.attrValues;
		parToModify.attrValues = 0;
		return true;
}//End of modifyEMFHistoryIntegerAttribute
void ACS_EMF_MEDIAHandler::modifyEndHistoryAttrs(ACE_TCHAR* attribute)
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyEndHistoryAttrs - Entering");
	std::string dnName;
	dnName.append(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
	dnName.append(",");
	dnName.append(ACS_EMF_Common::parentDNofEMF);

	//emfNodeState
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoNodeState,(ACE_TCHAR*)IDLE);
	//emfOperResult
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() , (ACE_TCHAR*)theEMFHistoryInfoOperResult,attribute);
	//emfOperendtime
	modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*)theEMFHistoryInfoEndTime,(ACE_TCHAR*)ACS_EMF_Common::GetDateTimeString());

	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyEndHistoryAttrs - Leaving");

}
/*===================================================================
   ROUTINE: modifyEMFHistoryAttribute
=================================================================== */
bool ACS_EMF_MEDIAHandler::modifyEMFHistoryAttribute(char* aObjectDN , char* attrName,char* value)
{
	ACS_CC_ImmParameter parToModify;
	ACS_CC_ReturnType enResult;
	ACE_TCHAR* dnName = const_cast<char*>(aObjectDN);

	parToModify.attrName = attrName;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	//void* new_value[1] = {reinterpret_cast<void*>(value)};
	//parToModify.attrValues = new_value;

        parToModify.attrValues = new void*[1];
        parToModify.attrValues[0] = reinterpret_cast<void*>(value);

	if( (enResult = immHandle.modifyAttribute(dnName, &parToModify)) != ACS_CC_SUCCESS )
	{
		ACE_TCHAR szText[128];
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when modifying the IMM parameter \"%s\""),
				__LINE__,
				(ACE_TCHAR*)attrName);
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyEMFHistoryAttribute - Error when modifying the IMM parameter");
		delete[] parToModify.attrValues;
		 parToModify.attrValues = 0;
		return false;
	}
	delete[] parToModify.attrValues;
	parToModify.attrValues = 0;
	return true;
}//End of modifyEMFHistoryAttribute


ACE_INT32 ACS_EMF_MEDIAHandler::getHistoryInstancesCnt()
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::getHistoryInstancesCnt - Entering");
	OmHandler immHandle;
	ACS_CC_ReturnType result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS )
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: OmHandler Initialization FAILURE!!! in modifyStructObjInitialAttr");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}
	std::vector<std::string> p_dnList;
	result = immHandle.getClassInstances(theEMFHistoryInfoClassName, p_dnList);
	if(result != ACS_CC_SUCCESS)
	{
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Method OmHandler::getCLassInstances Failure!!!");
		DEBUG(1,"ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR CODE: %d",immHandle.getInternalLastError());
		DEBUG(1,"ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR MESSAGE: %s",immHandle.getInternalLastErrorText());
		immHandle.Finalize();
	}
	int instancesNumber  = p_dnList.size();
	DEBUG(1,"ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - instancesNumber = %d",instancesNumber);
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving ACS_EMF_MEDIAHandler::getHistoryInstancesCnt");
	if (instancesNumber > 0)
	{
		instancesNumber = instancesNumber - 1;
	}
	immHandle.Finalize();
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
	return instancesNumber;
}//End of getHistoryInstancesCnt

/*===================================================================
   ROUTINE: modifyStructObjInitialAttr
=================================================================== */
ACE_INT32 ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr(int operationType)
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Entering");
	OmHandler immHandle;
	ACS_CC_ReturnType res;
	res = immHandle.Init();
	if ( res != ACS_CC_SUCCESS )
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: OmHandler Initialization FAILURE!!!");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}
	std::string dnOfEMFStruObj(theEMFStruRDN);
	dnOfEMFStruObj.append(",");
	dnOfEMFStruObj.append(ACS_EMF_Common::parentDNofEMF);
	DEBUG(1," ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - dnOfEMFStruObj=[%s]",dnOfEMFStruObj.c_str());

	ACS_CC_ImmParameter paramToChange;
	int actionName = operationType;

	// actionName
	paramToChange.attrName = new ACE_TCHAR [30];
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruActionName);
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&actionName);



	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify actionName attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// state
	int stateOfAction = RUNNING;
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruState);
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&stateOfAction);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify state attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;	
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// result
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruResult);
	int actionResult = NOT_AVAIALABLE;
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&actionResult);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify result attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}
	
	// resultInfo
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruResultInfo);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValues[0] = reinterpret_cast<void*>((char *) "-");
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify resultInfo attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// actionStartTime
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruActionStartTime);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValues[0] = reinterpret_cast<void*>((ACE_TCHAR*)ACS_EMF_Common::GetDateTimeWithoutOffset());
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify actionStartTime attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// actionEndTime
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruActionEndTime);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValues[0] = reinterpret_cast<void*>((char *) "-");
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify actionEndTime attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// lastUpdateTime
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruLastUpdatedTime);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValues[0] = reinterpret_cast<void*>((ACE_TCHAR*)ACS_EMF_Common::GetDateTimeWithoutOffset());
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - ERROR: Unable to modify latUpdatedTime attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
		return EMF_RC_ERROR;
	}

	immHandle.Finalize();
	delete [] paramToChange.attrName;
	delete [] paramToChange.attrValues;
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjInitialAttr - Leaving");
	return EMF_RC_OK;
}//End of modifyStructObjInitialAttr

/*===================================================================
   ROUTINE: modifyStructObjFinalAttr
=================================================================== */
ACE_INT32 ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr(int resultOfaction, std::string reasonForFailure)
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Entering");
	OmHandler immHandle;
	ACS_CC_ReturnType res;
	res = immHandle.Init();
	if ( res != ACS_CC_SUCCESS )
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - ERROR: OmHandler Initialization FAILURE!!!");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Leaving");
		return EMF_RC_ERROR;
	}

	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrName = new ACE_TCHAR [30];
	std::string dnOfEMFStruObj(theEMFStruRDN);
	dnOfEMFStruObj.append(",");
	dnOfEMFStruObj.append(ACS_EMF_Common::parentDNofEMF);



#if 0
	int stateOfAction = FINISHED;

	// state
	paramToChange.attrName = new ACE_TCHAR [30];
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruState);
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&stateOfAction);

	std::string dnOfEMFStruObj(theEMFStruRDN);
	dnOfEMFStruObj.append(",");
	dnOfEMFStruObj.append(ACS_EMF_Common::parentDNofEMF);

	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ERROR: Unable to modify state attribute");
		DEBUG(1,"%s","Leaving ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		return -1;
	}
#endif


	// result
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruResult);
	int actionResult = resultOfaction;
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&actionResult);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - ERROR: Unable to modify result attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// resultInfo
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruResultInfo);
	paramToChange.attrType = ATTR_STRINGT;
        ACE_TCHAR reason[ACS_EMF_ARRAYSIZE];
        ACE_OS::strcpy(reason,reasonForFailure.c_str());
	
        paramToChange.attrValues[0] = reinterpret_cast<void*>(reason);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - ERROR: Unable to modify resultInfo attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// actionEndTime
	char* endTime = (ACE_TCHAR*)ACS_EMF_Common::GetDateTimeWithoutOffset();
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruActionEndTime);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValues[0] = reinterpret_cast<void*>(endTime);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - ERROR: Unable to modify actionEndTime attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Leaving");
		return EMF_RC_ERROR;
	}

	// lastUpdateTime
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruLastUpdatedTime);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValues[0] = reinterpret_cast<void*>(endTime);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str() , &paramToChange);

	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - ERROR: Unable to modify latUpdatedTime attribute");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete [] paramToChange.attrValues;
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Leaving");
		return -1;
	}
	immHandle.Finalize();
	delete [] paramToChange.attrName;
	delete [] paramToChange.attrValues;
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjFinalAttr - Leaving");
	return EMF_RC_OK;
}

/*===================================================================
   ROUTINE: moveObjectsOneLevelDown
=================================================================== */
bool ACS_EMF_MEDIAHandler::moveObjectsOneLevelDown(int countValue)
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::moveObjectsOneLevelDown - Entering");
	ACE_TCHAR fromRDN[50];
	ACE_TCHAR toRDN[50];

	for (int i = countValue; i > 1;  i--)
	{
		if(m_stopFlag)
		{
			ERROR(1,"%s","ACS_EMF_MEDIAHandler::moveObjectsOneLevelDown - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
			DEBUG(1,"%s","ACS_EMF_MEDIAHandler::moveObjectsOneLevelDown - Leaving");
			return false;
		}
		sprintf(fromRDN,"%s=%d",theEMFHistoryInfoRDN,i-1);
		sprintf(toRDN,"%s=%d",theEMFHistoryInfoRDN,i);
		if(!copyObjToObj(fromRDN,toRDN))//Sending DVD by default as there is no use of this function in USBINT
		{
			ERROR(1,"%s","ACS_EMF_MEDIAHandler::moveObjectsOneLevelDown - Failure in copyObjToObj\n");
			return false;
		}
	}
			DEBUG(1,"%s","ACS_EMF_MEDIAHandler::moveObjectsOneLevelDown - Leaving");
	return true;
}//End of moveObjectsOneLevelDown


//#if 0
///*===================================================================
//   ROUTINE: validateFileNamesToMedia
//=================================================================== */
//bool ACS_EMF_MEDIAHandler::validateFileNamesToMedia(std::vector<std::string> fileList)
//{
//	DEBUG(1,"%s","validateFileNamesToMedia - Entering");
//	bool result = true;
//
//	for(int i = fileList.size() - 1 ; i >= 0 ; i--)
//	{
//		DEBUG(1,"validateFileNamesToMedia - File name recieved for validation is %s\n",fileList[i].c_str());
//
//		ACE_INT32 dwResult = ACS_EMF_Common::ValidateFileArgumentToMedia(fileList[i].c_str());
//		if (dwResult != EMF_RC_OK)
//		{
//			int errorId = 0;
//			ACE_TCHAR errorText[150];
//			ACE_OS::strcpy(errorText,ACS_EMF_Execute::GetResultCodeText(dwResult, &errorId));
//			DEBUG(1,"validateFileNamesToMedia - ERROR:%s",errorText);
//			DEBUG(1,"validateFileNamesToMedia - EXIT CODE = %d",errorId);
//			//ACS_EMF_Common::setErrorText(0, errorId, errorText);
//			//acs_emf_commandhandler::setExitCode(errorId, errorText);
//			DEBUG(1,"%s","validateFileNamesToMedia - Leaving");
//			return false;
//		}
//
//	}
//	DEBUG(1,"%s","validateFileNamesToMedia - Leaving");
//	return result;
//}//End of validateFileNamesToMedia
//#endif
/*===================================================================
   ROUTINE: createStructObj
=================================================================== */
ACE_INT32 ACS_EMF_MEDIAHandler::createStructObj()
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - Entering");
	//OmHandler immHandle;
	ACS_CC_ReturnType res;
	std::vector<std::string> p_dnList;
	char* p_className = const_cast<char *>("AxeExternalMediaExternalMediaActionProgress");

	res = immHandle.getClassInstances(p_className, p_dnList);
	if(res != ACS_CC_SUCCESS)
	{
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - Method OmHandler::getCLassInstances Failure!!!");
		DEBUG(1,"ACS_EMF_MEDIAHandler::createStructObj - ERROR CODE: %d",immHandle.getInternalLastError());
		DEBUG(1,"ACS_EMF_MEDIAHandler::createStructObj - ERROR MESSAGE: %s",immHandle.getInternalLastErrorText());
	}
	int instancesNumber  = p_dnList.size();
	DEBUG(1,"ACS_EMF_MEDIAHandler::createStructObj - Structure instances = %d",instancesNumber);

	if (instancesNumber == 0)
	{		// Create a strut objects
		char* parentObj = const_cast<char *>("AxeExternalMediaexternalMediaMId=1");


		/*The vector of attributes*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;
		/*the attributes*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType attr2;
		ACS_CC_ValuesDefinitionType attr3;
		ACS_CC_ValuesDefinitionType attr4;
		ACS_CC_ValuesDefinitionType attr5;
		ACS_CC_ValuesDefinitionType attr6;
		ACS_CC_ValuesDefinitionType attr7;
		ACS_CC_ValuesDefinitionType attr8;

		/*Fill the rdn Attribute */
		char attrdn[]= "id";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_STRINGT;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = const_cast<char *>("id=emfInstance");
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] = reinterpret_cast<void*>(rdnValue);


		/*Fill the actionName */
		char actionName[]="actionName";
		attr2.attrName = actionName;
		attr2.attrType = ATTR_INT32T;
		attr2.attrValuesNum = 0;

		/*Fill the result */
		char result[]="result";
		attr3.attrName = result;
		attr3.attrType = ATTR_INT32T;
		attr3.attrValuesNum = 0;

		/*Fill the resultInfo */
		char resultInfo[]= "resultInfo";
		attr4.attrName = resultInfo;
		attr4.attrType = ATTR_STRINGT;
		attr4.attrValuesNum = 0;

		/*Fill the actionStartTime */
		char actionStartTime[]= "actionStartTime";
		attr5.attrName = actionStartTime;
		attr5.attrType = ATTR_STRINGT;
		attr5.attrValuesNum = 0;

		/*Fill the actionEndTime */
		char actionEndTime[]= "actionEndTime";
		attr6.attrName = actionEndTime;
		attr6.attrType = ATTR_STRINGT;
		attr6.attrValuesNum = 0;

		/*Fill the lastUpdatedTime */
		char lastUpdatedTime[]= "lastUpdatedTime";
		attr7.attrName = lastUpdatedTime;
		attr7.attrType = ATTR_STRINGT;
		attr7.attrValuesNum = 0;

		/*Fill the state */
		char state[]="state";
		attr8.attrName = state;
		attr8.attrType = ATTR_INT32T;
		attr8.attrValuesNum = 0;

		/**/
		AttrList.push_back(attrRdn);
		AttrList.push_back(attr2);
		AttrList.push_back(attr3);
		AttrList.push_back(attr4);
		AttrList.push_back(attr5);
		AttrList.push_back(attr6);
		AttrList.push_back(attr7);
		AttrList.push_back(attr8);
		res = immHandle.createObject(p_className, parentObj, AttrList );

		if( res == ACS_CC_SUCCESS )
		{
			DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - structure object is created successfully");
			DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - Leaving");
			delete [] attrRdn.attrValues;
			attrRdn.attrValues = 0;
			return EMF_RC_OK;
		}
		else
		{
			DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - structure object creation is failed !!");
			ERROR(1,"ACS_EMF_MEDIAHandler::createStructObj - ERROR CODE: %d",immHandle.getInternalLastError());
			ERROR(1,"ACS_EMF_MEDIAHandler::createStructObj - ERROR MESSAGE: %s",immHandle.getInternalLastErrorText());
			DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - Leaving");
			delete [] attrRdn.attrValues;
			attrRdn.attrValues = 0;
			return EMF_RC_ERROR;
		}
	}
	else
	{
		// Struct object is alreay created.
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - structure object is already present in IMM");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::createStructObj - Leaving");
		return EMF_RC_OK;
	}
}

ACE_INT32 ACS_EMF_MEDIAHandler::modifyStructObjState()
{
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjState - Entering");
	OmHandler immHandle;
	ACS_CC_ReturnType res;
	res = immHandle.Init();
	if ( res != ACS_CC_SUCCESS )
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjState - ERROR: OmHandler Initialization FAILURE!!! in modifyStructObjState");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjState - Leaving");
		return EMF_RC_ERROR;
	}

	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrName = new ACE_TCHAR [30];
	int stateOfAction = FINISHED;

	// state
	ACE_OS::strcpy( paramToChange.attrName ,theEMFStruState);
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&stateOfAction);

	std::string dnOfEMFStruObj(theEMFStruRDN);
	dnOfEMFStruObj.append(",");
	dnOfEMFStruObj.append(ACS_EMF_Common::parentDNofEMF);
	res = immHandle.modifyAttribute( dnOfEMFStruObj.c_str(),&paramToChange);
	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjState - ERROR: Unable to modify state attribute");
		DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjState - Leaving");
		immHandle.Finalize();
		delete [] paramToChange.attrName;
		delete []  paramToChange.attrValues;
		return EMF_RC_ERROR;
	}
	immHandle.Finalize();
	delete [] paramToChange.attrName;
	delete []  paramToChange.attrValues;
	DEBUG(1,"%s","ACS_EMF_MEDIAHandler::modifyStructObjState - Leaving");
	return EMF_RC_OK;
}

void ACS_EMF_MEDIAHandler::stopMediaOperation()
{
	// Exit signal for all thread.
	m_stopFlag=true;
	// Log message
	ERROR(1,"%s","ACS_EMF_MEDIAHandler::stopMediaOperation - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
}
