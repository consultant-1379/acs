//******************************************************************************
//
//  NAME
//     acs_lm_electronickey_runtimeowner.cpp
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
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */

#include "acs_lm_electronickey_runtimeowner.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <sys/eventfd.h>
//#include <stringstream>
using namespace std;
/*=================================================================
ROUTINE: ACS_LM_ElectronicKey_RuntimeOwner constructor
=================================================================== */
	ACS_LM_ElectronicKey_RuntimeOwner::ACS_LM_ElectronicKey_RuntimeOwner()
:theOmHandlerPtr(0), theSentinelptr(0)
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::ACS_LM_ElectronicKey_RuntimeOwner()  %s","Entering");
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::ACS_LM_ElectronicKey_RuntimeOwner()  %s","Leaving");
}//end of constructor
/*=================================================================
ROUTINE: ACS_LM_ElectronicKey_RuntimeOwner destructor
=================================================================== */
ACS_LM_ElectronicKey_RuntimeOwner::~ACS_LM_ElectronicKey_RuntimeOwner()
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::~ACS_LM_ElectronicKey_RuntimeOwner()  %s","Entering");
	clearLisenceKeyList();
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::~ACS_LM_ElectronicKey_RuntimeOwner()  %s","Leaving");
}//end of destructor
/*=================================================================
ROUTINE: setInternalOMhandler
=================================================================== */

void ACS_LM_ElectronicKey_RuntimeOwner::setInternalOMhandler(OmHandler*  aOmHandlerPtr)
{
	theOmHandlerPtr = aOmHandlerPtr;
}//end of setInternalOMhandler
/*=================================================================
ROUTINE: initElectronicKeyLMRunTimeOwner
=================================================================== */
bool ACS_LM_ElectronicKey_RuntimeOwner::initElectronicKeyLMRunTimeOwner()
{

	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::initElectronicKeyLMRunTimeOwner()  %s","Entering");

	if(init(ACS_LM_ELECTRONICKEYIMPLEMENTER) == ACS_CC_SUCCESS)
	{
		DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::initElectronicKeyLMRunTimeOwner()  %s","Succes!");
		return true;
	}
	else
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::initElectronicKeyLMRunTimeOwner()  %s","Failed");
		return false;
	}

	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::initElectronicKeyLMRunTimeOwner()  %s","Leaving");

}//end of initElectronicKeyLMRunTimeOwner
/*=================================================================
ROUTINE: updateElectronicKeyRDNList
=================================================================== */
bool ACS_LM_ElectronicKey_RuntimeOwner::updateElectronicKeyRDNList()
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::updateElectronicKeyRDNList()  %s","Entering");
	ACS_CC_ReturnType myResult = ACS_CC_SUCCESS;
	theElectronicKeyRDNList.clear();
	std::vector<std::string> p_dnList;
	myResult = theOmHandlerPtr->getClassInstances(ELECTRONICKEY_CLASSNAME,p_dnList);
	if ( myResult != ACS_CC_SUCCESS)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateElectronicKeyRDNList()  %s","Failed");
		return false;
	}
	std::vector<std::string>::iterator myItr = p_dnList.begin();
	std::vector<std::string>::iterator myItrEnd = p_dnList.end();
	for(;myItr != myItrEnd;++myItr)
	{
		theElectronicKeyRDNList.push_back(*myItr);
	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::updateElectronicKeyRDNList()  %s","Leaving");
	return true;
}//end of updateElectronicKeyRDNList
/*=================================================================
ROUTINE: finalizeElectronicKeyLMRunTimeOwner
=================================================================== */
bool ACS_LM_ElectronicKey_RuntimeOwner::finalizeElectronicKeyLMRunTimeOwner()
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::finalizeElectronicKeyLMRunTimeOwner()  %s","Entering");
	if(finalize() != ACS_CC_SUCCESS)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::finalizeElectronicKeyLMRunTimeOwner()  %s","Failed");
		return false;
	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::finalizeElectronicKeyLMRunTimeOwner()  %s","Leaving");
	return true;
}//end of finalizeElectronicKeyLMRunTimeOwner
/*=================================================================
ROUTINE: setSentinelObjectPtr
=================================================================== */
void ACS_LM_ElectronicKey_RuntimeOwner::setSentinelObjectPtr(ACS_LM_Sentinel* aSentinelptr)
{
	theSentinelptr = aSentinelptr;
}//end of setSentinelObjectPtr
/*=================================================================
ROUTINE: getSerialNumberRDNObject
=================================================================== */
string ACS_LM_ElectronicKey_RuntimeOwner::getSerialNumberRDNObject(const char* aOBJName)
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::getSerialNumberRDNObject()  %s","Entering");
	std::string myRDNString(aOBJName);
	size_t myPos1 = myRDNString.find_first_of("=");
	size_t myPos2 = myRDNString.find_first_of(",");
	string mySerialNumber =  myRDNString.substr(myPos1+1,myPos2-myPos1-1);
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::getSerialNumberRDNObject()  %s","Leaving");
	return mySerialNumber;
}//end of getSerialNumberRDNObject

