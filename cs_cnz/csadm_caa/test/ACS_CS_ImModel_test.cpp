#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>


#include "ACS_CS_ImModel.h"

#include "ACS_CS_ImConfigurationInfo.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImHardwareConfigurationCategory.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImCp.h"
#include "ACS_CS_ImUtils.h"
#include <set>

#include "ACS_CS_ImDefaultObjectCreator.h"


#include "gtest/gtest.h"
#include "gmock/gmock.h"

//-----------------------------------------------------------------------------
TEST(addObjectTest, objectAddedSizeGrows)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImCp *cp = new ACS_CS_ImCp();

   cp->cpId = "1234";
   cp->systemIdentifier = 1;
   cp->defaultName = "CP1";
   cp->alias = "CP1";
   cp->apzSystem = 1;
   cp->cpType = 2;

   // Act
   bool added = model->addObject(cp);
   set<const ACS_CS_ImBase *> objects;
   model->getObjects(objects);
   int i = objects.size();

   // Assert
   EXPECT_TRUE(added == true && i == 1);

   delete model;
}


//-----------------------------------------------------------------------------
TEST(addObjectTest, addingSameObjectTwice)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCp *cp = new ACS_CS_ImCp();

   cp->cpId = "1234";
   cp->systemIdentifier = 1;
   cp->defaultName = "CP1";
   cp->alias = "CP1";
   cp->apzSystem = 1;
   cp->cpType = 2;

   // Act
   bool added1 = model->addObject(cp);
   bool added2 = model->addObject(cp);

   EXPECT_TRUE(added1 == true && added2 == false);

   delete model;

}


//-----------------------------------------------------------------------------
TEST(applySubsetTest, deleteOneObject)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   model->addObject(confInfo);

   // Prepare the subset
   ACS_CS_ImModel *subsetModel = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *subsetConfInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   subsetConfInfo->action = ACS_CS_ImBase::DELETE;
   subsetModel->addObject(subsetConfInfo);

   // Act
   model->applySubset(subsetModel);

   // Check
   set<const ACS_CS_ImBase *> objects;
   model->getObjects(objects);
   int i = objects.size();

   EXPECT_EQ(i, 0) << "The object was not deleted";

   delete model;

}


//-----------------------------------------------------------------------------
TEST(applySubsetTest, addOneObject)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();

   // Prepare the subset
   ACS_CS_ImModel *subsetModel = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *subsetConfInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   subsetModel->addObject(subsetConfInfo);

   // Act
   model->applySubset(subsetModel);

   // Check
   set<const ACS_CS_ImBase *> objects;
   model->getObjects(objects);
   int i = objects.size();

   EXPECT_EQ(i, 1) << "The object was not added";

   delete model;
   delete subsetModel;

}



//-----------------------------------------------------------------------------
TEST(applySubsetTest, updateOneObject)
{
   // Arrange

   // Prepare the model
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   confInfo->isBladeClusterSystem = false;
   model->addObject(confInfo);

   // Prepare the subset
   ACS_CS_ImModel *subsetModel = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *subsetConfInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   subsetConfInfo->isBladeClusterSystem = true; // This is the value to update
   subsetConfInfo->action = ACS_CS_ImBase::MODIFY;
   subsetModel->addObject(subsetConfInfo);

   // Act
   model->applySubset(subsetModel);

   // Check
   set<const ACS_CS_ImBase *> objects;
   model->getObjects(objects);
   int i = objects.size();
   EXPECT_EQ(1, i) << "The number of objects should not have been changed";

   const ACS_CS_ImConfigurationInfo *confInfo2;
   confInfo2 = dynamic_cast<const ACS_CS_ImConfigurationInfo *> (model->getObject("configurationInfoId=1"));
   if(NULL == confInfo2)
	   FAIL() << "Model does not contain the requested object!";
   EXPECT_TRUE(confInfo2->isBladeClusterSystem == true) << "Failed to update the object";

   delete subsetModel;
   delete model;

}


//-----------------------------------------------------------------------------
TEST(getObjectTest, retreivingAnObject)
{
   // Arrange

   // Prepare the model
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   confInfo->isBladeClusterSystem = true;
   model->addObject(confInfo);

   // Act
   const ACS_CS_ImConfigurationInfo *retreivedConfInfo = dynamic_cast<const ACS_CS_ImConfigurationInfo *> (model->getObject("configurationInfoId=1"));

   // Check
   EXPECT_EQ(confInfo, retreivedConfInfo) << "Should be the same pointer";
   EXPECT_TRUE(retreivedConfInfo->isBladeClusterSystem == true) << "Wrong content in object";

   delete model;

}







//-----------------------------------------------------------------------------
TEST(getParentNameTest, GettingTheParentNameOfAnObject)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = new ACS_CS_ImShelf();
   shelf->rdn ="Shelf=1,HardwareConfigurationCategory=1,ConfigurationInfo=1";

   // Act
   string name = ACS_CS_ImUtils::getParentName(shelf);

   EXPECT_TRUE("HardwareConfigurationCategory=1,ConfigurationInfo=1" == name);

   delete model;
   delete shelf;

}


