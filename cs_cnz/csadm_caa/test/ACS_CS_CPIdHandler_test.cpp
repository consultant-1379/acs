#include "ACS_CS_ImBase.h"
#include "ACS_CS_CPIdHandler.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_TableOperationWorker.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_Util.h"

#include <set>

#include "gtest/gtest.h"
#include "gmock/gmock.h"


class CpChange_Observer : public ACS_CS_API_CpTableObserver
{
    virtual void update(const ACS_CS_API_CpTableChange_R1& change)
    {
    	data = *(change.cpData);
    	updateCalled = true;
    }

public:
    CpChange_Observer() : updateCalled(false) {}

    ACS_CS_API_CpTableData_R1 data;
    bool updateCalled;
};

class SubscriptionContextCp {
public:
	~SubscriptionContextCp()
	{
		TearDown();
	}

	static SubscriptionContextCp * instance()
	{
		if(NULL == instance_)
		{
			instance_ = new SubscriptionContextCp();
			static SubscriptionContextCp::SubscriptionContextDestroyer d;
		}

		return instance_;
	}

private:
	static SubscriptionContextCp *instance_;

	SubscriptionContextCp()
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

		cpSubscriber = new CpChange_Observer();
		if (ACS_CS_API_SubscriptionMgr::getInstance()->subscribeCpTableChanges(*cpSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			FAIL() << "Could not register for CP table changes!";
		}
		sleep(2);
	}

