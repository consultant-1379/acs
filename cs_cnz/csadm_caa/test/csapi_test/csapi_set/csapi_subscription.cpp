#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include "csapi_subscription.h"

using namespace std;

HWC_Observer *hwc = NULL;
NE_Observer *ne = NULL;
OgChange_Observer *og = NULL;
CpChange_Observer *cp = NULL;
OmProfileChange_Observer *om = NULL;

string getOperationValue(ACS_CS_API_TableChangeOperation::OpType operation)
{
	string ret = "UNKNOWN";

	switch(operation)
	{
	case ACS_CS_API_TableChangeOperation::Add:
		ret = "Add";
		break;
	case ACS_CS_API_TableChangeOperation::Change:
		ret = "Change";
		break;
	case ACS_CS_API_TableChangeOperation::Delete:
		ret = "Delete";
		break;
	case ACS_CS_API_TableChangeOperation::Unspecified:
		ret = "Unspecified";
		break;
	}

	return ret;
}

string getIpAddressValue(uint32_t address)
{
	if(address == 0xFFFFFFFF)
		return "Unspecified";

	char str[INET_ADDRSTRLEN];

	address = ntohl(address);
	inet_ntop(AF_INET, &(address), str, INET_ADDRSTRLEN);

	return str;
}

uint32_t getIpAddressValue(const std::string &address)
{
	uint32_t intAddress = 0xFFFFFFFF;
	if(address != "")
	{
		inet_pton(AF_INET, address.c_str(), &intAddress);
		if(intAddress)
		{
			intAddress = ntohl(intAddress);
		}
	}
	return intAddress;
}

string getDhcpValue(uint16_t dhcpOption)
{
	string ret = "UNKNOWN";

	switch(dhcpOption)
	{
	case 0:
		ret = "NONE";
		break;
	case 1:
		ret = "NORMAL";
		break;
	case 2:
		ret = "CLIENT";
		break;
	}

	return ret;
}

string getPhaseValue(ACS_CS_API_OmProfilePhase::PhaseValue phase)
{
    string ret = "(incorrect value)";

    switch (phase)
    {
        case ACS_CS_API_OmProfilePhase::Idle:
            ret = "Idle";
            break;

        case ACS_CS_API_OmProfilePhase::Validate:
            ret = "Validate";
            break;

        case ACS_CS_API_OmProfilePhase::ApNotify:
            ret = "ApNotify";
            break;

        case ACS_CS_API_OmProfilePhase::CpNotify:
            ret = "CpNotify";
            break;

        case ACS_CS_API_OmProfilePhase::Commit:
            ret = "Commit";
            break;
    }

    return ret;
}

string getFbnValue(uint16_t fbn)
{
	string ret = "UNKNOWN";

	switch(fbn)
	{
	case 100:
		ret = "SCB_RP";
		break;
	case 110:
		ret = "RPBI_S";
		break;
	case 120:
		ret = "GESB";
		break;
	case 200:
		ret = "CPUB";
		break;
	case 210:
		ret = "MAUB";
		break;
	case 300:
		ret = "APUB";
		break;
	case 310:
		ret = "DISK";
		break;
	case 320:
		ret = "DVD";
		break;
	case 330:
		ret = "GEA";
		break;
	}

	return ret;
}

string getChangeReasonValue(ACS_CS_API_OmProfileChange_R1::ChangeReasonValue reason)
{
    string ret = "(invalid value)";

    switch (reason)
    {
        case ACS_CS_API_OmProfileChange_R1::NoChange:
            ret = "NoChange";
            break;

        case ACS_CS_API_OmProfileChange_R1::NechCommand:
            ret = "NechCommand";
            break;

        case ACS_CS_API_OmProfileChange_R1::AutomaticProfileAlignment:
            ret = "AutomaticProfileAlignment";
            break;

        case ACS_CS_API_OmProfileChange_R1::NechCommandForApgOnly:
            ret = "NechCommandForApgOnly";
            break;
    }

    return ret;
}