/*=================================================================
ROUTINE: getLicenseKeyObject
=================================================================== */
LkData* ACS_LM_ElectronicKey_RuntimeOwner::getLicenseKeyObject(string aLicenseSerialNumber)
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::getLicenseKeyObject()  %s","Entering");
	std::list<LkData*>::iterator myItr = theLKlist.begin();
	std::list<LkData*>::iterator myItrEnd = theLKlist.end();
	string licenseSerialNum;
	for( ; myItr != myItrEnd; ++myItr)
	{
		licenseSerialNum = (*myItr)->lkId;

		if (licenseSerialNum == aLicenseSerialNumber)
		{
			return *myItr;
		}

	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::getLicenseKeyObject()  %s","Leaving");
	return 0;
}//end of getLicenseKeyObject
/*=================================================================
ROUTINE: updateCallback
=================================================================== */
ACS_CC_ReturnType ACS_LM_ElectronicKey_RuntimeOwner::updateCallback(const char* aOBJName, const char** aAttrName)
{
	(void)aAttrName;
	const std::string strTemp = "T00:00:00";
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","Entering");
	//Obtaining the SerialNumber of the LicenseKey from RDN string
	string myLicenseSerialNumber = getSerialNumberRDNObject(aOBJName);

	ACS_CC_ImmParameter parToModify;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	//Obtaining the LicenseKey Object from LKLIST based on Serial Number
	LkData* myLKPtr = getLicenseKeyObject(myLicenseSerialNumber);

	if(myLKPtr == 0)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","Failed- Unable to get the License Key pointer");
		return ACS_CC_FAILURE;
	}

	//Updating the LicenseKeyID
#if 0
	char* myLicenseIdValue = const_cast<char*>(myLKPtr->lkId.c_str());

	//	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_LICENSE_ID;
	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_RDN_ID;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValue[1] = {reinterpret_cast<void*>(myLicenseIdValue)};
	parToModify.attrValues = attrValue;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","modifyRuntimeObj of ELECTRONICKEY_RDN_ID Failed ");
		result =  ACS_CC_FAILURE;
	}
#endif
	//Updating the capacityValue
#if 0
	std::ostringstream myOstr;
	myOstr << myLKPtr->value;
	std::string myTempStr = myOstr.str();

	char* myLicenseValue = const_cast<char*>(myTempStr.c_str());
