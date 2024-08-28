#include "ACS_CS_ImBase.h"
#include "ACS_CS_HWCHandler.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_TableOperationWorker.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_SubscriptionAgent.h"
#include "ACS_CS_API_HWCTableObserver_R1.h"
#include "ACS_CS_API_HWCTableChange_R1.h"
#include "ACS_CS_ImDefaultObjectCreator.h"


#include <set>

#include "gtest/gtest.h"
#include "gmock/gmock.h"


class HWCChange_Observer : public ACS_CS_API_HWCTableObserver_R1
{
    virtual void update(const ACS_CS_API_HWCTableChange_R1& change)
    {
    	data.dataSize = change.dataSize;
    	if(data.dataSize > 0)
    	{
    		delete[] data.hwcData;
			data.hwcData = new ACS_CS_API_HWCTableData_R1[data.dataSize];
//			memcpy(data.hwcData, change.hwcData, data.dataSize * sizeof(ACS_CS_API_HWCTableData_R1));

			for(int i = 0; i < data.dataSize; i++)
			{
				data.hwcData[i] = change.hwcData[i];
			}
    	}
    	updateCalled = true;
    }

public:
    HWCChange_Observer() : updateCalled(false)
    {
    	data.dataSize = 0;
    	data.hwcData = NULL;
    }

    ~HWCChange_Observer()
    {
    	delete[] data.hwcData;
    }

    ACS_CS_API_HWCTableChange_R1 data;
    bool updateCalled;
};

class SubscriptionContextHWC {
public:
	~SubscriptionContextHWC()
	{
		TearDown();
	}

	static SubscriptionContextHWC * instance()
	{
		if(NULL == instance_)
		{
			instance_ = new SubscriptionContextHWC();
			static SubscriptionContextHWC::SubscriptionContextDestroyer d;
		}

		return instance_;
	}

private:
	static SubscriptionContextHWC *instance_;

	SubscriptionContextHWC()
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

		hwcSubscriber = new HWCChange_Observer();
		if (ACS_CS_API_SubscriptionMgr::getInstance()->subscribeHWCTableChanges(*hwcSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			FAIL() << "Could not register for HWC table changes!";
		}
		sleep(2);
	}

	void TearDown()
	{
		if (ACS_CS_API_SubscriptionMgr::getInstance()->unsubscribeHWCTableChanges(*hwcSubscriber) != ACS_CS_API_NS::Result_Success)
		{
			delete hwcSubscriber;
			FAIL() << "Could not unregister for HWC table changes!";
		}
		delete hwcSubscriber;

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
	HWCChange_Observer *hwcSubscriber;
	ACS_CS_ReactorRunner *reactorRunner;

private:
	class SubscriptionContextDestroyer {
	public:
		~SubscriptionContextDestroyer()
		{
			delete SubscriptionContextHWC::instance_;
		}
	};
};

SubscriptionContextHWC *SubscriptionContextHWC::instance_ = NULL;
static SubscriptionContextHWC *subscriptionContextInstance = SubscriptionContextHWC::instance();

TEST(subscription, handleHWCSubscription_CpBlade_Create)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();




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

	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	blade->type = CPBLADE_T;
	blade->action = ACS_CS_ImBase::CREATE;
	blade->rdn = "cpBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->cpRdn = "cpId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "11.11.11.11";
	blade->ipAddressEthB = "22.22.22.22";
	blade->ipAliasEthA = "112.113.114.115";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
//	subset->addObject(cp);
	subset->addObject(blade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
	model->addObject(shelf2);
	model->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Add == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(cp->systemIdentifier, hwcData.hwcData->sysId);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_CpBlade_Modify)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

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

	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	blade->type = CPBLADE_T;
	blade->action = ACS_CS_ImBase::MODIFY;
	blade->rdn = "cpBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->cpRdn = "cpId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "11.11.11.11";
	blade->ipAddressEthB = "22.22.22.22";
	blade->ipAliasEthA = "112.113.114.115";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
//	subset->addObject(cp);
	subset->addObject(blade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	model->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Change == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(cp->systemIdentifier, hwcData.hwcData->sysId);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_CpBlade_Delete)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

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

	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	blade->type = CPBLADE_T;
	blade->action = ACS_CS_ImBase::DELETE;
	blade->rdn = "cpBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->cpRdn = "cpId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "11.11.11.11";
	blade->ipAddressEthB = "22.22.22.22";
	blade->ipAliasEthA = "112.113.114.115";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
//	subset->addObject(cp);
	subset->addObject(blade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	model->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Delete == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(cp->systemIdentifier, hwcData.hwcData->sysId);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_ApBlade_Create)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