	void TearDown()
	{
		if (ACS_CS_API_SubscriptionMgr::getInstance()->unsubscribeCpTableChanges(*cpSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			delete cpSubscriber;
			FAIL() << "Could not unregister for CP table changes!";
		}
		delete cpSubscriber;

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
	CpChange_Observer *cpSubscriber;
	ACS_CS_ReactorRunner *reactorRunner;

private:
	class SubscriptionContextDestroyer {
	public:
		~SubscriptionContextDestroyer()
		{
			delete SubscriptionContextCp::instance_;
		}
	};
};

SubscriptionContextCp *SubscriptionContextCp::instance_ = NULL;
static SubscriptionContextCp *subscriptionContextInstance = SubscriptionContextCp::instance();

TEST(subscription, handleCPIdTableSubscription_Create)
{
	// Arrange
	ACS_CS_CPIdHandler *handler = new ACS_CS_CPIdHandler();

	ACS_CS_ImCp *cp = new ACS_CS_ImCp();
	cp->type = CP_T;
	cp->action = ACS_CS_ImBase::CREATE;
	cp->rdn = "cpId=123,cpCategoryId=1,configurationInfoId=1";
	cp->cpId = "cpId=123";
	cp->systemIdentifier = 321;
	cp->defaultName = "BC1";
	cp->alias = "AliaS";
	cp->apzSystem = 0;
	cp->cpType = 21200;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->cpSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	char tmp[128];
	unsigned long size = 128;

	CpChange_Observer *cpSubscriber = subscriptionContextInstance->cpSubscriber;

	EXPECT_EQ(cp->applicationId, cpSubscriber->data.applicationId);
	EXPECT_EQ(cp->aptSubstate, cpSubscriber->data.aptSubstate);
	EXPECT_EQ(cp->apzSubstate, cpSubscriber->data.apzSubstate);
	EXPECT_EQ(cp->apzSystem, cpSubscriber->data.apzSystem);
	EXPECT_EQ(cp->blockingInfo, cpSubscriber->data.blockingInfo);
	cpSubscriber->data.cpAliasName.getName(tmp, size);
	EXPECT_TRUE(cp->alias.compare(tmp) == 0);
	EXPECT_EQ(cp->cpCapacity, cpSubscriber->data.cpCapacity);
	cpSubscriber->data.cpName.getName(tmp, size);
	EXPECT_TRUE(cp->defaultName.compare(tmp) == 0);
	EXPECT_EQ(cp->cpState, cpSubscriber->data.cpState);
	EXPECT_EQ(cp->cpType, cpSubscriber->data.cpType);
	EXPECT_EQ(cp->stateTransition, cpSubscriber->data.stateTransition);
	EXPECT_EQ(cp->systemIdentifier, cpSubscriber->data.cpId);
	EXPECT_EQ(ACS_CS_API_TableChangeOperation::Add, cpSubscriber->data.operationType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->cpSubscriber->updateCalled = false;
}

TEST(subscription, handleCPIdTableSubscription_Modify)
{
	// Arrange
	ACS_CS_CPIdHandler *handler = new ACS_CS_CPIdHandler();

	ACS_CS_ImCp *cp = new ACS_CS_ImCp();
	cp->type = CP_T;
	cp->action = ACS_CS_ImBase::MODIFY;
	cp->rdn = "cpId=123,cpCategoryId=1,configurationInfoId=1";
	cp->cpId = "cpId=123";
	cp->systemIdentifier = 421;
	cp->defaultName = "BC2";
	cp->alias = "AliaS";
	cp->apzSystem = 0;
	cp->cpType = 21201;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->cpSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	char tmp[128];
	unsigned long size = 128;

	CpChange_Observer *cpSubscriber = subscriptionContextInstance->cpSubscriber;

	EXPECT_EQ(cp->applicationId, cpSubscriber->data.applicationId);
	EXPECT_EQ(cp->aptSubstate, cpSubscriber->data.aptSubstate);
	EXPECT_EQ(cp->apzSubstate, cpSubscriber->data.apzSubstate);
	EXPECT_EQ(cp->apzSystem, cpSubscriber->data.apzSystem);
	EXPECT_EQ(cp->blockingInfo, cpSubscriber->data.blockingInfo);
	cpSubscriber->data.cpAliasName.getName(tmp, size);
	EXPECT_TRUE(cp->alias.compare(tmp) == 0);
	EXPECT_EQ(cp->cpCapacity, cpSubscriber->data.cpCapacity);
	cpSubscriber->data.cpName.getName(tmp, size);
	EXPECT_TRUE(cp->defaultName.compare(tmp) == 0);
	EXPECT_EQ(cp->cpState, cpSubscriber->data.cpState);
	EXPECT_EQ(cp->cpType, cpSubscriber->data.cpType);
	EXPECT_EQ(cp->stateTransition, cpSubscriber->data.stateTransition);
	EXPECT_EQ(cp->systemIdentifier, cpSubscriber->data.cpId);
	EXPECT_EQ(ACS_CS_API_TableChangeOperation::Change, cpSubscriber->data.operationType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->cpSubscriber->updateCalled = false;
}

TEST(subscription, handleCPIdTableSubscription_Delete)
{
	// Arrange
	ACS_CS_CPIdHandler *handler = new ACS_CS_CPIdHandler();

	ACS_CS_ImCp *cp = new ACS_CS_ImCp();
	cp->type = CP_T;
	cp->action = ACS_CS_ImBase::DELETE;
	cp->rdn = "cpId=123,cpCategoryId=1,configurationInfoId=1";
	cp->cpId = "cpId=123";
	cp->systemIdentifier = 521;
	cp->defaultName = "BC3";
	cp->alias = "AliaS";
	cp->apzSystem = 0;
	cp->cpType = 21202;

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->cpSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	// Assert
	char tmp[128];
	unsigned long size = 128;

	CpChange_Observer *cpSubscriber = subscriptionContextInstance->cpSubscriber;

	EXPECT_EQ(cp->applicationId, cpSubscriber->data.applicationId);
	EXPECT_EQ(cp->aptSubstate, cpSubscriber->data.aptSubstate);
	EXPECT_EQ(cp->apzSubstate, cpSubscriber->data.apzSubstate);
	EXPECT_EQ(cp->apzSystem, cpSubscriber->data.apzSystem);
	EXPECT_EQ(cp->blockingInfo, cpSubscriber->data.blockingInfo);
	cpSubscriber->data.cpAliasName.getName(tmp, size);
	EXPECT_TRUE(cp->alias.compare(tmp) == 0);
	EXPECT_EQ(cp->cpCapacity, cpSubscriber->data.cpCapacity);
	cpSubscriber->data.cpName.getName(tmp, size);
	EXPECT_TRUE(cp->defaultName.compare(tmp) == 0);
	EXPECT_EQ(cp->cpState, cpSubscriber->data.cpState);
	EXPECT_EQ(cp->cpType, cpSubscriber->data.cpType);
	EXPECT_EQ(cp->stateTransition, cpSubscriber->data.stateTransition);
	EXPECT_EQ(cp->systemIdentifier, cpSubscriber->data.cpId);
	EXPECT_EQ(ACS_CS_API_TableChangeOperation::Delete, cpSubscriber->data.operationType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->cpSubscriber->updateCalled = false;
}
