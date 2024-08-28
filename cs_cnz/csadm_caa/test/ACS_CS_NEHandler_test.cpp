#include "ACS_CS_ImBase.h"
#include "ACS_CS_TableOperation.h"
#include "ACS_CS_NEHandler.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_TableOperationWorker.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_SubscriptionAgent.h"

#include <set>

#include "gtest/gtest.h"
#include "gmock/gmock.h"


class NEChange_Observer : public ACS_CS_API_NetworkElementObserver
{
    virtual void update(const ACS_CS_API_NetworkElementChange& change)
    {
    	data = change;
    	updateCalled = true;
    }

public:
    NEChange_Observer() : updateCalled(false) {}

    ACS_CS_API_NetworkElementChange data;
    bool updateCalled;
};

class OmProfileChange_Observer : public ACS_CS_API_OmProfilePhaseObserver
{
    virtual void update(const ACS_CS_API_OmProfileChange& change)
    {
    	data = change;
    	updateCalled = true;
    }

public:
    OmProfileChange_Observer() : updateCalled(false) {}

    ACS_CS_API_OmProfileChange data;
    bool updateCalled;
};

class SubscriptionContextNE {
public:
	~SubscriptionContextNE()
	{
		TearDown();
	}

	static SubscriptionContextNE * instance()
	{
		if(NULL == instance_)
		{
			instance_ = new SubscriptionContextNE();
			static SubscriptionContextNE::SubscriptionContextDestroyer d;
		}

		return instance_;
	}

private:
	static SubscriptionContextNE *instance_;

	SubscriptionContextNE()
	{
		SetUp();
	}