#endif
	int * myLicenseValue=new int(myLKPtr->value);
	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_VALUE;
	parToModify.attrType = ATTR_UINT32T;
	parToModify.attrValuesNum = 1;
	void* attrValueLicenseValue[1] = {reinterpret_cast<void*>(myLicenseValue)};
	parToModify.attrValues = attrValueLicenseValue;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","modifyRuntimeObj of ELECTRONICKEY_VALUE Failed ");
		result =  ACS_CC_FAILURE;
	}

	//Updating the StartDate
	std::string strStartDate("");
	strStartDate = myLKPtr->startDate;
	strStartDate = strStartDate.substr(0,strStartDate.length()-1);
	strStartDate.append(strTemp);

	replace(strStartDate.begin(),strStartDate.end(),'/','-');

	char* myLicenseStartDate = const_cast<char*>(strStartDate.c_str());
	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_STARTDATE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValueStartDate[1] = {reinterpret_cast<void*>(myLicenseStartDate)};
	parToModify.attrValues = attrValueStartDate;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","modifyRuntimeObj of ELECTRONICKEY_STARTDATE Failed ");
		result =  ACS_CC_FAILURE;
	}

	//Updating the EndDate
	std::string strEndDate("");
	strEndDate = myLKPtr->endDate;

	if(myLKPtr->endDate != "NEVER")
	{
		strEndDate = strEndDate.substr(0,(strEndDate.length()-1));
		strEndDate.append(strTemp);
		replace(strEndDate.begin(),strEndDate.end(),'/','-');
	}
	DEBUG("---------- ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s strTemp%s", strEndDate.c_str(), strTemp.c_str());
	char* myLicenseEndDate = const_cast<char*>(strEndDate.c_str());
	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_EXPIRYDATE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValueEndDate[1] = {reinterpret_cast<void*>(myLicenseEndDate)};
	parToModify.attrValues = attrValueEndDate;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","modifyRuntimeObj of ELECTRONICKEY_EXPIRYDATE Failed ");
		result =  ACS_CC_FAILURE;
	}

	//Updating the Description
	char* myLicenseDescription = const_cast<char*>(myLKPtr->vendorInfo.c_str());
	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_DESCRIPTION;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValueDescription[1] = {reinterpret_cast<void*>(myLicenseDescription)};
	parToModify.attrValues = attrValueDescription;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","modifyRuntimeObj of ELECTRONICKEY_DESCRIPTION Failed ");
		result =  ACS_CC_FAILURE;
	}

	//Updating the daysToExpire
	std::string days_temp;
	if(myLKPtr->endDate.find("NEVER")== string::npos)
	{
		ACE_INT64 days = daysToExpire(myLKPtr->endDate);
		if(days > 0)
		{
			ostringstream convert;
			convert << days;
			days_temp = convert.str();
			INFO("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  daysToExpire=%d", days);
		}
		else
		{
			days_temp.assign("EXPIRED");
			INFO("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s", "License key is expired");
		}
	}
	else
	{
		days_temp = myLKPtr->endDate;
		INFO("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s", "License key will never expired");
	}

	char* myDaysToExpire = const_cast<char*>(days_temp.c_str());
	parToModify.attrName = (ACE_TCHAR *)ELECTRONICKEY_DAYS_TO_EXPIRE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValueDaysExpire[1] = {reinterpret_cast<void*>(myDaysToExpire)};
	parToModify.attrValues = attrValueDaysExpire;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","modifyRuntimeObj of ELECTRONICKEY_VALUE Failed ");
		result =  ACS_CC_FAILURE;
	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::updateCallback()  %s","Leaving ");
	return ACS_CC_SUCCESS;
}//end of updateCallback
/*=================================================================
ROUTINE: adminOperationCallback
=================================================================== */
void ACS_LM_ElectronicKey_RuntimeOwner::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
}//end of adminOperationCallback
/*=================================================================
ROUTINE: createRuntimeObjectElectronicKey
=================================================================== */
ACS_CC_ReturnType ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKey(std::string & aRDNName)
{

	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKey()  %s","Entering ");
	ACS_CC_ReturnType returnCode;

	//The list of attributes
	vector<ACS_CC_ValuesDefinitionType> myAttrList;

	//the attributes
	ACS_CC_ValuesDefinitionType myAttributeRDN;
	//	ACS_CC_ValuesDefinitionType myAttributeLicenseId;
	ACS_CC_ValuesDefinitionType myAttributeValue;
	ACS_CC_ValuesDefinitionType myAttributeStartDate;
	ACS_CC_ValuesDefinitionType myAttributeExpiryDate;
	ACS_CC_ValuesDefinitionType myAttributeDescription;
	ACS_CC_ValuesDefinitionType myAttributeDaysToExpire;


	//Fill the rdn Attribute
	char myCharArrayAttributeRDN[] =  ELECTRONICKEY_RDN_ID;
	myAttributeRDN.attrName = myCharArrayAttributeRDN;
	myAttributeRDN.attrType = ATTR_STRINGT;
	myAttributeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>((char*)aRDNName.c_str())};
	myAttributeRDN.attrValues = valueRDN;