//-----------------------------------------------------------------------------
TEST(createAssociationsTest, CreatingAssociationsOfObjects)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   // Act
   model->addObject(confInfo);
   model->addObject(hwCategory);
   model->addObject(shelf);
   model->createAssociations();

   EXPECT_TRUE(confInfo->hardwareConfigurationCategory == hwCategory && *(hwCategory->shelves.begin()) == shelf);

   delete model;

}



//-----------------------------------------------------------------------------
TEST(getParentOfTest, gettingTheParentOfAnObject)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   // Act
   model->addObject(hwCategory);
   model->addObject(shelf);

   model->createAssociations();

   ACS_CS_ImHardwareConfigurationCategory* hwcParentOf = dynamic_cast<ACS_CS_ImHardwareConfigurationCategory*>(model->getParentOf(shelf->rdn));

   EXPECT_TRUE(hwcParentOf == hwCategory);

   delete model;
}

TEST(operatorComparison, twoModelsAreEqual)
{
   // Arrange
   ACS_CS_ImModel *model1 = new ACS_CS_ImModel();
   ACS_CS_ImModel *model2 = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   // Act
   model1->addObject(confInfo);
   model1->addObject(hwCategory);
   model1->addObject(shelf);
   model1->createAssociations();

   model2->addObject(confInfo->clone());
   model2->addObject(hwCategory->clone());
   model2->addObject(shelf->clone());
   model2->createAssociations();


   EXPECT_TRUE(*model1 == *model2);

   delete model1;
   delete model2;
}

TEST(operatorComparison, twoModelsNotEqualIfDifferentObjects)
{
   // Arrange
   ACS_CS_ImModel *model1 = new ACS_CS_ImModel();
   ACS_CS_ImModel *model2 = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   shelf->rdn ="Shelf=2,HardwareConfigurationCategory=1,ConfigurationInfo=1";

   // Act
   model1->addObject(confInfo);
   model1->addObject(hwCategory);
   model1->addObject(shelf);
   model1->createAssociations();

   model2->addObject(confInfo->clone());
   model2->addObject(hwCategory->clone());
   model2->addObject(shelf2);
   model2->createAssociations();


   EXPECT_TRUE(*model1 != *model2);

   delete model1;
   delete model2;
}


TEST(operatorComparison, twoModelsEqualIfDifferentObjectsWithThreeBlades)
{
   // Arrange
   ACS_CS_ImModel *model1 = new ACS_CS_ImModel();
   ACS_CS_ImModel *model2 = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   ACS_CS_ImApBlade *apBlade = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImOtherBlade *otherBlade = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   model1->addObject(confInfo);
   model1->addObject(hwCategory);
   model1->addObject(shelf);
   model1->addObject(apBlade);
   model1->addObject(cpBlade);
   model1->addObject(otherBlade);
   model1->createAssociations();

   model2->addObject(confInfo->clone());
   model2->addObject(hwCategory->clone());
   model2->addObject(shelf->clone());
   model2->addObject(apBlade->clone());
   model2->addObject(cpBlade->clone());
   model2->addObject(otherBlade->clone());
   model2->createAssociations();


   EXPECT_TRUE(*model1 == *model2);

   delete model1;
   delete model2;
}


TEST(operatorComparison, twoModelsEqualIfDifferentObjectsWithTwoBlades)
{
   // Arrange
   ACS_CS_ImModel *model1 = new ACS_CS_ImModel();
   ACS_CS_ImModel *model2 = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   ACS_CS_ImApBlade *apBlade = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   model1->addObject(confInfo);
   model1->addObject(hwCategory);
   model1->addObject(shelf);
   model1->addObject(apBlade);
   model1->addObject(cpBlade);
   model1->createAssociations();

   model2->addObject(confInfo->clone());
   model2->addObject(hwCategory->clone());
   model2->addObject(shelf->clone());
   model2->addObject(apBlade->clone());
   model2->addObject(cpBlade->clone());
   model2->createAssociations();

   EXPECT_TRUE(*model1 == *model2);

   delete model1;
   delete model2;
}

TEST(operatorComparison, twoModelsEqualIfDifferentObjectsWithOneBlade)
{
   // Arrange
   ACS_CS_ImModel *model1 = new ACS_CS_ImModel();
   ACS_CS_ImModel *model2 = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImHardwareConfigurationCategory *hwCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   ACS_CS_ImApBlade *apBlade = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   model1->addObject(confInfo);
   model1->addObject(hwCategory);
   model1->addObject(shelf);
   model1->addObject(apBlade);
   model1->createAssociations();

   model2->addObject(confInfo->clone());
   model2->addObject(hwCategory->clone());
   model2->addObject(shelf->clone());
   model2->addObject(apBlade->clone());
   model2->createAssociations();

   EXPECT_TRUE(*model1 == *model2);

   delete model1;
   delete model2;
}