	ACS_CS_ImAp *ap = new ACS_CS_ImAp();
	ap->type = AP_T;
	ap->action = ACS_CS_ImBase::CREATE;
	ap->rdn = "apId=1234,apCategoryId=1,configurationInfoId=1";
	ap->apId = "apId=1234";

	ACS_CS_ImApBlade *blade = new ACS_CS_ImApBlade();
	blade->type = APBLADE_T;
	blade->action = ACS_CS_ImBase::CREATE;
	blade->rdn = "apBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->apRdn = "apId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "13.13.13.13";
	blade->ipAddressEthB = "32.32.32.32";
	blade->ipAliasEthA = "122.123.124.125";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(blade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	model->addObject(ap);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Add == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_ApBlade_Modify)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

	ACS_CS_ImAp *ap = new ACS_CS_ImAp();
	ap->type = AP_T;
	ap->action = ACS_CS_ImBase::CREATE;
	ap->rdn = "apId=1234,apCategoryId=1,configurationInfoId=1";
	ap->apId = "apId=1234";

	ACS_CS_ImApBlade *blade = new ACS_CS_ImApBlade();
	blade->type = APBLADE_T;
	blade->action = ACS_CS_ImBase::MODIFY;
	blade->rdn = "apBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->apRdn = "apId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "13.13.13.13";
	blade->ipAddressEthB = "32.32.32.32";
	blade->ipAliasEthA = "122.123.124.125";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(blade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	model->addObject(ap);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Change == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_ApBlade_Delete)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

	ACS_CS_ImAp *ap = new ACS_CS_ImAp();
	ap->type = AP_T;
	ap->action = ACS_CS_ImBase::CREATE;
	ap->rdn = "apId=1234,apCategoryId=1,configurationInfoId=1";
	ap->apId = "apId=1234";

	ACS_CS_ImApBlade *blade = new ACS_CS_ImApBlade();
	blade->type = APBLADE_T;
	blade->action = ACS_CS_ImBase::DELETE;
	blade->rdn = "apBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->apRdn = "apId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "13.13.13.13";
	blade->ipAddressEthB = "32.32.32.32";
	blade->ipAliasEthA = "122.123.124.125";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(blade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	model->addObject(ap);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Delete == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_OtherBlade_Create)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

	ACS_CS_ImOtherBlade *blade = new ACS_CS_ImOtherBlade();
	blade->type = OTHERBLADE_T;
	blade->action = ACS_CS_ImBase::CREATE;
	blade->rdn = "otherBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "14.14.13.13";
	blade->ipAddressEthB = "42.42.32.32";
	blade->ipAliasEthA = "142.143.124.125";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(blade);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Add == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_OtherBlade_Modify)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

	ACS_CS_ImOtherBlade *blade = new ACS_CS_ImOtherBlade();
	blade->type = OTHERBLADE_T;
	blade->action = ACS_CS_ImBase::MODIFY;
	blade->rdn = "otherBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "14.14.13.13";
	blade->ipAddressEthB = "42.42.32.32";
	blade->ipAliasEthA = "142.143.124.125";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(blade);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Change == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_OtherBlade_Delete)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

	ACS_CS_ImOtherBlade *blade = new ACS_CS_ImOtherBlade();
	blade->type = OTHERBLADE_T;
	blade->action = ACS_CS_ImBase::DELETE;
	blade->rdn = "otherBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "14.14.13.13";
	blade->ipAddressEthB = "42.42.32.32";
	blade->ipAliasEthA = "142.143.124.125";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
	subset->addObject(blade);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize <= 0)
	{
		FAIL() << "No data received!";
	}

	EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Delete == hwcData.hwcData->operationType);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData->aliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData->aliasEthB);
	EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData->dhcpMethod);
	EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData->fbn);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData->ipEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData->ipEthB);
	if(shelf)
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData->magazine);
	else
		FAIL() << "Could not fetch Shelf from the model!";
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData->netmaskAliasEthA);
	EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData->netmaskAliasEthB);
	EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData->seqNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData->side);
	EXPECT_EQ(blade->slotNumber, hwcData.hwcData->slot);
	EXPECT_EQ(blade->systemNumber, hwcData.hwcData->sysNo);
	EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData->sysType);

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}

