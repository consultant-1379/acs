//******************************************************************************
//
//  NAME
//     acs_lm_testlkf.cpp
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
#include "acs_lm_testlkf.h"
//#include "ACS_LM_TraceMacro.h"
#include "acs_lm_tra.h"
/*=================================================================
	ROUTINE: ACS_LM_TestLkf constructor
=================================================================== */
ACS_LM_TestLkf::ACS_LM_TestLkf(ACS_LM_Persistent* persistFile)
:persistFile(persistFile)
{
DEBUG("ACS_LM_TestLkf::ACS_LM_TestLkf() %s","Entering");
DEBUG("ACS_LM_TestLkf::ACS_LM_TestLkf() %s","Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_TestLkf destructor
=================================================================== */
ACS_LM_TestLkf::~ACS_LM_TestLkf()
{
	DEBUG("ACS_LM_TestLkf::~ACS_LM_TestLkf() %s","Entering");

	for(std::list<Lk*>::iterator it=lkList.begin(); 
		it!= lkList.end(); ++it)
    {
		Lk* lk = (*it);
		delete lk;
	}

	lkList.clear();
	DEBUG("ACS_LM_TestLkf::~ACS_LM_TestLkf() %s","Leaving");
}//end of destructor

/*=================================================================
	ROUTINE: addTestLk
=================================================================== */
ACS_LM_AppExitCode ACS_LM_TestLkf::addTestLk(const LkData& lkData)
{
	DEBUG("ACS_LM_TestLkf::addTestLk() %s","Entering");

    ACS_LM_AppExitCode retCode = ACS_LM_RC_OK;
    bool lkFound = false;
	std::list<Lk*> tempLkList;
	copyList(tempLkList,lkList);
	//Search the List for the existing LK, if the LK found in list, it will modify the LK
    for(std::list<Lk*>::iterator it=lkList.begin(); 
		it!= lkList.end(); ++it)
    {	

		std::string id = (*it)->id;
		if(id.compare(lkData.lkId) == 0)
	    {
			//Update the LK
			DEBUG("addTestLk() Test LK already exists with LKID = %s",(lkData.lkId).c_str());
			(*it)->id = lkData.lkId;
			(*it)->paramName = lkData.paramName;
			(*it)->setName = lkData.setName;
			(*it)->status = lkData.status;
			(*it)->value = lkData.value;
		    
			lkFound = true;
		}
    }

    //If LK not found in List, it will add a new element to the list
    if(!lkFound)
    {
		DEBUG("%s","addTestLk(): adding Test LK");
	    Lk* lk = new Lk();
		lk->id = lkData.lkId;
		lk->paramName = lkData.paramName;
		lk->setName = lkData.setName;
		lk->status = lkData.status;
		lk->value = lkData.value;
	    lkList.push_back(lk);
    }

	if(retCode == ACS_LM_RC_OK)
	{
		if(persistFile->commit())
		{
			DEBUG("%s","addTestLk(): Commit() successful");
		}
		else
		{
			copyList(lkList,tempLkList);
			tempLkList.clear();
            retCode = ACS_LM_RC_PHYFILEERROR;
			DEBUG("%s","addTestLk(): Commit() failed");

		}
	}
	DEBUG("Exit addTestLk(): retCode = %d",retCode);
        
	DEBUG("ACS_LM_TestLkf::addTestLk() %s","Leaving");

    return retCode;	
}//end of addTestLk

//=============================================================================
//removetestLK - remove the test LK from the List and to the physical file.
//=============================================================================
/*=================================================================
	ROUTINE: removeTestLk
=================================================================== */
ACS_LM_AppExitCode ACS_LM_TestLkf::removeTestLk(const std::string& lkid)
{
	DEBUG("ACS_LM_TestLkf::removeTestLk() %s","Entering");

	Lk* tempLk = NULL;
	std::list<Lk*> tempLkList;
	copyList(tempLkList,lkList);

	ACS_LM_AppExitCode retCode = ACS_LM_RC_OK;
    bool lkFound =  false;	
	//Search for the given LK in the list and remove the LK from the list
    for(std::list<Lk*>::iterator it = lkList.begin(); it!= lkList.end(); ++it)
    {
		std::string id = (*it)->id;
		if(id.compare(lkid) == 0)
	    {
			DEBUG("removeTestLK() Test LK : %s is removed",lkid.c_str());
			tempLk = *it;
			lkFound = true;
		   	lkList.erase(it);
		    it = lkList.begin();
			break;
		}
    }

    if(!lkFound)
    {
		//If the given key not found returnt KEYNOTFOUND to operator
		retCode = ACS_LM_RC_LKEYNOTFOUND;
    }
	
	if(retCode == ACS_LM_RC_OK)
	{
		if(persistFile->commit())
		{
			if(tempLk != NULL)
			{
				delete tempLk;
			}
			DEBUG("%s","removeTestLk():Commit() successful");

		}
		else
		{
			copyList(lkList,tempLkList);			
			tempLkList.clear();	
            retCode = ACS_LM_RC_PHYFILEERROR;
			DEBUG("%s","removeTestLk():Commit() failed");
		}
	}

	DEBUG("Exit removeTestLk():retCode = ",retCode);

	DEBUG("ACS_LM_TestLkf::removeTestLk() %s","Leaving");
    return retCode;	
}//end of removeTestLk

//====================================================================================
//listtestlks - list all the LKs in the list and store them to a vector to be returned.
//====================================================================================
/*=================================================================
	ROUTINE: getTestLks
=================================================================== */
ACS_LM_AppExitCode ACS_LM_TestLkf::getTestLks(std::list<LkData*>& attr)
{
	DEBUG("ACS_LM_TestLkf::getTestLks() %s","Entering");

	ACS_LM_AppExitCode retCode = ACS_LM_RC_OK;

	//add all the list keys to the vector and returns the vector.
    for(std::list<Lk*>::reverse_iterator rit=lkList.rbegin();
		rit!=lkList.rend(); ++rit)
    {
		LkData *lkData = new LkData();
	    lkData->lkId = (*rit)->id;
	    lkData->paramName = (*rit)->paramName;
	    lkData->setName = (*rit)->setName;
	    lkData->status = (*rit)->status;
	    lkData->value = (*rit)->value;
	    attr.push_back(lkData);
	}
	DEBUG("getTestLks() attr.size() = %d, retCode = %d",(int)attr.size(),retCode);
	DEBUG("ACS_LM_TestLkf::getTestLks() %s","Leaving");
    return retCode;	
}//end of getTestLks
/*=================================================================
	ROUTINE: removeAllTestLks
=================================================================== */
ACS_LM_AppExitCode ACS_LM_TestLkf::removeAllTestLks()
{
	DEBUG("ACS_LM_TestLkf::removeAllTestLks() %s","Entering");

	ACS_LM_AppExitCode retCode = ACS_LM_RC_OK;
//    bool lkFound =  false;

    //Search for the given LK in the list and remove the LK from the list
    for(std::list<Lk*>::iterator it=lkList.begin(); 
		it!= lkList.end(); ++it)
    {
		DEBUG("removeAllTestLKs(): Test LK %s is removed",(*it)->id.c_str());
		delete (*it);	
    }
	lkList.clear();
	DEBUG("Exit removeAllTestLKs():retCode = ",retCode);
	DEBUG("ACS_LM_TestLkf::removeAllTestLks() %s","Leaving");
    return retCode;	
}//end of removeAllTestLks
/*=================================================================
	ROUTINE: restoreAllTestLks
=================================================================== */
void ACS_LM_TestLkf::restoreAllTestLks()
{
	DEBUG("ACS_LM_TestLkf::restoreAllTestLks() %s","Entering");

	copyList(lkList,tempLKList);
	DEBUG("restoreAllTestLks(): size of lklist %d ",(int)lkList.size());
    tempLKList.clear();

	DEBUG("ACS_LM_TestLkf::restoreAllTestLks() %s","Leaving");
}//end of restoreAllTestLks
/*=================================================================
	ROUTINE: removeAllTestLksTemp
=================================================================== */
ACS_LM_AppExitCode ACS_LM_TestLkf::removeAllTestLksTemp()
{
	DEBUG("ACS_LM_TestLkf::removeAllTestLksTemp() %s","Entering");

    ACS_LM_AppExitCode retCode = ACS_LM_RC_OK;
    INFO("%s","In ACS_LM_TestLkf::removeAllTestLksTemp");
    tempLKList.clear();
	copyList(tempLKList,lkList);
    lkList.clear();


    DEBUG("ACS_LM_TestLkf::removeAllTestLksTemp() %s","leaving");
    return retCode;
}//end of removeAllTestLksTemp
/*=================================================================
	ROUTINE: removeAllTestLksPerm
=================================================================== */
ACS_LM_AppExitCode ACS_LM_TestLkf::removeAllTestLksPerm()
{
    DEBUG("ACS_LM_TestLkf::removeAllTestLksPerm() %s","Entering");

    ACS_LM_AppExitCode retCode = ACS_LM_RC_OK;
     for(std::list<Lk*>::iterator it=tempLKList.begin(); 
		it!= tempLKList.end(); ++it)
    {
		DEBUG("removeAllTestLKsPerm(): Test LK %s is removed",(*it)->id.c_str());
		delete (*it);	
    }
	tempLKList.clear();

    DEBUG("ACS_LM_TestLkf::removeAllTestLksPerm() %s","Leaving");
    return retCode;
}//end of removeAllTestLksPerm
/*=================================================================
	ROUTINE: store
=================================================================== */
void ACS_LM_TestLkf::store(ACE_TCHAR * destBuf, ACE_UINT64& destPos)
{
    DEBUG("ACS_LM_TestLkf::store() %s","Entering");
	/* To avoid switching to Grace mode from License Controlled mode during UP
	   Installation, lkCount data type changed from ACE_UNIT64 to ACE_UINT32 */
	ACE_UINT32 lkCount = (ACE_UINT64)lkList.size();
	
	DEBUG("Size of license key list : %d",(ACE_UINT64)lkList.size());

	ACS_LM_Common::encode32(destBuf, destPos, lkCount);

	for(std::list<Lk*>::iterator it = lkList.begin();
		it != lkList.end(); ++it)
	{
        Lk* lk = (*it);
		lk->toBytes(destBuf, destPos);
	}	
    DEBUG("ACS_LM_TestLkf::store() %s","Leaving");
}//end of store
/*=================================================================
	ROUTINE: sizeInBytes
=================================================================== */
ACE_UINT64 ACS_LM_TestLkf::sizeInBytes()
{
	return ((ACE_UINT64)(lkList.size()*TESTLKF_RECORD_LEN)+4);
}//end of sizeInBytes
/*=================================================================
	ROUTINE: load
=================================================================== */
ACS_LM_TestLkf* ACS_LM_TestLkf::load(ACS_LM_Persistent* persistFile)
{
    DEBUG("ACS_LM_TestLkf::load(ACS_LM_Persistent*) %s","Entering");

	ACS_LM_TestLkf* testLkf = new ACS_LM_TestLkf(persistFile);

    DEBUG("ACS_LM_TestLkf::load() %s","leaving");
	return testLkf;
}//end of load
/*=================================================================
	ROUTINE: load
=================================================================== */
ACS_LM_TestLkf* ACS_LM_TestLkf::load(ACS_LM_Persistent* persistFile, ACE_TCHAR * srcBuff, ACE_UINT64& srcPos)
{
    DEBUG("%s","Entering ACS_LM_TestLkf::load");

	INFO("In ACS_LM_TestLkf::load,srcPos = %d",srcPos);

	ACS_LM_TestLkf* testLkf = new ACS_LM_TestLkf(persistFile);

	ACE_UINT64 lkCount = ACS_LM_Common::decode32(srcBuff, srcPos);
	DEBUG("ACS_LM_TestLkf::load lkCount =%d ",lkCount);
	if (lkCount > 1024)
	{
		DEBUG("ACS_LM_TestLkf::load lkCount is greater than 1024","%s");
		lkCount = 0;
	}//if NO of testkey more then the no of loop is more.Creating usage of memory
	for(ACE_UINT64 i=0; i<lkCount; i++)
	{
		Lk* lk = Lk::fromBytes(srcBuff, srcPos);
		testLkf->lkList.push_back(lk);
	}
	
    DEBUG("%s","Leaving ACS_LM_TestLkf::load");
    return testLkf;
}//end of load
/*=================================================================
	ROUTINE: copyList
=================================================================== */
void ACS_LM_TestLkf::copyList(std::list<Lk*>& destList, const std::list<Lk*>& srcList)
{
    DEBUG("ACS_LM_TestLkf::copyList() %s","Entering");

	int srcListSize = (int)srcList.size();
	DEBUG("copyList(): srcListSize = %d",srcListSize);
	if(!destList.empty())
	{
		destList.clear();
	}
	for(std::list<Lk*>::const_iterator itr = srcList.begin();
		itr!= srcList.end(); ++itr)
	{
		destList.push_back(*itr);
	}
	DEBUG("copyList() destListSize = %d",(int)destList.size());

    DEBUG("ACS_LM_TestLkf::copyList() %s","Leaving");
}//end of copyList
/*=================================================================
	ROUTINE: reload
=================================================================== */
void ACS_LM_TestLkf::reload(ACE_TCHAR * srcBuff, ACE_UINT64& srcPos)
{
    DEBUG("ACS_LM_TestLkf::reload() %s","Entering");

	DEBUG("In reload() srcPos = %d",srcPos );
	std::list<Lk*> destList;
	ACE_UINT64 lkCount = ACS_LM_Common::decode32(srcBuff, srcPos);
	for(ACE_UINT64 i=0; i<lkCount; i++)
	{
		Lk* lk = Lk::fromBytes(srcBuff, srcPos);		
        destList.push_back(lk);
	}	
    copyList(lkList,destList);

    DEBUG("ACS_LM_TestLkf::reload() %s","Leaving");
}//end of reload