string getClusterOpModeStr(ACS_CS_API_ClusterOpMode::Value mode)
{
    string ret = "(invalid value)";

    switch (mode)
    {
        case ACS_CS_API_ClusterOpMode::Normal:
            ret = "Normal";
            break;

        case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
            ret = "SwitchingToNormal";
            break;

        case ACS_CS_API_ClusterOpMode::Expert:
            ret = "Expert";
            break;

        case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
            ret = "SwitchingToExpert";
            break;
    }

    return ret;
}

void printProfileChange(const ACS_CS_API_OmProfileChange_R1 &omProfile)
{
    cout << "omProfile" << endl;
    cout << "  phase: " << getPhaseValue(omProfile.phase) << endl;
    cout << "  omProfileCurrent: " << omProfile.omProfileCurrent << endl;
    cout << "  apzCurrent: " << omProfile.apzCurrent << endl;
    cout << "  aptCurrent: " << omProfile.aptCurrent << endl;
    cout << "  omProfileRequested: " << omProfile.omProfileRequested << endl;
    cout << "  apzRequested: " << omProfile.apzRequested << endl;
    cout << "  aptRequested: " << omProfile.aptRequested << endl;
    cout << "  apzQueued: " << omProfile.apzQueued << endl;
    cout << "  aptQueued: " << omProfile.aptQueued << endl;
    cout << "  changeReason: " << getChangeReasonValue(omProfile.changeReason) << endl;
}

void HWC_Observer::update (const ACS_CS_API_HWCTableChange_R1& observee)
{
    cout << "### HWC Table observer has been called ###" << endl << endl;

    for(int i = 0; i < observee.dataSize; i++)
    {
    	const ACS_CS_API_HWCTableData_R1 &data = observee.hwcData[i];

    	cout << "operation type: " << getOperationValue(data.operationType) << endl;
    	cout << "aliasEthA: " << getIpAddressValue(data.aliasEthA) << endl;
    	cout << "aliasEthB: " << getIpAddressValue(data.aliasEthB) << endl;
    	cout << "dhcp method: " << getDhcpValue(data.dhcpMethod) << endl;
    	cout << "fbn: " << getFbnValue(data.fbn) << endl;
    	cout << "ipEthA: " << getIpAddressValue(data.ipEthA) << endl;
    	cout << "ipEthB: " << getIpAddressValue(data.ipEthB) << endl;
    	cout << "magazine: " << getIpAddressValue(data.magazine) << endl;
    	cout << "netmaskAliasEthA: " << getIpAddressValue(data.netmaskAliasEthA) << endl;
    	cout << "netmaskAliasEthB: " << getIpAddressValue(data.netmaskAliasEthB) << endl;
    	cout << "sequence number: " << data.seqNo << endl;
    	cout << "side: " << data.side << endl;
    	cout << "slot number: " << data.slot << endl;
    	cout << "system id: " << data.sysId << endl;
    	cout << "system number: " << data.sysNo << endl;
    	cout << "system type: " << data.sysType << endl;
    	cout << "product id: " << data.productId << endl;
		cout << "product revision: " << data.productRevision << endl;
		cout << "software package: " << data.softwarePackage << endl;
    }

    cout << endl << endl;
}

