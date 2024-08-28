
//	 Copyright Ericsson AB 2007. All rights reserved.
#include "ACS_CS_API_NetworkElement_Implementation.h"

// ACS_CS_API_NetworkElement_R1
#include "ACS_CS_API_NetworkElement_R1.h"


// Class ACS_CS_API_NetworkElement_R1 

//ACS_CS_API_NetworkElement_Implementation *ACS_CS_API_NetworkElement_R1::implementation = 0;


ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::isMultipleCPSystem (bool &multipleCPSystem)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->isMultipleCPSystem(multipleCPSystem);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::isTestEnvironment (bool &testEnvironment)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result =  impl->isTestEnvironment(testEnvironment);
		delete (impl);
		impl = 0;
		return result;
	}

	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getFrontAPG (APID &frontAPG)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getFrontAPG(frontAPG);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getNEID (ACS_CS_API_Name_R1 &neid)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getNEID(neid);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getSingleSidedCPCount (uint32_t &cpCount)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(true);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result =  impl->getSingleSidedCPCount(cpCount);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getDoubleSidedCPCount (uint32_t &cpCount)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(true);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result =  impl->getDoubleSidedCPCount(cpCount);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getAPGCount (uint32_t &apgCount)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result =  impl->getAPGCount(apgCount);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getAlarmMaster (CPID &alarmMaster)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getAlarmMaster(alarmMaster);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getClockMaster (CPID &clockMaster)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getClockMaster(clockMaster);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getBSOMIPAddress (uint32_t &addressEthA, uint32_t &addressEthB)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getBSOMIPAddress(addressEthA, addressEthB);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getDefaultCPName (CPID cpid, ACS_CS_API_Name_R1 &name)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getDefaultCPName(cpid, name);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getOmProfile (ACS_CS_API_OmProfileChange_R1 &omProfile)
{

	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getOmProfile(omProfile);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}


ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpModeOut)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getClusterOpMode(clusterOpModeOut);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getTrafficIsolated (CPID &trafficIsolatedCpId)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl != NULL)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getTrafficIsolated(trafficIsolatedCpId);
		delete (impl);
		impl = 0;
		return result;
	}

	// no success, notify caller we had a failure
	return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getTrafficLeader (CPID &trafficLeaderCpId)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation(false);

	if (impl != NULL)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getTrafficLeader(trafficLeaderCpId);
		delete (impl);
		impl = 0;
		return result;
	}

	// no success, notify caller we had a failure
	return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getAPTType (std::string &aptType)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getAPTType(aptType);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getNodeArchitecture(ACS_CS_API_CommonBasedArchitecture::ArchitectureValue &nodeArchitectureOut)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getNodeArchitecture(nodeArchitectureOut);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getApgNumber(ACS_CS_API_CommonBasedArchitecture::ApgNumber &apgNumberOut)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getApgNumber(apgNumberOut);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getBGCIVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2])
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getBGCIVlan(subnet, netmask);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;
}
ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getSOLVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2])
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getSOLVlan(subnet, netmask);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;
}
ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getDmxcAddress(uint32_t &addressBgciA, uint32_t &addressBgciB)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getDmxcAddress(addressBgciA, addressBgciB);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_R1::getApgBgciAddress(uint16_t apgNr, uint16_t side, uint32_t &ipA, uint32_t &ipB)
{
	ACS_CS_API_NetworkElement_Implementation* impl = 0;
	impl = new ACS_CS_API_NetworkElement_Implementation();

	if (impl)
	{
		ACS_CS_API_NS::CS_API_Result result = impl->getApgBgciAddress(apgNr, side, ipA, ipB);
		delete (impl);
		impl = 0;
		return result;
	}
	else
		return ACS_CS_API_NS::Result_Failure;
}
