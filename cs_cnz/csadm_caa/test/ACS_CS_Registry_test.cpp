#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "string.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"

using namespace std;

TEST(Registry_isMultipleCPSystem, MutlipleSystemTrue)
{
	// Arrange

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base = immReader->getObject("configurationInfoId=1");

	ACS_CS_ImConfigurationInfo* info = dynamic_cast<ACS_CS_ImConfigurationInfo*>(base);

	info->isBladeClusterSystem = true;
	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);

	ACS_CS_ImModelSaver saver(model);
	saved = saver.save();

	if(saved == ACS_CC_FAILURE){
		FAIL() << "ERROR: COULD NOT SAVE THE MODEL!!!";
	}

	// Act
	bool multipleCPSystem = ACS_CS_Registry::isMultipleCPSystem();

	// Assert
	EXPECT_TRUE(multipleCPSystem == true);

}

TEST(Registry_isMultipleCPSystem, MutlipleSystemFalse)
{
	// Arrange
	ACS_CC_ReturnType saved = ACS_CC_FAILURE;

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base = immReader->getObject("configurationInfoId=1");

	ACS_CS_ImConfigurationInfo* info = dynamic_cast<ACS_CS_ImConfigurationInfo*>(base);

	info->isBladeClusterSystem = false;
	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);

	ACS_CS_ImModelSaver saver(model);
	saved= saver.save();

	if(saved == ACS_CC_FAILURE){
		FAIL() << "ERROR: COULD NOT SAVE THE MODEL!!!";
	}

	// Act
	bool multipleCPSystem = ACS_CS_Registry::isMultipleCPSystem();

	// Assert
	EXPECT_TRUE(multipleCPSystem == false);

}


TEST(Registry_getFrontAP, FrontApGet)
{
	// Arrange

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base= immReader->getObject("bladeClusterInfoId=1,configurationInfoId=1");

	ACS_CS_ImBladeClusterInfo* bladeInfo = dynamic_cast<ACS_CS_ImBladeClusterInfo*>(base);

	bladeInfo->frontAp = AP1;
	bladeInfo->action = ACS_CS_ImBase::MODIFY;

	model->addObject(bladeInfo);

	ACS_CS_ImModelSaver saver(model);
	saved = saver.save();

	if(saved == ACS_CC_FAILURE){
		FAIL() << "ERROR: COULD NOT SAVE THE MODEL!!!";
	}

	// Act
	short unsigned frontAp = ACS_CS_Registry::getFrontAPG();

	// Assert
	EXPECT_TRUE(frontAp == AP1 + 2000);

}

TEST(Registry_getAptType, AptTypeGetFromApzFunctions)
{
	// Arrange
	string aptType = "UNDEFINED";

	// Act
	aptType = ACS_CS_Registry::getAPTType();

	// Assert
	EXPECT_TRUE(aptType.compare("MSC") == 0 || aptType.compare("HLR") == 0 || aptType.compare("BSC") == 0 || aptType.compare("WLN") == 0);

}

TEST(Registry_isTestEnvironment, TestSystemTrue)
{
	// Arrange

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base= immReader->getObject("advancedConfigurationId=1,configurationInfoId=1");

	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	info->isTestEnvironment = true;
	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);

	ACS_CS_ImModelSaver saver(model);
	saved = saver.save();

	if(saved == ACS_CC_FAILURE){
		FAIL() << "ERROR: COULD NOT SAVE THE MODEL!!!";
	}

	// Act
	bool isTestEnvironment = ACS_CS_Registry::isTestEnvironment();

	// Assert
	EXPECT_TRUE(isTestEnvironment == true);

}