TEST(subscription, handleHWCSubscription_TwoBladesAtOnce)
{
	// Arrange
	ACS_CS_HWCHandler *handler = new ACS_CS_HWCHandler();

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

	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	blade->type = CPBLADE_T;
	blade->action = ACS_CS_ImBase::CREATE;
	blade->rdn = "cpBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	blade->cpRdn = "cpId=1234,cpCategoryId=1,configurationInfoId=1";
	blade->ipAddressEthA = "11.11.11.11";
	blade->ipAddressEthB = "22.22.22.22";
	blade->ipAliasEthA = "112.113.114.115";
	blade->ipAliasEthB = "211.211.211.211";
	blade->aliasNetmaskEthA = "255.255.0.0";
	blade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImOtherBlade *otherBlade = new ACS_CS_ImOtherBlade();
	otherBlade->type = OTHERBLADE_T;
	otherBlade->action = ACS_CS_ImBase::MODIFY;
	otherBlade->rdn = "otherBladeId=123,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
	otherBlade->ipAddressEthA = "14.14.13.13";
	otherBlade->ipAddressEthB = "42.42.32.32";
	otherBlade->ipAliasEthA = "142.143.124.125";
	otherBlade->ipAliasEthB = "211.211.211.211";
	otherBlade->aliasNetmaskEthA = "255.255.0.0";
	otherBlade->aliasNetmaskEthB = "255.0.0.0";

	ACS_CS_ImModelSubset *subset = new ACS_CS_ImModelSubset(subscriptionContextInstance->CCBID);
//	subset->addObject(cp);
	subset->addObject(blade);
	subset->addObject(otherBlade);

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf2);
	model->addObject(cp);

	// Act
	handler->newTableOperationRequest(subset);
	int count = 0;
	while (!subscriptionContextInstance->hwcSubscriber->updateCalled && count < 5)
	{
		sleep(1);
		count++;
	}

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	// Assert
	ACS_CS_API_HWCTableChange_R1 hwcData = subscriptionContextInstance->hwcSubscriber->data;

	if(hwcData.dataSize != 2)
	{
		FAIL() << "Invalid number of changed objects received! - " << hwcData.dataSize;
	}

	if(ACS_CS_API_TableChangeOperation::Add == hwcData.hwcData[0].operationType)
	{
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData[0].aliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData[0].aliasEthB);
		EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData[0].dhcpMethod);
		EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData[0].fbn);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData[0].ipEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData[0].ipEthB);
		if(shelf)
			EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData[0].magazine);
		else
			FAIL() << "Could not fetch Shelf from the model!";
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData[0].netmaskAliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData[0].netmaskAliasEthB);
		EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData[0].seqNo);
		EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData[0].side);
		EXPECT_EQ(blade->slotNumber, hwcData.hwcData[0].slot);
		EXPECT_EQ(cp->systemIdentifier, hwcData.hwcData[0].sysId);
		EXPECT_EQ(blade->systemNumber, hwcData.hwcData[0].sysNo);
		EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData[0].sysType);

		EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Change == hwcData.hwcData[1].operationType);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAliasEthA), hwcData.hwcData[1].aliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAliasEthB), hwcData.hwcData[1].aliasEthB);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->dhcpOption), hwcData.hwcData[1].dhcpMethod);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->functionalBoardName), hwcData.hwcData[1].fbn);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAddressEthA), hwcData.hwcData[1].ipEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAddressEthB), hwcData.hwcData[1].ipEthB);
		if(shelf)
			EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData[1].magazine);
		else
			FAIL() << "Could not fetch Shelf from the model!";
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->aliasNetmaskEthA), hwcData.hwcData[1].netmaskAliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->aliasNetmaskEthB), hwcData.hwcData[1].netmaskAliasEthB);
		EXPECT_EQ(otherBlade->sequenceNumber, hwcData.hwcData[1].seqNo);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->side), hwcData.hwcData[1].side);
		EXPECT_EQ(otherBlade->slotNumber, hwcData.hwcData[1].slot);
		EXPECT_EQ(otherBlade->systemNumber, hwcData.hwcData[1].sysNo);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->systemType), hwcData.hwcData[1].sysType);
	}
	else if(ACS_CS_API_TableChangeOperation::Change == hwcData.hwcData[0].operationType)
	{
		EXPECT_TRUE(ACS_CS_API_TableChangeOperation::Add == hwcData.hwcData[1].operationType);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthA), hwcData.hwcData[1].aliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAliasEthB), hwcData.hwcData[1].aliasEthB);
		EXPECT_EQ(static_cast<uint16_t>(blade->dhcpOption), hwcData.hwcData[1].dhcpMethod);
		EXPECT_EQ(static_cast<uint16_t>(blade->functionalBoardName), hwcData.hwcData[1].fbn);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthA), hwcData.hwcData[1].ipEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->ipAddressEthB), hwcData.hwcData[1].ipEthB);
		if(shelf)
			EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData[1].magazine);
		else
			FAIL() << "Could not fetch Shelf from the model!";
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthA), hwcData.hwcData[1].netmaskAliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(blade->aliasNetmaskEthB), hwcData.hwcData[1].netmaskAliasEthB);
		EXPECT_EQ(blade->sequenceNumber, hwcData.hwcData[1].seqNo);
		EXPECT_EQ(static_cast<uint16_t>(blade->side), hwcData.hwcData[1].side);
		EXPECT_EQ(blade->slotNumber, hwcData.hwcData[1].slot);
		EXPECT_EQ(cp->systemIdentifier, hwcData.hwcData[1].sysId);
		EXPECT_EQ(blade->systemNumber, hwcData.hwcData[1].sysNo);
		EXPECT_EQ(static_cast<uint16_t>(blade->systemType), hwcData.hwcData[1].sysType);

		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAliasEthA), hwcData.hwcData[0].aliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAliasEthB), hwcData.hwcData[0].aliasEthB);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->dhcpOption), hwcData.hwcData[0].dhcpMethod);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->functionalBoardName), hwcData.hwcData[0].fbn);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAddressEthA), hwcData.hwcData[0].ipEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->ipAddressEthB), hwcData.hwcData[0].ipEthB);
		if(shelf)
			EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(shelf->address), hwcData.hwcData[0].magazine);
		else
			FAIL() << "Could not fetch Shelf from the model!";
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->aliasNetmaskEthA), hwcData.hwcData[0].netmaskAliasEthA);
		EXPECT_EQ(ACS_CS_HWCHandler::getIpAddress(otherBlade->aliasNetmaskEthB), hwcData.hwcData[0].netmaskAliasEthB);
		EXPECT_EQ(otherBlade->sequenceNumber, hwcData.hwcData[0].seqNo);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->side), hwcData.hwcData[0].side);
		EXPECT_EQ(otherBlade->slotNumber, hwcData.hwcData[0].slot);
		EXPECT_EQ(otherBlade->systemNumber, hwcData.hwcData[0].sysNo);
		EXPECT_EQ(static_cast<uint16_t>(otherBlade->systemType), hwcData.hwcData[0].sysType);
	}
	else
	{
		FAIL() << "Invalid operationType received!";
	}

	// Cleanup
	delete subset;
	delete handler;
	subscriptionContextInstance->hwcSubscriber->updateCalled = false;
}