void NE_Observer::update (const ACS_CS_API_NetworkElementChange& observee)
{
    cout << "### NE observer has been called ###" << endl;

    char buff[1024];
    memset(&buff, 0, sizeof(buff));
    size_t buflen = sizeof(buff);
    observee.neId.getName((char*) &buff, buflen);

    cout << "neId: " << buff << endl;
    printProfileChange(observee.omProfile);
    cout << "clusterMode: " << getClusterOpModeStr(observee.clusterMode) << endl;
    cout << "trafficIsolatedCpId: " << observee.trafficIsolatedCpId << endl;
    cout << "trafficLeaderCpId: " << observee.trafficLeaderCpId << endl;
    cout << "alarmMasterCpId: " << observee.alarmMasterCpId << endl;
    cout << "clockMasterCpId: " << observee.clockMasterCpId << endl;
    cout << endl << endl;

    if (observee.clusterMode == ACS_CS_API_ClusterOpMode::SwitchingToExpert)
    {
        cout << "###############################################################" << endl;
        cout << "Got request for switching cluster operation mode to Expert mode" << endl;
        cout << "Acknowleding request in 5 seconds" << endl << endl;

        sleep(5);
        ACS_CS_API_Set::setClusterOpModeChanged(ACS_CS_API_RequestedClusterOpMode::Expert);
        cout << "Cluster operation mode confirmed!" << endl;

        cout << "###############################################################" << endl << endl;
    }
    else if (observee.clusterMode == ACS_CS_API_ClusterOpMode::SwitchingToNormal)
    {
        cout << "###############################################################" << endl;
        cout << "Got request for switching cluster operation mode to Normal mode" << endl;
        cout << "Acknowleding request in 5 seconds" << endl << endl;

        sleep(5);
        ACS_CS_API_Set::setClusterOpModeChanged(ACS_CS_API_RequestedClusterOpMode::Normal);
        cout << "Cluster operation mode confirmed!" << endl;

        cout << "###############################################################" << endl << endl;
    }
}

void OgChange_Observer::update (const ACS_CS_API_OgChange& observer)
{
    cout << "### Og (operational group) change has been called ###" << endl;
    cout << "Reason code: ";

    switch (observer.emptyOgReason)
    {
        case ACS_CS_API_OgChange::NotEmpty:
            cout << "NotEmpty" << endl;
            break;

        case ACS_CS_API_OgChange::ClusterRecovery:
            cout << "ClusterRecovery" << endl;
            break;

        case ACS_CS_API_OgChange::OtherReason:
            cout << "OtherReason" << endl;
            break;
    }

    cout << "cpIdList, number of items: " << observer.ogCpIdList.size() << endl;

    for (size_t i = 0;i < observer.ogCpIdList.size(); i++)
    {
        cout << "   cpid[" << i << "] - " << observer.ogCpIdList[i] << endl;
    }

    cout << endl << endl;
}


void CpChange_Observer::update (const ACS_CS_API_CpTableChange_R1& observer)
{
    ACS_CS_API_CpTableData_R1 *data = observer.cpData;

    cout << "### Cp table change has been called ###" << endl;
    cout << "CPId: " << data->cpId << endl;
    cout << "OperationType: " << data->operationType << endl;

    char *buff = new char[1024];
    size_t size = sizeof(buff);

    data->cpName.getName(buff, size);
    cout << "cpName: " << buff << endl;

    data->cpAliasName.getName(buff, size);
    cout << "cpAliasname: " << buff << endl;

    cout << "apzSystem: " << data->apzSystem << endl;
    cout << "cpType: " << data->cpType << endl;
    cout << "cpState: " << data->cpState << endl;
    cout << "applicationId: " << data->applicationId << endl;
    cout << "apzSubstate: " << data->apzSubstate << endl;
    cout << "aptSubstate: " << data->aptSubstate << endl;
    cout << "stateTransition: " << data->stateTransition << endl;
    cout << "blockingInfo: " << data->blockingInfo << endl;
    cout << "cpCapacity: " << data->cpCapacity << endl;
    cout << "datasize: " << data->dataSize << endl;

    cout << endl << endl;
}