#if 0
	char myCharArrayAttributeLicenseId[] =  ELECTRONICKEY_LICENSE_ID;
	myAttributeLicenseId.attrName =  myCharArrayAttributeLicenseId;
	myAttributeLicenseId.attrType = ATTR_STRINGT;
	myAttributeLicenseId.attrValuesNum = 0;
	myAttributeLicenseId.attrValues = 0;
	//	myAttributeLicenseId.attrValuesNum = 1;
	//	myAttributeLicenseId.attrValues = aLkData.lkId;
#endif


	char myCharArrayAttributeValue[] =  ELECTRONICKEY_VALUE;
	myAttributeValue.attrName = myCharArrayAttributeValue;
	myAttributeValue.attrType = ATTR_UINT32T;
	myAttributeValue.attrValuesNum = 0;
	myAttributeValue.attrValues = 0;
	//myAttributeValue.attrValuesNum = 1;
	//myAttributeValue.attrValues = aLkData.value;

	char myCharArrayAttributeStartDate[] =  ELECTRONICKEY_STARTDATE;
	myAttributeStartDate.attrName = myCharArrayAttributeStartDate;
	myAttributeStartDate.attrType = ATTR_STRINGT;
	myAttributeStartDate.attrValuesNum = 0;
	myAttributeStartDate.attrValues = 0;
	//myAttributeStartDate.attrValuesNum = 1;
	//myAttributeStartDate.attrValues = aLkData.startDate;

	//Fill the attribute saImmSSULSA1ALimit
	char myCharArrayAttributeExpiryDate[] =  ELECTRONICKEY_EXPIRYDATE;
	myAttributeExpiryDate.attrName = myCharArrayAttributeExpiryDate;
	myAttributeExpiryDate.attrType = ATTR_STRINGT;
	myAttributeExpiryDate.attrValuesNum = 0;
	myAttributeExpiryDate.attrValues = 0;
	//	myAttributeExpiryDate.attrValuesNum = 1;
	//myAttributeExpiryDate.attrValues = aLkData.endDate;

	char myCharArrayAttributeDescription[] =  ELECTRONICKEY_DESCRIPTION;
	myAttributeDescription.attrName = myCharArrayAttributeDescription;
	myAttributeDescription.attrType = ATTR_STRINGT;
	myAttributeDescription.attrValuesNum = 0;
	myAttributeDescription.attrValues = 0;
	//	myAttributeDescription.attrValuesNum = 1;
	//myAttributeDescription.attrValues = aLkData.vendorInfo;

	char myCharArrayAttributeDaysToExpire[] =  ELECTRONICKEY_DAYS_TO_EXPIRE;
	myAttributeDaysToExpire.attrName = myCharArrayAttributeDaysToExpire;
	myAttributeDaysToExpire.attrType = ATTR_STRINGT;
	myAttributeDaysToExpire.attrValuesNum = 0;
	myAttributeDaysToExpire.attrValues = 0;

	myAttrList.push_back(myAttributeRDN);
	//	myAttrList.push_back(myAttributeLicenseId);
	myAttrList.push_back(myAttributeValue);
	myAttrList.push_back(myAttributeStartDate);
	myAttrList.push_back(myAttributeExpiryDate);
	myAttrList.push_back(myAttributeDescription);
	myAttrList.push_back(myAttributeDaysToExpire);
	//	std::string myParentRDN(LICENSEMODE_RDNVALUE);
	//	myParentRDN += ",";

	std::string myParentRDN(ACS_LM_Common::dnOfLMRoot);
	std::string myCompleteRDN = aRDNName + "," + myParentRDN;

	DEBUG("Class Name = %s", ELECTRONICKEY_CLASSNAME);
	DEBUG("Parent RDN = %s", myParentRDN.c_str());
	returnCode = createRuntimeObj(ELECTRONICKEY_CLASSNAME, myParentRDN.c_str(), myAttrList);

	if(returnCode == ACS_CC_SUCCESS)
	{
		theElectronicKeyRDNList.push_back(myCompleteRDN);
	}
	else
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKey()  Creation Runtime object %s failed",aRDNName.c_str());
	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKey()  %s","Leaving");
	return returnCode;
}//end of createRuntimeObjectElectronicKey
/*=================================================================
ROUTINE: createRuntimeObjectElectronicKeyObjects
=================================================================== */
bool ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects()
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects()  %s","Entering");
	if( theSentinelptr == 0)
	{
		INFO("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects()  %s","Leaving theSentinelptr is NULL ");
		return false;
	}
	clearLisenceKeyList();
	ACE_UINT32 daysToExpire = 0;
	if(theSentinelptr->getAllLks(theLKlist, daysToExpire))
	{
		clearAllElectronicKeysRunTimeObjectsinIMM();
		ACE_UINT32 totalLks = (ACE_UINT32)theLKlist.size();
		if(totalLks != 0)
		{
			for(std::list<LkData*>::iterator itr=theLKlist.begin(); itr != theLKlist.end(); ++itr)
			{
				std::string myStr(ELECTRONICKEY_RDN_ID);
				myStr += "=";
				myStr +=(*itr)->lkId;
				//int i;
				INFO("************ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects(), electronic key = %s",myStr.c_str());
				ACS_CC_ReturnType myReturnCode;
				for(int i=0; i<10 ; i++)
				{
					myReturnCode = createRuntimeObjectElectronicKey(myStr);
					if( myReturnCode != ACS_CC_SUCCESS )
					{
						for(int j=0; j< 50; ++j) ; //do nothing loop to wait for sometime, better than sleeping
						ERROR("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects() failed for electronic key %s, %d",myStr.c_str(),i);
						if(i>=9)
						{
							ERROR("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects() failed for electronic key %s for %d times",myStr.c_str(),i);
							return false;
						}
						continue;
					}
					else
					{
						INFO("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects() Success for electronic key %s, %d",myStr.c_str(),i);
						break;
					}
				}
			}
		}

	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::createRuntimeObjectElectronicKeyObjects()  %s","Leaving");
	return true;
}//end of createRuntimeObjectElectronicKeyObjects
/*=================================================================
ROUTINE: deleteRuntimeObjectElectronicKey
=================================================================== */
ACS_CC_ReturnType ACS_LM_ElectronicKey_RuntimeOwner::deleteRuntimeObjectElectronicKey(std::string& aRDNName)
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::deleteRuntimeObjectElectronicKey()  %s","Entering");
	ACS_CC_ReturnType returnCode;
	returnCode = deleteRuntimeObj(aRDNName.c_str());
	if(returnCode == ACS_CC_SUCCESS)
	{
		INFO("ACS_LM_ElectronicKey_RuntimeOwner::deleteRuntimeObjectElectronicKey()  %s","Passed");
	}
	else
	{
		ERROR("ACS_LM_ElectronicKey_RuntimeOwner::deleteRuntimeObjectElectronicKey()   aRDNName = %s Failed",aRDNName.c_str());
	}
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::deleteRuntimeObjectElectronicKey()  %s","Leaving");
	return returnCode;

}//end of deleteRuntimeObjectElectronicKey
/*=================================================================
ROUTINE: clearLisenceKeyList
=================================================================== */
void ACS_LM_ElectronicKey_RuntimeOwner::clearLisenceKeyList()
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::clearLisenceKeyList()  %s","Entering");
	std::list<LkData*>::iterator myItr = theLKlist.begin();
	std::list<LkData*>::iterator myItrEnd = theLKlist.end();
	for( ; myItr != myItrEnd; ++myItr)
	{
		delete *myItr;
	}
	theLKlist.clear();
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::clearLisenceKeyList()  %s","Leaving");
}//end of clearLisenceKeyList
/*=================================================================
ROUTINE: clearAllElectronicKeysRunTimeObjectsinIMM
=================================================================== */
bool ACS_LM_ElectronicKey_RuntimeOwner::clearAllElectronicKeysRunTimeObjectsinIMM()
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::clearAllElectronicKeysRunTimeObjectsinIMM()  %s","Entering");
	std::list<std::string>::iterator myItr = theElectronicKeyRDNList.begin();
	std::list<std::string>::iterator myItrEnd = theElectronicKeyRDNList.end();
	ACS_CC_ReturnType myReturnCode;
	for( ; myItr != myItrEnd; ++myItr)
	{
		for(int i=0; i<10 ; i++)
		{
			myReturnCode = deleteRuntimeObjectElectronicKey(*myItr);
			if(myReturnCode != ACS_CC_SUCCESS)
			{
				for(int j=0; j< 50; ++j) ; //do nothing loop to wait for sometime, better than sleeping
				ERROR("ACS_LM_ElectronicKey_RuntimeOwner::clearAllElectronicKeysRunTimeObjectsinIMM() failed for %s:%d",(*myItr).c_str(),i);
				if(i >= 9)
				{
					ERROR("ACS_LM_ElectronicKey_RuntimeOwner::clearAllElectronicKeysRunTimeObjectsinIMM() failed more than 10 times for %s",(*myItr).c_str());
					return false;
				}
				continue;
			}
			else
			{
				INFO("ACS_LM_ElectronicKey_RuntimeOwner::clearAllElectronicKeysRunTimeObjectsinIMM() Successful for %s:%d",(*myItr).c_str(),i);
				break;
			}
		}

	}
	theElectronicKeyRDNList.clear();
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::clearAllElectronicKeysRunTimeObjectsinIMM()  %s","Leaving");
	return true;
}//end of clearAllElectronicKeysRunTimeObjectsinIMM
/*=================================================================
ROUTINE: daysToExpire
=================================================================== */
ACE_INT64 ACS_LM_ElectronicKey_RuntimeOwner::daysToExpire(string expDate)
{
	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::daysToExpire  %s","Entering");
	std::string year = expDate.substr(0, 4);
	std::string monthAndDate = expDate.substr(5);
	size_t mmPos = monthAndDate.find_first_of("/");
	std::string month = monthAndDate.substr(0, mmPos);
	std::string day = monthAndDate.substr(mmPos+1);

	time_t curTimeUtc = ::time(NULL);
	struct tm lkExpTime = *localtime(&curTimeUtc);
	lkExpTime.tm_year = ::atoi(year.c_str()) - 1900;
	lkExpTime.tm_mon = ::atoi(month.c_str()) - 1;
	lkExpTime.tm_mday = ::atoi(day.c_str());
	lkExpTime.tm_hour = 0;
	lkExpTime.tm_min = 0;
	lkExpTime.tm_sec = 0;

	struct tm curTimeLocal = *localtime(&curTimeUtc);
	curTimeLocal.tm_hour = 0;
	curTimeLocal.tm_min = 0;
	curTimeLocal.tm_sec = 0;
	time_t curTime = mktime(&curTimeLocal);
	time_t expTime = mktime(&lkExpTime);
	ACE_INT64 daysToExpire = ((ACE_INT64)(difftime(expTime, curTime)/(60*60*24))) + 1;

	DEBUG("ACS_LM_ElectronicKey_RuntimeOwner::daysToExpire  %s","Leaving");
	return daysToExpire;
}//end of daysToExpire
/*=================================================================
ROUTINE: ACS_LM_ElectronicKey_RuntimeHandlerThread
=================================================================== */
ACS_LM_ElectronicKey_RuntimeHandlerThread::ACS_LM_ElectronicKey_RuntimeHandlerThread()
{
	theElectronicKeyRuntimeOwnerImplementer = NULL;
	theIsStop = false;
	unsigned int initval = 0U;
	m_StopEvent = eventfd(initval,0);
}//end of ACS_LM_ElectronicKey_RuntimeHandlerThread

