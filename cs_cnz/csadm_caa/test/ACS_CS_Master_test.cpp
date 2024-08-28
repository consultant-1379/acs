#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImConfigurationInfo.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImHardwareConfigurationCategory.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_Master.h"
#include "ACS_CS_Slave.h"


TEST(MasterSlaveReplication, DISABLED_MasterSendsModelClisentRcvModelModelsEqual)
{
	//Arrange

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
	ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
	ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

	model->addObject(confInfo);
	model->addObject(hwCategory);
	model->addObject(shelf);
	model->createAssociations();

	cout << "--------------------------------" << endl;
	cout << "------------ MASTER ------------" << endl;
	cout << "--------------------------------" << endl;
	model->print();

	ACS_CS_Master::start();
	ACS_CS_Slave::start();

	//Act

	ACS_CS_Master::sendOperation(model);

	ACS_CS_ImModel * copyOfModel = 0;
	copyOfModel = ACS_CS_Slave::getModel();

	if(copyOfModel == 0){
		FAIL();
	}
	else{
		cout << "-------------------------------" << endl;
		cout << "------------ SLAVE ------------" << endl;
		cout << "-------------------------------" << endl;
		copyOfModel->print();
	}

	//Assert

	EXPECT_TRUE(*model == *copyOfModel);
}
