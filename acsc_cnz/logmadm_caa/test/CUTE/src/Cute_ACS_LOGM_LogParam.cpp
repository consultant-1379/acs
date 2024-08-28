
/**@file Cute_ACS_LOGM_LogParam.cpp
 * Test functions to test acs_logm_logparam_oihandler are defined in this file
 * @version 1.1.1
 */
/* INCLUDE SECTION */
/* inclusion of ACE library */
#include <ace/ACE.h>
/* Inclusion of  Cute libraries*/
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_ACS_LOGM_LogParam.h"


/* Inclusion of required LOGM header files */
#include "acs_logm_logparam_oihandler.h"
#include "acs_logm_logmaint.h"
#include "acs_logm_common.h"


/*static variables declaration*/

acs_logm_logparam_oihandler* Cute_ACS_LOGM_LogParam::m_poLogParamOiHandler;

/* ROUTINE: Cute_ACS_LOGM_LogParam()
 */
void Cute_ACS_LOGM_LogParam::init()
{
	std::string myLogParamClassName(LOGM_LOG_PARAM_CLASS_NAME);
	std::string myLogParamImplName(LOGM_LOG_PARAM_IMPL_NAME);


	m_poLogParamOiHandler = new acs_logm_logparam_oihandler(  myLogParamClassName.c_str(),
						  myLogParamImplName.c_str(),
                                                          ACS_APGCC_ONE );

}/* end Cute_ACS_LOGM_LogParam()*/

/* ROUTINE: ~Cute_ACS_LOGM_LogParam()
 */

void Cute_ACS_LOGM_LogParam::destroy()
{
	delete m_poLogParamOiHandler;
	m_poLogParamOiHandler = 0;
}

/* ROUTINE:vTestCreate()
 */
void Cute_ACS_LOGM_LogParam::vTestCreate()
{
	char *className = "LogParam";
        char *parentName = "safApp=safImmService";
        ACS_CC_ReturnType result = m_poLogParamOiHandler->create(0, 0, className, parentName, 0);
        ASSERTM("\n Creation of logparm object is success ", result == ACS_CC_FAILURE );


}/*end vTestCreate()*/

/* ROUTINE:vTestModify()
 */
void Cute_ACS_LOGM_LogParam::vTestModifylogName()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"  Modification of the attribute(logName) of  Logparam(DATASTSIn)	"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	char* logName = "DATASTSIn";
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_NAME );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(logName);
	ACS_CC_ReturnType result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logName attribute is :"<<result<<endl;
	ASSERTM("\n Modification of logName is not success ", result == ACS_CC_FAILURE );

}
void Cute_ACS_LOGM_LogParam::vTestModifylogPath()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"  Modification of the attribute(logPath) of  Logparam(DATASTSIn)	"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;
	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	char *logPath= "/data/sts/data";
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_PATH );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(logPath);
	ACS_CC_ReturnType result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logpath attribute is :"<<result<<endl;
	ASSERTM("\n Modification of logPath is failed", result == ACS_CC_SUCCESS );

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"  Modification of the attribute(logPath) of  Logparam(DATASTSIn)	"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	cout<<endl<<"Modification of DATASTSIn with wrong path"<<endl;

	logPath= "/data/sts/output";
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_PATH );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(logPath);

	result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logpath attribute is :"<<result<<endl;
	ASSERTM("\n Modification of logPath is failed", result == ACS_CC_SUCCESS );

}

void Cute_ACS_LOGM_LogParam::vTestModifylogType()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"  Modification of the attribute(logType) of  Logparam(DATASTSIn)	"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	char* logType = "*.log";
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_TYPE);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] =  reinterpret_cast<void*>(logType);
	ACS_CC_ReturnType result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logType attribute is :"<<result<<endl;
	ASSERTM("\n Modification of logType os failed", result == ACS_CC_SUCCESS );

}

void Cute_ACS_LOGM_LogParam::vTestModifylogIOOperation()
{
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"  Modification of the attribute(logIOOperation) of  Logparam(DATASTSIn)	"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;


	cout<<endl<<"MODIFICATION OF logNoOfDays WITH WRONG INVALID VLAUE"<<endl;
	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	int *logIOOperation = new int(80);
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_OPERATION);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void *>(logIOOperation);
	ACS_CC_ReturnType result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logIOperation attribute is :"<<result<<endl;
	ASSERTM("\n Modification of logIOperation is failed", result == ACS_CC_FAILURE );


	cout<<endl<<"MODIFICATION OF logNoOfDays WITH VALID VLAUE"<<endl;
	*logIOOperation = 0;
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_OPERATION);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void *>(logIOOperation);
	result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logIOperation attribute is :"<<result<<endl;
	ASSERTM("\n Modification of logIOperation is failed", result == ACS_CC_SUCCESS );

}

void Cute_ACS_LOGM_LogParam::vTestModifylogNoOfDays()
{

	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"  Modification of the attribute(logNoOfDays) of  Logparam(DATASTSIn)	"<<endl;
	cout<<endl<<"*******************************************************************"<<endl;
	cout<<endl<<"MODIFICATION OF logNoOfDays WITH WRONG INVALID VLAUE"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[1] = 0;
	attrMods[0]->modAttr.attrName = new char[30];
	int *logNoOfDays = new int(-2);
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_OLD_DAYS);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void *>(logNoOfDays);
	ACS_CC_ReturnType result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logNoOfDays attribute is :"<<result<<endl;
	ASSERTM("\n Modification of no of days is failed ", result == ACS_CC_FAILURE );


	cout<<endl<<"MODIFICATION OF logNoOfDays WITH VALID VLAUE"<<endl;
	*logNoOfDays =10;
	strcpy(attrMods[0]->modAttr.attrName, SERVICE_LOG_OLD_DAYS);
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void *>(logNoOfDays);
	result = m_poLogParamOiHandler->modify(0, 0, "DATASTSIn,logMaintenanceMId=1",attrMods);
	cout<<"The Return code for the modification of logNoOfDays attribute is :"<<result<<endl;
	ASSERTM("\n Modification of no of days is failed ", result == ACS_CC_SUCCESS );

}/*end vTestModify()*/

/*ROUTINE:vTestDelete()
 */
void Cute_ACS_LOGM_LogParam::vTestDelete()
{
	ACS_CC_ReturnType result = m_poLogParamOiHandler->deleted(0, 0, "DATASTSIn,logMaintenanceMId=1");
        ASSERTM("\n Deletion of DATASTSIn ", result == ACS_CC_FAILURE );

}/* end vTestDelete() */


/*ROUTINE:make_suite_Cute_ACS_LOGM_LogParam()
 */
cute::suite Cute_ACS_LOGM_LogParam::make_suite_Cute_ACS_LOGM_LogParam()
{

	cute::suite s;
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestCreate));
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestModifylogName));
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestModifylogPath));
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestModifylogType));
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestModifylogIOOperation));
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestModifylogNoOfDays));
 	s.push_back(CUTE(Cute_ACS_LOGM_LogParam::vTestDelete));
	return s;
}/*end make_suite_Cute_ACS_LOGM_LogParam()*/
