/*
 * acs_bur_BrmBackup.cpp
 *
 *  Created on: Dec 12, 2011
 *      Author: egiacri,egimarr
 */

#include "acs_bur_BrmBackup.h"

acs_bur_BrmBackup::~acs_bur_BrmBackup() {
	// TODO Auto-generated destructor stub
	TRACE(backup_trace, "%s ", "acs_bur_BrmBackup::~acs_bur_BrmBackup() ");
    if (NULL != backup_trace)
    {
    	delete backup_trace;
    }
    if (NULL != burLog)
    {
    	delete burLog;
    }
}

void acs_bur_BrmBackup::setOpCode(int code)
{
	opCode = code;
}

void acs_bur_BrmBackup::setOpCode(int code, string functionName)
{
	opCode = code;
	m_functionName = functionName;
}

acs_bur_BrmBackup::acs_bur_BrmBackup(char *distName) {
	// TODO Auto-generated constructor stub
	brmBackupId = "";
	backupName = "";
	creationTime = "";
	status = 0;
	asyncActionProgress = "";
	opCode = 0;
	m_functionName = "";

	burLog = new ACS_TRA_Logging();
	burLog->Open("BUR");

	backup_trace = new ACS_TRA_trace("acs_bur_BrmBackup");

	TRACE(backup_trace,"acs_bur_BrmBackup() DN = %s",distName);

	ACS_CC_ReturnType result;

	OmHandler immBkHandle;

	char *nameObject = distName;

	setOpCode(NOERROR);

	result = immBkHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		burLog->Write("acs_bur_BrmBackup(): Error in Init function",LOG_LEVEL_ERROR);
		setOpCode(INITERROR,string("Init()"));
		return;
	}

	ACS_APGCC_ImmAttribute attribute_1;
	ACS_APGCC_ImmAttribute attribute_2;
	ACS_APGCC_ImmAttribute attribute_3;
	ACS_APGCC_ImmAttribute attribute_4;
	ACS_APGCC_ImmAttribute attribute_5;
	ACS_APGCC_ImmAttribute attribute_6;

	char attrdn[]= "brmBackupId";
	char attrBackupName[]="backupName";
	char attrCreationTime[]="creationTime";
	char attrStatus[]="status";
	char attrAsyncActionProgress[]="progressReport";
	char attrBackupDescriptor[]="backupDescriptor";

	attribute_1.attrName = attrdn;
	attribute_2.attrName = attrBackupName;
	attribute_3.attrName = attrCreationTime;
	attribute_4.attrName = attrStatus;
	attribute_5.attrName = attrAsyncActionProgress;
	attribute_6.attrName = attrBackupDescriptor;

	std::vector<ACS_APGCC_ImmAttribute *> attributes;

	attributes.push_back(&attribute_1);
	attributes.push_back(&attribute_2);
	attributes.push_back(&attribute_3);
	attributes.push_back(&attribute_4);
	attributes.push_back(&attribute_5);
	attributes.push_back(&attribute_6);

	result = immBkHandle.getAttribute(nameObject, attributes);

	if ( result != ACS_CC_SUCCESS ){
		burLog->Write("acs_bur_BrmBackup: Error in getAttribute function",LOG_LEVEL_ERROR);
		setOpCode(GETATTRERROR,string("getAttribute()"));
		immBkHandle.Finalize();
		return;
	}

	int i = 0;
	char attr_name[100];

	string strAttrName = "";

	while( i < (int)attributes.size() ){

		strAttrName = string(attributes[i]->attrName);
		TRACE(backup_trace, "%s %s", "acs_bur_BrmBackup::acs_bur_BrmBackup() attribute name is: ",strAttrName.c_str() );

		memset((void *)attr_name,(int)0,(size_t)100);

		strcpy( attr_name,strAttrName.c_str() );

		if (strcmp(attr_name,"backupName")==0){
			backupName = string((const char *)(attributes[i]->attrValues[0]));
		}else if (strcmp(attr_name,"creationTime")==0){
			creationTime = string((const char *)(attributes[i]->attrValues[0]));
		}
		else if (strcmp(attr_name,"status")==0){
			status =  (* reinterpret_cast<int *>(attributes[i]->attrValues[0]));
		}else if (strcmp(attr_name,"progressReport")==0){
			asyncActionProgress = string((const char *)(attributes[i]->attrValues[0]));
		}else if (strcmp(attr_name,"brmBackupId")==0){
			brmBackupId = string((const char *)(attributes[i]->attrValues[0]));
		}

		i++;
	}

	result = immBkHandle.Finalize();

	if ( result != ACS_CC_SUCCESS ){
		burLog->Write("acs_bur_BrmBackup: Error in Finalize function",LOG_LEVEL_ERROR);
		setOpCode(FINALIZEERROR,string("Finalize()"));
		return ;
	}

}

string acs_bur_BrmBackup::getBrmBackupId()
{
	return brmBackupId;
}

string acs_bur_BrmBackup::getBackupName()
{
	return backupName;
}

int acs_bur_BrmBackup::getStatus()
{
	return status;
}
string acs_bur_BrmBackup::getCreationTime()
{
	return creationTime;
}

string acs_bur_BrmBackup::getAsyncActionProgress()
{
	return asyncActionProgress;
}

int acs_bur_BrmBackup::getOpCode()
{
	return opCode;
}

string acs_bur_BrmBackup::getOpMessage()
{
	return m_functionName;
}
//

