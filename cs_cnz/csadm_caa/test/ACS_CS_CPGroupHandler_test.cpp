#include "ACS_CS_ImBase.h"
#include "ACS_CS_CPGroupHandler.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_TableOperationWorker.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_SubscriptionAgent.h"

#include <set>

#include "gtest/gtest.h"
#include "gmock/gmock.h"


class OgChange_Observer : public ACS_CS_API_OgObserver
{
    virtual void update(const ACS_CS_API_OgChange& change)
    {
    	data = change;
    	updateCalled = true;
    }

public:
    OgChange_Observer() : updateCalled(false) {}

    ACS_CS_API_OgChange data;
    bool updateCalled;
};

class SubscriptionContextOg {
public:
	~SubscriptionContextOg()
	{
		TearDown();
	}

	static SubscriptionContextOg * instance()
	{
		if(NULL == instance_)
		{
			instance_ = new SubscriptionContextOg();
			static SubscriptionContextOg::SubscriptionContextDestroyer d;
		}

		return instance_;
	}

private:
	static SubscriptionContextOg *instance_;

	SubscriptionContextOg()
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

		ogSubscriber = new OgChange_Observer();
		if (ACS_CS_API_SubscriptionMgr::getInstance()->subscribeOgChanges(*ogSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			FAIL() << "Could not register for CPGroup table changes!";
		}
		sleep(2);
	}

	void TearDown()
	{
		if (ACS_CS_API_SubscriptionMgr::getInstance()->unsubscribeOgChanges(*ogSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			delete ogSubscriber;
			FAIL() << "Could not unregister for CPGroup table changes!";
		}
		delete ogSubscriber;

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
	OgChange_Observer *ogSubscriber;
	ACS_CS_ReactorRunner *reactorRunner;

private:
	class SubscriptionContextDestroyer {
	public:
		~SubscriptionContextDestroyer()
		{
			delete SubscriptionContextOg::instance_;
		}
	};
};

SubscriptionContextOg *SubscriptionContextOg::instance_ = NULL;
static SubscriptionContextOg *subscriptionContextInstance = SubscriptionContextOg::instance();

TEST(subscription, handleGroupSubscription_OtherReason)
{
	// Arrange
	ACS_CS_CPGroupHandler *handler = new ACS_CS_CPGroupHandler();

	ACS_CS_ImCpGroup *opGroup = new ACS_CS_ImCpGroup();
	opGroup->type = CPGROUP_T;
	opGroup->action = ACS_CS_ImBase::CREATE;
	opGroup->cpGroupId = "OPGROUP";
	opGroup->groupName = "OPGROUP";
	opGroup->rdn = "cpGroupId=OPGROUP,cpGroupCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
	opGroup->cpRdns.clear();

	ACS_CS_ImBladeClusterInfo *bladeClusterInfo = new ACS_CS_ImBladeClusterInfo();
	bladeClusterInfo->type = BLADECLUSTERINFO_T;
	bladeClusterInfo->action = ACS_CS_ImBase::MODIFY;
	bladeClusterInfo->rdn = "bladeClusterInfoId=1,configurationInfoId=1";
	bladeClusterInfo->ogClearCode = OTHER_REASON;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(opGroup);
	subset->addObject(bladeClusterInfo);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->ogSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	ACS_CS_API_OgChange ogData = subscriptionContextInstance->ogSubscriber->data;

	EXPECT_EQ(bladeClusterInfo->ogClearCode, static_cast<OgClearCode>(ogData.emptyOgReason));
	EXPECT_EQ(opGroup->cpRdns.size(), ogData.ogCpIdList.size());

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->ogSubscriber->updateCalled = false;
}

TEST(subscription, handleGroupSubscription_ClusterRecovery)
{
	// Arrange
	ACS_CS_CPGroupHandler *handler = new ACS_CS_CPGroupHandler();

	ACS_CS_ImCpGroup *opGroup = new ACS_CS_ImCpGroup();
	opGroup->type = CPGROUP_T;
	opGroup->action = ACS_CS_ImBase::MODIFY;
	opGroup->cpGroupId = "OPGROUP";
	opGroup->groupName = "OPGROUP";
	opGroup->rdn = "cpGroupId=OPGROUP,cpGroupCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
	opGroup->cpRdns.clear();

	ACS_CS_ImBladeClusterInfo *bladeClusterInfo = new ACS_CS_ImBladeClusterInfo();
	bladeClusterInfo->type = BLADECLUSTERINFO_T;
	bladeClusterInfo->action = ACS_CS_ImBase::MODIFY;
	bladeClusterInfo->rdn = "bladeClusterInfoId=1,configurationInfoId=1";
	bladeClusterInfo->ogClearCode = CLUSTER_RECOVERY;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(opGroup);
	subset->addObject(bladeClusterInfo);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->ogSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	ACS_CS_API_OgChange ogData = subscriptionContextInstance->ogSubscriber->data;

	EXPECT_EQ(bladeClusterInfo->ogClearCode, static_cast<OgClearCode>(ogData.emptyOgReason));
	EXPECT_EQ(opGroup->cpRdns.size(), ogData.ogCpIdList.size());

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->ogSubscriber->updateCalled = false;
}

TEST(subscription, handleGroupSubscription_NotEmpty)
{
	// Arrange
	ACS_CS_CPGroupHandler *handler = new ACS_CS_CPGroupHandler();

	ACS_CS_ImCp *cp = new ACS_CS_ImCp();
	cp->type = CP_T;
	cp->action = ACS_CS_ImBase::CREATE;
	cp->rdn = "cpId=1234,cpCategoryId=1,configurationInfoId=1";
	cp->cpId = "cpId=1234";
	cp->systemIdentifier = 321;
	cp->defaultName = "BC60";
	cp->alias = "AliaS";
	cp->apzSystem = 0;
	cp->cpType = 21200;

	ACS_CS_ImCpGroup *opGroup = new ACS_CS_ImCpGroup();
	opGroup->type = CPGROUP_T;
	opGroup->action = ACS_CS_ImBase::MODIFY;
	opGroup->cpGroupId = "OPGROUP";
	opGroup->groupName = "OPGROUP";
	opGroup->rdn = "cpGroupId=OPGROUP,cpGroupCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
	opGroup->cpRdns.insert(cp->rdn);

	ACS_CS_ImBladeClusterInfo *bladeClusterInfo = new ACS_CS_ImBladeClusterInfo();
	bladeClusterInfo->type = BLADECLUSTERINFO_T;
	bladeClusterInfo->action = ACS_CS_ImBase::MODIFY;
	bladeClusterInfo->rdn = "bladeClusterInfoId=1,configurationInfoId=1";
	bladeClusterInfo->ogClearCode = NOT_EMPTY;

	ACS_CS_ImRepository *repo = ACS_CS_ImRepository::instance();
	repo->getModel()->addObject(cp);

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(opGroup);
	subset->addObject(bladeClusterInfo);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->ogSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	ACS_CS_API_OgChange ogData = subscriptionContextInstance->ogSubscriber->data;

	EXPECT_EQ(bladeClusterInfo->ogClearCode, static_cast<OgClearCode>(ogData.emptyOgReason));
	EXPECT_EQ(opGroup->cpRdns.size(), ogData.ogCpIdList.size());

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->ogSubscriber->updateCalled = false;
}