TEST(serialization, ModelObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImModelInfo.serialized";

   ACS_CS_ImModel *obj1 = new ACS_CS_ImModel();
   ACS_CS_ImModel *obj2 = new ACS_CS_ImModel();

   ACS_CS_ImIMMReader *immReader;
   immReader = new ACS_CS_ImIMMReader();
   immReader->loadModel(obj1);

   ofstream out(fileName);

   if (!out)
      FAIL();

   {
      boost::archive::text_oarchive oa(out);
      oa << *obj1;
   }

   ifstream in(fileName);
   if (!in)
      FAIL();

   {
      boost::archive::text_iarchive ia(in);
      ia >> *obj2;
   }

   EXPECT_TRUE(*obj1 == *obj2);

}

//-----------------------------------------------------------------------------
TEST(copyModelTest, copyingTheModel)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   model->addObject(confInfo);

   ACS_CS_ImModel *newModel = new ACS_CS_ImModel(*model);// Copy model

   EXPECT_NE(newModel, model) << "We have only copied the reference";

   const ACS_CS_ImConfigurationInfo *newConfInfo = dynamic_cast<const ACS_CS_ImConfigurationInfo *> (newModel->getObject("configurationInfoId=1"));

   ASSERT_TRUE(newConfInfo != 0);
   EXPECT_NE(newConfInfo, confInfo) << "We have only copied the reference";
   EXPECT_TRUE(newConfInfo->rdn == confInfo->rdn);

   delete model;
   delete newModel;
}


//-----------------------------------------------------------------------------
TEST(modelTest, copyConstructor)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   model->addObject(confInfo);

   // Act
   *newModel = *model;

   // Check
   EXPECT_NE(newModel, model) << "We have only copied the reference";

   const ACS_CS_ImConfigurationInfo *newConfInfo = dynamic_cast<const ACS_CS_ImConfigurationInfo*> (newModel->getObject("configurationInfoId=1"));

   ASSERT_TRUE(newConfInfo != 0);
   EXPECT_NE(newConfInfo, confInfo) << "We have only copied the reference";
   EXPECT_TRUE(newConfInfo->rdn == confInfo->rdn);

   delete model;
   delete newModel;
}


//-----------------------------------------------------------------------------
TEST(modelTest, compareNotEqualOperatorObjectIsMissing)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *confInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   model->addObject(confInfo);

   // Act

   // Check
   EXPECT_TRUE(*model != *newModel);

   delete model;
   delete newModel;
}
//-----------------------------------------------------------------------------
TEST(modelTest, compareEqualOperator)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();

   // Add COnfigurationInfo
   model->addObject(   ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject());
   newModel->addObject(ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject());

   // Add Ap
   model->addObject(   ACS_CS_ImDefaultObjectCreator::createApObject());
   newModel->addObject(ACS_CS_ImDefaultObjectCreator::createApObject());

   // Act

   // Check
   EXPECT_TRUE(*model == *newModel);

   delete model;
   delete newModel;
}


//-----------------------------------------------------------------------------
TEST(modelTest, compareNotEqualAp)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();


   // Add Ap
   ACS_CS_ImAp *ap = ACS_CS_ImDefaultObjectCreator::createApObject();
   ap->name = AP1;
   model->addObject( ap );

   ACS_CS_ImAp *newAp = ACS_CS_ImDefaultObjectCreator::createApObject();
   ap->name = AP2;
   newModel->addObject( newAp );

   // Act

   // Check
   EXPECT_TRUE(*model != *newModel);

   delete model;
   delete newModel;
}
//-----------------------------------------------------------------------------
TEST(modelTest, compareNotEqualCp)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();


   // Add Cp
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   cp->alias = "BC17";
   model->addObject( cp );

   ACS_CS_ImCp *newCp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   newCp->alias = "MYCP";
   newModel->addObject( newCp );

   // Act

   // Check
   EXPECT_TRUE(*model != *newModel);

   delete model;
   delete newModel;
}



//-----------------------------------------------------------------------------
TEST(modelTest, compareNotEqualOperatorBaseObjectAttributeDiffers)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo * info = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   info->action = ACS_CS_ImBase::CREATE;
   model->addObject(info);

   info = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   info->action = ACS_CS_ImBase::DELETE;
   newModel->addObject(info);

   // Act

   // Check
   EXPECT_TRUE(*model != *newModel);

   delete model;
   delete newModel;
}

//-----------------------------------------------------------------------------
TEST(modelTest, compareNotEqualOperatorObjectAttributeDiffers)
{
   // Arrange
   ACS_CS_ImModel *model       = new ACS_CS_ImModel();
   ACS_CS_ImModel *newModel    = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo * info = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   info->isBladeClusterSystem = false;
   model->addObject(info);

   info = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   info->isBladeClusterSystem = true;
   newModel->addObject(info);

   // Act

   // Check
   EXPECT_TRUE(*model != *newModel);

   delete model;
   delete newModel;
}