/*=================================================================
ROUTINE: ACS_LM_ElectronicKey_RuntimeHandlerThread destructor
=================================================================== */
ACS_LM_ElectronicKey_RuntimeHandlerThread::~ACS_LM_ElectronicKey_RuntimeHandlerThread()
{
	// Close the stop event, usually inside the object destructor
	ACE_OS::close(m_StopEvent);
}//end of destructor
/*=================================================================
ROUTINE: setImpl
=================================================================== */
void ACS_LM_ElectronicKey_RuntimeHandlerThread::setImpl(ACS_LM_ElectronicKey_RuntimeOwner *pImpl)
{
	theElectronicKeyRuntimeOwnerImplementer = pImpl;
	theIsStop = false;

}//end of setImpl
/*=================================================================
ROUTINE: stop
=================================================================== */
bool ACS_LM_ElectronicKey_RuntimeHandlerThread::stop()
{
	DEBUG("%s","ACS_LM_ElectronicKey_RuntimeHandlerThread::stop() Entered")
		// Signal the Stop event
		ACE_UINT64 stopEvent=1;

	// Signal to server to stop
	ssize_t numByte = ACE_OS::write(m_StopEvent, &stopEvent, sizeof(ACE_UINT64));

	if(sizeof(ACE_UINT64) != numByte)
	{
		ERROR("%s","Error occurred while signaling stop event ");
		ERROR("%s","Leaving ACS_LM_ElectronicKey_RuntimeHandlerThread::stop...");
		return false;
	}
	// theIsStop=true;
	DEBUG("%s","Leaving ACS_LM_ElectronicKey_RuntimeHandlerThread::stop() ");
	return true;
}//end of stop

