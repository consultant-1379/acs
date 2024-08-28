//	*********************************************************
//	 COPYRIGHT Ericsson 2012.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2012.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2012 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

#include <string>

#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImTypes.h"

#include "ACS_CS_API_CP_Implementation.h"

using namespace ACS_CS_API_NS;

ACS_CS_API_CP_Implementation::ACS_CS_API_CP_Implementation()
{
	model = new ACS_CS_ImModel();
	immReader = new ACS_CS_ImIMMReader();
	int cpload = immReader->loadCpObjects(model);

	if(cpload < 0)
	{
		if(model)
			delete model;
		model = NULL;
	}
}

ACS_CS_API_CP_Implementation::~ACS_CS_API_CP_Implementation()
{
	if(immReader)
		delete immReader;
	if(model)
		delete model;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCPId (const ACS_CS_API_Name_R1 &name, CPID &cpid)
{
	string cpName;
	bool cpIdFound = false;
	size_t nameLength = name.length();
	char* nameChar = new char[nameLength + 1];

	ACS_CS_API_NS::CS_API_Result resultValue = Result_Failure;

	resultValue = name.getName(nameChar, nameLength);

	// Get cp name string
	if (resultValue != Result_Success)
	{
		delete [] nameChar;
		return resultValue;
	}

	cpName = nameChar;
	delete [] nameChar;

	transform(cpName.begin(), cpName.end(), cpName.begin(), (int(*)(int))toupper);

	if(model)
	{
		set<const ACS_CS_ImBase *>::iterator clusterCpIt;
		set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

		set<const ACS_CS_ImBase *> clusterCpObjects;
		set<const ACS_CS_ImBase *> dualSidedCpObjects;
		model->getObjects(clusterCpObjects, CLUSTERCP_T);
		model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

		// Loop through CpServiceObjects
		for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
		{
			const ACS_CS_ImBase *base = *clusterCpIt;
			const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

			if (cp)
			{
				string alias = cp->alias;
				string defaultName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);

				transform(alias.begin(), alias.end(), alias.begin(), (int(*)(int))toupper);
				transform(defaultName.begin(), defaultName.end(), defaultName.begin(), (int(*)(int))toupper);

				if((cpName.compare(defaultName) == 0) || (cpName.compare(alias) == 0))
				{
					cpid = cp->systemIdentifier;
					cpIdFound = true;
				}

				if (cpIdFound)
					break;
			}
			else
			{
				resultValue = Result_Failure;
			}
		}

		for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && cpIdFound == false; dualSidedCpIt++)
		{
			const ACS_CS_ImBase *base = *dualSidedCpIt;
			const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

			if (cp)
			{
				string alias = cp->alias;
				string defaultName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
				transform(alias.begin(), alias.end(), alias.begin(), (int(*)(int))toupper);
				transform(defaultName.begin(), defaultName.end(), defaultName.begin(), (int(*)(int))toupper);


				//if((cpName.compare(defaultName) == 0))
				if((cpName.compare(defaultName) == 0) || (cpName.compare(alias) == 0))
				{
					cpid = cp->systemIdentifier;
					cpIdFound = true;
				}

				if (cpIdFound)
					break;
			}
			else
			{
				resultValue = Result_Failure;
			}
		}
	}

	if (resultValue == Result_Success && cpIdFound == false)
		resultValue = Result_NoEntry;

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCPName (CPID cpid, ACS_CS_API_Name_R1 &name)
{
	string defaultName;

	ACS_CS_API_NS::CS_API_Result resultValue = Result_NoEntry;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);
	const ACS_CS_ImDualSidedCp* dualSidedCp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

	if(clusterCp != 0)
	{
		defaultName = ACS_CS_ImUtils::getIdValueFromRdn(clusterCp->rdn);
	}
	else if(dualSidedCp != 0)
	{
		defaultName = ACS_CS_ImUtils::getIdValueFromRdn(dualSidedCp->rdn);
	}
	else
	{
		return resultValue;
	}

	if(defaultName.length() > 0)
	{
		name.setName(defaultName.c_str());
		resultValue = Result_Success;
	}
	else
	{
		resultValue = Result_NoValue;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getAPZType (CPID cpid, ACS_CS_API_NS::CS_API_APZ_Type &type)
{
	uint16_t cpType = 0;
	CS_API_Result resultValue = Result_Failure;  //default return value

	resultValue = this->getAPZSystem(cpid, cpType);

	if (resultValue == Result_Success)
	{
		if (cpType == 21255)     //check for type of APZ
		{
			type = APZ21255;
			resultValue = Result_Success;
		}
		else if (cpType == 21401)
		{
			type = APZ21401;
			resultValue = Result_Success;
		}
		else                     //none of above => undefined APZ type
		{
			resultValue = Result_Failure;
		}
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCPList (ACS_CS_API_IdList_R1 &cpList)
{
	CS_API_Result resultValue = Result_Failure;

	if(model)
	{
		int index = 0;
		set<const ACS_CS_ImBase *>::iterator clusterCpIt;
		set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

		set<const ACS_CS_ImBase *> clusterCpObjects;
		set<const ACS_CS_ImBase *> dualSidedCpObjects;
		model->getObjects(clusterCpObjects, CLUSTERCP_T);
		model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

		int entryCount = clusterCpObjects.size() + dualSidedCpObjects.size();
		(void)cpList.setSize(entryCount);

		if(entryCount > 0)
		{
			resultValue = Result_Success;

			for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
			{
				const ACS_CS_ImBase *base = *clusterCpIt;
				const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

				if(cp)
				{
					(void)cpList.setValue(cp->systemIdentifier, index++);
				}
				else
				{
					resultValue = Result_Failure;
					break;
				}
			}

			for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end(); dualSidedCpIt++)
			{
				const ACS_CS_ImBase *base = *dualSidedCpIt;
				const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

				if (cp)
				{
					(void)cpList.setValue(cp->systemIdentifier, index++);
				}
				else
				{
					resultValue = Result_Failure;
					break;
				}
			}
		}
		else
		{
			(void)cpList.setSize(0);
			return Result_Success;
		}
	}

	if(resultValue!=Result_Success){
		(void)cpList.setSize(0);
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getAPZSystem (CPID cpid, uint16_t &system)
{
	string apzSystem;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return Result_NoEntry;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);
	const ACS_CS_ImDualSidedCp* dualSidedCp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

	if(clusterCp != 0)
	{
		apzSystem = clusterCp->apzSystem;
	}
	else if(dualSidedCp != 0)
	{
		apzSystem = dualSidedCp->apzSystem;
	}
	else
	{
		return Result_NoEntry;
	}

	stringstream trim;
	trim << apzSystem;
	trim >> apzSystem;
	apzSystem = trim.str();

	if(apzSystem.length() != 8)
	{
		return Result_NoValue;
	}

	apzSystem = apzSystem.substr(3);

	int i = atoi(apzSystem.c_str());

	if(i < 21250 || i >= 21500)
	{
		return Result_NoValue;
	}

	system = i;

	return Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getMauType (CPID cpid, ACS_CS_API_NS::MauType &type)
{
	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return Result_NoEntry;

	const ACS_CS_ImDualSidedCp* dualSidedCp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

	if(dualSidedCp != 0)
	{
		type = static_cast<ACS_CS_API_NS::MauType>(dualSidedCp->mauType);
	}
	else
	{
		return Result_NoValue;
	}
	return Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCPType (CPID cpid, uint16_t &type)
{
	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return Result_NoEntry;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);
	const ACS_CS_ImDualSidedCp* dualSidedCp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

	if(clusterCp != 0)
	{
		type = clusterCp->cpType;
	}
	else if(dualSidedCp != 0)
	{
		type = dualSidedCp->cpType;
	}
	else
	{
		return Result_NoEntry;
	}

	if(type < 21200 || type >= 21299)
	{
		return Result_NoValue;
	}

	return Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCPAliasName (CPID cpid, ACS_CS_API_Name_R1 &alias, bool &isAlias)
{
	string cpAlias;
	string defaultName;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return Result_NoEntry;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);
	const ACS_CS_ImDualSidedCp* dualSidedCp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

	if(clusterCp != 0)
	{
		cpAlias = clusterCp->alias;
		defaultName = ACS_CS_ImUtils::getIdValueFromRdn(clusterCp->rdn);
	}
	else if(dualSidedCp != 0)
	{
		cpAlias = dualSidedCp->alias;
		defaultName = ACS_CS_ImUtils::getIdValueFromRdn(dualSidedCp->rdn);
	}
	else
	{
		return Result_NoEntry;
	}

	if(cpAlias.length() == 0 || cpAlias.compare("-") == 0)
	{
		//No valid alias found, return Default CP name instead
		alias.setName(defaultName.c_str());
		isAlias = false;
	}
	else
	{
		alias.setName(cpAlias.c_str());
		isAlias = true;
	}


//	transform(cpAlias.begin(), cpAlias.end(), cpAlias.begin(), (int(*)(int))toupper);
//	transform(defaultName.begin(), defaultName.end(), defaultName.begin(), (int(*)(int))toupper);
//
//	if (cpAlias.compare(defaultName) == 0)
//		isAlias = false;
//	else
//		isAlias = true;

	return Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getState (CPID cpid, ACS_CS_API_NS::CpState &cpState)
{
	CS_API_Result resultValue = Result_Failure;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		if(clusterCp->cpState >= 0 && clusterCp->cpState <= 255)
		{
			cpState = clusterCp->cpState;
			resultValue = Result_Success;
		}
		else
		{
			resultValue = Result_Failure;
		}
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getApplicationId (CPID cpid, ACS_CS_API_NS::ApplicationId &appId)
{
	CS_API_Result resultValue = Result_Failure;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		if(clusterCp->applicationId >= 0 && clusterCp->applicationId <= 255)
		{
			appId = clusterCp->applicationId;
			resultValue = Result_Success;
		}
		else
		{
			resultValue = Result_Failure;
		}
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getApzSubstate (CPID cpid, ACS_CS_API_NS::ApzSubstate &apzSubstate)
{
	CS_API_Result resultValue = Result_Failure;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		if(clusterCp->apzSubstate >= 0 && clusterCp->apzSubstate <= 255)
		{
			apzSubstate = clusterCp->apzSubstate;
			resultValue = Result_Success;
		}
		else
		{
			resultValue = Result_Failure;
		}
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getStateTransition (CPID cpid, ACS_CS_API_NS::StateTransition &stateTrans)
{
	CS_API_Result resultValue = Result_Failure;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		if(clusterCp->stateTransition >= 0 && clusterCp->stateTransition <= 255)
		{
			stateTrans = clusterCp->stateTransition;
			resultValue = Result_Success;
		}
		else
		{
			resultValue = Result_Failure;
		}
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getAptSubstate (CPID cpid, ACS_CS_API_NS::AptSubstate &aptSubstate)
{
	CS_API_Result resultValue = Result_Failure;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		if(clusterCp->aptSubstate >= 0 && clusterCp->aptSubstate <= 255)
		{
			aptSubstate = clusterCp->aptSubstate;
			resultValue = Result_Success;
		}
		else
		{
			resultValue = Result_Failure;
		}
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCPQuorumData (CPID cpid, ACS_CS_API_CpData_R1 &quorumData)
{
	quorumData.id = cpid;

	ACS_CS_API_NS::CS_API_Result returnValue = getState(cpid, quorumData.state);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	returnValue = getApplicationId(cpid, quorumData.applicationId);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	returnValue = getApzSubstate(cpid, quorumData.apzSubstate);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	returnValue = getStateTransition(cpid, quorumData.stateTransition);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	returnValue = getAptSubstate(cpid, quorumData.aptSubstate);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	returnValue = getBlockingInfo(cpid, quorumData.blockingInfo);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	returnValue = getCpCapacity(cpid, quorumData.cpCapacity);
	if (ACS_CS_API_NS::Result_Success != returnValue)
		return returnValue;

	return returnValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getBlockingInfo (const CPID cpid, ACS_CS_API_NS::BlockingInfo &blockingInfo)
{
	CS_API_Result resultValue = Result_Failure;

	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		blockingInfo = clusterCp->blockingInfo;
		resultValue = Result_Success;
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_CP_Implementation::getCpCapacity (const CPID cpid, ACS_CS_API_NS::CpCapacity &cpCapacity)
{
	CS_API_Result resultValue = Result_Failure;
	const ACS_CS_ImBase *base = getCpObject(cpid);

	if(base == 0)
		return resultValue;

	const ACS_CS_ImClusterCp* clusterCp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

	if(clusterCp != 0 )
	{
		cpCapacity = clusterCp->cpCapacity;
		resultValue = Result_Success;
	}
	else
	{
		resultValue = Result_NoEntry;
	}

	return resultValue;
}

const ACS_CS_ImBase* ACS_CS_API_CP_Implementation::getCpObject(const CPID cpid)
{
	const ACS_CS_ImBase *base = 0;

	if(model)
	{
		set<const ACS_CS_ImBase *>::iterator clusterCpIt;
		set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

		set<const ACS_CS_ImBase *> clusterCpObjects;
		set<const ACS_CS_ImBase *> dualSidedCpObjects;
		model->getObjects(clusterCpObjects, CLUSTERCP_T);
		model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

		for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
		{
			base = *clusterCpIt;
			const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

			if(cp && cp->systemIdentifier == cpid){
				return base;
			}
		}

		for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end(); dualSidedCpIt++)
		{
			base = *dualSidedCpIt;
			const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

			if(cp && cp->systemIdentifier == cpid){
				return base;
			}
		}
	}

	return 0;
}