void OmProfileChange_Observer::update (const ACS_CS_API_OmProfileChange_R1& observer)
{
    cout << "### Om profile change has been called ###" << endl;
    printProfileChange(observer);

    if (ACS_CS_API_OmProfilePhase::Validate == observer.phase || ACS_CS_API_OmProfilePhase::CpNotify == observer.phase)
    {
    	ACS_CS_API_SET_NS::CS_API_Set_Result result;

    	sleep(2);

    	if(observer.omProfileRequested != 0 && observer.omProfileRequested != 100){

			cout << "NOT ACCEPTED: Status is Validate but profile not 0 or 100. Calling setOmProfileNotificationStatusFailure... ";

			result = ACS_CS_API_Set::setOmProfileNotificationStatusFailure(observer.phase, ACS_CS_API_Set::CommandClassificationMissing);

			if ( ACS_CS_API_SET_NS::Result_Success != result )
			{
				cout << endl << "ERROR in call to setOmProfileNotificationStatusFailure()" << endl;
			}
			else
			{
				cout << "Done." << endl;
			}

			return;
		}

    	if (ACS_CS_API_OmProfilePhase::Validate == observer.phase){
    		ACS_CS_API_Set::setProfiles(observer.omProfileRequested, observer.aptCurrent, observer.apzCurrent);
    		//ACS_CS_API_Set::setProfiles(0, 0, 0);
    	}

        sleep(2);

    	cout << "ACCEPTED: Status is " << getPhaseValue(observer.phase) << ". Calling setOmProfileNotificationStatusSuccess()... ";

    	result = ACS_CS_API_Set::setOmProfileNotificationStatusSuccess(observer.phase);

		if ( ACS_CS_API_SET_NS::Result_Success != result )
		{
			cout << endl << "ERROR: Could not commit status changes" << endl;
		}
		else
		{
			cout << "Done." << endl;
		}
    }

    cout << endl << endl;
}

void cs_subscription_init()
{
    cout << "Getting instance of subscribe api" << endl;
    ACS_CS_API_SubscriptionMgr *instance = ACS_CS_API_SubscriptionMgr::getInstance();


	cout << "Subscribing for HWC Table notifications" << endl;

	hwc = new HWC_Observer();
	if (instance->subscribeHWCTableChanges(*hwc) !=  ACS_CS_API_NS::Result_Success)
	{
		cout << "Could not register for Hwc Table changes" << endl;
	}

    cout << "Subscribing for NE notifications" << endl;

    ne = new NE_Observer();
    if (instance->subscribeNetworkElementChanges(*ne) !=  ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not register for NE changes" << endl;
    }

    cout << "Subscribing for Og changes" << endl;

    og = new OgChange_Observer();
    if (instance->subscribeOgChanges(*og) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not register for Og changes" << endl;
    }

    cout << "Subscribing for Cp table changes" << endl;

    cp = new CpChange_Observer();
    if (instance->subscribeCpTableChanges(*cp) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not register for CP table changes" << endl;
    }

    ACS_CS_API_OmProfilePhase::PhaseValue phase = ACS_CS_API_OmProfilePhase::CpNotify;

    cout << "Subscribing for Om profile changes (" << getPhaseValue(phase) << ")" << endl;

    om = new OmProfileChange_Observer();



    if (instance->subscribeOmProfilePhaseChanges(phase, *om) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not register for Om profile changes" << endl;
    }

    cout << "Subscribe complete" << endl;
}


void cs_subscription_cleanup()
{
    cout << "Getting instance of subscribe api" << endl;
    ACS_CS_API_SubscriptionMgr *instance = ACS_CS_API_SubscriptionMgr::getInstance();

    cout << "Unsubscribing for HWC Table notifications" << endl;

	if (instance->unsubscribeHWCTableChanges(*hwc) != ACS_CS_API_NS::Result_Success)
	{
		cout << "Could not unregister for HWC Table changes" << endl;
	}

    cout << "Unsubscribing for NE notifications" << endl;

    if (instance->unsubscribeNetworkElementChanges(*ne) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not unregister for NE changes" << endl;
    }

    cout << "Unsubscribing for Og changes" << endl;

    if (instance->unsubscribeOgChanges(*og) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not unregister for Og changes" << endl;
    }

    cout << "Unsubscribing for Cp table changes" << endl;

    if (instance->unsubscribeCpTableChanges(*cp) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not unregister for CP table changes" << endl;
    }

    cout << "Unsubscribing for Om profile changes" << endl;

    if (instance->unsubscribeOmProfilePhaseChanges(ACS_CS_API_OmProfilePhase::Validate, *om) != ACS_CS_API_NS::Result_Success)
    {
        cout << "Could not unregister for Om profile changes" << endl;
    }

    cout << "Unsubscribe complete" << endl;
}

