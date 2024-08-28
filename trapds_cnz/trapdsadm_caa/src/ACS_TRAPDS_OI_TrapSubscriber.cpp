/*
 * ACS_TRAPDS_OI_TrapSubscriber.cpp
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#include "ACS_TRAPDS_OI_TrapSubscriber.h"

/*============================================================================
	ROUTINE: ACS_TRAPDS_OI_TrapSubscriber
 ============================================================================ */
ACS_TRAPDS_OI_TrapSubscriber::ACS_TRAPDS_OI_TrapSubscriber() :  acs_apgcc_objectimplementerinterface_V3(IMM_TRAPDS_Util::IMPLEMENTER_TRAPSUBSCRIBER),
m_ImmClassName(IMM_TRAPDS_Util::classTrapSubscriber)
{

}

/*============================================================================
	ROUTINE: ~ACS_TRAPDS_OI_TrapSubscriber
 ============================================================================ */
ACS_TRAPDS_OI_TrapSubscriber::~ACS_TRAPDS_OI_TrapSubscriber()
{
}

ACS_CC_ReturnType ACS_TRAPDS_OI_TrapSubscriber::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " class Name: " << className << endl;
	cout << " parent Name: " << parentname << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	int i = 0;
	int portService = 0;
	std::string nameService("");
	std::string objName("");
	while( attr[i] )
	{

		cout << " attrName: " << attr[i]->attrName << endl;
		if (strcmp(attr[i]->attrName,IMM_TRAPDS_Util::ATT_TRAPDS_NAME) == 0)
			nameService = reinterpret_cast<char *>(attr[i]->attrValues[0]);

		if (strcmp(attr[i]->attrName,IMM_TRAPDS_Util::ATT_TRAPDS_KEY) == 0)
			objName = reinterpret_cast<char *>(attr[i]->attrValues[0]);

		if (strcmp(attr[i]->attrName,IMM_TRAPDS_Util::ATT_TRAPDS_PORT) == 0)
			portService = *reinterpret_cast<int *>(attr[i]->attrValues[0]);

		i++;
	}

	ACS_TRAPDS_ServerConnection *obs = new ACS_TRAPDS_ServerConnectionImpl(nameService,portService);
	ACS_TRAPDS_SNMPTrapReceiver::getInstance()->addSubscriber(obs,objName);

	return result;
}

ACS_CC_ReturnType ACS_TRAPDS_OI_TrapSubscriber::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;


	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	std::string rdn("");
	IMM_TRAPDS_Util::getRdnObject(objName,rdn);
	ACS_TRAPDS_SNMPTrapReceiver::getInstance()->removeSubscriber(rdn);

	return result;
}

ACS_CC_ReturnType ACS_TRAPDS_OI_TrapSubscriber::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	int i = 0;
	while( attrMods[i] ){
		cout << " attrName: " << attrMods[i]->modAttr.attrName << endl;
		i++;
	}

	cout << "MODIFYCallback " << endl;
	cout << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	return result;
}

ACS_CC_ReturnType ACS_TRAPDS_OI_TrapSubscriber::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "COMPLETECallback " << endl;
	cout << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	return result;
}

void ACS_TRAPDS_OI_TrapSubscriber::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "ABORTCallback " << endl;
	cout << endl;
}

void ACS_TRAPDS_OI_TrapSubscriber::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "APPLYCallback " << endl;
	cout << endl;

}

ACS_CC_ReturnType ACS_TRAPDS_OI_TrapSubscriber::updateRuntime(const char* p_objName, const char** p_attrName)
{
	printf("------------------------------------------------------------\n");
	printf("                   updateRuntime called                     \n");
	printf("------------------------------------------------------------\n");

	cout<<"Object Name: "<<p_objName<<endl;
	cout<<"Attribute Name: "<<p_attrName<<endl;


	printf("------------------------------------------------------------\n");

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	return result;
}


void ACS_TRAPDS_OI_TrapSubscriber::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList) {

	printf("------------------------------------------------------------\n");
	printf("                   adminOperationCallback called             \n");
	printf("------------------------------------------------------------\n");

	cout<<"oiHandle   : "<< oiHandle <<endl;
	cout<<"invocation : "<< invocation <<endl;
	cout<<"p_objName  : "<< p_objName <<endl;
	cout<<"operationId: "<< operationId <<endl;

	/*start*/
	cout << endl;

	int dim=0;
	int i=0;
	while(paramList[i]){
		i++;
		dim++;
	}

	i = 0;
	while( paramList[i] ){
		switch ( paramList[i]->attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_FLOATT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<float *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_DOUBLET:
			cout << " paramListName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_NAMET:
		{
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;

		}
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;
			break;
		default:
			break;

		}
		i++;
	}

	cout << " Implementing Admin Operation with ID = " << operationId << endl;

	SaAisErrorT retVal;


//############## NO ACTION FOR THIS NUMBER

//	if (operationId > 1)
//	{
		retVal = SA_AIS_ERR_NO_OP;
		cout << " ..... NO ACTION FOR THIS NUMBER  \n "<< endl;
		this->adminOperationResult( oiHandle , invocation, retVal );
		return;
//	}

	cout << "---------------------------END ACTION-------------------------\n" << endl;

}