	void SetUp()
	{
		CCBID = 99999;
		handle1 = ACS_CS_Event::CreateEvent(true, false, ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);
		handle2 = ACS_CS_Event::CreateEvent(true, false, ACS_CS_NS::EVENT_NAME_SHUTDOWN);

		reactorRunner = new ACS_CS_ReactorRunner();
		reactorRunner->activate();
		sleep(1);
		ACS_CS_SubscriptionAgent::getInstance()->activate();

		neSubscriber = new NEChange_Observer();
		if (ACS_CS_API_SubscriptionMgr::getInstance()->subscribeNetworkElementChanges(*neSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			FAIL() << "Could not register for NE table changes!";
		}
		omProfileSubscriber = new OmProfileChange_Observer();
		if (ACS_CS_API_SubscriptionMgr::getInstance()->subscribeOmProfilePhaseChanges(ACS_CS_API_OmProfilePhase::CpNotify, *omProfileSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			FAIL() << "Could not register for OmProfile changes!";
		}
		sleep(2);
	}

	void TearDown()
	{
		if (ACS_CS_API_SubscriptionMgr::getInstance()->unsubscribeNetworkElementChanges(*neSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			delete neSubscriber;
			FAIL() << "Could not unregister for NE table changes!";
		}
		delete neSubscriber;

		if (ACS_CS_API_SubscriptionMgr::getInstance()->unsubscribeOmProfilePhaseChanges(ACS_CS_API_OmProfilePhase::CpNotify, *omProfileSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			delete omProfileSubscriber;
			FAIL() << "Could not unregister for OmProfile changes!";
		}
		delete omProfileSubscriber;

		ACS_CS_Event::SetEvent(handle1);
		ACS_CS_Event::SetEvent(handle2);

		ACS_CS_SubscriptionAgent::getInstance()->deActivate();
		ACS_CS_SubscriptionAgent::getInstance()->wait(3);
//		ACS_CS_SubscriptionAgent::stop();

		reactorRunner->deActivate();
		reactorRunner->wait(3);
	}

public:
	ACS_CS_EventHandle handle1;
	ACS_CS_EventHandle handle2;
	unsigned long long CCBID;
	NEChange_Observer *neSubscriber;
	OmProfileChange_Observer *omProfileSubscriber;
	ACS_CS_ReactorRunner *reactorRunner;

private:
	class SubscriptionContextDestroyer {
	public:
		~SubscriptionContextDestroyer()
		{
			delete SubscriptionContextNE::instance_;
		}
	};
};

SubscriptionContextNE *SubscriptionContextNE::instance_ = NULL;
static SubscriptionContextNE *subscriptionContextInstance = SubscriptionContextNE::instance();

TEST(subscription, DISABLED_handleNeTableSubscription)
{
	// Arrange
	ACS_CS_NEHandler *handler = new ACS_CS_NEHandler();

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();

	const ACS_CS_ImAdvancedConfiguration *advConfConst = dynamic_cast<const ACS_CS_ImAdvancedConfiguration *>(model->getObject("advancedConfigurationId=1,configurationInfoId=1"));
	const ACS_CS_ImBladeClusterInfo *bladeClusterConst = dynamic_cast<const ACS_CS_ImBladeClusterInfo *>(model->getObject("bladeClusterInfoId=1,configurationInfoId=1"));

	if(NULL == advConfConst || NULL == bladeClusterConst)
	{
		FAIL() << "Could not fetch ACS_CS_ImAdvancedConfiguration and/or ACS_CS_ImBladeClusterInfo!";
	}

	ACS_CS_ImAdvancedConfiguration *advConf;
	ACS_CS_ImBladeClusterInfo *bladeCluster;

	advConf = dynamic_cast<ACS_CS_ImAdvancedConfiguration *>(advConfConst->clone());
	bladeCluster = dynamic_cast<ACS_CS_ImBladeClusterInfo *>(bladeClusterConst->clone());

	if(NULL == advConfConst || NULL == bladeClusterConst)
	{
		FAIL() << "Could not dynamic_cast ACS_CS_ImAdvancedConfiguration and/or ACS_CS_ImBladeClusterInfo!";
	}

	bladeCluster->alarmMaster = 1;
	bladeCluster->clockMaster = 2;
	bladeCluster->clusterOpMode = EXPERT;
	advConf->trafficIsolatedCp = 5234;
	advConf->trafficLeaderCp = 1234;

	advConf->action = ACS_CS_ImBase::MODIFY;
	bladeCluster->action = ACS_CS_ImBase::MODIFY;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(bladeCluster);
	subset->addObject(advConf);

	model->applySubset(subset);

	advConfConst = dynamic_cast<const ACS_CS_ImAdvancedConfiguration *>(model->getObject("advancedConfigurationId=1,configurationInfoId=1"));

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->neSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	ACS_CS_API_NetworkElementChange neData = subscriptionContextInstance->neSubscriber->data;

	EXPECT_EQ(bladeCluster->clusterOpMode, static_cast<ClusterOpModeEnum>(neData.clusterMode));

	// TODO! Handle this!
	//EXPECT_EQ(ACS_CS_NEHandler::getCPID(bladeCluster->alarmMaster), neData.alarmMasterCpId);
	//EXPECT_EQ(ACS_CS_NEHandler::getCPID(bladeCluster->clockMaster), neData.clockMasterCpId);
	EXPECT_EQ(advConf->trafficIsolatedCp, neData.trafficIsolatedCpId);
	EXPECT_EQ(advConf->trafficLeaderCp, neData.trafficLeaderCpId);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->neSubscriber->updateCalled = false;
}

TEST(subscription, DISABLED_handlePhaseChange)
{
	// Arrange
	ACS_CS_NEHandler *handler = new ACS_CS_NEHandler();

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();

	const ACS_CS_ImAdvancedConfiguration *advConfConst = dynamic_cast<const ACS_CS_ImAdvancedConfiguration *>(model->getObject("advancedConfigurationId=1,configurationInfoId=1"));
	const ACS_CS_ImBladeClusterInfo *bladeClusterConst = dynamic_cast<const ACS_CS_ImBladeClusterInfo *>(model->getObject("bladeClusterInfoId=1,configurationInfoId=1"));

	if(NULL == advConfConst || NULL == bladeClusterConst)
	{
		FAIL() << "Could not fetch ACS_CS_ImAdvancedConfiguration and/or ACS_CS_ImBladeClusterInfo!";
	}

	ACS_CS_ImAdvancedConfiguration *advConf;
	ACS_CS_ImBladeClusterInfo *bladeCluster;

	advConf = dynamic_cast<ACS_CS_ImAdvancedConfiguration *>(advConfConst->clone());
	bladeCluster = dynamic_cast<ACS_CS_ImBladeClusterInfo *>(bladeClusterConst->clone());

	if(NULL == advConfConst || NULL == bladeClusterConst)
	{
		FAIL() << "Could not dynamic_cast ACS_CS_ImAdvancedConfiguration and/or ACS_CS_ImBladeClusterInfo!";
	}

	advConf->aptProfileCurrent = 1;
	advConf->aptProfileQueued = 2;
	advConf->aptProfileRequested = 3;
	advConf->apzProfileCurrent = 4;
	advConf->apzProfileQueued = 5;
	advConf->apzProfileRequested = 6;
	advConf->omProfileChangeTrigger = AUTOMATIC;
	bladeCluster->omProfile = 8;
	advConf->omProfileRequested = 9;
	bladeCluster->phase = CP_NOTIFY;
	advConf->initiateUpdatePhaseChange = true;

	advConf->action = ACS_CS_ImBase::MODIFY;
	bladeCluster->action = ACS_CS_ImBase::MODIFY;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(bladeCluster);
	subset->addObject(advConf);

	model->applySubset(subset);

	ACS_CS_SubscriptionAgent::getInstance()->enableProfileChangeNotification();

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->omProfileSubscriber->updateCalled && count < 10)
	{
		sleep(1);
		count++;
	}

	// Assert
	ACS_CS_API_OmProfileChange omProfileData = subscriptionContextInstance->omProfileSubscriber->data;

	EXPECT_EQ(advConf->aptProfileCurrent, omProfileData.aptCurrent);
	EXPECT_EQ(advConf->aptProfileQueued, omProfileData.aptQueued);
	EXPECT_EQ(advConf->aptProfileRequested, omProfileData.aptRequested);
	EXPECT_EQ(advConf->apzProfileCurrent, omProfileData.apzCurrent);
	EXPECT_EQ(advConf->apzProfileQueued, omProfileData.apzQueued);
	EXPECT_EQ(advConf->apzProfileRequested, omProfileData.apzRequested);
	EXPECT_EQ(advConf->omProfileChangeTrigger, static_cast<ProfileChangeTriggerEnum>(omProfileData.changeReason));
	EXPECT_EQ(bladeCluster->omProfile, omProfileData.omProfileCurrent);
	EXPECT_EQ(advConf->omProfileRequested, omProfileData.omProfileRequested);
	EXPECT_EQ(bladeCluster->phase, static_cast<OmProfilePhaseEnum>(omProfileData.phase));

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->omProfileSubscriber->updateCalled = false;
}