/*=================================================================
ROUTINE: svc
=================================================================== */
int ACS_LM_ElectronicKey_RuntimeHandlerThread::svc(void)
{
	DEBUG("%s", "ACS_LM_ElectronicKey_RuntimeHandlerThread::svc() Entered");
	// Create a fd to wait for request
	const nfds_t nfds = 2;
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_StopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = theElectronicKeyRuntimeOwnerImplementer->getSelObj();
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
			ERROR("ACS_LM_ElectronicKey_RuntimeHandlerThread::svc(), exit after error=%s", strerror(errno) );
			ERROR("%s", "ACS_LM_ElectronicKey_RuntimeHandlerThread::svc() Leaving svc(), exit after poll error");
			break;
		}

		if(fds[0].revents & POLLIN)
		{
			// Received a stop request from server
			DEBUG("%s", "ACS_LM_ElectronicKey_RuntimeHandlerThread::svc() Leaving svc(), received a stop request from server");
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			// Received a IMM request on a CpVolume
			result = theElectronicKeyRuntimeOwnerImplementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			if(ACS_CC_SUCCESS != result)
			{
				ERROR("%s", "ACS_LM_ElectronicKey_RuntimeHandlerThread::svc() Leaving svc(), error on BrfImplementer dispatch event");
			}
			continue;
		}
	}
	return 0;
}//end of svc
