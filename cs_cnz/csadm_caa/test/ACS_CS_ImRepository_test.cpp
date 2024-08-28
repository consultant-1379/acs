/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImRepository.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <set>
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImCp.h"
#include "ACS_CS_ImCpCategory.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImUtils.h"



class ACS_CS_ImRepositoryTests : public ::testing::Test
{
protected:

   virtual void SetUp()
   {
      //ACS_CS_ImRepository::instance()->getModel()->print();

   }

   virtual void TearDown()
   {
   }

};

TEST_F(ACS_CS_ImRepositoryTests, getParentOf_ReturnsCorrectParentObject)
{
   // Arrange
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImCpCategory *cpCategory = ACS_CS_ImDefaultObjectCreator::createCpCategoryObject();
   ACS_CS_ImRepository::instance()->addObject(cp);
   ACS_CS_ImRepository::instance()->addObject(cpCategory);

   // Act
   const ACS_CS_ImBase *parent = ACS_CS_ImRepository::instance()->getParentOf("cpId=1,cpCategoryId=1,configurationInfoId=1");


   cout << "CP" << endl;
   ACS_CS_ImUtils::printImBaseObject(cp);
   cout << "Parent" << endl;
   ACS_CS_ImUtils::printImBaseObject(const_cast<ACS_CS_ImBase *>(parent));


   // Assert
   EXPECT_EQ(*parent, *cpCategory);
}

TEST_F(ACS_CS_ImRepositoryTests, DISABLED_saveCpObject_SavesCorrectly)
{
   // Arrange
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImRepository::instance()->addObject(cp);

   // Act
   ACS_CC_ReturnType saved = ACS_CS_ImRepository::instance()->save();

   // Assert
   EXPECT_TRUE(saved == ACS_CC_SUCCESS);
}


TEST_F(ACS_CS_ImRepositoryTests, DISABLED_saveCpBladeObject_SavesCorrectly)
{
   // Arrange
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImRepository::instance()->addObject(cpBlade);

   // Act
   ACS_CC_ReturnType saved = ACS_CS_ImRepository::instance()->save();

   // Assert
   EXPECT_TRUE(saved == ACS_CC_SUCCESS);
}

TEST_F(ACS_CS_ImRepositoryTests, modelAndCopyAreEqual)
{
   // Arrange

   // Act
   ACS_CS_ImModel *copy = ACS_CS_ImRepository::instance()->getModelCopy();
   ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();

   // Assert
   EXPECT_EQ(*model, *copy);
}

TEST_F(ACS_CS_ImRepositoryTests, modelAndCopySizeAreEqual)
{
   // Arrange

   // Act
   ACS_CS_ImModel *copy = ACS_CS_ImRepository::instance()->getModelCopy();
   ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();

   // Assert
   EXPECT_EQ(model->size(), copy->size());
}

TEST_F(ACS_CS_ImRepositoryTests, modelAndModelCopySizeAreEqual)
{
   // Arrange

   // Act
   ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImModel *copy = ACS_CS_ImRepository::instance()->getModel();


   // Assert
   EXPECT_EQ(model->size(), copy->size());
}


TEST_F(ACS_CS_ImRepositoryTests, saveOnly_SavesCorrectly)
{
   // Arrange

   // Act
   ACS_CC_ReturnType saved = ACS_CS_ImRepository::instance()->save();

   // Assert
   EXPECT_TRUE(saved == ACS_CC_SUCCESS);
}




