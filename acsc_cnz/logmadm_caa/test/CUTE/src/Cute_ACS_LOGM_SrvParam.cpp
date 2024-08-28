
/**@file Cute_ACS_LOGM_SrvParam.cpp
 * Test functions to test acs_logm_srvparam_oihandler are defined in this file
 * @version 1.1.1
 */
/* INCLUDE SECTION */
/* inclusion of ACE library */
#include <ace/ACE.h>
/* Inclusion of  Cute libraries*/
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_ACS_LOGM_SrvParam.h"


/* Inclusion of required LOGM header files */
#include "acs_logm_srvparam_oihandler.h"
#include "acs_logm_logmaint.h"
#include "acs_logm_common.h"


/*static variables declaration*/

acs_logm_srvparam_oihandler* Cute_ACS_LOGM_SrvParam::m_poSrvParamOiHandler;

/* ROUTINE: Cute_ACS_LOGM_SrvParam()
 */
void Cute_ACS_LOGM_SrvParam::init()
{

	std::string myLOGMObjName("");
	myLOGMObjName.append("logMaintenanceMId=1,safApp=safImmService");

	std::string myLOGMLogParamImplName(LOGM_SERVICE_PARAM_IMPL_NAME);

	m_poSrvParamOiHandler = new acs_logm_srvparam_oihandler(  myLOGMObjName.c_str(),
                                                          myLOGMLogParamImplName.c_str(),
                                                          ACS_APGCC_ONE );

}/* end Cute_ACS_LOGM_SrvParam()*/

/* ROUTINE: ~Cute_ACS_LOGM_SrvParam()
 */

void Cute_ACS_LOGM_SrvParam::destroy()
{
	delete m_poSrvParamOiHandler;
	m_poSrvParamOiHandler = 0;
}

/* ROUTINE:vTestCreate()
 */
void Cute_ACS_LOGM_SrvParam::vTestCreate()
{
	char *className = "LogMaintanenceM";
        char *parentName = "logMaintenanceMId=1,safApp=safImmService";
        ACS_CC_ReturnType result = m_poSrvParamOiHandler->create(0, 0, className, parentName, 0);
        ASSERTM("\n Creation of srvparam object is success ", result == ACS_CC_FAILURE );

}/*end vTestCreate()*/

/* ROUTINE:vTestModify()
 */
void Cute_ACS_LOGM_SrvParam::vTestModifysrvTrgHour()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvTrgHour)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[2]; 
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];

	int *srvTrgHour = new int(1);
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_TRIGGER_HOUR );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] =  reinterpret_cast<void*>(srvTrgHour);
	ACS_CC_ReturnType result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	ASSERTM("\n Modification of srvparam srvTrgHour is success ", result == ACS_CC_SUCCESS );

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvTrgHour)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of service trigger hour with wrong value"<<endl;
	*srvTrgHour =24;
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_TRIGGER_HOUR );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvTrgHour);
	result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	delete srvTrgHour;
	ASSERTM("\n Modification of srvparam srvTrgHour is success ", result == ACS_CC_FAILURE );

}
void Cute_ACS_LOGM_SrvParam::vTestModifysrvTrgMin()
{

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvTrgMin)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[2];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	int *srvTrgMin = new int(10);
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_TRIGGER_MINUTES);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvTrgMin);
	ACS_CC_ReturnType result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	ASSERTM("\n Modification of srvparam srvTrgMin is success ", result == ACS_CC_SUCCESS );

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvTrgMin)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	cout<<endl<<"Modification of service trigger minute with wrong value"<<endl;
	//int *srvTrgMin = new int(10);
	*srvTrgMin = 80;
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_TRIGGER_MINUTES);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvTrgMin);
	result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	delete srvTrgMin;
	ASSERTM("\n Modification of srvparam srvTrgMin is success ", result == ACS_CC_FAILURE );


}
void Cute_ACS_LOGM_SrvParam::vTestModifysrvTrgFreq()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvTrgFreq)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[2];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	int* srvTrgFreq = new int(0);
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_TRIGGER_FREQUENCY);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvTrgFreq);
	ACS_CC_ReturnType result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	ASSERTM("\n Modification of srvparam srvTrgFreq is failure ", result == ACS_CC_SUCCESS );


	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvTrgFreq)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	cout<<endl<<"Modification of service trigger Frequency with wrong value"<<endl;
	*srvTrgFreq = 10;
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_TRIGGER_FREQUENCY);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvTrgFreq);
	result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	delete srvTrgFreq;
	ASSERTM("\n Modification of srvparam srvTrgFreq is failure ", result == ACS_CC_FAILURE );

}
void Cute_ACS_LOGM_SrvParam::vTestModifysrvLogFileCount()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvLogFileCount)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[2];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	int * srvLogFileCount = new int(6);
	strcpy(attrMods[0]->modAttr.attrName, LOG_FILE_COUNT);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvLogFileCount);
	ACS_CC_ReturnType result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	ASSERTM("\n Modification of srvparam srvLogFileCount is failure ", result == ACS_CC_SUCCESS );

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvLogFileCount)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	cout<<endl<<"Modification of service log Type with wrong value"<<endl;
	*srvLogFileCount = 5;
	strcpy(attrMods[0]->modAttr.attrName,LOG_FILE_COUNT);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvLogFileCount);
	result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	delete srvLogFileCount;
	ASSERTM("\n Modification of srvparam srvLogFileCount is failure ", result == ACS_CC_FAILURE );

}
void Cute_ACS_LOGM_SrvParam::vTestModifysrvStsDel()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvStsDel)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;
	ACS_APGCC_AttrModification *attrMods[2];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	int* srvStsDel = new int(0);
	strcpy(attrMods[0]->modAttr.attrName, STS_STATUS_LOG_DEL);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvStsDel);
	ACS_CC_ReturnType result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	ASSERTM("\n Modification of srvparam srvTrgStsDelete  is failure ", result == ACS_CC_SUCCESS );

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"Modification of the attribute(srvStsDel)of class 'LogMaintenanaceM'"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	cout<<endl<<"Modification of service STS delete with wrong value"<<endl;
	*srvStsDel = 6;
	strcpy(attrMods[0]->modAttr.attrName, STS_STATUS_LOG_DEL);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(srvStsDel);
	result = m_poSrvParamOiHandler->modify(0, 0, "logMaintenanceMId=1",attrMods);
	ASSERTM("\n Modification of srvparam srvTrgStsDelete  is failure ", result == ACS_CC_FAILURE );
	delete srvStsDel;

}/*end vTestModify()*/

/*ROUTINE:vTestDelete()
 */
void Cute_ACS_LOGM_SrvParam::vTestDelete()
{
	ACS_CC_ReturnType result = m_poSrvParamOiHandler->deleted(0, 0, "logMaintenanceMId=1");
	ASSERTM("\n Deletion of root object is success ", result == ACS_CC_FAILURE );

}/* end vTestDelete() */


/*ROUTINE:make_suite_Cute_ACS_LOGM_SrvParam()
 */
cute::suite Cute_ACS_LOGM_SrvParam::make_suite_Cute_ACS_LOGM_SrvParam(){

	cute::suite s;
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestCreate));
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestModifysrvTrgHour));
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestModifysrvTrgMin));
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestModifysrvTrgFreq));
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestModifysrvLogFileCount));
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestModifysrvStsDel));
 	s.push_back(CUTE(Cute_ACS_LOGM_SrvParam::vTestDelete));
	return s;
}/*end make_suite_Cute_ACS_LOGM_SrvParam()*/
