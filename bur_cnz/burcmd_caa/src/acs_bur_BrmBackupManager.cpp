/*
 * acs_bur_BrmBackupManager.cpp
 *
 *  Created on: Dec 2, 2011
 *      Author: egimarr
 */
#include "acs_bur_BrmBackupManager.h"
#include "ACS_TRA_trace.h"
#include "acs_bur_trace.h"
#include "ACS_TRA_Logging.h"

acs_bur_BrmBackupManager::acs_bur_BrmBackupManager(char *p)
{
	burLog = new ACS_TRA_Logging();
	burLog->Open("BUR");

	bbmTrace = new ACS_TRA_trace("acs_bur_BrmBackupManager");
	TRACE(bbmTrace, "%s", "acs_bur_BrmBackupManager::acs_bur_BrmBackupManager()");

	ACS_CC_ReturnType result;

	OmHandler immHandle;

	char *nameObject = p;

	setOpCode(NOERROR);

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		burLog->Write("acs_bur_BrmBackupManager:Error in Init function",LOG_LEVEL_DEBUG);
		setOpCode(INITERROR,"Init()");
		return;
	}

	ACS_APGCC_ImmAttribute attribute_1,attribute_2,attribute_3,attribute_4;

	char attrdn[]= "brmBackupManagerId";
	char attrBackupDomain[]="backupDomain";
	char attrBackupType[]="backupType";
	char attrAsyncActionProgress[]="progressReport";

	attribute_1.attrName = attrdn;
	attribute_2.attrName = attrBackupDomain;
	attribute_3.attrName = attrBackupType;
	attribute_4.attrName = attrAsyncActionProgress;


	std::vector<ACS_APGCC_ImmAttribute *> attributes;

	attributes.push_back(&attribute_1);
	attributes.push_back(&attribute_2);
	attributes.push_back(&attribute_3);
	attributes.push_back(&attribute_4);

	result = immHandle.getAttribute(nameObject, attributes);

	if ( result != ACS_CC_SUCCESS ){
		burLog->Write("acs_bur_BrmBackupManager: Error in getAttribute() function",LOG_LEVEL_DEBUG);
		setOpCode(GETATTRERROR,"getAttribute()");
		immHandle.Finalize();
		return;
	}

	int i = 0;
	char attr_name[100];

	string strAttrName = "";

	while( i < (int)attributes.size() ){

		strAttrName = string(attributes[i]->attrName);

		memset((void *)attr_name,(int)0,(size_t)100);

		strcpy( attr_name,strAttrName.c_str() );

		if (strcmp(attr_name,"backupDomain")==0){
			backupDomain = string((const char *)(attributes[i]->attrValues[0]));
		} else if (strcmp(attr_name,"backupType")==0){
			backupType = string((const char *)(attributes[i]->attrValues[0]));
		}else if (strcmp(attr_name,"progressReport")==0){
			asyncActionProgress = string((const char *)(attributes[i]->attrValues[0]));
		}else if (strcmp(attr_name,"brmBackupManagerId")==0){
			brmBackupManagerId = string((const char *)(attributes[i]->attrValues[0]));
		}

		i++;
	}

	result = immHandle.Finalize();

	if ( result != ACS_CC_SUCCESS ){
		burLog->Write("acs_bur_BrmBackupManager: Error in Finalize() function",LOG_LEVEL_DEBUG);
		setOpCode(FINALIZEERROR,"Finalize()");
		return ;
	}

}

void acs_bur_BrmBackupManager::setOpCode(int code)
{
	opCode = code;
}

void acs_bur_BrmBackupManager::setOpCode(int code, string functionName)
{
	opCode = code;
	m_functionName = functionName;
}


string acs_bur_BrmBackupManager::getBrmBackupManagerId()
{
	return brmBackupManagerId;
}

string acs_bur_BrmBackupManager::getBackupDomain()
{
	return backupDomain;
}

string acs_bur_BrmBackupManager::getBackupType()
{
	return backupType;
}

string acs_bur_BrmBackupManager::getAsyncActionProgress()
{
	return asyncActionProgress;
}
int acs_bur_BrmBackupManager::getOpCode()
{
	return opCode;
}
//
string acs_bur_BrmBackupManager::getOpMessage()
{
	return m_functionName;
}
//
acs_bur_BrmBackupManager::~acs_bur_BrmBackupManager()
{
    if (NULL != bbmTrace)
    {
    	delete bbmTrace;
    }
    if (NULL != burLog)
    {
        delete burLog;
    }
}



